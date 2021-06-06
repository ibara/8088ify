%define	M	Byte [bx]
%define	m	Byte [bx]
;***********************************************************************
; MICROCOSM ASSOCIATES  8080/8085 CPU DIAGNOSTIC VERSION 1.0  (C) 1980
;***********************************************************************
;
;DONATED TO THE "SIG/M" CP/M USER'S GROUP BY:
;KELLY SMITH, MICROCOSM ASSOCIATES
;3055 WACO AVENUE
;SIMI VALLEY, CALIFORNIA, 93065
;(805) 527-9321 (MODEM, CP/M-NET (TM))
;(805) 527-0518 (VERBAL)
;
;  Updated by Mike Douglas October, 2012 
;
;	Added the following tests that were missing:
;	   mov c,m
;	   mov m,c
;	   ana b
;
;	Fixed the CPUER exit routine which did not display the
;	   low byte of the failure address properly.
;
;	Added display of the Microcosm welcome message
;
	org	00100H

;
	jmp	CPU
; WARN: Is the above jmp correct?	;JUMP TO 8080 CPU DIAGNOSTIC
;
WELCOM:	db	'MICROCOSM ASSOCIATES 8080/8085 CPU DIAGNOSTIC',13,10
	db	' VERSION 1.0  (C) 1980',13,10,'$'
;
BDOS	equ	00005H	;BDOS ENTRY TO CP/M
WBOOT	equ	00000H	;RE-ENTRY TO CP/M WARM BOOT
;
;
;
;MESSAGE OUTPUT ROUTINE
;
MSG:	push	dx	;EXILE D REG.
	xchg	bx, dx	;SWAP H&L REGS. TO D&E REGS.
	mov	cl, 9		;LET BDOS KNOW WE WANT TO SEND A MESSAGE
	push	ax
	mov	ah, cl
	int	21h
	pop	ax
	pop	dx	;BACK FROM EXILE
	ret
;
;
;
;CHARACTER OUTPUT ROUTINE
;
PCHAR:	mov	cl, 2
	push	ax
	mov	ah, cl
	int	21h
	pop	ax
	ret
;
;
;
BYTEO:	lahf
	xchg	al, ah
	push	ax
	xchg	al, ah
	call	BYTO1
; WARN: Is the above call correct?
	mov	dl, al
	call	PCHAR
; WARN: Is the above call correct?
	pop	ax
	xchg	al, ah
	sahf
	call	BYTO2
; WARN: Is the above call correct?
	mov	dl, al
	jmp	PCHAR
; WARN: Is the above jmp correct?
BYTO1:	ror	al, 1
	ror	al, 1
	ror	al, 1
	ror	al, 1
BYTO2:	and	al, 0FH
	cmp	al, 0AH
	jns	L@0
	jmp	BYTO3
; WARN: Is the above jmp correct?
L@0:
	add	al, 7
BYTO3:	add	al, 30H
	ret
;
;
;
;************************************************************
;           MESSAGE TABLE FOR OPERATIONAL CPU TEST
;************************************************************
;
OKCPU:	db	0DH,0AH,' CPU IS OPERATIONAL$'
;
NGCPU:	db	0DH,0AH,' CPU HAS FAILED!    ERROR EXIT=$'
;
;
;
;************************************************************
;                8080/8085 CPU TEST/DIAGNOSTIC
;************************************************************
;
;NOTE: (1) PROGRAM ASSUMES "CALL",AND "LXI SP" INSTRUCTIONS WORK!
;
;      (2) INSTRUCTIONS NOT TESTED ARE "HLT","DI","EI","RIM","SIM",
;          AND "RST 0" THRU "RST 7"
;
;
;
;TEST JUMP INSTRUCTIONS AND FLAGS
;
CPU:	mov	sp, STACK		;SET THE STACK POINTER
	mov	bx, WELCOM
	call	MSG
; WARN: Is the above call correct?
	and	al, 0	;INITIALIZE A REG. AND CLEAR ALL FLAGS
	jnz	L@1
	jmp	J010
; WARN: Is the above jmp correct?
L@1:	;TEST "JZ"
	call	CPUER
; WARN: Is the above call correct?
J010:	jnae	L@2
	jmp	J020
; WARN: Is the above jmp correct?
L@2:	;TEST "JNC"
	call	CPUER
; WARN: Is the above call correct?
J020:	jnp	L@3
	jmp	J030
; WARN: Is the above jmp correct?
L@3:	;TEST "JPE"
	call	CPUER
; WARN: Is the above call correct?
J030:	js	L@4
	jmp	J040
; WARN: Is the above jmp correct?
L@4:	;TEST "JP"
	call	CPUER
; WARN: Is the above call correct?
J040:	jz	L@5
	jmp	J050
; WARN: Is the above jmp correct?
L@5:	;TEST "JNZ"
	jnb	L@6
	jmp	J050
; WARN: Is the above jmp correct?
L@6:	;TEST "JC"
	jp	L@7
	jmp	J050
; WARN: Is the above jmp correct?
L@7:	;TEST "JPO"
	jns	L@8
	jmp	J050
; WARN: Is the above jmp correct?
L@8:	;TEST "JM"
	jmp	J060
