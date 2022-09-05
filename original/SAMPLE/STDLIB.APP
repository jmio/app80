	.MODEL	TINY
	.CODE
	.286

_function GETCHAR : AX
	MOV	AH,1
	INT	21H
	_return	AX
_endf

_procedure PUTCHAR (DL) : AX,DX
	MOV	AH,2
	INT	21H
_endp

_procedure LINEFEED : DX
	_proc	PUTCHAR(13)
	_proc	PUTCHAR(10)
_endp

_function TOUPPER (AL) : AX
	_if	AL >= 'a' && AL <= 'z'
			SUB	AL,32
	_endif
	_return	AX
_endf

_procedure WRITESTR (^BX) : AX
	_if BYTE PTR [BX] <> 0
		_repeat
			AX = _func TOUPPER(BYTE PTR[BX])
			_proc PUTCHAR(AL)
		_until BYTE PTR [++BX] = 0
        _endif
_endp

_procedure EXIT (AL)
	MOV	AH,4CH
	INT	21H
_endp

_function LINEINPUT(^BX) : CX,AX
	XOR	CX,CX
	_repeat
		INC	CX
		AX = _func GETCHAR
		MOV	[BX],AL
		INC	BX
	_until	AL = 0DH
	DEC	BX
	MOV	BYTE PTR [BX],0
	DEC	CX
	_return	CX
_endf

_procedure WRITESTRLN(^BX)
	_proc	WRITESTR(BX)
	_proc	LINEFEED
_endp

_function BINASC(^AX)
	AND	AX,0FH
	_if	AL <= 9
		ADD	AL,'0'
	_else
		ADD	AL,'A'-10
	_endif
	_return	AX
_endf

_procedure NUMSTR2(^AX,^CX) : DX
	_if	AX > 0
		XOR	DX,DX
		IDIV	CX		; ax = dx:ax / cx , dx = dx:ax mod cx
		_proc 	NUMSTR2(AX,CX)
		DX = 	_func BINASC(DX)
		MOV	BYTE PTR [BX],DL
		INC	BX
	_endif
_endp

_procedure NUMSTR(AX,CX,BX)
	_if	AX = 0
		MOV	BYTE PTR [BX],'0'
		INC	BX
	_else
		_proc	NUMSTR2(AX,CX)
	_endif
	MOV	BYTE PTR [BX],0
_endp

_procedure PRINTNUM2(^AX,^CX) : DX
	_if	AX > 0
		XOR	DX,DX
		IDIV	CX		; ax = dx:ax / cx , dx = dx:ax mod cx
		_proc 	PRINTNUM2(AX,CX)
		DX = 	_func BINASC(DX)
		_proc	PUTCHAR(DL)
	_endif
_endp

_procedure PRINTNUM(AX,CX)
	_if	AX = 0
		_proc	PUTCHAR('0')
	_else
		_proc	PRINTNUM2(AX,CX)
	_endif
_endp

_function DIVFRAC(^AX,^BX) : DX,SI,CX,DI
	XOR	SI,SI
	_if	BX > 0
		XOR	DX,DX
		IDIV	BX		; ax = dx:ax / cx , dx = dx:ax mod cx
		_loopcx	4
			_if	DX = 0
				_exit
			_endif
			MOV	DI,10
			MOV	AX,DX
			MUL	DI
			IDIV	BX
			AND	AX,0FH
			PUSH	CX
			DEC	CX
			SHL	CX,2
			SHL	AX,CL
			POP	CX
			OR	SI,AX
		_endl
	_endif
	_return	SI
_endf
	_public
	END
