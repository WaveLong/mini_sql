/*
+--------------------------------------------------------------------------------------+
| 文件名称: block_handle.h
| 功    能:	以双向链表的形式管理缓存块，提供块的插入、删除功能
| 作    者: wang xin
| 联系方式: wangxinn@zju.edu.cn
| 生成日期: 2021/05/07 17:23
| 版 本 号: V1
| 修改日志: 
| 版权说明: 2021, All right reserved.
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
	std::string path_; // 数据库文件存放的位置
public:
	BlockHandle(std::string path, int left);
	~BlockHandle();

	void insert_into_head(BlockInfo* b);
	void remove_block(BlockInfo* b);
	BlockInfo* remove_from_tail();

	void write_into_disk();
};

