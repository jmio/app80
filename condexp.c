/*
                         Generic APP Source File

                          Filename : CONDEXP.C
                          Version  : 95/03/01
                            Written by Mio

                              条件式処理
*/

#include <stdio.h>
#include <string.h>

#include "appmain.h"
#include "appdefs.h"
#include "appsub.h"
#include "apperr.h"
#include "proto.h"
#include "condexp.h"

static int i_tmpnum = 0 ;          /* 生成されるラベルに付加される番号 */

/* 条件によるコード出力ジャンプテーブル */
typedef void (*coutfuncptr)() ;
typedef struct {
   char *cp ;
   coutfuncptr cop_false ;
   coutfuncptr cop_true ;
} cond_jmptbl ;


/* 条件文字列の分割 */
char *get_args(char *cp_line,
               char *cp_arg1,char *cp_cond,char *cp_arg2,char *cp_rest)
{
   char *cp_save ;

   /* 式の左辺の取り出し */
   cp_line = gettoken(cp_line,WHITESPACE,ARG1_DELIM,cp_arg1) ;

   /* 演算子の取り出し */
   cp_save = cp_line ;
   cp_line = gettoken2(cp_line,WHITESPACE,COND_UDELIM,cp_cond) ;

   /* オーバーラン？ */
   if (strcmp(cp_cond,OP_LOR) == 0 || strcmp(cp_cond,OP_LAND) == 0) {
      cp_cond[0] = '\0' ;
      cp_arg2[0] = '\0' ;
      cp_line = cp_save ;
   } else {
      cp_line = gettoken(cp_line,WHITESPACE,ARG2_DELIM,cp_arg2) ;
   }

   /* 残りの式の取り出し */
   if (cp_rest != NULL) { /* 残りの式が不要の時、ヌルポインタで call */
      cp_line = gettoken(cp_line,WHITESPACE,REST_DELIM,cp_rest) ;
   }
   return cp_line ;
}

/* 条件ジャンプコード出力 */
static
void cond_output(char *cp_1,char *cp_c,char *cp_2,
                 int i_revf,char *cp_label,int i_num)
{
   static cond_jmptbl CONDTBL[] = {
      { OP_EQUAL  , cond_equal  , cond_not_e  } ,
      { OP_EQUAL2 , cond_equal  , cond_not_e  } , /* Mio's APP Compatible */
      { OP_NOT_E  , cond_not_e  , cond_equal  } ,
      { OP_NOT_E2 , cond_not_e  , cond_equal  } , /* Mio's APP Compatible */
      { OP_ABOVE  , cond_above  , cond_b_or_e } ,
      { OP_BELOW  , cond_below  , cond_a_or_e } ,
      { OP_A_OR_E , cond_a_or_e , cond_below  } ,
      { OP_B_OR_E , cond_b_or_e , cond_above  } ,
      { OP_TEST   , cond_not_t  , cond_test   } ,
      { OP_NOT_T  , cond_test   , cond_not_t  } ,
      { OP_NOT_T2 , cond_test   , cond_not_t  } ,
      { OP_AND    , cond_not_a  , cond_and    } ,
      { OP_NOT_A  , cond_and    , cond_not_a  } ,
      { NULL      , NULL        , NULL        } ,
   } ;

   /* フラグ処理用定数 */
   static char *special_tbl[] = { "(flag)" , NULL } ;
   static char *flag_onof[] = {"ON","OFF",NULL} ;
   static char *flag_tbl[] = {"Z","NZ","C","NC","E","NE","P","NP","O","NO",NULL} ;

   int i ;

   /* フラグ (flag) 関係処理 */
   if (app_strlen(cp_c) == 0) { /* フラグチェックか? */
      i = app_scantbl_with_cut(special_tbl,cp_1) ;
      if (i > 0) {
         /* arg1 の分解 */
         cp_1 = gettoken(cp_1,WHITESPACE,FLAG_DELIM,cp_c) ;
         cp_1 = gettoken(cp_1,WHITESPACE,REST_DELIM,cp_2) ;
         cutspc(cp_2) ;
         /* ON or OFF ? */
         if ((i = app_scantbl(flag_onof,cp_2)) == 0) {
            app_error(LARGCOND_ERROR,1) ;
         }
         cond_flags(cp_c,!(i == 1),cp_label,i_num) ;
         return ;
      }

      /* 互換性オプション時のフラグ処理 */
      if ((ia_option['O'] == TRUE) && (app_scantbl(flag_tbl,cp_1) > 0)){
         int i_save = ia_option['N'] ;
         ia_option['N'] = TRUE ;
         jcc_output(cp_1,cp_1,cp_label,i_num) ;
         ia_option['N'] = i_save ;
         return ;
      }
   }

   for (i = 0 ; CONDTBL[i].cp != NULL ; i++ ) {
      /* テーブルの文字列と一致するか？ */
      if (strcmp(CONDTBL[i].cp,cp_c) == 0) { /* 文字列が一致したら */
         if (i_revf == FALSE) {
            CONDTBL[i].cop_false(cp_1,cp_2,cp_label,i_num) ;
         } else {
            CONDTBL[i].cop_true(cp_1,cp_2,cp_label,i_num) ;
         }
         return ;
      }
   }

   app_error(LARGCOND_ERROR,1) ;
}

