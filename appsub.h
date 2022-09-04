/*
                         Generic APP Source File

                          Filename : APPSUB.H
                          Version  : 95/03/01
                            Written by Mio

                    APP 評価ルーチン用サブルーチン群
*/

#ifndef APPSUB_H
#define APPSUB_H

/* include_file のラインバッファサイズ */
#define LINEBUFSIZ 256           /* 行読み取り用バッファサイズ */

/* gettoken で用いる定数 */
#define WHITESPACE        " \t"         /* 読み飛ばしキャラクタ群 */
#define LDELIM            " \t\n;"      /* デフォルト行デリミタ */

#define QUOTES            "\"\'"        /* クオート */
#define REPBGN            "("           /* 括弧 */
#define REPEND            ")"           /* 括弧閉じ */

/* 特殊キャラクタ */
#define UNDERBAR          '_'
#define COMMENT           ';'

#define cutspc(x)         cutspcs((x)," \t\n")

#include "appsub.prt"                   /* 関数プロトタイプ */

#endif /* of APPSUB_H */
