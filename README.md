# app80
Assembler pre-processor for Z80.

These files were created about 25 years ago and are used with sjasm to write small firmware for Z80 CPUs running on FPGAs.
<BR><BR>
The original source code for this program was for 8086 and written in Turbo Pascal. It is stored under [original](original/) folder, and Japanese documentation on preprocessor grammars can be found at [here](original/APPMAN.PRN).
<BR><BR>
C version of app80 can also be built for 8086.
Open [makefile](./makefile) and define -DAPP86 instead of -DAPP80 and then run 
    make app86.exe
<BR>
but some source must be modified to conform to the target assembler's temporary labelling conventions [cond86.h](./cond86.h).
<BR><BR>
MCI Eg. copyright notice on them indicates that they were created and used when I worked for that company, and I have permission from the co-author to release them under the MIT licence.
