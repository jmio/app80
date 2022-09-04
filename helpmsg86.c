char HELPMSG[] = " \
Usage : APP86 inputfile[.APP] [Outputfile[.ASM or .H]] [-options] \n \
 \n \
     -H       Make APP header file \n \
     -O       Old fashion (Mio's APP Compatible) conversion \n \
     -N       NEAR conditional \"JMP\" output \n \
     -Dsym    Define APP internal symbol       (e.g. -DCPU286) \n \
     -Ipath   Search path for include files    (e.g. -Ia:\\ -Ia:\\applib) \n \
     -R       Make global symbol xxx_END: with all \"_endf\" \n \
     -S       Conditional jump output \"Signed\" code (same as _signed ) \n \
     -?       Display this help message \n \
";
