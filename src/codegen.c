#include "ast.h"
#include "riscv.h"
#include "symbol.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <printf.h>

typedef enum {
  UNKNOWN_SECTION,
  TEXT_SECTION,
  BSS_SECTION,
  DATA_SECTION,
  RODATA_SECTION
} Section;

typedef enum {
  INT_VALUE,
  REG_VALUE
} ValueKind;

typedef struct {
  ValueKind kind;
  union {
    int ival;
    RegID reg;
  } value;
} ExprResult;

static FILE *out;
static Section current_section = UNKNOWN_SECTION;
static Node *current_trans_unit;
static Node *current_func_decl;
static bool has_explicit_return;
void visit(Node *node);
ExprResult visit_expr(Node *node);
void gen_init(Node *node);

char *new_label()
{
  static int id = 1;
  char *label;
  asprintf(&label, ".L%d", id);
  ++id;
  return label;
}

Symbol *new_global_constant()
{
  static int id = 0;
  char *name;
  asprintf(&name, ".LC%d", id);
  Symbol *sym = malloc(sizeof(Symbol));
  sym->name = name;
  ++id;
  return sym;
}

RegID get_reg(Symbol *sym, RegAttr attr)
{
  if (sym && sym->reg >= 0 && sym->reg <= num_of_regs)
    return sym->reg;

  for (int i = 0; i < num_of_regs; ++i) {
    if (((reg_info[i].attr & attr) == attr) && reg_info[i].available) {
      reg_info[i].available = false;
      if (sym) 
        sym->reg = i;
      return reg_info[i].id;
    }
  }
  assert(0);
  return 0;
}

void free_reg(RegID reg)
{
  reg_info[reg].available = true;
}

void emit_directive(const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  fprintf(out, "\t");
  vfprintf(out, fmt, ap);
  fprintf(out, "\n");
  va_end(ap);
}

void print_address(Address *addr)
{
  switch (addr->kind) {
  case ADDR_OFFSET:
    fprintf(out, "%d", addr->value.offset);
    break;
  case ADDR_SYM_HI:
    fprintf(out, "%%hi(%s)", addr->value.sym->name);
    break;
  case ADDR_SYM_LO:
    fprintf(out, "%%lo(%s)", addr->value.sym->name);
    break;
  case ADDR_SYM:
    fprintf(out, "%s", addr->value.sym->name);
    break;
  }
}

void print_register(RegID reg)
{
  fprintf(out, "%s", reg_info[reg].name);
}

void emit_insn(int opc, ...)
{
  const InstrInfo *insn = get_instruction_info(opc);
  va_list ap;
  va_start(ap, opc);
  fprintf(out, "\t%s\t", insn->name);
  for (char *p = insn->args; *p != '\0'; ++p) {
    if (*p != '%') {
      putc(*p, out);
      continue;
    }
    ++p;
    switch (*p) {
    case 'A': {
      Address *addr = va_arg(ap, Address *);
      print_address(addr);
      break;
    }
    case 'R': {
      RegID reg = va_arg(ap, RegID);
      print_register(reg);
      break;
    }
    case 's': {
      char *argp = va_arg(ap, char *);
      fprintf(out, "%s", argp);
      break;
    }
    case 'd': {
      int argp = va_arg(ap, int);
      fprintf(out, "%d", argp);
      break;
    }
    default:
      assert(0);
    }
  }
  fprintf(out, "\n");
  va_end(ap);
}

void emit_move(RegID reg, ExprResult res)
{
  if (res.kind == INT_VALUE)
    emit_insn(OPC_LI, reg, res.value.ival);
  else if (reg != res.value.reg)
    emit_insn(OPC_MV, reg, res.value.reg);
}

void emit_label(const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  vfprintf(out, fmt, ap);
  fprintf(out, ":\n");
  va_end(ap);
}

void emit_section(Section sec)
{
  if (current_section == sec)
    return;

  switch (sec)
  {
  case TEXT_SECTION:
    emit_directive(".text"); break;
  case BSS_SECTION:
    emit_directive(".bss"); break;
  case DATA_SECTION:
    emit_directive(".data"); break;
  case RODATA_SECTION:
    emit_directive(".rodata"); break;
  default:
    assert(0);
  }
  current_section = sec;
}

