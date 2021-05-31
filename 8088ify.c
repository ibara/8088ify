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

#include <stdio.h>
#include <stdlib.h>

/*
 * 8088ify -- Intel 8080 CP/M to x86 (8088) MS-DOS assembly translator
 * Written for PCjam 2021: https://pcjam.gitlab.io/
 */

static int lineno;
static int pass;
static int errors;
static unsigned short addr;

static char *lab;
static char *op;
static char *a1;
static char *a2;

static char line[256];

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
assemble(FILE *fp, FILE *fq)
{
	int eoa;

	eoa = egetline(fp);

	// XXX
	printf("%s\n", line);

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
