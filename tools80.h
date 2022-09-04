/*
                         Generic APP Source File

                          Filename : TOOLS80.H
                          Version  : 95/03/07
                            Written by Mio

                           APP80 用ツール集
*/

#ifndef TOOLS80_H
#define TOOLS80_H

#define NONE  0
#define Acc   1
#define Breg  2
#define Creg  3
#define Dreg  4
#define Ereg  5
#define Hreg  6
#define Lreg  7
#define Ireg  8		/* 95/07/01	*/
#define Rreg  9

#define WORDreg 10
#define HLreg   11
#define BCreg   12
#define DEreg   13
#define PCreg   14	/* 95/07/01	*/
#define SPreg   15
#define IXreg   16
#define IYreg   17

#define REGptr  20
#define HLptr   21
#define BCptr   22
#define DEptr   23
#define PCptr   24	/* 95/07/08	*/
#define SPptr   25
#define IXptr   26	/* 95/07/06	*/
#define IYptr   27

#include "tools80.prt"

#endif /* of TOOLS80_H */
