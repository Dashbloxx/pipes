#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "ws_server.h"

int main(int argc, const char **argv)
{
	pthread_t thread;
	int rc;

	rc = pthread_create(&thread, NULL, ws_server, NULL);
	if (rc) {
		printf("ERROR; return code from pthread_create() is %d\n", rc);
		exit(-1);
	}

	// Perform some other tasks in the main thread while the WebSocket server runs in the background
	// ...

	pthread_join(thread, NULL);  // Wait for the WebSocket server thread to complete
	return 0;
}
