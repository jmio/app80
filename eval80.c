/*
                         Generic APP Source File

                          Filename : EVAL80.C
                          Version  : 95/03/01
                            Written by Mio

                  APP80 向け、評価ルーチンベースファイル
*/

#include <stdio.h>
#include <string.h>

#include "appmain.h"
#include "appdefs.h"
#include "appsub.h"

#include "ifdefs.h"              /* 条件 Pre Processing 処理 */
#include "ifswitch.h"            /* 条件分岐処理 */
#include "miscloop.h"            /* 各種ループ処理 */
#include "function.h"            /* 関数処理 */
#ifdef APP86
# include "misc86.h"             /* x86 依存命令の処理 */
#else
# include "misc80.h"             /* x80 依存命令の処理 */
#endif

#include "eval80.h"

/* ジャンプテーブルの構造 */
typedef int (*intfunc_ptr)(char *) ;
typedef struct {
   char        *cp ; /* 予約語文字列 */
   intfunc_ptr ifp ; /* 処理ルーチン */
   int         idf ; /* ifdef による disable フラグ */
   int         ihf ; /* ヘッダ作成時コールフラグ */
} jmptable ;

/*
   ジャンプテーブル本体 :
   追加する際は、予約語、処理ルーチンの組を以下のテーブルの末尾{NULL,NULL}
   より手前の位置に、同じ形式で追加すること。
*/
const jmptable JMPTBL[] = {
  /* 予約語名 , 関数名 , ifdefフラグ , ヘッダ作成モードフラグ */
  /*  ifdef フラグ           ON = ifdef による disable フラグに左右されない */
  /*  ヘッダ作成モードフラグ ON = ヘッダ作成時にもコールされる */
  { "ifdef"       , eval_ifdef       , ON  , OFF } ,
  { "ifndef"      , eval_ifndef      , ON  , OFF } ,
  { "elsedef"     , eval_elsedef     , ON  , OFF } ,
  { "endifdef"    , eval_endifdef    , ON  , OFF } ,
  { "define"      , eval_define      , OFF , OFF } ,
  { "include"     , eval_include     , OFF , ON  } ,
  { "header"      , eval_include     , OFF , OFF } ,
  { "warning"     , eval_warning     , OFF , OFF } ,
#ifdef IFSWITCH_H
  { "if"          , eval_if          , OFF , OFF } ,
  { "else"        , eval_else        , OFF , OFF } ,
  { "endif"       , eval_endif       , OFF , OFF } ,
  { "switch"      , eval_switch      , OFF , OFF } ,
  { "case"        , eval_case        , OFF , OFF } ,
  { "default"     , eval_default     , OFF , OFF } ,
  { "endsw"       , eval_endsw       , OFF , OFF } ,
#endif
#ifdef MISCLOOP_H
  { "while"       , eval_while       , OFF , OFF } ,
  { "wend"        , eval_wend        , OFF , OFF } ,
  { "repeat"      , eval_repeat      , OFF , OFF } ,
  { "until"       , eval_until       , OFF , OFF } ,
  { "continue"    , eval_continue    , OFF , OFF } ,
  { "exit"        , eval_exit        , OFF , OFF } ,
  { "let"         , eval_let         , OFF , OFF } ,
  { "for"         , eval_for         , OFF , OFF } ,
  { "next"        , eval_next        , OFF , OFF } ,
# ifdef MISC86_H
  { "loopcx"      , eval_loopcx      , OFF , OFF } ,
  { "endl"        , eval_endl        , OFF , OFF } ,
# endif
# ifdef MISC80_H
  { "loop"        , eval_loop        , OFF , OFF } ,
  { "endl"        , eval_endl        , OFF , OFF } ,
# endif
#endif
#ifdef FUNCTION_H
  { "function"    , eval_function    , OFF , ON  } ,
  { "function_h"  , eval_function_h  , OFF , OFF } ,
  { "procedure"   , eval_procedure   , OFF , ON  } ,
  { "procedure_h" , eval_procedure_h , OFF , OFF } ,
  { "endf"        , eval_endf        , OFF , OFF } ,
  { "endp"        , eval_endp        , OFF , OFF } ,
  { "endpi"       , eval_endpi       , OFF , OFF } ,
  { "func"        , eval_func        , OFF , OFF } ,
  { "proc"        , eval_proc        , OFF , OFF } ,
  { "return"      , eval_return      , OFF , OFF } ,
  { "public"      , eval_public      , OFF , OFF } ,
  { "extrn"       , eval_extrn       , OFF , OFF } ,
  { "extern"      , eval_extrn       , OFF , OFF } ,
#endif
#ifdef MISC86_H
  { "short"       , eval_short       , OFF , OFF } ,
  { "near"        , eval_near        , OFF , OFF } ,
  { "signed"      , eval_signed      , OFF , OFF } ,
  { "unsigned"    , eval_unsigned    , OFF , OFF } ,
#endif
#ifdef MISC80_H
  { "short"       , eval_short       , OFF , OFF } ,
  { "near"        , eval_near        , OFF , OFF } ,
  { "cpu"         , eval_cpu         , OFF , OFF } ,
#endif
  { NULL          , NULL             , OFF , OFF } ,
} ;

