	;_procedure	POOL
POOL:
	;	_if	BYTE PTR SISU_ERF = 0 && BYTE PTR SISU_OPN <> 0
	CMP	BYTE PTR SISU_OPN,0
	JE	$ELSE_1
	CMP	BYTE PTR SISU_ERF,0
	JNE	$ELSE_1
	MOV	AX,0		; ｼｽ監との通信は正常
	;	_else
	JMP	$ENDIF_1
$ELSE_1:
	MOV	SI,OFFSET CNV_12R + 0
	MOV	CX,R12CNT
	OR	BYTE PTR 31[SI],"1"
	;	_switch
	;	_case	WORD PTR 1[SI] = "11"	; ID=定時電文？
	CMP	WORD PTR 1[SI],"11"
	JNE	$ELSE_2
	_PUTRB	TEIJI_PBF , CNV_12R+1 , CX
	_NXTPB	TEIJI_PBF
	MOV	AX,-1		; ｼｽ監との通信が異常、蓄積した
	JMP	$ENDIF_2
$ELSE_2:
	;	_case	WORD PTR 1[SI] = "14"	; ID=ﾜｰﾆﾝｸﾞ電文？
	CMP	WORD PTR 1[SI],"14"
	JNE	$ELSE_3
	_PUTRB	WRN_PBF , CNV_12R+1 , CX
	_NXTPB	WRN_PBF
	MOV	AX,-1		; ｼｽ監との通信が異常、蓄積した
	JMP	$ENDIF_3
$ELSE_3:
	;	_case	WORD PTR 1[SI] = "13"	; ID=ｱﾗｰﾑ電文？
	CMP	WORD PTR 1[SI],"13"
	JNE	$ELSE_4
	_PUTRB	ALM_PBF , CNV_12R+1 , CX
	_NXTPB	ALM_PBF
	MOV	AX,-1		; ｼｽ監との通信が異常、蓄積した
	JMP	$ENDIF_4
$ELSE_4:
	;	_default
	MOV	AX,-1		; ｼｽ監との通信が異常、蓄積せず
	;	_endsw
$ENDIF_4:
$ENDIF_3:
$ENDIF_2:
	;	_endif
$ENDIF_1:
	;_endp
	RET
;
;
	;_procedure	POOL_SEND	; 蓄積ﾊﾞｯﾌｧからｼｽ監への送信
POOL_SEND:
PS_TEIJI:;
	_CHKGB	TEIJI_PBF		; 定時計測ﾃﾞｰﾀが蓄積されているか？
	AND	CX,CX
	JZ	PS_WRN			; No
	_PUT422	PC98,TEIJI_PBF,CX	; 蓄積電文を422ﾎﾞｰﾄﾞへ渡す。
	_PR422TF	PC98
	JMP	PS_END
PS_WRN:	;
	_CHKGB	WRN_PBF			; ﾜｰﾆﾝｸﾞﾃﾞｰﾀが蓄積されているか？
	AND	CX,CX
	JZ	PS_ALM			; No
	_PUT422	PC98,WRN_PBF,CX		; 蓄積電文を422ﾎﾞｰﾄﾞへ渡す。
	_PR422TF	PC98
	JMP	PS_END
PS_ALM:	;
	_CHKGB	ALM_PBF			; ｱﾗｰﾑﾃﾞｰﾀが蓄積されているか？
	AND	CX,CX
	JZ	PS_END			; No
	_PUT422	PC98,ALM_PBF,CX		; 蓄積電文を422ﾎﾞｰﾄﾞへ渡す。
	_PR422TF	PC98
	JMP	PS_END
PS_END:	;
	;_endp
	RET
