#ifndef SCANNER_H
#define SCANNER_H

#include "location.h"

/* Token ID */
enum {
  TOK_LE = 256,
  TOK_GE,
  TOK_EQ,
  TOK_NE,
  TOK_AND,
  TOK_OR,
  TOK_ELSE,
  TOK_FOR,
  TOK_IF,
  TOK_INT,
  TOK_RETURN,
  TOK_VOID,
  TOK_WHILE,
  TOK_NUMBER,
  TOK_IDENTIFIER,
  TOK_EOF,
  TOK_BAND // 新加
};

typedef struct {
  /* Token ID */
  int id;
  /* Attributes */
  union {
    int ival;
    char *ptr;
  };
  Location loc;
} Token;

extern Token token;
extern Location current_location;
extern char *scan_buffer;
extern char *scan_buffer_end;
extern char *current_scan_ptr;

void init_scanner();
Token next_token();
void dump_token(Token tok);

#endif
