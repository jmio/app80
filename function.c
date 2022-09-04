/*
                         Generic APP Source File

                          Filename : FUNCTION.C
                          Version  : 95/03/09
                            Written by Mio

                              関数の処理
*/

#include <stdio.h>
#include <string.h>

#include "appmain.h"
#include "appdefs.h"
#include "appsub.h"
#include "apperr.h"
#include "superstr.h"
#include "proto.h"
#include "ifswitch.h"
#include "function.h"

/* function or procedure */
enum FUNCPRC  { FUNC , PROC } ;
enum NORMINTR { NORM , INTR } ;

/* 関数情報構造体 */
typedef struct FUNC_TBL {
   char *cp_fname ;             /* 関数名 */

   int  i_args ;                /* 引き数の数 */
   char **cpp_fargs ;           /* 引数リスト（ないときは NULLptr） */

   int  i_save ;                /* 保存レジスタ数 */
   char **cpp_fsave ;           /* 保存レジスタリスト（ないときは NULLptr） */

   int  i_predef ;              /* 前に定義されているか */
   int  i_called ;              /* 呼び出された回数 */
   int  i_func_or_proc ;        /* _procedure , _function の別 */
   struct FUNC_TBL *ftp_next ;  /* テーブルリンク */
} func_tbl ;

static func_tbl *FUNCTOP = NULL ; 
static func_tbl *fun_tmp = NULL ; /* 処理中の function 情報 */
static int i_retfound ;           /* return があったか？ */

#define KW_ENDF  "endf"           /* return 時の JMP 出力判断用キーワード */

/* 関数形式のパラメータよみとり */
static
func_tbl *get_funargs(char *cp)
{
   char ca_tmp[MAXARGLEN] ;
   int i ;
   func_tbl *ftp = app_malloc(sizeof(func_tbl)) ;

   ftp->cp_fname = NULL ;
   ftp->i_args   = 0 ;
   ftp->cpp_fargs = NULL ;
   ftp->i_save   = 0 ;
   ftp->cpp_fsave = NULL ;
   ftp->ftp_next = NULL ;

   /* 関数名の取り出し */
   cp = gettoken(cp,WHITESPACE,FUNDELIMS,ca_tmp) ;
   cutspc(ca_tmp) ;
   i = app_strlen(ca_tmp) ;
   if ((i == 0) || (i > MAXFNLEN)) {
      app_error(FNAME_ERROR,1) ; /* 関数名不正 */
   }
   ftp->cp_fname = app_strdup(ca_tmp) ;
   cutspc(cp) ;

   /* 引き数部分の取り出し */
   if (cp[0] == '(') { /* 引き数がある時だけ */
      cp = gettoken(&cp[1],WHITESPACE,ARGDELIMS,ca_tmp) ;
      cutspc(ca_tmp) ; /* 前後のスペースをカット */
      if (app_strlen(ca_tmp) > 0) { /* 内容があった時だけ */
         ftp->i_args = strtocpa(ca_tmp,&(ftp->cpp_fargs),MAXARGS,ARGCUTDELIM) ;
      }
      cutspc(++cp) ; /* 閉じ括弧の次から */
   }

   /* 保存レジスタの取り出し */
   if (cp[0] == ':') { /* 保存レジスタ指定のある時だけ */
      cp = gettoken(&cp[1],WHITESPACE,REGDELIMS,ca_tmp) ;
      cutspc(ca_tmp) ; /* 前後のスペースをカット */
      if (app_strlen(ca_tmp) > 0) { /* 内容があった時だけ */
         ftp->i_save = strtocpa(ca_tmp,&(ftp->cpp_fsave),MAXARGS,ARGCUTDELIM) ;
      }
   }

   return ftp ;
}

/* 関数形式領域の開放 */
static
void ftp_release(func_tbl *ftp)
{
   if (ftp == NULL) {
      app_error(FUNFREE_NULL,1) ;
   }

   /* 名前領域の開放 */
   app_free(ftp->cp_fname) ;

   /* 引数領域の開放 */
   if (ftp->i_args > 0) {
      cpa_release(ftp->cpp_fargs) ;
   }

   /* 保存変数領域の開放 */
   if (ftp->i_save > 0) {
      cpa_release(ftp->cpp_fsave) ;
   }
}

