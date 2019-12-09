#include "tinyc.h"

#ifdef DEBUG

void _dump_token(FILE *stream, Token *t) {
  assert(t != NULL);
  if (t->kind < 256) {
    fprintf(stream, "{kind: %c}\n", (char)(t->kind));
  } else {
    switch (t->kind) {
      case TK_NUM:
        fprintf(stream, "{kind: TK_NUM, val: %d}\n", t->val);
        break;
      case TK_IDENT:
        fprintf(stream, "{kind: TK_IDENT, str: ");
        fprintf(stream, "%.*s", t->len, t->str);
        fprintf(stream, "}\n");
        break;
      case TK_IF:
        fprintf(stream, "{kind: TK_IF}\n");
        break;
      case TK_ELSE:
        fprintf(stream, "{kind: TK_ELSE}\n");
        break;
      case TK_RETURN:
        fprintf(stream, "{kind: TK_RETURN}\n");
        break;
      case TK_EQ:
        fprintf(stream, "{kind: TK_EQ}\n");
        break;
      case TK_NE:
        fprintf(stream, "{kind: TK_NE}\n");
        break;
      case TK_LE:
        fprintf(stream, "{kind: TK_LE}\n");
        break;
      case TK_GE:
        fprintf(stream, "{kind: TK_GE}\n");
        break;
      case TK_EOF:
        fprintf(stream, "{kind: TK_EOF}\n");
        break;
    }
  }
}

inline void dump_token(Token *t) { _dump_token(stderr, t); }
#endif