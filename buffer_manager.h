/*
+--------------------------------------------------------------------------------------+
| 文件名称: buffer_manager.h
| 功    能: 1. 使用file_handle和block_handle对文件的读写进行同一管理，包含了三个数据成员：空闲
|						块链表、文件块链表、文件映射，前两者为了方便缓存块的管理，文件映射方便文件的读写。
|						2. 需要注意busy_blocks_和fhandle_数据需要保持一致性。
|						3. 缓存替换时使用了简单的LRU算法，将最未使用的块直接替换，没有平衡脏数据写出的性能。
|						可以优化为优先选最久没用并且非脏块			
| 作    者: wang xin
| 联系方式: wangxinn@zju.edu.cn
| 生成日期: 2021/05/07 17:26
| 版 本 号: V1
| 修改日志: 
| 版权说明: 2021, All right reserved.
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
	// 所有的块被放在两个链表中，busy_blocks_里的元素同时被放在fhandle_里
	// fhandle_是一个map，即<file, blocks>的映射
	// 替换块时，需要从busy_blocks_里找到最不常使用的，同时需要从fhandle_里移除

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