/* 関数情報リストの開放 */
static
void ftp_relall(func_tbl *ftp)
{
   if (ftp != NULL) {
      ftp_relall(ftp->ftp_next) ;
      ftp_release(ftp) ;
   }
}

/* 関数が以前定義されていないか、また引数リストが同一かチェック */
static 
void check_and_define(func_tbl *ftp_t,int i_defp)
{
   func_tbl *ftp = FUNCTOP ;

   /* 同じ名前の定義が存在するか？ */
   while (ftp != NULL) {
      if (strcmp(ftp->cp_fname,ftp_t->cp_fname) == 0) { /* 名前が一致 */
         if ((ftp->i_predef == TRUE) && (i_defp == TRUE)) {
            app_error(FUNDUP_ERROR,1) ; /* 同じ名前の関数が既にある */
         }
         if (ftp->i_args == ftp_t->i_args) {
            if (ftp->i_args != 0) {
               if (cpa_equalp(ftp->cpp_fargs,ftp_t->cpp_fargs) == FALSE) {
                  app_error(FUNDEF_ERROR,1) ; /* ヘッダと実体の定義が違う */
               }
            }
            /* 型が完全に一致した */
            if (i_defp == TRUE) {
               ftp->i_predef = TRUE ; /* 定義済みの印 */
            }
            return ;
         }
         app_error(FUNDEF_ERROR,1) ; /* ヘッダと実体の定義が違う */
      }
      ftp = ftp->ftp_next ; /* 次のリンク */
   }

   /* 見つからなかったら、リストに新たに追加する */
   ftp_t->i_predef = i_defp ;   /* ヘッダの時 FALSE (オリジナル版の逆) */
   ftp_t->ftp_next = FUNCTOP ;
   FUNCTOP         = ftp_t ;
}

/* 関数呼出時の変数保存、引渡し処理 */
static
void funpush_output(char *cp_from,char *cp_to)
{
   int i_pushf = FALSE ,
       i_leaf  = FALSE ;

   if (*cp_to == PUSH_CHAR) {
      cp_to++ ;
      i_pushf = TRUE ;
   }
   if (*cp_to == LEA_CHAR) {
      cp_to++ ;
      i_leaf  = TRUE ;
   }

   /* 変数の保存を行う */
   if (i_pushf == TRUE) {
      push_output(cp_to) ;
   }

   /* 引数を渡す */
   if (i_leaf == TRUE) {
      lea_output(cp_to,cp_from) ;
   } else {
      mov_output(cp_to,cp_from) ;
   }
}

/* 関数呼出時の保存変数復帰処理 */
static
void funpop_output(char *cp)
{
   int i_popf = FALSE ;

   if (*cp == PUSH_CHAR) {
      cp++ ;
      i_popf = TRUE ;
   }
   if (*cp == LEA_CHAR) {
      cp++ ;
   }

   /* 変数の保存を行う */
   if (i_popf == TRUE) {
      pop_output(cp) ;
   }
}

/* 関数呼出時の引数の展開処理 */
static 
func_tbl *check_and_movout(func_tbl *ftp_t)
{
   int i ;
   func_tbl *ftp = FUNCTOP ;

   /* 同じ名前の定義が存在するか？ */
   while (ftp != NULL) {
      if (strcmp(ftp->cp_fname,ftp_t->cp_fname) == 0) { /* 名前が一致 */
         if (ftp->i_args != ftp_t->i_args) {
            app_error(FUNARG_ERROR1,1) ; /* 定義と呼び出しで引数の数が違う */
         }
         if (ftp->i_func_or_proc != ftp_t->i_func_or_proc) {
            app_error(FUNARG_ERROR2,1) ; /* 定義と呼び出しでタイプが違う */
         }
         for (i = 0; i < ftp->i_args ; i++) { /* 引数を渡す */
            char *cp_to   = ftp->cpp_fargs[i] ,
                 *cp_from = ftp_t->cpp_fargs[i] ;
            funpush_output(cp_from,cp_to) ;       /* PUSH output */
         }
         return ftp ;
      }
      ftp = ftp->ftp_next ; /* 次のリンク */
   }

   /* 見つからなかったらエラー */
   app_error(FUNUNDEF_ERROR,1) ;
   return NULL ;
}

