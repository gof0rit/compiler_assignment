#include "ast.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

Type void_type = {VOID_TYPE};
Type int_type = {INT_TYPE};
Type *void_ty = &void_type;
Type *int_ty = &int_type;

Type *create_array_type(Type *elem_type, NodeList *dim_list)
{
  int n = get_length_of_node_list(dim_list);

  ArrayType *type =
      (ArrayType *)malloc(sizeof(union Type) + n * sizeof(Node *));
  type->kind = ARRAY_TYPE;
  type->elem_type = elem_type;
  type->num_of_dims = n;
  copy_node_list_to_array(type->dims, dim_list);
  return (Type *)type;
}

Type *create_func_type(Type *ret_type, NodeList *param_list)
{
  int n = get_length_of_node_list(param_list);

  FuncType *type = (FuncType *)malloc(sizeof(union Type) + n * sizeof(Type *));
  type->kind = FUNC_TYPE;
  type->ret_type = ret_type;
  type->num_of_params = n;
  
  n = 0;
  NodeList *p = param_list;
  while (p) {
    assert(p->node->kind == PARAM_DECL);
    type->param_types[n++] = ((ParamDecl *)p->node)->type;
    p = p->next;
  }

  return (Type *)type;
}

int get_type_align(Type *type)
{
  switch (type->kind) {
  case INT_TYPE:
    return 4;
  case ARRAY_TYPE:
    return get_type_align(type->array_type.elem_type);
  case FUNC_TYPE:
    return 2;
  default:
    assert(0);
  }
}

int get_type_size(Type *type)
{
  switch (type->kind) {
  case INT_TYPE:
    return 4;
  case ARRAY_TYPE: {
    int size = get_type_size(type->array_type.elem_type);
    for (int i = 0; i < type->array_type.num_of_dims; ++i)
      size *= CONST_EXPR_VALUE(type->array_type.dims[i]);
    return size;
  }
  default:
    assert(0);
  }
}

void dump_type(Type *type)
{
  switch (type->kind) {
  case VOID_TYPE:
    printf("void");
    break;
  case INT_TYPE:
    printf("int");
    break;
  case ARRAY_TYPE:
    dump_type(type->array_type.elem_type);
    for (int i = 0; i < type->array_type.num_of_dims; ++i)
      printf("[%d]", CONST_EXPR_VALUE(type->array_type.dims[i]));
    break;
  case FUNC_TYPE:
    dump_type(type->func_type.ret_type);
    printf(" (");
    for (int i = 0; i < type->func_type.num_of_params; ++i) {
      if (i) printf(", ");
      dump_type(type->func_type.param_types[i]);
    }
    printf(")");
    break;
  }
}

