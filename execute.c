/*
* Copyright (c) 2019,YBX is the most handsome man in NCEPU
* All rights reserved.
*
* 文件名称: execute.c
* 文件标识: 见README.md
* 摘要: 实现执行语句部分，语句是怎么执行的
*      这里的语句是指if while else等句子，其他的都是表达式
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

#include <math.h>
#include <string.h>
#include "MEM.h"
#include "DBG.h"
#include "ybxcompiler.h"

// 执行语句，根据不同的Statement类型执行不同的处理
static StatementResult execute_statement(YBX_Interpreter *inter, LocalEnvironment *env,
                  Statement *statement);


// 执行表达式语句，表达式有很多种
static StatementResult execute_expression_statement(YBX_Interpreter *inter, LocalEnvironment *env,
                             Statement *statement)
{
    StatementResult result;
    YBX_Value v;

    result.type = NORMAL_STATEMENT_RESULT;

    v = ybx_eval_expression(inter, env, statement->u.expression_s);	// 表达式求值（statement->u.expression_s是表示语句是一个表达式，statement->u.expression_s指向真正的表达式）
    if (v.type == YBX_STRING_VALUE)
	{
        ybx_release_string(v.u.string_value);											// 有疑问，这里不是应该增加的么？（）
    }

    return result;
}

// 执行全局语句
static StatementResult execute_global_statement(YBX_Interpreter *inter, LocalEnvironment *env,
                         Statement *statement)      //如果传递的函数还没有运行，则env为NULL
{
    IdentifierList *pos;
    StatementResult result;

    result.type = NORMAL_STATEMENT_RESULT;

    if (env == NULL) {
        ybx_runtime_error(statement->line_number,
                          GLOBAL_STATEMENT_IN_TOPLEVEL_ERR,
                          MESSAGE_ARGUMENT_END);
    }
    for (pos = statement->u.global_s.identifier_list; pos; pos = pos->next) {
        GlobalVariableRef *ref_pos;
        GlobalVariableRef *new_ref;
        Variable *variable;
        for (ref_pos = env->global_variable; ref_pos;
             ref_pos = ref_pos->next) {
            if (!strcmp(ref_pos->variable->name, pos->name))
                goto NEXT_IDENTIFIER;
        }
        variable = ybx_search_global_variable(inter, pos->name);
        if (variable == NULL) {
            ybx_runtime_error(statement->line_number,
                              GLOBAL_VARIABLE_NOT_FOUND_ERR,
                              STRING_MESSAGE_ARGUMENT, "name", pos->name,
                              MESSAGE_ARGUMENT_END);
        }
        new_ref = MEM_malloc(sizeof(GlobalVariableRef));
        new_ref->variable = variable;
        new_ref->next = env->global_variable;
        env->global_variable = new_ref;
      NEXT_IDENTIFIER:
        ;
    }

    return result;
}

// elsif语句
static StatementResult execute_elsif(YBX_Interpreter *inter, LocalEnvironment *env,
              Elsif *elsif_list, YBX_Boolean *executed)
{
    StatementResult result;
    YBX_Value   cond;
    Elsif *pos;

    *executed = YBX_FALSE;
    result.type = NORMAL_STATEMENT_RESULT;
    for (pos = elsif_list; pos; pos = pos->next) {
        cond = ybx_eval_expression(inter, env, pos->condition);
        if (cond.type != YBX_BOOLEAN_VALUE) {
            ybx_runtime_error(pos->condition->line_number,
                              NOT_BOOLEAN_TYPE_ERR, MESSAGE_ARGUMENT_END);
        }
        if (cond.u.boolean_value) {
            result = ybx_execute_statement_list(inter, env,
                                                pos->block->statement_list);
            *executed = YBX_TRUE;
            if (result.type != NORMAL_STATEMENT_RESULT)
                goto FUNC_END;
        }
    }

  FUNC_END:
    return result;
}

// if句子
static StatementResult execute_if_statement(YBX_Interpreter *inter, LocalEnvironment *env,
                     Statement *statement)
{
    StatementResult result;
    YBX_Value   cond;

    result.type = NORMAL_STATEMENT_RESULT;
    cond = ybx_eval_expression(inter, env, statement->u.if_s.condition);
    if (cond.type != YBX_BOOLEAN_VALUE) {
        ybx_runtime_error(statement->u.if_s.condition->line_number,
                          NOT_BOOLEAN_TYPE_ERR, MESSAGE_ARGUMENT_END);
    }
    DBG_assert(cond.type == YBX_BOOLEAN_VALUE, ("cond.type..%d", cond.type));

    if (cond.u.boolean_value) {
        result = ybx_execute_statement_list(inter, env,
                                            statement->u.if_s.then_block
                                            ->statement_list);
    } else {
        YBX_Boolean elsif_executed;
        result = execute_elsif(inter, env, statement->u.if_s.elsif_list,
                               &elsif_executed);
        if (result.type != NORMAL_STATEMENT_RESULT)
            goto FUNC_END;
        if (!elsif_executed && statement->u.if_s.else_block) {
            result = ybx_execute_statement_list(inter, env,
                                                statement->u.if_s.else_block
                                                ->statement_list);
        }
    }

  FUNC_END:
    return result;
}

// while句子
static StatementResult execute_while_statement(YBX_Interpreter *inter, LocalEnvironment *env,
                        Statement *statement)
{
    StatementResult result;
    YBX_Value   cond;

    result.type = NORMAL_STATEMENT_RESULT;
    for (;;) {  //死循环
        /* 通过条件语句判断 */
        cond = ybx_eval_expression(inter, env, statement->u.while_s.condition);
        if (cond.type != YBX_BOOLEAN_VALUE) {
            ybx_runtime_error(statement->u.while_s.condition->line_number,
                              NOT_BOOLEAN_TYPE_ERR, MESSAGE_ARGUMENT_END);
        }
        DBG_assert(cond.type == YBX_BOOLEAN_VALUE,
                   ("cond.type..%d", cond.type));
        if (!cond.u.boolean_value)
            break;

        result = ybx_execute_statement_list(inter, env,
                                            statement->u.while_s.block
                                            ->statement_list);
        if (result.type == RETURN_STATEMENT_RESULT) {
            break;
        } else if (result.type == BREAK_STATEMENT_RESULT) {
            result.type = NORMAL_STATEMENT_RESULT;
            break;
        }
    }

    return result;
}

