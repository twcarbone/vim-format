function! One() range abort dict closure
    echo 1
    retu 2
endfunction

fu! Two(...)
    return 1
endf

function! Three(a)
    return
endfunction

function! Four(a,)
    echo 1
endfunction

function! Five(a, ...)
    echo 1
endfunction

function! Six(a, b = 1)
    echo 1
endfunction
