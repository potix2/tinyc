#include "tinyc.h"

#ifdef DEBUG
static char *_TOKENKIND_STRING[] = {
    "TK_RESERVED",
    "TK_IDENT",
    "TK_NUM",
    "TK_EOF",
};

static char *kind_to_str(TokenKind kind) { return _TOKENKIND_STRING[kind]; }

void _dump_token(FILE *stream, Token *t) {
  assert(t != NULL);
  fprintf(stream, "{kind: %s, val: %d, str: %s, len: %d}\n",
          kind_to_str(t->kind), t->val, t->str, t->len);
}

inline void dump_token(Token *t) { _dump_token(stderr, t); }
#endif