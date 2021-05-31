; Hello world for Intel 8080, CP/M 2.2
	org	100h
bdos	equ	0005h	; BDOS entry point
start:	mvi	c,9h	; BDOS function: output string
	lxi	d,msg$	; address of msg
	call	bdos
	ret
msg$:	db	'Hello, world from Assembler!', 0dh,0ah, '$'	; $-terminated message
	end
