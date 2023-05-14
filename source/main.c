#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libwebsockets.h>

static int callback_echo(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len)
{
	switch (reason) {
		case LWS_CALLBACK_ESTABLISHED:
			printf("Connection established\n");
			break;
		case LWS_CALLBACK_RECEIVE:
			printf("Received data: %.*s\n", (int)len, (char *)in);
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

int main(int argc, const char **argv)
{
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
		fprintf(stderr, "Failed to create WebSockets context\n");
		return 1;
	}

	printf("WebSockets server started at ws://%s:%d\n", address, port);

	while (1) {
		lws_service(context, 50);
	}

	lws_context_destroy(context);
	return 0;
}
