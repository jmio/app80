/*
                         Generic APP Source File

                          Filename : CPROTO.C
                          Version  : 95/03/03
                            Written by Mio

                  C ソースファイル、プロトタイプメーカ
*/

#include <stdio.h>
#include <string.h>

/* 各 CPU 共通 INCLUDE */
#include "appdefs.h"
#include "appsub.h"
#include "apperr.h"

/* 各 CPU 依存 INCLUDE */
#include "cproto.h"

/* トップファイルの評価 */
int eval_file (char *cp_fname,int i_paramcount,char **cpp_paramstr)
{
   /* 開始（コマンドライン define 等）処理 */
   eval_begin() ;

   /* トップファイルの評価 */
   include_file(cp_fname) ;

   /* 完了処理 */
   eval_exit() ;
   return 0 ;
}

/* 開始処理 */
int eval_begin(void)
{
   app_outstr("/*\n") ;
   app_outstr("    注意：このファイルは自動生成されたものです\n") ;
   app_outstr("*/\n") ;
   app_outstr("\n") ;
   
   return 0 ;
}

/* エラー発生時の終了前処理 (APPSUB.C からコールされる) */
int eval_errexit(int i_errcode)
{
    return i_errcode ;
}

/* 通常の終了前処理 */
int eval_exit(void)
{
    return 0 ;
}


/* 行の評価  eval_line */
int eval_line(char *cp_line)
{
   static char caa_linebuf[LINEBUFS][LINEBUFSIZ] ;
   static long l_lineno = 0 ;

#define i_ml(l) ((int)((l) % LINEBUFS))

   char *cp ;
   int i_lineptr = i_ml(l_lineno) ;
   long l ;

   strcpy(caa_linebuf[i_lineptr],cp_line) ;

   /* 関数の開始行を見つけたら */
   if ((cp_line[0] == '{') && (cp_line[1] == '\n')) {
      for (l = l_lineno-1 ; (l > 0) && (l > l_lineno-LINEBUFS) ; l--) {
         /* static な関数は出力しない */
         if (strstr(caa_linebuf[i_ml(l)],STATIC_KW) != NULL) {
            break ;
         }
         /* コメントまでさかのぼって */
         if (caa_linebuf[i_ml(l)][0] == '/') {
            /* 逆順で出力 */
            for (;l < l_lineno-1;l++) {
               /* プリプロセッサ指令以外を出力 */
               if (caa_linebuf[i_ml(l)][0] != '#') {
                  app_outstr(caa_linebuf[i_ml(l)]) ;
               }
            }
            /* 定義行にセミコロンの付加 */
            if ((cp = strchr(caa_linebuf[i_ml(l)],'\n')) != NULL) {
               strcpy(cp," ;\n") ;
            }
            /* 定義行の出力 */
            app_outstr(caa_linebuf[i_ml(l)]) ;
            app_outstr("\n") ; /* 空白行を開ける */
            break ;
         }
      }
      /* 行バッファオーバーフロー */
      if (l <= l_lineno - LINEBUFS) {
         app_error(CPROTO_ERR1,1) ;
      }
   }

   l_lineno++ ;
   return ;

#undef i_ml
}
