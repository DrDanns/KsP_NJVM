asf	2
rdint
popl	0
rdint
popl	1
L1:
pushl	0
pushl	1
ne
brf	L2
pushl	0
pushl	1
gt
brf	L3
pushl	0
pushl	1
sub
popl	0
jmp	L4
L3:
pushl	1
pushl	0
sub
popl	1
L4:
jmp	L1
L2:
pushl	0
wrint
pushc	'\n'
wrchr
rsf
halt
