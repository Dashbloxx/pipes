#pragma once

/* These are used for output to the log... */
#define LOG_INFO "\x1b[37m[\x1b[34;1mINFO\x1b[37m]: \x1b[0m"
#define LOG_ERROR "\x1b[37m[\x1b[31;1mERROR\x1b[37m]: \x1b[0m"
#define LOG_WARN "\x1b[37m[\x1b[33;1mWARN\x1b[37m]: \x1b[0m"

/* These are used for input and output for command-related stuff... */
#define COMMUNICATE_COMMAND "\x1b[37m[\x1b[32;1mCOMMAND\x1b[37m]: \x1b[0m"

int sendf(FILE* teletype, const char* prefix, const char* format, ...);