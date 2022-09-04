/*
                         Generic APP Source File

                          Filename : MOVE85.H
                          Version  : 95/03/17
                            Written by Miu

                          データ転送出力処理
*/

#ifndef CODE80_H
#define CODE80_H

#define MAXEXPRLEN      256           /* 評価結果バッファサイズ */

/* コード出力マクロ */

#define OUTPUT1(x)        app_printf("\t%s\n",(x))
#define OUTPUT2(x,y)      app_printf("\t%s\t%s\n",(x),(y))
#define OUTPUT3(x,y,z)    app_printf("\t%s\t%s,%s\n",(x),(y),(z))
#define JNn_OUTPUT(c,l,n) app_printf("\tJR\tN%s,%s_%d\n",(c),(l),(n))
#define Jnn_OUTPUT(c,l,n) app_printf("\tJR\t%s,%s_%d\n",(c),(l),(n))

#define CPU85		8085
#define CPU80		8080
#define CPU180		180
#define ASMZ80		81
#define CPUZ80		80

#define NONEtype	0	/* ｲﾐﾃﾞｨｴｲﾄ,[]			*/
#define BYTEtype	1	/* A,B,C,.....(byte)..		*/
#define WORDtype	2	/* BC,DE,HL,..(word)..		*/

#define BYTEptr		30	/* (BYTE)???????	*/
#define WORDptr		40	/* (WORD)???????	*/

#define BYTEREGptr	50	/* (BYTE)???????	*/
#define BYTEHLptr	51	/* (BYTE)[WORDreg]	*/
#define BYTEBCptr	52	/* (BYTE)[WORDreg]	*/
#define BYTEDEptr	53	/* (BYTE)[WORDreg]	*/
#define BYTEPCptr       54	/* 95/07/08	*/
#define BYTESPptr       55
#define BYTEIXptr	56	/* (BYTE)[WORDreg]	*/
#define BYTEIYptr	57	/* (BYTE)[WORDreg]	*/

#define WORDREGptr	60	/* (WORD)???????	*/
#define WORDHLptr	61	/* (WORD)[WORDreg]	*/
#define WORDBCptr	62	/* (WORD)[WORDreg]	*/
#define WORDDEptr	63	/* (WORD)[WORDreg]	*/
#define WORDPCptr       64	/* 95/07/08	*/
#define WORDSPptr       65
#define WORDIXptr	66	/* (WORD)[WORDreg]	*/
#define WORDIYptr	67	/* (WORD)[WORDreg]	*/

extern int i_cpu ;
extern int ia_option[] ;

#include "code80.prt"

#endif /* of CODE80_H */
