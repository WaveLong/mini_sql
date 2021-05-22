/*
+--------------------------------------------------------------------------------------+
| 文件名称: block_info.h
| 功    能: 块基本信息，提供了读取和写入某个文件的块，脏数据的写回逻辑由buffer_manager完成
| 作    者: wang xin
| 联系方式: wangxinn@zju.edu.cn
| 生成日期: 2021/05/07 17:24
| 版 本 号: V1
| 修改日志: 
| 版权说明: 2021, All right reserved.
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
	FileInfo* file_; // 该块所属的文件
	int block_num_; // 该块在文件中的块号
	char* data_; // 块中的数据

	// 用于块替换算法，LRU算法
	// 设置双向链表，每次访问过的块挂在链表头，挂在链表尾的一定是最少使用的
	bool dirty_; // 是否修改过
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
	// 重载运算符
	bool operator < (BlockInfo b) const { return block_num_ < b.block_num_; }
	bool operator == (BlockInfo b) const { return file_ == b.file_ && block_num_ == b.block_num_; }


	//  块中内容的排布：
	//  前一个块号（4B）
	//  下一个块号（4B）
	//  块内记录数量（4B）
	//  记录（起始地址为12B)

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

