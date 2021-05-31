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

FILE *fq;

static char line[256];

static char lab[256];
static char op[256];
static char a1[256];
static char a2[256];
static char comm[256];

static char bdos[256];

static int bang;
static int bdosfound;

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
	while (!endoftoken(line[i]))
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
eight(char *a)
{

	if (!strcmp(a, "A") || !strcmp(a, "a"))
		return "al";
	if (!strcmp(a, "B") || !strcmp(a, "b"))
		return "ch";
	if (!strcmp(a, "C") || !strcmp(a, "c"))
		return "cl";
	if (!strcmp(a, "D") || !strcmp(a, "d"))
		return "dh";
	if (!strcmp(a, "E") || !strcmp(a, "e"))
		return "hl";
	if (!strcmp(a, "H") || !strcmp(a, "h"))
		return "bh";
	if (!strcmp(a, "L") || !strcmp(a, "l"))
		return "bl";

	return a;
}

static char *
sixteen(char *a)
{

	if (!strcmp(a, "B") || !strcmp(a, "b"))
		return "cx";
	if (!strcmp(a, "D") || !strcmp(a, "d"))
		return "dx";
	if (!strcmp(a, "H") || !strcmp(a, "h"))
		return "bx";
	if (!strcmp(a, "PSW") || !strcmp(a, "psw"))
		return "ax";

	return a;
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
inr(void)
{

	fprintf(fq, "inc\t%s", eight(a1));
}

static void
dcr(void)
{

	fprintf(fq, "dcr\t%s", eight(a1));
}

static void
mvi(void)
{

	fprintf(fq, "mov\t%s, %s", eight(a1), a2);
}

static void
dcx(void)
{

	fprintf(fq, "dec\t%s", sixteen(a1));
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
ret(void)
{

	fprintf(fq, "ret");
}

/*
 * Checking special cases for call.
 *
 * Returns 1 if 0005h
 * Returns 2 if 0000h
 * Returns 0 otherwise
 */
static int
numcheck(void)
{
	int base, ret;

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
	}

	return 0;
}

static int
isbdos(void)
{
	int base;

	if (a1[0] == '\0')
		return 0;

	return numcheck();
}

static void
call(void)
{

	if (isbdos() == 1) {
		fprintf(fq, "push\tax\n");
		fprintf(fq, "\tmov\tah, cl\n");
		fprintf(fq, "\tint\t21h\n");
		fprintf(fq, "\tpop\tax");
	} else if (isbdos() == 2) {
		fprintf(fq, "mov\tah, 4ch\n");
		fprintf(fq, "\tint\t21h");
	} else {
		fprintf(fq, "call\t%s", a1);
	}
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
		if (numcheck()) {
			for (i = 0; i < sizeof(bdos); i++)
				bdos[i] = '\0';
			for (i = 0; i < strlen(lab); i++)
				bdos[i] = lab[i];
			bdosfound = 1;
		}
	}
}

static void
db(void)
{

	fprintf(fq, "db\t%s", a1);
}

static void
end(void)
{

	fprintf(fq, "end");
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
	{ "inr", inr },
	{ "dcr", dcr },
	{ "mvi", mvi },
	{ "dcx", dcx },
	{ "mov", mov },
	{ "hlt", hlt },
	{ "ret", ret },
	{ "call", call },
	{ "org", org },
	{ "equ", equ },
	{ "db", db },
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
	int eoa;

	while (1) {
		eoa = egetline(fp);
		lex();
		translate();

		if (eoa)
			break;
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
