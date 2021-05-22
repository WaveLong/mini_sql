#include "api.h"
MiniDBAPI::MiniDBAPI(string path) {
	path_ = path;
	current_db_ = "";
	cm_ = new CatalogManager(path);
}
MiniDBAPI::~MiniDBAPI() {
	delete bm_;
	delete cm_;
}

void MiniDBAPI::quit() {
	delete bm_;
	delete cm_;
}
void MiniDBAPI::help() {
	cout << "MiniDB 1.0 by wangxin" << endl;
	cout << "SQL FUNCTIONS: " << endl;
	cout << "#HELP" << endl;
	cout << "#QUIT" << endl;
	cout << "#USE" << endl;
	cout << "#SHOW DATABASE" << endl;
	cout << "#SHOW TABLE" << endl;
	cout << "#CREATE DATABASE" << endl;
	cout << "#CREATE TABLE" << endl;
	cout << "#CREATE INDEX" << endl;
	cout << "#DROP DATABASE" << endl;
	cout << "#DROP TABLE" << endl;
	cout << "#DROP INDEX" << endl;
	cout << "#SELECT" << endl;
	cout << "#INSERT" << endl;
	cout << "#UPDATE" << endl;
	cout << "#DELETE" << endl;
	cout << "#EXCUTE FILE" << endl;
}
void MiniDBAPI::use(SQLUse& st) {
	Database *db = cm_->database_by_name(st.db_name());
	if (db == NULL) {
		throw DatabaseNotExistException();
	}

	if (current_db_ != "") {
		cout << "The old database is " << current_db_ << endl;
		cout << "Closing the old database..." << endl;
		cm_->write_archive_file();
		delete bm_;
	}
	current_db_ = st.db_name();
	bm_ = new BufferManager(path_);
}

void MiniDBAPI::create_database(SQLCreateDB& st) {
	string db_name = st.db_name();
	if (cm_->database_by_name(db_name) != NULL) {
		throw DatabaseAlreadyExistsException();
	}

	path folder_path = path(path_ + "//" + db_name);
	if (exists(folder_path)) {
		remove_all(folder_path);
		cout << "The existing database folder has been removed!" << endl;
	}

	create_directories(folder_path);
	cout << "Database folder has been created!" << endl;

	cm_->create_database(db_name);
	cm_->write_archive_file();
	cout << "Database has been created!" << endl;
}
void MiniDBAPI::create_table(SQLCreateTb& st) {
	if (current_db_.size() == 0) {
		throw NoDatabaseSelectedException();
	}

	Database* db = cm_->database_by_name(current_db_);
	if (db == NULL) {
		throw DatabaseNotExistException();
	}
	if (db->get_table(st.tb_name()) != NULL) {
		throw TableAlreadyExistsException();
	}

	string file_name = path_ + "//" + current_db_ + "//" + st.tb_name() + ".records";
	path p = path(file_name);
	if (exists(p)) {
		cout << "Table file already exists!" << endl;
		remove_all(p);
		cout << "The existing table file has been removed!" << endl;
	}

	db->create_table(st);
	cout << "Table has been created!" << endl;
	cm_->write_archive_file();
}
//void create_index();


void MiniDBAPI::insert(SQLInsert& st) {
	if (current_db_.size() == 0) {
		throw NoDatabaseSelectedException();
	}

	Database* db = cm_->database_by_name(current_db_);
	if (db == NULL) {
		throw DatabaseNotExistException();
	}
	if (db->get_table(st.tb_name()) == NULL) {
		throw TableNotExistException();
	}

	RecordManager* rm = new RecordManager(bm_, cm_, current_db_);
	rm->insert_value(st);
	delete rm;
}
void MiniDBAPI::select(SQLSelect& st) {
	if (current_db_.size() == 0) {
		throw NoDatabaseSelectedException();
	}

	Database* db = cm_->database_by_name(current_db_);
	if (db == NULL) {
		throw DatabaseNotExistException();
	}
	if (db->get_table(st.tb_name()) == NULL) {
		throw TableNotExistException();
	}

	RecordManager* rm = new RecordManager(bm_, cm_, current_db_);
	rm->select_value(st);
	delete rm;
}
void MiniDBAPI::update_record(SQLUpdate& st) {
	if (current_db_.size() == 0) {
		throw NoDatabaseSelectedException();
	}

	Database* db = cm_->database_by_name(current_db_);
	if (db == NULL) {
		throw DatabaseNotExistException();
	}
	if (db->get_table(st.tb_name()) == NULL) {
		throw TableNotExistException();
	}

	RecordManager* rm = new RecordManager(bm_, cm_, current_db_);
	rm->update_value(st);
	delete rm;
}
void MiniDBAPI::delete_record(SQLDelete& st) {
	if (current_db_ == "") {
		throw NoDatabaseSelectedException();
	}

	Database* db = cm_->database_by_name(current_db_);
	if (db == NULL) {
		throw DatabaseNotExistException();
	}
	if (db->get_table(st.tb_name()) == NULL) {
		throw TableNotExistException();
	}

	RecordManager* rm = new RecordManager(bm_, cm_, current_db_);
	rm->delete_value(st);
	delete rm;
}

void MiniDBAPI::show_database() {
	vector<Database*> dbs = cm_->database();
	cout << "DATABASE LIST:" << endl;
	for (auto db : dbs) {
		cout << "\t" << db->db_name() << endl;
	}
}
void MiniDBAPI::show_tables() {
	if (current_db_ == "") {
		throw NoDatabaseSelectedException();
	}
	vector<Table*> tbs = cm_->database_by_name(current_db_)->tables();
	cout << "TABLE LIST:" << endl;
	for (auto t : tbs) {
		cout << "\t" << t->table_name() << endl;
	}
}

void MiniDBAPI::drop_database(SQLDropDb& st) {
	string db = st.db_name();
	if (!cm_->delete_database(db)) {
		throw DatabaseNotExistException();
	}
	if (db == current_db_) {
		current_db_ = "";
		delete bm_;
	}
	cm_->write_archive_file();

	string folder = path_ + "//" + db;
	if (exists(folder)) {
		remove_all(folder);
		cout << "Database folder has been removed!" << endl;
	}
	else {
		cout << "Database folder doesn't exist! " << endl;
	}
}

void MiniDBAPI::drop_table(SQLDropTb& st) {
	if (current_db_ == "") {
		throw NoDatabaseSelectedException();
	}
	string name = st.tb_name();
	Database* db = cm_->database_by_name(current_db_);
	if (db->drop_table(name) == false) {
		throw TableNotExistException();
	}
	
	string file_path = path_ + "//" + current_db_ + "//" + name + ".records";
	if (exists(file_path)) {
		remove_all(file_path);
		cout << "Table file has been removed!" << endl;
	}
	else {
		cout << "Table file not exist!" << endl;
	}
	// 修改索引文件，暂时跳过

	cm_->write_archive_file();
}

void MiniDBAPI::exec_file(Interpreter *itp, SQLExec& st) {
	if (!exists(st.file_name())) {
		throw SQLInstructionFileNotExist();
	}
	string content;
	ifstream in(st.file_name(), ios::in | ios::binary);
	in.seekg(0, ios::end);
	content.resize(in.tellg());
	in.seekg(0, ios::beg);
	in.read(&content[0], content.size());
	in.close();

	vector<string>sqls = itp->split(content, ";");
	for (auto& s : sqls)itp->excute_sql(s);
}