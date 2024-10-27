#ifndef PANDA_TYPE_H
#define PANDA_TYPE_H

#include "node.h"

union Type;
typedef union Type Type;

typedef enum {
  VOID_TYPE,
  INT_TYPE,
  ARRAY_TYPE,
  FUNC_TYPE
} TypeKind;

typedef struct ArrayType {
  TypeKind kind;
  Type *elem_type;
  int num_of_dims;
  Node *dims[];
} ArrayType;

typedef struct FuncType {
  TypeKind kind;
  Type *ret_type;
  int num_of_params;
  Type *param_types[];
} FuncType;

union Type {
  TypeKind kind;
  struct ArrayType array_type;
  struct FuncType func_type;
};

extern Type *void_ty, *int_ty;

Type *create_array_type(Type *elem_type, NodeList *dim_list);
Type *create_func_type(Type *ret_type, NodeList *param_list);

void dump_type(Type *type);
int get_type_align(Type *type);
int get_type_size(Type *type);

#endif
