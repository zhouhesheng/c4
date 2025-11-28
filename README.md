# c4 - C in four functions

An exercise in minimalism.
Try the following:
    gcc -o c4 c4.c
    ./c4 hello.c
    ./c4 -s hello.c
    
    ./c4 c4.c hello.c
    ./c4 c4.c c4.c hello.c

# c5 - C in four functions with switch-case and struct support
    gcc -o c5 c5.c
    ./c5 hellos.c
    ./c5 -s hellos.c

## macOS
修改 main 函数， 用 gcc 编译运行：
signed main(signed argc, char **argv)

如果用c4编译，需要修改回来


# XC step by step

## XC1 设计
next() 用于词法分析，获取下一个标记，它将自动忽略空白字符。
program() 语法分析的入口，分析整个 C 语言程序。
expression(level) 用于解析一个表达式。
eval() 虚拟机的入口，用于解释目标代码。

## XC2 虚拟机
### 内存
代码段（text）用于存放代码（指令）。
数据段（data）用于存放初始化了的数据，如int i = 10;，就需要存放到数据段中。
未初始化数据段（bss）用于存放未初始化的数据，如 int i[1000];，因为不关心其中的真正数值，所以单独存放可以节省空间，减少程序的体积。
栈（stack）用于处理函数调用相关的数据，如调用帧（calling frame）或是函数的局部变量等。
堆（heap）用于为程序动态分配内存。

### 寄存器
PC 程序计数器，它存放的是一个内存地址，该地址中存放着 下一条 要执行的计算机指令。
SP 指针寄存器，永远指向当前的栈顶。注意的是由于栈是位于高地址并向低地址增长的，所以入栈时 SP 的值减小。
BP 基址指针。也是用于指向栈的某些位置，在调用函数时会使用到它。
AX 通用寄存器，我们的虚拟机中，它用于存放一条指令执行后的结果。

### 指令集

将 MOV 指令拆分成 5 个指令，这些指令只接受一个参数：
IMM <num> 将 <num> 放入寄存器 ax 中。
LC 将对应地址中的字符载入 ax 中，要求 ax 中存放地址。
LI 将对应地址中的整数载入 ax 中，要求 ax 中存放地址。
SC 将 ax 中的数据作为字符存放入地址中，要求栈顶存放地址。
SI 将 ax 中的数据作为整数存放入地址中，要求栈顶存放地址。

子函数调用
命令有 CALL, ENT, ADJ 及 LEV。
ENT <size> 指的是 enter，用于实现 ‘make new call frame’ 的功能，即保存当前的栈指针，同时在栈上保留一定的空间，用以存放局部变量。
ADJ <size> 用于实现 ‘remove arguments from frame’。在将调用子函数时压入栈中的数据清除，本质上是因为我们的 ADD 指令功能有限。
LEV ‘restore call frame and PC’ 是ENT的反操作，包括了POP和RET功能

CALL <addr>:跳转到addr处,调用函数
ENT <size>:保存当前栈底指针并为函数调用栈安排空间
ADJ 销毁函数调用栈中的数据
LEA <offset>:用于获取函数调用中传入的参数
LEV 做销毁函数栈的扫尾工作,恢复栈底指针等,对应汇编中ret指令
PUSH:将ax的值压入栈中

JMP <addr>:无条件跳转指令,将当前pc置为addr
JZ:判断ax是否为0,如果为0,则跳转
JNZ:判断ax是否为0,如果不为0,则跳转

## XC3 词法分析器
词法分析器以源码字符串为输入，输出为标记流（token stream），即一连串的标记，每个标记通常包括： (token, token value) 即标记本身和标记的值。例如，源码中若包含一个数字 '998' ，词法分析器将输出 (Number, 998)，即（数字，998）。

## XC4 递归下降

终结符与非终结符

首先是 BNF 范式，就是一种用来描述语法的语言，例如，四则运算的规则可以表示如下：
<expr> ::= <expr> + <term>
         | <expr> - <term>
         | <term>

