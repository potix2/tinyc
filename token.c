#include "tinyc.h"

extern char *user_input;

// 新しいトークンを作成する
static Token *new_token(TokenKind kind, char *str, int len) {
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

// 入力文字列pをトークナイズしてそれを返す
Vector *tokenize(char *p) {
  user_input = p;

  Vector *tokens = new_vec();

  while (*p) {
    // 空白文字をスキップ
    if (isspace(*p)) {
      p++;
      continue;
    }

    if (strncmp(p, "<=", 2) == 0 || strncmp(p, ">=", 2) == 0 ||
        strncmp(p, "==", 2) == 0 || strncmp(p, "!=", 2) == 0) {
      vec_push(tokens, new_token(TK_RESERVED, p, 2));
      p += 2;
      continue;
    }

    if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' ||
        *p == ')' || *p == '<' || *p == '>' || *p == ';' || *p == '=') {
      vec_push(tokens, new_token(TK_RESERVED, p++, 1));
      continue;
    }

    if ('a' <= *p && *p <= 'z') {
      vec_push(tokens, new_token(TK_IDENT, p++, 1));
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