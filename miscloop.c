/*
                         Generic APP Source File

                          Filename : MISCLOOP.C
                          Version  : 95/03/01
                            Written by Mio

                            各種ループ処理
*/

#include <stdio.h>
#include <string.h>

#include "appdefs.h"
#include "apperr.h"
#include "appsub.h"
#include "proto.h"
#include "condexp.h"
#include "superstr.h"
#include "miscloop.h"

int i_loopnum = 0 ;              /* loop ラベル・カウンタ */
int i_looplev = 0 ;              /* loop ブロックネスティングレベル */
char *cp_lcondstk[LPNESTMAX] ,   /* loop 条件スタック1 */
     *cp_lcondstk2[LPNESTMAX] ;  /* loop 条件スタック2 (for) */
int ia_loopn[LPNESTMAX] ,        /* loop ラベル番号スタック */
    ia_contf[LPNESTMAX] ,        /* continue フラグスタック */
    ia_exitf[LPNESTMAX] ;        /* exit フラグスタック */

/* _while の処理 */
int eval_while(char *cp_line)
{
   if (++i_looplev >= LPNESTMAX) {
      app_error(LOOP_STK_OVER,1) ;       /* loop のネストが深すぎる */
   }
   i_loopnum++ ;

   ncjmp_output(TMPL_CONTINUE,i_loopnum) ; /* _continue 時のラベル */
   tmplabel_output(TMPL_BEGIN,i_loopnum) ; /* ループの頭のラベル */

   ia_loopn[i_looplev] = i_loopnum ;
   ia_contf[i_looplev] = TRUE ;
   ia_exitf[i_looplev] = FALSE ;

   cp_lcondstk[i_looplev]  = app_strdup(cp_line) ; /* 条件の展開はあとで */
   cp_lcondstk2[i_looplev] = NULL ;                /* 条件の展開はあとで */

   return 0 ;
}

/* _wend の処理 */
#pragma argsused
int eval_wend(char *cp_line)
{
   if ((i_looplev == 0) || (cp_lcondstk[i_looplev] == NULL)) {
      app_error(LOOP_STK_EMPTY,1) ;     /* ループ外の _wend */
   }

   tmplabel_output(TMPL_CONTINUE,ia_loopn[i_looplev]) ; /* continue ラベル */
   eval_cond(cp_lcondstk[i_looplev],FALSE,TMPL_BEGIN,
             ia_loopn[i_looplev],0,TRUE); /* 条件ジャンプの生成 */
   app_free(cp_lcondstk[i_looplev]) ;     /* 条件文字列の開放 */
   if (ia_exitf[i_looplev] == TRUE) {
      tmplabel_output(TMPL_EXIT,ia_loopn[i_looplev]) ; /* exit ラベル */
   }
   i_looplev-- ;

   return 0 ;
}

/* _repeat の処理 */
#pragma argsused
int eval_repeat(char *cp_line)
{
   if (++i_looplev >= LPNESTMAX) {
      app_error(LOOP_STK_OVER,1) ;       /* loop のネストが深すぎる */
   }
   i_loopnum++ ;

   tmplabel_output(TMPL_BEGIN,i_loopnum) ;    /* ループの頭のラベル */

   ia_loopn[i_looplev] = i_loopnum ;
   ia_contf[i_looplev] = FALSE ;
   ia_exitf[i_looplev] = FALSE ;
   cp_lcondstk[i_looplev]  = NULL ; /* 条件はここにない */
   cp_lcondstk2[i_looplev] = NULL ;

   return 0 ;
}

/* _until の処理 */
int eval_until(char *cp_line)
{
   if ((i_looplev == 0) || (cp_lcondstk[i_looplev] != NULL)) {
      app_error(LOOP_STK_EMPTY,1) ;     /* ループ外の _wend */
   }

   if (ia_contf[i_looplev] == TRUE) {
      tmplabel_output(TMPL_CONTINUE,ia_loopn[i_looplev]); /* continue ラベル */
   }
   /* 条件ジャンプの生成 */
   eval_cond(cp_line,TRUE,TMPL_BEGIN,ia_loopn[i_looplev],0,FALSE);
   if (ia_exitf[i_looplev] == TRUE) {
      tmplabel_output(TMPL_EXIT,ia_loopn[i_looplev]) ; /* exit ラベル */
   }
   i_looplev-- ;

   return 0 ;
}

/* _continue の処理 */
#pragma argsused
int eval_continue(char *cp_line)
{
   if (i_looplev == 0) {
      app_error(NOT_IN_LOOP,1) ;
   }
   ia_contf[i_looplev] = TRUE ;
   ncjmp_output(TMPL_CONTINUE,ia_loopn[i_looplev]) ;
   return 0 ;
}

