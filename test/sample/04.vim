echo 1 && 0
echo 1 || 0
echo 5 ?? 0
echo 1 ? "this" : "that"

echo 1 == 2
echo 1 ==? 2
echo 1 ==# 2
echo 1 != 2
echo 6 >  4
echo 1 >= 5
echo 5 <  3
echo 3 <= 2
echo "foo" is "bar"
echo "foo" isnot "bar"

echo 1 << 1
echo 2 >> 1

echo 1 + 2
echo 2 - 1
echo "sp" .. "am"
echo "sp" . "am"

echo 2 * 1
echo 6 / 2
echo 6 % 4

echo !1
echo -1
echo +1

let i = 42
let i += 42
let i -= 42
let i *= 42
let i /= 42
let i %= 42
let j = "sp"
let j .= "am"
let j ..= "am"

if 1
    echo 1
endif

if 2
    echo 1
else
    echo 2
endif

if 3
    echo 1
elseif 2
    echo 3
endif

if 0
    echo "here"
elseif 2
    echo "there"
elseif 3
    echo "and here"
else
    echo "everywhere"
endif

echo [1, 2, 3,]
let items = [1, 2, 3]

for item in [1, 2, 3]
    echo "for each item"
    continue
endfor

for item in items
    let i += 1
endfor

while 1
    let foo = 6
    break
endwhile

function! Foo() range abort dict closure
    echo 1
endfunction

function! Foo(...)
    return 1
endfunction

function! Foo(a)
    return
endfunction

function! Foo(a,)
    echo 1
endfunction

function! Foo(a, ...)
    echo 1
endfunction

function! Foo(a, b = 1)
    echo 1
endfunction

echo Foo(Bar() + 1, baz[4]) / 2
echo fns[4]([2, 3])
