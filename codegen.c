#include "tinyc.h"

static int _label_counter = 0;
static char *gen_label(char *prefix) {
  char *label = malloc(strlen(prefix) + 6);
  snprintf(label, strlen(prefix) + 6, ".L%s%04d", prefix, _label_counter++);
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

void gen(Node *node) {
  char *l1 = NULL;
  char *l2 = NULL;

  switch (node->kind) {
    case ND_RETURN:
      gen(node->lhs);
      printf("  pop rax\n");
      printf("  mov rsp, rbp\n");
      printf("  pop rbp\n");
      printf("  ret\n");
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