#!/bin/sh
# Created by nepinhum: 2026-08-26
# Licensed under the MIT License. See LICENSE for details.

set -eu

root=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT HUP INT TERM

fail() {
    printf 'FAIL: %s\n' "$1" >&2
    exit 1
}

"$root/hexpeek" --help >"$tmp/help.out" 2>"$tmp/help.err" ||
    fail "--help should succeed"
grep -Fq 'usage: hexpeek <file>' "$tmp/help.out" ||
    fail "help should contain usage"
test ! -s "$tmp/help.err" || fail "help should not write stderr"

set +e
"$root/hexpeek" >"$tmp/noarg.out" 2>"$tmp/noarg.err"
status=$?
set -e
test "$status" -eq 2 ||
    fail "missing operand should exit with status 2"
grep -Fq 'usage: hexpeek <file>' "$tmp/noarg.err" ||
    fail "missing operand should print usage to stderr"

set +e
"$root/hexpeek" one two >"$tmp/many.out" 2>"$tmp/many.err"
status=$?
set -e
test "$status" -eq 2 ||
    fail "extra operands should exit with status 2"
grep -Fq 'usage: hexpeek <file>' "$tmp/many.err" ||
    fail "extra operands should print usage to stderr"

printf '\177ELF\002\001\001\000\000\000\000\000\000\000\000\000ABC\001' >"$tmp/sample.bin"

cat >"$tmp/expected.txt" <<'EOF'
00000000  7f 45 4c 46 02 01 01 00  00 00 00 00 00 00 00 00  |.ELF............|
00000010  41 42 43 01                                       |ABC.            |
EOF

"$root/hexpeek" "$tmp/sample.bin" >"$tmp/actual.txt" 2>"$tmp/dump.err" ||
    fail "dumping a readable file should succeed"
cmp -s "$tmp/expected.txt" "$tmp/actual.txt" ||
    fail "hex dump output should match exactly"
test ! -s "$tmp/dump.err" || fail "successful dump should not write stderr"

if test -w /dev/full; then
    set +e
    "$root/hexpeek" "$tmp/sample.bin" >/dev/full 2>"$tmp/full.err"
    status=$?
    set -e
    test "$status" -eq 1 ||
        fail "stdout write failure should exit with status 1"
    grep -Fq 'hexpeek: stdout:' "$tmp/full.err" ||
        fail "stdout write failure should be reported"
fi

: >"$tmp/empty.bin"
"$root/hexpeek" "$tmp/empty.bin" >"$tmp/empty.out" 2>"$tmp/empty.err" ||
    fail "empty file should succeed"
test ! -s "$tmp/empty.out" || fail "empty file should produce no rows"
test ! -s "$tmp/empty.err" || fail "empty file should not write stderr"

missing="$tmp/does-not-exist.bin"
set +e
"$root/hexpeek" "$missing" >"$tmp/missing.out" 2>"$tmp/missing.err"
status=$?
set -e
test "$status" -eq 1 ||
    fail "missing file should exit with status 1"
test ! -s "$tmp/missing.out" || fail "missing file should not write stdout"
grep -Fq "hexpeek: $missing:" "$tmp/missing.err" ||
    fail "missing-file diagnostic should name the program and path"

printf 'ok: all tests passed\n'
