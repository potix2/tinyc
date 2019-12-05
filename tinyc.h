#ifndef __TINYC_H
#define __TINYC_H

typedef enum {
  TK_RESERVED,  // 記号
  TK_NUM,       // 整数トークン
  TK_EOF,       // 入力の終わりを表すトークン
} TokenKind;

typedef struct Token Token;

struct Token {
  TokenKind kind;  // トークンの型
  Token *next;     // 次の入力トークン
  int val;         // kindがTK_NUMの場合、その数値
  char *str;       // トークン文字列
  int len;         // トークンの長さ
};

// 抽象構文木のノードの種類
typedef enum {
  ND_ADD,  // +
  ND_SUB,  // -
  ND_MUL,  // *
  ND_DIV,  // /
  ND_NUM,  // 整数
  ND_EQ,   // ==
  ND_NEQ,  // !=
  ND_LT,   // <
  ND_LTE,  // <=
} NodeKind;

typedef struct Node Node;
struct Node {
  NodeKind kind;  // ノードの型
  Node *lhs;      // 左辺
  Node *rhs;      // 右辺
  int val;        // kindがND_NUMの場合のみ使う
};

// 現在着目しているトークン
extern Token *token;

// 入力プログラム
extern char *user_input;

extern Token *tokenize(char *p);
extern Node *expr();
extern void gen(Node *node);

#endif