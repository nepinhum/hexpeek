# Created by nepinhum: 2026-08-26
# Licensed under the MIT License. See LICENSE for details.

CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -O2
CPPFLAGS ?=
LDFLAGS ?=

.PHONY: all test clean

all: hexpeek

hexpeek: src/main.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $< $(LDFLAGS) -o $@

test: hexpeek
	sh tests/test.sh

clean:
	rm -f hexpeek
