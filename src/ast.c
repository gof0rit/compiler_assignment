#include "ast.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

int get_length_of_node_list(NodeList *nl)
{
  int i = 0;
  while (nl) {
    ++i;
    nl = nl->next;
  }
  return i;
}

/* User is responsible for safety. */
void copy_node_list_to_array(Node *na[], NodeList *nl)
{
  int i = 0;
  while (nl) {
    na[i++] = nl->node;
    nl = nl->next;
  }
}

Node *create_trans_unit(NodeList *node_list, Location loc)
{
  TransUnit *node = (TransUnit *)malloc(sizeof(union Node));
  node->kind = TRANS_UNIT;
  node->loc = loc;
  node->nl = node_list;
  return (Node *)node;
}

Node *create_func_decl(Type *type, Node *name, NodeList *param_list, Node *body)
{
  int n = get_length_of_node_list(param_list);
  FuncDecl *node = (FuncDecl *)malloc(sizeof(union Node) + n * sizeof(Node *));
  node->kind = FUNC_DECL;
  node->num_of_params = n;
  node->type = type;
  node->name = name;
  node->body = body;
  copy_node_list_to_array(node->params, param_list);
  return (Node *)node;
}

Node *create_var_decl(Node *name, Type *type, Node *init)
{
  VarDecl *node = (VarDecl *)malloc(sizeof(union Node));
  node->kind = VAR_DECL;
  node->name = name;
  node->type = type;
  node->init = init;
  return (Node *)node;
}

Node *create_var_decl_group(NodeList *var_list)
{
  int n = get_length_of_node_list(var_list);
  VarDeclGroup *node =
      (VarDeclGroup *)malloc(sizeof(union Node) + n * sizeof(Node *));
  node->kind = VAR_DECL_GROUP;
  node->num_of_vars = n;
  copy_node_list_to_array(node->vars, var_list);
  return (Node *)node;
}

Node *create_param_decl(Node *name, Type *type)
{
  ParamDecl *node = (ParamDecl *)malloc(sizeof(union Node));
  node->kind = PARAM_DECL;
  node->type = type;
  node->name = name;
  return (Node *)node;
}

Node *create_ident_expr(char *name, Location loc)
{
  IdentExpr *node = (IdentExpr *)malloc(sizeof(union Node));
  node->kind = IDENT_EXPR;
  node->name = name;
  node->loc = loc;
  return (Node *)node;
}

Node *create_const_expr(int value, Location loc)
{
  ConstExpr *node = (ConstExpr *)malloc(sizeof(union Node));
  node->kind = CONST_EXPR;
  node->value = value;
  node->loc = loc;
  return (Node *)node;
}

Node *create_decl_ref_expr(char *name, Location loc, Symbol *sym)
{
  DeclRefExpr *node = (DeclRefExpr *)malloc(sizeof(union Node));
  node->kind = DECL_REF_EXPR;
  node->name = name;
  node->loc = loc;
  node->sym = sym;
  return (Node *)node;
}

Node *create_array_subscript_expr(Location loc, Node *declref,
                                  NodeList *subscripts_list)
{
  int n = get_length_of_node_list(subscripts_list);
  ArraySubscriptExpr *node = (ArraySubscriptExpr *)malloc(sizeof(union Node));
  node->kind = ARRAY_SUBSCRIPT_EXPR;
  node->loc = loc;
  node->declref = declref;
  node->num_of_subscripts = n;
  copy_node_list_to_array(node->subscripts, subscripts_list);
  return (Node *)node;
}

Node *create_unary_op(Operator op, Node *expr)
{
  UnaryOp *node = (UnaryOp *)malloc(sizeof(union Node));
  node->kind = UNARY_OP;
  node->op = op;
  node->operand = expr;
  return (Node *)node;
}

Node *create_binary_op(Operator op, Node *left, Node *right)
{
  BinaryOp *node = (BinaryOp *)malloc(sizeof(union Node));
  node->kind = BINARY_OP;
  node->op = op;
  node->left = left;
  node->right = right;
  return (Node *)node;
}

Node *create_call_expr(Node *declref, NodeList *arg_list)
{
  int n = get_length_of_node_list(arg_list);
  CallExpr *node = (CallExpr *)malloc(sizeof(union Node) + n * sizeof(Node *));
  node->kind = CALL_EXPR;
  node->declref = declref;
  node->num_of_args = n;
  copy_node_list_to_array(node->args, arg_list);
  return (Node *)node;
}

