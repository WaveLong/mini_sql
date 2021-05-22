#pragma once
#include <string>
#include <map>
#include "block_info.h"

using namespace std;
class FileInfo
{
private:
	string db_name_, file_name_;
	string type_; // INDEX/RECORDS
	string total_name_;

	int records_amount_;
	int records_len_;

public:
	FileInfo() : db_name_(""), file_name_(""), type_(""), total_name_(""), records_amount_(0), records_len_(0) {}
	FileInfo(string db_name, string file_name, string type, int amount, int len) :
		db_name_(db_name), file_name_(file_name), type_(type), records_amount_(amount), records_len_(len) {
		total_name_ = file_name_;
		total_name_ += type_;
		total_name_ += db_name_;
	}
	~FileInfo() {}

	string db_name() { return db_name_; }
	void set_db_name(string& st) { db_name_ = st; }

	string file_name() { return file_name_; }
	void set_file_name(string& st) { file_name_ = st; }

	string type() { return type_; }
	void set_type(string& st) { type_ = st; }

	string total_name() { return total_name_; }

	bool operator == (FileInfo f) const { return total_name_ == f.total_name_; }
	bool operator < (FileInfo f) const { return total_name_ == f.total_name_; }
};