/* トップファイルの評価 */
int eval_file (char *cp_fname,int i_paramcount,char **cpp_paramstr)
{
   /* 開始（コマンドライン define 等）処理 */
   eval_begin(i_paramcount,cpp_paramstr) ;

   /* トップファイルの評価 */
   include_file(cp_fname) ;

   /* 完了処理 */
   eval_normalexit() ;
   return 0 ;
}

/* 開始処理 */
int eval_begin(int i_paramcount,char **cpp_paramstr)
{
   int i ;

   for (i = 0;i < i_paramcount;i++) {
      char c = toupper(cpp_paramstr[i][0]) ;
      switch(c) {
      case INCLUDE_OPTION:
         add_incpath(&cpp_paramstr[i][1]) ;
      break ;
      case DEFINE_OPTION:
         sympush(&cpp_paramstr[i][1]) ;
      break ;
      }
   }
   return 0 ;
}

/* エラー発生時の終了前処理 (APPERR.C からコールされる) */
int eval_errexit(int i_errcode)
{
    /* 内部定義シンボルテーブルの開放 */
    symclear() ;

#ifdef FUNCTION_H
    /* 関数定義テーブルの開放 */
    exit_function() ;
#endif

    return i_errcode ;
}

/* 通常の終了前処理 */
int eval_normalexit()
{
    /* 内部定義シンボルテーブルの開放 */
    symclear() ;

    /* インクルードパス領域の開放 */
    clear_incpath() ;

#ifdef FUNCTION_H
    /* 関数定義テーブルの開放 */
    exit_function() ;
#endif

    return 0 ;
}

/* APP 行以外の出力 (ifdef 制御込み) */
static
void normal_output(char *cp)
{
   if (ia_option['H'] == FALSE) {
#if defined(IFSWITCH_H) && defined(MISCLOOP_H)
      if ((in_ifblkp() > 0) || (in_loopblkp() > 0)) {
         if (*cp == '\t') {
            do { cp++ ; } while (*cp == '\t') ;
            cp -- ;
         }
      }
#endif
      if (output_enable() == ON) {
         app_outstr(cp) ;
      }
   }
}

/* APP 予約語のコメントアウト */
static
void comment_output(char *cp)
{
   if (ia_option['H'] == FALSE) {
      if (*cp == '\t') {
         do { cp++ ; } while (*cp == '\t') ;
         cp -- ;
      }
      app_printf("\t;%s",cp) ;
   }
}

/* 行の評価  eval_line */
int eval_line(char *cp_line)
{
   char ca_resw[RESWBUFSIZ] ; /* 切り出した予約語 */
   char *cp_orig = cp_line ;  /* 行バッファ位置のセーブ用 */

   int i ;

   /* 予約語らしきものがあるか？ */
   if ((cp_line = get_appresw(cp_line,ca_resw)) == NULL) {
      normal_output(cp_orig) ; /* なければそのまま出力 */
      return ;
   }

   /* ジャンプテーブルのスキャン、及び解釈 */
   for (i = 0 ; JMPTBL[i].cp != NULL ; i++ ) {
      /* ヘッダ作成モード時、関係無いコマンドは無視 */
      if ((ia_option['H'] == TRUE) && (JMPTBL[i].ihf == OFF)) {
         continue ;
      }
      /* ifdef で disable かつ、ifdef 系のコマンド以外の時無視 */
      if ((output_enable() == OFF) && (JMPTBL[i].idf == OFF)) {
         continue ;
      }
      /* テーブルの文字列と一致するか？ */
      if (strcmp((JMPTBL[i].cp),ca_resw) == 0) { /* 文字列が一致したら */
         comment_output(cp_orig) ;               /* コメント出力 */
         (void)JMPTBL[i].ifp(cp_line) ;          /* ジャンプ */
         break ;
      }
   }

   /* アンダーバーのあとは予約語以外だった? */
   if (JMPTBL[i].cp == NULL) {
      normal_output(cp_orig) ; /* そのまま出力 */
      return ;
   }
   return ;
}