Node *create_init_list_expr(NodeList *init_list)
{
  int n = get_length_of_node_list(init_list);
  InitListExpr *node =
      (InitListExpr *)malloc(sizeof(union Node) + n * sizeof(Node *));
  node->kind = INIT_LIST_EXPR;
  node->num_of_inits = n;
  copy_node_list_to_array(node->inits, init_list);
  return (Node *)node;
}

Node *create_compound_stmt(NodeList *item_list)
{
  int n = get_length_of_node_list(item_list);

  CompoundStmt *node =
     (CompoundStmt *)malloc(sizeof(union Node) + n * sizeof(Node *));
  node->kind = COMPOUND_STMT;
  node->num_of_items = n;

  copy_node_list_to_array(node->items, item_list);
  return (Node *)node;
}

Node *create_null_stmt()
{
  Node *node = malloc(sizeof(union Node));
  node->kind = NULL_STMT;
  return node;
}

Node *create_if_stmt(Node *cond, Node *then_stmt, Node *else_stmt)
{
  IfStmt *node = (IfStmt *)malloc(sizeof(union Node));
  node->kind = IF_STMT;
  node->cond = cond;
  node->then_stmt = then_stmt;
  node->else_stmt = else_stmt;
  return (Node *)node;
}

Node *create_while_stmt(Node *cond, Node *body)
{
  WhileStmt *node = (WhileStmt *)malloc(sizeof(union Node));
  node->kind = WHILE_STMT;
  node->cond = cond;
  node->body = body;
  return (Node *)node;
}

Node *create_for_stmt(Node *init, Node *cond, Node *incr, Node *body)
{
  ForStmt *node = (ForStmt *)malloc(sizeof(union Node));
  node->kind = FOR_STMT;
  node->init = init;
  node->cond = cond;
  node->incr = incr;
  node->body = body;
  return (Node *)node;
}

Node *create_return_stmt(Node *expr)
{
  ReturnStmt *node = (ReturnStmt *)malloc(sizeof(union Node));
  node->kind = RETURN_STMT;
  node->expr = expr;
  return (Node *)node;
}

NodeList *create_node_list(Node *node)
{
  NodeList *nl = (NodeList *)malloc(sizeof(NodeList));
  nl->node = node;
  nl->next = NULL;
  return nl;
}

void dump_operator(Operator op)
{
  switch (op) {
  case OP_UNKNOWN:
    printf("OP_UNKNOWN"); break;
  case OP_PLUS:
    printf("OP_PLUS"); break;
  case OP_MINUS:
    printf("OP_MINUS"); break;
  case OP_ASSIGN:
    printf("OP_ASSIGN"); break;
  case OP_OR:
    printf("OP_OR"); break;
  case OP_AND:
    printf("OP_AND"); break;
  case OP_EQ:
    printf("OP_EQ"); break;
  case OP_NE:
    printf("OP_NE"); break;
  case OP_LT:
    printf("OP_LT"); break;
  case OP_GT:
    printf("OP_GT"); break;
  case OP_LE:
    printf("OP_LE"); break;
  case OP_GE:
    printf("OP_GE"); break;
  case OP_ADD:
    printf("OP_ADD"); break;
  case OP_SUB:
    printf("OP_SUB"); break;
  case OP_MUL:
    printf("OP_MUL"); break;
  case OP_DIV:
    printf("OP_DIV"); break;
  case OP_REM:
    printf("OP_REM"); break;
  case OP_BAND:
    printf("OP_BAND");break;
  }
}

