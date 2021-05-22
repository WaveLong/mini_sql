#pragma once

#include <string>
using namespace std;

void _to_lower(string& s) {
	for (auto& ch : s) ch = tolower(ch);
}
bool _is_all_num(const string& s) {
	bool res = true;
	for (auto ch : s)res &= isdigit(ch);
	return res;
}