; WARN: Is the above jmp correct?	;TEST "JMP" (IT'S A LITTLE LATE,BUT WHAT THE HELL!
J050:	call	CPUER
; WARN: Is the above call correct?
J060:	add	al, 6	;A=6,C=0,P=1,S=0,Z=0
	jz	L@9
	jmp	J070
; WARN: Is the above jmp correct?
L@9:	;TEST "JNZ"
	call	CPUER
; WARN: Is the above call correct?
J070:	jnb	L@10
	jmp	J080
; WARN: Is the above jmp correct?
L@10:	;TEST "JC"
	jp	L@11
	jmp	J080
; WARN: Is the above jmp correct?
L@11:	;TEST "JPO"
	js	L@12
	jmp	J090
; WARN: Is the above jmp correct?
L@12:	;TEST "JP"
J080:	call	CPUER
; WARN: Is the above call correct?
J090:	add	al, 070H	;A=76H,C=0,P=0,S=0,Z=0
	jp	L@13
	jmp	J100
; WARN: Is the above jmp correct?
L@13:	;TEST "JPO"
	call	CPUER
; WARN: Is the above call correct?
J100:	jns	L@14
	jmp	J110
; WARN: Is the above jmp correct?
L@14:	;TEST "JM"
	jnz	L@15
	jmp	J110
; WARN: Is the above jmp correct?
L@15:	;TEST "JZ"
	jnae	L@16
	jmp	J120
; WARN: Is the above jmp correct?
L@16:	;TEST "JNC"
J110:	call	CPUER
; WARN: Is the above call correct?
J120:	add	al, 081H	;A=F7H,C=0,P=0,S=1,Z=0
	jns	L@17
	jmp	J130
; WARN: Is the above jmp correct?
L@17:	;TEST "JM"
	call	CPUER
; WARN: Is the above call correct?
J130:	jnz	L@18
	jmp	J140
; WARN: Is the above jmp correct?
L@18:	;TEST "JZ"
	jnb	L@19
	jmp	J140
; WARN: Is the above jmp correct?
L@19:	;TEST "JC"
	jp	L@20
	jmp	J150
; WARN: Is the above jmp correct?
L@20:	;TEST "JPO"
J140:	call	CPUER
; WARN: Is the above call correct?
J150:	add	al, 0FEH	;A=F5H,C=1,P=1,S=1,Z=0
	jnb	L@21
	jmp	J160
; WARN: Is the above jmp correct?
L@21:	;TEST "JC"
	call	CPUER
; WARN: Is the above call correct?
J160:	jnz	L@22
	jmp	J170
; WARN: Is the above jmp correct?
L@22:	;TEST "JZ"
	jp	L@23
	jmp	J170
; WARN: Is the above jmp correct?
L@23:	;TEST "JPO"
	jns	L@24
	jmp	AIMM
; WARN: Is the above jmp correct?
L@24:	;TEST "JM"
J170:	call	CPUER
; WARN: Is the above call correct?
;
;
;
;TEST ACCUMULATOR IMMEDIATE INSTRUCTIONS
;
AIMM:	cmp	al, 0	;A=F5H,C=0,Z=0
	jnb	L@25
	jmp	CPIE
; WARN: Is the above jmp correct?
L@25:	;TEST "CPI" FOR RE-SET CARRY
	jnz	L@26
	jmp	CPIE
; WARN: Is the above jmp correct?
L@26:	;TEST "CPI" FOR RE-SET ZERO
	cmp	al, 0F5H	;A=F5H,C=0,Z=1
	jnb	L@27
	jmp	CPIE
; WARN: Is the above jmp correct?
L@27:	;TEST "CPI" FOR RE-SET CARRY ("ADI")
	jz	L@28
	jmp	CPIE
; WARN: Is the above jmp correct?
L@28:	;TEST "CPI" FOR RE-SET ZERO
	cmp	al, 0FFH	;A=F5H,C=1,Z=0
	jnz	L@29
	jmp	CPIE
; WARN: Is the above jmp correct?
L@29:	;TEST "CPI" FOR RE-SET ZERO
	jnb	L@30
	jmp	ACII
; WARN: Is the above jmp correct?
L@30:	;TEST "CPI" FOR SET CARRY
CPIE:	call	CPUER
; WARN: Is the above call correct?
ACII:	adc	al, 00AH	;A=F5H+0AH+CARRY(1)=0,C=1
	adc	al, 00AH	;A=0+0AH+CARRY(0)=0BH,C=0
	cmp	al, 00BH
	jnz	L@31
	jmp	SUII
; WARN: Is the above jmp correct?
L@31:	;TEST "ACI"
	call	CPUER
; WARN: Is the above call correct?
SUII:	sub	al, 00CH	;A=FFH,C=0
	sub	al, 00FH	;A=F0H,C=1
	cmp	al, 0F0H
	jnz	L@32
	jmp	SBII
; WARN: Is the above jmp correct?
L@32:	;TEST "SUI"
	call	CPUER
; WARN: Is the above call correct?
SBII:	sbb	al, 0F1H	;A=F0H-0F1H-CARRY(0)=FFH,C=1
	sbb	al, 00EH	;A=FFH-OEH-CARRY(1)=F0H,C=0
	cmp	al, 0F0H
	jnz	L@33
	jmp	ANII
; WARN: Is the above jmp correct?
L@33:	;TEST "SBI"
	call	CPUER
; WARN: Is the above call correct?
ANII:	and	al, 055H	;A=F0H<AND>55H=50H,C=0,P=1,S=0,Z=0
	cmp	al, 050H
	jnz	L@34
	jmp	ORII
; WARN: Is the above jmp correct?
L@34:	;TEST "ANI"
	call	CPUER
; WARN: Is the above call correct?
ORII:	or	al, 03AH	;A=50H<OR>3AH=7AH,C=0,P=0,S=0,Z=0
	cmp	al, 07AH
	jnz	L@35
	jmp	XRII
; WARN: Is the above jmp correct?
L@35:	;TEST "ORI"
	call	CPUER
; WARN: Is the above call correct?
XRII:	xor	al, 00FH	;A=7AH<XOR>0FH=75H,C=0,P=0,S=0,Z=0
	cmp	al, 075H
	jnz	L@36
	jmp	C010
; WARN: Is the above jmp correct?
L@36:	;TEST "XRI"
	call	CPUER
; WARN: Is the above call correct?
;
;
;
;TEST CALLS AND RETURNS
;
C010:	and	al, 000H	;A=0,C=0,P=1,S=0,Z=1
	jnb	L@37
	call	CPUER
; WARN: Is the above call correct?
L@37:	;TEST "CC"
	jp	L@38
	call	CPUER
; WARN: Is the above call correct?
L@38:	;TEST "CPO"
	jns	L@39
	call	CPUER
; WARN: Is the above call correct?
L@39:	;TEST "CM"
	jz	L@40
	call	CPUER
; WARN: Is the above call correct?
L@40:	;TEST "CNZ"
	cmp	al, 000H
	jnz	L@41
	jmp	C020
; WARN: Is the above jmp correct?
L@41:	;A=0,C=0,P=0,S=0,Z=1
	call	CPUER
; WARN: Is the above call correct?
C020:	sub	al, 077H	;A=89H,C=1,P=0,S=1,Z=0
	jnae	L@42
	call	CPUER
; WARN: Is the above call correct?
L@42:	;TEST "CNC"
	jnp	L@43
	call	CPUER
; WARN: Is the above call correct?
L@43:	;TEST "CPE"
	js	L@44
	call	CPUER
; WARN: Is the above call correct?
L@44:	;TEST "CP"
	jnz	L@45
	call	CPUER
; WARN: Is the above call correct?
L@45:	;TEST "CZ"
	cmp	al, 089H
	jnz	L@46
	jmp	C030
; WARN: Is the above jmp correct?
L@46:	;TEST FOR "CALLS" TAKING BRANCH
	call	CPUER
; WARN: Is the above call correct?
C030:	and	al, 0FFH	;SET FLAGS BACK!
	jp	L@47
	call	CPOI
; WARN: Is the above call correct?
L@47:	;TEST "CPO"
	cmp	al, 0D9H
	jnz	L@48
	jmp	MOVI
; WARN: Is the above jmp correct?
L@48:	;TEST "CALL" SEQUENCE SUCCESS
	call	CPUER
; WARN: Is the above call correct?
CPOI:	jnp	L@49
	ret
L@49:	;TEST "RPE"
	add	al, 010H	;A=99H,C=0,P=0,S=1,Z=0
	jnp	L@50
	call	CPEI
; WARN: Is the above call correct?
L@50:	;TEST "CPE"
	add	al, 002H	;A=D9H,C=0,P=0,S=1,Z=0
	jp	L@51
	ret
L@51:	;TEST "RPO"
	call	CPUER
; WARN: Is the above call correct?
CPEI:	jp	L@52
	ret
L@52:	;TEST "RPO"
	add	al, 020H	;A=B9H,C=0,P=0,S=1,Z=0
	jns	L@53
	call	CMI
; WARN: Is the above call correct?
L@53:	;TEST "CM"
	add	al, 004H	;A=D7H,C=0,P=1,S=1,Z=0
	jnp	L@54
	ret
L@54:	;TEST "RPE"
	call	CPUER
; WARN: Is the above call correct?
CMI:	js	L@55
	ret
L@55:	;TEST "RP"
	add	al, 080H	;A=39H,C=1,P=1,S=0,Z=0
	js	L@56
	call	TCPI
; WARN: Is the above call correct?
L@56:	;TEST "CP"
	add	al, 080H	;A=D3H,C=0,P=0,S=1,Z=0
	jns	L@57
	ret
L@57:	;TEST "RM"
	call	CPUER
; WARN: Is the above call correct?
TCPI:	jns	L@58
	ret
L@58:	;TEST "RM"
	add	al, 040H	;A=79H,C=0,P=0,S=0,Z=0
	jnae	L@59
	call	CNCI
; WARN: Is the above call correct?
L@59:	;TEST "CNC"
	add	al, 040H	;A=53H,C=0,P=1,S=0,Z=0
	js	L@60
	ret
L@60:	;TEST "RP"
	call	CPUER
; WARN: Is the above call correct?
CNCI:	jnb	L@61
	ret
L@61:	;TEST "RC"
	add	al, 08FH	;A=08H,C=1,P=0,S=0,Z=0
	jnb	L@62
	call	CCI
; WARN: Is the above call correct?
L@62:	;TEST "CC"
	sub	al, 002H	;A=13H,C=0,P=0,S=0,Z=0
	jnae	L@63
	ret
L@63:	;TEST "RNC"
	call	CPUER
; WARN: Is the above call correct?
CCI:	jnae	L@64
	ret
L@64:	;TEST "RNC"
	add	al, 0F7H	;A=FFH,C=0,P=1,S=1,Z=0
	jz	L@65
	call	CNZI
; WARN: Is the above call correct?
L@65:	;TEST "CNZ"
	add	al, 0FEH	;A=15H,C=1,P=0,S=0,Z=0
	jnb	L@66
	ret
L@66:	;TEST "RC"
	call	CPUER
; WARN: Is the above call correct?
CNZI:	jnz	L@67
	ret
L@67:	;TEST "RZ"
	add	al, 001H	;A=00H,C=1,P=1,S=0,Z=1
	jnz	L@68
	call	CZI
; WARN: Is the above call correct?
L@68:	;TEST "CZ"
	add	al, 0D0H	;A=17H,C=1,P=1,S=0,Z=0
	jz	L@69
	ret
L@69:	;TEST "RNZ"
	call	CPUER
; WARN: Is the above call correct?
CZI:	jz	L@70
	ret
L@70:	;TEST "RNZ"
	add	al, 047H	;A=47H,C=0,P=1,S=0,Z=0
	cmp	al, 047H	;A=47H,C=0,P=1,S=0,Z=1
	jnz	L@71
	ret
L@71:	;TEST "RZ"
	call	CPUER
; WARN: Is the above call correct?
;
;
;
;TEST "MOV","INR",AND "DCR" INSTRUCTIONS
;
MOVI:	mov	al, 077H
	inc	al
	mov	ch, al
	inc	ch
	mov	cl, ch
	dec	cl
	mov	dh, cl
	mov	dl, dh
	mov	bh, dl
	mov	bl, bh
	mov	al, bl	;TEST "MOV" A,L,H,E,D,C,B,A
	dec	al
	mov	cl, al
	mov	dl, cl
	mov	bl, dl
	mov	ch, bl
	mov	dh, ch
	mov	bh, dh
	mov	al, bh	;TEST "MOV" A,H,D,B,L,E,C,A
	mov	dh, al
	inc	dh
	mov	bl, dh
	mov	cl, bl
	inc	cl
	mov	bh, cl
	mov	ch, bh
	dec	ch
	mov	dl, ch
	mov	al, dl	;TEST "MOV" A,E,B,H,C,L,D,A
	mov	dl, al
	inc	dl
	mov	ch, dl
	mov	bh, ch
	inc	bh
	mov	cl, bh
	mov	bl, cl
	mov	dh, bl
	dec	dh
	mov	al, dh	;TEST "MOV" A,D,L,C,H,B,E,A
	mov	bh, al
	dec	bh
	mov	dh, bh
	mov	ch, dh
	mov	bl, ch
	inc	bl
	mov	dl, bl
	dec	dl
	mov	cl, dl
	mov	al, cl	;TEST "MOV" A,C,E,L,B,D,H,A
	mov	bl, al
	dec	bl
	mov	bh, bl
	mov	dl, bh
	mov	dh, dl
	mov	cl, dh
	mov	ch, cl
	mov	al, ch
	cmp	al, 077H
	jz	L@72
	call	CPUER
; WARN: Is the above call correct?
L@72:	;TEST "MOV" A,B,C,D,E,H,L,A
;
;
;
;TEST ARITHMETIC AND LOGIC INSTRUCTIONS
;
	xor	al, al
	mov	ch, 001H
	mov	cl, 003H
	mov	dh, 007H
	mov	dl, 00FH
	mov	bh, 01FH
	mov	bl, 03FH
	add	al, ch
	add	al, cl
	add	al, dh
	add	al, dl
	add	al, bh
	add	al, bl
	add	al, al
	cmp	al, 0F0H
	jz	L@73
	call	CPUER
; WARN: Is the above call correct?
L@73:	;TEST "ADD" B,C,D,E,H,L,A
	sub	al, ch
	sub	al, cl
	sub	al, dh
	sub	al, dl
	sub	al, bh
	sub	al, bl
	cmp	al, 078H
	jz	L@74
	call	CPUER
; WARN: Is the above call correct?
L@74:	;TEST "SUB" B,C,D,E,H,L
	sub	al, al
	jz	L@75
	call	CPUER
; WARN: Is the above call correct?
L@75:	;TEST "SUB" A
	mov	al, 080H
	add	al, al
	mov	ch, 001H
	mov	cl, 002H
	mov	dh, 003H
	mov	dl, 004H
	mov	bh, 005H
	mov	bl, 006H
	adc	al, ch
	mov	ch, 080H
	add	al, ch
	add	al, ch
	adc	al, cl
	add	al, ch
	add	al, ch
	adc	al, dh
	add	al, ch
	add	al, ch
	adc	al, dl
	add	al, ch
	add	al, ch
	adc	al, bh
	add	al, ch
	add	al, ch
	adc	al, bl
	add	al, ch
	add	al, ch
	adc	al, al
	cmp	al, 037H
	jz	L@76
	call	CPUER
; WARN: Is the above call correct?
L@76:	;TEST "ADC" B,C,D,E,H,L,A
	mov	al, 080H
	add	al, al
	mov	ch, 001H
	sbb	al, ch
	mov	ch, 0FFH
	add	al, ch
	sbb	al, cl
	add	al, ch
	sbb	al, dh
	add	al, ch
	sbb	al, dl
	add	al, ch
	sbb	al, bh
	add	al, ch
	sbb	al, bl
	cmp	al, 0E0H
	jz	L@77
	call	CPUER
; WARN: Is the above call correct?
L@77:	;TEST "SBB" B,C,D,E,H,L
	mov	al, 080H
	add	al, al
	sbb	al, al
	cmp	al, 0FFH
	jz	L@78
	call	CPUER
; WARN: Is the above call correct?
L@78:	;TEST "SBB" A
	mov	al, 0FFH
	mov	ch, 0FEH
	mov	cl, 0FCH
	mov	dh, 0EFH
	mov	dl, 07FH
	mov	bh, 0F4H
	mov	bl, 0BFH
	and	al, ch	;changed from ANA A (mwd)
	and	al, cl
	and	al, dh
	and	al, dl
	and	al, bh
	and	al, bl
	and	al, al
	cmp	al, 024H
	jz	L@79
	call	CPUER
; WARN: Is the above call correct?
L@79:	;TEST "ANA" B,C,D,E,H,L,A
	xor	al, al
	mov	ch, 001H
	mov	cl, 002H
	mov	dh, 004H
	mov	dl, 008H
	mov	bh, 010H
	mov	bl, 020H
	or	al, ch
	or	al, cl
	or	al, dh
	or	al, dl
	or	al, bh
	or	al, bl
	or	al, al
	cmp	al, 03FH
	jz	L@80
	call	CPUER
; WARN: Is the above call correct?
L@80:	;TEST "ORA" B,C,D,E,H,L,A
	mov	al, 000H
	mov	bh, 08FH
	mov	bl, 04FH
	xor	al, ch
	xor	al, cl
	xor	al, dh
	xor	al, dl
	xor	al, bh
	xor	al, bl
	cmp	al, 0CFH
	jz	L@81
	call	CPUER
; WARN: Is the above call correct?
L@81:	;TEST "XRA" B,C,D,E,H,L
	xor	al, al
	jz	L@82
	call	CPUER
; WARN: Is the above call correct?
L@82:	;TEST "XRA" A
	mov	ch, 044H
	mov	cl, 045H
	mov	dh, 046H
	mov	dl, 047H
	mov	bh, (TEMP0 / 0FFH)		;HIGH BYTE OF TEST MEMORY LOCATION
	mov	bl, (TEMP0 AND 0FFH)		;LOW BYTE OF TEST MEMORY LOCATION
	mov	m, ch
	mov	ch, 000H
	mov	ch, m
	mov	al, 044H
	cmp	al, ch
	jz	L@83
	call	CPUER
; WARN: Is the above call correct?
L@83:	;TEST "MOV" M,B AND B,M
	mov	m, cl	;added (mwd)
	mov	cl, 000H		;added (mwd)
	mov	cl, m	;added (mwd)
	mov	al, 045H		;added (mwd)
	cmp	al, cl	;added (mwd)
	jz	L@84
	call	CPUER
; WARN: Is the above call correct?
L@84:	;TEST "MOV" M,C AND C,M	added (mwd)
	mov	m, dh
	mov	dh, 000H
	mov	dh, m
	mov	al, 046H
	cmp	al, dh
	jz	L@85
	call	CPUER
; WARN: Is the above call correct?
L@85:	;TEST "MOV" M,D AND D,M
	mov	m, dl
	mov	dl, 000H
	mov	dl, m
	mov	al, 047H
	cmp	al, dl
	jz	L@86
	call	CPUER
; WARN: Is the above call correct?
L@86:	;TEST "MOV" M,E AND E,M
	mov	m, bh
	mov	bh, (TEMP0 / 0FFH)
	mov	bl, (TEMP0 AND 0FFH)
	mov	bh, m
	mov	al, (TEMP0 / 0FFH)
	cmp	al, bh
	jz	L@87
	call	CPUER
; WARN: Is the above call correct?
L@87:	;TEST "MOV" M,H AND H,M
	mov	m, bl
	mov	bh, (TEMP0 / 0FFH)
	mov	bl, (TEMP0 AND 0FFH)
	mov	bl, m
	mov	al, (TEMP0 AND 0FFH)
	cmp	al, bl
	jz	L@88
	call	CPUER
; WARN: Is the above call correct?
L@88:	;TEST "MOV" M,L AND L,M
	mov	bh, (TEMP0 / 0FFH)
	mov	bl, (TEMP0 AND 0FFH)
	mov	al, 032H
	mov	m, al
	cmp	al, m
	jz	L@89
	call	CPUER
; WARN: Is the above call correct?
L@89:	;TEST "MOV" M,A
	add	al, m
	cmp	al, 064H
	jz	L@90
	call	CPUER
; WARN: Is the above call correct?
L@90:	;TEST "ADD" M
	xor	al, al
	mov	al, m
	cmp	al, 032H
	jz	L@91
	call	CPUER
; WARN: Is the above call correct?
L@91:	;TEST "MOV" A,M
	mov	bh, (TEMP0 / 0FFH)
	mov	bl, (TEMP0 AND 0FFH)
	mov	al, m
	sub	al, m
	jz	L@92
	call	CPUER
; WARN: Is the above call correct?
L@92:	;TEST "SUB" M
	mov	al, 080H
	add	al, al
	adc	al, m
	cmp	al, 033H
	jz	L@93
	call	CPUER
; WARN: Is the above call correct?
L@93:	;TEST "ADC" M
	mov	al, 080H
	add	al, al
	sbb	al, m
	cmp	al, 0CDH
	jz	L@94
	call	CPUER
; WARN: Is the above call correct?
L@94:	;TEST "SBB" M
	and	al, m
	jz	L@95
	call	CPUER
; WARN: Is the above call correct?
L@95:	;TEST "ANA" M
	mov	al, 025H
	or	al, m
	cmp	al, 037H
	jz	L@96
	call	CPUER
; WARN: Is the above call correct?
L@96:	;TEST "ORA" M
	xor	al, m
	cmp	al, 005H
	jz	L@97
	call	CPUER
; WARN: Is the above call correct?
L@97:	;TEST "XRA" M
	mov	m, 055H
	inc	m
	dec	m
	add	al, m
	cmp	al, 05AH
	jz	L@98
	call	CPUER
; WARN: Is the above call correct?
L@98:	;TEST "INR","DCR",AND "MVI" M
	mov	cx, 12FFH
	mov	dx, 12FFH
	mov	bx, 12FFH
	lahf
	inc	cx
	sahf
	lahf
	inc	dx
	sahf
	lahf
	inc	bx
	sahf
	mov	al, 013H
	cmp	al, ch
	jz	L@99
	call	CPUER
; WARN: Is the above call correct?
L@99:	;TEST "LXI" AND "INX" B
	cmp	al, dh
	jz	L@100
	call	CPUER
; WARN: Is the above call correct?
L@100:	;TEST "LXI" AND "INX" D
	cmp	al, bh
	jz	L@101
	call	CPUER
; WARN: Is the above call correct?
L@101:	;TEST "LXI" AND "INX" H
	mov	al, 000H
	cmp	al, cl
	jz	L@102
	call	CPUER
; WARN: Is the above call correct?
L@102:	;TEST "LXI" AND "INX" B
	cmp	al, dl
	jz	L@103
	call	CPUER
; WARN: Is the above call correct?
L@103:	;TEST "LXI" AND "INX" D
	cmp	al, bl
	jz	L@104
	call	CPUER
; WARN: Is the above call correct?
L@104:	;TEST "LXI" AND "INX" H
	lahf
	dec	cx
	sahf
	lahf
	dec	dx
	sahf
	lahf
	dec	bx
	sahf
	mov	al, 012H
	cmp	al, ch
	jz	L@105
	call	CPUER
; WARN: Is the above call correct?
L@105:	;TEST "DCX" B
	cmp	al, dh
	jz	L@106
	call	CPUER
; WARN: Is the above call correct?
L@106:	;TEST "DCX" D
	cmp	al, bh
	jz	L@107
	call	CPUER
; WARN: Is the above call correct?
L@107:	;TEST "DCX" H
	mov	al, 0FFH
	cmp	al, cl
	jz	L@108
	call	CPUER
; WARN: Is the above call correct?
L@108:	;TEST "DCX" B
	cmp	al, dl
	jz	L@109
	call	CPUER
; WARN: Is the above call correct?
L@109:	;TEST "DCX" D
	cmp	al, bl
	jz	L@110
	call	CPUER
; WARN: Is the above call correct?
L@110:	;TEST "DCX" H
	mov	[TEMP0], al
	xor	al, al
	mov	al, [TEMP0]
	cmp	al, 0FFH
	jz	L@111
	call	CPUER
; WARN: Is the above call correct?
L@111:	;TEST "LDA" AND "STA"
	mov	bx, [TEMPP]
	mov	[TEMP0], bx
	mov	al, [TEMPP]
	mov	ch, al
	mov	al, [TEMP0]
	cmp	al, ch
	jz	L@112
	call	CPUER
; WARN: Is the above call correct?
L@112:	;TEST "LHLD" AND "SHLD"
	mov	al, [TEMPP+1]
	mov	ch, al
	mov	al, [TEMP0+1]
	cmp	al, ch
	jz	L@113
	call	CPUER
; WARN: Is the above call correct?
L@113:	;TEST "LHLD" AND "SHLD"
	mov	al, 0AAH
	mov	[TEMP0], al
	mov	ch, bh
	mov	cl, bl
	xor	al, al
	mov	si, cx
	mov	al, [si]
	cmp	al, 0AAH
	jz	L@114
	call	CPUER
; WARN: Is the above call correct?
L@114:	;TEST "LDAX" B
	inc	al
	mov	di, cx
	mov	[di], al
	mov	al, [TEMP0]
	cmp	al, 0ABH
	jz	L@115
	call	CPUER
; WARN: Is the above call correct?
L@115:	;TEST "STAX" B
	mov	al, 077H
	mov	[TEMP0], al
	mov	bx, [TEMPP]
	mov	dx, 00000H
	xchg	bx, dx
	xor	al, al
	mov	si, dx
	mov	al, [si]
	cmp	al, 077H
	jz	L@116
	call	CPUER
; WARN: Is the above call correct?
L@116:	;TEST "LDAX" D AND "XCHG"
	xor	al, al
	add	al, bh
	add	al, bl
	jz	L@117
	call	CPUER
; WARN: Is the above call correct?
L@117:	;TEST "XCHG"
	mov	al, 0CCH
	mov	di, dx
	mov	[di], al
	mov	al, [TEMP0]
	cmp	al, 0CCH
	mov	di, dx
	mov	[di], al
	mov	al, [TEMP0]
	cmp	al, 0CCH
	jz	L@118
	call	CPUER
; WARN: Is the above call correct?
L@118:	;TEST "STAX" D
	mov	bx, 07777H
	lahf
	add	bx, bx
	rcr	si, 1
	sahf
	rcl	si, 1
	mov	al, 0EEH
	cmp	al, bh
	jz	L@119
	call	CPUER
; WARN: Is the above call correct?
L@119:	;TEST "DAD" H
	cmp	al, bl
	jz	L@120
	call	CPUER
; WARN: Is the above call correct?
L@120:	;TEST "DAD" H
	mov	bx, 05555H
	mov	cx, 0FFFFH
	lahf
	add	bx, cx
	rcr	si, 1
	sahf
	rcl	si, 1
	mov	al, 055H
	jnae	L@121
	call	CPUER
; WARN: Is the above call correct?
L@121:	;TEST "DAD" B
	cmp	al, bh
	jz	L@122
	call	CPUER
; WARN: Is the above call correct?
L@122:	;TEST "DAD" B
	mov	al, 054H
	cmp	al, bl
	jz	L@123
	call	CPUER
; WARN: Is the above call correct?
L@123:	;TEST "DAD" B
	mov	bx, 0AAAAH
	mov	dx, 03333H
	lahf
	add	bx, dx
	rcr	si, 1
	sahf
	rcl	si, 1
	mov	al, 0DDH
	cmp	al, bh
	jz	L@124
	call	CPUER
; WARN: Is the above call correct?
L@124:	;TEST "DAD" D
	cmp	al, bl
	jz	L@125
	call	CPUER
; WARN: Is the above call correct?
L@125:	;TEST "DAD" B
	stc
	jnae	L@126
	call	CPUER
; WARN: Is the above call correct?
L@126:	;TEST "STC"
	cmc
	jnb	L@127
	call	CPUER
; WARN: Is the above call correct?
L@127:	;TEST "CMC
	mov	al, 0AAH
	not	al
	cmp	al, 055H
	jz	L@128
	call	CPUER
; WARN: Is the above call correct?
L@128:	;TEST "CMA"
	or	al, al	;RE-SET AUXILIARY CARRY
	daa
	cmp	al, 055H
	jz	L@129
	call	CPUER
; WARN: Is the above call correct?
L@129:	;TEST "DAA"
	mov	al, 088H
	add	al, al
	daa
	cmp	al, 076H
	jz	L@130
	call	CPUER
; WARN: Is the above call correct?
L@130:	;TEST "DAA"
	xor	al, al
	mov	al, 0AAH
	daa
	jnae	L@131
	call	CPUER
; WARN: Is the above call correct?
L@131:	;TEST "DAA"
	cmp	al, 010H
	jz	L@132
	call	CPUER
; WARN: Is the above call correct?
L@132:	;TEST "DAA"
	xor	al, al
	mov	al, 09AH
	daa
	jnae	L@133
	call	CPUER
; WARN: Is the above call correct?
L@133:	;TEST "DAA"
	jz	L@134
	call	CPUER
; WARN: Is the above call correct?
L@134:	;TEST "DAA"
	stc
	mov	al, 042H
	rol	al, 1
	jnb	L@135
	call	CPUER
; WARN: Is the above call correct?
L@135:	;TEST "RLC" FOR RE-SET CARRY
	rol	al, 1
	jnae	L@136
	call	CPUER
; WARN: Is the above call correct?
L@136:	;TEST "RLC" FOR SET CARRY
	cmp	al, 009H
	jz	L@137
	call	CPUER
; WARN: Is the above call correct?
L@137:	;TEST "RLC" FOR ROTATION
	ror	al, 1
	jnae	L@138
	call	CPUER
; WARN: Is the above call correct?
L@138:	;TEST "RRC" FOR SET CARRY
	ror	al, 1
	cmp	al, 042H
	jz	L@139
	call	CPUER
; WARN: Is the above call correct?
L@139:	;TEST "RRC" FOR ROTATION
	rcl	al, 1
	rcl	al, 1
	jnae	L@140
	call	CPUER
; WARN: Is the above call correct?
L@140:	;TEST "RAL" FOR SET CARRY
	cmp	al, 008H
	jz	L@141
	call	CPUER
; WARN: Is the above call correct?
L@141:	;TEST "RAL" FOR ROTATION
	rcr	al, 1
	rcr	al, 1
	jnb	L@142
	call	CPUER
; WARN: Is the above call correct?
L@142:	;TEST "RAR" FOR RE-SET CARRY
	cmp	al, 002H
	jz	L@143
	call	CPUER
; WARN: Is the above call correct?
L@143:	;TEST "RAR" FOR ROTATION
	mov	cx, 01234H
	mov	dx, 0AAAAH
	mov	bx, 05555H
	xor	al, al
	push	cx
	push	dx
	push	bx
	lahf
	xchg	al, ah
	push	ax
	xchg	al, ah
	mov	cx, 00000H
	mov	dx, 00000H
	mov	bx, 00000H
	mov	al, 0C0H
	add	al, 0F0H
	pop	ax
	xchg	al, ah
	sahf
	pop	bx
	pop	dx
	pop	cx
	jnb	L@144
	call	CPUER
; WARN: Is the above call correct?
L@144:	;TEST "PUSH PSW" AND "POP PSW"
	jz	L@145
	call	CPUER
; WARN: Is the above call correct?
L@145:	;TEST "PUSH PSW" AND "POP PSW"
	jp	L@146
	call	CPUER
; WARN: Is the above call correct?
L@146:	;TEST "PUSH PSW" AND "POP PSW"
	jns	L@147
	call	CPUER
; WARN: Is the above call correct?
L@147:	;TEST "PUSH PSW" AND "POP PSW"
	mov	al, 012H
	cmp	al, ch
	jz	L@148
	call	CPUER
; WARN: Is the above call correct?
L@148:	;TEST "PUSH B" AND "POP B"
	mov	al, 034H
	cmp	al, cl
	jz	L@149
	call	CPUER
; WARN: Is the above call correct?
L@149:	;TEST "PUSH B" AND "POP B"
	mov	al, 0AAH
	cmp	al, dh
	jz	L@150
	call	CPUER
; WARN: Is the above call correct?
L@150:	;TEST "PUSH D" AND "POP D"
	cmp	al, dl
	jz	L@151
	call	CPUER
; WARN: Is the above call correct?
L@151:	;TEST "PUSH D" AND "POP D"
	mov	al, 055H
	cmp	al, bh
	jz	L@152
	call	CPUER
; WARN: Is the above call correct?
L@152:	;TEST "PUSH H" AND "POP H"
	cmp	al, bl
	jz	L@153
	call	CPUER
; WARN: Is the above call correct?
L@153:	;TEST "PUSH H" AND "POP H"
	mov	bx, 00000H
	lahf
	add	bx, sp
	rcr	si, 1
	sahf
	rcl	si, 1
	mov	[SAVSTK], bx	;SAVE THE "OLD" STACK-POINTER!
	mov	sp, TEMP4
	lahf
	dec	sp
	sahf
	lahf
	dec	sp
	sahf
	lahf
	inc	sp
	sahf
	lahf
	dec	sp
	sahf
	mov	al, 055H
	mov	[TEMP2], al
	not	al
	mov	[TEMP3], al
	pop	cx
	cmp	al, ch
	jz	L@154
	call	CPUER
; WARN: Is the above call correct?
L@154:	;TEST "LXI","DAD","INX",AND "DCX" SP
	not	al
	cmp	al, cl
	jz	L@155
	call	CPUER
; WARN: Is the above call correct?
L@155:	;TEST "LXI","DAD","INX", AND "DCX" SP
	mov	bx, TEMP4
	mov	sp, bx
	mov	bx, 07733H
	lahf
	dec	sp
	sahf
	lahf
	dec	sp
	sahf
	pop	si
	xchg	bx, si
	push	si
	mov	al, [TEMP3]
	cmp	al, 077H
	jz	L@156
	call	CPUER
; WARN: Is the above call correct?
L@156:	;TEST "SPHL" AND "XTHL"
	mov	al, [TEMP2]
	cmp	al, 033H
	jz	L@157
	call	CPUER
; WARN: Is the above call correct?
L@157:	;TEST "SPHL" AND "XTHL"
	mov	al, 055H
	cmp	al, bl
	jz	L@158
	call	CPUER
; WARN: Is the above call correct?
L@158:	;TEST "SPHL" AND "XTHL"
	not	al
	cmp	al, bh
	jz	L@159
	call	CPUER
; WARN: Is the above call correct?
L@159:	;TEST "SPHL" AND "XTHL"
	mov	bx, [SAVSTK]	;RESTORE THE "OLD" STACK-POINTER
	mov	sp, bx
	mov	bx, CPUOK
	jmp	bx	;TEST "PCHL"
;
;
;
CPUER:	mov	bx, NGCPU		;OUTPUT "CPU HAS FAILED    ERROR EXIT=" TO CONSOLE
	call	MSG
; WARN: Is the above call correct?
	pop	bx	;HL = ADDRESS FOLLOWING CALL CPUER
	push	bx
	mov	al, bh
	call	BYTEO
; WARN: Is the above call correct?	;SHOW ERROR EXIT ADDRESS HIGH BYTE
	pop	bx
	mov	al, bl
	call	BYTEO
; WARN: Is the above call correct?	;SHOW ERROR EXIT ADDRESS LOW BYTE
	mov	cl, 0
	mov	dl, 0
	mov	ah, 4ch
	int	21h
	ret	;EXIT TO CP/M WARM BOOT
;
;
;
CPUOK:	mov	bx, OKCPU		;OUTPUT "CPU IS OPERATIONAL" TO CONSOLE
	call	MSG
; WARN: Is the above call correct?
	mov	cl, 0
	mov	dl, 0
	mov	ah, 4ch
	int	21h
	ret	;EXIT TO CP/M WARM BOOT
;
;
;
TEMPP:	dw	TEMP0	;POINTER USED TO TEST "LHLD","SHLD",
; AND "LDAX" INSTRUCTIONS
;
TEMP0:	resb	1	;TEMPORARY STORAGE FOR CPU TEST MEMORY LOCATIONS
TEMP1:	resb	1	;TEMPORARY STORAGE FOR CPU TEST MEMORY LOCATIONS
TEMP2:	resb	1	;TEMPORARY STORAGE FOR CPU TEST MEMORY LOCATIONS
TEMP3:	resb	1	;TEMPORARY STORAGE FOR CPU TEST MEMORY LOCATIONS
TEMP4:	resb	1	;TEMPORARY STORAGE FOR CPU TEST MEMORY LOCATIONS
SAVSTK:	resb	2	;TEMPORARY STACK-POINTER STORAGE LOCATION
;
;
;
STACK	equ	TEMPP+256	;DE-BUG STACK POINTER STORAGE AREA
;
;
;
	
