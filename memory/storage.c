/*
* Copyright (c) 2019,YBX is the most handsome man in NCEPU
* All rights reserved.
*
* 文件名称: storage.c
* 文件标识: 见README.md
* 摘要: 可以支持C语言调用
*      面向普通用户的头文件
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
#include <stdlib.h>
#include <assert.h>
#include "memory.h"

// �ڴ��
typedef union {
	long        l_dummy;
	double      d_dummy;
	void        *p_dummy;
} Cell;

#define CELL_SIZE               (sizeof(Cell))
#define DEFAULT_PAGE_SIZE       (1024)  	// cell num 

typedef struct MemoryPage_tag MemoryPage;
typedef MemoryPage *MemoryPageList;

// �ڴ�ҳ
struct MemoryPage_tag {
	int                 cell_num;				// �ڴ�����
	int                 use_cell_num;		// ��ʹ�õ��ڴ�����
	MemoryPageList      next;			// ��һ���ڴ�ҳ
	Cell                cell[1];					// ��ʼ�ڴ��ֻ��һ��
};

// �洢��
struct MEM_Storage_tag {
	MemoryPageList      page_list;		//�ڴ�ҳ��
	int                 current_page_size;	// ҳ��С
};

#define larger(a, b) (((a) > (b)) ? (a) : (b))

// 打开创建一个存储器	
MEM_Storage	MEM_open_storage_func(MEM_Controller controller,
								  char *filename, int line, int page_size)
{
	MEM_Storage storage;

	storage = MEM_malloc_func(controller, filename, line,sizeof(struct MEM_Storage_tag));		// ����һ���洢���ռ�
	storage->page_list = NULL;																									// ��ʱ�洢���е�ҳ�ǿյ�
	assert(page_size >= 0);
	if (page_size > 0)
	{
		storage->current_page_size = page_size;
	} 
	else 
	{
		storage->current_page_size = DEFAULT_PAGE_SIZE;
	}

	return storage;
}

// 从存储器中分配内存	
void* MEM_storage_malloc_func(MEM_Controller controller,
	char *filename, int line, MEM_Storage storage,
	size_t size)
{
	int							cell_num;
	MemoryPage *			new_page;
	void*						p;

	cell_num = ((size - 1) / CELL_SIZE) + 1;								// ����������ڴ�����

	if (storage->page_list != NULL &&  (storage->page_list->use_cell_num + cell_num< storage->page_list->cell_num)) 
	{
		// �洢�����ڴ�ҳ�л��п��е��ڴ��
		p = &(storage->page_list->cell[storage->page_list->use_cell_num]);
		storage->page_list->use_cell_num += cell_num;
	} 
	else
	{
		// �洢����û�п��е��ڴ��
		int alloc_cell_num;

		alloc_cell_num = larger(cell_num, storage->current_page_size);

		new_page = MEM_malloc_func(controller, filename, line, sizeof(MemoryPage) + CELL_SIZE * (alloc_cell_num - 1));
		new_page->next = storage->page_list;
		new_page->cell_num = alloc_cell_num;
		storage->page_list = new_page;

		p = &(new_page->cell[0]);
		new_page->use_cell_num = cell_num;
	}

	return p;
}

void
	MEM_dispose_storage_func(MEM_Controller controller, MEM_Storage storage)
{
	MemoryPage  *temp;

	while (storage->page_list) {
		temp = storage->page_list->next;
		MEM_free_func(controller, storage->page_list);
		storage->page_list = temp;
	}
	MEM_free_func(controller, storage);
}
