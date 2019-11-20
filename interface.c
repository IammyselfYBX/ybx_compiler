/*
* Copyright (c) 2019,YBX is the most handsome man in NCEPU
* All rights reserved.
*
* 文件名称: interface.c
* 文件标识: 见README.md
* 摘要: 内置函数的声明使用
*	   对指针的使用见native.c
*
* 当前版本: 1.1
* 作者: 杨秉学
* 完成日期: 2019年11月17日
*
* 取代版本:1.0
* 原作者: 杨秉学
* 完成日期: 2018年5月2日
*/

#include "MEM.h"
#include "DBG.h"
#define GLOBAL_VARIABLE_DEFINE
#include "ybxcompiler.h"

// 注册内置函数
static void add_native_functions(YBX_Interpreter *inter)
{
	YBX_add_native_function(inter, "print", ybx_nv_print_proc);		// 注册print函数
	YBX_add_native_function(inter, "fopen", ybx_nv_fopen_proc);		// 注册fopen函数
	YBX_add_native_function(inter, "fclose", ybx_nv_fclose_proc);	// 注册fclose函数
	YBX_add_native_function(inter, "fgets", ybx_nv_fgets_proc);		// 注册fget函数
	YBX_add_native_function(inter, "fputs", ybx_nv_fputs_proc);		// 注册fput函数
}

// 创建解释器
YBX_Interpreter * YBX_create_interpreter(void) 
{
	MEM_Storage storage;
	YBX_Interpreter *interpreter;

	storage = MEM_open_storage(0);															// 打开一个存储器
	interpreter = MEM_storage_malloc(storage,sizeof(struct YBX_Interpreter_tag));			// 从存储器中分配内存（一个解释器的大小）
	interpreter->interpreter_storage = storage;												//解释器的存储器
	interpreter->execute_storage = NULL;													// 运行时的存储器
	interpreter->variable = NULL;		
	interpreter->function_list = NULL;
	interpreter->statement_list = NULL;
	interpreter->current_line_number = 1;

	ybx_set_current_interpreter(interpreter);
	add_native_functions(interpreter);														// 注册内置函数

	return interpreter;
}

// 调用编译器
void YBX_compile(YBX_Interpreter *interpreter, FILE *fp)
{
	extern int yyparse(void);
	extern FILE *yyin;

	ybx_set_current_interpreter(interpreter);

	yyin = fp;																			// 源代码文件
	if (yyparse())																		// 语法分析
	{													
		/* BUGBUG */
		fprintf(stderr, "Error ! Error ! Error !\n");
		exit(1);
	}
	ybx_reset_string_literal_buffer();													// 重置字面字符串的缓存
}

// 调用解释器
void YBX_interpret(YBX_Interpreter *interpreter)
{
	interpreter->execute_storage = MEM_open_storage(0);								// 分配运行时存储器
	ybx_add_std_fp(interpreter);																			// 将标准输入、标准输出、标准错误添加到全局变量列表中
	ybx_execute_statement_list(interpreter, NULL, interpreter->statement_list);	// 开始执行第一条语句
}

// 释放全局的字符串
static void release_global_strings(YBX_Interpreter *interpreter) {
	while (interpreter->variable) {
		Variable *temp = interpreter->variable;
		interpreter->variable = temp->next;
		if (temp->value.type == YBX_STRING_VALUE) {
			ybx_release_string(temp->value.u.string_value);
		}
	}
}

// 释放解释器
void YBX_dispose_interpreter(YBX_Interpreter *interpreter)
{
	release_global_strings(interpreter);

	if (interpreter->execute_storage) {
		MEM_dispose_storage(interpreter->execute_storage);
	}

	MEM_dispose_storage(interpreter->interpreter_storage);
}

// 注册内置函数
void YBX_add_native_function(YBX_Interpreter *interpreter,
	char *name, YBX_NativeFunctionProc *proc)
{
	FunctionDefinition *fd;

	fd = ybx_malloc(sizeof(FunctionDefinition));
	fd->name = name;
	fd->type = NATIVE_FUNCTION_DEFINITION;
	fd->u.native_f.proc = proc;
	fd->next = interpreter->function_list;

	interpreter->function_list = fd;								// 解释器的函数列表（期初起初只有内置函数）
}
