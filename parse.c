#include "tinyc.h"

Node *code[100];
char *user_input;
// 現在着目しているトークン
Vector *tokens;
int current;

// 次のトークンが期待している記号のときには、トークンを1つ読み進めて
// 真を返す。それ以外の場合には偽を返す。
static bool consume(char *op) {
  Token *token = tokens->data[current];
  if (token->kind != TK_RESERVED || strlen(op) != token->len ||
      memcmp(token->str, op, token->len)) {
    return false;
  }
  current++;
  return true;
}

static Token *consume_ident() {
  Token *token = tokens->data[current];
  if (token->kind != TK_IDENT) {
    return NULL;
  }
  current++;
  return token;
}

// 次のトークンが期待している記号のときには、トークンを1つ読み進める。
// それ以外の場合にはエラーを報告する。
static void expect(char *op) {
  Token *token = tokens->data[current];
  if (token->kind != TK_RESERVED || strlen(op) != token->len ||
      memcmp(token->str, op, token->len)) {
    error_at(token->str, "'%c'ではありません", op);
  }
  current++;
}

// 次のトークンが数値の場合、トークンを1つ読み進めてその数値を返す。
// それ以外の場合にはエラーを報告する。
static int expect_number() {
  Token *token = tokens->data[current];
  if (token->kind != TK_NUM) {
    error_at(token->str, "数ではありません");
  }
  current++;
  return token->val;
}

static bool at_eof() {
  Token *token = tokens->data[current];
  return token->kind == TK_EOF;
}

static Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

static Node *new_node_num(int val) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;
  return node;
}

static Node *expr();

// primary = num | ident | "(" expr ")"
static Node *primary() {
  // 次のトークンが"("なら、"(" expr ")" のはず
  if (consume("(")) {
    Node *node = expr();
    expect(")");
    return node;
  }

  Token *tok = consume_ident();
  if (tok) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_LVAR;
    node->offset = (tok->str[0] - 'a' + 1) * 8;
    return node;
  }

  // そうでなければ数値のはず
  return new_node_num(expect_number());
}

// unary = ( "+" | "-" )? primary
static Node *unary() {
  if (consume("+")) return primary();
  if (consume("-")) return new_node(ND_SUB, new_node_num(0), primary());
  return primary();
}

// mul = unary ("*" unary | "/" unary)*
static Node *mul() {
  Node *node = unary();
  for (;;) {
    if (consume("*"))
      node = new_node(ND_MUL, node, unary());
    else if (consume("/"))
      node = new_node(ND_DIV, node, unary());
    else
      return node;
  }
}

// add = mul ("+" mul | "-" mul)*
static Node *add() {
  Node *node = mul();
  for (;;) {
    if (consume("+"))
      node = new_node(ND_ADD, node, mul());
    else if (consume("-"))
      node = new_node(ND_SUB, node, mul());
    else
      return node;
  }
}

// relational = add( "<" add | "<=" add | ">" add | ">=" add)*
static Node *relational() {
  Node *node = add();
  for (;;) {
    if (consume("<"))
      node = new_node(ND_LT, node, add());
    else if (consume("<="))
      node = new_node(ND_LTE, node, add());
    else if (consume(">"))
      node = new_node(ND_LT, add(), node);
    else if (consume(">="))
      node = new_node(ND_LTE, add(), node);
    else
      return node;
  }
}

// equality = relational ("==" relational | "!=" relational)*
static Node *equality() {
  Node *node = relational();
  for (;;) {
    if (consume("=="))
      node = new_node(ND_EQ, node, relational());
    else if (consume("!="))
      node = new_node(ND_NEQ, node, relational());
    else
      return node;
  }
}

// assign = equality("=" assign)?
static Node *assign() {
  Node *node = equality();
  if (consume("=")) node = new_node(ND_ASSIGN, node, assign());
  return node;
}

static Node *expr() { return assign(); }

static Node *stmt() {
  Node *node = expr();
  expect(";");
  return node;
}

Node *program(Vector *in) {
  tokens = in;
  int i = 0;
  while (!at_eof()) code[i++] = stmt();
  code[i] = NULL;
}