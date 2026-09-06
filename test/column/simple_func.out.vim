function! Foo(
    \long_parameter
\)
    return 1
endfunction

function! Foo()
\range
\dict
    return 1
endfunction

function! Foo(
    \a,
    \b = 1,
    \ ...
\) range dict
    return 1
endfunction

function! Foo(
    \a,
    \b = 1,
    \ ...
\)
\range
\dict
\abort
\closure
    return 1
endfunction
