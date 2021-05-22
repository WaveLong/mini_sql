/*
+--------------------------------------------------------------------------------------+
| �ļ�����: file_handle.h
| ��    ��: �����ļ��Ķ�д����Ҫ����Ϊ��ѯһ���ļ��Ƿ񱻶��롢��Щ�鱻���롢��Ķ����ɾ�����ļ�
|						�Ķ����ɾ������ȡ�ļ��顢�����е��ļ�д�ص�
| ��    ��: wang xin
| ��ϵ��ʽ: wangxinn@zju.edu.cn
| ��������: 2021/05/07 17:20
| �� �� ��: V1
| �޸���־: 
| ��Ȩ˵��: 2021, All right reserved.
+--------------------------------------------------------------------------------------+
*/

#pragma once

#include <string>
#include <map>
#include <set>
#include "block_info.h"
#include "file_info.h"

using namespace std;

// ÿ��file�Ŀ��а�������Ϣ���д�룿 prev/next/record_count
class FileHandle
{
private:
	map<FileInfo*, set<BlockInfo*>> filehandle_;
	map<string, FileInfo*> r_filehandle_;

	string path_; // ���ݿ��ļ������ļ���

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

