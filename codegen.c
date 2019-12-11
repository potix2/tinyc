#include "tinyc.h"

static void gen(Node *node);
static char *gen_label(char *prefix) {
  static int cnt = 0;
  char *label = malloc(strlen(prefix) + 7);
  snprintf(label, strlen(prefix) + 7, ".L%s%04d", prefix, cnt++);
  return label;
}

static void gen_lval(Node *node) {
  if (node->kind != ND_LVAR) {
    error("代入の左辺値が変数ではありません");
  }

  printf("  mov rax, rbp\n");
  printf("  sub rax, %d\n", node->offset);
  printf("  push rax\n");
}

// x86_64 Calling Conventionに従って関数呼び出しコードを生成する
// SEE: https://wiki.osdev.org/System_V_ABI
static void gen_apply(Node *node) {
  assert(node->args->len <= 6);

  static char *regs[6] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
  int i = 0;
  for (i = 0; i < node->args->len; i++) gen(node->args->data[i]);
  for (; i > 0; i--) {
    printf("  pop rax\n");
    printf("  mov %s, rax\n", regs[i - 1]);
  }
  printf("  call %s\n", node->name);
  printf("  push rax\n");
}

static void gen(Node *node) {
  char *l1 = NULL;
  char *l2 = NULL;

  switch (node->kind) {
    case ND_FUNC:
      return;

    case ND_RETURN:
      gen(node->lhs);
      printf("  pop rax\n");
      printf("  mov rsp, rbp\n");
      printf("  pop rbp\n");
      printf("  ret\n");
      return;

    case ND_BLOCK:
      for (int i = 0; i < node->stmts->len; i++) {
        gen(node->stmts->data[i]);
        printf("  pop rax\n");
      }
      return;

    case ND_APPLY:
      if (node->args->len > 6) {
        error("Not supported for calling funcation with more than 6 arguments");
      }
      gen_apply(node);
      return;

    case ND_IF:
      gen(node->cond);
      printf("  pop rax\n");
      printf("  cmp rax, 0\n");
      l1 = gen_label("else");
      l2 = gen_label("end");
      printf("  je %s\n", l1);
      gen(node->then);
      printf("  jmp %s\n", l2);
      printf("%s:\n", l1);
      if (node->els != NULL) {
        gen(node->els);
      } else {
        printf("  nop\n");
      }
      printf("%s:\n", l2);
      return;

    case ND_WHILE:
      l1 = gen_label("begin");
      l2 = gen_label("end");
      printf("%s:\n", l1);
      gen(node->cond);
      printf("  pop rax\n");
      printf("  cmp rax, 0\n");
      printf("  je %s\n", l2);
      gen(node->body);
      printf("  jmp %s\n", l1);
      printf("%s:\n", l2);
      return;

    case ND_FOR:
      l1 = gen_label("begin");
      l2 = gen_label("end");
      if (node->init != NULL) {
        gen(node->init);
      }
      printf("%s:\n", l1);
      if (node->cond != NULL) {
        gen(node->cond);
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  je %s\n", l2);
      }
      gen(node->body);
      if (node->inc != NULL) {
        gen(node->inc);
      }
      printf("  jmp %s\n", l1);
      printf("%s:\n", l2);
      return;

    case ND_NUM:
      printf("  push %d\n", node->val);
      return;

    case ND_LVAR:
      gen_lval(node);
      printf("  pop rax\n");
      printf("  mov rax, [rax]\n");
      printf("  push rax\n");
      return;

    case ND_ASSIGN:
      gen_lval(node->lhs);
      gen(node->rhs);

      printf("  pop rdi\n");
      printf("  pop rax\n");
      printf("  mov [rax], rdi\n");
      printf("  push rdi\n");
      return;
  }

  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch (node->kind) {
    case ND_ADD:
      printf("  add rax, rdi\n");
      break;
    case ND_SUB:
      printf("  sub rax, rdi\n");
      break;
    case ND_MUL:
      printf("  imul rax, rdi\n");
      break;
    case ND_DIV:
      printf("  cqo\n");
      printf("  idiv rdi\n");
      break;
    case ND_EQ:
      printf("  cmp rax, rdi\n");
      printf("  sete al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_NEQ:
      printf("  cmp rax, rdi\n");
      printf("  setne al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_LT:
      printf("  cmp rax, rdi\n");
      printf("  setl al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_LTE:
      printf("  cmp rax, rdi\n");
      printf("  setle al\n");
      printf("  movzb rax, al\n");
      break;
  }

  printf("  push rax\n");
}

void gen_program(Program *prog) {
  // アセンブリの前半部分を出力
  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  // プロローグ
  // 変数の領域を確保する
  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
  printf("  sub rsp, %d\n", prog->variables->len * 8);

  for (int i = 0; i < prog->code->len; i++) {
    gen(prog->code->data[i]);

    // 式の評価結果としてスタックに一つの値が残っている
    // はずなので、スタックが溢れないようにポップしておく
    printf("  pop rax\n");
  }

  // エピローグ
  // 最後の式の結果がRAXに残っているのでそれが返り値になる
  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");
  printf("  ret\n");
}