/*
+--------------------------------------------------------------------------------------+
| �ļ�����: block_handle.h
| ��    ��:	��˫���������ʽ������飬�ṩ��Ĳ��롢ɾ������
| ��    ��: wang xin
| ��ϵ��ʽ: wangxinn@zju.edu.cn
| ��������: 2021/05/07 17:23
| �� �� ��: V1
| �޸���־: 
| ��Ȩ˵��: 2021, All right reserved.
+--------------------------------------------------------------------------------------+
*/

#pragma once

#include "block_info.h"
#include "configuration.h"

class BlockHandle
{
private:
	BlockInfo* first_block_, *last_block_;
	int left_;
	std::string path_; // ���ݿ��ļ���ŵ�λ��
public:
	BlockHandle(std::string path, int left);
	~BlockHandle();

	void insert_into_head(BlockInfo* b);
	void remove_block(BlockInfo* b);
	BlockInfo* remove_from_tail();

	void write_into_disk();
};

