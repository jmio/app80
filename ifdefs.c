/*
                         Generic APP Source File

                          Filename : IFDEFS.C
                          Version  : 95/03/01
                            Written by Mio

            各機種向け、シンボル定義／インクルード関係処理下請け
*/

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "appdefs.h"
#include "appsub.h"
#include "apperr.h"
#include "proto.h"
#include "ifdefs.h"

/* _define で定義される APP 内部シンボルを記憶しておくための
   シンボルリストの構造 */
typedef struct SYMLIST {
   char *cp_symbol ;           /* シンボル文字列(malloc される) */
   void *cp_value ;            /* シンボルの持つ値(将来の拡張用) */
   struct SYMLIST *sp_next ;   /* 次を差すポインタ */
} symlist ;

static int i_out_enable_flag  = ON ;    /* ifdef による出力 enable フラグ */

static int ia_ifdstk[IFDSTKSIZ] ;       /* ifdef ネスティングスタック */
static int i_ifdlev           = 0 ;     /* ifdef のネスティングレベル */

static char *cpa_ipath[MAXINCPATH] ;    /* インクルードパス配列 */
static int i_incpaths = 0 ;

static symlist *SYMTOP = NULL ;  /* シンボルリストの先頭 */

#define SELF_NAME  "$SELF$"     /* 自分自身の basename と置換する文字列 */

/* ifdef,ifndef を発見したとき、プッシュする */
static
void ifdefpush(int i_onoff)
{
   if (i_ifdlev >= IFDSTKSIZ) {
      app_error(IFD_STK_OVER,1) ;
   }
   ia_ifdstk[i_ifdlev++] = i_onoff ;
}

/* endifdef でポップする */
static
int ifdefpop(void)
{
   if (i_ifdlev == 0) {
      app_error(IFD_STK_EMPTY,1) ;
   }
   return (int)(ia_ifdstk[--i_ifdlev]) ;
}

/* シンボルが定義されているかどうかを調べる */
static
int symdefine_p(char *cp)
{
   symlist *slp = SYMTOP ;
   while (slp != NULL) {
      if (strcmp((slp->cp_symbol),cp) == 0) {
         return TRUE ;
      }
      slp = slp->sp_next ;
   }
   return FALSE ;
}

/* 現在の ifdef 出力制御状態を得る */
int output_enable(void)
{
   return i_out_enable_flag ;
}

/* 内部シンボルを定義する */
void sympush(char *cp)
{
   symlist *slp ;

   /* リスト構造体の作成 */
   slp            = (symlist *)app_malloc(sizeof(symlist)) ;

   /* シンボル文字列の記憶域を確保し、コピー */
   slp->cp_symbol = app_strdup(cp) ;

   /* シンボルの値（未使用） */
   slp->cp_value  = (void *)NULL ;

   /* リストをリンクさせる */
   slp->sp_next   = SYMTOP ;
   SYMTOP         = slp ;
}

/* 内部シンボルテーブルのクリア : 後始末で call される */
void symclear(void)
{
   symlist *slp = SYMTOP ;

   while (slp != NULL) {
      app_free(slp->cp_symbol) ;
      /* app_free(slp->cp_value) ; */ /* value 拡張時使用 */
      slp = slp->sp_next ;
   }
}

/* _define の処理 */
int eval_define(char *cp_buf)
{
   char ca_symname[MAXSYMLEN] ;

   cutspc(cp_buf) ;                                  /* ごみを取り除く */
   gettoken(cp_buf,WHITESPACE,LDELIM,ca_symname) ;   /* シンボル名の切り出し */
   if (symdefine_p(ca_symname) == FALSE) {
      sympush(ca_symname) ;                          /* シンボルの定義 */
   }
   return 0 ;
}

