#ifndef PANDA_SEMA_H
#define PANDA_SEMA_H

Node *build_param_decl(SymTable *symtab, Node *name, Type *type);
Node *build_var_decl(SymTable *symtab, Node *name, Type *type, Node *init);
Node *build_decl_ref_expr(SymTable *symtab, char *name, Location loc);
Node *build_func_decl(SymTable *symtab, Type *type, Node *name,
                      NodeList *param_list, Node *body);
Node *build_call_expr(Node *declref, NodeList *arg_list);
Node *build_array_subscript_expr(Node *declref, NodeList *subscript_list);

#endif