ExprResult load_value(Symbol *sym)
{
  ExprResult res;
  res.kind = REG_VALUE;

  //if (sym->reg != -1) {
  //  res.value.reg = sym->reg;
  //  return res;
  //}

  int i;
  for (i = 0; i < num_of_regs; ++i)
    if ((reg_info[i].attr & RA_TEMP) && reg_info[i].available)
      break;
  assert(i < num_of_regs);

  sym->reg = reg_info[i].id;
  reg_info[i].available = false;

  if (sym->attr & SA_GLOBAL) {
    Address addr_hi = {ADDR_SYM_HI, {.sym = sym}};
    Address addr_lo = {ADDR_SYM_LO, {.sym = sym}};
    emit_insn(OPC_LUI, sym->reg, &addr_hi);
    emit_insn(OPC_LW, sym->reg, &addr_lo, sym->reg);
  } else {
    emit_insn(OPC_LW, sym->reg, &sym->addr, REG_FP);
  }

  res.value.reg = sym->reg;
  return res;
}

void store_value(Symbol *sym, ExprResult res)
{
  RegID reg;
  if (res.kind == INT_VALUE) {
    reg = get_reg(NULL, RA_TEMP);
    emit_insn(OPC_LI, reg, res.value.ival);
  }
  else
    reg = res.value.reg;

  Address *addr = &sym->addr;
  if (addr->kind == ADDR_OFFSET)
    emit_insn(OPC_SW, reg, addr, REG_FP);

  free_reg(res.value.reg);
}

void copy_memory(Symbol *to, Symbol *from)
{
  RegID reg_a = get_reg(NULL, RA_TEMP);
  RegID reg_v = get_reg(NULL, RA_TEMP);
  Address addr_hi = {ADDR_SYM_HI, {.sym = from}};
  Address addr_lo = {ADDR_SYM_LO, {.sym = from}};
  emit_insn(OPC_LUI, reg_a, &addr_hi);
  emit_insn(OPC_ADDI_A, reg_a, reg_a, &addr_lo);
  for (int i = 0; i < to->size; i = i + 4) {
    Address addr_from = {ADDR_OFFSET, {.offset = i}};
    Address addr_to = {ADDR_OFFSET, {.offset = to->addr.value.offset + i}};
    emit_insn(OPC_LW, reg_v, &addr_from, reg_a);
    emit_insn(OPC_SW, reg_v, &addr_to, REG_FP);
  }
}

void visit_trans_unit(Node *node)
{
  current_trans_unit = node;
  emit_directive(".file\t\"%s\"", node->trans_unit.loc.file);
  for (NodeList *nl = node->trans_unit.nl; nl; nl = nl->next)
    visit(nl->node);
}

int caculate_frame_size(Node *node)
{
  int size = 8; /* ra, s0 */

  /* Arguments. */
  for (int i = 0; i < node->func_decl.num_of_params; i++) {
    size += get_type_size(PARAM_DECL_TYPE(node->func_decl.params[i]));
  }

  /* Local variables. */
  NodeList *np = FUNC_DECL_VAR_LIST(node);
  while (np) {
    Symbol *sym = IDENT_EXPR_SYM(VAR_DECL_NAME(np->node));
    sym->size = get_type_size(VAR_DECL_TYPE(np->node));
    size += sym->size;
    if (VAR_DECL_INIT(np->node) &&
        VAR_DECL_INIT(np->node)->kind == INIT_LIST_EXPR) {
      /* Constant value. */
      emit_section(RODATA_SECTION);
      sym->init = new_global_constant();
      emit_label(sym->init->name);
      gen_init(VAR_DECL_INIT(np->node));
    }
    np = np->next;
  }
  return size;
}

