/*
* Copyright (c) 2019,YBX is the most handsome man in NCEPU
* All rights reserved.
*
* 文件名称: bxcompiler.h
* 文件标识: 见README.md
* 摘要: 定义了编译器的全部变量类型，变量空间、语法、流程控制、解释器定义、运算符号、表达式、函数、形参
*      
* 		这是一个私有的头文件，尽对编译器内部可见
*
* 当前版本: 1.1
* 作者: 杨秉学
* 完成日期: 2019年11月17日
*
* 取代版本:1.0
* 原作者: 杨秉学
* 完成日期: 2018年5月2日
*/
#ifndef PRIVATE_CROWBAR_H_INCLUDED
#define PRIVATE_CROWBAR_H_INCLUDED
#include <stdio.h>
#include "MEM.h"
#include "YBX_dev.h"
#include "YBX.h"

#define smaller(a, b) ((a) < (b) ? (a) : (b))
#define larger(a, b) ((a) > (b) ? (a) : (b))

#define MESSAGE_ARGUMENT_MAX    (256)
#define LINE_BUF_SIZE           (1024)



// 数学操作符号
#define dkc_is_math_operator(operator) \
	((operator) == ADD_EXPRESSION || (operator) == SUB_EXPRESSION\
	|| (operator) == MUL_EXPRESSION || (operator) == DIV_EXPRESSION\
	|| (operator) == MOD_EXPRESSION)


// 比较符号
#define dkc_is_compare_operator(operator) \
	((operator) == EQ_EXPRESSION || (operator) == NE_EXPRESSION\
	|| (operator) == GT_EXPRESSION || (operator) == GE_EXPRESSION\
	|| (operator) == LT_EXPRESSION || (operator) == LE_EXPRESSION)

// 逻辑符号
#define dkc_is_logical_operator(operator) \
	((operator) == LOGICAL_AND_EXPRESSION || (operator) == LOGICAL_OR_EXPRESSION)



// 编译时的错误枚举
typedef enum
{
	PARSE_ERR = 1,						// 语法分析错误
	CHARACTER_INVALID_ERR,				// 字符无效
	FUNCTION_MULTIPLE_DEFINE_ERR,		// 函数多重定义
	COMPILE_ERROR_COUNT_PLUS_1
} CompileError;

// 运行时错误枚举
typedef enum 
{
	VARIABLE_NOT_FOUND_ERR = 1,			// 变量没找到
	FUNCTION_NOT_FOUND_ERR,				// 函数没找到
	ARGUMENT_TOO_MANY_ERR,				// 参数太多
	ARGUMENT_TOO_FEW_ERR,				// 参数不够
	NOT_BOOLEAN_TYPE_ERR,				// 没有bool类型
	MINUS_OPERAND_TYPE_ERR,				// 负号类型错误
	BAD_OPERAND_TYPE_ERR,				// 错误的操作负号
	NOT_BOOLEAN_OPERATOR_ERR,			// 没有布尔操作负号
	FOPEN_ARGUMENT_TYPE_ERR,			// 打开文件参数类型错误
	FCLOSE_ARGUMENT_TYPE_ERR,			// 关闭文件参数类型错误
	FGETS_ARGUMENT_TYPE_ERR,			// fgets参数错误
	FPUTS_ARGUMENT_TYPE_ERR,			// fputs参数错误
	NOT_NULL_OPERATOR_ERR,				// 没有null操作符
	DIVISION_BY_ZERO_ERR,				// 除0
	GLOBAL_VARIABLE_NOT_FOUND_ERR,		// 全局变量没找到
	GLOBAL_STATEMENT_IN_TOPLEVEL_ERR,	// 全局语句在顶层结构
	BAD_OPERATOR_FOR_STRING_ERR,		// string类型的错误
	RUNTIME_ERROR_COUNT_PLUS_1
} RuntimeError;

// 参数类型
typedef enum 
{
	INT_MESSAGE_ARGUMENT = 1,
	DOUBLE_MESSAGE_ARGUMENT,
	STRING_MESSAGE_ARGUMENT,
	CHARACTER_MESSAGE_ARGUMENT,
	POINTER_MESSAGE_ARGUMENT,
	MESSAGE_ARGUMENT_END
} MessageArgumentType;

// 参数格式
typedef struct 
{
	char *format;
} MessageFormat;

