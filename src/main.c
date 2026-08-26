// Created by nepinhum: 2026-08-26
// Licensed under the MIT License. See LICENSE for details.

#include <stdio.h>
#include <string.h>
#include <errno.h>

#define ROW_SIZE 16U

static void usage(FILE *stream)
{
    fputs("usage: hexpeek <file>\n", stream);
}

static void report_error(const char *path,
                         const char *fallback,
                         int error_number)
{
    const char *detail = error_number != 0
        ? strerror(error_number)
        : fallback;

    fprintf(stderr, "hexpeek: %s: %s\n", path, detail);
}

static void print_row(size_t offset,
                      const unsigned char bytes[ROW_SIZE],
                      size_t count)
{
    printf("%08zx  ", offset);

    for (size_t i = 0U; i < ROW_SIZE; ++i) {
        if (i < count) {
            printf("%02x ", (unsigned int)bytes[i]);
        } else {
            fputs("   ", stdout);
        }
        if (i == 7U) {
            putchar(' ');
        }
    }

    fputs(" |", stdout);
    for (size_t i = 0U; i < ROW_SIZE; ++i) {
        if (i < count && bytes[i] >= 0x20U && bytes[i] <= 0x7eU) {
            putchar((int)bytes[i]);
        } else if (i < count) {
            putchar('.');
        } else {
            putchar(' ');
        }
    }
    puts("|");
}

static int dump(FILE *file, const char *path)
{
    unsigned char bytes[ROW_SIZE];
    size_t offset = 0U;

    for (;;) {
        errno = 0;
        size_t count = fread(bytes, 1U, ROW_SIZE, file);
        int read_error = errno;

        if (count > 0U) {
            print_row(offset, bytes, count);
            offset += count;
        }
        if (count == ROW_SIZE) {
            continue;
        }
        if (ferror(file) != 0) {
            report_error(path, "read error", read_error);
            return 1;
        }
        return 0;
    }
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

    errno = 0;
    FILE *file = fopen(argv[1], "rb");
    if (file == NULL) {
        int open_error = errno;
        report_error(argv[1], "open error", open_error);
        return 1;
    }

    int status = dump(file, argv[1]);
    errno = 0;
    if (fclose(file) != 0) {
        int close_error = errno;
        if (status == 0) {
            report_error(argv[1], "close error", close_error);
            status = 1;
        }
    }

    return status;
}
