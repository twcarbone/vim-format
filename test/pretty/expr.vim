let   i   =   42
let   i=42
  let i 	=	42
let i = 42    

let g:foo = "baz"

echo    "hello"
echo   3+   3

let list = [1,2,3]
echo [ 1 ,  2,3   ]

if   1
echo "if"
  endif

if 2
 echo "if"
             else
  echo "else"
    endif

while  0    
echo "while"
  endwhile    

for  item	in  ["a","b"  ,  "c"]
    echo item
    endfor

echo list[ 0 ]
echo list[1:]
echo list[:1]

echo - 1+ 2

echo  1 ? "true": 	"false"

echo add ([1,2  ,3],--4+  5)

function!    Six (		a, b=1  )
" non-trailing comment
    let i = 1
return 1<<2  	" trailing comment
endfunction

echo "foo"    ==#   "Foo"
  echo "foo" ==? "Foo"

echo "hello"	->toupper(    )
