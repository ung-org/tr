/*
 * UNG's Not GNU
 *
 * Copyright (c) 2011-2019, Jakob Kaivo <jkk@ung.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#define _POSIX_C_SOURCE 2
#include <errno.h>
#include <limits.h>
#include <locale.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

union trc {
	unsigned char c;
	struct {
		unsigned int delete	:1;
		unsigned int squeeze	:1;
	} s;
};

enum {
	COMPLEMENT_COLLATE	= 1 << 0,
	COMPLEMENT_BINARY	= 1 << 1,
	DELETE			= 1 << 2,
	SQUEEZE			= 1 << 3,
};

static void init(union trc tr[], const char *s1, const char *s2, int flags)
{
	size_t l1 = strlen(s1);
	size_t l2 = s2 ? strlen(s2) : 0;

	for (size_t i = 0; i < UCHAR_MAX; i++) {
		if (flags & (DELETE | SQUEEZE)) {
			tr[i].s.delete = 0;
			tr[i].s.squeeze = 0;
		} else {
			tr[i].c = i;
		}
	}

	for (size_t i = 0; i < l1; i++) {
		int c = s1[i];

		if (flags & DELETE) {
			tr[c].s.delete = 1;
		} else if (flags & SQUEEZE) {
			tr[c].s.squeeze = 1;
		} else {
			tr[c].c = s2[i];
		}
	}

	if ((flags & DELETE) && (flags & SQUEEZE)) {
		for (size_t i = 0; i < l2; i++) {
			int c = s2[i];
			tr[c].s.squeeze = 1;
		}
	}
}

int main(int argc, char *argv[])
{
	setlocale(LC_ALL, "");
	int flags = 0;

	int c;
	while ((c = getopt(argc, argv, "cCds")) != -1) {
		switch (c) {
		case 'c':
			flags &= ~COMPLEMENT_COLLATE;
			flags |= COMPLEMENT_BINARY;
			break;

		case 'C':
			flags &= ~COMPLEMENT_BINARY;
			flags |= COMPLEMENT_COLLATE;
			break;

		case 'd':
			flags |= DELETE;
			break;

		case 's':
			flags |= SQUEEZE;
			break;

		default:
			return 1;
		}
	}

	if (argc <= optind) {
		fprintf(stderr, "tr: missing operand\n");
		return 1;
	}

	union trc tr[UCHAR_MAX];
	init(tr, argv[optind], argv[optind + 1], flags);

	while ((c = getchar()) != EOF) {
		if (flags & DELETE) {
			if (!tr[c].s.delete) {
				putchar(c);
			}
		} else {
			if (flags & SQUEEZE) {
			}
			putchar(tr[c].c);
		}
	}

	return 0;
}
