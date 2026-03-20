function! One() range abort dict
    echo 1
    function! Inner() closure
    endfunction
    retu 2
endfunction

fu! Two(...)
    return 1
endf

function! Three(isnot)
    return
endfunction

function! Four(a,)
    echo 1
endfunction

function! Five(a, ...)
    echo 1
endfunction

let is = 5

function! Six(a, b = is)
    echo 1
endfunction
