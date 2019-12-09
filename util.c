#include "tinyc.h"

#ifndef VEC_DEFAULT_CAPACITY
#define VEC_DEFAULT_CAPACITY 256
#endif

Vector *new_vec_with_capacity(int capacity) {
  Vector *v = calloc(1, sizeof(Vector));
  v->data = calloc(capacity, sizeof(void *));
  v->capacity = capacity;
  v->len = 0;
  return v;
}

/* inline */ Vector *new_vec(void) {
  return new_vec_with_capacity(VEC_DEFAULT_CAPACITY);
}

static void vec_extend_capacity(Vector *v) {
  void **new_data = calloc(v->capacity * 2, sizeof(void *));
  memcpy(new_data, v->data, v->capacity * sizeof(void *));
  free(v->data);
  v->data = new_data;
  v->capacity = v->capacity * 2;
}

void vec_push(Vector *v, void *elem) {
  if (!v) {
    error("called vec_push with NULL");
  }
  if (v->capacity <= v->len) {
    vec_extend_capacity(v);
  }
  v->data[v->len] = elem;
  v->len += 1;
}

void vec_pushi(Vector *v, int val) {
  int *elem = calloc(1, sizeof(int));
  *elem = val;
  vec_push(v, elem);
}

void *vec_pop(Vector *v) {
  if (!v) {
    error("called vec_pop with NULL");
  }
  if (v->len == 0) {
    error("called vec_pop with empty vector");
  }
  void *ret = v->data[v->len - 1];
  v->len -= 1;
  return ret;
}

void *vec_last(Vector *v) {
  if (!v) {
    error("called vec_pop with NULL");
  }
  if (v->len == 0) {
    error("called vec_pop with empty vector");
  }
  void *ret = v->data[v->len - 1];
  return ret;
}

// bool vec_contains(Vector *v, void *elem) {}

// bool vec_union1(Vector *v, void *elem) {}
