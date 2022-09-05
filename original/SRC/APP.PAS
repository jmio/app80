{

                 80x86 アセンブラ構造化支援簡易プリプロセッサ 

                             Ｍｉｏ’ｓ　ＡＰＰ

                  Copyright (C) Mio ( Naoki Nishiyama ) 1992 

　　　　　　    　　　　　　Version "Private" β


                                - History -

       ＜公開版＞
       Version 1.40u : シンボルなどを大文字に変換しないオプションを追加。
       Version 1.40p : func,proc の出口でネストの整合チェックを行うようにした。
       Version 1.40i : for ループの増／減値に１以外を指定できるようにした。
       Version 1.40f : 関数、プロシージャからのリターン部分をまとめる。
                       サブルーチンの終わりにラベル生成を行うようにした。
       Version 1.40  : 擬似 2 Pass 時の Pass 1 でファイルインクルードを
                       行うようにした。
                       ジャンプ命令スキップをラベル生成で行うようにした。
       Version 1.39z : func の引き数処理の不具合修正
       Version 1.39v : 疑似 2 Pass 時ヘッダか付かないように変更
       Version 1.39  : function システムの実現方法改良試験版
                       ・BP レジスタのみに依存する方式に変更
                       ・func,proc の引き数に関数が記述可能
                       ・return 文に関数を記述可能
       Version 1.34  : 条件式保存変数一本化時のバグ fix , 型指定の試験的導入
       Version 1.33  : 予約語処理部分のバグ fix
       Version 1.32  : 予約語判別部の改変（プロファイリング解析、高速化）
       Version 1.31  : for 文の不具合修正、パラメータ省略対応
       Version 1.30  : 条件式保存変数一本化、exit,continue 多重脱出
       Version 1.21  : ソースファイルの整理、ヘルプ分割リンク
       Version 1.20  : 分割アセンブル , 擬似 2 Pass サポート
       Version 1.00  : function , procedure サポート

}

{$M 32000,0,655350}
Uses crt,dos ;

Type
 fptype = (f,p) ;
 pptype = (pre,post) ;
 cstptr = ^cst ;
 cst = record
        name : string[40] ;
        str  : string ;
        next : cstptr ;
       end ;

