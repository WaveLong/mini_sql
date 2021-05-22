/*
+--------------------------------------------------------------------------------------+
| �ļ�����: block_info.h
| ��    ��: �������Ϣ���ṩ�˶�ȡ��д��ĳ���ļ��Ŀ飬�����ݵ�д���߼���buffer_manager���
| ��    ��: wang xin
| ��ϵ��ʽ: wangxinn@zju.edu.cn
| ��������: 2021/05/07 17:24
| �� �� ��: V1
| �޸���־: 
| ��Ȩ˵��: 2021, All right reserved.
+--------------------------------------------------------------------------------------+
*/
#pragma once

# include <iostream>
# include "configuration.h"
# include "file_info.h"
class FileInfo;

class BlockInfo
{
private:
	FileInfo* file_; // �ÿ��������ļ�
	int block_num_; // �ÿ����ļ��еĿ��
	char* data_; // ���е�����

	// ���ڿ��滻�㷨��LRU�㷨
	// ����˫������ÿ�η��ʹ��Ŀ��������ͷ����������β��һ��������ʹ�õ�
	bool dirty_; // �Ƿ��޸Ĺ�
	BlockInfo* prev_, * next_;

public:
	BlockInfo(int num) : block_num_(num), file_ (NULL), dirty_(false), prev_(NULL), next_ (NULL) {
		data_ = new char[PAGE_SIZE];
		memset(data_, 0, PAGE_SIZE);
		set_record_count(0);
	}
	~BlockInfo() {
		delete []data_; 
	}

	// get and set
	FileInfo* file() { return file_; }
	void set_file(FileInfo* file) { file_ = file; }

	int block_num() { return block_num_; }
	void set_block_num(int num) { block_num_ = num; }

	char* data() { return data_; }

	bool dirty() { return dirty_; }
	void set_dirty(bool dt) { dirty_ = dt; }

	BlockInfo* next() { return next_; }
	void set_next(BlockInfo* next) { next_ = next; }

	BlockInfo* prev() { return prev_; }
	void set_prev(BlockInfo* prev) { prev_ = prev; }
	// ���������
	bool operator < (BlockInfo b) const { return block_num_ < b.block_num_; }
	bool operator == (BlockInfo b) const { return file_ == b.file_ && block_num_ == b.block_num_; }


	//  �������ݵ��Ų���
	//  ǰһ����ţ�4B��
	//  ��һ����ţ�4B��
	//  ���ڼ�¼������4B��
	//  ��¼����ʼ��ַΪ12B)

	int get_prev_block_num() { return *((int*)data_); }
	void set_prev_block_num(int num) { *((int*)data_) = num; }

	int get_next_block_num() { return *((int*)(data_ + ADRR_LEN)); }
	void set_next_block_num(int num) { *((int*)(data_ + ADRR_LEN)) = num; }

	int get_record_count() { return *((int*)(data_ + 2*ADRR_LEN)); }
	void set_record_count(int num) { *((int*)(data_ + 2*ADRR_LEN)) = num; }
	void increase_record_count() { *((int*)(data_ + 2*ADRR_LEN)) += 1; }
	void decrease_record_count() { *((int*)(data_ + 2*ADRR_LEN)) -= 1; }

	char* get_content_address() { return data_ + ADRR_BASE; }

	void read_info(std::string path);
	void write_info(std::string path);
};