void set_frame_offset(Node *node, int frame_size)
{
  int size = 0;
  /* Arguments. */
  for (int i = 0; i < node->func_decl.num_of_params; i++) {
    Symbol *sym = IDENT_EXPR_SYM(PARAM_DECL_NAME(node->func_decl.params[i]));
    sym->addr.kind = ADDR_OFFSET;
    sym->addr.value.offset = size - frame_size;
    if (i < 8)
      sym->reg = REG_A0 + i;
    size += get_type_size(PARAM_DECL_TYPE(node->func_decl.params[i]));
  }

  /* Local variables. */
  NodeList *np = FUNC_DECL_VAR_LIST(node);
  while (np) {
    Symbol *sym = IDENT_EXPR_SYM(VAR_DECL_NAME(np->node));
    sym->addr.kind = ADDR_OFFSET;
    sym->addr.value.offset = size - frame_size;
    size += get_type_size(VAR_DECL_TYPE(np->node));
    np = np->next;
  }
}

void visit_func_decl(Node *node)
{
  current_func_decl = node;
  has_explicit_return = false;

  int frame_size = caculate_frame_size(node);
  set_frame_offset(node, frame_size);
  Address addr_ra = {ADDR_OFFSET, {.offset = frame_size - 4}};
  Address addr_fp = {ADDR_OFFSET, {.offset = frame_size - 8}};

  /* Prologue. */
  emit_section(TEXT_SECTION);
  emit_directive(".globl\t%s", IDENT_EXPR_NAME(FUNC_DECL_NAME(node)));
  emit_directive(".type\t%s, @function", IDENT_EXPR_NAME(FUNC_DECL_NAME(node)));
  emit_label("%s", IDENT_EXPR_NAME(FUNC_DECL_NAME(node)));

  emit_insn(OPC_ADDI, REG_SP, REG_SP, -frame_size);
  emit_insn(OPC_SW, REG_RA, &addr_ra, REG_SP);
  emit_insn(OPC_SW, REG_FP, &addr_fp, REG_SP);
  emit_insn(OPC_ADDI, REG_FP, REG_SP, frame_size);

  /* Arguments. */
  for (int i = 0; i < node->func_decl.num_of_params; i++) {
    Symbol *sym = IDENT_EXPR_SYM(PARAM_DECL_NAME(node->func_decl.params[i]));
    if (i < 8) {
      ExprResult res = {REG_VALUE, {.reg = REG_A0 + i}};
      store_value(sym, res);
    }
  }

  visit(node->func_decl.body);

  /* Epilogue */
  if (has_explicit_return)
    emit_label(".L%s_exit", IDENT_EXPR_NAME(FUNC_DECL_NAME(node)));
  emit_insn(OPC_LW, REG_RA, &addr_ra, REG_SP);
  emit_insn(OPC_LW, REG_FP, &addr_fp, REG_SP);
  emit_insn(OPC_ADDI, REG_SP, REG_SP, frame_size);
  emit_directive("ret");

  current_func_decl = NULL;
}

void gen_init_const_expr(Node *node)
{
  emit_directive(".word\t%d", CONST_EXPR_VALUE(node));
}

void gen_init_list_expr(Node *node)
{
  for (int i = 0; i < node->init_list_expr.num_of_inits; ++i)
    gen_init(node->init_list_expr.inits[i]);
}

void gen_init(Node *node)
{
  switch (node->kind) {
  case CONST_EXPR: gen_init_const_expr(node); break;
  case INIT_LIST_EXPR: gen_init_list_expr(node); break;
  default: assert(0);
  }
}

void visit_var_decl(Node *node)
{
  Symbol *sym = IDENT_EXPR_SYM(VAR_DECL_NAME(node));
  if (sym->attr & SA_GLOBAL) {
    if (VAR_DECL_INIT(node))
      emit_section(DATA_SECTION);
    else
      emit_section(BSS_SECTION);

    int align = get_type_align(VAR_DECL_TYPE(node));
    int size = get_type_size(VAR_DECL_TYPE(node));

    emit_directive(".globl\t%s", IDENT_EXPR_NAME(VAR_DECL_NAME(node)));
    emit_directive(".align\t%d", align);
    emit_directive(".type\t%s, @object", IDENT_EXPR_NAME(VAR_DECL_NAME(node)));
    emit_directive(".size\t%s, %d", IDENT_EXPR_NAME(VAR_DECL_NAME(node)), size);
    emit_label("%s", IDENT_EXPR_NAME(VAR_DECL_NAME(node)));

    /* value */
    if (!VAR_DECL_INIT(node))
      emit_directive(".zero\t%d", size);
    else
      gen_init(VAR_DECL_INIT(node));
  }
  else if (VAR_DECL_INIT(node)) {
    if (VAR_DECL_INIT(node)->kind == INIT_LIST_EXPR) {
      copy_memory(sym, sym->init);
    }
    else {
      ExprResult init = visit_expr(VAR_DECL_INIT(node));
      store_value(sym, init);
    }
  }

}

