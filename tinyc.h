#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>

// error.c
noreturn void error(char *format, ...);
noreturn void error_at(char *loc, char *fmt, ...);

// util.c

typedef struct Vector {
  void **data;
  int capacity;
  int len;
} Vector;

Vector *new_vec_with_capacity(int capacity);
Vector *new_vec(void);
void vec_push(Vector *v, void *elem);
void vec_pushi(Vector *v, int val);
void *vec_pop(Vector *v);
void *vec_last(Vector *v);
// bool vec_contains(Vector *v, void *elem);
// bool vec_union1(Vector *v, void *elem);

void run_all_tests();

// parse.c

typedef enum {
  TK_RESERVED,  // 記号
  TK_IDENT,     // 識別子
  TK_NUM,       // 整数トークン
  TK_EOF,       // 入力の終わりを表すトークン
} TokenKind;

typedef struct Token Token;

struct Token {
  TokenKind kind;  // トークンの型
  int val;         // kindがTK_NUMの場合、その数値
  char *str;       // トークン文字列
  int len;         // トークンの長さ
};

// 抽象構文木のノードの種類
typedef enum {
  ND_ADD,     // +
  ND_SUB,     // -
  ND_MUL,     // *
  ND_DIV,     // /
  ND_NUM,     // 整数
  ND_EQ,      // ==
  ND_NEQ,     // !=
  ND_LT,      // <
  ND_LTE,     // <=
  ND_ASSIGN,  // =
  ND_LVAR,    // ローカル変数
} NodeKind;

typedef struct Node Node;
typedef struct Node {
  NodeKind kind;  // ノードの型
  Node *lhs;      // 左辺
  Node *rhs;      // 右辺
  int val;        // kindがND_NUMの場合のみ使う
  int offset;     // kindがND_LVARの場合のみ使う
} Node;

// 入力プログラム
extern Node *code[100];

Vector *tokenize(char *p);
Node *program(Vector *tokens);
void gen(Node *node);

// debug.c
#ifdef DEBUG
void dump_token();
#endif