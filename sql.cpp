#include "sql.h"
// Tkey
Tkey::Tkey(int type) {
	key_type_ = type;
	if (key_type_ == -2) {
		key_length_ = sizeof(int);
	}
	else if (key_type_ == -1) {
		key_length_ = sizeof(float);
	}
	else {
		key_length_ = key_type_;
	}

	key_ = new char[key_length_];
}
Tkey::Tkey(const Tkey& k) {
	key_type_ = k.key_type_;
	key_length_ = k.key_length_;
	memcpy(key_, k.key_, k.key_length_);
}
Tkey::~Tkey() {
	delete []key_;
}

void Tkey::read_value(const char* content) {
	if (key_type_ == -2) {
		int v = atoi(content);
		memcpy(key_, &v, key_length_);
	}
	else if (key_type_ == -1) {
		float v = atof(content);
		memcpy(key_, &v, key_length_);
	}
	else {
		memcpy(key_, content, key_length_);
	}
}

void Tkey::read_value(const string& s) {
	read_value(s.c_str());
}

bool Tkey::operator == (Tkey& t) const {
	if (key_type_ != t.key_type_)return false;

	if (key_type_ == -2) {
		return *((int*)key_) == *((int*)t.key_);
	}
	else if (key_type_ == -1){
		return *((float*)key_) == *((float*)t.key_);
	}
	else {
		return strcmp(key_, t.key_) == 0;
	}
	return false;
}
bool Tkey::operator != (Tkey& t) const {
	return !(*this == t);
}
bool Tkey::operator < (Tkey& t) const {
	if (key_type_ != t.key_type_)return false;

	if (key_type_ == -2) {
		return *((int*)key_) < *((int*)t.key_);
	}
	else if (key_type_ == -1) {
		return *((float*)key_) < *((float*)t.key_);
	}
	else {
		return strcmp(key_, t.key_) < 0;
	}
	return false;
}

bool Tkey::operator <= (Tkey& t) const {
	if (key_type_ != t.key_type_)return false;

	if (key_type_ == -2) {
		return *((int*)key_) <= *((int*)t.key_);
	}
	else if (key_type_ == -1) {
		return *((float*)key_) <= *((float*)t.key_);
	}
	else {
		return strcmp(key_, t.key_) <= 0;
	}
	return false;
}
bool Tkey::operator > (Tkey& t) const {
	if (key_type_ != t.key_type_)return false;
	return !(*this <= t);
}
bool Tkey::operator >= (Tkey& t) const {
	if (key_type_ != t.key_type_)return false;
	return !(*this < t);
}

ostream& operator<<(ostream& out, Tkey& object){
	if (object.key_type_ == -2) {
		int v;
		memcpy(&v, object.key_, object.key_length_);
		out << v;
	}
	else if (object.key_type_ == -1) {
		float v;
		memcpy(&v, object.key_, object.key_length_);
		out << v;
	}
	else {
		out << object.key_;
	}
	return out;
};
// SQL

int SQL::parse_data_type(vector<string> sql_vec, Attribute *attr, int pos) {
	//_to_lower(sql_vec[pos]);
	if (sql_vec[pos] == "int") {
		attr->set_data_type(-2);
		attr->set_attr_length(sizeof(int));
		pos++;
		if (sql_vec[pos] == ",")pos++;
	}
	else if (sql_vec[pos] == "float") {
		attr->set_data_type(-1);
		attr->set_attr_length(sizeof(float));
		pos++;
		if (sql_vec[pos] == ",")pos++;
	}
	else if (sql_vec[pos] == "char") {
		pos++;
		if (sql_vec[pos] == "(") {
			pos++;
			// if (_is_all_num(sql_vec[pos])) {
			if (true) {
				attr->set_data_type(stoi(sql_vec[pos]));
				attr->set_attr_length(stoi(sql_vec[pos]));
				pos++;
				if (sql_vec[pos] == ")") {
					pos++;
					if (sql_vec[pos] == ",") {
						pos++;
					}
				}
				else {
					throw SyntaxErrorException();
				}
			}
			else {
				throw SyntaxErrorException();
			}
		}
		else {
			throw SyntaxErrorException();
		}
	}
	else {
		throw SyntaxErrorException();
	}
	return pos;
}
// SQLUse
void SQLUse::parse(vector<string>& sql_vec) {
	sql_type_ = USE;
	if (sql_vec.size() < 2) {
		throw SyntaxErrorException();
	}
	db_name_ = sql_vec[1];
}

