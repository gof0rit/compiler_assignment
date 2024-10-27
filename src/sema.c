#include "ast.h"
#include "diag.h"
#include "symbol.h"
#include <stdio.h>

Node *build_param_decl(SymTable *symtab, Node *name, Type *type)
{
  Symbol *sym = find_symbol(symtab, IDENT_EXPR_NAME(name), false);
  if (sym) {
    error("%s:%d:%d: %s is redefined", sym->loc.file, sym->loc.line,
          sym->loc.column, sym->name);
  }
  sym = insert_symbol(symtab, IDENT_EXPR_NAME(name), IDENT_EXPR_LOC(name),
                      type);
  sym->attr = SA_LOCAL | SA_VAR;
  IDENT_EXPR_SYM(name) = sym;
  return create_param_decl(name, type);
}

Node *build_var_decl(SymTable *symtab, Node *name, Type *type, Node *init)
{
  // 查询符号表中是否存在该变量，若存在则是重复定义错误，若不存在则插入符号表
  Symbol *sym = find_symbol(symtab, IDENT_EXPR_NAME(name), false);
  if (sym) {
    error("%s:%d:%d: %s is redefined", sym->loc.file, sym->loc.line,
          sym->loc.column, sym->name);
  }
  sym = insert_symbol(symtab, IDENT_EXPR_NAME(name), IDENT_EXPR_LOC(name),
                      type);
  sym->attr = symtab->up ? SA_LOCAL | SA_VAR : SA_GLOBAL | SA_VAR;
  IDENT_EXPR_SYM(name) = sym;
  return create_var_decl(name, type, init);
}

Node *build_decl_ref_expr(SymTable *symtab, char *name, Location loc)
{
  Symbol *sym = find_symbol(symtab, name, true);
  if (!sym) {
    error("%s:%d:%d: %s is undefined", loc.file, loc.line, loc.column, name);
  }
  return create_decl_ref_expr(name, loc, sym);
}

Node *build_func_decl(SymTable *symtab, Type *type, Node *name,
                      NodeList *param_list, Node *body)
{
  Symbol *sym = find_symbol(symtab, IDENT_EXPR_NAME(name), false);
  if (sym) {
    error("%s:%d:%d: %s is redefined", sym->loc.file, sym->loc.line,
          sym->loc.column, sym->name);
  }
  sym = insert_symbol(symtab, IDENT_EXPR_NAME(name), IDENT_EXPR_LOC(name),
                      type);
  sym->attr = SA_GLOBAL | SA_FUNC;
  IDENT_EXPR_SYM(name) = sym;
  return create_func_decl(type, name, param_list, body);
}

Node *build_call_expr(Node *declref, NodeList *arg_list)
{
  Symbol *sym = DECL_REF_EXPR_SYM(declref);
  if (sym) {
    if (sym->attr & SA_FUNC) {
      /* TODO: check arguments and return value. */
    }
    else {
      error("%s:%d:%d: %s is not a function", DECL_REF_EXPR_LOC(declref).file,
            DECL_REF_EXPR_LOC(declref).line, DECL_REF_EXPR_LOC(declref).column,
            DECL_REF_EXPR_NAME(declref));
      printf("%s:%d:%d: %s is previous defined here\n", sym->loc.file,
             sym->loc.line, sym->loc.column, sym->name);
    }
  }
  return create_call_expr(declref, arg_list);
}

Node *build_array_subscript_expr(Node *declref, NodeList *subscript_list)
{
  Symbol *sym = DECL_REF_EXPR_SYM(declref);
  if (sym) {
    /* TODO: check array subscripts. */
  }
  return create_array_subscript_expr(DECL_REF_EXPR_LOC(declref), declref,
                                     subscript_list);
}

