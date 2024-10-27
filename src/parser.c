#include "ast.h"
#include "diag.h"
#include "location.h"
#include "scanner.h"
#include "sema.h"
#include "symbol.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define DEBUG 0

typedef enum
{
  Prec_Unknown,        // Not binary operator.
  Prec_Comma,          // ,
  Prec_Assignment,     // =
  Prec_LogicalOr,      // ||
  Prec_LogicalAnd,     // &&
  Prec_BitAnd,         // & 新加-按位与
  Prec_Equality,       // ==, !=
  Prec_Relational,     // >=, <=, >, <
  Prec_Shift,          // << 新加-逻辑左移
  Prec_Additive,       // -, +
  Prec_Multiplicative, // *, /
} PrecLevel;

typedef struct Declarator
{
  Type *type;
  Node *name;
  /* parameter list or array dimension list */
  NodeList *list;
} Declarator;

typedef struct DeclaratorList
{
  Declarator *decltor;
  struct DeclaratorList *next;
} DeclaratorList;

static SymTable *current_symtab;
static NodeList *local_var_list_head, *local_var_list_tail;

/* forward declaration */
Node *parse_assignment_expression();
Node *parse_initializer();
Node *parse_expression();
Node *parse_statement();
Node *parse_rhs_of_binary_expression(Node *lhs, PrecLevel prec);
NodeList *parse_initializer_list();
NodeList *parse_parameter_list();

/* Consume an expected token. */
static void consume(int ID)
{
  if (token.id != ID)
  {
    error("%s:%d:%d: unexpected token ", token.loc.file, token.loc.line,
          token.loc.column);
    dump_token(token);
    printf("\n");
    exit(-1);
  }
  next_token();
}

/*
type-specifier:
  void
  int
*/
Type *parse_type_specifier()
{
#if DEBUG
  printf("%s\n", __func__);
#endif

  if (token.id == TOK_VOID)
  {
    next_token();
    return void_ty;
  }
  else if (token.id == TOK_INT)
  {
    next_token();
    return int_ty;
  }
  printf("error: expected type specifier!\n");
  return void_ty;
}

/*
declarator:
  identifier
  declarator [ assignment-expression_opt ]
  declarator ( parameter_list_opt )
*/
void parse_declarator(Declarator *decltor)
{
#if DEBUG
  printf("%s\n", __func__);
#endif

  decltor->name = create_ident_expr(token.ptr, token.loc);
  consume(TOK_IDENTIFIER);
  /* function */
  if (token.id == '(')
  {
    consume('(');
    decltor->list = parse_parameter_list();
    decltor->type = create_func_type(decltor->type, decltor->list);
    consume(')');
    return;
  }
  /* declaration */
  if (token.id == '[')
  {
    NodeList **tail;
    tail = &decltor->list;
    while (token.id == '[')
    {
      *tail = create_node_list(NULL);
      consume('[');
      if (token.id != ']')
      {
        (*tail)->node = parse_assignment_expression();
      }
      consume(']');
      tail = &((*tail)->next);
    }
    *tail = NULL;
    decltor->type = create_array_type(decltor->type, decltor->list);
  }
}

/*
init-declarator:
  declarator
  declarator = initializer
*/
Node *parse_init_declarator(Declarator *decltor)
{
#if DEBUG
  printf("%s\n", __func__);
#endif

  parse_declarator(decltor);
  if (token.id == '=')
  {
    consume('=');
    return parse_initializer();
  }
  return NULL;
}

/*
init-declarator-list:
  init-declarator
  init-declarator-list , init-declarator
*/
NodeList *parse_init_declarator_list(Type *type)
{
#if DEBUG
  printf("%s\n", __func__);
#endif

  Declarator decltor;
  decltor.type = type;
  decltor.name = NULL;
  decltor.list = NULL;

  NodeList *head, *tail;
  head = create_node_list(NULL);
  tail = head;

  Node *init = parse_init_declarator(&decltor);
  tail->node = build_var_decl(current_symtab, decltor.name, decltor.type, init);
  while (token.id == ',')
  {
    consume(',');
    decltor.type = type;
    decltor.name = NULL;
    decltor.list = NULL;
    tail->next = create_node_list(NULL);
    tail = tail->next;
    init = parse_init_declarator(&decltor);
    tail->node = build_var_decl(current_symtab, decltor.name, decltor.type, init);
  }
  tail->next = NULL;

  return head;
}

