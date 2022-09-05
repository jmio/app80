{ App 支援ツール #01 : ソースの大文字化 }
var
 under,quote,kanji : boolean ;
 s , name : string ;
 f,f2 : text ;
 i : word ;

procedure uperr(x : word) ;
begin
 Case x of
   1 : Writeln('ファイル名の指定が正しくありません') ;
   2 : Writeln('テンポラリファイルがオープンできません') ;
   3 : Writeln('テンポラリファイル (apuptmp.tmp) の消去に失敗しました') ;
 end ;
 halt(x) ;
end ;

begin
 Writeln('APUP (APP TOOLS #1) Release 0.0 (C) Mio') ;
 if paramcount = 0 then begin
  Writeln('Usage : APUP filename[.app]') ;
  Writeln('APP ソースプログラムのシンボル、レジスタ名を大文字にそろえます。') ;
  halt(0) ;
 end ;
 name := paramstr(1) ;
 if pos('.',name)=0 then name := name + '.app' ;
 {$I-}
 assign(f,name) ;
 reset(f) ;
 if IOresult > 0 then upERR(1) ;
 assign(f2,'apuptmp.tmp') ;
 rewrite(f2) ;
 if IOresult > 0 then upERR(2) ;
 {$I+}
 while not(eof(f)) do begin
  readln(f,s) ;
  under := false ;
  quote := false ;
  kanji := false ;
  for i := 1 to length(s) do begin
   if not(kanji) then 
   case s[i] of
    '_'             : under := true ;
    '''' , '"'      : quote := not(quote) ;
    ' ',#9,'(',':'  : under := false ;
    ';'             : i := length(s) ;
   else
    if not(under or quote) then s[i] := upcase(s[i]) ;
   end ;
   kanji := false ;
   if ord(s[i]) in [$81..$9F,$E0..$FC] then kanji := true ;
  end ;
  writeln(f2,s) ;
 end ;
 close(f2) ;
 close(f) ;
 {$I-}
 erase(f) ;
 if IOresult > 0 then upERR(3) ;
 rename(f2,name) ;
 {$I+}
end.
