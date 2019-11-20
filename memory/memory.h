#ifndef PRIVATE_MEM_H_INCLUDED
#define PRIVATE_MEM_H_INCLUDED
#include "../MEM.h"

typedef union Header_tag Header;

// �ڴ������
struct MEM_Controller_tag {
    FILE        *error_fp;							// �����ڴ�й¶��Խ��ȣ�����ļ�
    MEM_ErrorHandler    error_handler;	// ������
    MEM_FailMode        fail_mode;			// �ڴ����ʱ�Ĵ�����ʽ
    Header      *block_header;					// �������ڴ���б�
};
#endif /* PRIVATE_MEM_H_INCLUDED */

