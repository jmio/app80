/*
                         Generic APP Source File

                          Filename : APPMAIN.H
                          Version  : 95/03/01
                            Written by Mio

                    Generic APP メインルーチンファイル
*/

#ifndef APPMAIN_H
#define APPMAIN_H

#define MAXFNAMES 2            /* コマンドラインで指定できるファイルネーム数 */
                               /* これを変更する時は main の app_getopt に
                                  わたす戻り値バッファ配列を増やすことが必要 */

#define MAXPARAMS 100          /* ファイル名以外の最大パラメータ数 */

/* Ｃプロトタイプメーカとの共用オプション */
#if !defined(CPROTO) && !defined(MKDEP)
# include "version.h"
# ifdef APP86
#  define DEFAULT_IN_EXT               ".app"
#  define DEFAULT_OUT_EXT              ".asm"
#  define DEFAULT_HD_EXT               ".h"
# else
#  define DEFAULT_IN_EXT               ".app"
#  define DEFAULT_OUT_EXT              ".asm"
#  define DEFAULT_HD_EXT               ".h"
# endif
#endif

#ifdef CPROTO
# include "cprtver.h"
# define DEFAULT_IN_EXT               ".c"
# define DEFAULT_OUT_EXT              ".prt"
# define DEFAULT_HD_EXT               ".prt"   /* DUMMY */
#endif

#ifdef MKDEP
# include "mkdepver.h"
# define DEFAULT_IN_EXT               ".c"
# define DEFAULT_OUT_EXT              ".dep"
# define DEFAULT_HD_EXT               ".dep"   /* DUMMY */
#endif

/* グローバル変数 */
extern FILE *fp_out ;                  /* 出力ファイル */
extern int ia_option[] ;               /* オプション on/off フラグ */

#include "appmain.prt"                 /* 関数プロトタイプ */

#endif /* of APPMAIN_H */