// SQLCreateDB
void SQLCreateDB::parse(vector<string>& sql_vec) {
	sql_type_ = CRT_DB;
	if (sql_vec.size() <= 2) {
		//cout << "Invalid Format!" << endl;
		throw  SyntaxErrorException();
	}
	else {
		db_name_ = sql_vec[2];
	}
}
// SQLCreateTb
void SQLCreateTb::parse(vector<string>& sql_vec) {
	sql_type_ = CRT_TB;
	
	int pos = 2;
	if (sql_vec.size() <= pos) {
		throw  SyntaxErrorException();

		/*cout << "Invalid Format!" << endl;
		return;*/
	}

	tb_name_ = sql_vec[pos++];

	if (sql_vec[pos] != "(") {
		throw  SyntaxErrorException();
	}
	pos++;

	bool has_pk = false;
	while (1) {
		if (sql_vec[pos] == "primary") {
			pos++;
			if (sql_vec[pos] != "key") {
				throw SyntaxErrorException();
			}
			pos++;

			if (has_pk) {
				throw SyntaxErrorException();
			}

			for (int i = 0; i < attrs_.size(); ++i) {
				if (attrs_[i]->attribute_name() == sql_vec[pos]) {
					attrs_[i]->set_primary_key(true);
					break;
				}
			}

			pos++;

			if (sql_vec[pos] != ")") {
				throw SyntaxErrorException();
			}
			has_pk = true;
		}
		else {
			Attribute* attr = new Attribute();
			attr->set_attribute_name(sql_vec[pos]);
			pos++;
			pos = parse_data_type(sql_vec, attr, pos);
			attrs_.push_back(attr);
		}

		if (sql_vec[pos] == ")") break;
	}
}

// SQLInsert
void SQLInsert::parse(vector<string>& sql_vec) {
	sql_type_ = IN;
	int pos = 1;
	if (sql_vec[pos] != "into") {
		throw SyntaxErrorException();
	}
	pos++;
	tb_name_ = sql_vec[pos++];
	if (sql_vec[pos] != "values") {
		throw SyntaxErrorException();
	}
	pos++;
	if (sql_vec[pos] != "(") {
		throw SyntaxErrorException();
	}
	pos++;

	while (pos < sql_vec.size() && sql_vec[pos] != ")") {
		sql_value_.push_back(sql_vec[pos++]);
		if (sql_vec[pos] == ",")pos++;
	}

	if (pos == sql_vec.size()) {
		throw SyntaxErrorException();
	}
}

// SQLSelect
void SQLSelect::parse(vector<string>& sql_vec) {
	sql_type_ = SL;

	int pos = 1;
	while (pos < sql_vec.size() && sql_vec[pos] != "from") {
		if (sql_vec[pos] == ",") {
			pos++;
			continue;
		}
		cols_.push_back(sql_vec[pos++]);
	}
	if (pos == sql_vec.size() || sql_vec[pos] != "from") {
		throw SyntaxErrorException();
	}
	pos++;
	tb_name_ = sql_vec[pos++];

	if (pos == sql_vec.size())return;
	if (sql_vec[pos] != "where") {
		throw SyntaxErrorException();
	}

	pos++;

	while (1) {
		WhereCondition tmp;
		tmp.key_ = sql_vec[pos++];
		if (sql_vec[pos] == "==") {
			tmp.condition_ = EQ;
		}
		else if (sql_vec[pos] == "<>") {
			tmp.condition_ = NEQ;
		}
		else if (sql_vec[pos] == ">") {
			tmp.condition_ = GT;
		}
		else if (sql_vec[pos] == ">=") {
			tmp.condition_ = GE;
		}
		else if (sql_vec[pos] == "<") {
			tmp.condition_ = LS;
		}
		else if (sql_vec[pos] == "<=") {
			tmp.condition_ = LE;
		}
		else {
			throw SyntaxErrorException();
		}
		pos++;

		tmp.value_ = sql_vec[pos];
		wheres_.push_back(tmp);
		pos++;
		if (pos == sql_vec.size())break;

		if (sql_vec[pos] == "," || sql_vec[pos] == "and")pos++;
		else throw SyntaxErrorException();
	}
}

