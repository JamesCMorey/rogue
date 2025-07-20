#include "log.h"
#include <stdio.h>
#include <stdarg.h>

static char *lognames[LOG_COUNT] = { "general.log", "bingus.log", "debug.log"};
static FILE *logfd[LOG_COUNT];

// ------ Log Primatives ------

void log_clear(LogType type) {
	fclose(logfd[type]);
	logfd[type] = fopen(lognames[type], "w");
}

void log_raw(LogType type, char *data, size_t sz, size_t elem_sz) {
	fwrite(data, sz, elem_sz, logfd[type]);
	fflush(logfd[type]);
}

void log_fmt(LogType type, char *text, ...) {
	va_list args;

	va_start(args, text);
	vfprintf(logfd[type], text, args);
	va_end(args);

	fflush(logfd[type]);
}

// ------ Init/Teardown ------

void log_init() {
	for (int i = 0; i < LOG_COUNT; ++i) {
		// overwrite for now
		logfd[i] = fopen(lognames[i], "w");
	}
}

void log_teardown() {
	for (int i = 0; i < LOG_COUNT; ++i) {
		fclose(logfd[i]);
	}
}
