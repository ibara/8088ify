/*
 * Copyright (c) 2021 Brian Callahan <bcallah@openbsd.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * 8088ify -- Intel 8080 CP/M to 8086 (8088) MS-DOS assembly translator
 * Written for PCjam 2021: https://pcjam.gitlab.io/
 */

static FILE *fq;

static char line[256];

static char lab[256];
static char op[256];
static char a1[256];
static char a2[256];
static char comm[256];

static char bdos[256];
static char warm[256];

static int bang;
static int bdosfound;
static int labno;
static int warmfound;

static int
egetline(FILE *fp)
{
	int ch, i;

	for (i = 0; i < sizeof(line); i++)
		line[i] = '\0';

	for (i = 0; i < sizeof(line) - 1; i++) {
		ch = fgetc(fp);
		if (ch == '\n' || ch == EOF)
			break;
		line[i] = (ch == '\r') ? '\0' : ch;
	}

	while (ch != '\n' && ch != EOF)
		ch = fgetc(fp);

	if (ch == EOF)
		return 1;

	return 0;
}

static int
endoftoken(int ch)
{
	if (ch == ' ' || ch == '\t' || ch == ';' || ch == '\0' || ch == '!')
		return 1;

	return 0;
}

static void
lex(void)
{
	int i, j;

	/* Reset buffers */
	bang = 0;
	for (i = 0; i < sizeof(line); i++) {
		lab[i] = '\0';
		op[i] = '\0';
		a1[i] = '\0';
		a2[i] = '\0';
		comm[i] = '\0';
	}
	i = 0;

	/* Empty line special case */
	if (line[0] == '\0')
		return;

	/* Label */
	if (line[0] != ' ' && line[0] != '\t') {
		j = 0;
		while (!endoftoken(line[i]) && line[i] != ':')
			lab[j++] = line[i++];
		if (line[i] == '!') {
			bang = i;
			return;
		}
		if (line[i] == ':')
			++i;
	}

	/* Whitespace */
	while (i < sizeof(line) - 1 && (line[i] == ' ' || line[i] == '\t' ||
	       line[i] == ';')) {
		if (line[i] == '!') {
			bang = i;
			return;
		}
		if (line[i] == ';') {
			j = 0;
			while (line[i] != '\0')
				comm[j++] = line[i++];
			return;
		}
		++i;
	}
	if (i == sizeof(line) - 1)
		return;

	/* Opcode */
	j = 0;
	while (!endoftoken(line[i]))
		op[j++] = tolower(line[i++]);

	if (line[i] == '!') {
		bang = i;
		return;
	}

	/* Whitespace */
	while (i < sizeof(line) - 1 && (line[i] == ' ' || line[i] == '\t' ||
	       line[i] == ';')) {
		if (line[i] == '!') {
			bang = i;
			return;
		}
		if (line[i] == ';') {
			j = 0;
			while (line[i] != '\0')
				comm[j++] = line[i++];
			return;
		}
		++i;
	}
	if (i == sizeof(line) - 1)
		return;

	/* First argument */
	j = 0;
	if (!strcmp(op, "db")) {
again:
		/* Whitespace */
		while (i < sizeof(line) - 1 && (line[i] == ' ' ||
		       line[i] == '\t' || line[i] == ';')) {
			if (line[i] == '!') {
				bang = i;
				return;
			}
			if (line[i] == ';') {
				j = 0;
				while (line[i] != '\0')
					comm[j++] = line[i++];
				return;
			}
			++i;
		}
		if (i == sizeof(line) - 1)
			return;

		if (line[i] == '\'') {
			a1[j++] = line[i++];
			for (; i < sizeof(line) - 1; i++) {
				if (line[i] == '\'') {
					if (i != sizeof(line) - 2 &&
					    line[i + 1] == '\'') {
						a1[j++] = '\'';
						++i;
						continue;
					} else {
						break;
					}
				}

				a1[j++] = line[i];
			}
			a1[j++] = line[i++];
			/* Whitespace */
			while (i < sizeof(line) - 1 && (line[i] == ' ' ||
			       line[i] == '\t' || line[i] == ';')) {
				if (line[i] == '!') {
					bang = i;
					return;
				}
				if (line[i] == ';') {
					j = 0;
					while (line[i] != '\0')
						comm[j++] = line[i++];
					return;
				}
				++i;
			}
			if (i == sizeof(line) - 1)
				return;

			if (line[i] == ',') {
				a1[j++] = line[i++];
				goto again;
			}
		} else {
			while (line[i] != ';' && line[i] != '!' &&
			       line[i] != '\0' && line[i] != ',')
				a1[j++] = line[i++];
			if (line[i] == ',') {
				a1[j++] = line[i++];
				goto again;
			}
		}
	} else {
		while (!endoftoken(line[i]) && line[i] != ',')
			a1[j++] = line[i++];
	}
	if (line[i] == ',')
		++i;

	if (line[i] == '!') {
		bang = i;
		return;
	}

	/* Whitespace */
	while (i < sizeof(line) - 1 && (line[i] == ' ' || line[i] == '\t' ||
	       line[i] == ';')) {
		if (line[i] == '!') {
			bang = i;
			return;
		}
		if (line[i] == ';') {
			j = 0;
			while (line[i] != '\0')
				comm[j++] = line[i++];
			return;
		}
		++i;
	}
	if (i == sizeof(line) - 1)
		return;

	/* Second argument */
	j = 0;
	while (line[i] != '\0' && line[i] != ';' && line[i] !='!')
		a2[j++] = line[i++];

	if (line[i] == '!') {
		bang = i;
		return;
	}

	/* Whitespace */
	while (i < sizeof(line) - 1 && (line[i] == ' ' || line[i] == '\t' ||
	       line[i] == ';')) {
		if (line[i] == '!') {
			bang = i;
			return;
		}
		if (line[i] == ';') {
			j = 0;
			while (line[i] != '\0')
				comm[j++] = line[i++];
			return;
		}
		++i;
	}
}

