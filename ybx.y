/* Copyright (c) 2019,YBX is the most handsome man in NCEPU 
/* All rights reserved.
/*
/* 文件名称: ybx.y
/* 文件标识: 见README.md
/* 摘要: 分析树的构建
/*
/* 当前版本: 1.1
/* 作者: 杨秉学
/* 完成日期: 2019年11月17日
/*
/* 取代版本:1.0
/* 原作者: 杨秉学
/* 完成日期: 2018年5月2日      */

%{
#include <stdio.h>
#include "ybxcompiler.h"
#define YYDEBUG 1
%}
%union {
    char                *identifier;
    ParameterList       *parameter_list;
    ArgumentList        *argument_list;
    Expression          *expression;
    Statement           *statement;
    StatementList       *statement_list;
    Block               *block;
    Elsif               *elsif;
    IdentifierList      *identifier_list;
}
/*%token%定义的都是终结符 ，下面这几个都是状态*/
%token <expression>     INT_LITERAL
%token <expression>     DOUBLE_LITERAL
%token <expression>     STRING_LITERAL
%token <identifier>     IDENTIFIER

/*定义 函数、if else 等终结符*/
%token FUNCTION IF ELSE ELSIF WHILE FOR RETURN_T BREAK CONTINUE NULL_T
        LP RP LC RC SEMICOLON COMMA ASSIGN LOGICAL_AND LOGICAL_OR
        EQ NE GT GE LT LE ADD SUB MUL DIV MOD TRUE_T FALSE_T GLOBAL_T
		
		
/*%type%定义的都是非终结符*/
%type   <parameter_list> parameter_list
%type   <argument_list> argument_list
%type   <expression> expression expression_opt
        logical_and_expression logical_or_expression
        equality_expression relational_expression
        additive_expression multiplicative_expression
        unary_expression primary_expression
%type   <statement> statement global_statement
        if_statement while_statement for_statement
        return_statement break_statement continue_statement
%type   <statement_list> statement_list
%type   <block> block
%type   <elsif> elsif elsif_list
%type   <identifier_list> identifier_list


%%

translation_unit
        : definition_or_statement
        | translation_unit definition_or_statement
        ;
definition_or_statement
        : function_definition
        | statement
        {
            YBX_Interpreter *inter = ybx_get_current_interpreter();

            inter->statement_list
                = ybx_chain_statement_list(inter->statement_list, $1);
        }
        ;
function_definition
        : FUNCTION IDENTIFIER LP parameter_list RP block
        {
            ybx_function_define($2, $4, $6);
        }
        | FUNCTION IDENTIFIER LP RP block
        {
            ybx_function_define($2, NULL, $5);
        }
        ;
parameter_list
        : IDENTIFIER
        {
            $$ = ybx_create_parameter($1);
        }
        | parameter_list COMMA IDENTIFIER
        {
            $$ = ybx_chain_parameter($1, $3);
        }
        ;
argument_list
        : expression
        {
            $$ = ybx_create_argument_list($1);
        }
        | argument_list COMMA expression
        {
            $$ = ybx_chain_argument_list($1, $3);
        }
        ;
statement_list
        : statement
        {
            $$ = ybx_create_statement_list($1);
        }
        | statement_list statement
        {
            $$ = ybx_chain_statement_list($1, $2);
        }
        ;
expression
        : logical_or_expression
        | IDENTIFIER ASSIGN expression
        {
            $$ = ybx_create_assign_expression($1, $3);
        }
        ;
logical_or_expression
        : logical_and_expression
        | logical_or_expression LOGICAL_OR logical_and_expression
        {
            $$ = ybx_create_binary_expression(LOGICAL_OR_EXPRESSION, $1, $3);
        }
        ;
logical_and_expression
        : equality_expression
        | logical_and_expression LOGICAL_AND equality_expression
        {
            $$ = ybx_create_binary_expression(LOGICAL_AND_EXPRESSION, $1, $3);
        }
        ;
equality_expression
        : relational_expression
        | equality_expression EQ relational_expression
        {
            $$ = ybx_create_binary_expression(EQ_EXPRESSION, $1, $3);
        }
        | equality_expression NE relational_expression
        {
            $$ = ybx_create_binary_expression(NE_EXPRESSION, $1, $3);
        }
        ;
