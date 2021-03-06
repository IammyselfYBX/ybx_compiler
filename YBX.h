/*
* Copyright (c) 2019,YBX is the most handsome man in NCEPU
* All rights reserved.
*
* 文件名称: YBX.h
* 文件标识: 见README.md
* 摘要: 定义解释器，并对解释器的行为进行定义
*      可以支持C语言调用
*      面向普通用户的头文件
*
*   对外可见
*
* 当前版本: 1.1
* 作者: 杨秉学
* 完成日期: 2019年11月17日
*
* 取代版本:1.0
* 原作者: 杨秉学
* 完成日期: 2018年5月2日
*/

#ifndef PUBLIC_YBX_H_INCLUDED
#define PUBLIC_YBX_H_INCLUDED
#include <stdio.h>

typedef struct YBX_Interpreter_tag YBX_Interpreter;					//指向解释器的指针，公有头文件仅定义标识符，具体实现在ybxcompiler.h，目的就是为了让 ybxcompiler.h 私有化
//YBX_Interpreter本身无法声明变量，只能传递指针(指针都是4字节)，不能使用 sizeof() ，不能引用内部成员

YBX_Interpreter *YBX_create_interpreter(void);						//生成解释器，因为实现不可见，只能传递指针
void YBX_compile(YBX_Interpreter *interpreter, FILE *fp);			//生成分析树
void YBX_interpret(YBX_Interpreter *interpreter);					//运行
void YBX_dispose_interpreter(YBX_Interpreter *interpreter);		    //运行完成后回收解释器

#endif /* PUBLIC_YBX_H_INCLUDED */