/* _exit の処理 */
#pragma argsused
int eval_exit(char *cp_line)
{
   if (i_looplev == 0) {
      app_error(NOT_IN_LOOP,1) ;
   }
   ia_exitf[i_looplev] = TRUE ;
   ncjmp_output(TMPL_EXIT,ia_loopn[i_looplev]) ;
   return 0 ;
}

/* _let の処理 (_for の第一パラメータも) */
int eval_let(char *cp_line)
{
   char ca_arg1[ARGSTRMAX],         /* 代入式の左辺 */
        ca_cond[ARGSTRMAX],         /* 代入演算子 */
        ca_arg2[ARGSTRMAX] ;        /* 代入式の右辺 */

   cp_line = get_args(cp_line,ca_arg1,ca_cond,ca_arg2,NULL) ;

   if (strcmp(ca_cond,OP_LET) != 0) {
      app_error(LARGCOND_ERROR,1) ;
   }
   cutspc(ca_arg1) ;
   cutspc(ca_arg2) ;
   mov_output(ca_arg1,ca_arg2) ;

   return 0 ;
}

/* _for の処理 */
int eval_for(char *cp_line)
{
   char **cpp_tmp ;                       /* ３つの引数の分割用 */
   int i ;

   if (++i_looplev >= LPNESTMAX) {
      app_error(LOOP_STK_OVER,1) ;       /* loop のネストが深すぎる */
   }
   i_loopnum++ ;

   i = strtocpa(cp_line,&cpp_tmp,3,",;") ;
   if (i != 3) {
      if (i != 0) {
         cpa_release(cpp_tmp) ;
      }
      app_error(FOR_ARGUMENT,1) ;
   }

   /* mov の出力 */
   if (app_strlen(cpp_tmp[0]) > 0) {
      eval_let(cpp_tmp[0]) ;
   }

   /* 条件式があるか */
   if (app_strlen(cpp_tmp[1]) > 0) {
      cp_lcondstk[i_looplev] = app_strdup(cpp_tmp[1]) ;
      ncjmp_output(TMPL_FORBEGIN,i_loopnum) ; /* _continue 時のラベル */
   } else {
      cp_lcondstk[i_looplev] = NULL ;
   }

   /* 増分指定があるか */
   if (app_strlen(cpp_tmp[2]) > 0) {
      cp_lcondstk2[i_looplev] = app_strdup(cpp_tmp[2]) ;
   } else {
      cp_lcondstk2[i_looplev] = NULL ;
   }

   tmplabel_output(TMPL_BEGIN,i_loopnum) ; /* ループの頭のラベル */
   ia_loopn[i_looplev] = i_loopnum ;
   ia_contf[i_looplev] = FALSE ;
   ia_exitf[i_looplev] = FALSE ;

   cpa_release(cpp_tmp) ;
   return 0 ;
}

/* _next の処理 */
#pragma argsused
int eval_next(char *cp_line)
{
   char ca_tmp[128] ;

   if (i_looplev == 0) {
      app_error(LOOP_STK_EMPTY,1) ;     /* ループ外の _next */
   }

   tmplabel_output(TMPL_CONTINUE,ia_loopn[i_looplev]) ; /* continue ラベル */

   /* 増分の処理 */
   if (cp_lcondstk2[i_looplev] != NULL) {
      expr_eval(cp_lcondstk2[i_looplev],ca_tmp) ;
      app_free(cp_lcondstk2[i_looplev]) ; /* 増分文字列の開放 */
   }

   tmplabel_output(TMPL_FORBEGIN,ia_loopn[i_looplev]) ; /* for_bg ラベル */

   /* 条件の処理 */
   if (cp_lcondstk[i_looplev] != NULL) {
      eval_cond(cp_lcondstk[i_looplev],FALSE,TMPL_BEGIN,
                ia_loopn[i_looplev],0,TRUE); /* 条件ジャンプの生成 */
      app_free(cp_lcondstk[i_looplev]) ;     /* 条件文字列の開放 */
   } else { /* 条件がないとき */
      ncjmp_output(TMPL_BEGIN,ia_loopn[i_looplev]) ;
   }

   if (ia_exitf[i_looplev] == TRUE) {
      tmplabel_output(TMPL_EXIT,ia_loopn[i_looplev]) ; /* exit ラベル */
   }

   i_looplev-- ;

   return 0 ;
}

/* 今 loop ブロックの中か ? */
int in_loopblkp(void)
{
   return i_looplev ;
}
