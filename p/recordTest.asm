//
// version
//
	.vers	7

//
// execution framework
//
__start:
	call	_main
	call	_exit
__stop:
	jmp	__stop

//
// Integer readInteger()
//
_readInteger:
	asf	0
	rdint
	popr
	rsf
	ret

//
// void writeInteger(Integer)
//
_writeInteger:
	asf	0
	pushl	-3
	wrint
	rsf
	ret

//
// Character readCharacter()
//
_readCharacter:
	asf	0
	rdchr
	popr
	rsf
	ret

//
// void writeCharacter(Character)
//
_writeCharacter:
	asf	0
	pushl	-3
	wrchr
	rsf
	ret

//
// Integer char2int(Character)
//
_char2int:
	asf	0
	pushl	-3
	popr
	rsf
	ret

//
// Character int2char(Integer)
//
_int2char:
	asf	0
	pushl	-3
	popr
	rsf
	ret

//
// void exit()
//
_exit:
	asf	0
	halt
	rsf
	ret

//
// void writeString(String)
//
_writeString:
	asf	1
	pushc	0
	popl	0
	jmp	_writeString_L2
_writeString_L1:
	pushl	-3
	pushl	0
	getfa
	call	_writeCharacter
	drop	1
	pushl	0
	pushc	1
	add
	popl	0
_writeString_L2:
	pushl	0
	pushl	-3
	getsz
	lt
	brt	_writeString_L1
	rsf
	ret

//
// void move(record { Integer x; Integer y; }, record { Integer x; Integer y; })
//
_move:
	asf	0
	pushl	-4
	pushl	-4
	getf	0
	pushl	-3
	getf	0
	add
	putf	0
	pushl	-4
	pushl	-4
	getf	1
	pushl	-3
	getf	1
	add
	putf	1
__0:
	rsf
	ret

//
// void main()
//
_main:
	asf	6
	pushc	5
	popl	0
	pushc	10
	popl	1
	pushc	4
	popl	2
	pushc	8
	popl	3
	new	2
	popl	4
	pushl	4
	pushl	0
	putf	0
	pushl	4
	pushl	1
	putf	1
	new	2
	popl	5
	pushl	5
	pushl	2
	putf	0
	pushl	5
	pushl	3
	putf	1
	pushl	4
	getf	0
	call	_writeInteger
	drop	1
	pushl	4
	getf	1
	call	_writeInteger
	drop	1
	pushl	4
	pushl	5
	call	_move
	drop	2
	pushl	4
	getf	0
	call	_writeInteger
	drop	1
	pushl	4
	getf	1
	call	_writeInteger
	drop	1
__1:
	rsf
	ret
