_procedure	POOL
	_if	BYTE PTR SISU_ERF = 0 && BYTE PTR SISU_OPN <> 0
		MOV	AX,0		; ���ĂƂ̒ʐM�͐���
	_else
		MOV	SI,OFFSET CNV_12R + 0
		MOV	CX,R12CNT
		OR	BYTE PTR 31[SI],"1"
		_switch
		_case	WORD PTR 1[SI] = "11"	; ID=�莞�d���H
			_PUTRB	TEIJI_PBF , CNV_12R+1 , CX
			_NXTPB	TEIJI_PBF
			MOV	AX,-1		; ���ĂƂ̒ʐM���ُ�A�~�ς���
		_case	WORD PTR 1[SI] = "14"	; ID=ܰ�ݸޓd���H
			_PUTRB	WRN_PBF , CNV_12R+1 , CX
			_NXTPB	WRN_PBF
			MOV	AX,-1		; ���ĂƂ̒ʐM���ُ�A�~�ς���
		_case	WORD PTR 1[SI] = "13"	; ID=�װѓd���H
			_PUTRB	ALM_PBF , CNV_12R+1 , CX
			_NXTPB	ALM_PBF
			MOV	AX,-1		; ���ĂƂ̒ʐM���ُ�A�~�ς���
		_default
			MOV	AX,-1		; ���ĂƂ̒ʐM���ُ�A�~�ς���
		_endsw
	_endif
_endp
;
;
_procedure	POOL_SEND	; �~���ޯ̧���缽�Ăւ̑��M
PS_TEIJI:;
	_CHKGB	TEIJI_PBF		; �莞�v���ް����~�ς���Ă��邩�H
	AND	CX,CX
	JZ	PS_WRN			; No
	_PUT422	PC98,TEIJI_PBF,CX	; �~�ϓd����422�ް�ނ֓n���B
	_PR422TF	PC98
	JMP	PS_END
PS_WRN:	;
	_CHKGB	WRN_PBF			; ܰ�ݸ��ް����~�ς���Ă��邩�H
	AND	CX,CX
	JZ	PS_ALM			; No
	_PUT422	PC98,WRN_PBF,CX		; �~�ϓd����422�ް�ނ֓n���B
	_PR422TF	PC98
	JMP	PS_END
PS_ALM:	;
	_CHKGB	ALM_PBF			; �װ��ް����~�ς���Ă��邩�H
	AND	CX,CX
	JZ	PS_END			; No
	_PUT422	PC98,ALM_PBF,CX		; �~�ϓd����422�ް�ނ֓n���B
	_PR422TF	PC98
	JMP	PS_END
PS_END:	;
_endp