/* _function_h,_procedure_h の処理 */
static
int _eval_function_h(char *cp_line,int i_forp)
{
   func_tbl *ftp ;

   ftp = get_funargs(cp_line) ;     /* 関数の属性の取得 */
   ftp->i_func_or_proc = i_forp ;
   check_and_define(ftp,FALSE) ;    /* 関数をテーブルに登録 */

   return 0 ;
}

/* _function,_procedure の処理 */
static
int _eval_function(char *cp_line,int i_forp)
{
   int i ;
   func_tbl *ftp ;

/* 旧形式のヘッダを認識するための情報 */
#define HEADCHARPOS (i_forp == FUNC ? -10 : -11)
#define HEADCHAR    '@'

   if (cp_line[HEADCHARPOS] == HEADCHAR) { /* 旧形式のヘッダ？ */
      _eval_function_h(cp_line,i_forp) ;
      return 0 ;
   }

   if (fun_tmp != NULL) { /* すでに関数の中？ */
      app_error(INFUNC_ERROR,1) ;
   }

   ftp = get_funargs(cp_line) ;       /* 関数の属性の取得 */
   ftp->i_func_or_proc = i_forp ;
   check_and_define(ftp,TRUE) ;       /* 関数をテーブルに登録 */
   label_output(ftp->cp_fname) ;      /* 関数名ラベル出力 */

   /* PUSH 命令出力 */
   for (i = 0 ; i < ftp->i_save ; i++) {
      push_output(ftp->cpp_fsave[i]) ;
   }

   fun_tmp = ftp ;         /* 処理中の関数の情報へのポインタ保存 */
   i_retfound = FALSE ;

   return 0 ;
}


/* _endf,_endp の処理 */
#pragma argsused
static
void _eval_endf(char *cp_line,int i_forp,int i_intr)
{
   char ca_label[LBLBFLEN] ;
   int i ;

   if (fun_tmp == NULL) { /* 今、関数の中ではない */
      app_error(ENDF_ERROR,1) ;
   }

   if (fun_tmp->i_func_or_proc != i_forp) { /* procedure に対する endf */
      app_error(ENDF_ERROR,1) ;             /* またはその逆 */
   }

   /* 関数出口のラベル */
   if ((i_retfound == TRUE) || (ia_option['R'] == TRUE)) {
      sprintf(ca_label,LABEL_FEND,fun_tmp->cp_fname) ; /* ラベル文字列生成 */
      label_output(ca_label) ;
   }

   /* POP 命令出力 */
   for (i = (fun_tmp->i_save)-1 ; i >= 0 ; i--) {
      pop_output(fun_tmp->cpp_fsave[i]) ;
   }

   /* RET 命令出力 */
   if (i_intr == TRUE) { /* 割り込みサブルーチンか? */
      iret_output() ;
   } else {
      ret_output() ;
   }

   if (in_ifblkp() > 0) { /* 閉じられていない if が存在する */
      app_error(NO_ENDIF,1) ;
   }

   if (in_loopblkp() > 0) { /* 閉じられていないループが存在する */
      app_error(NO_ENDLOOP,1) ;
   }

   fun_tmp = NULL ; /* 関数ブロックからの脱出 */
}

/* _func,_proc の処理 */
static
int _eval_func(char *cp_line,int i_forp)
{
   int i ;
   func_tbl *ftp,*ftp_d ;

   ftp = get_funargs(cp_line) ;   /* 関数の属性の取得 */
   ftp->i_func_or_proc = i_forp ; /* チェックのため属性を与える */

   ftp_d = check_and_movout(ftp) ;         /* 引数 mov の出力 */
   /* ここで ftp_d は定義側のテーブルポインタ */
   call_output(ftp_d->cp_fname) ;          /* 関数の call */
   for (i = (ftp_d->i_args)-1 ; i >= 0 ; i--) {
      funpop_output(ftp_d->cpp_fargs[i]) ; /* 保存変数の復帰 */
   }
   (ftp_d->i_called)++ ;                   /* 呼ばれた回数++ */

   ftp_release(ftp) ;             /* テンポラリエリアの開放 */

   return 0 ;
}