static char *
eight(const char *a)
{
	char b[256];
	static char c[256];
	int i;

	for (i = 0; i < sizeof(b); i++)
		b[i] = '\0';

	for (i = 0; i < strlen(a); i++) {
		if (a[i] == ' ' || a[i] == '\t')
			break;
		b[i] = a[i];
	}

	if (!strcmp(b, "A") || !strcmp(b, "a"))
		return "al";
	if (!strcmp(b, "B") || !strcmp(b, "b"))
		return "ch";
	if (!strcmp(b, "C") || !strcmp(b, "c"))
		return "cl";
	if (!strcmp(b, "D") || !strcmp(b, "d"))
		return "dh";
	if (!strcmp(b, "E") || !strcmp(b, "e"))
		return "dl";
	if (!strcmp(b, "H") || !strcmp(b, "h"))
		return "bh";
	if (!strcmp(b, "L") || !strcmp(b, "l"))
		return "bl";
	if (!strcmp(b, "M") || !strcmp(b, "m"))
		return "m";

	for (i = 0; i < sizeof(c); i++)
		c[i] = '\0';

	c[0] = '[';
	for (i = 0; i < strlen(a); i++)
		c[i + 1] = a[i];
	c[i + 1] = ']';

	return c;
}

static char *
sixteen(const char *a)
{
	char b[256];
	static char c[256];
	int i;

	for (i = 0; i < sizeof(b); i++)
		b[i] = '\0';

	for (i = 0; i < strlen(a); i++) {
		if (a[i] == ' ' || a[i] == '\t')
			break;
		b[i] = a[i];
	}

	if (!strcmp(b, "B") || !strcmp(b, "b"))
		return "cx";
	if (!strcmp(b, "D") || !strcmp(b, "d"))
		return "dx";
	if (!strcmp(b, "H") || !strcmp(b, "h"))
		return "bx";
	if (!strcmp(b, "PSW") || !strcmp(b, "psw"))
		return "ax";
	if (!strcmp(b, "SP") || !strcmp(b, "sp"))
		return "sp";

	for (i = 0; i < sizeof(c); i++)
		c[i] = '\0';

	c[0] = '[';
	for (i = 0; i < strlen(a); i++)
		c[i + 1] = a[i];
	c[i + 1] = ']';

	return c;
}

static void
nop(void)
{

	fprintf(fq, "nop");
}

static void
lxi(void)
{

	fprintf(fq, "mov\t%s, %s", sixteen(a1), a2);
}

