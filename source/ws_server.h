#pragma once

#include <ws.h>

void onopen(ws_cli_conn_t *client);
void onclose(ws_cli_conn_t *client);
void onmessage(ws_cli_conn_t *client, const unsigned char *msg, uint64_t size, int type);

int wsserver();