#include "tinyc.h"

static void vector_test() {
  Vector *v = new_vec_with_capacity(64);
  for (int i = 0; i < 64; i++) {
    vec_push(v, (int *)i);
  }

  assert(64 == v->capacity);
  assert(64 == v->len);
  assert((int *)0 == (int *)v->data[0]);
  assert((int *)1 == (int *)v->data[1]);
  assert((int *)32 == (int *)v->data[32]);
  assert((int *)63 == (int *)v->data[63]);
}

static void vector_auto_extend_test() {
  Vector *v = new_vec_with_capacity(64);
  for (int i = 0; i < 65; i++) {
    vec_push(v, (int *)i);
  }

  assert(64 < v->capacity);
  assert(65 == v->len);
  assert((int *)0 == (int *)v->data[0]);
  assert((int *)1 == (int *)v->data[1]);
  assert((int *)32 == (int *)v->data[32]);
  assert((int *)63 == (int *)v->data[63]);
  assert((int *)64 == (int *)v->data[64]);
}

void run_all_tests() {
  vector_test();
  vector_auto_extend_test();
}