void visit_var_decl_group(Node *node)
{
  for (int i = 0; i < node->var_decl_group.num_of_vars; ++i)
    visit(node->var_decl_group.vars[i]);
}

void visit_param_decl(Node *node)
{
  /* Should not be visited. */
  assert(0);
}

ExprResult visit_ident_expr(Node *node)
{
  ExprResult res;
  char *name = IDENT_EXPR_NAME(node);
  Symbol *sym = IDENT_EXPR_SYM(node);
  return res;
}

ExprResult visit_const_expr(Node *node)
{
  ExprResult res;
  res.kind = REG_VALUE;
  RegID reg = get_reg(NULL, RA_TEMP);
  emit_insn(OPC_LI, reg, CONST_EXPR_VALUE(node));
  res.value.reg = reg;
  return res;
}

ExprResult visit_decl_ref_expr(Node *node)
{
  char *name = DECL_REF_EXPR_NAME(node);
  Symbol *sym = DECL_REF_EXPR_SYM(node);
  return load_value(sym);
}

ExprResult visit_array_subscript_expr(Node *node)
{
  /* TODO: array codegen */
  ExprResult res = {REG_VALUE, {.reg = REG_ZERO}};
  return res;
}

ExprResult visit_unary_op(Node *node)
{
  ExprResult res = visit_expr(node->unary_op.operand);
  if (node->unary_op.op == OP_PLUS)
    return res;
  else if (node->unary_op.op == OP_MINUS)
    emit_insn(OPC_SUB, res.value.reg, REG_ZERO, res.value.reg); 

  return res;
}

ExprResult gen_or(ExprResult rs1, ExprResult rs2)
{
  RegID rd = get_reg(NULL, RA_TEMP);
  char *l_false = new_label();
  char *l_true = new_label();
  emit_insn(OPC_LI, rd, 1);
  emit_insn(OPC_BNE, rs1.value.reg, REG_ZERO, l_true);
  emit_label(l_false);
  emit_insn(OPC_SLTU, rd, REG_ZERO, rs2.value.reg);
  emit_label(l_true);
  emit_insn(OPC_ANDI, rd, rd, 1);
  free(l_false);
  free(l_true);
  free_reg(rs1.value.reg);
  free_reg(rs2.value.reg);
  ExprResult res = {REG_VALUE, {.reg = rd}};
  return res;
}

ExprResult gen_and(ExprResult rs1, ExprResult rs2)
{
  RegID rd = get_reg(NULL, RA_TEMP);
  char *l_false = new_label();
  char *l_true = new_label();
  emit_insn(OPC_LI, rd, 0);
  emit_insn(OPC_BEQ, rs1.value.reg, REG_ZERO, l_true);
  emit_label(l_false);
  emit_insn(OPC_SLTU, rd, REG_ZERO, rs2.value.reg);
  emit_label(l_true);
  emit_insn(OPC_ANDI, rd, rd, 1);
  free(l_false);
  free(l_true);
  free_reg(rs1.value.reg);
  free_reg(rs2.value.reg);
  ExprResult res = {REG_VALUE, {.reg = rd}};
  return res;
}

ExprResult gen_eq(ExprResult rs1, ExprResult rs2)
{
  RegID rd = get_reg(NULL, RA_TEMP);
  emit_insn(OPC_XOR, rd, rs1.value.reg, rs2.value.reg);
  emit_insn(OPC_SLTIU, rd, rd, 1);
  free_reg(rs1.value.reg);
  free_reg(rs2.value.reg);
  ExprResult res = {REG_VALUE, {.reg = rd}};
  return res;
}

