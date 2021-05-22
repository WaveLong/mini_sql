#include "file_handle.h"

void FileHandle::insert_file_block(BlockInfo* b) {
	filehandle_[b->file()].insert(b);
}
void FileHandle::insert_file(FileInfo* f) {
	r_filehandle_[f->total_name()] = f;
	filehandle_[f] = set<BlockInfo*>();
}
void FileHandle::insert_file(FileInfo* f, set<BlockInfo*>& blocks) {
	r_filehandle_[f->total_name()] = f;
	filehandle_[f] = blocks;
}

// ɾ������ֻ�ǽ����filehandle_���ó�����������д��
// ɾ���ļ��е�ĳ����
void FileHandle::remove_file_block(BlockInfo* b) {
	if (filehandle_.find(b->file()) == filehandle_.end() ||
		filehandle_[b->file()].find(b) == filehandle_[b->file()].end()) {
		return;
	}
	filehandle_[b->file()].erase(b);
	if (filehandle_[b->file()].empty()) {
		filehandle_.erase(b->file());
		r_filehandle_.erase(b->file()->total_name());
	}
}

// ɾ��һ���ļ�
void FileHandle::remove_file(FileInfo* f) {
	if (filehandle_.find(f) == filehandle_.end()) return;
	filehandle_.erase(f);
	r_filehandle_.erase(f->total_name());
}
// �����ļ���·������ȡ���е�һ��
BlockInfo* FileHandle::get_file_block(string db_name, string file_name, string type, int block_num) {
	// Ҳ���Ե��ú�����������
	string tmp = db_name;
	tmp += file_name;
	tmp += type;
	FileInfo* fp = r_filehandle_[tmp];
	if (fp == NULL)return NULL;
	auto blocks = filehandle_[fp];
	for (auto b : blocks) {
		if (block_num == b->block_num())return b;
	}
	return NULL;
}

BlockInfo* FileHandle::get_file_block(FileInfo* fp, int block_num) {
	if (fp == NULL)return NULL;
	auto blocks = filehandle_[fp];
	for (auto b : blocks) {
		if (block_num == b->block_num())return b;
	}
	return NULL;
}

FileInfo* FileHandle::get_file(string db_name, string file_name, string type) {
	string tmp = file_name;
	tmp += type;
	tmp += db_name;
	return r_filehandle_[tmp];
}

// �����е��ļ���д�ش���
void FileHandle::write_into_disk() {
	for (auto it : filehandle_) {
		for (auto b : it.second) {
			if (b->dirty()) {
				b->write_info(path_);
				b->set_dirty(false);
			}
		}
	}
}