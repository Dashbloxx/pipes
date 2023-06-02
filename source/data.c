/*  Had to add `-I/usr/include/mariadb` to CFLAGS in Makefile in order to have `mysql.h` available...
    And yes, we are using mariadb instead of MySQL, because it is faster & still is horizontally scalable... */
#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/rand.h>

#include "data.h"
#include "logger.h"
#include "db.h"

void generate_token(char *token, size_t token_length) {
    const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    size_t charset_length = strlen(charset);
    
    for (size_t i = 0; i < token_length; i++) {
        token[i] = charset[rand() % charset_length];
    }
    
    token[token_length] = '\0';  // Add null terminator at the end
}

/* Use database & create specific tables if they don't exist yet... */
void initialize_data() {
    const char *query = "CREATE TABLE IF NOT EXISTS users (id CHAR(36) NOT NULL PRIMARY KEY, username VARCHAR(255) NOT NULL, password_hash VARCHAR(255) NOT NULL, token VARCHAR(255) NOT NULL, account_creation_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP, last_active_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP);";
    if (mysql_query(conn, query) != 0) {
        sendf(stderr, LOG_ERROR, "Failed to execute query: %s\n", mysql_error(conn));
        mysql_close(conn);

        /* Instead of returning -1, we just stop the program... */
        exit(1);
    }
}

/*  We use SHA256 for `password_hash`...
    If we get a return code of -2, then it means that the account already exists...
    If it's -1 however, it just means that the SQL execution failed somehow...
    Return code of 0 means that everything went okay... */
int create_account(const char *username, const char *password_hash, char *token_buffer) {
    char *query = malloc(1024);
    char *token = malloc(65); /* It's 65 bytes instead of 64 since we also want to add in a null terminator... */

    /* Check if the account already exists */
    sprintf(query, "SELECT COUNT(*) FROM users WHERE username = '%s';", username);
    if (mysql_query(conn, query) != 0) {
        sendf(stderr, LOG_ERROR, "Failed to execute query: %s\n", mysql_error(conn));
        free(token);
        free(query);
        return -1;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    MYSQL_ROW row = mysql_fetch_row(result);
    int account_count = atoi(row[0]);
    mysql_free_result(result);

    /* If the account already exists, return without creating a new one */
    if (account_count > 0) {
        sendf(stderr, LOG_ERROR, "Account already exists for username: %s\n", username);
        free(token);
        free(query);
        return -2;
    }

    /* Generate a secure random token using OpenSSL */
    generate_token(token, sizeof(token));

    /* Update the SQL query to include the token field */
    sprintf(query, "INSERT INTO users (id, username, password_hash, token) VALUES (UUID(), '%s', '%s', '%s');", username, password_hash, token);

    if (mysql_query(conn, query) != 0) {
        sendf(stderr, LOG_ERROR, "Tried creating an account but got an error trying to execute a query: %s\n", mysql_error(conn));
        free(token);
        free(query);
        return -1;
    }

    /* Copy the generated token to the provided buffer */
    strcpy(token_buffer, token);
    free(token);
    free(query);
    return 0;
}

/*  Function to enter account and get token
    0 = login failure, 1 = successful login */
int enter_account(char *username, char *password_hash, char *token_buffer) {
    char *query = malloc(1024);
    MYSQL_RES *result;
    MYSQL_ROW row;
    
    /* Construct the SQL query to select the token for the given username and password */
    sprintf(query, "SELECT token FROM users WHERE username = '%s' AND password_hash = '%s';", username, password_hash);
    
    /* Execute the query */
    if (mysql_query(conn, query) != 0) {
        sendf(stderr, LOG_ERROR, "Failed to execute query: %s\n", mysql_error(conn));
        free(query);
        return 0;  // Return 0 to indicate login failure
    }
    
    /* Store the query result */
    result = mysql_use_result(conn);
    
    /* Check if a row is returned */
    if ((row = mysql_fetch_row(result)) != NULL) {
        /* Copy the token to the provided buffer */
        strncpy(token_buffer, row[0], 64);
        token_buffer[65] = '\0';  /* Ensure null termination */
        
        mysql_free_result(result);
        free(query);
        return 1;  // Return 1 to indicate successful login
    }
    
    mysql_free_result(result);
    free(query);
    return 0;  // Return 0 to indicate login failure
}