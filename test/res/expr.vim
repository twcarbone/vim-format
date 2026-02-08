echo 1 ? "this" : "that"
echo "us" ?? "them"

echo 1 || 0

echo 1 && 0

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

let items = [0, 1, 2, 3]
echo [0, 1, 2, 3][0]
echo [0, 1, 2, 3][:]
echo [0, 1, 2, 3][:2]
echo items[2:]
echo items[1:3]
echo items[1 + 1:6 / 2]

function! Add(a, b)
endfunction

echo ReturnsFuncRef()("passed to returned FuncRef")
echo list_of_funcrefs[4](5)
echo Add(1, 2)
echo 2->Add(3)
echo 3->Add(4)->Add(5)
