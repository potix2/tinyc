#!/bin/bash

run_embedded_tests() {
    ./tinyc -test
    actual="$?"
    if [ "$actual" != "0" ]; then
        echo "failed to embedded tests"
        exit 1
    fi
}

try() {
    expected="$1"
    input="$2"

    ./tinyc "$input" > tmp.s
    gcc -o tmp tmp.s
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}

try_funcall() {
    expected="$1"
    input="$2"

    ./tinyc "$input" > tmp.s
    gcc -o tmp tmp.s tests/foo.c
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}


# 組み込みテストを実行
run_embedded_tests

# try 0 0
# try 42 42
try 1 "return 1;"
try 21 "5+20-4;"
try 41 " 12 + 34 - 5;"
try 47 "5+6*7;"
try 15 "5*(9-6);"
try 4 "(3+5)/2;"
try 10 "-10 + 20;"
try 7 "2+3-(-2);"
try 1 "12 == 3 * 4;"
try 0 "12 != 3 * 4;"
try 0 "4 < 2;"
try 1 "4 > 2;"
try 1 "4 >= 4;"
try 1 "5 >= 4;"
try 1 "4 <= 4;"
try 0 "5 <= 4;"
try 3 " a = 3;"
try 5 " a = 3; return a + 2;"
try 18 "a = 3; b = 5 * 6; a + b / 2;"
try 6 " a = b = 3; a+ b;"
try 5 " return 5; return 8;"
try 14 "foo = 3; bar = 5 * 6 - 8; return foo + bar / 2;"
try 2 "a = 1; if(a == 1) return 2; else return 3;"
try 3 "a = 0; if(a) return 2; else return 3;"
try 100 "if(0) return 3; return 100;"
try 10 "i = 0; while(i < 10) i = i + 1; return i;"
try 32 "a = 1; for(b = 0; b < 5; b = b + 1) a = a * 2; return a;"
try 45 "i = 0; x = 0; for(; i < 10; i = i + 1) x = x + i; return x;"
try 100 "i = 0; for(;;) if ( i < 10 ) i = i + 1; else return 100;"
try 20 "a = 1; b = 1; if ( 1 ) { a = 10; b = 20; } else { a = 0; b = 0; } return b;"
try_funcall 0 "foo();"
echo OK
