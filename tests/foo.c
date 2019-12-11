#include <stdio.h>
int foo() { printf("OK in foo\n"); }
int bar(int x, int y) {
  printf("OK %d in foo\n", x * 2 + y * 3);
  return x * 2 + y * 3;
}