typedef struct Expression_tag Expression;

// 表达式类型
typedef enum 
{
	BOOLEAN_EXPRESSION = 1,						// 布尔表达式
	INT_EXPRESSION,								// 整数表达式
	DOUBLE_EXPRESSION,							// 浮点表达式
	STRING_EXPRESSION,							// 字符串表达式
	IDENTIFIER_EXPRESSION,						// 定义
	ASSIGN_EXPRESSION,							// 赋值
	ADD_EXPRESSION,								// 加法 +
	SUB_EXPRESSION,								// 减法 -
	MUL_EXPRESSION,								// 乘法 *
	DIV_EXPRESSION,								// 除法 /
	MOD_EXPRESSION,								// 求于 %
	EQ_EXPRESSION,								// 等于
	NE_EXPRESSION,								// 不等于
	GT_EXPRESSION,								// 大于
	GE_EXPRESSION,								// 大于等于
	LT_EXPRESSION,								// 小于
	LE_EXPRESSION,								// 小于等于
	LOGICAL_AND_EXPRESSION,						// &&
	LOGICAL_OR_EXPRESSION,						// ||
	MINUS_EXPRESSION,							// 取负号
	FUNCTION_CALL_EXPRESSION,					// 函数调用
	NULL_EXPRESSION,							// 空表达式null
	EXPRESSION_TYPE_COUNT_PLUS_1
} ExpressionType;

// 参数列表
typedef struct ArgumentList_tag 
{
	Expression *expression;
	struct ArgumentList_tag *next;
} ArgumentList;

// 赋值表达式
typedef struct 
{
	char *variable;								//变量名（即等号左边的变量）
	Expression  *operand;						// 操作数（即等号右边的变量）
} AssignExpression;

// 二元表达式
typedef struct 
{
	Expression  *left;							// 二元符号的左边的表达式
	Expression  *right;							// 二元符号的右边的表达式
} BinaryExpression;

// 函数调用表达式
typedef struct 
{
	char *identifier;							// 函数的名字或者
	ArgumentList *argument;						// 形参列表
} FunctionCallExpression;

// 表达式定义
struct Expression_tag 
{
	ExpressionType type;													// 表达式类型
	int line_number;														// 行号（表达式所在位置）
	union 
	{
		YBX_Boolean             boolean_value;								// 布尔表达式
		int                     int_value;									// 整形表达式
		double                  double_value;								// 浮点表达式
		char                    *string_value;								// 字符串表达式
		char                    *identifier;								// 定义表达式
		AssignExpression        assign_expression;							// 赋值表达式
		BinaryExpression        binary_expression;							// 二元表达式
		Expression              *minus_expression;							// 负号表达式
		FunctionCallExpression  function_call_expression;					// 函数调用表达式
	} u;
};

typedef struct Statement_tag Statement;

// 语句列表
typedef struct StatementList_tag 
{
	Statement   *statement;						// 语句
	struct StatementList_tag    *next;			// 下一条语句
} StatementList;

// 一个语句块，即由一系列语句组成
typedef struct 
{
	StatementList       *statement_list;		//语句链表，用链表存储每一条语句
} Block;

typedef struct IdentifierList_tag 
{
	char        *name;
	struct IdentifierList_tag   *next;
} IdentifierList;

typedef struct 
{
	IdentifierList      *identifier_list;
} GlobalStatement;

typedef struct Elsif_tag 
{
	Expression  *condition;
	Block       *block;
	struct Elsif_tag    *next;
} Elsif;

typedef struct 
{
	Expression  *condition;
	Block       *then_block;
	Elsif       *elsif_list;
	Block       *else_block;
} IfStatement;

typedef struct 
{
	Expression  *condition;
	Block       *block;
} WhileStatement;

typedef struct 
{
	Expression  *init;
	Expression  *condition;
	Expression  *post;
	Block       *block;
} ForStatement;

typedef struct 
{
	Expression *return_value;
} ReturnStatement;

typedef enum 
{
	EXPRESSION_STATEMENT = 1,				// 表达式语句
	GLOBAL_STATEMENT,						// global语句
	IF_STATEMENT,							// if语句
	WHILE_STATEMENT,						// while语句
	FOR_STATEMENT,							// for语句
	RETURN_STATEMENT,						// return语句
	BREAK_STATEMENT,						// break语句
	CONTINUE_STATEMENT,						// continue语句
	STATEMENT_TYPE_COUNT_PLUS_1
} StatementType;

