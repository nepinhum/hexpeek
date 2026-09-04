// Created by nepinhum: 2026-08-26
// Licensed under the MIT License. See LICENSE for details.

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

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

static void print_row(unsigned long long offset,
                      const unsigned char bytes[ROW_SIZE],
                      size_t count)
{
    printf("%08llx  ", offset);

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
    unsigned long long offset = 0U;

    for (;;) {
        errno = 0;
        size_t count = fread(bytes, 1U, ROW_SIZE, file);
        int read_error = errno;

        if (count > 0U) {
            print_row(offset, bytes, count);
            if (offset > ULLONG_MAX - (unsigned long long)count) {
                report_error(path, "offset overflow", 0);
                return 1;
            }
            offset += (unsigned long long)count;
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

static int flush_output(void)
{
    errno = 0;
    if (fflush(stdout) != 0) {
        int write_error = errno;
        report_error("stdout", "write error", write_error);
        return 1;
    }
    if (ferror(stdout) != 0) {
        report_error("stdout", "write error", 0);
        return 1;
    }
    return 0;
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
    if (status == 0 && flush_output() != 0) {
        status = 1;
    }

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
