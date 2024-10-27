#ifndef PANDA_DIAG_H
#define PANDA_DIAG_H

#include <stdarg.h>

extern int num_of_errors;
extern int num_of_warnings;

void error(const char *fmt, ...);
void warning(const char *fmt, ...);

#endif

