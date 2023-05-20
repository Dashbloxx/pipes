#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <pthread.h>

#include "include/wserver.h"
#include "include/log.h"

void *wserver(void *arg) {
    int server_socket, client_socket;
    struct sockaddr_in server_address, client_address;
    socklen_t client_address_len;

    /* Create the server socket... */
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        sendf(stderr, LOG_ERROR, "Error creating socket...\nLine %d\n", __LINE__);
        exit(1);
    }

    /* Set up the server address... */
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080);
    server_address.sin_addr.s_addr = INADDR_ANY;

    /* Bind the server socket to the specified address and port */
    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        sendf(stderr, LOG_ERROR, "Error binding socket...\nLine %d\nFile %s\n", __LINE__, __FILE__);
        exit(1);
    }

    /* Start listening for client connections */
    if (listen(server_socket, 10) == -1) {
        sendf(stderr, LOG_ERROR, "Error listening for new connections...\nLine %d\nFile %s\n", __LINE__, __FILE__);
        exit(1);
    }

    sendf(stderr, LOG_INFO, "Web server has successfully started!\n");

    while (1) {
        /* Accept a client connection */
        client_address_len = sizeof(client_address);
        client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_address_len);
        if (client_socket == -1) {
            sendf(stderr, LOG_ERROR, "Error accepting connection...\nLine %d\nFile %s\n", __LINE__, __FILE__);
            exit(1);
        }

        /* Read the HTTP request */
        char request[1024];
        memset(request, 0, sizeof(request));
        read(client_socket, request, sizeof(request) - 1);

        /* Extract the requested file path from the request */
        char file_path[256];
        sscanf(request, "GET /%s", file_path);

        /* Construct the absolute file path */
        char absolute_path[512];
        snprintf(absolute_path, sizeof(absolute_path), "public/%s", file_path);

        /* Check if the requested file exists */
        struct stat file_stat;
        if (stat(absolute_path, &file_stat) == -1) {
            /* File not found, send 404 response */
            const char* response = "HTTP/1.1 404 Not Found\r\n"
                                   "Content-Type: text/html\r\n"
                                   "\r\n"
                                   "<html><body><h1>404 Not Found</h1></body></html>";
            write(client_socket, response, strlen(response));
        } else {
            /* File found, open and send the file contents as the response */
            FILE* file = fopen(absolute_path, "r");
            if (file == NULL) {
                sendf(stderr, LOG_ERROR, "Error opening file...\nLine %d\nFile %s\n", __LINE__, __FILE__);
                exit(1);
            }

            /* Determine the file size */
            fseek(file, 0, SEEK_END);
            long file_size = ftell(file);
            fseek(file, 0, SEEK_SET);

            /* Allocate memory for the file contents */
            char* file_contents = (char*)malloc(file_size + 1);
            if (file_contents == NULL) {
                sendf(stderr, LOG_ERROR, "Error allocating memory...\nLine %d\nFile %s\n", __LINE__, __FILE__);
                exit(1);
            }

            /* Read the file contents */
            fread(file_contents, 1, file_size, file);
            fclose(file);

            /* Null-terminate the file contents */
            file_contents[file_size] = '\0';

            /* Prepare the HTTP response */
            const char* response = "HTTP/1.1 200 OK\r\n"
                                   "Content-Type: text/html\r\n"
                                   "\r\n";
            write(client_socket, response, strlen(response));
            write(client_socket, file_contents, file_size);

            /* Free the allocated memory */
            free(file_contents);
        }

        /* Close the client socket */
        close(client_socket);
    }

    /* Close the server socket */
    close(server_socket);

    pthread_exit(NULL);
}