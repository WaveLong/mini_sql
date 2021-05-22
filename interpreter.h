/*
+---------------------------------------------------------------------------------------+
| 文件名称: interpreter.h													   	
| 功    能: 最顶层的和用户交互的部分，将输入的字符串解析为SQL函数并运行																	
| 作    者: wang xin																		
| 联系方式: wangxinn@zju.edu.cn															
| 生成日期: 2021/05/12 20:05														
| 版 本 号: V1																			
| 修改日志: 																				
| 版权说明: 2021, All right reserved.													
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