/* _ifdef,_ifndef の処理 */
static 
int _eval_ifdef(char *cp_buf,int i_condition)
{
   char ca_symname[MAXSYMLEN] ;

   cutspc(cp_buf) ;                                  /* ごみを取り除く */
   gettoken(cp_buf,WHITESPACE,LDELIM,ca_symname) ;   /* シンボル名の切り出し */
   ifdefpush(i_out_enable_flag) ;                    /* 現在の条件を PUSH */

   /* 条件に合わないブロック内では、ネストのみカウントする */
   if (i_out_enable_flag == ON) {
      i_out_enable_flag = (symdefine_p(ca_symname) == i_condition ? ON : OFF);
   }
   return 0 ;
}

/* _ifdef の処理 */
int eval_ifdef(char *cp_buf)
{
    return _eval_ifdef(cp_buf,TRUE) ;
}

/* _ifndef の処理 */
int eval_ifndef(char *cp_buf)
{
    return _eval_ifdef(cp_buf,FALSE) ;
}

/* _elsedef の処理 */
#pragma argsused
int eval_elsedef(char *cp_buf)
{
   if (i_ifdlev == 0) {
      app_error(IFD_STK_EMPTY,1) ;
   }
   /* １レベル前が OFF のときだけ反転 */
   if (ia_ifdstk[i_ifdlev-1] == ON) {
      i_out_enable_flag = (i_out_enable_flag == ON ? OFF : ON ) ;
   }
   return 0 ;
}

/* _endifdef の処理 */
#pragma argsused
int eval_endifdef(char *cp_buf)
{
   i_out_enable_flag = ifdefpop() ;
   return 0 ;
}

/* インクルードファイルの検索 */
static
int search_include_file(char *cp_fname)
{
   int i = -1,
       i_count = 0 ;
   char ca_buf[PNLENGTH] ;

   strcpy(ca_buf,cp_fname) ;
   while (((i = open(ca_buf,O_RDONLY)) < 0) && (i_count < i_incpaths)) {
      strcpy(ca_buf,cpa_ipath[i_count]) ; /* インクルードパスの一つを試す */
      i = app_strlen(ca_buf) ;
      if (ca_buf[i-1] != PATH_SEPL) {
         ca_buf[i] = PATH_SEPL ; /* パス区切り記号以外で終わってたら追加 */
         ca_buf[i+1]   = '\0' ;
      }
      strcat(ca_buf,cp_fname) ; /* フルパス名を得る */
      i_count++ ;
   }

   if (i < 0) { /* 見つからなかったら */
      app_error(FILE_NOT_FOUND,0) ;
      fprintf(_erout,"%s\n",cp_fname) ;
      exit(FILE_NOT_FOUND) ;
   }

   close(i) ;
   return include_file(ca_buf) ;
}

/* _include,_header の処理 */
int eval_include(char *cp_buf)
{
   char ca_nfn[PNLENGTH] ;

   cutspc(cp_buf) ;                              /* ごみを取り除く */
   gettoken(cp_buf,WHITESPACE,LDELIM,ca_nfn) ;   /* ファイル名の切り出し */

   if (strstr(ca_nfn,SELF_NAME) != NULL) {
      char ca_mynam[PNLENGTH] ;
      get_ownbasename(ca_mynam) ;                /* ベースネームを得る */
      str_replace(ca_nfn,SELF_NAME,ca_mynam) ;   /* $SELF$ と置換 */
   }

   return search_include_file(ca_nfn) ; /* インクルード */
}

/* インクルードパスの追加 */
int add_incpath(char *cp_path)
{
   if (i_incpaths >= MAXINCPATH) {
      app_error(TOO_MANY_ARG,1) ;
   }
   cutspc(cp_path) ;
   if (app_strlen(cp_path) > 0) {
      cpa_ipath[i_incpaths++] = app_strdup(cp_path) ;
   }
   return 0 ;
}

/* インクルードパス領域の開放 */
void clear_incpath(void)
{
   int i ;
   for (i = 0 ; i < i_incpaths ; i++) {
      app_free(cpa_ipath[i]) ;
   }
   i_incpaths = 0 ;
}

/* _warning の処理 */
int eval_warning(char *cp_buf)
{
   cutspc(cp_buf) ;
   strcat(cp_buf,"\n") ;
   app_warning(cp_buf) ;

   return 0 ;
}
