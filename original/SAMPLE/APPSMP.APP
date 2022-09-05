;
; ğŒ®ƒTƒ“ƒvƒ‹
;
	_if	ax = 0
		; equal sample
	_endif

	_if	ax < 0
		; less than sample
	_endif

	_if	ax > 0
		; more than sample
	_endif

	_if	ax <= 0
		; less or equal sample
	_endif

	_if	ax >= 0
		; more or equal sample
	_endif

	_if	ax <> 0
		; not equal sample
	_endif

	_if	ax & 0001B
		; bit test (bit on) sample
	_endif

	_if	ax ~& 0001B
		; bit test (bit off) samplr
	_endif

	_if	ax = 0 && bx = 0
		; double condition sample (and)
	_endif

	_if	ax = 0 || bx = 0
		; double condition sample (or)
	_endif

	_if	++ax < 100
		; pre increment sample
	_endif

	_if	cx < dx || ax = 100 || byte ptr [bx] = 0
		;   3rd       2nd         1st      ( •]‰¿‡˜ )
		; complex explession sample
	_endif