static void
stax(void)
{

	fprintf(fq, "mov\tdi, %s\n", sixteen(a1));
	fprintf(fq, "\tmov\t[di], al");
}

static void
inx(void)
{

	fprintf(fq, "lahf\n");
	fprintf(fq, "\tinc\t%s\n", sixteen(a1));
	fprintf(fq, "\tsahf");
}

static void
inr(void)
{

	fprintf(fq, "inc\t%s", eight(a1));
}

static void
dcr(void)
{

	fprintf(fq, "dec\t%s", eight(a1));
}

static void
mvi(void)
{

	fprintf(fq, "mov\t%s, %s", eight(a1), a2);
}

static void
rlc(void)
{

	fprintf(fq, "rol\tal, 1");
}

static void
dad(void)
{

	fprintf(fq, "lahf\n");
	fprintf(fq, "\tadd\tbx, %s\n", sixteen(a1));
	fprintf(fq, "\trcr\tsi, 1\n");
	fprintf(fq, "\tsahf\n");
	fprintf(fq, "\trcl\tsi, 1");
}

static void
ldax(void)
{

	fprintf(fq, "mov\tsi, %s\n", sixteen(a1));
	fprintf(fq, "\tmov\tal, [si]");
}

static void
dcx(void)
{

	fprintf(fq, "lahf\n");
	fprintf(fq, "\tdec\t%s\n", sixteen(a1));
	fprintf(fq, "\tsahf");
}

static void
rrc(void)
{

	fprintf(fq, "ror\tal, 1");
}

static void
ral(void)
{

	fprintf(fq, "rcl\tal, 1");
}

static void
rar(void)
{

	fprintf(fq, "rcr\tal, 1");
}

static void
shld(void)
{

	fprintf(fq, "mov\t[%s], bx", a1);
}

static void
daa(void)
{

	fprintf(fq, "daa");
}

static void
lhld(void)
{

	fprintf(fq, "mov\tbx, [%s]", a1);
}

static void
cma(void)
{

	fprintf(fq, "not\tal");
}

static void
sta(void)
{

	fprintf(fq, "mov\t[%s], al", a1);
}

static void
stc(void)
{

	fprintf(fq, "stc");
}

static void
lda(void)
{

	fprintf(fq, "mov\tal, [%s]", a1);
}

static void
cmc(void)
{

	fprintf(fq, "cmc");
}

static void
mov(void)
{

	fprintf(fq, "mov\t%s, %s", eight(a1), eight(a2));
}

static void
hlt(void)
{

	fprintf(fq, "hlt");
}

static void
add(void)
{

	fprintf(fq, "add\tal, %s", eight(a1));
}

static void
adc(void)
{

	fprintf(fq, "adc\tal, %s", eight(a1));
}

static void
sub(void)
{

	fprintf(fq, "sub\tal, %s", eight(a1));
}

static void
sbb(void)
{

	fprintf(fq, "sbb\tal, %s", eight(a1));
}

static void
ana(void)
{

	fprintf(fq, "and\tal, %s", eight(a1));
}

static void
xra(void)
{

	fprintf(fq, "xor\tal, %s", eight(a1));
}

static void
ora(void)
{

	fprintf(fq, "or\tal, %s", eight(a1));
}

static void
cmp(void)
{

	fprintf(fq, "cmp\tal, %s", eight(a1));
}

static void
push(void)
{

	if (!strcmp(a1, "PSW") || !strcmp(a1, "psw")) {
		fprintf(fq, "lahf\n");
		fprintf(fq, "\txchg\tal, ah\n");
		fprintf(fq, "\tpush\tax\n");
		fprintf(fq, "\txchg\tal, ah");
	} else {
		fprintf(fq, "push\t%s", sixteen(a1));
	}
}

static void
newlab(void)
{

	fprintf(fq, "\nL@%d:", labno++);
}

static void
adi(void)
{

	fprintf(fq, "add\tal, %s", a1);
}

static void
rst(void)
{

	if (!strcmp(a1, "0")) {
		fprintf(fq, "mov\tah, 4ch\n");
		fprintf(fq, "\tint\t21h");
	} else {
		fprintf(fq, "int\t%s", a1);
	}
}

