#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#include "include/wserver.h"
#include "include/wsserver.h"
#include "include/log.h"

int main() {
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
        fprintf(stderr, "Error waiting for websocket server to finish...\nLine %d\nFile %s\n", __LINE__, __FILE__);
        exit(1);
    }

    return 0;
}
