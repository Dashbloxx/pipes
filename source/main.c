#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#include "ws_server.h"
#include "http_server.h"
#include "logger.h"
#include "env.h"

int main(int argc, const char **argv)
{
	sendf(stdout, LOG_INFO, "Pipes server %s. Copyright (C) 2023 Matthew Edward Majfud-Wilinski\n\tThis binary has been compiled on %s at %s.\n", VERSION, __DATE__, __TIME__);

	/* Let's fork this process, creating a clone of the current process... */
	pid_t pid = fork();

	if(pid == -1) {
		sendf(stderr, LOG_ERROR, "Failed to use fork()...\n");
	}
	else if(pid == 0) {
		/* We are the child process! */
		wserver();
	}
	else {
		/* We seem to be the parent process! */
		wsserver();
	}

	return 0;
}
