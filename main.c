/*
* Copyright (c) 2019,YBX is the most handsome man in NCEPU
* All rights reserved.
*
* 文件名称: main.c
* 文件标识: 见README.md
* 摘要: 
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
#include "YBX.h"
#include "MEM.h"

int	main(int argc, char **argv)
{
	YBX_Interpreter *interpreter;

	FILE *fp;
	//支持交互式
	if(argc == 1)
	{
		printf("ybx 1.1.0\n");
		printf("Copyright (c) 2019,Welcome to use ybx compiler\n");
		printf("---------------------------------------------------------\n");
		fp = stdin;
	}

	//支持文本输入
	if(argc == 2)
	{
		fp = fopen(argv[1], "r");
		if (fp == NULL) 
		{
			fprintf(stderr, "%s not found.\n", argv[1]);
			exit(1);
		}
	}

	//如果不符合规范报错
	if (argc >= 3) 
	{
		fprintf(stderr, "usage:%s <filename>", argv[0]);
		printf("\n");
		exit(1);
	}

	interpreter = YBX_create_interpreter();				//创建解释器
	YBX_compile(interpreter, fp);						//编译,生成分析树
	YBX_interpret(interpreter);							//解释
	YBX_dispose_interpreter(interpreter);				//释放解释器

	MEM_dump_blocks(stdout);							// 检查内存是否释放

	return 0;
}
