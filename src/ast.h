#ifndef AST_H
#define AST_H

#include "node.h"
#include "location.h"
#include "map.h"
#include "symbol.h"
#include "type.h"

typedef enum {
  TRANS_UNIT,
  FUNC_DECL,
  VAR_DECL,
  VAR_DECL_GROUP,
  PARAM_DECL,
  IDENT_EXPR,
  CONST_EXPR,
  DECL_REF_EXPR,
  ARRAY_SUBSCRIPT_EXPR,
  CALL_EXPR,
  INIT_LIST_EXPR,
  UNARY_OP,
  BINARY_OP,
  COMPOUND_STMT,
  NULL_STMT,
  IF_STMT,
  WHILE_STMT,
  FOR_STMT,
  RETURN_STMT
} NodeKind;

typedef enum {
  OP_UNKNOWN,
  OP_PLUS,
  OP_MINUS,
  OP_ASSIGN,
  OP_OR,
  OP_AND,
  OP_EQ,
  OP_NE,
  OP_LT,
  OP_GT,
  OP_LE,
  OP_GE,
  OP_ADD,
  OP_SUB,
  OP_MUL,
  OP_DIV,
  OP_REM,
  OP_BAND,         // 新加-按位与
  OP_SLL           // 新加-逻辑左移
} Operator;

typedef struct TransUnit {
  NodeKind kind;
  Location loc;
  NodeList *nl;
  SymTable *symtab;
} TransUnit;

typedef struct FuncDecl {
  NodeKind kind;
  Location loc;
  int num_of_params;
  Type *type;
  Node *name;
  Node *body;
  NodeList *var_list;
  SymTable *symtab;
  Node *params[];
} FuncDecl;

#define FUNC_DECL_NAME(n) n->func_decl.name
#define FUNC_DECL_VAR_LIST(n) n->func_decl.var_list

typedef struct VarDecl {
  NodeKind kind;
  Location loc;
  Node *name;
  Type *type;
  Node *init;
} VarDecl;

#define VAR_DECL_NAME(n) n->var_decl.name
#define VAR_DECL_TYPE(n) n->var_decl.type
#define VAR_DECL_INIT(n) n->var_decl.init

typedef struct VarDeclGroup {
  NodeKind kind;
  Location loc;
  int num_of_vars;
  Node *vars[];
} VarDeclGroup;

typedef struct ParamDecl {
  NodeKind kind;
  Location loc;
  Type *type;
  Node *name;
} ParamDecl;

#define PARAM_DECL_TYPE(n) n->param_decl.type
#define PARAM_DECL_NAME(n) n->param_decl.name

typedef struct IdentExpr {
  NodeKind kind;
  Location loc;
  char *name;
  Symbol *sym;
} IdentExpr;

#define IDENT_EXPR_NAME(n) n->ident_expr.name
#define IDENT_EXPR_LOC(n) n->ident_expr.loc
#define IDENT_EXPR_SYM(n) n->ident_expr.sym

typedef struct ConstExpr {
  NodeKind kind;
  Location loc;
  int value;
} ConstExpr;

#define CONST_EXPR_VALUE(n) n->const_expr.value

typedef struct DeclRefExpr {
  NodeKind kind;
  Location loc;
  char *name;
  Symbol *sym;
} DeclRefExpr;

#define DECL_REF_EXPR_NAME(n) n->decl_ref_expr.name
#define DECL_REF_EXPR_SYM(n) n->decl_ref_expr.sym
#define DECL_REF_EXPR_LOC(n) n->decl_ref_expr.loc

typedef struct ArraySubscriptExpr {
  NodeKind kind;
  Location loc;
  Node *declref;
  int num_of_subscripts;
  Node *subscripts[];
} ArraySubscriptExpr;

typedef struct UnaryOp {
  NodeKind kind;
  Location loc;
  Operator op;
  Node *operand;
} UnaryOp;

typedef struct BinaryOp {
  NodeKind kind;
  Location loc;
  Operator op;
  Node *left;
  Node *right;
} BinaryOp;

typedef struct CallExpr {
  NodeKind kind;
  Location loc;
  Node *declref;
  int num_of_args;
  Node *args[];
} CallExpr;

typedef struct InitListExpr {
  NodeKind kind;
  Location loc;
  int num_of_inits;
  Node *inits[];
} InitListExpr;

typedef struct CompoundStmt {
  NodeKind kind;
  Location loc;
  int num_of_items;
  /* A block item is a declaration or statement. */
  Node *items[];
} CompoundStmt;

typedef struct IfStmt {
  NodeKind kind;
  Location loc;
  Node *cond;
  Node *then_stmt;
  Node *else_stmt;
} IfStmt;

typedef struct WhileStmt {
  NodeKind kind;
  Location loc;
  Node *cond;
  Node *body;
} WhileStmt;

typedef struct ForStmt {
  NodeKind kind;
  Location loc;
  Node *init; /* initialization */
  Node *cond; /* condition expression */
  Node *incr; /* increment expression */
  Node *body; /* loop body */
} ForStmt;

typedef struct ReturnStmt {
  NodeKind kind;
  Location loc;
  Node *expr;
} ReturnStmt;

union Node {
  NodeKind kind;
  struct TransUnit trans_unit;
  struct FuncDecl func_decl;
  struct VarDecl var_decl;
  struct VarDeclGroup var_decl_group;
  struct ParamDecl param_decl;
  struct IdentExpr ident_expr;
  struct ConstExpr const_expr;
  struct DeclRefExpr decl_ref_expr;
  struct ArraySubscriptExpr array_subscript_expr;
  struct UnaryOp unary_op;
  struct BinaryOp binary_op;
  struct CallExpr call_expr;
  struct InitListExpr init_list_expr;
  struct CompoundStmt compound_stmt;
  struct IfStmt if_stmt;
  struct WhileStmt while_stmt;
  struct ForStmt for_stmt;
  struct ReturnStmt return_stmt;
};

struct NodeList {
  NodeKind kind;
  Node *node;
  struct NodeList *next;
};

Node *create_trans_unit(NodeList *node_list, Location loc);
Node *create_func_decl(Type *type, Node *name, NodeList *param_list,
                       Node *body);
Node *create_var_decl(Node *name, Type *type, Node *init);
Node *create_var_decl_group(NodeList *var_list);
Node *create_param_decl(Node *name, Type *type);
Node *create_ident_expr(char *name, Location loc);
Node *create_const_expr(int value, Location loc);
Node *create_decl_ref_expr(char *name, Location loc, Symbol *sym);
Node *create_array_subscript_expr(Location loc, Node *declref,
                                  NodeList *subscripts_list);
Node *create_unary_op(Operator op, Node *expr);
Node *create_binary_op(Operator op, Node *left, Node *right);
Node *create_call_expr(Node *declref, NodeList *arg_list);
Node *create_init_list_expr(NodeList *init_list);
Node *create_compound_stmt(NodeList *item_list);
Node *create_null_stmt();
Node *create_if_stmt(Node *cond, Node *then_stmt, Node *else_stmt);
Node *create_while_stmt(Node *cond, Node *body);
Node *create_for_stmt(Node *init, Node *cond, Node *incr, Node *body);
Node *create_return_stmt(Node *expr);

NodeList *create_node_list(Node *node);
int get_length_of_node_list(NodeList *nl);
void copy_node_list_to_array(Node *na[], NodeList *nl);

void dump_node(Node *node);

#endif
