" literal string
echo 'test'
echo '''test'
echo 'test'''
echo 'te''st'
echo 'te''''s''t'
echo ''''

" interpolated literal string
echo $'hello'' world'
echo $'hello ''{1 + 1}'
echo $'''{1 + 1}'' = 2'
echo $'{2 * 4} ''=='' {7 + 1}'
echo $'{{literal {1 + 1}''{{curly braces}}}}'

" string constant
echo "test"
echo "\"test"
echo "test\""
echo "te\"st"
echo "te\"\"s\"t"
echo "\""

" interpola\ed string constant
echo $"hello\" world"
echo $"hello \"{1 + 1}"
echo $"\"{1 + 1}\" = 2"
echo $"{2 * 4} \"==\" {7 + 1}"
echo $"\{literal {1 + 1}\"\{curly braces\}\}"
echo $"{{literal {1 + 1}\"{{curly braces}}}}"
