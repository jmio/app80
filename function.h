/*
                         Generic APP Source File

                          Filename : FUNCTION.H
                          Version  : 95/03/09
                            Written by Mio

                              関数の処理
*/

#ifndef FUNCTION_H
#define FUNCTION_H

#define MAXFNLEN   50               /* 関数名の最大の長さ */
#define MAXARGS    10               /* 関数の引き数の最大 */
#define MAXARGLEN  256              /* 関数個別の引き数の長さ */

#define FUNDELIMS    ":(;\n"        /* 関数名のデリミタ */
#define ARGDELIMS    ")"            /* 引数ブロックのデリミタ */
#define ARGCUTDELIM  ","            /* 引数の区切り */
#define REGDELIMS    ";\n"          /* 保存変数ブロックのデリミタ */

/* ヘッダ生成オプションで使用されるヘッダ文字列 */
#define OLD_FUNC_H  "@_function"
#define OLD_PROC_H  "@_procedure"

#define NEW_FUNC_H  "_function_h"
#define NEW_PROC_H  "_procedure_h"

#define LABEL_FEND  "%s_END"        /* 関数出口のラベルの書式文字列 */

#define PUSH_CHAR            '^'    /* 関数形式の変数保存指示 */
#define LEA_CHAR             '*'    /* 関数形式の変数 LEA 渡し指示 */

#include "function.prt"

#endif /* of FUNCTION_H */