void SQLDelete::parse(vector<string>& sql_vec) {
	sql_type_ = DEL;
	int pos = 1;
	if (sql_vec[pos] != "from") {
		throw SyntaxErrorException();
	}
	pos++;
	tb_name_ = sql_vec[pos++];
	if (pos == sql_vec.size())return;
	if (sql_vec[pos] != "where") {
		throw SyntaxErrorException();
	}
	pos++;
	while (1) {
		WhereCondition tmp;
		tmp.key_ = sql_vec[pos++];
		if (sql_vec[pos] == "==") {
			tmp.condition_ = EQ;
		}
		else if (sql_vec[pos] == "<>") {
			tmp.condition_ = NEQ;
		}
		else if (sql_vec[pos] == ">") {
			tmp.condition_ = GT;
		}
		else if (sql_vec[pos] == ">=") {
			tmp.condition_ = GE;
		}
		else if (sql_vec[pos] == "<") {
			tmp.condition_ = LS;
		}
		else if (sql_vec[pos] == "<=") {
			tmp.condition_ = LE;
		}
		else {
			throw SyntaxErrorException();
		}
		pos++;

		tmp.value_ = sql_vec[pos];
		wheres_.push_back(tmp);
		pos++;
		if (pos == sql_vec.size())break;

		if (sql_vec[pos] == "," || sql_vec[pos] == "and")pos++;
		else throw SyntaxErrorException();
	}
}

void SQLUpdate::parse(vector<string>& sql_vec) {
	int pos = 1;
	tb_name_ = sql_vec[pos++];
	if (sql_vec[pos] != "set") {
		throw SyntaxErrorException();
	}
	pos++;
	while (1) {
		KV tmp;
		tmp.k_ = sql_vec[pos++];
		if (sql_vec[pos] != "=") {
			throw SyntaxErrorException();
		}
		pos++;
		tmp.v_ = sql_vec[pos++];
		kvs_.push_back(tmp);

		if (pos == sql_vec.size()) {
			return;
		}
		if (sql_vec[pos] == "where") {
			pos++;
			break;
		}
		if (sql_vec[pos] == ",")pos++;
	}

	while (1) {
		WhereCondition tmp;
		tmp.key_ = sql_vec[pos++];
		if (sql_vec[pos] == "==") {
			tmp.condition_ = EQ;
		}
		else if (sql_vec[pos] == "<>") {
			tmp.condition_ = NEQ;
		}
		else if (sql_vec[pos] == ">") {
			tmp.condition_ = GT;
		}
		else if (sql_vec[pos] == ">=") {
			tmp.condition_ = GE;
		}
		else if (sql_vec[pos] == "<") {
			tmp.condition_ = LS;
		}
		else if (sql_vec[pos] == "<=") {
			tmp.condition_ = LE;
		}
		else {
			throw SyntaxErrorException();
		}
		pos++;

		tmp.value_ = sql_vec[pos];
		wheres_.push_back(tmp);
		pos++;
		if (pos == sql_vec.size())break;

		if (sql_vec[pos] == "," || sql_vec[pos] == "and")pos++;
		else throw SyntaxErrorException();
	}
}

void SQLDropDb::parse(vector<string>& sql_vec) {
	sql_type_ = DP_DB;
	int pos = 1;
	if (sql_vec[pos] != "database") {
		throw SyntaxErrorException();
	}
	pos++;
	db_name_ = sql_vec[pos];
	pos++;
	if (pos != sql_vec.size()) {
		throw SyntaxErrorException();
	}
}

void SQLDropTb::parse(vector<string>& sql_vec) {
	sql_type_ = DP_TB;
	int pos = 1;
	if (sql_vec[pos] != "table") {
		throw SyntaxErrorException();
	}
	pos++;
	tb_name_ = sql_vec[pos];
	pos++;
	if (pos != sql_vec.size()) {
		throw SyntaxErrorException();
	}
}

void SQLExec::parse(vector<string>& sql_vec) {
	sql_type_ = EXEC;
	int pos = 1;
	file_name_ = sql_vec[pos++];
	if (pos != sql_vec.size()) {
		throw SyntaxErrorException();
	}
}