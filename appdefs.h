/*
                         Generic APP Source File

                          Filename : APPDEFS.H
                          Version  : 95/03/01
                            Written by Mio

                      Generic APP 定数定義ファイル
*/

#ifndef APPDEFS_H
#define APPDEFS_H

/* パラメータ */

#define RESWBUFSIZ 200          /* 予約語スキャン用バッファサイズ */
#define PNLENGTH   128          /* パス名の最大長 */
#define LBLBFLEN   40           /* ラベル生成用バッファサイズ */

/* 定数 */

#define OFF         0
#define ON          (!OFF)

#define FALSE       0
#define TRUE        (!FALSE)

#ifdef __MSDOS__
# define PATH_SEPL  '\\'
#else
# define PATH_SEPL  '/'
#endif

/* テンポラリ・ラベルの頭 */
#define TMPL_HEAD   "$_"	

/* 特殊処理をする（文字の後に内容が続く）
   コマンドライン・オプションキャラクタ */
#define INCLUDE_OPTION 'I'
#define DEFINE_OPTION  'D'
#define HELP_OPTION    '?'

typedef unsigned char BYTE ;

#endif /* of APPDEFS_H */
