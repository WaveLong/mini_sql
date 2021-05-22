/*
+---------------------------------------------------------------------------------------+
| �ļ�����: interpreter.h													   	
| ��    ��: ���ĺ��û������Ĳ��֣���������ַ�������ΪSQL����������																	
| ��    ��: wang xin																		
| ��ϵ��ʽ: wangxinn@zju.edu.cn															
| ��������: 2021/05/12 20:05														
| �� �� ��: V1																			
| �޸���־: 																				
| ��Ȩ˵��: 2021, All right reserved.													
+---------------------------------------------------------------------------------------+
*/

#pragma once
#include <fstream>
#include <vector>
#include <string>
#include <regex>
#include <boost/algorithm/string.hpp>

#include "api.h"
#include "sql.h"

//#include "commons.h"

using namespace std;

class Interpreter
{
friend class MiniDBAPI;
private:
	MiniDBAPI* api_;
	string sql_string_;
	vector<string> sql_vec_;
	int sql_type_;
	void parse_sql_type();
	void convert_to_sql_vec();
	void run();

	vector<string> split(string& s, string sep);

public:
	Interpreter();
	~Interpreter();
	void excute_sql(string sql_string);
};

