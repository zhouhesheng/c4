# c4 - C in four functions

An exercise in minimalism.

Try the following:

    gcc -o c4 c4.c
    ./c4 hello.c
    ./c4 -s hello.c
    
    ./c4 c4.c hello.c
    ./c4 c4.c c4.c hello.c

## macOS
修改 main 函数， 用 gcc 编译运行：
signed main(signed argc, char **argv)

如果用c4编译，需要修改回来

# c5 中文解释

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
