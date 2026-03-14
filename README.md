> [!WARNING]
> This project is a work in progress. Only basic syntax is supported, for now.

# vim-format
A code formatter for legacy vimscript.

![CI](https://github.com/twcarbone/vim-format/actions/workflows/ci.yml/badge.svg?branch=master)

### Input
```vimscript
function    Example(  a  , b=  [2,3 , 5])
if 2
 echo "if"
    endif
return (  (1+1  ) / 2 ) <<1
endfunction
```
### Output
```vimscript
function Example(a, b = [2, 3, 5])
    if 2
        echo "if"
    endif
    return ((1 + 1) / 2) << 1
endfunction
```

# Quick start
```
git clone git@github.com:twcarbone/vim-format.git
cd vim-format
make build
./build/src/vim-format -h
```

# Architecture
1. Hand-written lexer/tokenizer
2. Hybrid recursive descent and top-down operator precedence (ie, Pratt) parser
3. AST based pretty-printing
