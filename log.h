#pragma once

#include <stddef.h>

#define GEN_LOG 0
#define SCN_LOG 1

typedef enum {
	LOG_GEN=0,
	LOG_SCN,
	LOG_DEBUG,
	LOG_COUNT
} LogType;

void log_teardown();
void log_init();

void log_raw(LogType type, char *data, size_t sz, size_t elem_sz);
void log_fmt(LogType type, char *text, ...);
void log_fmt_ts(LogType type, char *text, ...);
void log_clear(LogType type);
