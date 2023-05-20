#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "include/log.h"

/* Log text using this function... */
int sendf(FILE* teletype, const char* prefix, const char* format, ...) {
	char* full_format = NULL;
	va_list args;

	/* Allocate memory for the full format string, including the prefix */
	full_format = (char*) malloc(strlen(prefix) + strlen(format) + 1);
	if (full_format == NULL) {
		return -1;
	}

	/* Concatenate the prefix and format strings */
	strcpy(full_format, prefix);
	strcat(full_format, format);

	/* Call vfprintf with the TTY file pointer and the full format string and any additional arguments */
	va_start(args, format);
	if (teletype == NULL) {
		vprintf(full_format, args);
	} else {
		vfprintf(teletype, full_format, args);
	}
	va_end(args);

	/* Free the memory we allocated */
	free(full_format);
    return 0;
}