#pragma once

/* These are used for output to the log... */
#define LOG_INFO "\x1b[37m[\x1b[34;1mINFO\x1b[37m]: \x1b[0m"
#define LOG_ERROR "\x1b[37m[\x1b[31;1mERROR\x1b[37m]: \x1b[0m"
#define LOG_WARN "\x1b[37m[\x1b[33;1mWARN\x1b[37m]: \x1b[0m"
#define LOG_DEBUG "\x1b[37m[\x1b[32;1mDEBUG\x1b[37m]: \x1b[0m"

int sendf(FILE* teletype, const char* prefix, const char* format, ...);
