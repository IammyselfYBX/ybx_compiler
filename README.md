# ybx_compiler
### 终端词法语法分析器
---

##### Building and installing it
```
To install from the GIT repository:
  0. Clone the code from GIT:
     git clone https://github.com/IammyselfYBX/ybxcompiler.git

  1. cd into the source directory.

To install :

  2. Run "make". 

  3. If you want to clean binary file , Run "make clean".

  4. Run "make install", possibly as root if the destination permissions
     require that.

  5. See if it works.  

  6. IF you want to uninstall , Run "make uninstall"
```
#### 应用介绍
- 一、程序结构
&emsp;ybx程序支持<b>顶层结构<b>，即使在函数外面也可以书写执行语句，即不需要写
```c
int main(int argc, char *argv[])
{
    return 0;
}
```
而直接写下面的依旧可以运行
```c
printf("hello world\n");
```
- 二、变量
    > 1.实现布尔类型、整数类型、实数型、字符串类型、指针类型<br>
    > 2.变量使用的时候不需要声明就可以直接使用，在赋初值的时候就包含声明的过程，所以，如果直接引用一个还没有付初值的变量会报错<br>
    > 3.变量命名与C语言同， 字母开头+[数字/字母]*

- 三、运算符
    > - 取反<br>
    > \+ \- * / % 加 减 乘 除 取余<br>
    > \> \>= < <= == != 数值比较<br>
    > && || 逻辑与 逻辑或<br>
    > = 赋值

- 四、流程控制
    > 1.支持条件控制if、循环控制while、for<br>
    > 不允许悬空else，所以改用 elsif ，支持break、continue、return语句


##### 各文件介绍
- 一、模块
    主要是由3个模块组成
    > 主程序模块<br>
    > 通用模块 ———— 内存管理模块:memory MEM.h<br>
    > 通用模块 ———— 提醒报错模块:debug DBG.h<br>

- 二、主程序模块YBX
    > 1.YBX.h 定义解释器，并对解释器的行为进行定义<br>
    > 2.YBX_dev.h 定义ybx语言中的变量类型，以及如何调用C语言函数，如何注册到解释器的规则<br>
    > 3.ybx.h 解释器定义  YBX_Interpreter_tag  <br>
        &emsp;1)MEM_Storage         interpreter_storage;		// 解释器的存储器，在解释器生成的时候生成，解释器结束的时候释放，由 util.c 的 crb_malloc 分配<br>
        &emsp;2)MEM_Storage         execute_storage;			// 运行时的存储器<br>
        &emsp;3)Variable            *variable;					// 变量列表<br>
        &emsp;4)FunctionDefinition  *function_list;				// 函数列表<br>
        &emsp;5)StatementList       *statement_list;			// 语句列表（表达式也是一种语句）<br>
        &emsp;6)int                 current_line_number;		// 当前行号，当出现错误时用于指明出错位置的行号

    > 4.词法分析 ———— ybx.l<br>
    > 5.分析树的构建 ———— ybx.y、create.c<br>
        常量折叠<br>
    > 6.错误信息 ———— error_message.c<br>
    > 7.垃圾回收机制 ———— string_pool.c <br>
        目前采用 引用计数 这样的原始方式处理<br>

- 三、通用模块
&emsp;内存管理模块与提示报错模块分别在 memory 与 debug 文件夹里面，这不是本编译器重点，属于任何程序通用模块<br>
&emsp;其实也可以不用该通用模块的方案就是使用[valgrind](http://valgrind.org/)
```bash
valgrind ybx
```