<term> ::= <term> * <factor>
         | <term> / <factor>
         | <factor>

<factor> ::= ( <expr> )
           | Num

用尖括号 <> 括起来的就称作 非终结符 ，因为它们可以用 ::= 右侧的式子代替。| 表示选择，如 <expr> 可以是 <expr> + <term>、<expr> - <term>或 <term> 中的一种。而没有出现在::=左边的就称作 终结符 ，一般终结符对应于词法分析器输出的标记。
四则运算的递归下降
例如，我们对 3 * (4 + 2) 进行语法分析。我们假设词法分析器已经正确地将其中的数字识别成了标记 Num。

递归下降是从起始的非终结符开始（顶），本例中是 <expr>，实际中可以自己指定，不指定的话一般认为是第一个出现的非终结符。

1. <expr> => <expr>
2.           => <term>        * <factor>
3.              => <factor>     |
4.                 => Num (3)   |
5.                              => ( <expr> )
6.                                   => <expr>           + <term>
7.                                      => <term>          |
8.                                         => <factor>     |
9.                                            => Num (4)   |
10.                                                        => <factor>
11.                                                           => Num (2)

可以看到，整个解析的过程是在不断对非终结符进行替换（向下），直到遇见了终结符（底）。而我们可以从解析的过程中看出，一些非终结符如<expr>被递归地使用了。

为什么选择递归下降
从上小节对四则运算的递归下降解析可以看出，整个解析的过程和语法的 BNF 表示是十分接近的，更为重要的是，我们可以很容易地直接将 BNF 表示转换成实际的代码。方法是为每个产生式（即 非终结符 ::= ...）生成一个同名的函数。

这里会有一个疑问，就是上例中，当一个终结符有多个选择时，如何确定具体选择哪一个？如为什么用 <expr> ::= <term> * <factor> 而不是 <expr> ::= <term> / <factor> ？这就用到了上一章中提到的“向前看 k 个标记”的概念了。我们向前看一个标记，发现是 *，而这个标记足够让我们确定用哪个表达式了。

另外，递归下下降方法对 BNF 方法本身有一定的要求，否则会有一些问题，如经典的“左递归”问题。

左递归
原则上我们是不讲这么深入，但我们上面的四则运算的文法就是左递归的，而左递归的语法是没法直接使用递归下降的方法实现的。因此我们要消除左递归，消除后的文法如下：

<expr> ::= <term> <expr_tail>
<expr_tail> ::= + <term> <expr_tail>
              | - <term> <expr_tail>
              | <empty>

<term> ::= <factor> <term_tail>
<term_tail> ::= * <factor> <term_tail>
              | / <factor> <term_tail>
              | <empty>

<factor> ::= ( <expr> )
           | Num

消除左递归的相关方法，这里不再多说，请自行查阅相关的资料。

## XC5 变量定义 函数定义 语句 表达式

EBNF 是对前一章提到的 BNF 的扩展，它的语法更容易理解，实现起来也更直观。但真正看起来还是很烦，如果不想看可以跳过。

program ::= {global_declaration}+

global_declaration ::= enum_decl | variable_decl | function_decl

enum_decl ::= 'enum' [id] '{' id ['=' 'num'] {',' id ['=' 'num'] '}'

variable_decl ::= type {'*'} id { ',' {'*'} id } ';'

function_decl ::= type {'*'} id '(' parameter_decl ')' '{' body_decl '}'

parameter_decl ::= type {'*'} id {',' type {'*'} id}

body_decl ::= {variable_decl}, {statement}

statement ::= non_empty_statement | empty_statement

non_empty_statement ::= if_statement | while_statement | '{' statement '}'
                     | 'return' expression | expression ';'

if_statement ::= 'if' '(' expression ')' statement ['else' non_empty_statement]

while_statement ::= 'while' '(' expression ')' non_empty_statement


# tinyc https://github.com/pandolia/tinyc