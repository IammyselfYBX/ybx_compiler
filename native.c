/*
* Copyright (c) 2019,YBX is the most handsome man in NCEPU
* All rights reserved.
*
* 文件名称: native.c
* 文件标识: 见README.md
* 摘要: 内置函数对指针的使用
*      对内置函数的声明见 interface.c
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
#include "YBX_dev.h"
#include "ybxcompiler.h"

#define NATIVE_LIB_NAME "crowbar.lang.file"

static YBX_NativePointerInfo st_native_lib_info = {
    NATIVE_LIB_NAME
};

YBX_Value ybx_nv_print_proc(YBX_Interpreter *interpreter,
                            int arg_count, YBX_Value *args)
{
    YBX_Value value;

    value.type = YBX_NULL_VALUE;

    if (arg_count < 1) {
        ybx_runtime_error(0, ARGUMENT_TOO_FEW_ERR,
                          MESSAGE_ARGUMENT_END);
    } else if (arg_count > 1) {
        ybx_runtime_error(0, ARGUMENT_TOO_MANY_ERR,
                          MESSAGE_ARGUMENT_END);
    }
    switch (args[0].type) {
    case YBX_BOOLEAN_VALUE:
        if (args[0].u.boolean_value) {
            printf("true");
        } else {
            printf("false");
        }
        break;
    case YBX_INT_VALUE:
        printf("%d", args[0].u.int_value);
        break;
    case YBX_DOUBLE_VALUE:
        printf("%f", args[0].u.double_value);
        break;
    case YBX_STRING_VALUE:
        printf("%s", args[0].u.string_value->string);
        break;
    case YBX_NATIVE_POINTER_VALUE:
        printf("(%s:%p)",
               args[0].u.native_pointer.info->name,
               args[0].u.native_pointer.pointer);
        break;
    case YBX_NULL_VALUE:
        printf("null");
        break;
    }

    return value;
}

YBX_Value ybx_nv_fopen_proc(YBX_Interpreter *interpreter,
                            int arg_count, YBX_Value *args)
{
    YBX_Value value;
    FILE *fp;

    if (arg_count < 2) {
        ybx_runtime_error(0, ARGUMENT_TOO_FEW_ERR,
                          MESSAGE_ARGUMENT_END);
    } else if (arg_count > 2) {
        ybx_runtime_error(0, ARGUMENT_TOO_MANY_ERR,
                          MESSAGE_ARGUMENT_END);
    }
    if (args[0].type != YBX_STRING_VALUE
        || args[1].type != YBX_STRING_VALUE) {
        ybx_runtime_error(0, FOPEN_ARGUMENT_TYPE_ERR,
                          MESSAGE_ARGUMENT_END);
    }
    
    fp = fopen(args[0].u.string_value->string,
               args[1].u.string_value->string);
    if (fp == NULL) {
        value.type = YBX_NULL_VALUE;
    } else {
        value.type = YBX_NATIVE_POINTER_VALUE;
        value.u.native_pointer.info = &st_native_lib_info;
        value.u.native_pointer.pointer = fp;
    }

    return value;
}

// 检查原生指针
static YBX_Boolean check_native_pointer(YBX_Value *value)
{
    return value->u.native_pointer.info == &st_native_lib_info;
}

YBX_Value ybx_nv_fclose_proc(YBX_Interpreter *interpreter,
                             int arg_count, YBX_Value *args)
{
    YBX_Value value;
    FILE *fp;

    value.type = YBX_NULL_VALUE;
    if (arg_count < 1) {
        ybx_runtime_error(0, ARGUMENT_TOO_FEW_ERR,
                          MESSAGE_ARGUMENT_END);
    } else if (arg_count > 1) {
        ybx_runtime_error(0, ARGUMENT_TOO_MANY_ERR,
                          MESSAGE_ARGUMENT_END);
    }
    if (args[0].type != YBX_NATIVE_POINTER_VALUE
        || !check_native_pointer(&args[0])) {
        ybx_runtime_error(0, FCLOSE_ARGUMENT_TYPE_ERR,
                          MESSAGE_ARGUMENT_END);
    }
    fp = args[0].u.native_pointer.pointer;
    fclose(fp);

    return value;
}

// 读取?
YBX_Value ybx_nv_fgets_proc(YBX_Interpreter *interpreter,
                            int arg_count, YBX_Value *args)
{
    YBX_Value value;
    FILE *fp;
    char buf[LINE_BUF_SIZE];
    char *ret_buf = NULL;
    int ret_len = 0;

    if (arg_count < 1) {
        ybx_runtime_error(0, ARGUMENT_TOO_FEW_ERR,
                          MESSAGE_ARGUMENT_END);
    } else if (arg_count > 1) {
        ybx_runtime_error(0, ARGUMENT_TOO_MANY_ERR,
                          MESSAGE_ARGUMENT_END);
    }
    if (args[0].type != YBX_NATIVE_POINTER_VALUE
        || !check_native_pointer(&args[0])) {
        ybx_runtime_error(0, FGETS_ARGUMENT_TYPE_ERR,
                          MESSAGE_ARGUMENT_END);
    }
    fp = args[0].u.native_pointer.pointer;

    while (fgets(buf, LINE_BUF_SIZE, fp)) {
        int new_len;
        new_len = ret_len + strlen(buf);
        ret_buf = MEM_realloc(ret_buf, new_len + 1);
        if (ret_len == 0) {
            strcpy(ret_buf, buf);
        } else {
            strcat(ret_buf, buf);
        }
        ret_len = new_len;
        if (ret_buf[ret_len-1] == '\n')
            break;
    }
    if (ret_len > 0) {
        value.type = YBX_STRING_VALUE;
        value.u.string_value = ybx_create_crowbar_string(interpreter, ret_buf);
    } else {
        value.type = YBX_NULL_VALUE;
    }

    return value;
}

// 可变参数的打印？
YBX_Value ybx_nv_fputs_proc(YBX_Interpreter *interpreter,
                            int arg_count, YBX_Value *args)
{
    YBX_Value value;
    FILE *fp;

    value.type = YBX_NULL_VALUE;
    if (arg_count < 2) {
        ybx_runtime_error(0, ARGUMENT_TOO_FEW_ERR,
                          MESSAGE_ARGUMENT_END);
    } else if (arg_count > 2) {
        ybx_runtime_error(0, ARGUMENT_TOO_MANY_ERR,
                          MESSAGE_ARGUMENT_END);
    }
    if (args[0].type != YBX_STRING_VALUE
        || (args[1].type != YBX_NATIVE_POINTER_VALUE
            || !check_native_pointer(&args[1]))) {
        ybx_runtime_error(0, FPUTS_ARGUMENT_TYPE_ERR,
                          MESSAGE_ARGUMENT_END);
    }
    fp = args[1].u.native_pointer.pointer;

    fputs(args[0].u.string_value->string, fp);

    return value;
}

// 将stdin，stdout，stderr添加到全局变量中
void ybx_add_std_fp(YBX_Interpreter *inter)
{
    YBX_Value fp_value;

    fp_value.type = YBX_NATIVE_POINTER_VALUE;
    fp_value.u.native_pointer.info = &st_native_lib_info;

    fp_value.u.native_pointer.pointer = stdin;
    YBX_add_global_variable(inter, "STDIN", &fp_value);		//将stdin添加到全局变量列表中

    fp_value.u.native_pointer.pointer = stdout;				
    YBX_add_global_variable(inter, "STDOUT", &fp_value);	// 将stdout添加到全局变量链表中

    fp_value.u.native_pointer.pointer = stderr;
    YBX_add_global_variable(inter, "STDERR", &fp_value);	// 将stderr添加到全局变量列表中
}