/* _function の処理 */
int eval_function(char *cp_line)
{
   if (ia_option['H']) { /* ヘッダ生成？ */
      app_outstr(ia_option['O'] ? OLD_FUNC_H : NEW_FUNC_H ) ;
      app_outstr(cp_line) ;
   } else {
      _eval_function(cp_line,FUNC) ;
   }
   return 0 ;
}

/* _procedure の処理 */
int eval_procedure(char *cp_line)
{
   if (ia_option['H']) { /* ヘッダ生成？ */
      app_outstr(ia_option['O'] ? OLD_PROC_H : NEW_PROC_H ) ;
      app_outstr(cp_line) ;
   } else {
      _eval_function(cp_line,PROC) ;
   }
   return 0 ;
}

/* _function_h の処理 */
int eval_function_h(char *cp_line)
{
   _eval_function_h(cp_line,FUNC) ;
   return 0 ;
}

/* _procedure_h の処理 */
int eval_procedure_h(char *cp_line)
{
   _eval_function_h(cp_line,PROC) ;
   return 0 ;
}

/* _endf の処理 */
int eval_endf(char *cp_line)
{
   _eval_endf(cp_line,FUNC,NORM) ;
   return 0 ;
}

/* _endp の処理 */
int eval_endp(char *cp_line)
{
   _eval_endf(cp_line,PROC,NORM) ;
   return 0 ;
}

/* _endpi の処理 */
int eval_endpi(char *cp_line)
{
   _eval_endf(cp_line,PROC,INTR) ;
   return 0 ;
}

/* _func の処理 */
int eval_func(char *cp_line)
{
   _eval_func(cp_line,FUNC) ;
   return 0 ;
}

/* _proc の処理 */
int eval_proc(char *cp_line)
{
   _eval_func(cp_line,PROC) ;
   return 0 ;
}

/* _return の処理 */
int eval_return(char *cp_line)
{
   char ca_buf[LINEBUFSIZ],
        ca_resw[RESWBUFSIZ],
        ca_label[LBLBFLEN] ;

   int i ;

   if (fun_tmp == NULL) { /* 今、関数の中ではない */
      app_error(ENDF_ERROR,1) ;
   }

   /* 戻り値の代入 */
   {
      char ca_eval[MAXARGLEN] ;
      cutspc(cp_line) ; /* ごみを取り除く */
      gettoken(cp_line,WHITESPACE,LDELIM,ca_eval) ; /* 戻り値の取り出し */
      if (app_strlen(ca_eval) > 0) { /* 戻り値があるとき */
         if (fun_tmp->i_func_or_proc == FUNC) { /* 戻り値の代入 */
            retval_output(ca_eval) ; /* 戻り値の mov */
         } else {
            app_error(RETARG_ERROR,1) ; /* procedure に戻り値は不要 */
         }
      }
   }

   sprintf(ca_label,LABEL_FEND,fun_tmp->cp_fname) ; /* ラベル文字列生成 */

   /* 次行の先読みによる JMP 命令出力判断 */
   prefetch_line(ca_buf) ;
   get_appresw(ca_buf,ca_resw) ;
   if (strcmp(ca_resw,KW_ENDF) != 0) {
      jmp_output(ca_label) ;
   }

   i_retfound = TRUE ;

   return 0 ;
}

/* _public の処理 */
#pragma argsused
int eval_public(char *cp_line)
{
   func_tbl *ftp = FUNCTOP ;

   /* 現在までの定義関数の PUBLIC 宣言 */
   while (ftp != NULL) {
      if (ftp->i_predef == TRUE) {
         public_output(ftp->cp_fname) ;
         (ftp->i_called)++ ;
      }
      ftp = ftp->ftp_next ; /* 次のリンク */
   }

   return 0 ;
}

/* _extrn の処理 */
#pragma argsused
int eval_extrn(char *cp_line)
{
   func_tbl *ftp = FUNCTOP ;

   /* 現在までの未定義関数の EXTERN 宣言 */
   while (ftp != NULL) {
      if (ftp->i_predef == FALSE) { /* ヘッダ = FALSE (original と逆) */
         extern_output(ftp->cp_fname) ;
         ftp->i_predef = TRUE ;
      }
      ftp = ftp->ftp_next ; /* 次のリンク */
   }

   return 0 ;
}

/* 完了処理 */
int exit_function(void)
{
   ftp_relall(FUNCTOP) ;

   return 0 ;
}
