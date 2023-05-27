#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#include "ws_server.h"
#include "http_server.h"
#include "logger.h"
#include "env.h"
#include "data.h"
#include "db.h"

MYSQL *conn;

int main(int argc, const char **argv)
{
	sendf(stdout, LOG_INFO, "Pipes server %s. Copyright (C) 2023 Matthew Edward Majfud-Wilinski\n\tThis binary has been compiled on %s at %s.\n", VERSION, __DATE__, __TIME__);

	/* Let's first connect to our MySQL/MariaDB database before starting Web server & WebSocket server... */

	conn = mysql_init(NULL);
	if (conn == NULL) {
		sendf(stderr, LOG_ERROR, "Failed to initialize MySQL connection...\n");
		exit(1);
	}

	/* We get the database creds from env.h, which contains passwords & security-related stuff... */
	if (mysql_real_connect(conn, DB_HOST, DB_USERNAME, DB_PASSWORD, DB_NAME, 0, NULL, 0) == NULL) {
		sendf(stderr, LOG_ERROR, "Failed to connect to the MySQL server: %s\n", mysql_error(conn));
		mysql_close(conn);
		exit(1);
	}
	else {
		sendf(stdout, LOG_INFO, "Successfully connected to MySQL server...\n");
	}

	/* Let's initialize some stuff in our database if they don't exist yet... */
	initialize_data();

	/* Let's fork this process, creating a clone of the current process... */
	pid_t pid = fork();

	if(pid == -1) {
		sendf(stderr, LOG_ERROR, "Failed to use fork()...\n");
	}
	else if(pid == 0) {
		/* We are the child process! */
		wsserver();
	}
	else {
		/* We seem to be the parent process! */
		wserver();
	}

	return 0;
}
