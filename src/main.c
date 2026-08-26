// Copyright (c) 2026 nepinhum. All rights reserved.
// Generated: 2026-08-26

#include <stdio.h>
#include <string.h>

static void usage(FILE *stream)
{
    fputs("usage: hexpeek <file>\n", stream);
}

int main(int argc, char **argv)
{
    if (argc == 2 &&
        (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)) {
        usage(stdout);
        return 0;
    }

    if (argc != 2) {
        usage(stderr);
        return 2;
    }

    return 0;
}
