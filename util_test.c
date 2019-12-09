#include "tinyc.h"

static void vector_test() {
  Vector *v = new_vec_with_capacity(64);
  for (int i = 0; i < 64; i++) {
    vec_pushi(v, i);
  }

  assert(64 == v->capacity);
  assert(64 == v->len);
  assert((intptr_t)0 == (intptr_t)v->data[0]);
  assert((intptr_t)1 == (intptr_t)v->data[1]);
  assert((intptr_t)32 == (intptr_t)v->data[32]);
  assert((intptr_t)63 == (intptr_t)v->data[63]);
}

static void vector_auto_extend_test() {
  Vector *v = new_vec_with_capacity(64);
  for (int i = 0; i < 65; i++) {
    vec_pushi(v, i);
  }

  assert(64 < v->capacity);
  assert(65 == v->len);
  assert((intptr_t)0 == (intptr_t)v->data[0]);
  assert((intptr_t)1 == (intptr_t)v->data[1]);
  assert((intptr_t)32 == (intptr_t)v->data[32]);
  assert((intptr_t)63 == (intptr_t)v->data[63]);
  assert((intptr_t)64 == (intptr_t)v->data[64]);
}

void run_all_tests() {
  vector_test();
  vector_auto_extend_test();
}