static void
ret(void)
{

	fprintf(fq, "ret");
}

static void
rz(void)
{

	fprintf(fq, "jnz\tL@%d\n\t", labno);
	ret();
	newlab();
}

/*
 * Checking special cases for call/jmp.
 *
 * Returns 1 if 0005h
 * Returns 2 if 0000h
 * Returns 0 otherwise
 */
static int
numcheck(void)
{
	int base;

	if (isdigit(a1[0]) && (a1[strlen(a1) - 1] == 'H' ||
	    a1[strlen(a1) - 1] == 'h')) {
		base = 16;
		goto check;
	} else if (isdigit(a1[0]) && (a1[1] == 'X' || a1[1] == 'x')) {
		base = 16;
		goto check;
	} else if (isdigit(a1[0])) {
		base = 10;
check:
		if (strtol(a1, NULL, base) == 5)
			return 1;
		if (strtol(a1, NULL, base) == 0)
			return 2;
	} else if (isalpha(a1[0])) {
		if (bdosfound) {
			if (!strcmp(a1, bdos))
				return 1;
		}
		if (warmfound) {
			if (!strcmp(a1, warm))
				return 2;
		}
	}

	return 0;
}

static int
isbdos(void)
{

	if (a1[0] == '\0')
		return 0;

	return numcheck();
}

static void
call(void)
{
	int b;

	b = isbdos();
	if (b == 1) {
		fprintf(fq, "push\tax\n");
		fprintf(fq, "\tmov\tah, cl\n");
		fprintf(fq, "\tint\t21h\n");
		fprintf(fq, "\tpop\tax");
	} else if (b == 2) {
		fprintf(fq, "mov\tah, 4ch\n");
		fprintf(fq, "\tint\t21h");
	} else {
		fprintf(fq, "call\t%s\n", a1);
		fprintf(fq, "; WARN: Is the above call correct?");
	}
}

static void
jmp(void)
{
	int b;

	b = isbdos();
	if (b == 1) {
		fprintf(fq, "push\tax\n");
		fprintf(fq, "\tmov\tah, cl\n");
		fprintf(fq, "\tint\t21h\n");
		fprintf(fq, "\tpop\tax\n");
		fprintf(fq, "\tret");
	} else if (b == 2) {
		fprintf(fq, "mov\tcl, 0\n");
		fprintf(fq, "\tmov\tdl, 0\n");
		fprintf(fq, "\tmov\tah, 4ch\n");
		fprintf(fq, "\tint\t21h\n");
		fprintf(fq, "\tret");
	} else {
		fprintf(fq, "jmp\t%s\n", a1);
		fprintf(fq, "; WARN: Is the above jmp correct?");
	}
}

static void
rnz(void)
{

	fprintf(fq, "jz\tL@%d\n\t", labno);
	ret();
	newlab();
}

static void
pop(void)
{

	if (!strcmp(a1, "PSW") || !strcmp(a1, "psw")) {
		fprintf(fq, "pop\tax\n");
		fprintf(fq, "\txchg\tal, ah\n");
		fprintf(fq, "\tsahf");
	} else {
		fprintf(fq, "pop\t%s", sixteen(a1));
	}
}

static void
jnz(void)
{

	fprintf(fq, "jz\tL@%d\n\t", labno);
	jmp();
	newlab();
}

static void
cnz(void)
{

	fprintf(fq, "jz\tL@%d\n\t", labno);
	call();
	newlab();
}

static void
jz(void)
{

	fprintf(fq, "jnz\tL@%d\n\t", labno);
	jmp();
	newlab();
}

static void
cz(void)
{

	fprintf(fq, "jnz\tL@%d\n\t", labno);
	call();
	newlab();
}

static void
aci(void)
{

	fprintf(fq, "adc\tal, %s", a1);
}

static void
rnc(void)
{

	fprintf(fq, "jnae\tL@%d\n\t", labno);
	ret();
	newlab();
}

static void
jnc(void)
{

	fprintf(fq, "jnae\tL@%d\n\t", labno);
	jmp();
	newlab();
}

static void
out(void)
{

	fprintf(fq, "out\t%s, al", a1);
}

static void
cnc(void)
{

	fprintf(fq, "jnae\tL@%d\n\t", labno);
	call();
	newlab();
}