void dump_node_internal(Node *node, int level, int flag, bool last)
{
  if (!node)
    return;

  if (level > 0) {
    for (int i = 0; i < level - 1; ++i) {
      if (flag >> i & 0x1)
        printf("| ");
      else
        printf("  ");
    }
    if (last) {
      printf("`-");
      flag &= ~(1 << (level - 1));
    } else {
      printf("|-");
      flag |= 1 << (level - 1);
    }
  }

  //printf("%p: ", node);

  switch (node->kind) {
  case TRANS_UNIT:
    printf("TRANS_UNIT %s\n", node->trans_unit.loc.file);
    for (NodeList *nl = node->trans_unit.nl; nl; nl = nl->next)
      dump_node_internal(nl->node, level + 1, flag, nl->next == NULL);
    break;
  case FUNC_DECL:
    printf("FUNC_DECL: ");
    dump_type(node->func_decl.type);
    printf("\n");
    dump_node_internal(node->func_decl.name, level + 1, flag, false);
    for (int i = 0; i < node->func_decl.num_of_params; ++i)
      dump_node_internal(node->func_decl.params[i], level + 1, flag, false);
    dump_node_internal(node->func_decl.body, level + 1, flag, true);
    break;
  case VAR_DECL:
    printf("VAR_DECL: ");
    dump_type(node->var_decl.type);
    printf("\n");
    dump_node_internal(node->var_decl.name, level + 1, flag, false);
    dump_node_internal(node->var_decl.init, level + 1, flag, true);
    break;
  case VAR_DECL_GROUP:
    printf("VAR_DECL_GROUP\n");
    for (int i = 0; i < node->var_decl_group.num_of_vars; ++i)
      dump_node_internal(node->var_decl_group.vars[i], level + 1, flag,
                         i == node->var_decl_group.num_of_vars - 1);
    break;
  case PARAM_DECL:
    printf("PARAM_DECL: ");
    dump_type(node->param_decl.type);
    printf("\n");
    dump_node_internal(node->param_decl.name, level + 1, flag, true);
    break;
  case IDENT_EXPR:
    printf("IDENT_EXPR %s\n", node->ident_expr.name);
    break;
  case CONST_EXPR:
    printf("CONST_EXPR %d\n", node->const_expr.value);
    break;
  case DECL_REF_EXPR:
    printf("DECL_REF_EXPR %s\n", node->decl_ref_expr.name);
    break;
  case ARRAY_SUBSCRIPT_EXPR:
    printf("ARRAY_SUBSCRIPT_EXPR\n");
    dump_node_internal(node->array_subscript_expr.declref, level + 1, flag, true);
    for (int i = 0; i < node->array_subscript_expr.num_of_subscripts; ++i)
      dump_node_internal(node->array_subscript_expr.subscripts[i], level + 1, flag, true);
    break;
  case UNARY_OP:
    printf("UNARY_OP ");
    dump_operator(node->unary_op.op);
    printf("\n");
    dump_node_internal(node->unary_op.operand, level + 1, flag, true);
    break;
  case BINARY_OP:
    printf("BINARY_OP ");
    dump_operator(node->binary_op.op);
    printf("\n");
    dump_node_internal(node->binary_op.left, level + 1, flag, false);
    dump_node_internal(node->binary_op.right, level + 1, flag, true);
    break;
  case CALL_EXPR:
    printf("CALL_EXPR\n");
    dump_node_internal(node->call_expr.declref, level + 1, flag, true);
    for (int i = 0; i < node->call_expr.num_of_args; ++i)
      dump_node_internal(node->call_expr.args[i], level + 1, flag, true);
    break;
  case INIT_LIST_EXPR:
    printf("INIT_LIST_EXPR\n");
    for (int i = 0; i < node->init_list_expr.num_of_inits; ++i)
      dump_node_internal(node->init_list_expr.inits[i], level + 1, flag,
                         i == node->init_list_expr.num_of_inits - 1);
    break;
  case COMPOUND_STMT:
    printf("COMPOUND_STMT\n");
    for (int i = 0; i < node->compound_stmt.num_of_items; ++i)
      dump_node_internal(node->compound_stmt.items[i], level + 1, flag,
                         i == node->compound_stmt.num_of_items - 1);
    break;
  case NULL_STMT:
    printf("NULL_STMT\n");
    break;
  case IF_STMT:
    printf("IF_STMT\n");
    dump_node_internal(node->if_stmt.cond, level + 1, flag, false);
    dump_node_internal(node->if_stmt.then_stmt, level + 1, flag, false);
    dump_node_internal(node->if_stmt.else_stmt, level + 1, flag, true);
    break;
  case WHILE_STMT:
    printf("WHILE_STMT\n");
    dump_node_internal(node->while_stmt.cond, level + 1, flag, false);
    dump_node_internal(node->while_stmt.body, level + 1, flag, true);
    break;
  case FOR_STMT:
    printf("FOR_STMT\n");
    dump_node_internal(node->for_stmt.init, level + 1, flag, false);
    dump_node_internal(node->for_stmt.cond, level + 1, flag, false);
    dump_node_internal(node->for_stmt.incr, level + 1, flag, false);
    dump_node_internal(node->for_stmt.body, level + 1, flag, true);
    break;
  case RETURN_STMT:
    printf("RETURN_STMT\n");
    dump_node_internal(node->return_stmt.expr, level + 1, flag, true);
    break;
  }
}

void dump_node(Node *node)
{
  dump_node_internal(node, 0, 0, true);
}

