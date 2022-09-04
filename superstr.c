/*
                         Generic APP Source File

                          Filename : SUPERSTR.C
                          Version  : 95/03/13
                            Written by Mio

                      ややこしい文字列操作の下請け
                        （char* 配列操作関数群）
*/

#include <stdio.h>
#include <string.h>

#include "appdefs.h"
#include "appsub.h"

#include "superstr.h"

#define MAXSTRLEN 256

/* カンマで区切られた文字列から、char *[] 型配列を作成して返す
   （但し、文字列ポインタ配列のサイズは固定長） */
int strtocpa(char *cp,char ***cppp_list,int i_maxlen,char *cp_delim)
{
    char ca_tmp[MAXSTRLEN] ;
    int i = 0 ;

    /* ポインタ配列の確保 */
    *cppp_list = (char **)app_malloc(sizeof(void *)*(i_maxlen+1)) ;

    /* 引き数配列の作成 */
    while ((app_strlen(cp) > 0) && (i < i_maxlen)) {
       cp = gettoken(cp,WHITESPACE,cp_delim,ca_tmp) ;
       cutspc(ca_tmp) ;
       if (app_strlen(ca_tmp) > 0) {
          (*cppp_list)[i++] = app_strdup(ca_tmp) ; /* 配列要素の作成 */
       } else {
          (*cppp_list)[i++] = "" ;                 /* ヌル文字列 */
       }
       if ((*cp != '\0') && (strchr(cp_delim,(int)*cp) != NULL)) {
          cp++ ; /* カンマの次から */
       }
    }

    if (i == 0) {
       app_free(*cppp_list) ; /* 要素がないとき */
       *cppp_list = NULL ;
    } else {
       (*cppp_list)[i] = NULL ; /* デリミタ */
    }

    return i ; /* 要素の個数 */
}

/* char *[] 型引数リストの表示（主にデバッグ用） */
void cpa_print(char *cpa_list[])
{
   int i ;
   for (i = 0 ;cpa_list[i] != NULL;i++) {
      printf("ARG %d:[%s]\n",i,cpa_list[i]) ;
   }
}

/* char *[] 型引数リスト（自分自身、及び配列要素）の開放 */
void cpa_release(char *cpa_list[])
{
   int i ;
   if (cpa_list != NULL) {
      for (i = 0 ;cpa_list[i] != NULL;i++) {
         if (app_strlen(cpa_list[i]) > 0) { /* ヌル文字列でないとき */
            app_free(cpa_list[i]) ; /* リストの要素の開放 */
         }
      }
   }
   app_free(cpa_list) ; /* 自分自身の開放 (NULL の時は この関数内で error) */
}

/* char *[] 型引数リストの内容の同一性チェック */
int cpa_equalp(char *cpa_list1[],char *cpa_list2[])
{
   int i ;

   /* 各要素のうち、一つでも異なっていればペケ */
   for (i = 0 ; cpa_list1[i] != NULL ; i++) {
      if (strcmp(cpa_list1[i],cpa_list2[i]) != 0) {
         return FALSE ;
      }
   }
   /* ２番目のリストの方が長い */
   if (cpa_list2[i] != NULL) {
      return FALSE ;
   }
   /* 完全に一致 */
   return TRUE ;
}
