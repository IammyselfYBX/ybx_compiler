/* Copyright (c) 2019,YBX is the most handsome man in NCEPU 
/* All rights reserved.
/*
/* 文件名称: ybx.l
/* 文件标识: 见README.md
/* 摘要: 词法分析
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
#include <string.h>
#include "DBG.h"
#include "ybxcompiler.h"
#include "y.tab.h"

/* 一个功能性函数——习惯性用法：不能少 */
int yywrap(void)
{
    return 1;
}

/* 行编号增加 */
static void increment_line_number(void)
{
    ybx_get_current_interpreter()->current_line_number++;
}
%}

%start COMMENT STRING_LITERAL_STATE

%%
<INITIAL>"function"     return FUNCTION;
<INITIAL>"if"           return IF;
<INITIAL>"else"         return ELSE;
<INITIAL>"elsif"        return ELSIF;
<INITIAL>"while"        return WHILE;
<INITIAL>"for"          return FOR;
<INITIAL>"return"       return RETURN_T;
<INITIAL>"break"        return BREAK;
<INITIAL>"continue"     return CONTINUE;
<INITIAL>"null"         return NULL_T;
<INITIAL>"true"         return TRUE_T;
<INITIAL>"false"        return FALSE_T;
<INITIAL>"global"       return GLOBAL_T;
<INITIAL>"("            return LP;
<INITIAL>")"            return RP;
<INITIAL>"{"            return LC;
<INITIAL>"}"            return RC;
<INITIAL>";"            return SEMICOLON;
<INITIAL>","            return COMMA;
<INITIAL>"&&"           return LOGICAL_AND;
<INITIAL>"||"           return LOGICAL_OR;
<INITIAL>"="            return ASSIGN;
<INITIAL>"=="           return EQ;
<INITIAL>"!="           return NE;
<INITIAL>">"            return GT;
<INITIAL>">="           return GE;
<INITIAL>"<"            return LT;
<INITIAL>"<="           return LE;
<INITIAL>"+"            return ADD;
<INITIAL>"-"            return SUB;
<INITIAL>"*"            return MUL;
<INITIAL>"/"            return DIV;
<INITIAL>"%"            return MOD;
<INITIAL>[A-Za-z_][A-Za-z_0-9]* {
    yylval.identifier = ybx_create_identifier(yytext);
    return IDENTIFIER;
}

<INITIAL>([1-9][0-9]*)|"0" {
    Expression  *expression = ybx_alloc_expression(INT_EXPRESSION);
    sscanf(yytext, "%d", &expression->u.int_value);
    yylval.expression = expression;
    return INT_LITERAL;
}

<INITIAL>[0-9]+\.[0-9]+ {
    Expression  *expression = ybx_alloc_expression(DOUBLE_EXPRESSION);
    sscanf(yytext, "%lf", &expression->u.double_value);
    yylval.expression = expression;
    return DOUBLE_LITERAL;
}
<INITIAL>\" {
    ybx_open_string_literal();
    BEGIN STRING_LITERAL_STATE;
}
<INITIAL>[ \t] ;
<INITIAL>\n {increment_line_number();}
<INITIAL>#     BEGIN COMMENT;             /* 如果有#开头，转换到<COMMENT> */
<INITIAL>.      {
    char buf[LINE_BUF_SIZE];

    if (isprint(yytext[0])) {
        buf[0] = yytext[0];
        buf[1] = '\0';
    } else {
        sprintf(buf, "0x%02x", (unsigned char)yytext[0]);
    }

    ybx_compile_error(CHARACTER_INVALID_ERR,
                      STRING_MESSAGE_ARGUMENT, "bad_char", buf,
                      MESSAGE_ARGUMENT_END);
}

 /* 接下来的<COMMENT>模块表示遇到换行符 \n 执行的操作：丢弃该行，病切换回 <INITIAL> */
<COMMENT>\n     {                      
    increment_line_number(); 
    BEGIN INITIAL;
}
<COMMENT>.      ;
<STRING_LITERAL_STATE>\"        {
    Expression *expression = ybx_alloc_expression(STRING_EXPRESSION);
    expression->u.string_value = ybx_close_string_literal();
    yylval.expression = expression;
    BEGIN INITIAL;
    return STRING_LITERAL;
}
<STRING_LITERAL_STATE>\n        {
    ybx_add_string_literal('\n');
    increment_line_number();
}
<STRING_LITERAL_STATE>\\\"      ybx_add_string_literal('"');
<STRING_LITERAL_STATE>\\n       ybx_add_string_literal('\n');
<STRING_LITERAL_STATE>\\t       ybx_add_string_literal('\t');
<STRING_LITERAL_STATE>\\\\      ybx_add_string_literal('\\');
<STRING_LITERAL_STATE>.         ybx_add_string_literal(yytext[0]);
%%
