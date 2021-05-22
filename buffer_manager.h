/*
+--------------------------------------------------------------------------------------+
| �ļ�����: buffer_manager.h
| ��    ��: 1. ʹ��file_handle��block_handle���ļ��Ķ�д����ͬһ�����������������ݳ�Ա������
|						�������ļ��������ļ�ӳ�䣬ǰ����Ϊ�˷��㻺���Ĺ����ļ�ӳ�䷽���ļ��Ķ�д��
|						2. ��Ҫע��busy_blocks_��fhandle_������Ҫ����һ���ԡ�
|						3. �����滻ʱʹ���˼򵥵�LRU�㷨������δʹ�õĿ�ֱ���滻��û��ƽ��������д�������ܡ�
|						�����Ż�Ϊ����ѡ���û�ò��ҷ����			
| ��    ��: wang xin
| ��ϵ��ʽ: wangxinn@zju.edu.cn
| ��������: 2021/05/07 17:26
| �� �� ��: V1
| �޸���־: 
| ��Ȩ˵��: 2021, All right reserved.
+--------------------------------------------------------------------------------------+
*/

#pragma once
#include "block_handle.h"
#include "file_handle.h"
#include <map>
#include <set>

class BufferManager
{
private:
	// ���еĿ鱻�������������У�busy_blocks_���Ԫ��ͬʱ������fhandle_��
	// fhandle_��һ��map����<file, blocks>��ӳ��
	// �滻��ʱ����Ҫ��busy_blocks_���ҵ����ʹ�õģ�ͬʱ��Ҫ��fhandle_���Ƴ�

	BlockHandle* free_blocks_, *busy_blocks_; // free list and used list
	FileHandle* fhandle_;
	std::string path_; // db path

public:
	BufferManager(std::string p) : path_(p), 
		free_blocks_(new BlockHandle(p, BUFFER_SIZE)), busy_blocks_(new BlockHandle(p, 0)), fhandle_(new FileHandle(p)) {}
	~BufferManager() {
		delete free_blocks_;
		delete busy_blocks_;
	}

	BlockInfo* get_usable_block();
	BlockInfo* get_file_block(std::string db_name, std::string file_name, std::string type, int block_num);

	BlockHandle* free_blocks() { return free_blocks_; }
	BlockHandle* busy_blocks() { return busy_blocks_; }

	void write_block(BlockInfo* block);
	void write_into_disk();
};

