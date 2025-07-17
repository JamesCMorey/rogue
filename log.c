#include "log.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>

static char *lognames[LOG_COUNT] = { "general.log", "bingus.log", "debug.log"};

static FILE *logfd[LOG_COUNT];

void log_teardown() {
	for (int i = 0; i < LOG_COUNT; ++i) {
		fclose(logfd[i]);
	}
}

void log_init() {
	for (int i = 0; i < LOG_COUNT; ++i) {
		// overwrite for now
		logfd[i] = fopen(lognames[i], "w");
	}
}

void log_clear(LogType type) {
	fclose(logfd[type]);
	logfd[type] = fopen(lognames[type], "w");
}

static char *timestamp()
{
	char *ts;
	time_t t;
	struct tm *tminfo;

	ts = malloc(sizeof(char) * 9);
	time(&t);
	tminfo = localtime(&t);

	sprintf(ts, "%d:%d:%d", tminfo->tm_hour, tminfo->tm_min,
		tminfo->tm_sec);

	return ts;
}

/* raw log */
void log_raw(LogType type, char *data, size_t sz, size_t elem_sz) {
	fwrite(data, sz, elem_sz, logfd[type]);
	fflush(logfd[type]);
}

/* log with format */
void log_fmt(LogType type, char *text, ...) {
	va_list args;

	va_start(args, text);
	vfprintf(logfd[type], text, args);
	va_end(args);

	fflush(logfd[type]);
}

/* log with format and prepended timestamp */
void log_fmt_ts(LogType type, char *text, ...)
{
	va_list args;

	fprintf(logfd[type], "%s: ", timestamp());

	va_start(args, text);
	vfprintf(logfd[type], text, args);
	va_end(args);

	fflush(logfd[type]);
}
