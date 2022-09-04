/*
                         Generic APP Source File

                          Filename : APPMAIN.C
                          Version  : 95/03/01
                            Written by Mio

                    Generic APP メインルーチンファイル
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "appmain.h"
#include "appsub.h"
#include "appdefs.h"
#include "helpmsg.h"

/* グローバル変数 */
FILE *fp_out ;                      /* 出力ファイル */
int ia_option[256] ;                 /* オプション on/off フラグ */

#ifdef __MSDOS__
/* HELPメッセージ（本当は関数ではない） */
extern far int HELPMSG() ;
#else
#define far
#endif

/* ファイルの評価 (EVALxx.C に存在) */
int eval_file (char *cp_fname,int i_paramconut,char **cpp_paramstr) ;

/* メインルーチン */
int main(int argc,char **argv)
{
   char *cpa_fname[MAXFNAMES] ,        /* ファイル名パラメータ */
        *cpa_paramstr[MAXPARAMS] ;     /* ファイル名以外のパラメータ */
   int  i ,
        i_fnames ,                     /* ファイル名パラメータ数 */
        i_paramcount ;                 /* ファイル名以外のパラメータ数 */

   /* タイトル表示 */
   printf("%s %s %s\n",GAPP_TITLE,GAPP_VERSION,GAPP_COPYRIGHT) ;

   /* ファイル名用固定長領域の確保 */
   for (i = 0 ; i < MAXFNAMES ; i++) {
      cpa_fname[i] = app_malloc(PNLENGTH) ;
   }

   /* コマンドラインパラメータの処理 */
   i_fnames = getparam(argc,argv,cpa_fname,cpa_paramstr,&i_paramcount) ;

#ifdef __MSDOS__
   /* メモリ残量の表示 */
   printf("Available memory %ld bytes\n",(long)app_avmem()) ;
#endif

   /* ファイル名が指定されていない時の処理 */
   if (i_fnames == 0) {
      strcpy(cpa_fname[0],"CON") ;
      strcpy(cpa_fname[1],"CON") ;
   } else {
      /* 拡張子の取り扱い */
      if (getdotpos(cpa_fname[0]) == NULL) { 
         /* 入力ファイル名に拡張子がないとき */
         strcat(cpa_fname[0],DEFAULT_IN_EXT) ;
      }
      /* 出力ファイル名の省略 */
      if (i_fnames == 1) {
         char *cp ;
         /* 拡張子まで切り出して .ASM を追加 */
         strcpy(cpa_fname[1],cpa_fname[0]) ;
         cp = getdotpos(cpa_fname[1]) ;
         if (cp != NULL) {
            *cp = '\0' ; /* 拡張子以降をカット */
         }
         if (ia_option['H']) {
            strcat(cpa_fname[1],DEFAULT_HD_EXT) ;
         } else {
            strcat(cpa_fname[1],DEFAULT_OUT_EXT) ;
         }
      }
   }

   /* 出力ファイルのオープン */
   fp_out = open_file(cpa_fname[1],"wt") ;

   /* 入力ファイルの評価 */
   eval_file(cpa_fname[0],i_paramcount,cpa_paramstr) ;

   /* ファイル名用固定長領域の開放 */
   for (i = 0 ; i < MAXFNAMES ; i++) {
      app_free(cpa_fname[i]) ;
   }

   /* オプション用可変長領域の開放 */
   for (i = 0 ; i < i_paramcount ; i++) {
      app_free(cpa_paramstr[i]) ;
   }

   /* 出力ファイルのクローズ */
   fclose(fp_out) ;
   return 0 ;
}

/* コマンドライン・パラメータの解釈 */
int getparam(int argc,char **argv,char **cpp,char **cpp_o,int *i_opc)
{
   int i,ii ,
       i_fpc = 0,           /* オプション以外のパラメータカウンタ */
       i_result = 0;        /* too many args チェック用パラメータカウンタ */
   char c ;

   /* オプション配列のクリア */
   for (i = '\0' ; i <= '\xff' ; i++) {
      ia_option[i] = OFF ;
   }

   /* オプションパラメータカウンタクリア */
   *i_opc = 0 ;

   /* パラメータのスキャン */
   for (i = 1;i < argc;i++) {
      if (argv[i][0] == '-') {

         for (ii = 1;ii < app_strlen(argv[i]);ii++) {
            c = toupper(argv[i][ii]) ;
            switch(c) {
            case HELP_OPTION:
               printf("%s\n",(char far *)HELPMSG) ;
               exit(0) ;
            case INCLUDE_OPTION:
            case DEFINE_OPTION:
               if (*i_opc < MAXPARAMS) {
                  /* 文字列オプションはその場で領域を確保してコピー */
                  cpp_o[(*i_opc)++] = app_strdup(&(argv[i][ii])) ;
                  ii = app_strlen(argv[i]) ; /* break of for */
                  continue ; /* break */
               }
            break ;
            default:
               ia_option[c] = ON ;
            }
         }
      } else {
         i_result++ ;
         if (i_fpc < MAXFNAMES) { 
            /* ファイル名はファイル名用固定長領域にコピー */
            strcpy(cpp[i_fpc++],argv[i]) ;
         }
      }
   }
   return i_result ;
}