relational_expression
        : additive_expression
        | relational_expression GT additive_expression
        {
            $$ = ybx_create_binary_expression(GT_EXPRESSION, $1, $3);
        }
        | relational_expression GE additive_expression
        {
            $$ = ybx_create_binary_expression(GE_EXPRESSION, $1, $3);
        }
        | relational_expression LT additive_expression
        {
            $$ = ybx_create_binary_expression(LT_EXPRESSION, $1, $3);
        }
        | relational_expression LE additive_expression
        {
            $$ = ybx_create_binary_expression(LE_EXPRESSION, $1, $3);
        }
        ;
additive_expression
        : multiplicative_expression
        | additive_expression ADD multiplicative_expression
        {
            $$ = ybx_create_binary_expression(ADD_EXPRESSION, $1, $3);
        }
        | additive_expression SUB multiplicative_expression
        {
            $$ = ybx_create_binary_expression(SUB_EXPRESSION, $1, $3);
        }
        ;
multiplicative_expression
        : unary_expression
        | multiplicative_expression MUL unary_expression
        {
            $$ = ybx_create_binary_expression(MUL_EXPRESSION, $1, $3);
        }
        | multiplicative_expression DIV unary_expression
        {
            $$ = ybx_create_binary_expression(DIV_EXPRESSION, $1, $3);
        }
        | multiplicative_expression MOD unary_expression
        {
            $$ = ybx_create_binary_expression(MOD_EXPRESSION, $1, $3);
        }
        ;
unary_expression
        : primary_expression
        | SUB unary_expression
        {
            $$ = ybx_create_minus_expression($2);
        }
        ;
primary_expression
        : IDENTIFIER LP argument_list RP
        {
            $$ = ybx_create_function_call_expression($1, $3);
        }
        | IDENTIFIER LP RP
        {
            $$ = ybx_create_function_call_expression($1, NULL);
        }
        | LP expression RP
        {
            $$ = $2;
        }
        | IDENTIFIER
        {
            $$ = ybx_create_identifier_expression($1);
        }
        | INT_LITERAL
        | DOUBLE_LITERAL
        | STRING_LITERAL
        | TRUE_T
        {
            $$ = ybx_create_boolean_expression(YBX_TRUE);
        }
        | FALSE_T
        {
            $$ = ybx_create_boolean_expression(YBX_FALSE);
        }
        | NULL_T
        {
            $$ = ybx_create_null_expression();
        }
        ;
statement
        : expression SEMICOLON
        {
          $$ = ybx_create_expression_statement($1);
        }
        | global_statement
        | if_statement
        | while_statement
        | for_statement
        | return_statement
        | break_statement
        | continue_statement
        ;
global_statement
        : GLOBAL_T identifier_list SEMICOLON
        {
            $$ = ybx_create_global_statement($2);
        }
        ;
identifier_list
        : IDENTIFIER
        {
            $$ = ybx_create_global_identifier($1);
        }
        | identifier_list COMMA IDENTIFIER
        {
            $$ = ybx_chain_identifier($1, $3);
        }
        ;
if_statement
        : IF LP expression RP block
        {
            $$ = ybx_create_if_statement($3, $5, NULL, NULL);
        }
        | IF LP expression RP block ELSE block
        {
            $$ = ybx_create_if_statement($3, $5, NULL, $7);
        }
        | IF LP expression RP block elsif_list
        {
            $$ = ybx_create_if_statement($3, $5, $6, NULL);
        }
        | IF LP expression RP block elsif_list ELSE block
        {
            $$ = ybx_create_if_statement($3, $5, $6, $8);
        }
        ;
elsif_list
        : elsif
        | elsif_list elsif
        {
            $$ = ybx_chain_elsif_list($1, $2);
        }
        ;
elsif
        : ELSIF LP expression RP block
        {
            $$ = ybx_create_elsif($3, $5);
        }
        ;
while_statement
        : WHILE LP expression RP block
        {
            $$ = ybx_create_while_statement($3, $5);
        }
        ;
for_statement
        : FOR LP expression_opt SEMICOLON expression_opt SEMICOLON
          expression_opt RP block
        {
            $$ = ybx_create_for_statement($3, $5, $7, $9);
        }
        ;
expression_opt
        : /* empty */
        {
            $$ = NULL;
        }
        | expression
        ;
return_statement
        : RETURN_T expression_opt SEMICOLON
        {
            $$ = ybx_create_return_statement($2);
        }
        ;
break_statement
        : BREAK SEMICOLON
        {
            $$ = ybx_create_break_statement();
        }
        ;
continue_statement
        : CONTINUE SEMICOLON
        {
            $$ = ybx_create_continue_statement();
        }
        ;
block
        : LC statement_list RC
        {
            $$ = ybx_create_block($2);
        }
        | LC RC
        {
            $$ = ybx_create_block(NULL);
        }
        ;
%%
