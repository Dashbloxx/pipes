#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "ws_server.h"
#include "http_server.h"
#include "logger.h"

#define VERSION "0.0.1-dev"

int main(int argc, const char **argv)
{
	sendf(stdout, LOG_INFO, "Pipes server %s. Copyright (C) 2023 Matthew Edward Majfud-Wilinski\n\tThis binary has been compiled on %s at %s.\n", VERSION, __DATE__, __TIME__);
    
    pthread_t wserver_thread, wsserver_thread;

    /* Create a new thread for the web server... */
    if(pthread_create(&wserver_thread, NULL, wserver, NULL) != 0) {
        sendf(stderr, LOG_ERROR, "Error creating a new thread for web server...\nLine %d\nFile %s\n", __LINE__, __FILE__);
        exit(1);
    }

    /* Create a new thread for the websocket server... */
    if(pthread_create(&wsserver_thread, NULL, wsserver, NULL) != 0) {
        sendf(stderr, LOG_ERROR, "Error creating a new thread for websocket server...\nLine %d\nFile %s\n", __LINE__, __FILE__);
        exit(1);
    }

    /* Wait for the web server thread to finish... */
    if (pthread_join(wserver_thread, NULL) != 0) {
        sendf(stderr, LOG_ERROR, "Error waiting for web server thread to finish...\nLine %d\nFile %s\n", __LINE__, __FILE__);
        exit(1);
    }

    /* Wait for the websocket server thread to finish... */
    if (pthread_join(wsserver_thread, NULL) != 0) {
        sendf(stderr, LOG_ERROR, "Error waiting for websocket server to finish...\nLine %d\nFile %s\n", __LINE__, __FILE__);
        exit(1);
    }

    return 0;
}
