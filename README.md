# hexpeek

`hexpeek` is a tiny hex viewer written in C.

## Build

```sh
make
```

## Usage

```sh
./hexpeek <file>
./hexpeek --help
```

Each line contains a byte offset, 16 hexadecimal bytes and printable ASCII.
Non printable bytes are shown as `.`.

## Test

```sh
make test
```

Excludes editing, search, stdin, paging, configurable
row widths, offsets/ranges, format parsing and `mmap` for now.
