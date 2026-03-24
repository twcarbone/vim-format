let g:abort = [1, 2, 3]
let g:abort[0] = 3 + 2
let g:abort[1:2] = [6, 6]
let bb = 10
let bb += 2
let bb -= 2
let bb *= 2
let bb /= 2
let bb %= 2
let bb .= 2
let bb ..= 2
let cc = g:abort
let is = bb

" for

for fu in g:abort
    continue
    break
endfor

for [a, isnot] in [[4, 5], [6, 7]]
    echo isnot
    con
    brea
endfo

" while

while 0
    echo "string"
endwhile

wh 0
    ec "string"
endwh

" if

if 1
endif

if 1
elseif 2
endif

if 1
else
endif

if 1
elseif 2
else
endif

if 1
elsei 2
el
en

" expression-syntax

" expr1
echo 1 ? "this" : is
echo "us" ?? "them"

" expr2
echo 1 || 0

" expr3
echo 1 && 0

" expr4
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

" expr5
echo 1 << 1
echo 2 >> 1

" expr6
echo 1 + 2
echo 2 - 1
echo "sp" .. "am"
echo "sp" . "am"

" expr7
echo 2 * 1
echo 6 / 2
echo 6 % 4

" expr9
echo !1
echo -1
echo +1

" expr10
echo [0, 1, 2, 3][0]
echo [0, 1, 2, 3][:]
echo [0, 1, 2, 3][:2]
echo b:items[2:]
echo b:items[1:3]
echo b:items[1 + 1:6 / 2]
echo g:dict.closure
echo ReturnsFuncRef()("passed to returned FuncRef")
echo 2->Add(3)
echo 3->Add(4)->Add(5)

" expr11
let g:m_bar = 42
let b:in = [0, 1, 2, 3]
let dict_a = {"apple": 4, "banana": [1 * 2, 2], "cherry": "pie"}
let dict_b = {"test": g:m_bar}
let dict_c = {g:m_bar: 43}
echo list_of_funcrefs[4](5)
echo Add(1, 2)
echo (1 + (3 + 4)) * 2 - (10)
echo 'string with "embedded" double quote'
