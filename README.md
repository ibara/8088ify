8088ify
=======
`8088ify` is an Intel 8080 CP/M 2.2 to Intel 8086 (8088)
MS-DOS assembly program translator. This means that
`8088ify` reads in assembly language written for the Intel
8080 and outputs an equivalent assembly program for the
Intel 8086/8088.

`8088ify` was written for
[PCjam 2021](https://pcjam.gitlab.io/).

Why?
----
`8088ify` arose as a tongue-in-cheek reference to the jam's
purpose: celebrating the 40th anniversary of the IBM PC, aka
the IBM 5150. It is very clearly a retrocomputing event.
What better way to commemorate this retrocomputing milestone
than to create a program that aids in retrocomputing from
the 5150's own vantage point: modernizing code written for
its precedessor CPU.

It may be lesser-known that Intel had the porting of 8080
assmebly code to 8086/8088 in mind when designing the
8086/8088. According to
[this retrocomputing forum post](https://retrocomputingforum.com/t/translation-of-8080-code-to-8086/1309),
Intel even produced documentation of conversion tables
between the 8080 and the 8086/8088. Unfortunately, I was
unable to find that document. However, there was a
commercial tool written by Digital Research, Inc., XLT86,
that could translate from 8080 to 8086/8088 assembly. XLT86
was designed for translation from CP/M-80 to CP/M-86 and
related DRI operating systems. The XLT86 users manual, which
contains DRI's own 8080 to 8086/8088 conversion tables, is
[available](http://s100computers.com/Software%20Folder/Assembler%20Collection/Digital%20Research%20XLT86%20Manual.pdf),
and which I used for `8088ify`.

As I could not find any open source tools or any tools to
convert not only from 8080 to 8086/8088 but also CP/M to
MS-DOS, `8088ify` was born.

Building
--------
Run your C compiler on `8088ify.c`. It is a single-file C
utility and written in ANSI C. As `8088ify` was written on
[OpenBSD](https://www.openbsd.org/), I can verify that it
works equally as well on Unix as MS-DOS.

`8088ify` should compile with any ANSI C compiler.

Running
-------
`usage: 8088ify infile.asm outfile.asm`

So long as your system is able to open the input and output
files, `8088ify` will not fail. That is to say, it is only
a mechanical translator. `8088ify` does not perform any
semantic or syntactic analysis; it assumes the input
assembly is valid. The user should review the output before
attempting assembly.

Caveats
-------
`8088ify` assumes no single line of input assembly code will
exceed 255 characters. It will truncate lines longer than
255 characters, but still output assembly for what it did
read in before truncation.

Comments are carried over to the output assembly. They may
not make sense for an 8086/8088 CPU.

License
-------
ISC License. See `LICENSE` for details.
