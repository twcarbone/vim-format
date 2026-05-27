lockvar 2 foo
lockv! bar baz

let List = [1, 2, 3]
let Dict = {"one": 1, "two": 2}
function Add(a, b)
    return a:a + a:b
endfunction

lockvar 1 List[Dict.one:Add(1, 1)] w: g: Dict["on" .. "e"] bar baz

unlockvar! bar baz
unlo 2 foo
