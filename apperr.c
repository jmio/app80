/*
                         Generic APP Source File

                          Filename : APPERR.C
                          Version  : 95/03/01
                            Written by Mio

                          APP 共通エラー処理
*/

#include <stdio.h>
#include <string.h>

#include "appdefs.h"
#include "appsub.h"
#include "apperr.h"

long l_err_lineno ;
char *cp_err_fname ;

/* ----------------------------------------------------------- */

/* 各 CPU 別、エラー発生時後処理ルーチン (EVALxx.C に存在) */
int eval_errexit(int) ;

/* ----------------------------------------------------------- */

/* エラー処理 */
void app_error(int i_errcode,int i_isfatal)
{
   if (l_err_lineno > 0) {
      fprintf(_erout,"%s(%ld):",cp_err_fname,(long)l_err_lineno) ;
   }

   switch (i_errcode) {
   case ERR_NULL_STR:
      fputs("メモリアロケーション・エラー(Null Pointer => strlen)\n",_erout) ;
   break ;
   case FILE_NOT_FOUND:
      fputs("インクルード・ファイルが見つかりません:",_erout) ;
   break ;
   case TOO_MANY_ARG:
      fputs("コマンドライン引数が多すぎます\n",_erout) ;
   break ;

   case IF_STK_OVER:
      fputs("if のネスティングが深すぎます\n",_erout) ;
   break ;
   case IF_STK_EMPTY:
      fputs("if と endif の数が合いません\n",_erout) ;
   break ;

   case SW_STK_OVER:
      fputs("switch のネスティングが深すぎます\n",_erout) ;
   break ;
   case SW_STK_EMPTY:
      fputs("switch ブロック内命令がブロック外にあります\n",_erout) ;
   break ;

   case LOOP_STK_OVER:
      fputs("ループのネスティングが深すぎます\n",_erout) ;
   break ;
   case LOOP_STK_EMPTY:
      fputs("ループの対応が取れていません\n",_erout) ;
   break ;
   case NOT_IN_LOOP:
      fputs("ループブロック外にループ内命令があります\n",_erout) ;
   break ;
   case FOR_ARGUMENT:
      fputs("for の引数の数がちがいます\n",_erout) ;
   break ;

   case NO_ENDIF:
      fputs("対応の取れていない if/switch 文があります\n",_erout) ;
   break ;
   case NO_ENDLOOP:
      fputs("対応の取れていないループ文があります\n",_erout) ;
   break ;

   case REP_ERROR:
      fputs("括弧の対応が取れていません\n",_erout) ;
   break ;
   case QUOTE_ERROR:
      fputs("クオートが閉じられていません\n",_erout) ;
   break ;

   case LANDOR_ERROR:
      fputs(" || と && の混在はできません\n",_erout) ;
   break ;
   case LEXTRA_ERROR:
   case LARG1MISS_ERROR:
   case LARGCOND_ERROR:
      fputs("条件式の文法が正しくありません\n",_erout) ;
   break ;
   case EXP_SYNTAX_ERROR:
      fputs("式の文法が正しくありません\n",_erout) ;
   break ;

   case LVALUE_ERROR:
      fputs("条件式の左辺に適切でない値があります\n",_erout) ;
   break ;
   case ARGTYPE_ERROR:
      fputs("条件式の両辺の型を指定してください\n",_erout) ;
   break ;
   case TYPE_NOT_MATCH:
      fputs("条件式の両辺の型が異なっています\n",_erout) ;
   break ;

   case SELF_INCLUDE:
      fputs("自分自身をインクルードしようとしています\n",_erout) ;
   break ;

   case FNAME_ERROR:
      fputs("関数名が正しくありません\n",_erout) ;
   break ;
   case FUNDUP_ERROR:
      fputs("既に同名の関数が定義済みです\n",_erout) ;
   break ;
   case FUNDEF_ERROR:
      fputs("引数形式の異なる同名の定義があります\n",_erout) ;
   break ;
   case ENDF_ERROR:
      fputs("関数の外に関数内命令があります\n",_erout) ;
   break ;
   case INFUNC_ERROR:
      fputs("関数を入れ子にすることはできません\n",_erout) ;
   break ;
   case FUNARG_ERROR1:
      fputs("引数の数が違います\n",_erout) ;
   break ;
   case FUNARG_ERROR2:
      fputs("関数の型(func,proc)が違います\n",_erout) ;
   break ;
   case FUNUNDEF_ERROR:
      fputs("未定義の関数を呼び出そうとしています\n",_erout) ;
   break ;
   case RETARG_ERROR:
      fputs("procedure に戻り値は不要です\n",_erout) ;
   break ;

   case IFD_STK_OVER:
      fputs("if(n)def のネスティングが深すぎます\n",_erout) ;
   break ;
   case IFD_STK_EMPTY:
      fputs("if(n)def と endifdef の数が合いません\n",_erout) ;
   break ;

   case STRDUP_FAIL:
      fputs("メモリアロケーション・エラー(Fixlen Str too short)\n",_erout) ;
   break ;
   case FREE_NULL:
      fputs("メモリアロケーション・エラー(Null Pointer => free)\n",_erout) ;
   break ;
   case MEMORY_NOT_ENOUGH:
      fputs("メモリアロケーション・エラー(Not Enough Memory)\n",_erout) ;
   break ;

   case CPROTO_ERR1:
      fputs("行バッファにおさまらない定義があります\n",_erout) ;
   break ;
   case NOT_SUPPORTED:
      fputs("この操作はサポートされていません\n",_erout) ;
   break ;

   default:
      if (i_isfatal > 0) {
        fprintf(_erout,"未定義のエラーが発生しました(errcode=%d)\n",i_errcode);
      }
   }

   eval_errexit(i_errcode) ; /* EVALxx.C 内のエラー後処理を呼び出す */
   if (i_isfatal == 0) {
      return ; /* 後のメッセージの表示を perror 等に任せるとき */
   }
   exit(i_errcode) ;
}

/* ワーニング処理 */
void app_warning(char *cp)
{
   if (l_err_lineno > 0) {
      fprintf(_erout,"%s(%ld):",cp_err_fname,(long)l_err_lineno) ;
   }
   fputs(cp,_erout) ;
   return ;
}

