#include "tinyc.h"

noreturn void error(char *format, ...) {
  va_list args;

  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  exit(-1);
}