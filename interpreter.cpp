#include "interpreter.h"
Interpreter::Interpreter() {
	string p = ".//minidb";
	api_ = new MiniDBAPI(p);
}

Interpreter::~Interpreter() {
	delete api_;
}

void Interpreter::parse_sql_type() {
	if (sql_vec_.size() <= 0) {
		sql_type_ = EMPTY;
		return;
	}
	//to_lower_copy(sql_vec_[0]);
	if (sql_vec_[0] == "quit") {
		sql_type_ = QUIT;
	}
	else if (sql_vec_[0] == "use") {
		sql_type_ = USE;
	}
	else if (sql_vec_[0] == "help") {
		sql_type_ = HELP;
	}
	else if (sql_vec_[0] == "create") {
		//_to_lower(sql_vec_[1]);
		if (sql_vec_[1] == "database"){
			sql_type_ = CRT_DB;
		}
		else if (sql_vec_[1] == "table") {
			sql_type_ = CRT_TB;
		}
		else if (sql_vec_[1] == "index") {
			sql_type_ = CRT_IDX;
		}
	}
	else if (sql_vec_[0] == "drop") {
		//_to_lower(sql_vec_[1]);
		if (sql_vec_[1] == "database") {
			sql_type_ = DP_DB;
		}
		else if (sql_vec_[1] == "table") {
			sql_type_ = DP_TB;
		}
		else if (sql_vec_[1] == "index") {
			sql_type_ = DP_IDX;
		}
	}
	else if (sql_vec_[0] == "select") {
		sql_type_ = SL;
	}
	else if (sql_vec_[0] == "insert") {
		sql_type_ = IN;
	}
	else if (sql_vec_[0] == "delete") {
		sql_type_ = DEL;
	}
	else if (sql_vec_[0] == "update") {
		sql_type_ = UP;
	}
	else if (sql_vec_[0] == "show") {
		//_to_lower(sql_vec_[1]);
		if (sql_vec_[1] == "database") {
			sql_type_ = SH_DB;
		}
		else if (sql_vec_[1] == "table") {
			sql_type_ = SH_TB;
		}
	}
	else if (sql_vec_[0] == "exec") {
		sql_type_ = EXEC;
	}
	return;
}
vector<string> Interpreter::split(string& s, string sep) {
	char* cstr = const_cast<char*>(s.c_str());
	char* content;
	vector<string> res;
	content = strtok(cstr, sep.c_str());
	while (content) {
		res.push_back(content);
		content = strtok(NULL, sep.c_str());
	}
	return res;
}
// eg: 
// select id, name 
// from student
// where id == 4;
void Interpreter::convert_to_sql_vec() {

	// 去掉换行
	regex reg("[\t\n\r]");
	sql_string_ = regex_replace(sql_string_, reg, " ");

	// 除掉";"后的内容
	// ";"严格匹配，"."表示除了换行以外的任何单字符，"*"出现零次或者多次，"$"从末尾匹配
	reg = ";.*$";
	sql_string_ = regex_replace(sql_string_, reg, "");

	// 除掉开头和结尾的空格
	reg = "(^ +)|( +$)";
	sql_string_ = regex_replace(sql_string_, reg, "");

	// 在关系运算符、括号、逗号前后插入空格
	// insert into #tb values(v1, v2, ..., vn)
	// -> insert into #tb values ( v1 , v2 , ... , vn ) 
	reg = " ?(\\(|\\)|,|=|<|>) ?";
	sql_string_ = regex_replace(sql_string_, reg, " $1 ");

	reg = "< *>";
	sql_string_ = regex_replace(sql_string_, reg, "<>");

	reg = "= *=";
	sql_string_ = regex_replace(sql_string_, reg, "==");

	reg = "< *=";
	sql_string_ = regex_replace(sql_string_, reg, "<=");

	reg = "> *=";
	sql_string_ = regex_replace(sql_string_, reg, ">=");

	// 除去多余的空格，即保留一个空格
	reg = " {2,}";
	sql_string_ = regex_replace(sql_string_, reg, " ");

	//reg = "\\s+";
	//sql_vec_ = vector<string>(sregex_token_iterator(sql_string_.begin(), sql_string_.end(), reg, -1), sregex_token_iterator());
	sql_vec_ = split(sql_string_, " ");
}
void Interpreter::run() {
	try {
		switch (sql_type_) {
		case QUIT: {
			api_->quit();
			return;
		}
		case HELP: {
			api_->help();
			return;
		}
		case SH_DB: {
			api_->show_database();
			return;
		}
		case SH_TB: {
			api_->show_tables();
			return;
		}
		case USE: {
			SQLUse st = SQLUse(sql_vec_);
			api_->use(st);
			return;
		}
		case CRT_DB: {
			SQLCreateDB st = SQLCreateDB(sql_vec_);
			api_->create_database(st);
			return;
		}
		case CRT_TB: {
			SQLCreateTb st = SQLCreateTb(sql_vec_);
			api_->create_table(st);
			return;
		}
		/*
		case CRT_IDX:
			SQLCreateIDX st = SQLCreateIDX(sql_vec_);
			api_->create_index(st);
			return;
			*/
		case DP_DB: {
			SQLDropDb st = SQLDropDb(sql_vec_);
			api_->drop_database(st);
			return;
		}

		case DP_TB: {
			SQLDropTb st = SQLDropTb(sql_vec_);
			api_->drop_table(st);
			return;
		}
			/*
		case DP_IDX:
			SQLDropIndex st = SQLDropIndex(sql_vec_);
			api_->drop_index(st);
			return;
		*/
		case SL: {
			SQLSelect st = SQLSelect(sql_vec_);
			api_->select(st);
			return;
		}
		case IN: {
			SQLInsert st = SQLInsert(sql_vec_);
			api_->insert(st);
			return;
		}
		case DEL: {
			SQLDelete st = SQLDelete(sql_vec_);
			api_->delete_record(st);
			return;
		}
		case UP: {
			SQLUpdate st = SQLUpdate(sql_vec_);
			api_->update_record(st);
			return;
		}	
		case EXEC: {
			SQLExec st = SQLExec(sql_vec_);
			api_->exec_file(this, st);
			return;
		}
		default:
			break;
		}
	}
	catch (SyntaxErrorException&) {
		cerr << "Syntax Error!" << endl;
	}
	catch (NoDatabaseSelectedException&) {
		cerr << "No Database Selected!" << endl;
	}
	catch (DatabaseNotExistException&) {
		cerr << "Database not exist!" << endl;
	}
	catch (DatabaseAlreadyExistsException&) {
		cerr << "Database already exists! " << endl;
	}
	catch (TableNotExistException&) {
		cerr << "Table not exist!" << endl;
	}
	catch (TableAlreadyExistsException&) {
		cerr << "Table already exists!" << endl;
	}
	catch (PrimaryKeyConflictException&) {
		cerr << "Primary key has existed!" << endl;
	}
	catch (SQLInstructionFileNotExist&) {
		cerr << "SQL instruction file not exist!" << endl;
	}
}

void Interpreter::excute_sql(string sql_string) {
	sql_string_ = sql_string;
	cout << "SQL STATEMENT: " << sql_string_ << endl;
	convert_to_sql_vec();
	parse_sql_type();
	run();
	//cout << endl;
}