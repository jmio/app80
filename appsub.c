/*
                         Generic APP Source File

                          Filename : APPSUB.C
                          Version  : 95/03/01
                            Written by Mio

                    APP 評価ルーチン用サブルーチン群
*/

#include <stdio.h>
#include <string.h>

#ifdef __MSDOS__
# include <alloc.h>
#else
# include <malloc.h>
#endif

#include <stdarg.h>
#include <ctype.h>

#include "appmain.h"
#include "appdefs.h"
#include "apperr.h"
#include "appsub.h"

/* ----------------------------------------------------------- */

/* 各 CPU 別、行評価ルーチンのコール (EVALxx.C に存在) */
int eval_line(char *) ;

/* ----------------------------------------------------------- */

/* 行のプリフェッチバッファ、およびフラグ */
static FILE *fp_in_cpy = NULL ;
static int i_pftch = FALSE ;
static char ca_pftch[LINEBUFSIZ] ;

/* メモリ残量 */
long app_avmem(void)
{
#ifdef __MSDOS__
   return (long)coreleft() ;
#else
   return (long)0 ;
#endif
}

/* メモリの確保 */
void *app_malloc(size_t st)
{
   void *v_result = malloc(st) ;

   if (v_result == NULL) {
      app_error(MEMORY_NOT_ENOUGH,1) ;
   }
   return v_result ;
}

/* メモリの開放 */
void app_free(void *vp)
{
   if (vp == NULL) {
      app_error(FREE_NULL,0) ;
   } else {
      free(vp) ;
   }
   return ;
}

/* 新たに確保したメモリに文字列をコピー : 可変長バッファ */
char *app_strdup(char *cp_orig)
{
   char *cp_new = (char *)app_malloc(app_strlen(cp_orig)+1) ;

   strcpy(cp_new,cp_orig) ;
   return cp_new ;
}

/* 新たに確保したメモリに文字列をコピー : 固定長バッファ */
char *app_strdup2(char *cp_orig,size_t s)
{
   char *cp_new ;

   if ((app_strlen(cp_orig)+1) < s) {
      app_error(STRDUP_FAIL,1) ;
   } ;
   cp_new = app_malloc(s) ;
   strcpy(cp_new,cp_orig) ;
   return cp_new ;
}

/* ファイルのオープン */
FILE *open_file(const char *cp_fname,const char *cp_mode)
{
   FILE *fp_result = fopen(cp_fname,cp_mode) ; /* 結果 */

   if (fp_result == NULL) {
      app_error(FILE_ERROR,0) ;
      perror(cp_fname) ; /* エラーメッセージ出力 */
      exit(1) ;
   }
   return fp_result ;
}

/* ファイルからの１行読みだし */
static
int read_line(char *cp_line,FILE *fp_in)
{
   /* バッファクリア */
   cp_line[0] = '\0' ;

   /* ファイル先読みが行われているか？ */
   if (i_pftch == TRUE) {
      strcpy(cp_line,ca_pftch) ;
      i_pftch = FALSE ;
      return TRUE ;
   }

   /* EOF ? */
   if (feof(fp_in)) {
      return FALSE ;
   }

   /* 行われていなければ通常に読みだす */
   if (fgets(cp_line,LINEBUFSIZ,fp_in) != NULL) {
      return TRUE ;
   }

   return FALSE ;
}

/* ファイルからの１行先読み */
int prefetch_line(char *cp_buf)
{
   /* バッファクリア */
   cp_buf[0] = '\0' ;

   /* ファイル先読みが行われているか？ */
   if ((i_pftch == TRUE) || (fp_in_cpy == NULL)) {
      return FALSE ; /* 失敗 */
   }

   /* EOF ? */
   if (feof(fp_in_cpy)) {
      return FALSE ;
   }

   /* 行われていなければ通常に読みだす */
   if (fgets(ca_pftch,LINEBUFSIZ,fp_in_cpy) != NULL) {
      strcpy(cp_buf,ca_pftch) ;
      i_pftch = TRUE ;
      return TRUE ;
   }

   return FALSE ;
}

