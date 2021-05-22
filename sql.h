/*
+--------------------------------------------------------------------------------------+
| 文件名称: sql.h
| 功    能: 定义了SQL语句的结构，包括：Key, CreateDatabase, CreateTable, CreateIndex, 
|						DropDatabase, DropTable, DropIndex, Delete, Insert, Update, Select, Use
|						create: 0 - database 0, table 1, index 2
|						drop :	10 - database 10, table 11, index 12
|						delete: 20
|						insert: 30
|						update: 40
|						select: 50
|						use		: 60
|						quit  : 70
|						help  : 80
|						show  : 90 -database 90, table 91
|						exec  : 100
| 作    者: wang xin
| 联系方式: wangxinn@zju.edu.cn
| 生成日期: 2021/05/08 19:11
| 版 本 号: V1
| 修改日志: 
| 版权说明: 2021, All right reserved.
+--------------------------------------------------------------------------------------+
*/

#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "catalog_manager.h"
#include "exceptions.h"
#include "configuration.h"
//#include "commons.h"

#define EMPTY -1

#define CRT_DB 0
#define CRT_TB 1
#define CRT_IDX 2

#define DP_DB 10
#define DP_TB 11
#define DP_IDX 12

#define DEL 20
#define IN 30
#define UP 40
#define SL 50

#define USE 60
#define QUIT 70
#define HELP 80

#define SH_DB 90
#define SH_TB 91

#define EXEC 100

using namespace std;

class Database;
class Table;
class Attribute;
class Index;

class Tkey
{
private:
	int key_type_; //-2: int; -1: float; >= 0 : string，数值为长度
	int key_length_;
	char* key_;
public:
	// constructor
	Tkey(int type);
	Tkey(const Tkey& k);
	~Tkey();

	void read_value(const char* content);
	void read_value(const string& s);

	int key_type() { return key_type_; }
	int key_length() { return key_length_; }
	char* key() { return key_; }

	bool operator == (Tkey& t) const;
	bool operator != (Tkey& t) const;
	bool operator < (Tkey& t) const;
	bool operator <= (Tkey& t) const;
	bool operator > (Tkey& t) const;
	bool operator >= (Tkey& t) const;

	friend ostream& operator<<(ostream& out, Tkey& object);
};

// 将查询语句转换为语句中对应的函数的参数
// 如：select语句就包含了 table, where, output_column
// 所有语句的输入都是分词之后的string向量
class SQL
{
protected:
	int sql_type_;
public:
	SQL() : sql_type_(-1) {};
	SQL(int t) : sql_type_(t) {};
	virtual ~SQL() {};

	int sql_type() { return sql_type_; }
	void set_sql_type(int t) { sql_type_ = t; }

	virtual void parse(vector<string>& sql_vec) = 0;
	int parse_data_type(vector<string> sql_vec, Attribute* attr, int pos);
};

// use #db_name
class SQLUse :public SQL
{
private:
	string db_name_;
public:
	SQLUse(vector<string>& sql_vec) { parse(sql_vec); }
	void parse(vector<string>& sql_vec);
	
	string db_name() { return db_name_; }
};

// create database #db_name;
class SQLCreateDB : public SQL
{
private:
	string db_name_;
public:
	SQLCreateDB(vector<string>& sql_vec) { parse(sql_vec); }

	void parse(vector<string>& sql_vec);

	string db_name() { return db_name_; }
};

// create table #table_name ( col1 type, col2 type, ..., primary key colk );
class SQLCreateTb : public SQL
{
private:
	string tb_name_;
	vector<Attribute*>attrs_;
public:
	SQLCreateTb(vector<string>& sql_vec) { parse(sql_vec); }
	void parse(vector<string>& sql_vec);

	string tb_name() { return tb_name_; }
	vector<Attribute*> attributes() { return attrs_; }
};

// create index on #tb_name named #index_name;

// insert into #tb_name values (v1, v2, v3, ...)
class SQLInsert :public SQL 
{
private:
	string tb_name_;
	//vector<Tkey*>values_; // 如何解析数据类型？？
	vector<string> sql_value_;
public:
	SQLInsert(vector<string>& sql_vec) { parse(sql_vec); }
	void parse(vector<string>& sql_vec);

	string tb_name() { return tb_name_; }
	vector<string> &sql_value() { return sql_value_; }
};

typedef struct {
	string key_;
	int condition_;
	string value_;
} WhereCondition;

// select #cols from #tb_name where #where condition
class SQLSelect : public SQL
{
private:
	string tb_name_;
	vector<WhereCondition> wheres_;
	vector<string> cols_;
	
public:
	SQLSelect(vector<string>& sql_vec) { parse(sql_vec); }
	void parse(vector<string>& sql_vec);

	string tb_name() { return tb_name_; }
	vector<string>& output_cols() { return cols_; }
	vector<WhereCondition>& where_condition() { return wheres_; }
};

// delete from #tb_name where #where condition
class SQLDelete :public SQL
{
private:
	string tb_name_;
	vector<WhereCondition> wheres_;
public:
	SQLDelete(vector<string>& sql_vec) { parse(sql_vec); }
	void parse(vector<string>& sql_vec);

	string tb_name() { return tb_name_; }
	vector<WhereCondition>& where_condition() { return wheres_; }
};

// update #tb_name set col1 = v1, col2 = v2 ... where #condition
typedef struct {
	string k_;
	string v_;
} KV;
class SQLUpdate :public SQL
{
private:
	string tb_name_;
	vector<WhereCondition> wheres_;
	vector<KV> kvs_;
public:
	SQLUpdate(vector<string>& sql_vec) { parse(sql_vec); }
	void parse(vector<string>& sql_vec);

	string tb_name() { return tb_name_; }
	vector<WhereCondition>& where_condition() { return wheres_; }
	vector<KV>& kvs() { return kvs_; }
};

//drop database #db_name
class SQLDropDb :public SQL
{
private:
	string db_name_;

public:
	SQLDropDb(vector<string>& sql_vec) { parse(sql_vec); }
	void parse(vector<string>& sql_vec);

	string db_name() { return db_name_; }
};

// drop table #tb_name
class SQLDropTb : public SQL
{
private:
	string tb_name_;
public:
	SQLDropTb(vector<string>& sql_vec) { parse(sql_vec); }
	void parse(vector<string>& sql_vec);

	string tb_name() { return tb_name_; }
};

class SQLExec : public SQL
{
private:
	string file_name_;
public:
	SQLExec(vector<string>& sql_vec) { parse(sql_vec); }
	void parse(vector<string>& sql_vec);

	string file_name() { return file_name_; }
};