// 语句（表达式也是一种语句）
struct Statement_tag 
{
	StatementType       type;				// 语句类型
	int                 line_number;		// 语句所在位置
	union 
	{
		Expression      *expression_s;		// 表达式语句
		GlobalStatement global_s;			// 全局，即带global的语句
		IfStatement     if_s;				// if语句
		WhileStatement  while_s;			// while语句
		ForStatement    for_s;				// for语句
		ReturnStatement return_s;			// return 语句
	} u;
};

// 形参列表（注意形参是没有值的）
//将形参存储到链表，因为 ybx 是无类型语言，所以不需要存储数据类型
typedef struct ParameterList_tag 
{
	char        *name;						// 形参名称
	struct ParameterList_tag *next;			// 下一个形参
} ParameterList;

typedef enum 
{
	YBX_FUNCTION_DEFINITION = 1,		// 用户自定义函数
	NATIVE_FUNCTION_DEFINITION				// 内置函数
} FunctionDefinitionType;

// 函数定义
typedef struct FunctionDefinition_tag 
{
	char *name;								// 函数名
	FunctionDefinitionType type;			// 函数类型：用户自定义或者内置类型

	union 
	{
		struct								// 用户自定义函数
		{
			ParameterList *parameter;		// 形参列表，形参的定义
			Block *block;					// 语句块，函数的主体
		} ybx_f;

		struct								// 内置函数
		{
			YBX_NativeFunctionProc *proc;	// 内置函数指针
		} native_f;
	} u;

	struct FunctionDefinition_tag *next;	// 下一个函数定义
} FunctionDefinition;

// 变量链表
typedef struct Variable_tag 
{
	char        *name;						// 变量名
	YBX_Value   value;						// 变量值
	struct Variable_tag *next;				// 下一个变量
} Variable;

// 语句执行完成的类型
typedef enum 
{
	NORMAL_STATEMENT_RESULT = 1,			// 语句正常执行
	RETURN_STATEMENT_RESULT,				// 语句由return返回
	BREAK_STATEMENT_RESULT,					// 语句由break中断返回
	CONTINUE_STATEMENT_RESULT,				// 语句由continue返回
	STATEMENT_RESULT_TYPE_COUNT_PLUS_1
} StatementResultType;

typedef struct 
{
	StatementResultType type;
	union
	{
		YBX_Value       return_value; 		//语句的返回值
	} u;
} StatementResult;

// 全局变量的引用
typedef struct GlobalVariableRef_tag 
{
	Variable    *variable;
	struct GlobalVariableRef_tag *next;
} GlobalVariableRef;

// 执行环境，用于存放局部变量和全局变量的引用
typedef struct 
{
	Variable    *variable;
	GlobalVariableRef   *global_variable;
} LocalEnvironment;

struct YBX_String_tag
{
	int         ref_count;
	char        *string;
	YBX_Boolean is_literal;
};

typedef struct 
{
	YBX_String  *strings;
} StringPool;

// 定义解释器的结构体	
// 在YBX.h中定义的是YBX_Interpreter_tag 的声明，这里将YBX_Interpreter_tag 定义
struct YBX_Interpreter_tag 
{

	MEM_Storage         interpreter_storage;		// 解释器的存储器，在解释器生成的时候生成，解释器结束的时候释放
	MEM_Storage         execute_storage;			// 运行时的存储器
	Variable            *variable;					// 变量链表，通过该链表得到全部变量
	FunctionDefinition  *function_list;				// 函数链表，
	StatementList       *statement_list;			// 语句列表（表达式也是一种语句）
	int                 current_line_number;		// 当前行号，当出现错误时用于指明出错位置，仅编译的时候使用
};