/* ファイルのインクルード */
int include_file(char *cp_fname)
{
   FILE *fp_in,*fp_in_old ;   /* 入力ファイル */
   long l_lineno = 0 ;        /* 入力ファイル行番号 */
   char *cp_old_fname ;       /* インクルードする側のファイル名ポインタ */
   char ca_line[LINEBUFSIZ] ; /* 行バッファ */

   /* ファイルのオープン */
   fp_in = open_file(cp_fname,"rt") ;

   /* プリフェッチ用のコピー作成 */
   fp_in_old = fp_in_cpy ;
   fp_in_cpy = fp_in ;

   /* ファイル名をエラー処理ルーチンから参照可能にする */
   cp_old_fname = cp_err_fname ;
   cp_err_fname = cp_fname ;

   /* ファイルの評価 */
   while(read_line(ca_line,fp_in) == TRUE) {
      l_err_lineno = ++l_lineno ; /* 行番号をファイル内スタティックに */
      (void)eval_line(ca_line) ;  /* 評価 (EVALxx.C) */
   }

   fclose(fp_in) ;

   fp_in_cpy = fp_in_old ;
   cp_err_fname = cp_old_fname ;

   return 0 ;
}

/* 出力ファイルに文字列を書き込む */
void app_outstr(char *cp_outstr)
{
   if (fputs(cp_outstr,fp_out) == EOF) {
      app_error(FILE_ERROR,0) ;
      fclose(fp_out) ;
      perror("app_outstr") ;
      exit(1) ;
   }
}

/* 出力ファイルに printf 形式で書きこむ */
void app_printf(char *cp_format , ...)
{
   va_list v_argptr;

   va_start(v_argptr, cp_format);
   if (vfprintf(fp_out,cp_format,v_argptr) == EOF) {
      app_error(FILE_ERROR,0) ;
      perror("app_printf") ;
      exit(1) ;
   }

   va_end(v_argptr);
}

/* 自分自身のファイル名を得る */
void get_ownname(char *cp)
{
   strcpy(cp,cp_err_fname) ;
   return ;
}

/* 自分自身のファイル名を得る */
void get_ownbasename(char *cp)
{
   get_ownname(cp) ;
   cp = getdotpos(cp) ;
   if (cp != NULL) { /* 拡張子があったら、そこで切る */
      *cp = '\0' ;
   }
   return ;
}

/* ヌルポインタ、ヌル文字列チェック */
int app_strlen(char *cp)
{
   if (cp == NULL) {
      app_error(ERR_NULL_STR,1) ; /* ヌルポインタを検出した */
   }
   if (*cp == '\0') {
      return 0 ;
   }
   return (int)strlen(cp) ;
}

/* cp_line から cp_white を読み飛ばし、cp_delim のなかの１文字がみつかるまで
   切り出して cp_dist に copy                         */
char* gettoken(char *cp_line,
               char *cp_white,         /* 読み飛ばすキャラクタ群 */
               char *cp_delim,         /* デリミタキャラクタ群 */
               char *cp_dist)
{
   int i_tmpquote   = -1 ,               /* 検出したクオートキャラクタ */
       i_quote      = FALSE ,            /* クオート中かどうかのフラグ */
       i_replevel   = 0 ,                /* 括弧のネスティングレベル */
       i ;

   while((i = *cp_line) != (int)'\x00') { /* 前についているごみを飛ばす */
      if (strchr(cp_white,i) == NULL) {
         break ;
      }
      cp_line++ ;
   }

   while(((i = *cp_line) != (int)'\x00') && 
         ((strchr(cp_delim,(int)(*cp_line)) == NULL) || 
          (i_replevel > 0) || 
          (i_quote == TRUE))
        ) {

      if (i_quote == FALSE) {
         if (strchr(QUOTES,i) != NULL) { /* クオートを発見した */
            i_quote = TRUE ;
            i_tmpquote = i ; /* クオートキャラクタを記憶しておく */
         }
         if (strchr(REPBGN,i) != NULL) { /* 括弧を発見した */
            i_replevel++ ;
         } else {
            if ((i_replevel > 0) && (strchr(REPEND,i) != NULL)) {
               i_replevel-- ;
            }
         }
      } else {
         if (i_tmpquote == i) { /* クオートの終わりを発見した */
            i_quote = FALSE ;
         }
      }

      *cp_dist++ = *cp_line++ ;
   }
   *cp_dist = '\x00' ;

   if (i_replevel > 0) {
      app_error(REP_ERROR,1) ;
   }
   if (i_quote == TRUE) {
      app_error(QUOTE_ERROR,1) ;
   }

   return cp_line ;
}