ExprResult gen_ne(ExprResult rs1, ExprResult rs2)
{
  RegID rd = get_reg(NULL, RA_TEMP);
  emit_insn(OPC_XOR, rd, rs1.value.reg, rs2.value.reg);
  emit_insn(OPC_SLTU, rd, REG_ZERO, rd);
  free_reg(rs1.value.reg);
  free_reg(rs2.value.reg);
  ExprResult res = {REG_VALUE, {.reg = rd}};
  return res;
}

ExprResult gen_le(ExprResult rs1, ExprResult rs2)
{
  RegID rd = get_reg(NULL, RA_TEMP);
  emit_insn(OPC_SLT, rd, rs2.value.reg, rs1.value.reg);
  emit_insn(OPC_XORI, rd, rd, 1);
  free_reg(rs1.value.reg);
  free_reg(rs2.value.reg);
  ExprResult res = {REG_VALUE, {.reg = rd}};
  return res;
}

ExprResult gen_ge(ExprResult rs1, ExprResult rs2)
{
  RegID rd = get_reg(NULL, RA_TEMP);
  emit_insn(OPC_SLT, rd, rs1.value.reg, rs2.value.reg);
  emit_insn(OPC_XORI, rd, rd, 1);
  free_reg(rs1.value.reg);
  free_reg(rs2.value.reg);
  ExprResult res = {REG_VALUE, {.reg = rd}};
  return res;
}

ExprResult gen_binary_op(Opcode opc, ExprResult rs1, ExprResult rs2)
{
  RegID rd = get_reg(NULL, RA_TEMP);
  emit_insn(opc, rd, rs1.value.reg, rs2.value.reg);
  free_reg(rs1.value.reg);
  free_reg(rs2.value.reg);
  ExprResult res = {REG_VALUE, {.reg = rd}};
  return res;
}

ExprResult visit_binary_op(Node *node)
{
  Opcode opc;

  if (node->binary_op.op == OP_ASSIGN) {
    Symbol *sym = DECL_REF_EXPR_SYM(node->binary_op.left);
    ExprResult rs2 = visit_expr(node->binary_op.right);
    store_value(sym, rs2);
    return rs2;
  }

  ExprResult rs1 = visit_expr(node->binary_op.left);
  ExprResult rs2 = visit_expr(node->binary_op.right);

  switch (node->binary_op.op) {
  default: assert(0); break;
  case OP_OR: return gen_or(rs1, rs2);
  case OP_AND: return gen_and(rs1, rs2);
  case OP_EQ: return gen_eq(rs1, rs2);
  case OP_NE: return gen_ne(rs1, rs2);
  case OP_LT: return gen_binary_op(OPC_SLT, rs1, rs2);
  case OP_GT: return gen_binary_op(OPC_SLT, rs2, rs1);
  case OP_LE: return gen_le(rs1, rs2);
  case OP_GE: return gen_ge(rs1, rs2);
  case OP_ADD: return gen_binary_op(OPC_ADD, rs1, rs2);
  case OP_SUB: return gen_binary_op(OPC_SUB, rs1, rs2);
  case OP_MUL: return gen_binary_op(OPC_MUL, rs1, rs2);
  case OP_DIV: return gen_binary_op(OPC_DIV, rs1, rs2);
  case OP_REM: return gen_binary_op(OPC_REM, rs1, rs2);
  case OP_BAND: return gen_binary_op(OPC_AND, rs1, rs2); // 新加-按位与
  case OP_SLL: return gen_binary_op(OPC_SLL, rs1, rs2); // 新加-逻辑左移
  }
}

ExprResult visit_call_expr(Node *node)
{
  for (int i = 0; i < node->call_expr.num_of_args; ++i) {
    ExprResult arg = visit_expr(node->call_expr.args[i]);
    if (i < 8)
      emit_move(REG_A0 + i, arg);
  }
  emit_insn(OPC_CALL, DECL_REF_EXPR_NAME(node->call_expr.declref));
  ExprResult res = {REG_VALUE, {.reg = REG_A0}};
  return res;
}

ExprResult visit_expr(Node *node)
{
  switch (node->kind) {
  case IDENT_EXPR: return visit_ident_expr(node);
  case CONST_EXPR: return visit_const_expr(node);
  case DECL_REF_EXPR: return visit_decl_ref_expr(node);
  case ARRAY_SUBSCRIPT_EXPR: return visit_array_subscript_expr(node);
  case UNARY_OP: return visit_unary_op(node);
  case BINARY_OP: return visit_binary_op(node);
  case CALL_EXPR: return visit_call_expr(node);
  default: assert(0);
  }
}