/*
declaration:
  type-specifier init-declarator-list ;
*/
Node *parse_declaration()
{
#if DEBUG
  printf("%s\n", __func__);
#endif

  Type *type = parse_type_specifier();
  NodeList *nl = parse_init_declarator_list(type);
  consume(';');
  if (!local_var_list_head)
  {
    local_var_list_head = local_var_list_tail = nl;
  }
  else
  {
    while (local_var_list_tail->next)
      local_var_list_tail = local_var_list_tail->next;
    if (local_var_list_tail != nl)
      local_var_list_tail->next = nl;
  }

  return create_var_decl_group(nl);
}

/*
parameter-declaration:
  type-specifier declarator_opt
*/
Node *parse_parameter_declaration()
{
#if DEBUG
  printf("%s\n", __func__);
#endif

  Type *type = parse_type_specifier();
  Declarator decltor;
  decltor.type = type;
  decltor.name = NULL;
  if (token.id != ',' || token.id != ')')
  {
    parse_declarator(&decltor);
  }
  return build_param_decl(current_symtab, decltor.name, decltor.type);
}

/*
parameter-list:
  parameter-declaration
  parameter-list , parameter-declaration
*/
NodeList *parse_parameter_list()
{
#if DEBUG
  printf("%s\n", __func__);
#endif

  /* Create symbol table. */
  current_symtab = new_symbol_table(current_symtab);

  if (token.id == ')')
    return NULL;

  Node *node = parse_parameter_declaration();
  NodeList *head = create_node_list(node);
  NodeList *tail = head;
  while (token.id == ',')
  {
    consume(',');
    node = parse_parameter_declaration();
    tail->next = create_node_list(node);
    tail = tail->next;
  }

  return head;
}

/*
initializer:
  assignment-expression
  { initializer-list }
*/
Node *parse_initializer()
{
#if DEBUG
  printf("%s\n", __func__);
#endif

  if (token.id == '{')
  {
    consume('{');
    NodeList *nl = parse_initializer_list();
    consume('}');
    return create_init_list_expr(nl);
  }

  return parse_assignment_expression();
}

/*
initializer-list:
  initializer
  initializer-list , initializer
*/
NodeList *parse_initializer_list()
{
#if DEBUG
  printf("%s\n", __func__);
#endif

  NodeList *head, *tail;
  tail = head = create_node_list(NULL);
  tail->node = parse_initializer();
  while (token.id == ',')
  {
    consume(',');
    if (token.id != ')')
    {
      tail->next = create_node_list(NULL);
      tail = tail->next;
      tail->node = parse_initializer();
    }
  }
  tail->next = NULL;
  return head;
}

PrecLevel get_binop_predence_level(int token_id)
{
  switch (token_id)
  {
  case ',':
    return Prec_Comma;
  case '=':
    return Prec_Assignment;
  case TOK_OR:
    return Prec_LogicalOr;
  case TOK_AND:
    return Prec_LogicalAnd;
  case '&': // 新加：按位与
    return Prec_BitAnd;
  case TOK_EQ:
  case TOK_NE:
    return Prec_Equality;
  case '<':
  case '>':
  case TOK_LE:
  case TOK_GE:
    return Prec_Relational;
  case TOK_SLL:
    return Prec_Shift;
  case '+':
  case '-':
    return Prec_Additive;
  case '*':
  case '/':
  case '%':
    return Prec_Multiplicative;
  default:
    return Prec_Unknown;
  }
}

