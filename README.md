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
I could not find an open source translator between Intel
8080 and Intel 8086.

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

Building
--------
Run your C compiler on `8088ify.c`. It is a single-file C
utility and written in ANSI C. As `8088ify` was written on
[OpenBSD](https://www.openbsd.org/), I can verify that it
works equally as well on Unix as MS-DOS.

`8088ify` should compile with any ANSI C compiler that
includes a `strtol()` function. I may remedy this in the
future with a built-in `strtol()` function, but as Open
Watcom v2 has the function, I have not (yet) found a need.

`8088ify` can be compiled as a standalone application for
Unix, natively compiled on MS-DOS using Open Watcom v2,
cross compiled on Unix for MS-DOS using the Amsterdam
Compiler Kit, or cross compiled on Unix for CP/M using the
Amsterdam Compiler Kit.

When compiling for Unix, the following compiler invocation
is recomended:
```
$ cc -O2 -pipe -o 8088ify 8088ify.c
```

When compiling for MS-DOS with Open Watcom v2, the following
compiler invocation is recommended:
```
> wcl -0 -os -mt 8088ify.c
```

When compiling for MS-DOS with the Amsterdam Compiler Kit,
the following compiler invocation is recommended:
```
$ ack -mmsdos -O2 -o 8088ify.com 8088ify.c
```

When compiling for CP/M with the Amsterdam Compiler Kit, the
following compiler invocation is recommended:
```
$ ack -mcpm -O2 -o 8088ify.com 8088ify.c
```

The included `Makefile` is for Unix, sorry.

Running
-------
`usage: 8088ify infile.asm outfile.asm`

So long as your system is able to open the input and output
files, `8088ify` will not fail. That is to say, it is only
a mechanical translator. `8088ify` does not perform any
semantic or syntactic analysis; it assumes the input
assembly is valid. The user should review the output before
attempting assembly.

Creating binaries
-----------------
`8088ify` targets
[nasm](https://nasm.us/).
It has been a long time since nasm built 16-bit DOS
binaries. In this repository you will find binaries of nasm
0.98.31, as found on
[Sourceforge](https://sourceforge.net/projects/nasm/files/DOS%2016-bit%20binaries%20%28OBSOLETE%29/),
which do work on an 8086 (tested via DOSBox-X).

To create binaries, the following nasm command can be used:
```
nasm -f bin -o prog.com prog.asm
```
Where `prog.asm` is the name of your assembly program output
from `8088ify` and `prog.com` is the name you want for your
final binary. This also means that all programs translated
by `8088ify` target the tiny memory model only. This could
be improved in the future.

NOTE: This version of nasm is licensed under the LGPLv2.1+.
You can find a copy of the LGPLv2.1 license
[here](https://www.gnu.org/licenses/old-licenses/lgpl-2.1.en.html).
This license does not affect the license of `8088ify`.

Caveats
-------
`8088ify` assumes no single line of input assembly code will
exceed 255 characters. It will truncate lines longer than
255 characters, but still output assembly for what it did
read in before truncation.

Comments are carried over to the output assembly. They may
not make sense for an 8086/8088 CPU.

An attempt is made to detect calls to the CP/M BDOS:
`call 0005h`. The first `equ` statement to assign the value
5 to a label will be assumed to be the BDOS label and used
for all `call` checks.

Calls to `0000h` are also special-cased and will result in
an MS-DOS termination call.

Line separation with `!` is detected but not properly used.
Split those lines before running `8088ify`.

No macro facilities. Preprocess your assembly before running
it through `8088ify`.

Bugs
----
Lots. Let me know about them via Issues (even better: Pull
Requests with diffs).

License
-------
ISC License. See `LICENSE` for details.
