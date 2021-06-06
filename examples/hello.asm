; Hello world for Intel 8080, CP/M 2.2
	org	100h
bdos	equ	0005h	; BDOS entry point
start:	lxi	h,msg	; address of msg
loop:	mvi	c,2h	; BDOS function: output character
	mov	e, m	; Load character
	push	h	; H gets clobbered by bdos call
	call	bdos	; Print
	pop	h	; Restore H
	inx	h	; Increment address
	mov	a, e	; Is the character we just printed 0ah?
	cpi	0ah
	jnz	loop	; Go again if not
	ret
msg:	db	'Hello, world from Assembler!'
	db	0dh
	db	0ah
	end
