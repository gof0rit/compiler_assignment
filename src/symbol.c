#include "ast.h"
#include "location.h"
#include "symbol.h"
#include <assert.h>
#include <stdlib.h>

Symbol *find_symbol(SymTable *symtab, char *name, bool upward)
{
  if (!symtab)
    return NULL;

  Symbol *sym = strmap_find(symtab->strmap, name);
  if (sym)
    return sym;
  
  if (upward)
    return find_symbol(symtab->up, name, upward);
  return NULL;
}

Symbol *insert_symbol(SymTable *symtab, char *name, Location loc, Type *type)
{
  assert(symtab);
  Symbol *sym = malloc(sizeof(Symbol));

  sym->name = name;
  sym->type = type;
  sym->loc = loc;
  sym->reg = -1;
  sym->init = NULL;
  strmap_insert(symtab->strmap, name, sym);
  return sym;
}

SymTable *new_symbol_table(SymTable *up)
{
  SymTable *symtab = malloc(sizeof(SymTable));
  symtab->strmap = strmap_new();
  symtab->up = up;
  return symtab;
}

