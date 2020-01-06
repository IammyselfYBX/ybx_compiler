/*
* Copyright (c) 2019,YBX is the most handsome man in NCEPU
* All rights reserved.
*
* 文件名称: string_pool.c
* 文件标识: 见README.md
* 摘要: 字符串与垃圾回收机制
*      采用引用计数的方式实现（这里是管理字符串的指针数量，当计时器为0时启动回收机制）
*
*   eg:a = "a" + "b" + "c"
*   先执行 "a" + "b" 生成 ab，然后在继续生成 abc，但是生成 abc 之后还要把 ab 的内存空间释放。
*
*
*
* 当前版本: 1.1
* 作者: 杨秉学
* 完成日期: 2019年11月17日
*
* 取代版本:1.0
* 原作者: 杨秉学
* 完成日期: 2018年5月2日
*/

#include <stdio.h>
#include <string.h>
#include "MEM.h"
#include "DBG.h"
#include "ybxcompiler.h"

// 为字符串分配内存
static YBX_String * alloc_ybx_string(YBX_Interpreter *inter, char *str, YBX_Boolean is_literal)
{
    YBX_String *ret;

    ret = MEM_malloc(sizeof(YBX_String));
    ret->ref_count = 0;
    ret->is_literal = is_literal;
    ret->string = str;

    return ret;
}

// 字面量的字符串
YBX_String * ybx_literal_to_ybx_string(YBX_Interpreter *inter, char *str)
{
    YBX_String *ret;

    ret = alloc_ybx_string(inter, str, YBX_TRUE);
    ret->ref_count = 1;

    return ret;
}

// 为字符串添加引用计数
void ybx_refer_string(YBX_String *str)
{
    str->ref_count++;
}

// 为字符串减少引用计数
void ybx_release_string(YBX_String *str)
{
    str->ref_count--;

    DBG_assert(str->ref_count >= 0, ("str->ref_count..%d\n",
                                     str->ref_count));
    if (str->ref_count == 0) {
        if (!str->is_literal) {
            MEM_free(str->string);
        }
        MEM_free(str);
    }
}

// 创建一个字符串
YBX_String * ybx_create_crowbar_string(YBX_Interpreter *inter, char *str)
{
    YBX_String *ret = alloc_ybx_string(inter, str, YBX_FALSE);
    ret->ref_count = 1;

    return ret;
}