static void
sui(void)
{

	fprintf(fq, "sub\tal, %s", a1);
}

static void
in(void)
{

	fprintf(fq, "in\tal, %s", a1);
}

static void
rc(void)
{

	fprintf(fq, "jnb\tL@%d\n\t", labno);
	ret();
	newlab();
}

static void
jc(void)
{

	fprintf(fq, "jnb\tL@%d\n\t", labno);
	jmp();
	newlab();
}

static void
cc(void)
{

	fprintf(fq, "jnb\tL@%d\n\t", labno);
	call();
	newlab();
}

static void
sbi(void)
{

	fprintf(fq, "sbb\tal, %s", a1);
}

static void
rpo(void)
{

	fprintf(fq, "jp\tL@%d\n\t", labno);
	ret();
	newlab();
}

static void
jpo(void)
{

	fprintf(fq, "jp\tL@%d\n\t", labno);
	jmp();
	newlab();
}

static void
xthl(void)
{

	fprintf(fq, "pop\tsi\n");
	fprintf(fq, "\txchg\tbx, si\n");
	fprintf(fq, "\tpush\tsi");
}

static void
cpo(void)
{

	fprintf(fq, "jp\tL@%d\n\t", labno);
	call();
	newlab();
}

static void
ani(void)
{

	fprintf(fq, "and\tal, %s", a1);
}

static void
rpe(void)
{

	fprintf(fq, "jnp\tL@%d\n\t", labno);
	ret();
	newlab();
}

static void
pchl(void)
{

	fprintf(fq, "jmp\tbx");
}

static void
jpe(void)
{

	fprintf(fq, "jnp\tL@%d\n\t", labno);
	jmp();
	newlab();
}

static void
xchg(void)
{

	fprintf(fq, "xchg\tbx, dx");
}

static void
cpe(void)
{

	fprintf(fq, "jnp\tL@%d\n\t", labno);
	call();
	newlab();
}

static void
xri(void)
{

	fprintf(fq, "xor\tal, %s", a1);
}

static void
rp(void)
{

	fprintf(fq, "js\tL@%d\n\t", labno);
	ret();
	newlab();
}

static void
jp(void)
{

	fprintf(fq, "js\tL@%d\n\t", labno);
	jmp();
	newlab();
}

static void
di(void)
{

	fprintf(fq, "cli");
}

static void
cp(void)
{

	fprintf(fq, "js\tL@%d\n\t", labno);
	call();
	newlab();
}

static void
ori(void)
{

	fprintf(fq, "or\tal, %s", a1);
}

static void
rm(void)
{

	fprintf(fq, "jns\tL@%d\n\t", labno);
	ret();
	newlab();
}

static void
sphl(void)
{

	fprintf(fq, "mov\tsp, bx");
}

static void
jm(void)
{

	fprintf(fq, "jns\tL@%d\n\t", labno);
	jmp();
	newlab();
}

static void
ei(void)
{

	fprintf(fq, "sti");
}

static void
cm(void)
{

	fprintf(fq, "jns\tL@%d\n\t", labno);
	call();
	newlab();
}

static void
cpi(void)
{

	fprintf(fq, "cmp\tal, %s", a1);
}

static void
org(void)
{

	fprintf(fq, "org\t%s", a1);
}

static void
equ(void)
{
	int i;

	fprintf(fq, "equ\t%s", a1);
	if (bdosfound == 0) {
		if (numcheck() == 1) {
			for (i = 0; i < sizeof(bdos); i++)
				bdos[i] = '\0';
			for (i = 0; i < strlen(lab); i++)
				bdos[i] = lab[i];
			bdosfound = 1;
		}
	}
	if (warmfound == 0)
		if (numcheck() == 2) {
			for (i = 0; i < sizeof(warm); i++)
				warm[i] = '\0';
			for (i = 0; i < strlen(lab); i++)
				warm[i] = lab[i];
			warmfound = 1;
	}
}

static void
db(void)
{

	fprintf(fq, "db\t%s", a1);
}

static void
dw(void)
{

	fprintf(fq, "dw\t%s", a1);
}

static void
ds(void)
{

	fprintf(fq, "resb\t%s", a1);
}

