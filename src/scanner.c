#include "scanner.h"
#include <assert.h>
#include <ctype.h>
#include <search.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int opt_dump_token;

char *scan_buffer;
char *scan_buffer_end;
char *current_scan_ptr;

/* current token */
Token token;
Location current_location;

void init_scanner()
{
  ENTRY e;
  ENTRY *ep;
  static char *keywords[] = {
    "else", "for", "if", "int", "return", "void", "while"
  };
  static int token_id[] = {
    TOK_ELSE, TOK_FOR, TOK_IF, TOK_INT, TOK_RETURN, TOK_VOID, TOK_WHILE
  };

  // 创建哈希表并插入保留字条目到哈希表中
  const size_t capacity = sizeof keywords / sizeof keywords[0];
  hcreate(capacity);
  for (size_t i = 0; i < capacity; ++i) {
    e.key = keywords[i];
    e.data = (void *) &token_id[i];
    ep = hsearch(e, ENTER);
  }
}

static int is_buffer_end()
{
  return current_scan_ptr == scan_buffer_end;
}

static void new_line()
{
  current_location.line++;
  current_location.column = 0;
}

static char next_char()
{
  current_location.column++;
  return *current_scan_ptr++;
}

/* Look ahead a char in the buffer. Index 0 means the first one,
   1 means the second one. */
static char look_ahead(int index)
{
  return *(current_scan_ptr + index);
}

/* Consume an expected char. */
static void consume(char expected_char)
{
  char c = next_char();
  // printf("Expected '%c', but got '%c'\n", expected_char, c);
  assert(c == expected_char);
}

Token next_token_impl()
{
  char c = next_char();

  /* Skip comment and space. */
  while ((c == '/' && (look_ahead(0) == '*' || look_ahead(0) == '/'))
         || isspace(c)) {
    if (c == '\n')
      new_line();
    else if (c == '/' && look_ahead(0) == '*') {
      consume('*');
      while (!(look_ahead(0) == '*' && look_ahead(1) == '/')) {
        c = next_char();
        if (c == '\n')
           new_line();
        if (is_buffer_end()) {
          token.id = TOK_EOF;
          return token;
        }
      }
      consume('*');
      consume('/');
    }
    else if (c == '/' && look_ahead(0) == '/') {
      consume('/');
      while (look_ahead(0) != '\n') { // 读字符到注释尾
        c = next_char();
        if (is_buffer_end()) {
          token.id = TOK_EOF;
          return token;
        }
      }
      consume('\n');
      new_line();
    }
    c = next_char();
  }

  token.loc = current_location;
  switch (c)
  {
  /* integer-constant -> 0  */
  case '0':
    token.id = TOK_NUMBER;
    token.ival = 0;
    break;
  /* integer-constant -> [1~9] [0~9]*  */
  case '1': case '2': case '3': case '4':
  case '5': case '6': case '7': case '8': case '9':
    token.id = TOK_NUMBER;
    token.ival = c - '0';
    while (isdigit(look_ahead(0))) {
      c = next_char();
      token.ival = token.ival * 10 + (c - '0');
    }
    break;
  /* identifier -> [_a~zA~Z] [_a~zA~Z0~9]*  */
  case '_':
  case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G':
  case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N':
  case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U':
  case 'V': case 'W': case 'X': case 'Y': case 'Z':
  case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g':
  case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n':
  case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u':
  case 'v': case 'w': case 'x': case 'y': case 'z': {
    char *id_start = current_scan_ptr - 1;
    size_t len;
    token.id = TOK_IDENTIFIER;
    while (isalnum(look_ahead(0)) || look_ahead(0) == '_')
      next_char();

    len = current_scan_ptr - id_start;
    token.ptr = (char *)malloc(len + 1);
    token.ptr = memcpy(token.ptr, id_start, len);
    token.ptr[len] = '\0';

    /* keyword */
    ENTRY e;
    ENTRY *ep;
    e.key = token.ptr;
    ep = hsearch(e, FIND);
    if (ep)
      token.id = *(int*)ep->data;
    break;
  }
  /* op_le -> <=  */
  case '<':
    token.id = c;
    if (look_ahead(0) == '=') {
      next_char();
      token.id = TOK_LE;
    }
    break;
  /* FIXME: 实现如下词法分析代码片段, 开始 */
  /* op_ge -> >=  */
  case '>':
    token.id = c;
    if (look_ahead(0) == '=') {
      next_char();
      token.id = TOK_GE;
    }
    break;
  /* op_eq -> ==  */
  case '=':
    token.id = c;
    if (look_ahead(0) == '=') {
      next_char();
      token.id = TOK_EQ;
    }
    break;
  /* op_ne -> !=  */
  case '!':
    token.id = c;
    if (look_ahead(0) == '=') {
      next_char();
      token.id = TOK_NE;
    }
    break;
  /* op_and -> &&  */
  case '&':
    token.id = c;
    if (look_ahead(0) == '&') {
      next_char();
      token.id = TOK_AND;
    }
    break;
  /* op_or -> ||  */
  case '|':
    token.id = c;
    if (look_ahead(0) == '|') {
      next_char();
      token.id = TOK_OR;
    }
    break;
    /* 实现之后删除该语句 */
    // assert(0 && "FIXME");
  /* FIXME: 实现如上词法分析代码片段，结束 */

  /* end of file */
  case '\0':
    if (is_buffer_end()) {
      token.id = TOK_EOF;
      --current_scan_ptr; /* for safety */
    } else
      token.id = c;
    break;
  /* others */
  default:
    token.id = c;
    break;
  }
  return token;
}

Token next_token()
{
  Token token = next_token_impl();
  if (opt_dump_token) {
    dump_token(token);
    printf("\n");
  }
  return token;
}

void dump_token(Token tok)
{
  switch (tok.id) {
  default:
    if (isprint(tok.id))
      printf("PUNCTUATOR: %c", tok.id);
    break;
  case TOK_LE:
    printf("PUNCTUATOR: <="); break;
  case TOK_GE:
    printf("PUNCTUATOR: >="); break;
  case TOK_EQ:
    printf("PUNCTUATOR: =="); break;
  case TOK_NE:
    printf("PUNCTUATOR: !="); break;
  case TOK_AND:
    printf("PUNCTUATOR: &&"); break;
  case TOK_OR:
    printf("PUNCTUATOR: ||"); break;
  case TOK_ELSE:
    printf("KEYWORD: ELSE"); break;
  case TOK_FOR:
    printf("KEYWORD: FOR"); break;
  case TOK_IF:
    printf("KEYWORD: IF"); break;
  case TOK_INT:
    printf("KEYWORD: INT"); break;
  case TOK_RETURN:
    printf("KEYWORD: RETURN"); break;
  case TOK_VOID:
    printf("KEYWORD: VOID"); break;
  case TOK_WHILE:
    printf("KEYWORD: WHILE"); break;
  case TOK_NUMBER:
    printf("NUMBER: %d", tok.ival); break;
  case TOK_IDENTIFIER:
    printf("IDENTIFIER: %s", tok.ptr); break;
  case TOK_EOF:
    printf("EOF"); break;
  }
}

