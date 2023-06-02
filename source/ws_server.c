#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <cjson/cJSON.h>
#include <ws.h>

#include "env.h"
#include "logger.h"
#include "data.h"

int rc;

void onopen(ws_cli_conn_t *client) {
    char *cli;
    cli = ws_getaddress(client);
	sendf(stdout, LOG_DEBUG, "Connection made with `%s`...\n", cli);
}

void onclose(ws_cli_conn_t *client) {
    char *cli;
    cli = ws_getaddress(client);
	sendf(stdout, LOG_DEBUG, "Connection with `%s` ended...\n", cli);
}

void onmessage(ws_cli_conn_t *client, const unsigned char *input_data, uint64_t size, int type) {
    char *cli;
    cli = ws_getaddress(client);
    sendf(stdout, LOG_DEBUG, "Recieved a message: %s (%zu), from: %s\n", input_data, size, cli);

	cJSON *input_json_obj, *output_json_obj;
	cJSON *type_obj;

	/* Data to be inputted by the client, and data to be outputted by the server... */
	char *type_raw, *output_data;

	/* Create a JSON object from `input_data` aka. inputted data coming from client... */
	input_json_obj = cJSON_Parse(input_data);
	if (input_json_obj == NULL) {
		/* There seems to be some sort of parsing error... */\
		sendf(stderr, LOG_ERROR, "Failed to parse the recieved JSON...\n");
		/* In the future we can make some JSON that tells the client that there was an error parsing the JSON sent by the client, but for now we do nothing... */
		return;
	}

	/* Let's get the type of request... */
	type_obj = cJSON_GetObjectItemCaseSensitive(input_json_obj, "type");
	if (type_obj != NULL && cJSON_IsString(type_obj)) {
		/* Since type_obj exists and is a string, let's set type_raw to the string version of type_obj... */
		type_raw = cJSON_GetStringValue(type_obj);
	} else {
		/* Setting type_obj failed, therefore we just quit... */
		return;
	}

	/* Start comparing the type and do things that are meant to be done for the specific type... */
	if(strcmp(type_raw, "version") == 0) {
		/* Since `type_raw` is equal to `version`, let's send the backend's version to the client... */
		output_json_obj = cJSON_CreateObject();
		cJSON_AddItemToObject(output_json_obj, "version", cJSON_CreateString(VERSION));
		output_data = cJSON_Print(output_json_obj);
		ws_sendframe_txt(client, output_data);
		free(output_data);
	} else if(strcmp(type_raw, "create_account") == 0) {
		char *username_raw, *password_hash_raw, *token_buffer;
		cJSON *username_obj, *password_hash_obj;
		username_obj = cJSON_GetObjectItemCaseSensitive(input_json_obj, "username");
		if (username_obj != NULL && cJSON_IsString(username_obj)) {
			username_raw = cJSON_GetStringValue(username_obj);
		} else {
			return;
		}
		password_hash_obj = cJSON_GetObjectItemCaseSensitive(input_json_obj, "password_hash");
		if (password_hash_obj != NULL && cJSON_IsString(password_hash_obj)) {
			password_hash_raw = cJSON_GetStringValue(password_hash_obj);
		} else {
			return;
		}
		
		rc = create_account(username_raw, password_hash_raw, token_buffer);
		if(rc == 0) {
			output_json_obj = cJSON_CreateObject();
			cJSON_AddItemToObject(output_json_obj, "token", cJSON_CreateString(token_buffer));
			output_data = cJSON_Print(output_json_obj);
			ws_sendframe_txt(client, output_data);
			free(output_data);
		} else if(rc == -2) {
			output_json_obj = cJSON_CreateObject();
			cJSON_AddItemToObject(output_json_obj, "error", cJSON_CreateString("Account already exists!"));
			output_data = cJSON_Print(output_json_obj);
			ws_sendframe_txt(client, output_data);
			free(output_data);
		} else if(rc == -1) {
			output_json_obj = cJSON_CreateObject();
			cJSON_AddItemToObject(output_json_obj, "error", cJSON_CreateString("Some other error happened when trying to execute a SQL query..."));
			output_data = cJSON_Print(output_json_obj);
			ws_sendframe_txt(client, output_data);
			free(output_data);
		}
	} else {
		output_json_obj = cJSON_CreateObject();
		cJSON_AddItemToObject(output_json_obj, "error", cJSON_CreateString("Unknown request..."));
		output_data = cJSON_Print(output_json_obj);
		ws_sendframe_txt(client, output_data);
		free(output_data);
	}
}

int wsserver() {
	/* Register events. */
    struct ws_events evs;
    evs.onopen = &onopen;
    evs.onclose = &onclose;
    evs.onmessage = &onmessage;

	sendf(stdout, LOG_INFO, "WebSocket server started at ws://127.0.0.1:9000/\n");

    ws_socket(&evs, 9000, 0, 1000);
	return 0;
}
