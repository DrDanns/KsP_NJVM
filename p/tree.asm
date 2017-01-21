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
// record { Character sign; Node left; Node right; } newLeafNode(Character)
//
_newLeafNode:
	asf	1
	new	3
	popl	0
	pushl	0
	pushl	-3
	putf	0
	pushl	0
	pushn
	putf	1
	pushl	0
	pushn
	putf	2
	pushl	0
	popr
	jmp	__0
__0:
	rsf
	ret

//
// record { Character sign; Node left; Node right; } newInnerNode(Character, record { Character sign; Node left; Node right; }, record { Character sign; Node left; Node right; })
//
_newInnerNode:
	asf	1
	new	3
	popl	0
	pushl	0
	pushl	-5
	putf	0
	pushl	0
	pushl	-4
	putf	1
	pushl	0
	pushl	-3
	putf	2
	pushl	0
	popr
	jmp	__1
__1:
	rsf
	ret

//
// void Tree(record { Character sign; Node left; Node right; })
//
_Tree:
	asf	0
	pushl	-3
	getf	1
	pushn
	refeq
	dup
	brf	__5
	drop	1
	pushl	-3
	getf	2
	pushn
	refeq
__5:
	brf	__3
	pushc	13
	newa
	dup
	pushc	0
	pushc	110
	putfa
	dup
	pushc	1
	pushc	101
	putfa
	dup
	pushc	2
	pushc	119
	putfa
	dup
	pushc	3
	pushc	76
	putfa
	dup
	pushc	4
	pushc	101
	putfa
	dup
	pushc	5
	pushc	97
	putfa
	dup
	pushc	6
	pushc	102
	putfa
	dup
	pushc	7
	pushc	78
	putfa
	dup
	pushc	8
	pushc	111
	putfa
	dup
	pushc	9
	pushc	100
	putfa
	dup
	pushc	10
	pushc	101
	putfa
	dup
	pushc	11
	pushc	40
	putfa
	dup
	pushc	12
	pushc	34
	putfa
	call	_writeString
	drop	1
	pushl	-3
	getf	0
	call	_writeCharacter
	drop	1
	pushc	2
	newa
	dup
	pushc	0
	pushc	34
	putfa
	dup
	pushc	1
	pushc	41
	putfa
	call	_writeString
	drop	1
	jmp	__4
__3:
	pushc	14
	newa
	dup
	pushc	0
	pushc	110
	putfa
	dup
	pushc	1
	pushc	101
	putfa
	dup
	pushc	2
	pushc	119
	putfa
	dup
	pushc	3
	pushc	73
	putfa
	dup
	pushc	4
	pushc	110
	putfa
	dup
	pushc	5
	pushc	110
	putfa
	dup
	pushc	6
	pushc	101
	putfa
	dup
	pushc	7
	pushc	114
	putfa
	dup
	pushc	8
	pushc	78
	putfa
	dup
	pushc	9
	pushc	111
	putfa
	dup
	pushc	10
	pushc	100
	putfa
	dup
	pushc	11
	pushc	101
	putfa
	dup
	pushc	12
	pushc	40
	putfa
	dup
	pushc	13
	pushc	34
	putfa
	call	_writeString
	drop	1
	pushl	-3
	getf	0
	call	_writeCharacter
	drop	1
	pushc	3
	newa
	dup
	pushc	0
	pushc	34
	putfa
	dup
	pushc	1
	pushc	44
	putfa
	dup
	pushc	2
	pushc	32
	putfa
	call	_writeString
	drop	1
	pushl	-3
	getf	1
	call	_Tree
	drop	1
	pushc	2
	newa
	dup
	pushc	0
	pushc	44
	putfa
	dup
	pushc	1
	pushc	32
	putfa
	call	_writeString
	drop	1
	pushl	-3
	getf	2
	call	_Tree
	drop	1
	pushc	2
	newa
	dup
	pushc	0
	pushc	41
	putfa
	dup
	pushc	1
	pushc	10
	putfa
	call	_writeString
	drop	1
__4:
__2:
	rsf
	ret

//
// void main()
//
_main:
	asf	1
	pushc	45
	pushc	53
	call	_newLeafNode
	drop	1
	pushr
	pushc	42
	pushc	43
	pushc	49
	call	_newLeafNode
	drop	1
	pushr
	pushc	51
	call	_newLeafNode
	drop	1
	pushr
	call	_newInnerNode
	drop	3
	pushr
	pushc	45
	pushc	52
	call	_newLeafNode
	drop	1
	pushr
	pushc	55
	call	_newLeafNode
	drop	1
	pushr
	call	_newInnerNode
	drop	3
	pushr
	call	_newInnerNode
	drop	3
	pushr
	call	_newInnerNode
	drop	3
	pushr
	popl	0
	pushl	0
	call	_Tree
	drop	1
__6:
	rsf
	ret
