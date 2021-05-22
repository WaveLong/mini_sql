#include "block_info.h"
#include <fstream>

using namespace std;

void BlockInfo::read_info(string path) {
	path += ("//" + file_->db_name() + "//" + file_->file_name() + "." + file_->type());
	ifstream in(path, ios::binary | ios::in);
	in.seekg(block_num_ * PAGE_SIZE, ios::beg);
	in.read(data_, PAGE_SIZE);
	in.close();
}

void BlockInfo::write_info(string path) {
	path += ("//" + file_->db_name() + "//" + file_->file_name() + "." + file_->type());
	ofstream out(path, ios::binary | ios::out);
	out.seekp(block_num_ * PAGE_SIZE, ios::beg);
	out.write(data_, PAGE_SIZE);
	out.close();
}