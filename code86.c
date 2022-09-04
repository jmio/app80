/*
                         Generic APP Source File

                          Filename : CODE86.C
                          Version  : 95/05/12
                            Written by Mio

                         実際の ASM コード出力
                     （条件ジャンプは COND86 に存在）
*/

#include <stdio.h>
#include <string.h>

#include "appmain.h"
#include "appdefs.h"
#include "appsub.h"
#include "tools86.h"
#include "function.h"                        /* 条件式中の _func */
#include "superstr.h"

#include "code86.h"

#define  FUNC_KW          "_func"            /* 関数呼び出し */

#define  INCR             "++"               /* pre increment */
#define  DECR             "--"               /* pre decrement */
#define  ADD              "+="               /* pre decrement */
#define  SUB              "-="               /* pre decrement */

#define  BYTE_PTR_STR     "BYTE PTR "
#define  WORD_PTR_STR     "WORD PTR "

#define  RETVAL_REG_OLD   "BP"               /* 戻り値が入るレジスタ(-O) */
#define  RETVAL_REG       "AX"               /* 戻り値が入るレジスタ     */

/* バイトレジスタ／ワードレジスタ変換テーブル */
static char *cpa_repbregs[] = { "AX","AX","BX","BX","CX","CX","DX","DX" } ;

/* バイト・ワードキーワードチェック */
void bwcast_eval (char *cp_expr,char *cp_dist)
{
   /* (byte)(word) キーワードの検査 */
   switch(cast_eval(cp_expr)) {
   case BYTETYP1:
   case BYTETYP2:
      strcpy(cp_dist,BYTE_PTR_STR) ;
   break ;
   case WORDTYP1:
   case WORDTYP2:
      strcpy(cp_dist,WORD_PTR_STR) ;
   break ;
   default:
      cp_dist[0] = '\0' ; /* clear */
   }
}

/* 項の評価 */
void expr_eval(char *cp_expr,char *cp_dist)
{
   /* 関数呼出し？ */
   if (strstr(cp_expr,FUNC_KW) == cp_expr) {
      eval_func(&cp_expr[app_strlen(FUNC_KW)]) ; /* 関数呼び出し */
      strcpy(cp_dist,(ia_option['O'] ? RETVAL_REG_OLD : RETVAL_REG)) ;
      return ;
   }

   /* (byte),(word) を BYTE PTR,WORD PTR にする */
   bwcast_eval(cp_expr,cp_dist) ;

   /* ++ , -- キーワードの検査 */
   incdec_eval(cp_dist,cp_expr,INCR,inc_output) ;
   incdec_eval(cp_dist,cp_expr,DECR,dec_output) ;

   /* += , -= キーワードの検査 (for の増分用) */
   addsub_eval(cp_dist,cp_expr,ADD,add_output) ;
   addsub_eval(cp_dist,cp_expr,SUB,sub_output) ;

   strcat(cp_dist,cp_expr) ;
}

/* ラベルの生成 */
void label_output(char *cp)
{
   app_printf("%s:\n",cp) ;
}

/* MOV の出力（評価前） */
void mov_output(char *cp1,char *cp2)
{
   char ca1[MAXEXPRLEN],ca2[MAXEXPRLEN] ;

   bwcast_eval(cp1,ca1) ; /* ca1 に ポインタ文字列を得る */
   strcat(ca1,cp1) ;

   expr_eval(cp2,ca2) ;
   if (strcmp(ca1,ca2) != 0) { /* 同一の時はキャンセル */
      app_printf("\tMOV\t%s,%s\n",ca1,ca2) ;
   }
}

/* LEA の出力（評価前） */
void lea_output(char *cp1,char *cp2)
{
   char ca1[MAXEXPRLEN],ca2[MAXEXPRLEN] ;

   bwcast_eval(cp1,ca1) ; /* ca1 に ポインタ文字列を得る */
   strcat(ca1,cp1) ;

   expr_eval(cp2,ca2) ;
   if (strcmp(ca1,ca2) != 0) { /* 同一の時はキャンセル */
      app_printf("\tLEA\t%s,%s\n",ca1,ca2) ;
   }
}

/* 関数の戻り値代入 MOV 出力（評価前） */
void retval_output(char *cp1)
{
   char ca1[MAXEXPRLEN] ;

   expr_eval(cp1,ca1) ;
   mov_output((ia_option['O'] ? RETVAL_REG_OLD : RETVAL_REG),ca1) ;
}