static void
end(void)
{

	/* Do nothing */
}

/*
 * Big switch of all 8080 opcodes and their 8086 translations.
 */
struct trans {
	const char *op80;
	void (*cb)(void);
} tab[] = {
	{ "nop", nop },
	{ "lxi", lxi },
	{ "stax", stax },
	{ "inx", inx },
	{ "inr", inr },
	{ "dcr", dcr },
	{ "mvi", mvi },
	{ "rlc", rlc },
	{ "dad", dad },
	{ "ldax", ldax },
	{ "dcx", dcx },
	{ "rrc", rrc },
	{ "ral", ral },
	{ "rar", rar },
	{ "shld", shld },
	{ "daa", daa },
	{ "lhld", lhld },
	{ "cma", cma },
	{ "sta", sta },
	{ "stc", stc },
	{ "lda", lda },
	{ "cmc", cmc },
	{ "mov", mov },
	{ "hlt", hlt },
	{ "add", add },
	{ "adc", adc },
	{ "sub", sub },
	{ "sbb", sbb },
	{ "ana", ana },
	{ "xra", xra },
	{ "ora", ora },
	{ "cmp", cmp },
	{ "rnz", rnz },
	{ "pop", pop },
	{ "jnz", jnz },
	{ "jmp", jmp },
	{ "cnz", cnz },
	{ "push", push },
	{ "adi", adi },
	{ "rst", rst },
	{ "rz", rz },
	{ "ret", ret },
	{ "jz", jz },
	{ "cz", cz },
	{ "call", call },
	{ "aci", aci },
	{ "rnc", rnc },
	{ "jnc", jnc },
	{ "out", out },
	{ "cnc", cnc },
	{ "sui", sui },
	{ "rc", rc },
	{ "jc", jc },
	{ "in", in },
	{ "cc", cc },
	{ "sbi", sbi },
	{ "rpo", rpo },
	{ "jpo", jpo },
	{ "xthl", xthl },
	{ "cpo", cpo },
	{ "ani", ani },
	{ "rpe", rpe },
	{ "pchl", pchl },
	{ "jpe", jpe },
	{ "xchg", xchg },
	{ "cpe", cpe },
	{ "xri", xri },
	{ "rp", rp },
	{ "jp", jp },
	{ "di", di },
	{ "cp", cp },
	{ "ori", ori },
	{ "rm", rm },
	{ "sphl", sphl },
	{ "jm", jm },
	{ "ei", ei },
	{ "cm", cm },
	{ "cpi", cpi },
	{ "org", org },
	{ "equ", equ },
	{ "db", db },
	{ "dw", dw },
	{ "ds", ds },
	{ "end", end }
};

static void
translate(void)
{
	int i;

	if (lab[0] != '\0') {
		fprintf(fq, "%s", lab);
		if (!!strcmp(op, "equ"))
			fputc(':', fq);
	}

	for (i = 0; i < sizeof(tab) / sizeof(tab[0]); i++) {
		if (!strcmp(op, tab[i].op80)) {
			fputc('\t', fq);
			tab[i].cb();
			break;
		}
	}

	if (comm[0] != '\0') {
		if (lab[0] != '\0' || op[0] != '\0')
			fputc('\t', fq);
		fprintf(fq, "%s", comm);
	}
	fputc('\n', fq);
}

static void
assemble(FILE *fp)
{

	/* DRI XLT86 User's Guide page 10 */
	fprintf(fq, "%%define\tM\tByte [bx]\n");
	fprintf(fq, "%%define\tm\tByte [bx]\n");

	while (!egetline(fp)) {
		lex();
		translate();
	}
}

int
main(int argc, char *argv[])
{
	FILE *fp;

	if (argc != 3) {
		fputs("usage: 8088ify infile.asm outfile.asm\n", stderr);
		exit(1);
	}

	if ((fp = fopen(argv[1], "r")) == NULL) {
		fputs("8088ify: can't open input file\n", stderr);
		exit(1);
	}

	if ((fq = fopen(argv[2], "w+")) == NULL) {
		fclose(fp);
		fputs("8088ify: can't open output file\n", stderr);
		exit(1);
	}

	assemble(fp);

	fclose(fq);
	fclose(fp);

	return 0;
}
