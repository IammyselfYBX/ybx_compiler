/*
* Copyright (c) 2019,YBX is the most handsome man in NCEPU
* All rights reserved.
*
* 文件名称: util.c
* 文件标识: 见README.md
* 摘要: 声明编译器运行时的操作
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

static YBX_Interpreter *st_current_interpreter;

// 获取当前解释器的指针
YBX_Interpreter * ybx_get_current_interpreter(void)
{
    return st_current_interpreter;
}

// 设置当前解释器的指针
void ybx_set_current_interpreter(YBX_Interpreter *inter)
{
    st_current_interpreter = inter;
}

/* BUGBUG
YBX_NativeFunctionProc *
ybx_search_native_function(YBX_Interpreter *inter, char *name)
{
    NativeFunction *pos;

    for (pos = inter->native_function; pos; pos = pos->next) {
        if (!strcmp(pos->name, name))
            break;
    }
    if (pos) {
        return pos->proc;
    } else {
        return NULL;
    }
}
*/

// 根据函数名查找函数定义
FunctionDefinition * ybx_search_function(char *name)
{
    FunctionDefinition *pos;
    YBX_Interpreter *inter;

    inter = ybx_get_current_interpreter();
    for (pos = inter->function_list; pos; pos = pos->next) {
        if (!strcmp(pos->name, name))
            break;
    }
    return pos;
}

// 分配内存
void * ybx_malloc(size_t size)
{
    void *p;
    YBX_Interpreter *inter;

    inter = ybx_get_current_interpreter();
	
    p = MEM_storage_malloc(inter->interpreter_storage, size); // 首先分配存储器，再从存储器中分配指定量的内存

    return p;
}

// 运行时内存分配
void * ybx_execute_malloc(YBX_Interpreter *inter, size_t size)
{
    void *p;

    p = MEM_storage_malloc(inter->execute_storage, size);

    return p;
}

// 查找局部变量
Variable * ybx_search_local_variable(LocalEnvironment *env, char *identifier)
{
    Variable    *pos;

    if (env == NULL)
        return NULL;

    for (pos = env->variable; pos; pos = pos->next) 
	{
        if (!strcmp(pos->name, identifier))
            break;
    }

    if (pos == NULL) 
	{
        return NULL;
    } 
	else
	{
        return pos;
    }
}

// 查找全局变量
Variable * ybx_search_global_variable(YBX_Interpreter *inter, char *identifier)
{
    Variable    *pos;

    for (pos = inter->variable; pos; pos = pos->next) {
        if (!strcmp(pos->name, identifier))
            return pos;
    }

    return NULL;
}

// 将一些变量添加到局部变量环境中
void ybx_add_local_variable(LocalEnvironment *env,char *identifier, YBX_Value *value)
{
    Variable    *new_variable;

    new_variable = MEM_malloc(sizeof(Variable));
    new_variable->name = identifier;
    new_variable->value = *value;
    new_variable->next = env->variable;
    env->variable = new_variable;
}

// 将一些变量添加到全局变量环境中
void YBX_add_global_variable(YBX_Interpreter *inter, char *identifier,YBX_Value *value)
{
    Variable    *new_variable;

    new_variable = ybx_execute_malloc(inter, sizeof(Variable));						// 运行时内存分配
    new_variable->name = ybx_execute_malloc(inter, strlen(identifier) + 1);	        // 运行时分配内存
    strcpy(new_variable->name, identifier);						
    new_variable->next = inter->variable;
    inter->variable = new_variable;													// 加入到全局的变量列表中
    new_variable->value = *value;
}

// 获取操作符的字符串形式
char * ybx_get_operator_string(ExpressionType type)
{
    char        *str;

    switch (type) 
	{
    case BOOLEAN_EXPRESSION:    /* FALLTHRU */
    case INT_EXPRESSION:        /* FALLTHRU */
    case DOUBLE_EXPRESSION:     /* FALLTHRU */
    case STRING_EXPRESSION:     /* FALLTHRU */
    case IDENTIFIER_EXPRESSION:
        DBG_panic(("bad expression type..%d\n", type));
        break;
    case ASSIGN_EXPRESSION:
        str = "=";
        break;
    case ADD_EXPRESSION:
        str = "+";
        break;
    case SUB_EXPRESSION:
        str = "-";
        break;
    case MUL_EXPRESSION:
        str = "*";
        break;
    case DIV_EXPRESSION:
        str = "/";
        break;
    case MOD_EXPRESSION:
        str = "%";
        break;
    case LOGICAL_AND_EXPRESSION:
        str = "&&";
        break;
    case LOGICAL_OR_EXPRESSION:
        str = "||";
        break;
    case EQ_EXPRESSION:
        str = "==";
        break;
    case NE_EXPRESSION:
        str = "!=";
        break;
    case GT_EXPRESSION:
        str = "<";
        break;
    case GE_EXPRESSION:
        str = "<=";
        break;
    case LT_EXPRESSION:
        str = ">";
        break;
    case LE_EXPRESSION:
        str = ">=";
        break;
    case MINUS_EXPRESSION:
        str = "-";
        break;
    case FUNCTION_CALL_EXPRESSION:  /* FALLTHRU */
    case NULL_EXPRESSION:  /* FALLTHRU */
    case EXPRESSION_TYPE_COUNT_PLUS_1:
    default:
        DBG_panic(("bad expression type..%d\n", type));
    }

    return str;
}