/* CMP の出力（評価前） */
void cmp_output(char *cp1,char *cp2)
{
   char ca1[MAXEXPRLEN],ca2[MAXEXPRLEN] ;

   expr_eval(cp1,ca1) ;
   expr_eval(cp2,ca2) ;
   app_printf("\tCMP\t%s,%s\n",ca1,ca2) ;
}

/* AND の出力（評価前） */
void and_output(char *cp1,char *cp2)
{
   char ca1[MAXEXPRLEN],ca2[MAXEXPRLEN] ;

   expr_eval(cp1,ca1) ;
   expr_eval(cp2,ca2) ;
   app_printf("\tAND\t%s,%s\n",ca1,ca2) ;
}

/* TEST の出力（評価前） */
void test_output(char *cp1,char *cp2)
{
   char ca1[MAXEXPRLEN],ca2[MAXEXPRLEN] ;

   expr_eval(cp1,ca1) ;
   expr_eval(cp2,ca2) ;
   app_printf("\tTEST\t%s,%s\n",ca1,ca2) ;
}

/* 無条件 JMP の出力 */
void jmp_output(char *cp)
{
   app_printf("\tJMP\t%s\n",cp) ;
}

/* 無条件 JMP の出力（SHORT） */
void short_jmp_output(char *cp)
{
   if (ia_option['N']) {
      app_printf("\tJMP\t%s\n",cp) ;
   } else {
      app_printf("\tJMP\tSHORT %s\n",cp) ;
   }
}

/* JCXZ の出力 */
void jcxz_output(char *cp)
{
   app_printf("\tJCXZ\t%s\n",cp) ;
}

/* LOOP の出力 */
void loop_output(char *cp)
{
   app_printf("\tLOOP\t%s\n",cp) ;
}

/* サブルーチン CALL の出力 */
void call_output(char *cp)
{
   app_printf("\tCALL\t%s\n",cp) ;
}

/* サブルーチン RET の出力 */
void ret_output(void)
{
   app_outstr("\tRET\n") ;
}

/* 割り込み IRET の出力 */
void iret_output(void)
{
   app_outstr("\tIRET\n") ;
}

/* 変数の保存(PUSH)出力 */
void push_output(char *cp)
{
   int i ;

   /* バイトレジスタをワードレジスタに変換 */
   if ((i = is_bytereg(cp)) != 0) {
      cp = cpa_repbregs[i-1] ;
   }
   if (strcmp(cp,"ALL") == 0) {
      app_outstr("\tPUSHA\n") ;
   } else if (strcmp(cp,"FLAGS") == 0) {
      app_outstr("\tPUSHF\n") ;
   } else {
      app_printf("\tPUSH\t%s\n",cp) ;
   }
}

/* 変数の保存(POP)出力 */
void pop_output(char *cp)
{
   int i ;

   /* バイトレジスタをワードレジスタに変換 */
   if ((i = is_bytereg(cp)) != 0) {
      cp = cpa_repbregs[i] ;
   }
   if (strcmp(cp,"ALL") == 0) {
      app_outstr("\tPOPA\n") ;
   } else if (strcmp(cp,"FLAGS") == 0) {
      app_outstr("\tPOPF\n") ;
   } else {
      app_printf("\tPOP\t%s\n",cp) ;
   }
}

/* 変数のインクリメント出力（評価ずみ） */
void inc_output(char *cp)
{
   app_printf("\tINC\t%s\n",cp) ;
}

/* 変数のデクリメント出力（評価ずみ） */
void dec_output(char *cp)
{
   app_printf("\tDEC\t%s\n",cp) ;
}

/* 変数の加算出力 */
void add_output(char *cp1,char *cp2)
{
   app_printf("\tADD\t%s,%s\n",cp1,cp2) ;
}

/* 変数の減算出力 */
void sub_output(char *cp1,char *cp2)
{
   app_printf("\tSUB\t%s,%s\n",cp1,cp2) ;
}

/* PUBLIC 宣言 */
void public_output(char *cp)
{
   app_printf("\tPUBLIC\t%s\n",cp) ;
}

/* EXTERNAL 宣言 */
void extern_output(char *cp)
{
   app_printf("\tEXTRN\t%s:NEAR\n",cp) ;
}
