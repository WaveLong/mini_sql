/*
+---------------------------------------------------------------------------------------+
| �ļ�����: api.h													   	
| ��    ��: API�㣬ִ��Interpreter��������SQL���																			
| ��    ��: wang xin																		
| ��ϵ��ʽ: wangxinn@zju.edu.cn															
| ��������: 2021/05/12 15:58														
| �� �� ��: V1																			
| �޸���־: 																				
| ��Ȩ˵��: 2021, All right reserved.													
+---------------------------------------------------------------------------------------+
*/

#pragma once
#include <string>
#include <fstream>
#include <filesystem>
#include "buffer_manager.h"
#include "record_manager.h"
#include "catalog_manager.h"
#include "interpreter.h"
#include "sql.h"

using namespace std;
using namespace std::filesystem;
class Interpreter;
class MiniDBAPI
{
private:
	string path_;
	string current_db_;
	BufferManager* bm_;
	CatalogManager* cm_;
public:
	MiniDBAPI(string path);
	~MiniDBAPI();
	void quit();
	void help();
	void use(SQLUse &st);

	void create_database(SQLCreateDB& st);
	void create_table(SQLCreateTb& st);
	//void create_index();

	void drop_database(SQLDropDb &st);
	void drop_table(SQLDropTb &st);
	//void drop_index();

	void insert(SQLInsert& st);
	void select(SQLSelect& st);
	void update_record(SQLUpdate& st);
	void delete_record(SQLDelete& st);

	void show_database();
	void show_tables();

	void exec_file(Interpreter* itp, SQLExec& st);
};

