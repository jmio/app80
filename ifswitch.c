/*
                         Generic APP Source File

                          Filename : IFSWITCH.C
                          Version  : 95/03/01
                            Written by Mio

                             条件分岐処理
*/

#include <stdio.h>
#include <string.h>

#include "appmain.h"
#include "appdefs.h"
#include "appsub.h"
#include "apperr.h"
#include "proto.h"
#include "condexp.h"
#include "ifswitch.h"

static int i_iflev = 0 ;           /* 条件ブロックネスティングレベル */
static int i_ifnum = 0 ;           /* 生成されるラベルに付加される番号 */
static int i_swlev = 0 ;           /* _switch ブロックネスティングレベル */

static int i_delayflag = FALSE ;   /* ジャンプの生成を遅らせるためのフラグ */

static int ia_rifn[IFNESTMAX]  ;   /* _if ラベル番号スタック */
static int ia_felse[IFNESTMAX] ;   /* _else 検出スタック */
static int ia_cactr[SWNESTMAX] ;   /* _switch ネストスタック */

/* _if の処理 */
int eval_if(char *cp_line)
{
   if (++i_iflev >= IFNESTMAX) {
      app_error(IF_STK_OVER,1) ;  /* _if ネスティングオーバー */
   }
   ++i_ifnum ;
   ia_rifn[i_iflev]  = i_ifnum ;
   ia_felse[i_iflev] = FALSE ;
   eval_cond(cp_line,TRUE,TMPL_ELSE,i_ifnum,0,FALSE) ;

   return 0 ;
}
/* _else の処理 */
#pragma argsused
int eval_else(char *cp_line)
{
   if (i_iflev == 0) {
      app_error(IF_STK_EMPTY,1) ;             /* _if がないのに _else 検出 */
   }
   ia_felse[i_iflev] = TRUE ;
   if (i_delayflag == FALSE) {
      ncjmp_output(TMPL_ENDIF,ia_rifn[i_iflev]) ; /* _endif へのジャンプ */
   }
   tmplabel_output(TMPL_ELSE,ia_rifn[i_iflev]) ; /* ラベル出力 */

   return 0 ;
}

/* _endif の処理 */
#pragma argsused
int eval_endif(char *cp_line)
{
   if (i_iflev == 0) {
      app_error(IF_STK_EMPTY,1) ;        /* _if がないのに _endif 検出 */
   }

   if (ia_felse[i_iflev] == TRUE) {
      tmplabel_output(TMPL_ENDIF,ia_rifn[i_iflev]) ; /* ラベル出力 */
   } else {
      tmplabel_output(TMPL_ELSE,ia_rifn[i_iflev]) ; /* ラベル出力 */
   }
   i_iflev-- ;

   return 0 ;
}

/* _switch の処理 */
#pragma argsused
int eval_switch(char *cp_line)
{
   if (++i_swlev >= SWNESTMAX) {
      app_error(SW_STK_OVER,1) ; /* _switch のネストが深すぎる */
   }
   ia_cactr[i_swlev] = 0 ; /* _case カウンタのクリア */
   return 0 ;
}

/* _case の処理 */
int eval_case(char *cp_line)
{
   if (i_swlev == 0) {
      app_error(SW_STK_EMPTY,1) ; /* _switch のない _case */
   }
   if (ia_cactr[i_swlev] > 0) {
      (void)eval_else(cp_line) ; /* ２回目以降は _elseif */
   }
   ia_cactr[i_swlev]++ ; /* case カウンタ++ */
   return eval_if(cp_line) ; /* _if の処理 */
}

/* _default の処理 */
int eval_default(char *cp_line)
{
   if (i_swlev == 0) {
      app_error(SW_STK_EMPTY,1) ; /* _switch のない _default */
   }
   return eval_else(cp_line) ; /* _else の処理 */
}

/* _endsw の処理 */
int eval_endsw(char *cp_line)
{
   if (i_swlev == 0) {
      app_error(SW_STK_EMPTY,1) ; /* _switch のない _endsw */
   }
   while (ia_cactr[i_swlev]-- > 0) {
      (void)eval_endif(cp_line) ;
   }
   i_swlev-- ;
   return 0 ;
}

/* 今 if ブロックの中か ? */
int in_ifblkp(void)
{
   return i_iflev ;
}
