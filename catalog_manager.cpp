#include "catalog_manager.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <boost/filesystem.hpp>

// CalalogManager
CatalogManager::CatalogManager(std::string p) {
	path_ = p;
	read_archive_file();
}
CatalogManager::~CatalogManager() {
	write_archive_file();
}

Database* CatalogManager::database_by_name(std::string db_name) {
	for (auto db : dbs_) {
		if (db->db_name() == db_name)return db;
	}
	return NULL;
}

void CatalogManager::read_archive_file() {
	std::string file_name = path_ + "//" + "catalog";
	boost::filesystem::path file_path(file_name);
	
	if (boost::filesystem::exists(file_path)) {
		std::ifstream in(file_name, std::ios::binary | std::ios::in);
		boost::archive::binary_iarchive in_ar(in);
		in_ar >> (*this);
		in.close();
	}
}
void CatalogManager::write_archive_file() {
	std::string file_name = path_ + "//" + "catalog";
	std::ofstream out(file_name, std::ios::binary | std::ios::out);
	boost::archive::binary_oarchive out_ar(out);
	out_ar << (*this);
	out.close();
}

void CatalogManager::create_database(std::string db_name) {
	dbs_.push_back(new Database(db_name));
}
bool CatalogManager::delete_database(std::string db_name) {
	for (int i = 0; i < dbs_.size(); ++i) {
		if (dbs_[i]->db_name() == db_name) {
			dbs_.erase(dbs_.begin() + i);
			return true;
		}
	}
	return false;
}

// Database
Table* Database::get_table(std::string tb_name) {
	for (int i = 0; i < tbs_.size(); ++i) {
		if (tbs_[i]->table_name() == tb_name)return tbs_[i];
	}
	return NULL;
}

void Database::create_table(SQLCreateTb& st) {
	string tb_name = st.tb_name();
	vector<Attribute*>attrs = st.attributes();
	Table *t = new Table(tb_name);
	int len = 0;
	for (auto attr : attrs) {
		t->add_attribute(attr);
		len += attr->attr_length();
	}
	t->set_record_length(len);
	tbs_.push_back(t);
}

bool Database::drop_table(std::string tb_name) {
	for (int i = 0; i < tbs_.size(); ++i) {
		if (tbs_[i]->table_name() == tb_name) {
			tbs_.erase(tbs_.begin() + i);
			return true;
		}
	}
	return false;
}

bool Database::check_if_index_exist(std::string index_name) {
	return false;
}

// Table 
int Table::attribute_index(std::string attr_name) {
	for (int i = 0; i < attrs_.size(); ++i) {
		if (attrs_[i]->attribute_name() == attr_name) {
			return i;
		}
	}
	return -1;
}

