#include "ast.h"
#include "scanner.h"
#include "diag.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>

Node *parse_file(char *file_name); // 要编译的源代码文件
void gen_asm(Node *node, char *asm_file); // 生成汇编代码

#define BANNER "\
 _____                _          _____		\n\
|  __ \\              | |        / ____|	\n\
| |__) |_ _ _ __   __| | __ _  | |     		\n\
|  ___/ _` | '_ \\ / _` |/ _` | | |     	\n\
| |  | (_| | | | | (_| | (_| | | |____ 		\n\
|_|   \\__,_|_| |_|\\__,_|\\__,_|  \\_____|	\n\n"

const char *help_msg = "\
Usage: pcc [options] file...\n\
Options:\n\
  --dump-token		Parse and dump lexical tokens.\n\
  --dump-ast		Parse and dump AST.\n\
  -S			Compile source code and generate assembly.\n\
  -h, --help		Display help information.\n\
  -v, --version		Display version information.\n\
";

const char *version_msg = "Panda C compiler version 0.2";

int opt_dump_token = 1; // 是否生成词法单元
static int opt_dump_ast; // 是否生成抽象语法树
static int opt_gen_asm; // 是否生成汇编语言

// pcc编译指令选项
void parse_options(int argc, char *argv[])
{
  int c;
  int option_index = 0;
  static struct option long_options[] =
  {
    /* const char *name, int has_arg, int *flag, int val */
    {"dump-token", no_argument, &opt_dump_token, 1},
    {"dump-ast", no_argument, &opt_dump_ast, 1},
    {"asm", no_argument, 0, 'S'},
    {"help", no_argument, 0, 'h'},
    {"version", no_argument, 0, 'v'},
    {0, 0, 0, 0}
  }; // pcc编译指令可选参数

  if (argc == 1) {
    printf(BANNER);
    printf("%s\n", help_msg);
    exit(EXIT_SUCCESS);
  }

  while ((c = getopt_long(argc, argv, "Shv", long_options, &option_index))
         != -1)
  {
    switch (c)
    {
      default:
      case 'h':
        printf(BANNER);
        printf("%s\n", help_msg);
        exit(EXIT_SUCCESS);
      case 'v':
        printf(BANNER);
        printf("%s\n", version_msg);
        exit(EXIT_SUCCESS);
      case 'S':
        opt_gen_asm = 1;
        break;
      case 0:
        break;
    }
  }
}

// 生成与给定 C 源文件名对应的默认汇编文件名
char *get_default_asm_file(char *filename)
{
  char *asm_file = NULL;
  int len = strlen(filename);
  if (filename[len - 2] == '.' && filename[len - 1] == 'c') {
    asm_file = malloc(len + 1);
    strcpy(asm_file, filename);
    asm_file[len - 1] = 's'; // len长度不变
  }// a.c->a.s
  else {
    asm_file = malloc(len + 3);
    strcpy(asm_file, filename);
    strcat(asm_file, ".s");
  }
  return asm_file;
}

int main(int argc, char *argv[])
{
  parse_options(argc, argv);
  init_scanner(); // 初始化保留字条目到哈希表

  while (optind < argc) {
    char *src_file = argv[optind++];
    Node *node = parse_file(src_file);

    if (num_of_warnings && num_of_errors) {
      printf("%d warning%s and %d error%s generated.\n",
             num_of_warnings, num_of_warnings > 1 ? "s" : "",
             num_of_errors, num_of_errors > 1 ? "s" : "");
      return EXIT_FAILURE;
    }
    if (num_of_errors) {
      printf("%d error%s generated.\n",
             num_of_errors, num_of_errors > 1 ? "s" : "");
      return EXIT_FAILURE;
    }
    if (num_of_warnings) {
      printf("%d warning%s generated.\n",
             num_of_warnings, num_of_warnings > 1 ? "s" : "");
    }

    if (opt_dump_ast)
      dump_node(node); // 打印抽象语法树
    if (opt_gen_asm) {
      // 生成汇编文件
      char *asm_file = get_default_asm_file(basename(src_file));
      gen_asm(node, asm_file);
      free(asm_file);
    }
  }

  return EXIT_SUCCESS;
}
