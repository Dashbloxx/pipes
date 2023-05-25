#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <libwebsockets.h>

#include "ws_server.h"
#include "logger.h"

static int callback_echo(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len)
{
	switch (reason) {
		case LWS_CALLBACK_ESTABLISHED:
			sendf(stdout, LOG_INFO, "Connection established\n");
			break;
		case LWS_CALLBACK_RECEIVE:
			sendf(stdout, LOG_INFO, "Received data: %.*s\n", (int)len, (char *)in);
			lws_write(wsi, in, len, LWS_WRITE_TEXT);
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

void *wsserver(void *arg) {
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
		return NULL;
	}

	sendf(stdout, LOG_INFO, "WebSocket server started at ws://%s:%d/\n", address, port);

	while (1) {
		lws_service(context, 50);
	}

	lws_context_destroy(context);
	return NULL;
}
