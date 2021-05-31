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

/*
 * 8088ify -- Intel 8080 CP/M to 8086 (8088) MS-DOS assembly translator
 * Written for PCjam 2021: https://pcjam.gitlab.io/
 */

static char line[256];

static char lab[256];
static char op[256];
static char a1[256];
static char a2[256];
static char comm[256];

static int bang;

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
		return 0;

	return 1;
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
		op[j++] = line[i++];

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
	if (line[i] == '\'') {
		++i;
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
		++i;
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

static int
assemble(FILE *fp, FILE *fq)
{
	int eoa;

	eoa = egetline(fp);
	lex();

	// XXX
	printf("%s\t%s\t%s\t%s\t%s\n", lab, op, a1, a2, comm);

	return eoa;
}

int
main(int argc, char *argv[])
{
	FILE *fp, *fq;

	if (argc != 3) {
		fputs("usage: 8088 infile.asm outfile.asm\n", stderr);
		exit(1);
	}

	if ((fp = fopen(argv[1], "r")) == NULL) {
		fputs("8088: can't open input file\n", stderr);
		exit(1);
	}

	if ((fq = fopen(argv[2], "w+")) == NULL) {
		fclose(fp);
		fputs("8088: can't open output file\n", stderr);
		exit(1);
	}

	while (assemble(fp, fq))
		;

	fclose(fq);
	fclose(fp);

	return 0;
}