void visit_compound_stmt(Node *node)
{
  for (int i = 0; i < node->compound_stmt.num_of_items; ++i)
    visit(node->compound_stmt.items[i]);
}

void visit_if_stmt(Node *node)
{
  /* FIXME: 实现如下代码生成片段, 开始 */
  char *l_else = new_label();
  char *l_next = new_label();
  ExprResult cond = visit_expr(node->if_stmt.cond);
  emit_insn(OPC_BEQ, cond.value.reg, REG_ZERO, l_else);
  visit(node->if_stmt.then_stmt);
  emit_insn(OPC_J, l_next);
  emit_label(l_else);
  visit(node->if_stmt.else_stmt);

  /* 实现之后删除该语句 */
  // assert(0 && "FIXME");

  emit_label(l_next);
  free(l_else);
  free(l_next);
  free_reg(cond.value.reg);
  /* FIXME: 实现如上代码生产片段，结束 */
}

void visit_while_stmt(Node *node)
{
  char *l_begin = new_label();
  char *l_next = new_label();
  emit_label(l_begin);
  ExprResult cond = visit_expr(node->while_stmt.cond);
  emit_insn(OPC_BEQ, cond.value.reg, REG_ZERO, l_next);
  visit(node->while_stmt.body);
  emit_insn(OPC_J, l_begin);
  emit_label(l_next);
  free(l_begin);
  free(l_next);
  free_reg(cond.value.reg);
}

void visit_for_stmt(Node *node)
{
  char *l_begin = new_label();
  char *l_next = new_label();
  visit(node->for_stmt.init);
  emit_label(l_begin);
  ExprResult cond = visit_expr(node->for_stmt.cond);
  emit_insn(OPC_BEQ, cond.value.reg, REG_ZERO, l_next);
  visit(node->for_stmt.body);
  visit(node->for_stmt.incr);
  emit_insn(OPC_J, l_begin);
  emit_label(l_next);
  free(l_begin);
  free(l_next);
  free_reg(cond.value.reg);
}

void visit_return_stmt(Node *node)
{
  if (node->return_stmt.expr) {
    ExprResult res = visit_expr(node->return_stmt.expr);
    emit_move(REG_A0, res);
    free_reg(res.value.reg);
  }

  char *label;
  asprintf(&label, ".L%s_exit",
           IDENT_EXPR_NAME(FUNC_DECL_NAME(current_func_decl)));
  emit_insn(OPC_J, label);
  free(label);
  has_explicit_return = true;
}

void visit(Node *node)
{
  if (node == NULL)
    return;

  switch (node->kind) {
  case TRANS_UNIT: visit_trans_unit(node); break;
  case FUNC_DECL: visit_func_decl(node); break;
  case VAR_DECL: visit_var_decl(node); break;
  case VAR_DECL_GROUP: visit_var_decl_group(node); break;
  case PARAM_DECL: visit_param_decl(node); break;
  case IDENT_EXPR: visit_ident_expr(node); break;
  case CONST_EXPR: visit_const_expr(node); break;
  case DECL_REF_EXPR: visit_decl_ref_expr(node); break;
  case ARRAY_SUBSCRIPT_EXPR: visit_array_subscript_expr(node); break;
  case UNARY_OP: visit_unary_op(node); break;
  case BINARY_OP: visit_binary_op(node); break;
  case CALL_EXPR: visit_call_expr(node); break;
  case COMPOUND_STMT: visit_compound_stmt(node); break;
  case NULL_STMT: break;
  case IF_STMT: visit_if_stmt(node); break;
  case WHILE_STMT: visit_while_stmt(node); break;
  case FOR_STMT: visit_for_stmt(node); break;
  case RETURN_STMT: visit_return_stmt(node); break;
  default: assert(0);
  }
}

void gen_asm(Node *node, char *file_name)
{
  out = fopen(file_name, "w");
  if (!out) {
    printf("Error: failed to open file '%s'.\n", file_name);
    return;
  }

  visit(node);
}
