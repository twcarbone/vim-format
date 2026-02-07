for item in [1, 2, 3]
    echo "for each item"
    continue
endfor

let i = 1
let items = [4, 5, 6]

for item in items
    let i += 1
endfor

while 1
    let foo = 6
    break
endwhile
