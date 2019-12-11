#include "tinyc.h"

Node *code[100];
char *user_input;
// 現在着目しているトークン
Vector *tokens;
int current;
// ローカル変数
LVar *locals;

// 変数を名前で検索する。見つからなかった場合はNULLを返す。
static LVar *find_lvar(Token *tok) {
  for (LVar *var = locals; var; var = var->next)
    if (var->len == tok->len && !memcmp(tok->str, var->name, var->len))
      return var;
  return NULL;
}

// 次のトークンが期待している記号のときには、トークンを1つ読み進めて
// 真を返す。それ以外の場合には偽を返す。
static bool consume(TokenKind kind) {
  Token *token = tokens->data[current];
  if (token->kind != kind) {
    return false;
  }
#ifdef DEBUG
  fprintf(stderr, "consume: token=%d\n", kind);
#endif
  current++;
  return true;
}

static Token *consume_ident() {
  Token *token = tokens->data[current];
  if (token->kind != TK_IDENT) {
    return NULL;
  }
#ifdef DEBUG
  fprintf(stderr, "consume: ident=%.*s\n", token->len, token->str);
#endif
  current++;
  return token;
}

// 次のトークンが数値の場合、トークンを1つ読み進めてその数値を返す。
// それ以外の場合にはエラーを報告する。
static int consume_number() {
  Token *token = tokens->data[current];
  if (token->kind != TK_NUM) {
    error_at(token->str, "数ではありません");
  }
#ifdef DEBUG
  fprintf(stderr, "consume: number=%d\n", token->val);
#endif
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

static Node *new_if_node(Node *cond, Node *body, Node *alt_body) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_IF;
  node->cond = cond;
  node->then = body;
  node->els = alt_body;
  return node;
}

static Node *new_while_node(Node *cond, Node *body) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_WHILE;
  node->cond = cond;
  node->body = body;
  return node;
}

static Node *new_for_node(Node *init, Node *cond, Node *inc, Node *body) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_FOR;
  node->init = init;
  node->cond = cond;
  node->inc = inc;
  node->body = body;
  return node;
}

static Node *expr();

// primary = num | ident | "(" expr ")"
static Node *primary() {
  // 次のトークンが"("なら、"(" expr ")" のはず
  if (consume('(')) {
    Node *node = expr();
    consume(')');
    return node;
  }

  Token *tok = consume_ident();
  if (tok) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_LVAR;

    LVar *lvar = find_lvar(tok);
    if (lvar) {
      node->offset = lvar->offset;
    } else {
#ifdef DEBUG
      fprintf(stderr, "before allocating local variable: %.*s\n", tok->len,
              tok->str);
#endif
      lvar = calloc(1, sizeof(LVar));
      lvar->next = locals;
      lvar->name = tok->str;
      lvar->len = tok->len;
      lvar->offset = locals->offset + 8;
      node->offset = lvar->offset;
      locals = lvar;
#ifdef DEBUG
      fprintf(stderr, "after allocation\n");
#endif
    }
    return node;
  }

  // そうでなければ数値のはず
  return new_node_num(consume_number());
}

// unary = ( "+" | "-" )? primary
static Node *unary() {
  if (consume('+')) return primary();
  if (consume('-')) return new_node(ND_SUB, new_node_num(0), primary());
  return primary();
}

// mul = unary ("*" unary | "/" unary)*
static Node *mul() {
  Node *node = unary();
  for (;;) {
    if (consume('*'))
      node = new_node(ND_MUL, node, unary());
    else if (consume('/'))
      node = new_node(ND_DIV, node, unary());
    else
      return node;
  }
}

// add = mul ("+" mul | "-" mul)*
static Node *add() {
  Node *node = mul();
  for (;;) {
    if (consume('+'))
      node = new_node(ND_ADD, node, mul());
    else if (consume('-'))
      node = new_node(ND_SUB, node, mul());
    else
      return node;
  }
}

// relational = add( "<" add | "<=" add | ">" add | ">=" add)*
static Node *relational() {
  Node *node = add();
  for (;;) {
    if (consume('<'))
      node = new_node(ND_LT, node, add());
    else if (consume(TK_LE))
      node = new_node(ND_LTE, node, add());
    else if (consume('>'))
      node = new_node(ND_LT, add(), node);
    else if (consume(TK_GE))
      node = new_node(ND_LTE, add(), node);
    else
      return node;
  }
}

// equality = relational ("==" relational | "!=" relational)*
static Node *equality() {
  Node *node = relational();
  for (;;) {
    if (consume(TK_EQ))
      node = new_node(ND_EQ, node, relational());
    else if (consume(TK_NE))
      node = new_node(ND_NEQ, node, relational());
    else
      return node;
  }
}

// assign = equality("=" assign)?
static Node *assign() {
  Node *node = equality();
  if (consume('=')) node = new_node(ND_ASSIGN, node, assign());
  return node;
}

static Node *expr() { return assign(); }

// stmt = expr ";"
// | "if" "(" expr ")" stmt ("else" stmt)?
// | "while" "(" expr ")" stmt
// | "for" "(" expr? ";" expr? ";" expr? ")" stmt
// | "return" expr ";"
static Node *stmt() {
  Node *node;

  if (consume(TK_IF)) {
    consume('(');
    Node *cond_expr = expr();
    consume(')');
    Node *body = stmt();
    Node *alt_body = NULL;
    if (consume(TK_ELSE)) {
      alt_body = stmt();
    }
    return new_if_node(cond_expr, body, alt_body);
  }
  if (consume(TK_WHILE)) {
    consume('(');
    Node *cond_expr = expr();
    consume(')');
    Node *body = stmt();
    return new_while_node(cond_expr, body);
  }
  if (consume(TK_FOR)) {
    consume('(');

    Node *init_expr = NULL;
    if (!consume(';')) {
      init_expr = expr();
      consume(';');
    }

    Node *cond_expr = NULL;
    if (!consume(';')) {
      cond_expr = expr();
      consume(';');
    }

    Node *inc_expr = NULL;
    if (!consume(')')) {
      inc_expr = expr();
      consume(')');
    }
    Node *body = stmt();
    return new_for_node(init_expr, cond_expr, inc_expr, body);
  }

  if (consume(TK_RETURN)) {
    node = calloc(1, sizeof(Node));
    node->kind = ND_RETURN;
    node->lhs = expr();
  } else {
    node = expr();
  }

  consume(';');
  return node;
}

Node *program(Vector *in) {
  tokens = in;
  current = 0;

  // set dummy locals
  locals = calloc(1, sizeof(LVar));
  locals->name = "";
  locals->len = 0;
  locals->next = NULL;
  locals->offset = 0;

  int i = 0;
  while (!at_eof()) code[i++] = stmt();
  code[i] = NULL;
}

Program *parse(Vector *in) {
  Program *prog = malloc(sizeof(Program));
  return prog;
}