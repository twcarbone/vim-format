# black -l 20

items = [
    "apple",
    "banana",
    "cherry",
    "vanilla",
    "chocolate",
]

# Items render on the same line as the enclosing tokens, or are all on separate lines.
items = [
    "a",
    "b",
    "c",
]

# Even though all items would appear before the column limit on their own line, they are
# still rendered on separate lines.
#
# items = [
#     "a", "b", "c"
# ]

long_name = long_long_name + another_long_name

# vim:cc=20