Const
 { Version }
 Version     = 'Ver 1.40u' ;

 { Capacity }
 IfNestmax   = 128 ;
 LoopNestmax = 32 ;
 Funcmax     = 550 ;
 Argmax      = 10 ;
 FnArgmax    = 16 ;

 lineno   : word = 0 ;
 iflev    : word = 0 ;
 ifnum    : word = 0 ;
 lplev    : word = 0 ;
 whlev    : word = 0 ;
 whnum    : word = 0 ;
 swlev    : word = 0 ;
 funcs    : word = 0 ;
 tmpnum   : word = 0 ;
 skpnum   : word = 0 ;
 delaystr : string[80] = '' ;
 delayflg : boolean = false ;
 Checkbp  : boolean = false ;
 tempchr  : string[1] = '$' ;
 csttop   : cstptr = nil ;

 Resws = 31 ;
 reserved_word : array [1..Resws] of string[11] =
   ( '_if' ,
     '_else' ,
     '_endif' ,
     '_while' ,
     '_wend' ,     { #5 }
     '_continue' ,
     '_exit' ,
     '_switch' ,
     '_case' ,
     '_default' ,  { #10 }
     '_endsw' ,
     '_repeat' ,
     '_until' ,
     '_near' ,
     '_short' ,    { #15 }
     '_function' ,
     '_return' ,
     '_func' ,
     '_endf' ,
     '_procedure', { #20 }
     '_endp' , 
     '_proc' ,
     '_loopcx' , 
     '_endl' ,
     '_for' ,      { #25 }
     '_next' ,
     '_extrn' , 
     '_public' ,
     '_include' ,
     '_constout' ,
     '_endpi' ) ; { #31 }

 { Misc Const. }
 Tmpfilename = '$APPTMP$.$$$' ;
 Tab = #9 ;

 { App Op. }
  OpEq  = '=' ; { Char type }
  OpLt  = '<' ;
  OpGt  = '>' ;
  OpBit = '&' ;
  OpNbt = '~' ;
  OpNot = '!' ;
  Operators = OpEq + OpLt + OpGt + OpBit + OpNbt + OpNot ;

  OpLand = '&&' ; { Str Type }
  OpLor  = '||' ;
  Opincr = '++' ;
  Opadd  = '+=' ;
  Opdecr = '--' ;
  Opsub  = '-=' ;

  CastByte = '(BYTE)' ;
  CastWord = '(WORD)' ;
  Byteptr = 'BYTE PTR ' ;
  Wordptr = 'WORD PTR ' ;
  Retreg = 'BP' ;

  UnderBar = '_' ; { ResWord Mark }
  PreHchar = '@' ; { Pre Header Mark }
  Comment  = ';' ; { Comment Mark }
  Quote1   = '''' ; { Quote 1 }
  Quote2   = '"'  ; { Quote 2 }
  Quotes   = Quote1 + Quote2 ;
  Reps = '({[' ;
  Repe = ')}]' ;

  SaveSep = ':' ; { Function / Procedure savereg separator }
  None = 'NONE' ; { NoRegs save }
  Norec = 'NONREC' ; { NonRecursive .. Nosave BP (Before 1.39) }
  Flet = '=' ; { Function Retval or For start let }

  SaveMark = '^' ;
  XchgMark = '!' ;
  PtrMark  = '*' ;

  Spc = ' ' ;
  ResDelim = ', ;'+TAB ;

 { Tmp Label Symbol }
  CondTmp = 'TMP_' ;
  CondSkp = 'SKP_' ;

  IfElse = 'ELSE_' ;
  IfEnd  = 'ENDIF_' ;

  LoopBegin = 'BEGIN_' ;
  LoopCont  = 'CONT_' ;
  LoopExit  = 'EXIT_' ;
  ForBegin  = 'FORBG_' ;
  Endpstr   = '_END' ;
  Gensymstr = 'CTS_' ;

  Cstdefine = 'DB' ;
  Cstdelim = ',0' ;
  Ofsstr = 'OFFSET ' ;

 { 80x86 Instruction }
  Move    = 'MOV' ;
  Lea     = 'LEA' ;
  Compare = 'CMP' ;
  Jump    = 'JMP' ;
  BitTest = 'TEST' ;
  Add     = 'ADD' ;
  Sub     = 'SUB' ;
  Incr    = 'INC' ;
  Decr    = 'DEC' ;
  Push    = 'PUSH' ;
  Pop     = 'POP' ;
  Loop    = 'LOOP' ;
  Xchg    = 'XCHG' ;
  Jcxz    = 'JCXZ' ;
  Call    = 'CALL' ;
  Ret     = 'RET' ;
  Reti    = 'IRET' ;

  PreJcc  = 'J' ;

  JccEqu   = 'E' ;
  JccNot   = 'N' ;
  JccAbove = 'A' ;
  JccBelow = 'B' ;
  JccZero  = 'Z' ;

  JccNE    = JccNot   + JccEqu ;
  JccBE    = JccBelow + JccEqu ;
  JccAE    = JccAbove + JccEqu ;
  JccNZ    = JccNot   + JccZero ;

  SkipJmp = '$+5' ;

  StackReg1 = 'SP' ;
  StackReg2 = 'BP' ;
  StackTop  = 'WORD PTR [BP]' ;
  StackWidth = '2' ;
  CountReg  = 'CX' ;

  Extrn1  = 'EXTRN' ;
  Extrn2  = ':NEAR' ;
  Public  = 'PUBLIC' ;

Var
 Stdout : Text ;
 curfile : string[80] ;
 i,lastfunc,find : word ;
 lsave : array [1..LoopNestmax] of string[80] ;
 Option : array[#32..#128] of boolean ;
 Param : array[0..Argmax] of string[64] ;
 Infile , Outfile , Lnkfile : Text ;
 Line : string ;
 rifn : array [1..IfNestmax] of word ;
 rwhn : array [1..LoopNestmax] of word ;
 mexit,mcont : array [0..LoopNestmax] of boolean ;
 felse : array [0..IfNestmax] of boolean ;

 {Function Table}
 optsave,foundret,nonrec : boolean ;
 funsave : String[80] ;
 predef  : array [1..Funcmax] of boolean ;
 funtype : array [1..Funcmax] of fptype ;
 funname : array [1..Funcmax] of String[16] ;
 funarg  : array [1..Funcmax] of String[80] ;
 funcall : array [1..funcmax] of word ;

Procedure OutputResw ;
Var i : word ;
Begin
 Writeln(Stdout) ;
 Writeln(Stdout,'< APP 予約語一覧 >') ;
 for i := 1 to Resws do Write(Stdout,copy(reserved_word[i]+'                    ',1,20)) ;
 Writeln(Stdout) ; Writeln(Stdout) ;
 Writeln(Stdout,'Max procedures definitions           : ',Funcmax:5) ;
 Writeln(Stdout,'Max Conditional Branch Nesting Level : ',IfNestmax:5) ;
 Writeln(Stdout,'Max Loop Block Nesting Level         : ',LoopNestmax:5) ;
 Halt(0) ;
end ;

Procedure PPerr ( errno : word ) ;
Var
 msg : string ;
Begin
 Write(Stdout,#13) ;
 Write(Stdout,Curfile,'(',Lineno,'):[',errno,'] ') ;
 Case errno of
   { fatal error }
   2 : msg :='ファイル名の指定が正しくありません' ;
   3 : msg :='if と endif の数か一致していません' ;
   4 : msg :='while~wend,repeat~until 又は for~next の数が一致していません';
   5 : msg :='起動時のパラメータの数が多すぎます' ;
   6 : msg :='入出力ファイル名が同じです' ;
   7 : msg :='continue,exit はループ中に無ければなりません' ;
   8 : msg :='switch に対する endsw が見当たりません' ;
   9 : msg :='if ブロックの外に else があります' ;
  10 : msg :='function,procedure の書式が正しくありません' ;
  11 : msg :='非再帰型関数の中では '+StackReg2+' レジスタを操作することはできません' ;
  12 : msg :='continue,exit の脱出レベルが大きすぎます' ;
  13 : msg :='関数、手続きが未定義です（前方参照は２パス変換が必要です）' ;
  14 : msg :='この名前の関数、手続きは既に定義済みです' ;
  15 : msg :='func,proc 呼び出しの引き数の数か一致しません' ;
  16 : msg :='func,proc 呼び出しの引き数のかっこの対応に誤りがあります' ;
  17 : msg :='if,switch のネストが深すぎます' ;
  18 : msg :='ループのネストが深すぎます' ;
  20 : msg :='関数、手続きのブロックが閉じていません' ;
  21 : msg :='手続きから戻り値を得ることは出来ません' ;
  30 : msg :='関数を手続きとして呼び出すことは出来ません' ;
  31 : msg :='手続きを関数として呼び出すことは出来ません' ;
  33 : msg :='関数、手続きブロック外に return 文を書くことは出来ません' ;
  40 : msg :='function に対して endp を用いることは出来ません' ;
  41 : msg :='procedure に対して endf を用いることは出来ません' ;
  50 : msg :='関数、手続きブロック外に endp , endf を書くことは出来ません' ;
  62 : msg :='テンポラリーファイルがオープンできませんでした' ;
  63 : msg :='テンポラリーファイルの消去に失敗しました' ;
  70 : msg :='バイトレジスタをセーブすることはできません' ;
  80 : msg :='宣言形式が一致しない同名のサブルーチンがあります' ;
  81 : msg :='引き数形式が一致しない同名のサブルーチンがあります' ;
  83 : msg :='for 文の引き数が正しくありません' ;
  84 : msg :='next 文は for にのみ対応します' ;
  90 : msg :='括弧の対応が取れていません' ;
  91 : msg :='クオートの対応が取れていません' ;
  97 : msg := Opincr+' , '+Opdecr+' を変数の後に付けられるのは while のみです';
  99 : msg := Opland+' と '+Oplor+' を組み合わせて使うことは出来ません。';
  { not fatal }
  101 : msg :='警告：条件式の書き方が誤っています' ;
  102 : msg :='警告：function 中に return 文が見つかりません' ;
  120 : msg :='警告：引き数の受け渡しがうまくいっていない可能性があります' ;
  122 : msg :='警告：即値を書けない場所に即値があります' ;
  123 : msg :='警告：即値に型指定子を付けることはできません' ;
  140 : msg :='警告：予約語の綴りが間違っています' ;
  150 : msg :='警告：'+Opincr+' , '+Opdecr+' を即値に付けることはできません' ;
  151 : msg :='警告：'+Opadd+' , '+Opsub+' を即値に付けることはできません' ;
 else
  msg := '未定義エラー' ;
 end ;
 Writeln(Stdout,msg) ;
 If errno < 100 then begin
  Writeln(Stdout,'Fatal error.. 中断しました') ;
  {$I-}
  Assign(Infile,Tmpfilename) ;
  Erase(Infile) ;
  {$I+}
  halt(errno) ;
 end ;
end ;

Function Inchar ( chkstr : string ; chkchar : char ; i : word ) : word ;
Assembler ;
 Asm
  Sub BX,BX
  Sub CX,CX

  Mov AL,Chkchar
  Mov SI,I

  Les DI,Chkstr
  Seges
  Mov CL,[DI]
  Jcxz @Nonfound
  Add DI,SI
  Sub CX,SI
  Jb @Nonfound
  Mov DX,CX
  Inc CX

  Cld
  Cli
  Seges
  RepNz ScasB
  Sti
  Jne @Nonfound

  Sub DX,CX
  Add DX,SI
  Mov BX,DX
@Nonfound:
  Mov AX,BX
end ;

Function Gettok1 (Var start : word ;
                  line,Rephstr1,Rephstr2,Quotestr,Delimstr : string ) : string ;Var
 Res   : string ;
 Len,i,r,totallevel : word ;
 tmpquote,c  : char ;
 Quote : boolean ;
Begin
 tmpquote := #$ff ;
 quote := false ;
 totallevel := 0 ;
 Res := '' ;
 i   := start ;
 Len := length(line) ;
 If i <= Len then begin
  c := line[i] ;
  while (i <= len) and 
        ((inchar(delimstr,c,1) = 0) or (totallevel > 0) or quote) do begin
   If not(quote) then begin
    If inchar(quotestr,c,1) > 0 then begin
     quote := true ;
     tmpquote := c ;
    end ;
    If inchar(rephstr1,c,1) > 0 then Inc(totallevel)
      else If totallevel > 0 then if inchar(rephstr2,c,1) > 0 then dec(totallevel)
   end else begin
    If c = tmpquote then quote := false ;
   end ;
   res := res + c ;
   inc(i) ;
   c := line[i]
  end ;
  If i <= len then start := i + 1 else start := len + 1 ;
 end ;
 If totallevel > 0 then ppERR(90) ;
 If Quote then ppERR(91) ;
 Gettok1 := res ;
end ;

Function Gettok2 (Var start : word ;
                  line,Getstr : string ) : string ;
Var
 Res   : string ;
 Len,i : word ;
 c     : char ;
Begin
 Res := '' ;
 i   := start ;
 Len := length(line) ;
 If i <= Len then begin
  c := line[i] ;
  while (i <= len) and (inchar(getstr,c,1) > 0) do begin
   res := res + c ;
   inc(i) ;
   c := line[i]
  end ;
  If i <= len then start := i else start := len + 1 ;
 end ;
 Gettok2 := res ;
end ;

Procedure appmsg ; external ;
{$L appmsg.obj}

Procedure textout (strp : pointer) ;
Type
 pt = record ofs,seg : word ; end ;
Begin
 While char(strp^) <> #0 do begin
  Write(Stdout,char(strp^)) ;
  inc(pt(strp).ofs) ;
 end ;
end ;

Procedure Helpmsg ;
Begin
 textout(@appmsg) ;
 Halt(0) ;
end ;

Function Cutspc (x : string) : string ; forward ;
Procedure Toupperstr(var s)  ;
var
 st : array [0..255] of char absolute s ;
begin
 If not(option['U']) then for i := 1 to byte(st[0]) do st[i] := upcase(st[i]) ;
end ;

Procedure Cstpop(var p : cstptr) ;
Begin
 If p = nil then exit ;
 Cstpop(p^.next) ;
 Writeln(Outfile,p^.name,TAB,p^.str) ;
 Dispose(p) ;
 p := nil ;
end ;

Procedure Cstpush(n,s : string) ;
Var p : cstptr ;
Begin
 new(p) ;
 p^.name := n ;
 p^.str := s ;
 p^.next := csttop ;
 csttop := p ;
end ;

Function Gensym(f:boolean) : string ;
Const
 Gensymctr : word = 0 ;
Var
 g : string ;
Begin
 if f then inc(Gensymctr) ;
 Str(gensymctr,g) ;
 Gensym := Gensymstr+g ;
end ;

Procedure Toupperstr_noopt(var s) ;
var
 st : array [0..255] of char absolute s ;
begin
 for i := 1 to byte(st[0]) do st[i] := upcase(st[i]) ;
end ;

Function IsImm ( s : string ) : boolean ;
begin
 IsImm := ((s[1] in ['0'..'9']) and (s[length(s)] in ['0'..'9','B','H','b','h'])) ;
end ;

Function ByteRegs ( s : string ) : boolean ;
Begin
 Byteregs := ((length(s)=2) and (s[1] in ['a'..'d','A'..'D']) and
              (s[2] in ['l','h','L','H']));
end ;

{ File out Procedures }

Procedure Labelout ( name : string ; num : word ) ;
Begin
 Writeln (Outfile,tempchr,name,num,':') ;
end ;

Procedure Jmpout ( name1,name2 : string ; num : word ) ;
Begin
 Writeln (Outfile,TAB,name1,TAB,tempchr,name2,num) ;
end ;

Procedure Pushout ( name : string ) ;
Begin
 Toupperstr(name) ;
 if byteregs(name) then ppERR(70) ;
 Writeln (Outfile,TAB,Push,TAB,name) ;
end ;

Procedure Popout ( name : string ) ;
Begin
 Toupperstr(name) ;
 If IsImm(name) then ppERR(122) ;
 Writeln (Outfile,TAB,Pop,TAB,name) ;
end ;

Procedure Typecast (var s : string) ;
Var p : string ;

 Function Chkimm ( s : string ) : string ;
 Begin
  if isimm(s) then ppERR(123) ;
  Chkimm := s ;
 end ;

Begin
 if (s[1] <> '(') then exit ;
 p := copy(s,1,length(castbyte)) ;
 toupperstr_noopt(p) ;
 if Castbyte = p then 
  s := byteptr+chkimm(cutspc(copy(s,length(Castbyte)+1,255)));
 if Castword = p then 
  s := wordptr+chkimm(cutspc(copy(s,length(Castword)+1,255)));
end ;

Procedure Movout ( arg1,arg2 : string ) ;
Begin
 Toupperstr(arg1) ;
 if pos('''',arg2) + pos('"',arg2) = 0 then toupperstr(arg2) ;
 If IsImm(arg1) then ppERR(122) ;
 Typecast(arg1) ;
 Typecast(arg2) ;
 If arg2[1]='"' then begin
  cstpush(gensym(true),cstdefine+TAB+arg2+cstdelim) ;
  arg2 := ofsstr+gensym(false) ;
 end ;
 If arg1 <> arg2 then Writeln (Outfile,TAB,Move,TAB,arg1,',',arg2) ;
end ;

Procedure Leaout ( arg1,arg2 : string ) ;
Begin
 Toupperstr(arg1) ;
 If arg2[1]='"' then begin
  cstpush(gensym(true),cstdefine+TAB+arg2+cstdelim) ;
  arg2 := gensym(false) ;
 end ;
 if pos('''',arg2) + pos('"',arg2) > 0 then ppERR(122) ;
 If IsImm(arg1) or IsImm(arg2) then ppERR(122) ;
 If '['+arg1+']' <> arg2 then Writeln (Outfile,TAB,Lea,TAB,arg1,',',arg2) ;
end ;

Procedure Arg2out ( name,arg1,arg2 : string ) ;
Begin
 Toupperstr(arg1) ;
 if pos('''',arg2) + pos('"',arg2) = 0 then toupperstr(arg2) ;
 Typecast(arg1) ;
 Typecast(arg2) ;
 If IsImm(arg1) then ppERR(122) ;
 Writeln (Outfile,TAB,name,TAB,arg1,',',arg2) ;
end ;

Procedure Auxout ( name : string ) ;
Begin
 Writeln (Outfile,TAB,name) ;
end ;

Procedure Auxout2 ( name : string ) ;
Begin
 Writeln (Outfile,name) ;
end ;

{ File out end }

Procedure Gettoken (start : word ; line : string ; var arg1,cond,arg2 : string) ; forward ;
Procedure makeincr (var s : string ; chkstr,mnm : string ; pp : pptype ) ;
Var
 i,d,j : word ;
 r     : string ;
Begin
 r := s ;
 i := pos(chkstr,s) ;
 j := i ;
 if i > 0 then
  if (i = 1) or (s[i-1] = '[') then begin {pre incr/decr}
   if pp = post then exit ;
   i := i + length(chkstr) ;
   s := cutspc(gettok1(i,s,reps,repe,Quotes,'+-|'+Operators+Comment)) ;
   if (s[length(s)]=']') and (pos('[',s)=0) then s := copy(s,1,length(s)-1) ;
   toupperstr(s) ;
   if s <> '' then if not(isimm(s)) then begin
    Typecast(s) ;
    Auxout(mnm+TAB+s) ;
   end else ppERR(150);
   s := cutspc(copy(r,1,j-1) + copy(r,j+length(chkstr),255)) ;
  end else begin {post incr/decr}
   if pp = pre then ppERR(97) ;
   s := copy (s,1,i-1) + copy(s,i+length(chkstr),255) ;
   d := pos('[',s) ;
   r := s ;
   if (d>0) and (pos(']',r)>=i) then begin
    d := d + 1 ;
    s := Gettok1(d,s,reps,repe,Quotes,'+-|]'+Operators+Comment) ;
   end ;
   d := pos(Oplor,s) ;
   if d > 0 then s := copy(s,1,d-1) ;
   d := pos(Opland,s) ;
   if d > 0 then s := copy(s,1,d-1) ;
   s := cutspc(s) ;
   toupperstr(s) ;
   if s <> '' then if not(isimm(s)) then begin
    Typecast(s) ;
    Auxout(mnm+TAB+s) ;
   end else ppERR(150) ;
   s := r ;
  end ;
end ;

Procedure makeadsb (var s : string ; chkstr,mnm : string ) ;
Var
 i,j : word ;
 r     : string ;
Begin
 r := s ;
 i := pos(chkstr,s) ;
 j := i ;
 if i > 0 then begin {post add/sub}
  r := cutspc(copy(s,i+length(chkstr),255)) ;
  s := cutspc(copy(s,1,i-1)) ;
  if s <> '' then if not(isimm(s)) then begin
   toupperstr(s) ;
   Typecast(s) ;
   if pos(Quote1,r) + pos(Quote2,r) = 0 then toupperstr(r) ;
   Auxout(mnm+TAB+s+','+r) ;
  end else ppERR(150) ;
  s := r ;
 end ;
end ;

Procedure Evalfunctione (start : word ; line : string ; forp : fptype ) ; forward ;
Procedure CondExp( arg1 , cond , arg2 : string ; Reverse : boolean ; jpstr : string ; jpnum,level : word ) ;
Var Condstr,ar1,co1,ar2 : string ;
    tmp,tmp1,tmp2 : word ;
    maketmp,spflg : boolean ;

 Function CondRev ( cond : string ) : string ;
 Begin
  if cond[1] <> JccNot then cond  := JccNot + cond 
                       else cond := copy(cond,length(JccNot)+1,255) ;
  Condrev := cond ;
 end ;

 Procedure TestExp ;
  Begin
   case cond[1] of
    Opbit : Condstr := JccNZ ;
    Opnbt : Condstr := JccZero ;
   end ;
   If Not(Reverse) then Condstr := CondRev(Condstr) ;
   Arg2out(Bittest,arg1,arg2) ;
  end ;

 Procedure CmpExp ;
  Begin
   case cond[1] of
    Opeq  : Condstr := JccEqu ;
    Oplt  : If length(cond)=1 then Condstr := JccBelow else
            If cond[2] = Opeq then Condstr := JccBE else Condstr := JccNE ;
    Opgt  : If length(cond)=1 then Condstr := JccAbove else Condstr := JccAE ;
    OpNot : Condstr := JccNE ;
   else   Condstr := cond ;
   end ;
   If Not(Reverse) then Condstr := CondRev(Condstr) ;
   Arg2out(Compare,arg1,arg2) ;
  end ;

Begin
 maketmp := false ;
 tmp1 := pos(Oplor,arg2) ;
 tmp2 := pos(Opland,arg2) ;
 if (tmp1 >0) and (tmp2 > 0) then ppERR(99) ;
 if find in [5,26] then begin tmp := tmp2 ; tmp2 := tmp1 ; tmp1 := tmp end ;
 if tmp1 > 0 then begin
  Gettoken(tmp1+2,arg2,ar1,co1,ar2) ;
  if level = 0 then begin
   Condexp(ar1,co1,ar2,not(reverse),CondTmp,tmpnum,level+1) ;
   maketmp := true ;
  end else Condexp(ar1,co1,ar2,reverse,CondTmp,tmpnum,level+1) ;
  arg2 := cutspc(copy(arg2,1,tmp1-1)) ;
 end ;
 if tmp2 > 0 then begin
  Gettoken(tmp2+2,arg2,ar1,co1,ar2) ;
  Condexp(ar1,co1,ar2,reverse,jpstr,jpnum,level+1) ;
  arg2 := cutspc(copy(arg2,1,tmp2-1)) ;
 end ;
 spflg := false ;
 tmp := pos(reserved_word[18],arg1) ; { call function check }
 if tmp > 0 then begin
  EvalfunctionE(tmp+5,arg1,f) ;
  arg1 := Retreg ;
  spflg := true ;
 end ;
 makeincr(arg1,Opincr,incr,pre) ;
 makeincr(arg2,Opincr,incr,pre) ;
 makeincr(arg1,Opdecr,decr,pre) ;
 makeincr(arg2,Opdecr,decr,pre) ;
 If (arg1 = '') then ppERR(101) ;
 if isimm(arg1) then ppERR(122) ;
 If cond = '' then condstr := arg1 else
  if (cond = Opbit) or (cond = Opnbt + Opbit) then TestExp else CmpExp ;
 if length(condstr) > 3 then ppERR(101) ;
 { if spflg then Popout('BP') ; }
 if option['N'] then begin
  Jmpout(PreJcc+condstr,Condskp,skpnum) ;
  Jmpout(Jump,jpstr,jpnum) ;
  Labelout(CondSkp,Skpnum) ;
  Inc(skpnum) ;
 end else begin
  Jmpout(PreJcc+CondRev(condstr),jpstr,jpnum) ;
 end ;
 if maketmp then begin 
  Labelout(CondTmp,tmpnum) ;
  inc(tmpnum) ;
 end ;
end ;

Procedure Evalif( arg1 , cond , arg2 : string ) ;
Begin
 Inc(iflev) ;
 If iflev > IfNestMax then ppERR(17) ;
 Inc(ifnum) ;
 rifn[iflev] := ifnum ;
 felse[iflev] := false ;
 CondExp ( arg1 , cond , arg2 , true , IfElse , ifnum ,0) ;
End ;

Procedure Evalelse ;
Begin
 if iflev = 0 then ppERR(9) ;
 felse[iflev] := true ;
 if not(delayflg) then Jmpout(Jump,IfEnd,rifn[iflev]) ;
 Labelout(IfElse,rifn[iflev]) ;
end ;

Procedure Evalendif ;
const
 elsestr : array[boolean] of string[10] = ( IfElse , IfEnd ) ;
Begin
 if iflev = 0 then ppERR(3) ;
 Labelout(elsestr[felse[iflev]],rifn[iflev]) ;
 Dec(iflev) ;
end ;

Procedure Evalwhile( arg1 , cond , arg2 : string ) ;
Begin
 Inc(whlev) ;
 if whlev > LoopNestMax then ppERR(18) ;
 Inc(whnum) ;
 Jmpout(Jump,LoopCont,whnum)  ;
 Labelout(LoopBegin,whnum) ;
 rwhn[whlev] := whnum ;
 mcont[whlev] := true ;
 mexit[whlev] := false ;
 makeincr(arg1,Opincr,incr,post) ;
 makeincr(arg2,Opincr,incr,post) ;
 makeincr(arg1,Opdecr,decr,post) ;
 makeincr(arg2,Opdecr,decr,post) ;
 if arg1 = '' then ppERR(101) ;
 if cond = '' then if arg1 = arg2 then arg2 := '' ;
 lsave[whlev] := arg1+' '+cond+' '+arg2 ;
End ;

Procedure Evalfor( start : word ; line : string ) ;
var
 letc,rest : string ;
 arg1,cond,arg2 : string ;
Begin
 Inc(whlev) ;
 if whlev > LoopNestMax then ppERR(18) ;
 Inc(whnum) ;
 letc := cutspc(Gettok1(start,line,reps,repe,quotes,','+comment)) ;
 if start > length(line) then ppERR(83) ;
 rest := cutspc(copy(line,start,255)) ;
 Gettoken(1,letc,arg1,cond,arg2) ;
 if cond = flet then Movout(arg1,arg2) ;
 if rest[1] <> ',' then Jmpout(Jump,ForBegin,whnum)  ;
 Labelout(LoopBegin,whnum) ;
 rwhn[whlev] := whnum ;
 mcont[whlev] := false ;
 mexit[whlev] := false ;
 lsave[whlev] := rest ;
End ;

Procedure Evalwend ;
Var
 arg1,cond,arg2 : string ;
Begin
 if whlev = 0 then ppERR(4) ;
 Gettoken(1,lsave[whlev],arg1,cond,arg2) ;
 Labelout(LoopCont,rwhn[whlev]) ;
 CondExp ( arg1 , cond , arg2 , false  , LoopBegin , rwhn[whlev] ,0) ;
 If mexit[whlev] then LabelOut(LoopExit,rwhn[whlev]) ;
 Dec(whlev)
end ;

Procedure Evalnext ;
var
 tmp : word ;
 s,condc,arg1,cond,arg2,argt : string ;
Begin
 if whlev = 0 then ppERR(4) ;
 s := lsave[whlev] ;
 if s = '' then ppERR(84) ;
 tmp := 1 ;
 condc := cutspc(gettok1(tmp,s,reps,repe,quotes,','+comment)) ;
 argt := cutspc(copy(s,tmp,255)) ;
 if mcont[whlev] then Labelout(LoopCont,rwhn[whlev]) ;
 makeincr(argt,Opincr,incr,post) ;
 makeincr(argt,Opdecr,decr,post) ;
 makeincr(argt,Opincr,incr,pre) ;
 makeincr(argt,Opdecr,decr,pre) ;
 makeadsb(argt,Opadd,add) ;
 makeadsb(argt,Opsub,sub) ;
 Labelout(ForBegin,rwhn[whlev]) ;
 Gettoken(1,condc,arg1,cond,arg2) ;
 if cond = '' then arg2 := arg1 ;
 if (cond <> '') then begin
  if tmp = 0 then tmp := 255 ;
  CondExp (arg1,cond,cutspc(copy(arg2,1,tmp-1)),false,LoopBegin,rwhn[whlev],0)
 end else Jmpout (Jump,LoopBegin,rwhn[whlev]) ;
 If mexit[whlev] then Labelout(LoopExit,rwhn[whlev]) ;
 Dec(whlev)
end ;

Procedure Evalrepeat ;
Begin
 Inc(whlev) ;
 if whlev > LoopNestMax then ppERR(18) ;
 Inc(whnum) ;
 rwhn[whlev] := whnum ;
 mcont[whlev] := false ;
 mexit[whlev] := false ;
 lsave[whlev] := '' ;
 Labelout(LoopBegin,whnum) ;
End ;

Procedure Evalloopcx (cxval : string) ;
Var
 tmp : word ;
Begin
 Inc(whlev) ;
 if whlev > LoopNestMax then ppERR(18) ;
 Inc(whnum) ;
 rwhn[whlev] := whnum ;
 mcont[whlev] := false ;
 mexit[whlev] := false ;
 lsave[whlev] := '' ;
 If lplev > 0 then Pushout(CountReg) ;
 Inc(lplev) ;
 If cxval <> '' then Movout(CountReg,cxval) ;
 If not(IsImm(cxval)) then begin
  Jmpout(Jcxz,LoopExit,whnum) ;
  mexit[whlev] := true ;
 end ;
 Labelout(LoopBegin,whnum) ;
End ;

Procedure Evaluntil (arg1,cond,arg2 : string) ;
Begin
 if whlev = 0 then ppERR(4) ;
 if mcont[whlev] then LabelOut(LoopCont,rwhn[whlev]) ;
 CondExp ( arg1 , cond , arg2 , true , LoopBegin , rwhn[whlev] ,0) ;
 if mexit[whlev] then LabelOut(LoopExit,rwhn[whlev]) ;
 Dec(whlev)
end ;

Procedure Evalendl ;
Begin
 if whlev = 0 then ppERR(4) ;
 if mcont[whlev] then LabelOut(LoopCont,rwhn[whlev]) ;
 Jmpout(loop,LoopBegin,rwhn[whlev]) ;
 if mexit[whlev] then LabelOut(LoopExit,rwhn[whlev]) ;
 Dec(lplev) ;
 If lplev > 0 then Popout(CountReg) ;
 Dec(whlev)
end ;

Procedure Evalcontinue ( clev : word ) ;
Begin
 if whlev = 0 then ppERR(7) ;
 if whlev <= clev then ppERR(12) ;
 mcont[whlev - clev] := true ;
 Jmpout(Jump,LoopCont,rwhn[whlev - clev]) ;
end ;

Procedure Evalexit ( elev : word ) ;
Begin
 if whlev = 0 then ppERR(7) ;
 if whlev <= elev then ppERR(12) ;
 mexit[whlev - elev] := true ;
 Jmpout(Jump,LoopExit,rwhn[whlev - elev]) ;
end ;

Procedure Comment_out ; forward ;
Procedure Evalcase ( arg1 , cond , arg2 : string ) ;
Begin
 if swlev > 0 then Evalelse ;
 Comment_out ;
 Inc(swlev) ;
 Evalif ( arg1 , cond , arg2 ) ;
end ;

Procedure Evalendsw ;
begin
 While swlev > 0 do begin
  Dec(Swlev) ;
  Evalendif ;
 end ;
end ;

Function Cutspc (x : string) : string ;
 var i,j : word ;
begin
  i := 1 ; j := length(x) ;
  while (x[i] in [TAB,#32]) and (i < j) do inc(i) ;
  x := copy(x,i,255) ;
  i := length(x) ;
  while (x[i] in [TAB,#32]) and (i > 1) do dec(i) ;
  x := copy(x,1,i) ;
  if x <= ' ' then x := '' ;
  Cutspc := x ;
end ;

Procedure Evalfunction (start : word ; line : string ; forp : fptype ) ;
Var
 i,tmps,tmpe,tmpq,funcno : word ;
 Name,ftmp : String ;
 PreHead,newfunc : boolean ;
Begin
 newfunc := false ;
 Prehead := ((pos(PreHChar,line) < start) and (pos(PreHChar,line) > 0)) ;
 If CheckBP then ppERR(20) ;
 {Get function name}
 Name := Cutspc(Gettok1(start,line,'','',Quotes,':('+Comment)) ;
 if name = '' then pperr(10) ;
 funcno := 0 ;
 for i := 1 to funcs do 
  if (name = funname[i]) then
   if predef[i] = false then pperr(14) else funcno := i ;
 If funcno = 0 then begin
  Inc(funcs) ;
  funcno := funcs ;
  newfunc := true ;
 end ;
 foundret := false ;
 predef[funcno] := prehead ;
 if newfunc then begin
  funcall[funcno] := 0 ;
  funtype[funcno] := forp ;
  funname[funcno] := name ; {Store Function Name}
 end else begin
  if funtype[funcno] <> forp then ppERR(80) ;
 end ;
 {Get function Argments}
 tmpq := pos(Comment,line) ;
 tmps := pos('(',line) ;
 tmpe := pos(')',line) ;
 ftmp := '' ;
 if (tmps > 0) and (tmpe > 0) and (tmpe > tmps+1) and 
  ((tmpq = 0) or (tmpq > tmpe)) then ftmp := copy(line,tmps+1,tmpe-(tmps+1)) ;
 toupperstr(ftmp) ;
 if newfunc then funarg[funcno] := ftmp 
            else if funarg[funcno] <> ftmp then ppERR(81) ;
 if not(prehead) then begin
  if option['P'] then Auxout2(tempchr+funname[funcno]+':')
                 else Auxout2(funname[funcno]+':') ;
 end ;
 if prehead then exit ;
 funsave := '' ;
 nonrec := false ;
 start := pos(SaveSep,line) ;
 if start > 0 then begin
  inc(start) ;
  repeat
   Name := Cutspc(Gettok1(start,line,reps,repe,Quotes,','+Comment)) ;
   if name <> '' then begin
    toupperstr(name) ;
    if name = Norec then nonrec := true 
    else begin
     if name = 'ALL' then Auxout('PUSHA') else Pushout(name) ;
     funsave := name+','+funsave ;
    end ;
   end ;
  Until Name = '' ;
 end ;
 { if (forp = f) and not(nonrec) then Pushout(StackReg2) ; }
 CheckBP := True ;
 Lastfunc := funcno ;
end ;

Procedure EvalfunctionE (start : word ; line : string ; forp : fptype ) ;
Var
 i , j , argc , tmp , t0 , t1 , funcnum , tmpq , tmps , tmpe , k : word ;
 Args,savectl : String[90] ;
 ft : array[1..fnargmax] of string[40] ;
 Name , funtmp , funtmp2 , Movl : String ;
Begin
 {Get function name}
 Name := Cutspc(Gettok1(start,line,'','',Quotes,':('+Comment)) ;
 if name = '' then pperr(10) ;
 funcnum := 0 ;
 for i := 1 to funcs do if name = funname[i] then funcnum := i ;
 if funcnum = 0 then ppERR(13) ;
 if funtype[funcnum] <> forp then if forp = p then ppERR(30) else ppERR(31) ;
 Inc(funcall[funcnum]) ;
 {Get function Argments}
 tmpq := pos(Comment,line) ;
 tmps := pos('(',line) ;
 tmp := pos(')',line) ;
 repeat
  tmpe := tmp ;
  tmp := inchar(line,')',tmp+1) ;
 until tmp = 0 ;
 args := '' ;
 if (tmps > 0) and (tmpe > 0) 
               and (tmpe > tmps+1) and ((tmpq = 0) or (tmpq > tmpe)) then
   args := copy(line,tmps+1,tmpe-(tmps+1)) ;

 {Get Movl String}
 Movl := '' ;
 If pos(Flet,line) > 1 then Movl := cutspc(copy(line,1,Pos(Flet,line)-1)) ;
 if (forp = p) and (movl <> '') then ppERR(21) ;
 toupperstr(movl) ;

 {Manual Register Saving Control}

 t0 := 0 ;
 tmps := pos(SaveSep,line) ;
 if tmps > 0 then begin
  while tmps > 0 do begin
   t0 := tmps ;
   tmps := inchar(line,SaveSep,t0+1) ;
  end ;
  tmps := t0 ;
  t0 := inchar(line,')',tmps+1) ;
  if tmps < t0 then tmps := 0 ;
 end ;

 tmpe := pos(Comment,line) ;
 if tmpe = 0 then tmpe := 255 ;

 savectl := '' ;
 if (tmps > 0) and (tmpe > tmps) then begin
  if tmpe = 0 then savectl := cutspc(copy(line,tmps+1,255))
              else savectl := cutspc(copy(line,tmps+1,tmpe-(tmps+1))) ;
  toupperstr(savectl) ;
 end ;

 {Argments set}
 argc := 0 ; i := 1 ; j := 1 ;
 repeat
  tmp := i ;
  Name   := Cutspc(Gettok1(i,args,reps,repe,Quotes,','+Comment)) ;
  if pos(Quote2,name) + pos(Quote1,name) + pos(reserved_word[18],name)= 0 then toupperstr(name) ;
  k := pos(name,funarg[funcnum]) ;
  if (name<>'') and (k<>0) and (k<j) then ppERR(120) ;
  funtmp := Cutspc(Gettok1(j,funarg[funcnum],reps,repe,Quotes,','+Comment)) ;
  if ((Name = '') or (funtmp = '')) and (name+funtmp<>'') then ppERR(15) ;
  if Name <> '' then begin
   if funtmp[1] = SaveMark then begin
    funtmp := copy(funtmp,2,255) ;
    funtmp2 := funtmp ;
    if funtmp[1] = PtrMark then funtmp := copy(funtmp,2,255) ;
    if ByteRegs(funtmp) then funtmp[2] := 'X' ;
    if not(funtmp = movl) and not(savectl = None) then
     if (savectl='') or (pos(funtmp,savectl)>0) then begin
      inc(argc) ;
      Pushout(funtmp) ;
      ft[argc] := TAB+Pop+TAB+funtmp ;
     end ;
    funtmp := funtmp2 ;
   end ;

   tmp := pos(reserved_word[18],name) ; { call function check }
   if tmp > 0 then begin
    EvalfunctionE(tmp+5,name,f) ;
    name := Retreg ;
   end ;

   if funtmp[1] <> XChgMark then begin
    if funtmp[1] = PtrMark then begin
                                 if name = retreg then name := '['+name+']' ;
                                 Leaout(copy(funtmp,2,255),name) ;
                                end 
                           else Movout(funtmp,name) ;
   end else begin
         if isimm(name) then ppERR(122) ;
         funtmp2 := copy(funtmp,2,255) ;
         if funtmp2 <> name then begin
          Arg2out(Xchg,funtmp2,name) ;
          if (movl = funtmp2) and (movl = name) then ppERR(121) ;
          inc(argc) ;
          ft[argc] := TAB+Xchg+TAB+name+','+funtmp2 ;
         end ;
        end ;
  end ;
 until name = '' ;
 {Call Procedure}
 if option['P'] then Auxout(call+TAB+tempchr+funname[funcnum])
                else Auxout(call+TAB+funname[funcnum]) ;
 {Get Retval}
 if forp = f then
  If Movl<>'' then Movout(Movl,Retreg) ;
 for i := Argc downto 1 do Auxout2(ft[i]) ;
end ;

Procedure EvalReturn(start : word ; line : string ; forp : fptype ) ;
Var
 i,tmp : word ;
 Name : String ;
Begin
 if Checkbp = false then ppERR(33) ;
 if funtype[lastfunc] = f then begin
  Name := Cutspc(Gettok1(start,line,reps,repe,Quotes,Comment)) ;
  { if not(nonrec) then Popout(StackReg2) ; }
  tmp := pos(reserved_word[18],name) ; { call function check }
  if tmp > 0 then begin
   EvalfunctionE(tmp+5,name,f) ;
   name := Retreg ;
  end ;
  if name <> '' then Movout(Retreg,NAME) ;
 end ;
 delaystr := jump + TAB ;
 if option['P'] then delaystr := delaystr+tempchr+funname[lastfunc]+endpstr
                else delaystr := delaystr+funname[lastfunc]+endpstr ;
 foundret := true ;
end ;

Procedure Evalendp ;
Var i : word ;
    name : string ;
Begin
 if swlev > 0 then PPerr(8) ;
 if iflev > 0 then PPerr(3) ;
 if whlev > 0 then PPerr(4) ; 
 if (option['D'] or foundret) then
	if option['P'] then Auxout2(tempchr+funname[lastfunc]+endpstr+':')
                       else Auxout2(funname[lastfunc]+endpstr+':') ;
 i := 1 ;
 repeat
  Name := Cutspc(Gettok1(i,funsave,reps,repe,Quotes,','+Comment)) ;
  if name <> '' then if name = 'ALL' then Auxout('POPA') else Popout(name) ;
 until Name = '' ;
 Auxout(ret) ;
end ;

Procedure Evalendpi ;
Var i : word ;
    name : string ;
Begin
 if swlev > 0 then PPerr(8) ;
 if iflev > 0 then PPerr(3) ;
 if whlev > 0 then PPerr(4) ; 
 if option['P'] then Auxout2(tempchr+funname[lastfunc]+endpstr+':')
                else Auxout2(funname[lastfunc]+endpstr+':') ;
 i := 1 ;
 repeat
  Name := Cutspc(Gettok1(i,funsave,reps,repe,Quotes,','+Comment)) ;
  if name <> '' then if name = 'ALL' then Auxout('POPA') else Popout(name) ;
 until Name = '' ;
 Auxout(reti) ;
end ;

Procedure Gettoken(start : word ; line : string ; var arg1,cond,arg2 : string);
Var
 i,lim : word ;
Begin
 lim := length(line) ;
 arg1 := Cutspc(Gettok1(start,line,reps,repe,Quotes,Operators+Comment+OpLand+OpLor)) ;
 if start <= lim then Dec(Start) ;
 i := start ;
 cond := Cutspc(Gettok2(start,line,Operators)) ;
 if cond = OpLand then begin cond := ''; start := i ; end ;
 arg2 := Cutspc(Gettok1(start,line,reps,repe,Quotes,Comment)) ;
end ;

Function Tabr ( s : string ) : string ;
var i : word ;
Begin
  i := 1 ;
  While (s[i] = tab) do inc(i) ;
  if i > 2 then s := copy(s,i-1,255) ;
  Tabr := s ;
end ;

Function Tabr2 ( s : string ) : string ;
var i : word ;
Begin
 If not(option['T']) and (iflev + whlev > 0) then Tabr2 := tabr(s) 
                                             else tabr2 := s ;
end ;

Procedure Comment_out ;
begin
 if not(option['C']) then begin
  Write(Outfile,TAB,Comment);
  if option['R'] then Writeln(Outfile,line) else Writeln(Outfile,tabr(line)) ;
 end ;
end ;

Procedure Extrnout ;
Var i : word ;
Begin
 for i := 1 to funcs do
  if predef[i] then begin
   Auxout(TAB+Extrn1+TAB+funname[i]+Extrn2) ;
   predef[i] := false ;
  end ;
end ;

Procedure Publicout ;
Var i : word ;
Begin
 for i := 1 to funcs do
  If not(predef[i]) then begin
   Auxout(Public+TAB+funname[i]) ;
   inc(funcall[i]) ;
  end ;
end ;

Procedure FileEval( filename : string ; outf : boolean ) ; forward ;
Procedure Include_file (Start : word ; line : string ; outf : boolean ) ;
Var
 filesave : string ;
 linesave : word ;
Begin
 Filesave := Curfile ;
 Linesave := Lineno ;
 Fileeval(Cutspc(Gettok1(Start,line,'','',Quotes,Comment)),outf) ;
 Lineno := Linesave ;
 Curfile := Filesave ;
 Write(#13) ;
 Write('                                             ');
 Write(#13) ;
end ;

Procedure Evalline( var line : string ; outf : boolean ) ;
Label Trap ;
Var
 arg1,cond,arg2,chk : string ;
 i,tmp,start,com,und,rep,quo1,quo2 : word ;
Begin
 find := 0 ;
 If CheckBp and nonrec then if (Pos(StackReg2+',',line) > 0) then ppERR(11) ;
 rep := 1 ;
 und := inchar(line,UnderBar,1) ;
 if und = 0 then goto Trap ;
 while not(line[und+1] in ['a'..'z']) do begin
  und := inchar(line,UnderBar,und+1) ;
  if und = 0 then goto Trap ;
 end ;
 quo1 := Pos(Quote1,line) ;
 quo2 := Pos(Quote2,line) ;
 com := Pos(Comment,line) ;
 if com  = 0 then com  := 255 ;
 if quo1 = 0 then quo1 := 255 ;
 if quo2 = 0 then quo2 := 255 ;
 If (und > com) or (und > quo1) or (und > quo2) then goto Trap ;
 rep := und ;
 chk := cutspc(gettok1(rep,line,reps,repe,Quotes,ResDelim)) ;
 For i := 1 to Resws do
  if reserved_word[i] = chk then begin find := i ; i := Resws ; end ;
 if find = 0 then begin 
  if option['S'] then ppERR(140) ;
  Trap:
  if delaystr <> '' then begin
   Auxout(delaystr) ;
  end ;
  delaystr := '' ;
  Auxout2(tabr2(line)) ;  
  exit ; 
 end ;
 if not(find in [19,21]) and (delaystr <> '') then begin
  Auxout(delaystr) ;
  delayflg := true ;
 end ;
 delaystr := '' ;
 if not(find in [9,10]) and outf then Comment_out ; { case,default だけ別 }
 start := und + length(reserved_word[find]) ;
 case find of
  1 : Begin
       Gettoken(start,line,arg1,cond,arg2) ;
       Evalif(arg1,cond,arg2) ;
      end ;
  2 : Evalelse ;
  3 : Evalendif ;
  4 : Begin
       Gettoken(start,line,arg1,cond,arg2) ;
       Evalwhile(arg1,cond,arg2) ;
      end ;
  5 : Evalwend ;
  6 : Begin
       Val(Cutspc(Gettok1(start,line,'','',Quotes,Comment)),i,tmp) ;
       Evalcontinue(i) ;
      end ;
  7 : Begin
       Val(Cutspc(Gettok1(start,line,'','',Quotes,Comment)),i,tmp) ;
       Evalexit(i) ;
      end ;
  8 : Swlev := 0 ;
  9 : Begin
       Gettoken(start,line,arg1,cond,arg2) ;
       Evalcase(arg1,cond,arg2) ;
      end ;
 10 : begin 
       Evalelse ;
       Comment_out ;
      end ;
 11 : Evalendsw ;
 12 : Evalrepeat ;
 13 : Begin
       Gettoken(start,line,arg1,cond,arg2) ;
       Evaluntil(arg1,cond,arg2) ;
      end ;
 14 : Option['N'] := true ;
 15 : Option['N'] := false ;
 16 : Evalfunction(start,line,f) ;
 17 : Evalreturn(start,line,f) ;
 18 : EvalfunctionE(start,line,f) ;
 19 : begin
       if not(checkbp) then ppERR(50) ;
       if funtype[lastfunc] = f then begin
        if not(foundret) then ppERR(102) ;
        Evalendp ;
        CheckBP := false ;
       end else ppERR(41) ;
      end ;
 20 : Evalfunction(start,line,p) ;
 21 : begin
       if not(checkbp) then ppERR(50) ;
       if funtype[lastfunc] = p then 
        begin Evalendp ; checkBP := false end 
       else ppERR(40) ;
      end ;
 22 : EvalfunctionE(start,line,p) ;
 23 : Evalloopcx(Cutspc(Gettok1(Start,line,reps,repe,Quotes,Comment))) ;
 24 : Evalendl ;
 25 : Evalfor(start,line) ;
 26 : Evalnext ;
 27 : Extrnout ;
 28 : Publicout ;
 29 : Include_file(start,line,outf) ;
 30 : Cstpop(csttop) ;
 31 : begin
       if not(checkbp) then ppERR(50) ;
       if funtype[lastfunc] = p then 
        begin Evalendpi ; checkBP := false end 
       else ppERR(40) ;
      end ;
 end ;
 delayflg := false ;
end ;

Procedure Getparam ;
Var count,i,j : word ;
    c : char ;
    s : string ;
Begin
 If Paramcount > argmax then ppERR(5) ;
 count := 1 ;
 for c := #32 to #128 do option[c] := false ;
 for i := 0 to 10 do param[i] := '' ;
 for i := 0 to Paramcount do begin
  if i > 0 then s := Paramstr(i) 
           else begin s := getenv('APP') ; 
                      If (s<>'') and (s[1]<>'-') then s := '-' + s ; end ;
  if s[1] = '-' then begin
                      for j := 2 to length(s) do begin
                       case s[j] of
                        '-' : If j > 2 then option[upcase(s[j-1])]:=false ;
                        #32..#128 : option[upcase(s[j])]:=true ;
                       end ;
                      end ;
                     end
                else If s > '' then begin param[count] := s ; inc(count) end ;
 end ;
end ;

Procedure Extrf( line : string ) ;
var
 tmp1,tmp2,tmp3,tmp4 : word ;
begin
 tmp1 := pos(reserved_word[16],line) ; { function }
 tmp2 := pos(reserved_word[20],line) ; { procedure }
 tmp4 := pos(reserved_word[29],line) ; { include }
 if tmp1 + tmp2 + tmp4 = 0 then exit ;
 if (tmp4 > 0) and not(option['2']) then exit ;
 if tmp1 = 0 then tmp1 := tmp2 ;
 if tmp1 = 0 then tmp1 := tmp4 ;
 tmp2 := pos(Comment,line) ;
 tmp3 := pos(PreHChar,line) ;
 if (tmp2 > 0) and (tmp1 > tmp2) then exit ;
 if (tmp3 > 0) and (tmp1 > tmp3) then exit ;
 if (tmp4 > 0) then begin
                     Include_file(tmp4+length(reserved_word[29]),line,true) ;
                     exit ;
                    end ;
 Inc(funcs) ;
 Auxout2(PreHChar+copy(line,tmp1,255)) ;
end ;

Procedure FileEval( filename : string ; outf : boolean) ;

Procedure putlineno ;
var lnstr : string[10] ;
begin
 Write(#13) ;
 Write(filename,'(',Lineno,')') ;
end ;

Var
 Tmp : word ;
 Infile : text ;
Begin
 Assign(Infile,filename) ;
 {$I-}
 Reset(Infile) ;
 If IOresult > 0 then PPerr(2) ;
 {$I+}
 Lineno := 0 ;
 Curfile := filename ;
 If option['L'] then begin
  Tmp := pos('.',filename) ;
  If tmp = 0 then Write(Lnkfile,filename,' ') 
             else Write(Lnkfile,copy(filename,1,tmp-1),' ') ;
 end ;
 Write(#13) ;
 Write('                                             ');
 Write(#13) ;
 While not(Eof(Infile)) do begin
  Readln(Infile,Line) ;
  Inc(Lineno) ;
  If (lineno and 63) = 1 then Putlineno ;
  if option['H'] then Extrf(Line) else Evalline(Line,outf) ;
 end ;
 Putlineno ;
 if swlev > 0 then PPerr(8) ;
 if iflev > 0 then PPerr(3) ;
 if whlev > 0 then PPerr(4) ;
 if checkbp then PPerr(20) ;
 Cstpop(csttop) ;
 Close(Infile) ;
end ;

Begin {main}
 Assign(Stdout,'') ;
 Rewrite(Stdout) ;
 Write (Stdout,'Mio''s 80x86 Assembler tiny Pre Processor '+Version) ;
 Writeln (Stdout,' Copyright (c) Ｍｉｏ 1992,93') ;
 Getparam ;
 If option['?'] then Helpmsg ;
 If option['@'] then OutputResw ;
 If option['P'] then begin Tempchr := '@' ; option ['C'] := true end ;
 {$I-}
 If (param[1]<>'') and (param[2]='') then
  if pos('.',param[1])=0 then param[2] := param[1] 
   else param[2] := copy(param[1],1,pos('.',param[1])-1) ;
 If (pos('.',param[1])=0) and (param[1] <> '') then param[1] := param[1] + '.app' ;
 Assign(Infile,Param[1]) ;
 Reset(Infile) ;
 If IOresult > 0 then PPerr(2) ;
 Close(Infile) ;
 If (param[3] = '') and option['L'] then 
  if pos('.',param[2]) = 0 then param[3] := param[2] 
   else param[3] := copy(param[2],1,pos('.',param[2])-1);
 If (pos('.',param[2])=0) and (param[2] <> '') 
  then if option['H'] then param[2] := param[2] + '.H'
  else if option['P'] then param[2] := param[2] + '.pas' 
                      else param[2] := param[2] + '.asm' ;
 If (param[3] <> '') and option['L'] then 
  if pos('.',param[3]) =0 then param[3] := param[3] + '.lnk' ;
 If (param[1] = param[2]) and (param[1]<>'') then pperr(6) ;
 If (param[2] = param[3]) and (param[2]<>'') then pperr(6) ;
 Assign(Outfile,Param[2]) ;
 ReWrite(Outfile) ;
 If IOresult > 0 then PPerr(2) ;
 If option['L'] then begin
  Assign(Lnkfile,Param[3]) ;
  ReWrite(Lnkfile) ;
  If IOresult > 0 then PPerr(2) ;
 end ;
 {$I+}
 { Convert Main }
 if option['2'] then begin
  {Header tmp make}
  {$I-}
  optsave := option['L'] ;
  option['L'] := false ;
  option['H'] := true ;
  Close(Outfile) ;
  Assign(Outfile,Tmpfilename) ;
  Rewrite(Outfile) ;
  If IOresult > 0 then PPerr(62) ;
  Writeln('Pass 1 :') ;
  FileEval(Param[1],true) ;
  Close(Outfile) ;
  {Pass 2}
  Writeln ;
  Writeln('Pass 2 :') ;
  option['H'] := false ;
  Assign(Outfile,Param[2]) ;
  Rewrite(Outfile) ;
  If funcs > 0 then begin
   funcs := 0 ;
   If IOresult > 0 then PPerr(2) ;
   FileEval(Tmpfilename,false) ;
  end ;
  {Erase Tmpfile}
  Assign(Infile,Tmpfilename) ;
  Erase(Infile) ;
  If IOresult > 0 then PPerr(63) ;
  option['L'] := optsave ;
  {$I+}
 end ;
 FileEval(Param[1],true) ;
 { End Convert }
 Writeln(Stdout) ;
 if option ['W'] then
  for i := 1 to funcs do
   if funcall[i]=0 then begin
    Write(Stdout,'警告:') ;
    if funtype[i]=f then Write(Stdout,'Function  ') else Write(Stdout,'Procedure ') ;
    Writeln (Stdout,funname[i]:15,' が、一度も呼ばれていません') ;
   end ;
 If option['H'] then begin
  Writeln (Stdout,'Total ',funcs,' function(s) and procedure(s) found.') ;
  If funcs > 0 then Writeln (Stdout,'Header file (',param[2],') created.') ;
 end else Writeln (Stdout,'Convert completed.') ;
 If option['L'] then begin
  Writeln(Lnkfile) ;
  Close(Lnkfile) ;
 end ;
 Close(Outfile) ;
 If option['H'] and (funcs = 0) then Erase(outfile) ;
end {main}.
