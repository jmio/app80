/*
                         Generic APP Source File

                          Filename : COND86.C
                          Version  : 95/03/01
                            Written by Mio

                          条件ジャンプ出力処理
*/

#include <stdio.h>
#include <string.h>

#include "appmain.h"
#include "appdefs.h"
#include "appsub.h"
#include "apperr.h"
#include "code86.h"

#include "cond86.h"

static int i_nnum = 0 ;         /* 擬似 NEAR ジャンプ生成ラベルカウンタ */

/* 条件ジャンプ出力 */
void jcc_output(char *cp_s,char *cp_n,char *cp_label,int i_num)
{
   if (ia_option['N']) { /* near ジャンプ */
      char ca_tmp1[LBLBFLEN],ca_tmp2[LBLBFLEN] ;
      sprintf(ca_tmp1,"%s_%d",TMPL_NEAR,i_nnum++) ; /* テンポラリラベル生成 */
      sprintf(ca_tmp2,"%s_%d",cp_label,i_num) ;
      app_printf("\tJ%s\t%s\n",cp_n,ca_tmp1) ;   /*       JZ  <FALSE> */
      jmp_output(ca_tmp2) ;                      /*       JMP <TRUE>  */
      label_output(ca_tmp1) ;                    /* <FALSE>:          */
   } else {          /* short ジャンプ */
      app_printf("\tJ%s\t%s_%d\n",cp_s,cp_label,i_num) ;
   }
}

/* cp_1 と cp_2 を 比較した後、equal 条件でジャンプするコード */
void cond_equal(char *cp_1,char *cp_2,char *cp_label,int i_num)
{
   cmp_output(cp_1,cp_2) ;
   jcc_output("E","NE",cp_label,i_num) ;
}

/* cp_1 と cp_2 を 比較した後、not equal 条件でジャンプするコード */
void cond_not_e(char *cp_1,char *cp_2,char *cp_label,int i_num)
{
   cmp_output(cp_1,cp_2) ;
   jcc_output("NE","E",cp_label,i_num) ;
}

/* cp_1 と cp_2 を 比較した後、below 条件でジャンプするコード */
void cond_below(char *cp_1,char *cp_2,char *cp_label,int i_num)
{
   cmp_output(cp_1,cp_2) ;
   if (ia_option['S']) {
      jcc_output("L","NL",cp_label,i_num) ;
   } else {
      jcc_output("B","NB",cp_label,i_num) ;
   }
}

/* cp_1 と cp_2 を 比較した後、below or equal 条件でジャンプするコード */
void cond_b_or_e(char *cp_1,char *cp_2,char *cp_label,int i_num)
{
   cmp_output(cp_1,cp_2) ;
   if (ia_option['S']) {
      jcc_output("LE","NLE",cp_label,i_num) ;
   } else {
      jcc_output("BE","NBE",cp_label,i_num) ;
   }
}

/* cp_1 と cp_2 を 比較した後、above 条件でジャンプするコード */
void cond_above(char *cp_1,char *cp_2,char *cp_label,int i_num)
{
   cmp_output(cp_1,cp_2) ;
   if (ia_option['S']) {
      jcc_output("G","NG",cp_label,i_num) ;
   } else {
      jcc_output("A","NA",cp_label,i_num) ;
   }
}

/* cp_1 と cp_2 を 比較した後、above or equal 条件でジャンプするコード */
void cond_a_or_e(char *cp_1,char *cp_2,char *cp_label,int i_num)
{
   cmp_output(cp_1,cp_2) ;
   if (ia_option['S']) {
      jcc_output("GE","NGE",cp_label,i_num) ;
   } else {
      jcc_output("AE","NAE",cp_label,i_num) ;
   }
}

/* cp_1 と cp_2 を test した後、zero 条件でジャンプするコード */
void cond_test(char *cp_1,char *cp_2,char *cp_label,int i_num)
{
   test_output(cp_1,cp_2) ;
   jcc_output("Z","NZ",cp_label,i_num) ;
}

/* cp_1 と cp_2 を test した後、not zero 条件でジャンプするコード */
void cond_not_t(char *cp_1,char *cp_2,char *cp_label,int i_num)
{
   test_output(cp_1,cp_2) ;
   jcc_output("NZ","Z",cp_label,i_num) ;
}

/* cp_1 と cp_2 を and した後、zero 条件でジャンプするコード */
void cond_and(char *cp_1,char *cp_2,char *cp_label,int i_num)
{
   and_output(cp_1,cp_2) ;
   jcc_output("Z","NZ",cp_label,i_num) ;
}

/* cp_1 と cp_2 を and した後、not zero 条件でジャンプするコード */
void cond_not_a(char *cp_1,char *cp_2,char *cp_label,int i_num)
{
   and_output(cp_1,cp_2) ;
   jcc_output("NZ","Z",cp_label,i_num) ;
}

/* フラグによるジャンプ */
void cond_flags(char *cp_1,int i_tf,char *cp_label,int i_num)
{
   app_printf("\t%s%s\t%s_%d\n",(i_tf?"J":"JN"),cp_1,cp_label,i_num) ;
}
