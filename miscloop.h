/*
                         Generic APP Source File

                          Filename : MISCLOOP.H
                          Version  : 95/03/01
                            Written by Mio

                            各種ループ処理
*/

#ifndef MISCLOOP_H
#define MISCLOOP_H

#define LPNESTMAX         100          /* loop の最大ネスティングレベル */

#define TMPL_BEGIN        ".BEGIN"
#define TMPL_CONTINUE     ".CONT"
#define TMPL_EXIT         ".EXIT"
#define TMPL_FORBEGIN     ".FORBG"

extern int i_loopnum ;              /* loop ラベル・カウンタ */
extern int i_looplev ;              /* loop ブロックネスティングレベル */
extern char *cp_lcondstk[] ,        /* loop 条件スタック1 */
            *cp_lcondstk2[] ;       /* loop 条件スタック2 (for) */
extern int ia_loopn[] ,             /* loop ラベル番号スタック */
           ia_contf[] ,             /* continue フラグスタック */
           ia_exitf[] ;             /* exit フラグスタック */

#include "miscloop.prt"

#endif /* of MISCLOOP_H */
