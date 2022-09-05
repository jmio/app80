{$M 32000,0,0}
var x,caller : word ;
begin
	caller := 0 ;
	asm
	jmp	@bgn

{ 
　　再帰関数テスト：アッカーマン関数サンプル

　　以下のサブルーチンのＡＰＰ版

Var
 x , caller : word ;

function ack (x,y : word) : word ;
begin
 inc(caller) ;
 if x = 0 then ack := y + 1
 else if y = 0 then ack := ack (x - 1 , 1 )
 else ack := ack (x - 1 , ack (x , y - 1)) ;
end ;

 }

_function ack (^ax,^bx)
        inc	caller
	_if	ax == 0
		inc	bx
		_return	bx
	_else
		_if	bx == 0
			dec	ax
			_return	_func ack(ax,1) : none
		_else
			mov	cx,ax
			dec	ax
			dec	bx
			_return	_func ack(ax,_func ack(cx,bx)) : none
		_endif
	_endif
_endf

@bgn:
	x = _func ack(3,3) : none
	end ;
	writeln(' ACK (3,3)=',x) ;
	writeln(' Caller   =',Caller) ;
end.
