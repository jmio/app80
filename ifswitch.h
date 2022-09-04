/*
                         Generic APP Source File

                          Filename : IFSWITCH.H
                          Version  : 95/03/01
                            Written by Mio

                             条件分岐処理
*/

#ifndef IFSWITCH_H
#define IFSWITCH_H

#define IFNESTMAX 128           /* _if の最大ネスティングレベル */
#define SWNESTMAX 100           /* _switch の最大ネスティングレベル */

/* テンポラリ・ラベル */
#define TMPL_ELSE  ".ELSE"
#define TMPL_ENDIF ".ENDIF"

#include "ifswitch.prt"          /* 関数プロトタイプ */

#endif /* of IFSWITCH_H */
