/*
+--------------------------------------------------------------------------------------+
| 文件名称: catalog_manager.h
| 功    能: 管理表目录。数据层级为db->table->attribute/index，该模块只负责catalog文件的管理
|						数据类型：-2表示int， -1表示float，>=0 表示string，数值为其长度
| 作    者: wang xin
| 联系方式: wangxinn@zju.edu.cn
| 生成日期: 2021/05/07 18:56
| 版 本 号: V1
| 修改日志: 
| 版权说明: 2021, All right reserved.
+--------------------------------------------------------------------------------------+
*/

#pragma once
#include <vector>
#include <string>
#include <algorithm>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/vector.hpp>

#include "sql.h"

class Database;
class Table;
class Attribute;
class Index;
class SQLCreateTb;

class CatalogManager
{
private:
	friend class boost::serialization::access;
	template <class Archive> void serialize(Archive& ar, const unsigned int version) {
		ar& path_;
		ar& dbs_;
	}
	std::string path_;
	std::vector<Database*>dbs_;

public:
	CatalogManager(std::string p);
	~CatalogManager();

	std::vector<Database*> database() { return dbs_; }
	std::string path() { return path_; }

	Database* database_by_name(std::string db_name);

	void read_archive_file();
	void write_archive_file();

	void create_database(std::string db_name);
	bool delete_database(std::string db_name);
};

class Database {
private:
	// serialization
	friend class boost::serialization::access;
	template <class Archive> void serialize(Archive& ar, const unsigned int version) {
		ar& db_name_;
		ar& tbs_;
	}
	// data 
	std::string db_name_;
	std::vector<Table*> tbs_;

public:
	Database() {};
	Database(std::string db_name) : db_name_(db_name) {};
	~Database() {};

	std::string db_name() { return db_name_; }
	std::vector<Table*> tables() { return tbs_; }

	Table* get_table(std::string tb_name);

	void create_table(SQLCreateTb& st);
	bool drop_table(std::string tb_name);
	//void drop_index();

	bool check_if_index_exist(std::string index_name);
};
class Table {
private:
	friend class boost::serialization::access;
	template <class Archive> void serialize(Archive& ar, const unsigned int version) {
		ar& tb_name_;
		ar& record_length_;
		ar& block_count_;
		ar& first_block_num_;
		ar& first_rubbish_num_;
		ar& attrs_;
		ar& indexs_;
	}

	std::string tb_name_;
	int record_length_, block_count_; // 记录的长度、文件块的数量
	int first_block_num_, first_rubbish_num_; // 第一个使用了的块号、第一个空洞块号
	std::vector<Attribute*>attrs_;
	std::vector<Index*>indexs_;

public:
	Table() : tb_name_(""), record_length_(-1), block_count_(0), first_block_num_(-1), first_rubbish_num_(-1) {};
	Table(std::string s) : tb_name_(s), record_length_(-1), block_count_(0), first_block_num_(-1), first_rubbish_num_(-1) {};
	~Table() {};

	std::string table_name() { return tb_name_; }
	void set_table_name(std::string tb_name) { tb_name_ = tb_name; }

	int record_length() { return record_length_; }
	void set_record_length(int l) { record_length_ = l; }

	int block_count() { return block_count_; }
	void set_block_count(int n) { block_count_ = n; }
	void increase_block_count() { block_count_++; }
	void decrease_block_count() { block_count_--; }

	int first_block_num() { return first_block_num_; }
	void set_first_block_num(int n) { first_block_num_ = n; }

	int first_rubbish_num() { return first_rubbish_num_; }
	void set_first_rubbish_num(int n) { first_rubbish_num_ = n; }

	int attribute_count() { return attrs_.size(); }
	void add_attribute(Attribute* arr) { attrs_.push_back(arr); }
	int attribute_index(std::string attr_name);
	Attribute* attribute(std::string attr_name) { return attrs_[attribute_index(attr_name)]; }
	Attribute* attribute(int id) { return attrs_[id]; }
	//vector<Attribute*>& attributes() { return attrs_; }

	Index* index(int id) { return indexs_[id]; }
	void add_index(Index* id) { indexs_.push_back(id); }
};

class Attribute {
private:
	friend class boost::serialization::access;
	template <class Archive> void serialize(Archive& ar, const unsigned int version) {
		ar& attr_name_;
		ar& data_type_;
		ar& attr_length_;
		ar& is_primary_key_;
	}
	std::string attr_name_;
	int data_type_, attr_length_;
	bool is_primary_key_;

public:
	Attribute() :attr_name_(""), data_type_(-3), attr_length_(-1), is_primary_key_(false) {};
	Attribute(std::string name, int type, int len, bool b) :
		attr_name_(name), data_type_(type), attr_length_(len), is_primary_key_(b) {}
	~Attribute() {};

	std::string attribute_name() { return attr_name_; }
	void set_attribute_name(std::string name) { attr_name_ = name; }

	int data_type() { return data_type_; }
	void set_data_type(int type) { data_type_ = type; }

	int attr_length() { return attr_length_; }
	void set_attr_length(int l) { attr_length_ = l; }

	bool is_primary_key() { return is_primary_key_; }
	void set_primary_key(bool v) { is_primary_key_ = v; }
};

class Index {
private:
	friend class boost::serialization::access;
	template <class Archive> void serialize(Archive& ar, const unsigned int version) {}
};