Operator get_binop(int token_id)
{
  switch (token_id)
  {
  case TOK_OR:
    return OP_OR;
  case TOK_AND:
    return OP_AND;
  case TOK_EQ:
    return OP_EQ;
  case TOK_NE:
    return OP_NE;
  case '&':
    return OP_BAND;
  case '<':
    return OP_LT;
  case '>':
    return OP_GT;
  case TOK_LE:
    return OP_LE;
  case TOK_GE: 
    return OP_GE;
  case TOK_SLL: 
    return OP_SLL;
  case '+':
    return OP_ADD;
  case '-':
    return OP_SUB;
  case '*':
    return OP_MUL;
  case '/':
    return OP_DIV;
  case '%':
    return OP_REM;
  case '=':
    return OP_ASSIGN;
  default:
    return OP_UNKNOWN;
  }
}
/*
argument-expression-list:
  assignment-expression
  argument-expression-list , assignment-expression
*/

NodeList *parse_argument_expression_list()
{
  NodeList *head, *tail;
  tail = head = create_node_list(NULL);
  tail->node = parse_assignment_expression();
  while (token.id == ',')
  {
    tail->next = create_node_list(NULL);
    tail = tail->next;
    tail->node = parse_assignment_expression();
  }
  tail->next = NULL;
  return head;
}

/*
postfix-expression:
  primary-expression
  postfix-expression [ expression ]
  postfix-expression ( argument-expression-list_opt )

primary-expression:
  identifier
  constant
  ( expression )
*/
Node *parse_postfix_expression()
{
  /* primary expression */
  Node *node = NULL;
  if (token.id == TOK_IDENTIFIER)
  {
    node = build_decl_ref_expr(current_symtab, token.ptr, token.loc);
    consume(TOK_IDENTIFIER);
  }
  else if (token.id == TOK_NUMBER)
  {
    node = create_const_expr(token.ival, token.loc);
    consume(TOK_NUMBER);
  }
  else if (token.id == '(')
  {
    consume('(');
    node = parse_expression();
    consume(')');
  }

  /* postfix expression */
  if (token.id == '[')
  {
    NodeList *head, *tail;
    tail = head = create_node_list(NULL);
    while (token.id == '[')
    {
      consume('[');
      tail->node = parse_expression();
      tail->next = create_node_list(NULL);
      tail = tail->next;
      consume(']');
    }
    return build_array_subscript_expr(node, head);
  }
  if (token.id == '(')
  {
    consume('(');
    NodeList *args = NULL;
    if (token.id != ')')
      args = parse_argument_expression_list();
    consume(')');
    return build_call_expr(node, args);
  }
  return node;
}

/*
unary-expression:
  postfix-expression
  unary-operator unary-expression

unary-operator: one of
  +	-
*/
Node *parse_unary_expression()
{
#if DEBUG
  printf("%s\n", __func__);
#endif

  Node *expr;
  switch (token.id)
  {
  case '+':
    /* unary operator */
    next_token();
    expr = parse_postfix_expression();
    return create_unary_op(OP_PLUS, expr);
  case '-':
    /* unary operator */
    next_token();
    expr = parse_postfix_expression();
    return create_unary_op(OP_MINUS, expr);
  default:
    return parse_postfix_expression();
  }
}

/*
conditional-expression:
  logical-OR-expression

logical-OR-expression:
  logical-AND-expression
  logical-OR-expression || logical-AND-expression

logical-AND-expression:
  equality-expression
  logical-AND-expression && equality-expression

equality-expression:
  relational-expression
  equality-expression == relational-expression
  equality-expression != relational-expression

relational-expression:
  additive-expression
  relational-expression < additive-expression
  relational-expression > additive-expression
  relational-expression <= additive-expression
  relational-expression >= additive-expression

additive-expression:
  multiplicative-expression
  additive-expression + multiplicative-expression
  additive-expression - multiplicative-expression

multiplicative-expression:
  unary-expression
  multiplicative-expression * unary-expression
  multiplicative-expression / unary-expression
*/
Node *parse_rhs_of_binary_expression(Node *lhs, PrecLevel prec)
{
#if DEBUG
  printf("%s\n", __func__);
#endif

  Node *result = lhs;
  while (1)
  {
    PrecLevel next_prec = get_binop_predence_level(token.id);
    if (next_prec < prec)
      break;
    Operator op = get_binop(token.id);
    next_token();
    Node *rhs = parse_unary_expression();
    rhs = parse_rhs_of_binary_expression(rhs, next_prec + 1);
    result = create_binary_op(op, result, rhs);
  }
  return result;
}

