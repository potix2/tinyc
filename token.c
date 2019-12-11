#include "tinyc.h"

extern char *user_input;

static int is_head_of_ident(char c) {
  return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || (c == '_');
}

static int is_alnum(char c) {
  return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') ||
         ('0' <= c && c <= '9') || (c == '_');
}

// 新しいトークンを作成する
static Token *new_token(int kind, char *str, int len) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  tok->len = len;
  return tok;
}

// 新しい数値トークンを作成する
static Token *new_num_token(char **p) {
  Token *tok = new_token(TK_NUM, *p, 0);
  tok->val = strtol(*p, p, 10);
  return tok;
}

// 新しいIDENTトークンを作成する
static Token *new_ident_token(char **p) {
  char *begin = *p;
  int len = 0;
  while (begin[len] && is_alnum(begin[len])) len++;

  *p += len;
  return new_token(TK_IDENT, begin, len);
}

// 入力文字列pをトークナイズしてそれを返す
Vector *tokenize(char *p) {
  user_input = p;

  Vector *tokens = new_vec();

  while (*p) {
#ifdef DEBUG
    fprintf(stderr, "pos=%s\n", p);
    fprintf(stderr, "tokens->len = %d\n", tokens->len);
    if (tokens->len > 0) {
      dump_token(vec_last(tokens));
    }
#endif

    // 空白文字をスキップ
    if (isspace(*p)) {
      p++;
      continue;
    }

    if (strncmp(p, "<=", 2) == 0) {
      vec_push(tokens, new_token(TK_LE, p, 2));
      p += 2;
      continue;
    }

    if (strncmp(p, ">=", 2) == 0) {
      vec_push(tokens, new_token(TK_GE, p, 2));
      p += 2;
      continue;
    }

    if (strncmp(p, "==", 2) == 0) {
      vec_push(tokens, new_token(TK_EQ, p, 2));
      p += 2;
      continue;
    }

    if (strncmp(p, "!=", 2) == 0) {
      vec_push(tokens, new_token(TK_NE, p, 2));
      p += 2;
      continue;
    }

    if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' ||
        *p == ')' || *p == '<' || *p == '>' || *p == ';' || *p == '=' ||
        *p == '{' || *p == '}') {
      vec_push(tokens, new_token(*p, p, 1));
      p += 1;
      continue;
    }

    if (strncmp(p, "if", 2) == 0 && !is_alnum(p[2])) {
      vec_push(tokens, new_token(TK_IF, p, 0));
      p += 2;
      continue;
    }

    if (strncmp(p, "for", 3) == 0 && !is_alnum(p[3])) {
      vec_push(tokens, new_token(TK_FOR, p, 0));
      p += 3;
      continue;
    }

    if (strncmp(p, "else", 4) == 0 && !is_alnum(p[4])) {
      vec_push(tokens, new_token(TK_ELSE, p, 0));
      p += 4;
      continue;
    }

    if (strncmp(p, "while", 5) == 0 && !is_alnum(p[5])) {
      vec_push(tokens, new_token(TK_WHILE, p, 0));
      p += 5;
      continue;
    }
    if (strncmp(p, "return", 6) == 0 && !is_alnum(p[6])) {
      vec_push(tokens, new_token(TK_RETURN, p, 0));
      p += 6;
      continue;
    }

    if (is_head_of_ident(*p)) {
      vec_push(tokens, new_ident_token(&p));
      continue;
    }

    if (isdigit(*p)) {
      vec_push(tokens, new_num_token(&p));
      continue;
    }

    error_at(p, "トークナイズできません");
  }

  vec_push(tokens, new_token(TK_EOF, p, 0));
  return tokens;
}