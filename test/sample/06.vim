" ==============================================================================
" fn_stmt
"

echo ("\nfn_stmt\n\n")

" function Sub(a,,)        " E125: Illegal argument: ,)
" function Sub(,a)         " E125: Illegal argument: ,a)
" function Sub(,)          " E125: Illegal argument: ,)
" function Sub(...,)       " E475: Invalid argument: ...,)
" function Sub(a b)        " E475: Invalid argument: a b)
" function Sub(a = 1, b)   " E989: Non-default argument follows default argument
function Sub()
endfunction

" ==============================================================================
" call_expr
"

echo ("\ncall_expr\n\n")

function Add(a, b)
    return a:a + a:b
endfunction

function Call(funcref)
    return a:funcref
endfunction

" call_expr
"   : IDENTIFIER ...
"   | entry_expr ...
"   | slice_expr ...
"   | call_expr ...
"   | lambda_expr ...

" echo Add(1,2,,)          " E116: Invalid arguments for function Add(1, 2,,)
" echo Add(1 2)            " E116: Invalid arguments for function Add(1 2)
" echo Add(1,2,3)          " E118: Too many arguments for function: Add
" echo Add(1)              " E119: Not enough arguments for function: Add
echo Add(1,2)
echo Add(1,2,)

let Fn = function("Add")
echo Fn(3, 4)

let FnPartial = function("Add", [100])
echo FnPartial(1)

let myDict = {'a': function("Add", [5, 6])}
let myDict = {'b': function("Add", [110])}
echo myDict.a()

let myList = [function("Add", [7, 8]), function("Add", [9, 10])]
echo myList[0]()
" echo myList[:]()

echo Call(Fn)(11, 12)
echo Call(Call(Fn))(13, 14)

echo Fn->{x -> x(15, 16)}()
echo FnPartial->{x -> x(2)}()

" ==============================================================================
" lambda_expr
"
"   :h lambda
"

echo ("\nlambda_expr\n\n")

" echo 3.14->{..., -> 100}()    " E15: Invalid expression: "3.14->{..., -> 100}()"
" echo 3.14->{105}()            " E15: Invalid expression: "3.14->{105}()"
" echo 3.14->{x -> x * 2}       " E107: Missing parentheses: lambda
" echo {105}()                  " E117: Unknown function: 105
" echo 3.14->{x, y -> 100}()    " E119: Not enough arguments for function: <lambda>11
" echo 3.14->{x -> a:x * 2}()   " E121: Undefined variable: a:x
" echo 3.14->{x = 10 -> 100}()  " E274: No white space allowed before parenthesis
echo 3.14->{x -> x * 2}()
echo 3.14->{x -> 101}()
echo 3.14->{x, -> 102}()
echo 3.14->{x, ... -> 103}()
echo 3.14->{... -> 104}()
echo 3.14->{-> 105}()
echo {-> 106}()
echo ['quick', 'brown', 'fox'][1]->tr('own', 'ick')

let l = [{x -> 1000 + x}]
echo 500->l[0]()
