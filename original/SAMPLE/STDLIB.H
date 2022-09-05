@_procedure	PUTCHAR (DL) : AX,DX
@_procedure	LINEFEED : DX
@_procedure	WRITESTR (^BX) : AX
@_procedure	EXIT (AL)
@_procedure	WRITESTRLN (^BX)
@_procedure	NUMSTR2 (^AX,^CX) : DX
@_procedure	NUMSTR (AX,CX,BX)
@_procedure	PRINTNUM2 (^AX,^CX) : DX
@_procedure	PRINTNUM (AX,CX)
@_function	BINASC (^AX)
@_function	LINEINPUT (^BX) : CX,AX
@_function	TOUPPER (AL) : AX
@_function	GETCHAR : AX
@_function	DIVFRAC (^AX,^BX) : DX,SI,CX,DI
_extrn
