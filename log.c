#include "log.h"
#include <stdio.h>

#define LOG_FILENAME "bingus"
static FILE *logfd;

void log_teardown() {
	fclose(logfd);
}

void log_init() {
	// overwrite for now
	logfd = fopen(LOG_FILENAME, "w");
}

void wlog(char *msg, size_t sz, size_t elem_sz) {
	fwrite(msg, sz, elem_sz, logfd);
	fflush(logfd);
}

void plog(char *msg) {
	fprintf(logfd, "%s", msg);
	fflush(logfd);
}
