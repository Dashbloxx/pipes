#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <libwebsockets.h>
#include <jansson.h>

#include "ws_server.h"
#include "logger.h"
#include "env.h"

static int callback_echo(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len)
{
	char input_json_string_unparsed[4096];
	json_t *input_json, *output_json;
	json_error_t input_json_error, output_json_error;

	/* The string that contains the JSON to be outputted to the client... */
	char *output_json_string;

	switch (reason) {
		case LWS_CALLBACK_ESTABLISHED:
			sendf(stdout, LOG_DEBUG, "Connection established\n");
			break;
		case LWS_CALLBACK_RECEIVE:
			sendf(stdout, LOG_DEBUG, "Received data: %.*s\n", (int)len, (char *)in);
			/* The following commented code can be used to respond the data inputted... */
			// lws_write(wsi, in, len, LWS_WRITE_TEXT);

			/* Let's print the inputted data into a string named `input_json`... */
			sprintf(input_json_string_unparsed, "%.*s", (int)len, (char *)in);

			input_json = json_loads(input_json_string_unparsed, 0, &input_json_error);
			output_json = json_object();

			if(!input_json) {
				/* For this error we use stdout instead of stderr because the client can intentionally cause the error... */
				sendf(stdout, LOG_ERROR, "Error parsing JSON from string: %s\n", input_json_error);
				json_object_set_new(output_json, "type", json_string("error"));
				json_object_set_new(output_json, "error_message", json_string("There was an error parsing the JSON inputted by the client..."));

				output_json_string = json_dumps(output_json, JSON_INDENT(4));

				lws_write(wsi, output_json_string, strlen(output_json_string), LWS_WRITE_TEXT);

				//json_free(output_json_string);
				json_decref(output_json);
				json_decref(input_json);
				break;
			}

			/* Let's actually handle API now... */

			int api_call_type;

			/* Let's figure out what type of API call we're dealing with... */
			json_t *type_of_api_call = json_object_get(input_json, "type");
			if(type_of_api_call && json_is_string(type_of_api_call)) {
				/* Changed (char *) to (const char *) because we aren't going to change type_of_api_call_value... */
				const char *type_of_api_call_value = json_string_value(type_of_api_call);
				if(strcmp(type_of_api_call_value, "get_version") == 0) {
					/* The client wants the version of the server! */
					json_object_set_new(output_json, "type", json_string("version"));
					json_object_set_new(output_json, "post_version", json_string(VERSION));

					output_json_string = json_dumps(output_json, JSON_COMPACT);

					lws_write(wsi, output_json_string, strlen(output_json_string), LWS_WRITE_TEXT);
					sendf(stdout, LOG_DEBUG, "Sent data: %s\n", output_json_string);

					//json_free(output_json_string);
					json_decref(output_json);
					json_decref(input_json);
					break;
				}
				else {
					/* We don't know what the client wants... */
					sendf(stdout, LOG_ERROR, "API call unknown...\n");
					json_object_set_new(output_json, "type", json_string("error"));
					json_object_set_new(output_json, "error_message", json_string("Unknown API call..."));

					output_json_string = json_dumps(output_json, JSON_INDENT(4));

					lws_write(wsi, output_json_string, strlen(output_json_string), LWS_WRITE_TEXT);

					//json_free(output_json_string);
					json_decref(output_json);
					json_decref(input_json);
					break;
				}
			}

			break;
		default:
			break;
	}
	return 0;
}

static struct lws_protocols protocols[] = {
	{
		"echo-protocol",
		callback_echo,
		0,
		1024,
	},
	{ NULL, NULL, 0, 0 }
};

int wsserver() {
	struct lws_context_creation_info info;
	struct lws_context *context;
	const char *address = "127.0.0.1";
	int port = 9000;

	memset(&info, 0, sizeof(info));
	info.port = port;
	info.iface = address;
	info.protocols = protocols;

	context = lws_create_context(&info);
	if (!context) {
		sendf(stderr, LOG_ERROR, "Failed to create WebSockets context\n");
		return -1;
	}

	sendf(stdout, LOG_INFO, "WebSocket server started at ws://%s:%d/\n", address, port);

	while (1) {
		lws_service(context, 50);
	}

	lws_context_destroy(context);
	return 0;
}