/* 条件ジャンプ生成処理 */
void eval_cond(char *cp_line,int i_revf,char *cp_label,
               int i_num,int i_level,int i_lpf)
{
   char ca_arg1[ARGSTRMAX],         /* 条件式の左辺 */
        ca_cond[ARGSTRMAX],         /* 条件演算子 */
        ca_arg2[ARGSTRMAX],         /* 条件式の右辺 */
        ca_rest[ARGSTRMAX * 5] ;    /* 以降の式 */
   int  i_mktmpf = FALSE ;          /* テンポラリ・ラベル生成フラグ */
   char *cplor,*cpland ;

   /* 条件式の切り出し */
   cp_line = get_args(cp_line,ca_arg1,ca_cond,ca_arg2,ca_rest) ;
   cutspc(ca_arg1) ;
   cutspc(ca_arg2) ;
   cutspc(ca_rest) ; /* 残りの式からごみをカット */

   /* 残りの式があるとき */
   if (app_strlen(ca_rest) > 0) {
      cplor  = strstr(ca_rest,OP_LOR) ;  /* OR  をスキャン */
      cpland = strstr(ca_rest,OP_LAND) ; /* AND をスキャン */

      if (i_lpf == TRUE) { /* ループの終わりの AND , OR は対偶条件 */
         char *cptmp = cplor ;
         cplor  = cpland ;
         cpland = cptmp ;
      }

      if (cplor == ca_rest) { /* 論理 OR を発見 */
         if (cpland != NULL) {
            app_error(LANDOR_ERROR,1) ; /* OR と AND が混ざっている */
         }
         if (i_level == 0) { /* OR の先頭のとき */
            eval_cond(&ca_rest[2],!i_revf,TMPL_COND,i_tmpnum,i_level+1,i_lpf) ;
            i_mktmpf = TRUE ;
         } else {
            eval_cond(&ca_rest[2],i_revf,TMPL_COND,i_tmpnum,i_level+1,i_lpf) ;
         }
      } else if (cpland == ca_rest) { /* 論理 AND を発見 */
         if (cplor != NULL) {
            app_error(LANDOR_ERROR,1) ; /* AND と OR が混ざっている */
         }
         eval_cond(&ca_rest[2],i_revf,cp_label,i_num,i_level+1,i_lpf) ;
      } else {
         app_error(LEXTRA_ERROR,1) ;    /* お尻にごみがついている */
      }
   }

   /* ここに関数呼び出しのチェックを入れる */
   /* ここにインクリメント／ディクリメントチェックを入れる */

   if (app_strlen(ca_arg1) == 0) {
      app_error(LARG1MISS_ERROR,1) ;
   }

   if (app_strlen(ca_cond) > 3) { /* 条件エラー */
      app_error(LARGCOND_ERROR,1) ;
   }

   /* 条件ジャンプの出力 */
   cond_output(ca_arg1,ca_cond,ca_arg2,i_revf,cp_label,i_num) ;

   /* テンポラリ・ラベルの生成 */
   if (i_mktmpf == TRUE) {
      tmplabel_output(TMPL_COND,i_tmpnum++) ; /* ラベル出力 */
   }
}

/* 無条件ジャンプの出力 */
void ncjmp_output(char *cp_lstr,int i_num)
{
   char ca_label[LBLBFLEN] ;

   sprintf(ca_label,"%s_%d",cp_lstr,i_num) ; /* ラベル文字列生成 */
   short_jmp_output(ca_label) ;
}

/* テンポラリ・ラベルの生成 */
void tmplabel_output(char *cp_lstr,int i_num)
{
   char ca_label[LBLBFLEN] ;

   sprintf(ca_label,"%s_%d",cp_lstr,i_num) ; /* ラベル文字列生成 */
   label_output(ca_label) ;
}
