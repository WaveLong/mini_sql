/*
+--------------------------------------------------------------------------------------+
| 文件名称: file_handle.h
| 功    能: 管理文件的读写，主要功能为查询一个文件是否被读入、哪些块被读入、块的读入和删除、文件
|						的读入和删除、获取文件块、将所有的文件写回等
| 作    者: wang xin
| 联系方式: wangxinn@zju.edu.cn
| 生成日期: 2021/05/07 17:20
| 版 本 号: V1
| 修改日志: 
| 版权说明: 2021, All right reserved.
+--------------------------------------------------------------------------------------+
*/

#pragma once

#include <string>
#include <map>
#include <set>
#include "block_info.h"
#include "file_info.h"

using namespace std;

// 每个file的块中包含的信息如何写入？ prev/next/record_count
class FileHandle
{
private:
	map<FileInfo*, set<BlockInfo*>> filehandle_;
	map<string, FileInfo*> r_filehandle_;

	string path_; // 数据库文件所在文件夹

public:
	FileHandle(string& p) : path_(p) {};
	~FileHandle() {}

	string path() { return path_; }
	void set_path(string& st) { path_ = st; }

	map<FileInfo*, set<BlockInfo*>> get_file_handle() { return filehandle_; }

	void insert_file_block(BlockInfo* b);
	void insert_file(FileInfo* b);
	void insert_file(FileInfo* b, set<BlockInfo*>& blocks);

	void remove_file_block(BlockInfo* b);
	void remove_file(FileInfo* b);

	BlockInfo* get_file_block(string db_name, string file_name, string type, int block_num);
	BlockInfo* get_file_block(FileInfo* f, int block_num);
	FileInfo* get_file(string db_name, string file_name, string type);

	void write_into_disk();
};

