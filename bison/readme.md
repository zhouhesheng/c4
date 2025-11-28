# calc

bison -vdty calc.y
flex calc.l
gcc -std=c89 -Wno-implicit-function-declaration y.tab.c lex.yy.c
./a.out
(1+2)*3

%left 表明这些符号是左结合的。同一行的符号优先级相同，下面行的符号的优先级高于上面的

# scanner

bison -vdty parser.y
flex scanner.l
gcc -std=c89 -Wno-implicit-function-declaration y.tab.c lex.yy.c 
./a.out < test.c 