/*
+--------------------------------------------------------------------------------------+
| �ļ�����: record_manager.h
| ��    ��: �����¼�Ķ�ȡ��д�룬��buffer_manager��������ʱ������index
| ��    ��: wang xin
| ��ϵ��ʽ: wangxinn@zju.edu.cn
| ��������: 2021/05/06 23:02
| �� �� ��: V1
| �޸���־: 
| ��Ȩ˵��: 2021, All right reserved.
+--------------------------------------------------------------------------------------+
*/
#pragma once
#include <iostream>
#include <iomanip>
#include <string>
#include "buffer_manager.h"
#include "catalog_manager.h"
#include "configuration.h"
#include "sql.h"

using namespace std;

class RecordManager
{
private:
	BufferManager* bm_;
	CatalogManager* cm_;
	string db_name_;

	BlockInfo* get_block_info(Table* t, int block_num);

	vector<Tkey*> construct_data(vector<string>& str_values, Table* t, int &pk);
	bool test_primary_key_confilct(Table* t, vector<Tkey*>& values, int pk);

	//vector<int> convert_to_index(vector<string>& cols, vector<Attribute*>& attrs);
	vector<Tkey*> get_record(Table* t, BlockInfo* bp, int offset);
	void delete_record(Table* t, BlockInfo* bp, int offset);
	void update_record(Table* t, BlockInfo* bp, int offset, vector<Tkey*>& values, vector<KV>& kvs);

	vector<int> convert_output_to_index(Table* t, vector<string>& output_cols);
	bool satisfy(Tkey& v1, Tkey& v2, WhereCondition& w);
	bool satisfy(Table* t, vector<Tkey*> values, vector<WhereCondition>& wheres);

public:
	RecordManager(BufferManager* bm, CatalogManager *cm, string path) : bm_(bm), cm_(cm), db_name_(path) {};
	~RecordManager() {};

	// �����޸�Ϊsql���
	void insert_value(SQLInsert& st);
	void delete_value(SQLDelete& st);
	void select_value(SQLSelect& st);
	void update_value(SQLUpdate& st);
};

