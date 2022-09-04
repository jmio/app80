/*
                         Generic APP Source File

                          Filename : MISC80.C
                          Version  : 95/03/14
                            Written by Mio

                            APP80 依存命令
*/

#include <stdio.h>
#include <string.h>

#include "appmain.h"
#include "appdefs.h"
#include "appsub.h"
#include "apperr.h"
#include "condexp.h"
#include "miscloop.h"
#include "proto.h"
#include "code80.h"
#include "misc80.h"

extern int i_cpu ;

/* _near の処理 */
#pragma argsused
int eval_near(char *cp_line)
{
   ia_option['N'] = TRUE ;

   return 0 ;
}

/* _short の処理 */
#pragma argsused
int eval_short(char *cp_line)
{
   ia_option['N'] = FALSE ;

   return 0 ;
}

/* ＣＰＵタイプの指定 */
int eval_cpu(char *cp_line)
{
   int i ;
   char ca_nfn[256] ;
   static char *cpa_cpuname[] = { "CPU80","CPU85","CPUZ80","CPU180","ASMZ80",NULL } ;
   static int ia_cputable[] = { 0, CPU80 , CPU85 , CPUZ80,  CPU180 , ASMZ80 } ;

   cutspc(cp_line) ;                              /* ごみを取り除く */
   gettoken(cp_line,WHITESPACE,LDELIM,ca_nfn) ;   /* ｃｐｕ名の切り出し */
   i = app_scantbl(cpa_cpuname,ca_nfn) ;
   if (i == 0) {
         app_error(999,1) ;		/* CPU_MODE が不適切		*/
   }
   i_cpu = ia_cputable[i] ;
   if (i_cpu == ASMZ80) {		/* 95/03/29	*/
      app_outstr("\t.Z80\n") ;
   } else {
      app_outstr("\t.8085\n") ;
   }
   return 0 ;
}

/* _loop の処理 */
int eval_loop(char *cp_line)
{
   char ca_buf[LBLBFLEN] ,
        ca_arg1[ARGSTRMAX],         /* 代入式の左辺 */
        ca_cond[ARGSTRMAX],         /* 代入演算子 */
        ca_arg2[ARGSTRMAX] ;        /* 代入式の右辺 */

   if (++i_looplev >= LPNESTMAX) {
      app_error(LOOP_STK_OVER,1) ;         /* loop のネストが深すぎる */
   }
   i_loopnum++ ;

   ia_loopn[i_looplev] = i_loopnum ;
   ia_contf[i_looplev] = FALSE ;
   ia_exitf[i_looplev] = FALSE ;

   /* 代入式の切り出し */
   cp_line = get_args(cp_line,ca_arg1,ca_cond,ca_arg2,NULL) ;

   if (strcmp(ca_cond,OP_LET) == 0) { /* 代入演算子か？ */
      cutspc(ca_arg1) ;
      cutspc(ca_arg2) ;
      mov_output(ca_arg1,ca_arg2) ;
   } else if (app_strlen(ca_cond) != 0) { /* そうでないとき */
      app_error(LARGCOND_ERROR,1) ;
   }

   tmplabel_output(TMPL_BEGIN,i_loopnum) ; /* ループの頭のラベル */

   cp_lcondstk[i_looplev] = strdup(ca_arg1) ; /* 条件の展開はあとで */
   cp_lcondstk2[i_looplev] = NULL ;

   return 0 ;
}

/* _endl の処理 */
#pragma argsused
int eval_endl(char *cp_line)
{
   if ((i_looplev == 0) || (cp_lcondstk[i_looplev] == NULL)) {
      app_error(LOOP_STK_EMPTY,1) ;     /* ループ外の _endl */
   }

   tmplabel_output(TMPL_CONTINUE,ia_loopn[i_looplev]) ; /* continue ラベル */
   loop80_output(cp_lcondstk[i_looplev],TMPL_BEGIN,ia_loopn[i_looplev]) ;
   app_free(cp_lcondstk[i_looplev]) ;     /* ループ変数文字列の開放 */

   if (ia_exitf[i_looplev] == TRUE) {
      tmplabel_output(TMPL_EXIT,ia_loopn[i_looplev]) ; /* exit ラベル */
   }
   i_looplev-- ;

   return 0 ;
}