// for句子
static StatementResult execute_for_statement(YBX_Interpreter *inter, LocalEnvironment *env,
                      Statement *statement)
{
    StatementResult result;
    YBX_Value   cond;

    result.type = NORMAL_STATEMENT_RESULT;

    if (statement->u.for_s.init) {
        ybx_eval_expression(inter, env, statement->u.for_s.init);
    }
    for (;;) {
        if (statement->u.for_s.condition) {
            cond = ybx_eval_expression(inter, env,
                                       statement->u.for_s.condition);
            if (cond.type != YBX_BOOLEAN_VALUE) {
                ybx_runtime_error(statement->u.for_s.condition->line_number,
                                  NOT_BOOLEAN_TYPE_ERR, MESSAGE_ARGUMENT_END);
            }
            DBG_assert(cond.type == YBX_BOOLEAN_VALUE,
                       ("cond.type..%d", cond.type));
            if (!cond.u.boolean_value)
                break;
        }
        result = ybx_execute_statement_list(inter, env,
                                            statement->u.for_s.block
                                            ->statement_list);
        if (result.type == RETURN_STATEMENT_RESULT) {
            break;
        } else if (result.type == BREAK_STATEMENT_RESULT) {
            result.type = NORMAL_STATEMENT_RESULT;
            break;
        }

        if (statement->u.for_s.post) {
            ybx_eval_expression(inter, env, statement->u.for_s.post);
        }
    }

    return result;
}

// return句子
static StatementResult execute_return_statement(YBX_Interpreter *inter, LocalEnvironment *env,
                         Statement *statement)
{
    StatementResult result;

    result.type = RETURN_STATEMENT_RESULT;
    if (statement->u.return_s.return_value) {
        result.u.return_value
            = ybx_eval_expression(inter, env,
                                  statement->u.return_s.return_value);
    } else {
        result.u.return_value.type = YBX_NULL_VALUE;
    }

    return result;
}

// break句子
static StatementResult execute_break_statement(YBX_Interpreter *inter, LocalEnvironment *env,
                        Statement *statement)
{
    StatementResult result;

    result.type = BREAK_STATEMENT_RESULT;

    return result;
}

// continue句子
static StatementResult execute_continue_statement(YBX_Interpreter *inter, LocalEnvironment *env,
                           Statement *statement)
{
    StatementResult result;

    result.type = CONTINUE_STATEMENT_RESULT;

    return result;
}

// 语句执行——根据不同的 Statement 类型进行不同的处理
static StatementResult execute_statement(YBX_Interpreter *inter, LocalEnvironment *env, Statement *statement)
{
    StatementResult result;

    result.type = NORMAL_STATEMENT_RESULT;

    switch (statement->type)												// 根据语句的类型选择相应的语句执行调用
	{
    case EXPRESSION_STATEMENT:
        result = execute_expression_statement(inter, env, statement);		// 表达式语句
        break;
    case GLOBAL_STATEMENT:
        result = execute_global_statement(inter, env, statement);			// 带有global的语句
        break;
    case IF_STATEMENT:
        result = execute_if_statement(inter, env, statement);				// if语句
        break;
    case WHILE_STATEMENT:
        result = execute_while_statement(inter, env, statement);			// while语句
        break;
    case FOR_STATEMENT:
        result = execute_for_statement(inter, env, statement);				// for语句
        break;
    case RETURN_STATEMENT:
        result = execute_return_statement(inter, env, statement);			// return语句
        break;
    case BREAK_STATEMENT:
        result = execute_break_statement(inter, env, statement);			// break语句
        break;
    case CONTINUE_STATEMENT:
        result = execute_continue_statement(inter, env, statement);			// continue语句
        break;
    case STATEMENT_TYPE_COUNT_PLUS_1:   /* FALLTHRU */
    default:
        DBG_panic(("bad case...%d", statement->type));
    }

    return result;
}

// 执行语句链，开始的环境是空的
StatementResult ybx_execute_statement_list(YBX_Interpreter *inter, LocalEnvironment *env, StatementList *list)
{
    StatementList *pos;																		
    StatementResult result;																	//	语句执行的结果（注意是语句执行返回的类型）

    result.type = NORMAL_STATEMENT_RESULT;
    for (pos = list; pos; pos = pos->next) 
	{
        result = execute_statement(inter, env, pos->statement);			// 单条语句执行
        if (result.type != NORMAL_STATEMENT_RESULT)						// 执行中断（break、continue、return等），那么该条语句连接下来的语句全部不执行
            goto FUNC_END;
    }

  FUNC_END:
    return result;
}