/*
assignment-expression:
        conditional-expression
  unary-expression = assignment-expression
*/
Node *parse_assignment_expression()
{
#if DEBUG
  printf("%s\n", __func__);
#endif

  Node *lhs = parse_unary_expression();
  return parse_rhs_of_binary_expression(lhs, Prec_Assignment);
}

/*
expression:
  assignment-expression
  expression , assignment-expression
*/
Node *parse_expression()
{
#if DEBUG
  printf("%s\n", __func__);
#endif

  Node *lhs = parse_assignment_expression();
  return parse_rhs_of_binary_expression(lhs, Prec_Comma);
}

/*
expression-statement:
  expression_opt ;
*/
Node *parse_expression_statement()
{
#if DEBUG
  printf("%s\n", __func__);
#endif

  if (token.id != ';')
  {
    return parse_expression();
  }
  consume(';');
  // return create_expr_stmt(node);
  return create_null_stmt();
}

/*
selection-statement:
  if ( expression ) statement
  if ( expression ) statement else statement
*/
Node *parse_if_statement()
{
#if DEBUG
  printf("%s\n", __func__);
#endif

  consume(TOK_IF);
  consume('(');
  Node *cond = parse_expression();
  consume(')');
  Node *then_stmt = parse_statement();
  Node *else_stmt = NULL;
  if (token.id == TOK_ELSE)
  {
    consume(TOK_ELSE);
    else_stmt = parse_statement();
  }
  return create_if_stmt(cond, then_stmt, else_stmt);
}

/*
iteration-statement:
  while ( expression ) statement
  for ( expression_opt ; expression_opt ; expression_opt ) statement
*/
Node *parse_while_statement()
{
#if DEBUG
  printf("%s\n", __func__);
#endif

  consume(TOK_WHILE);
  consume('(');
  Node *cond = parse_expression();
  consume(')');
  Node *body = parse_statement();
  return create_while_stmt(cond, body);
}

Node *parse_for_statement()
{
#if DEBUG
  printf("%s\n", __func__);
#endif

  /* FIXME: 实现如下语法分析代码片段, 开始 */
  Node *init = NULL, *cond = NULL, *incr = NULL, *body = NULL;

  consume(TOK_FOR);
  consume('(');
  init = parse_expression();
  consume(';');
  cond = parse_expression();
  consume(';');
  incr = parse_expression();
  consume(')');
  body = parse_statement();

  /* 实现之后删除该语句 */
  // assert(0 && "FIXME");

  return create_for_stmt(init, cond, incr, body);
  /* FIXME: 实现如上语法分析代码片段，结束 */
}

/*
jump-statement:
  return expression_opt ;
*/
Node *parse_return_statement()
{
#if DEBUG
  printf("%s\n", __func__);
#endif

  Node *node = NULL;
  consume(TOK_RETURN);
  if (token.id != ';')
    node = parse_expression();
  consume(';');
  return create_return_stmt(node);
}

/*
block-item:
  declaration
  statement
*/
Node *parse_block_item()
{
#if DEBUG
  printf("%s\n", __func__);
#endif

  if (token.id == TOK_INT || token.id == TOK_VOID)
    return parse_declaration();
  return parse_statement();
}

/*
compound-statement:
        { block-item-list_opt }
*/
Node *parse_compound_statement()
{
#if DEBUG
  printf("%s\n", __func__);
#endif

  /* Create symbol table. */
  current_symtab = new_symbol_table(current_symtab);

  NodeList *head = NULL;
  NodeList **tail = &head;
  consume('{');
  while (token.id != '}')
  {
    *tail = create_node_list(NULL);
    (*tail)->node = parse_block_item();
    tail = &((*tail)->next);
  }
  *tail = NULL;
  consume('}');

  /* Set symbol table. */
  current_symtab = current_symtab->up;
  return create_compound_stmt(head);
}