/* create.c */
void ybx_function_define(char *identifier, ParameterList *parameter_list,Block *block);
ParameterList *ybx_create_parameter(char *identifier);
ParameterList *ybx_chain_parameter(ParameterList *list,char *identifier);
ArgumentList *ybx_create_argument_list(Expression *expression);
ArgumentList *ybx_chain_argument_list(ArgumentList *list, Expression *expr);
StatementList *ybx_create_statement_list(Statement *statement);
StatementList *ybx_chain_statement_list(StatementList *list,Statement *statement);
Expression *ybx_alloc_expression(ExpressionType type);
Expression *ybx_create_assign_expression(char *variable,Expression *operand);
Expression *ybx_create_binary_expression(ExpressionType operator,Expression *left,Expression *right);
Expression *ybx_create_minus_expression(Expression *operand);
Expression *ybx_create_identifier_expression(char *identifier);
Expression *ybx_create_function_call_expression(char *func_name,ArgumentList *argument);
Expression *ybx_create_boolean_expression(YBX_Boolean value);
Expression *ybx_create_null_expression(void);
Statement *ybx_create_global_statement(IdentifierList *identifier_list);
IdentifierList *ybx_create_global_identifier(char *identifier);
IdentifierList *ybx_chain_identifier(IdentifierList *list, char *identifier);
Statement *ybx_create_if_statement(Expression *condition,Block *then_block, Elsif *elsif_list,Block *else_block);
Elsif *ybx_chain_elsif_list(Elsif *list, Elsif *add);
Elsif *ybx_create_elsif(Expression *expr, Block *block);
Statement *ybx_create_while_statement(Expression *condition, Block *block);
Statement *ybx_create_for_statement(Expression *init, Expression *cond,Expression *post, Block *block);
Block *ybx_create_block(StatementList *statement_list);
Statement *ybx_create_expression_statement(Expression *expression);
Statement *ybx_create_return_statement(Expression *expression);
Statement *ybx_create_break_statement(void);
Statement *ybx_create_continue_statement(void);

/* string.c */
char *ybx_create_identifier(char *str);
void ybx_open_string_literal(void);
void ybx_add_string_literal(int letter);
void ybx_reset_string_literal_buffer(void);
char *ybx_close_string_literal(void);

/* execute.c */
StatementResult ybx_execute_statement_list(YBX_Interpreter *inter,LocalEnvironment *env, StatementList *list);

/* eval.c */
YBX_Value ybx_eval_binary_expression(YBX_Interpreter *inter,LocalEnvironment *env,ExpressionType operator,Expression *left, Expression *right);
YBX_Value ybx_eval_minus_expression(YBX_Interpreter *inter,LocalEnvironment *env, Expression *operand);
YBX_Value ybx_eval_expression(YBX_Interpreter *inter,LocalEnvironment *env, Expression *expr);

/* string_pool.c */
YBX_String *ybx_literal_to_ybx_string(YBX_Interpreter *inter, char *str);
void ybx_refer_string(YBX_String *str);
void ybx_release_string(YBX_String *str);
YBX_String *ybx_search_ybx_string(YBX_Interpreter *inter, char *str);
YBX_String *ybx_create_crowbar_string(YBX_Interpreter *inter, char *str);

/* util.c */
YBX_Interpreter *ybx_get_current_interpreter(void);
void ybx_set_current_interpreter(YBX_Interpreter *inter);
void *ybx_malloc(size_t size);
void *ybx_execute_malloc(YBX_Interpreter *inter, size_t size);
Variable *ybx_search_local_variable(LocalEnvironment *env,char *identifier);
Variable * ybx_search_global_variable(YBX_Interpreter *inter, char *identifier);
void ybx_add_local_variable(LocalEnvironment *env,char *identifier, YBX_Value *value);
YBX_NativeFunctionProc* ybx_search_native_function(YBX_Interpreter *inter, char *name);
FunctionDefinition *ybx_search_function(char *name);
char *ybx_get_operator_string(ExpressionType type);

/* error.c */
void ybx_compile_error(CompileError id, ...);
void ybx_runtime_error(int line_number, RuntimeError id, ...);

/* native.c */
YBX_Value ybx_nv_print_proc(YBX_Interpreter *interpreter,int arg_count, YBX_Value *args);
YBX_Value ybx_nv_fopen_proc(YBX_Interpreter *interpreter,int arg_count, YBX_Value *args);
YBX_Value ybx_nv_fclose_proc(YBX_Interpreter *interpreter,int arg_count, YBX_Value *args);
YBX_Value ybx_nv_fgets_proc(YBX_Interpreter *interpreter,int arg_count, YBX_Value *args);
YBX_Value ybx_nv_fputs_proc(YBX_Interpreter *interpreter,int arg_count, YBX_Value *args);
void ybx_add_std_fp(YBX_Interpreter *inter);

#endif /* PRIVATE_CROWBAR_H_INCLUDED */
