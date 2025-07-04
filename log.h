#pragma once

#include <stddef.h>

void log_teardown();
void log_init();
void wlog(char *msg, size_t sz, size_t elem_sz);
void plog(char *msg);