/*
statement:
  compound-statement
  expression-statement
  selection-statement
  iteration-statement
  jump-statement
*/
Node *parse_statement()
{
#if DEBUG
  printf("%s\n", __func__);
#endif

  switch (token.id)
  {
  case '{':
    return parse_compound_statement();
  case TOK_IF:
    return parse_if_statement();
  case TOK_FOR:
    return parse_for_statement();
  case TOK_WHILE:
    return parse_while_statement();
  case TOK_RETURN:
    return parse_return_statement();
  default:
    return parse_expression_statement();
  }
}

/*
external-declaration:
  function-definition
  declaration

function-definition:
  type-specifier declarator compound-statement
*/
Node *parse_external_declaration()
{
#if DEBUG
  printf("%s\n", __func__);
#endif

  Declarator decltor;
  decltor.type = parse_type_specifier();
  parse_declarator(&decltor);

  /* function */
  if (token.id == '{')
  {
    local_var_list_head = local_var_list_tail = NULL;
    Node *body = parse_compound_statement();
    Node *node = build_func_decl(current_symtab->up, decltor.type, decltor.name,
                                 decltor.list, body);
    FUNC_DECL_VAR_LIST(node) = local_var_list_head;
    /* Set symbol table. */
    current_symtab = current_symtab->up;
    return node;
  }

  /* first declaration */
  NodeList *var_list = create_node_list(NULL);
  Node *init = NULL;
  if (token.id == '=')
  {
    consume('=');
    init = parse_initializer();
  }
  var_list->node = build_var_decl(current_symtab, decltor.name, decltor.type, init);

  /* the rest of declarations */
  NodeList *second_var_list = NULL;
  if (token.id == ',')
  {
    consume(',');
    second_var_list = parse_init_declarator_list(decltor.type);
  }
  var_list->next = second_var_list;
  consume(';');
  return create_var_decl_group(var_list);
}

/*
translation-unit:
  external-declaration
  translation-unit external-declaration
*/
Node *parse_translation_unit()
{
#if DEBUG
  printf("%s\n", __func__);
#endif

  Location loc = current_location;
  /* Create symbol table. */
  current_symtab = new_symbol_table(NULL);

  NodeList *head = NULL;
  NodeList **tail = &head;
  next_token();
  while (token.id != TOK_EOF)
  {
    // 尾插法构建抽象语法树
    *tail = create_node_list(NULL);
    (*tail)->node = parse_external_declaration();
    tail = &((*tail)->next);
  }
  *tail = NULL;

  /* Set symbol table. */
  current_symtab = current_symtab->up;

  return create_trans_unit(head, loc);
}

Node *parse_file(char *file_name)
{
  long size;
  FILE *fp = fopen(file_name, "r");
  if (!fp)
  {
    printf("Error: failed to open file '%s'.\n", file_name);
    exit(EXIT_FAILURE);
  }

  /* Caculate the file size. */
  fseek(fp, 0, SEEK_END);
  size = ftell(fp);
  rewind(fp);

  /* Read the file into buffer. */
  current_scan_ptr = scan_buffer = malloc(size + 1);
  scan_buffer_end = scan_buffer + size + 1;
  if (!scan_buffer)
  {
    printf("Error: failed to malloc scan buffer.\n");
    fclose(fp);
    exit(EXIT_FAILURE);
  }
  if (size != fread(scan_buffer, 1, size, fp)) // 把源文件内容读入
  {
    printf("Error: failed to read entire file.\n");
    fclose(fp);
    exit(EXIT_FAILURE);
  }
  scan_buffer[size] = '\0';
  fclose(fp);

  current_location = (Location){file_name, 1, 0};// {文件名，行号，列号}
  return parse_translation_unit();
}
