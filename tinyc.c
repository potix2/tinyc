#include "tinyc.h"

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "引数の個数が正しくありません\n");
    return 1;
  }
  if (strncmp(argv[1], "-test", 5) == 0) {
    run_all_tests();
    return 0;
  }

  // トークナイズしてパースする
  Vector *tokens = tokenize(argv[1]);
#ifdef DEBUG
  fprintf(stderr, "dump tokens:\n");
  for (int i = 0; i < tokens->len; i++) {
    dump_token(tokens->data[i]);
  }
#endif
  Program *prog = parse(tokens);
  gen_program(prog);
  return 0;
}