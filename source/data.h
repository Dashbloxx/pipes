#pragma once

void generate_token(char *token, size_t token_length);
void initialize_data();
int create_account(const char *username, const char *password_hash, char *token_buffer);
int enter_account(char *username, char *password_hash, char *token_buffer);