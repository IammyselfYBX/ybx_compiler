/*
* Copyright (c) 2019,YBX is the most handsome man in NCEPU
* All rights reserved.
*
* 文件名称: YBX_dev.h
* 文件标识: 见README.md
* 摘要: 定义ybx语言中的变量类型，以及如何调用C语言函数，如何注册到解释器的规则
*      面向内置函数开发人员的接口
*
* 当前版本: 1.1
* 作者: 杨秉学
* 完成日期: 2019年11月17日
*
* 取代版本:1.0
* 原作者: 杨秉学
* 完成日期: 2018年5月2日
*/

#ifndef PUBLIC_YBX_DEV_H_INCLUDED
#define PUBLIC_YBX_DEV_H_INCLUDED
#include "YBX.h"

// 布尔型
typedef enum {
    YBX_FALSE = 0,
    YBX_TRUE = 1
} YBX_Boolean;

// 字符串类型
typedef struct YBX_String_tag YBX_String;

// 指针信息
typedef struct {
    char *name;
} YBX_NativePointerInfo;

// 值的全部类型枚举
typedef enum {
    YBX_BOOLEAN_VALUE = 1,			// 布尔类型
    YBX_INT_VALUE,					// 整形
    YBX_DOUBLE_VALUE,				// 浮点型
    YBX_STRING_VALUE,				// 字符串类型
    YBX_NATIVE_POINTER_VALUE,		// 指针类型
    YBX_NULL_VALUE					// null
} YBX_ValueType;

// 指针类型
typedef struct {
    YBX_NativePointerInfo       *info;
    void                        *pointer;               //这里为了容纳各种类型的指针,建议选择使用 void*
} YBX_NativePointer;

// 值（整形、布尔、字符串、浮点、指针等等）
typedef struct {
    YBX_ValueType       type;                           //用来区分类型
    union {                                             //实际是存储在联合体当中的
        YBX_Boolean     boolean_value;					// 布尔类型
        int             int_value;						// 整形
        double          double_value;					// 浮点型
        YBX_String      *string_value;					// 字符串
        YBX_NativePointer       native_pointer;		    // 指针
    } u;
} YBX_Value;

// 从 ybx 调用c语言的函数
typedef YBX_Value YBX_NativeFunctionProc(YBX_Interpreter *interpreter,
                                         int arg_count, YBX_Value *args);
/*
 * 这里的 interpreter 是指向解释器的指针
 * arg_count 表示传参的个数
 * args 表示传参都是什么
 * 
 * 就像
 * int main(int argc, char *argv[])
*/

// 将函数c函数注册到解释器中
void YBX_add_native_function(YBX_Interpreter *interpreter,
                             char *name, YBX_NativeFunctionProc *proc);
							 
// 注册全局变量
void YBX_add_global_variable(YBX_Interpreter *inter,
                             char *identifier, YBX_Value *value);

#endif /* PUBLIC_YBX_DEV_H_INCLUDED */
