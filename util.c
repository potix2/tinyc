#include "tinyc.h"

#ifndef VEC_DEFAULT_CAPACITY
#define VEC_DEFAULT_CAPACITY 16
#endif

Vector *new_vec_with_capacity(int capacity) {
  Vector *v = malloc(sizeof(Vector));
  v->data = malloc(sizeof(void *) * capacity);
  v->capacity = capacity;
  v->len = 0;
  return v;
}

/* inline */ Vector *new_vec(void) {
  return new_vec_with_capacity(VEC_DEFAULT_CAPACITY);
}

void vec_push(Vector *v, void *elem) {
  assert(v != NULL);
  if (v->capacity == v->len) {
    v->capacity *= 2;
    v->data = realloc(v->data, v->capacity * sizeof(void *));
  }
  v->data[v->len++] = elem;
}

void vec_pushi(Vector *v, int val) {
  assert(v != NULL);
  vec_push(v, (void *)(intptr_t)val);
}

void *vec_pop(Vector *v) {
  assert(v != NULL);
  assert(v->len);
  return v->data[--v->len];
}

void *vec_last(Vector *v) {
  assert(v != NULL);
  assert(v->len);
  return v->data[v->len - 1];
}

// bool vec_contains(Vector *v, void *elem) {}

// bool vec_union1(Vector *v, void *elem) {}
