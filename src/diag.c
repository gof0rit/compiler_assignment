#include "diag.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

int num_of_errors;
int num_of_warnings;

void error(const char *fmt, ...)
{
  if (num_of_errors++ > 10)
    return;

  va_list ap;
  va_start(ap, fmt);
  fprintf(stderr, "error: ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  va_end(ap);
}

void warning(const char *fmt, ...)
{
  if (num_of_warnings++ > 10)
    return;

  va_list ap;
  va_start(ap, fmt);
  fprintf(stderr, "warning: ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  va_end(ap);
}

