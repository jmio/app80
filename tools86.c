/*
                         Generic APP Source File

                          Filename : TOOLS86.C
                          Version  : 95/03/14
                            Written by Mio

                           APP86 用ツール集
*/

#include <stdio.h>
#include <string.h>

#include "appdefs.h"
#include "appsub.h"
#include "apperr.h"
#include "tools86.h"

#define MAXVARLEN 80                 /* 項、変数名最大長 */

/* ワードレジスタかどうか調べる */
int is_wordreg(char *cp)
{
   static char *cpa_regs86word[] = {"AX","BX","CX","DX",
                                    "SI","DI","BP", NULL } ;
   int i ;

   /* NULL 文字列 ? */
   if (cp[0] == '\0') {
      return 0 ;
   }

   /* ワードレジスタか？ */
   if ((i = app_scantbl(cpa_regs86word,cp)) != 0) {
      return i + WORDreg ;
   }
   return 0 ;
}

/* バイトレジスタかどうか調べる */
int is_bytereg(char *cp)
{
   static char *cpa_regs86byte[] = {"AL","AH","BL","BH",
                                    "CL","CH","DL","DH", NULL } ;
   int i ;

   /* NULL 文字列 ? */
   if (cp[0] == '\0') {
      return 0 ;
   }

   /* ワードレジスタか？ */
   if ((i = app_scantbl(cpa_regs86byte,cp)) != 0) {
      return i ;
   }
   return 0 ;
}

/* (byte),(word) キーワードの検査
   (byte) ... "(byte)"文字列を削除して 1 を返す
   (word) ... "(word)"文字列を削除して 2 を返す
   どちらも見つからない時 0 を返す   
*/
int cast_eval(char *cp)
{
   static char *cpa_wordbyte[] = {"(byte)","(word)","(BYTE)","(WORD)",NULL} ;
   return (int)app_scantbl_with_cut(cpa_wordbyte,cp) ;
}

/* ++,-- キーワードの検査 */
int incdec_eval(char *cp_ptr,char *cp_var,char *cp_incdec,void (*outfunc)())
{
   char *cp_i ;
   char ca_buf[MAXVARLEN] ;

   /* ++,-- の位置を調べる */
   cp_i = strstr(cp_var,cp_incdec) ;
   if (cp_i != NULL) {
      if ((cp_i != cp_var) && (cp_i[app_strlen(cp_incdec)] != '\0')) { 
         /* 先頭でなく、末尾でもないときアドレス指定とみなす e.g. [++BX] 等 */
         gettoken(cp_i+2,WHITESPACE,"])",ca_buf) ;
         str_replace(cp_var,cp_incdec,"") ; /* ++,-- の削除 */
      } else {
         str_replace(cp_var,cp_incdec,"") ; /* ++,-- の削除 */
         strcpy(ca_buf,cp_ptr) ;
         strcat(ca_buf,cp_var) ;
      }
      outfunc(ca_buf) ;
   }

   return 0 ;
}

/* += , -= キーワードの検査 */
int addsub_eval(char *cp_ptr,char *cp_var,char *cp_addsub,void (*outfunc)())
{
   char *cp_i ;
   char ca_buf1[MAXVARLEN] ,
        ca_buf2[MAXVARLEN] ;

   /* +=,-= の位置を調べる */
   cp_i = strstr(cp_var,cp_addsub) ;
   if (cp_i != NULL) {
      /* 変数の切り出し */
      cp_var = gettoken(cp_var,WHITESPACE,cp_addsub,ca_buf2) ;
      cutspc(ca_buf2) ;
      if (app_strlen(ca_buf2) == 0) {
         app_error(EXP_SYNTAX_ERROR,1) ;
      }
      strcpy(ca_buf1,cp_ptr) ; /* ポインタ文字列 */
      strcat(ca_buf1,ca_buf2) ; /* 変数本体 */

      /* 増分の切り出し */
      gettoken(cp_i+2,WHITESPACE,LDELIM,ca_buf2) ;
      cutspc(ca_buf2) ;
      if (app_strlen(ca_buf2) == 0) {
         app_error(EXP_SYNTAX_ERROR,1) ;
      }
      outfunc(ca_buf1,ca_buf2) ;

      /* += , -= の削除 */
      str_replace(cp_var,cp_addsub,"") ;
   }

   return 0 ;
}
