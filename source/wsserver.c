#include <pthread.h>

#include "include/wsserver.h"

void *wsserver(void *arg) {
    pthread_exit(NULL);
}