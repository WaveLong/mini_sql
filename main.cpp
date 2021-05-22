#include <string>
#include <iostream>
#include <vector>

#include "interpreter.h"
using namespace std;
int main() {
	string sql, line;
	Interpreter* itp = new Interpreter();

	while (1) {
		cout << "MinDB >> ";
		getline(cin, line);
		sql = line;
		while (sql.find(";") == string::npos) {
			getline(cin, line);
			sql += line;
		}

		itp->excute_sql(sql);
		if (sql == "quit;")break;
	}
	return 0;
}