/* cp_line から cp_delim のなかの１文字以外がみつかるまで
   切り出して cp_dist に copy          ~~~~               */
char* gettoken2(char *cp_line,char *cp_white,char *cp_delim,char *cp_dist)
{
   char *cp ;
   int i ;

   while((i = *cp_line) != (int)'\x00') { /* 前についているごみを飛ばす */
      if (strchr(cp_white,i) == NULL) {
         break ;
      }
      cp_line++ ;
   }

   while((i = *cp_line) != (int)'\x00') { /* NULL でないあいだ */
      if (strchr(cp_delim,i) == NULL) { /* デリミタ列に含まれているか？ */
         *cp_dist = '\x00' ;
         return cp_line ;
      }
      *cp_dist++ = *cp_line++ ;
   }
   *cp_dist = '\x00' ;
   return cp_line ;
}

/* _ ではじまる APP 予約語候補の切り出し */
char *get_appresw(char *cp_line,char *ca_resw)
{
   char *cp_pou ,             /* アンダーバーの位置 */
        *cp_poc ;             /* コメント記号の位置 */

   /* バッファクリア */
   ca_resw[0] = '\0' ;

   /* コメント記号を探す */
   cp_poc = app_strchr(cp_line,COMMENT,'\0') ;

   /* アンダーバーが見つからない時 */
   if ((cp_pou = app_strchr(cp_line,UNDERBAR,COMMENT)) == NULL) {
      return NULL ;
   }

   /* アンダーバーがあったが、コメント記号より後 */
   if ((cp_poc != NULL) && (cp_pou > cp_poc)) {
      return NULL ;
   }

   /* ホワイトスペースか、コメント記号まで切り出し */
   return gettoken(cp_pou+1,WHITESPACE,LDELIM,ca_resw) ;
}

/* 文字列の前後についているごみを取り除く */
void cutspcs(char *cp_buf,const char *cp_spcs)
{
   char *cp_save = cp_buf ;
   int  i_strlen = app_strlen(cp_buf) ;

   /* ヌル文字列? */
   if (i_strlen == 0) {
      return ;
   }

   /* ヌルの手前の位置に */
   cp_buf += (i_strlen - 1) ;

   /* お尻をきれいに */
   while (strchr(cp_spcs,*cp_buf) != NULL) {
      cp_buf-- ;
   }
   *(cp_buf+1) = '\0' ;

   /* 頭がきれいでなかったらずらす */
   cp_buf = cp_save ;
   while ((strchr(cp_spcs,*cp_buf) != NULL) && (*cp_buf != '\0')) {
      cp_buf++ ;
   }

   if (cp_buf != cp_save) {
      while (*cp_buf != '\0') {
         *cp_save++ = *cp_buf++ ;
      }
      *cp_save = '\0' ;
   }
}

/* cp_line 中に 特定キャラクタが含まれるかどうかを探索
   （クオート処理込み）*/
char* app_strchr(char *cp_line,int i_char,int i_delim)
{
   int i_tmpquote   = -1 ,               /* 検出したクオートキャラクタ */
       i_quote      = FALSE ,            /* クオート中かどうかのフラグ */
       i ;

   while((i = (int)(*cp_line)) != (int)'\x00') {
      if (i_quote == FALSE) {
         if ((i == i_char) || (i == i_delim)) {
             break ;                     /* 目的のキャラクタを検出 */
         }
         if (strchr(QUOTES,i) != NULL) { /* クオートを発見した */
            i_quote = TRUE ;
            i_tmpquote = i ; /* クオートキャラクタを記憶しておく */
         }
      } else {
         if (i_tmpquote == i) { /* クオートの終わりを発見した */
            i_quote = FALSE ;
         }
      }
      cp_line++ ;
   }

   if (i != i_char) {
      cp_line = NULL ; /* 発見できなかった */
   }

   return cp_line ;
}

