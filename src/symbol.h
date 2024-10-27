#ifndef PANDA_SYM_H
#define PANDA_SYM_H

#include "location.h"
#include "type.h"
#include <stdbool.h>

struct Symbol;
typedef struct Symbol Symbol;

typedef enum {
  ADDR_SYM,
  ADDR_SYM_LO,
  ADDR_SYM_HI,
  ADDR_OFFSET,
} AddrKind;

typedef struct {
  AddrKind kind;
  union {
    Symbol *sym;
    int offset;
  } value;
} Address;

typedef enum {
  SA_GLOBAL = 0x1,
  SA_LOCAL = 0x2,
  SA_VAR = 0x4,
  SA_FUNC = 0x8
} SymAttr;

struct Symbol {
  char *name;
  Location loc;
  Type *type;
  SymAttr attr;
  Address addr;
  int size;
  int reg;
  Symbol *init;
};

typedef struct SymTable {
  StrMap *strmap;
  struct SymTable *up;
} SymTable;

Symbol *find_symbol(SymTable *symtab, char *name, bool upward);
Symbol *insert_symbol(SymTable *symtab, char *name, Location loc, Type *type);
SymTable *new_symbol_table(SymTable *up);

#endif /* PANDA_SYM_H */