/* 文字列が数値かどうか調べる (数字なら TRUE を返す) */
int app_isnum(char *cp)
{
   int i = app_strlen(cp) ;

   if (i > 0) {
      if (isdigit(cp[0])) { /* 先頭が数字 */
         i-- ;              /* ヌルの手前 */
         if (isdigit(cp[i]) || (cp[i] == 'H') || (cp[i] == 'h')
                            || (cp[i] == 'B') || (cp[i] == 'b')) {
            return TRUE ;
         }
      }
   }
   return FALSE ;
}

/* 文字列テーブルをサーチし、エントリ番号を返す */
int app_scantbl(char **cpp_table,char *cp)
{
   int i ;

   for (i = 0 ; cpp_table[i] != NULL ; i++) {
      if (strcmp(cpp_table[i],cp) == 0) {
         return ++i ; /* 見つかった */
      }
   }
   return 0 ; /* 見つからなかった */
}

/* テーブル中の文字列が含まれるかどうかをサーチし、エントリ番号を返す
                                                 （文字列カットつき） */
int app_scantbl_with_cut(char **cpp_table,char *cp)
{
   int i ;
   char *cp_r ;

   for (i = 0 ; cpp_table[i] != NULL ; i++) {
      if ((cp_r = strstr(cp,cpp_table[i])) != NULL) { /* 見つかったか？ */
         char *cp_from = &cp_r[strlen(cpp_table[i])] ;
         while (*cp_from != '\0') { /* 該当部分のカット */
             *cp_r++ = *cp_from++ ;
         }
         *cp_r = '\0' ;
         return ++i ;
      }
   }
   return 0 ;
}

/* 文字列 cp_t 内で cp_s1 と一致する文字列全てを cp_s2 に置換する
   （但し cp_t から始まる領域は十分大きいこと） */
int str_replace(char *cp_t,char *cp_s1,char *cp_s2)
{
   int i_s1len = app_strlen(cp_s1) ,
       i_s2len = app_strlen(cp_s2) ,
       i_count = 0 ,              /* 置換回数カウンタ */
       i_diff ,
       i ;
   char *cp ;

   i_diff = i_s1len - i_s2len ;
   while((cp = strstr(cp_t,cp_s1)) != NULL){
      /* ずらす */
      if (i_diff > 0) { /* 短くなる場合 */
         char *cp_ptr ;
         for (cp_ptr = cp ; cp_ptr[i_s1len] != '\0' ; cp_ptr++) {
            cp_ptr[i_s2len] = cp_ptr[i_s1len] ;
         }
         cp_ptr[i_s2len] = cp_ptr[i_s1len] ; /* NULL CHAR */
      }
      if (i_diff < 0) { /* 長くなる場合 */
         char *cp_ptr = &cp_t[app_strlen(cp_t)] ;
         for (cp_ptr = cp_t + app_strlen(cp_t) ; cp_ptr >= cp + i_s1len ;cp_ptr--){
            cp_ptr[-i_diff] = cp_ptr[0] ;
         }
      }
      /* 置換 */
      for (i = 0 ; i < i_s2len ; i++) {
         cp[i] = cp_s2[i] ;
      }
      i_count++ ;
   }
   return i_count ;
}

/* パス名からドットの位置を探す */
char *getdotpos(char *cp)
{
   int i_len = app_strlen(cp) ;

   if ((i_len == 0) || (strchr(cp,'.') == NULL)) {
      return NULL ;
   }
   i_len-- ;
   for(;strchr("./\\",cp[i_len]) == NULL;i_len--) ;
   if (strchr("/\\",cp[i_len]) != NULL) {
      return NULL ;
   }

   return &cp[i_len] ;
}
