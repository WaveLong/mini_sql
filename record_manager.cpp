#include "record_manager.h"

// 获取表的某一块的内存块
// 暂时以string 作为Table的替代

//BlockInfo* RecordManager::get_block_info(Table* t, int block_num) {
//	if (block_num < 0)return NULL;
//	return bm_->get_file_block(db_name_, t->table_name(), "records", block_num);
//}

BlockInfo* RecordManager::get_block_info(Table* t, int block_num) {
	if (block_num < 0)return NULL;
	return bm_->get_file_block(db_name_, t->table_name(), "records", block_num);
}

vector<Tkey*> RecordManager::get_record(Table* t, BlockInfo* bp, int offset) {
	int len = t->record_length();
	char* content = bp->get_content_address() + offset * len;
	vector<Tkey*> record;

	int attr_cnt = t->attribute_count();
	for (int i = 0; i < attr_cnt; ++i) {
		int type = t->attribute(i)->data_type();
		int len = t->attribute(i)->attr_length();
		Tkey* tmp = new Tkey(type);
		//tmp->read_value(content);
		memcpy(tmp->key(), content, len);
		record.push_back(tmp);
		content += len;
	}

	return record;
}

vector<Tkey*> RecordManager::construct_data(vector<string>& str_values, Table *t, int &pk) {
	vector<Tkey*>key_values;
	int attrs_size = str_values.size();
	pk = -1;
	for (int i = 0; i < attrs_size; ++i) {
		Tkey* tmp = new Tkey(t->attribute(i)->data_type());
		tmp->read_value(str_values[i]);
		key_values.push_back(tmp);
		if (t->attribute(i)->is_primary_key()) {
			pk = i;
		}
	}
	return key_values;
}

bool RecordManager::test_primary_key_confilct(Table* t, vector<Tkey*>& values, int pk) {
	if (false) { // 存在索引

	}
	else {
		int block_num = t->first_block_num();
		for (int i = 0; i < t->block_count(); ++i) {
			BlockInfo* bp = get_block_info(t, block_num);
			int record_cnt = bp->get_record_count();
			for (int j = 0; j < record_cnt; ++j) {
				vector<Tkey*> tkey_value = get_record(t, bp, j);
				if (*(tkey_value[pk]) == *(values[pk])) {
					return true;
				}
			}
		}
	}
	return false;
}

void RecordManager::insert_value(SQLInsert& st) {
	// 不包含主键唯一性测试，只验证文件的读取写入是否正确

	//step1: 判断表是否存在，由api层负责
	Database* d = cm_->database_by_name(db_name_);
	Table* t = d->get_table(st.tb_name());
	int attrs_size = t->attribute_count();

	//step2: 构造数据
	int pk = -1;
	vector<Tkey*>key_values = construct_data(st.sql_value(), t, pk);

	//step3: 主键唯一性判断
	if (pk != -1 && test_primary_key_confilct(t, key_values, pk)) {
		throw PrimaryKeyConflictException();
	}

	// step4: 插入数据, 新的块插入到文件链表的头

	int r_len = t->record_length();
	int max_count = (PAGE_SIZE - ADRR_BASE) / r_len;

	char* content = NULL;
	int used_block = t->first_block_num();
	int rubbish_block = t->first_rubbish_num();
	int block_num, offset; // 用于index
	bool full = true;
	BlockInfo* bp = NULL, * first_bp = NULL;
	// 找出未满的已用的块
	while (used_block != -1) {
		bp = get_block_info(t, used_block);
		if (bp->get_record_count() >= max_count) {
			used_block = bp->get_next_block_num();
			continue;
		}
		full = false;
		// 获取插入数据位置
		content = bp->get_content_address() + r_len * bp->get_record_count();
		// 数据位置
		block_num = used_block;
		offset = bp->get_record_count();
		break;
	}
	if (full) { // 所有已用的块都满了
		if (rubbish_block != -1) { // 获取文件内部空闲块
			// 获取空闲块并写入数据
			bp = get_block_info(t, rubbish_block);
			content = bp->get_content_address();
			bp->set_record_count(0);

			// 将新块加入到文件块链表的头
			int first_block = t->first_block_num();

			t->set_first_rubbish_num(bp->get_next_block_num());
			t->set_first_block_num(rubbish_block);

			first_bp = get_block_info(t, first_block);

			if(first_bp) first_bp->set_prev_block_num(rubbish_block);
			bp->set_next_block_num(first_block);
			bp->set_prev_block_num(-1);

			block_num = rubbish_block;
			offset = 0;
		}
		else {
			int new_block_id = t->block_count();
			t->increase_block_count();
			bp = get_block_info(t, new_block_id);

			content = bp->get_content_address();
			bp->set_record_count(0);

			int first_block = t->first_block_num();
			first_bp = get_block_info(t, first_block);

			if (first_bp) first_bp->set_prev_block_num(new_block_id);
			bp->set_next_block_num(first_block);
			bp->set_prev_block_num(-1);

			t->set_first_block_num(new_block_id);

			block_num = new_block_id;
			offset = 0;
		}
	}
	// 写入数据
	for (int i = 0; i < attrs_size; ++i) {
		memcpy(content, key_values[i]->key(), key_values[i]->key_length());
		content += key_values[i]->key_length();
	}
	bp->increase_record_count();
	bm_->write_block(bp);
	if (first_bp != NULL)bm_->write_block(first_bp);
	// 更新index
}

vector<int> RecordManager::convert_output_to_index(Table* t, vector<string>& output_cols) {
	vector<int>out_cols_index;
	int attr_cnt = t->attribute_count();
	if (output_cols[0] == "*") {
		for (int i = 0; i < attr_cnt; ++i)out_cols_index.push_back(i);
		return out_cols_index;
	}

	vector<string>attr_names;

	for (int i = 0; i < attr_cnt; ++i) {
		attr_names.push_back(t->attribute(i)->attribute_name());
	}

	for (auto s : output_cols) {
		for (int i = 0; i < attr_cnt; ++i) {
			if (s == attr_names[i]) {
				out_cols_index.push_back(i);
				break;
			}
		}
	}

	return out_cols_index;
}

bool RecordManager::satisfy(Tkey& v1, Tkey &v2, WhereCondition& w) {
	switch (w.condition_) {
	case EQ:
		return v1 == v2;
	case NEQ:
		return v1 != v2;
	case GT:
		return v1 > v2;
	case GE:
		return v1 >= v2;
	case LS:
		return v1 < v2;
	case LE: 
		return v1 <= v2;
	}
	return false;
}

bool RecordManager::satisfy(Table* t, vector<Tkey*> values, vector<WhereCondition> &wheres) {
	if (wheres.size() == 0) return true;

	vector<string> where_names;
	for (auto w : wheres) where_names.push_back(w.key_);
	vector<int> where_id = convert_output_to_index(t, where_names);
	int w_cnt = wheres.size();
	for (int i = 0; i < w_cnt; ++i) {
		Tkey cmp_value = Tkey(t->attribute(where_id[i])->data_type());
		cmp_value.read_value(wheres[i].value_);
		if (satisfy(*(values[where_id[i]]), cmp_value, wheres[i]) == false) {
			return false;
		}
	}
	return true;
}

void RecordManager::select_value(SQLSelect& st) {
	Table* t = cm_->database_by_name(db_name_)->get_table(st.tb_name());
	vector<int>out_cols_index = convert_output_to_index(t, st.output_cols());
	
	for (auto i : out_cols_index) {
		cout << setw(9) << left << t->attribute(i)->attribute_name();
	}
	cout << endl;

	bool has_index = false;
	// 查看是否"=="关系上存在索引
	// 跳过

	// 查找
	vector<vector<Tkey*>> select_res;

	if (!has_index) { // 顺序查找
		int block_num = t->first_block_num();
		int b_cnt = t->block_count();
		for (int i = 0; i < b_cnt; ++i) {
			BlockInfo* bp = get_block_info(t, block_num);
			int r_cnt = bp->get_record_count();
			for (int j = 0; j < r_cnt; ++j) {
				vector<Tkey*> values = get_record(t, bp, j);
				if (satisfy(t, values, st.where_condition())) {
					select_res.push_back(values);
				}
			}
		}
	}
	else {

	}

	// 打印结果
	for (auto vs : select_res) {
		for(auto i : out_cols_index)cout << setw(9) << left << *(vs[i]);
		cout << endl;
	}
}

void RecordManager::delete_record(Table* t, BlockInfo* bp, int offset) {
	int r_len = t->record_length();
	char* old = bp->get_content_address() + r_len * offset;
	char* replaced = bp->get_content_address() + r_len * (bp->get_record_count() - 1);
	memcpy(old, replaced, r_len);
	bp->decrease_record_count();
	if (bp->get_record_count() == 0) {
		int prev_num = bp->get_prev_block_num();
		int next_num = bp->get_next_block_num();

		if (prev_num != -1) {
			BlockInfo* prev_bp = get_block_info(t, prev_num);
			prev_bp->set_next_block_num(next_num);
			bm_->write_block(prev_bp);
		}

		if (next_num != -1) {
			BlockInfo* next_bp = get_block_info(t, next_num);
			next_bp->set_prev_block_num(prev_num);
			bm_->write_block(next_bp);
		}

		bp->set_prev_block_num(-1);
		if (t->first_rubbish_num() != -1) {
			bp->set_next_block_num(t->first_rubbish_num());
		}
		else {
			bp->set_next_block_num(-1);
		}
		t->set_first_rubbish_num(bp->block_num());
	}
	bm_->write_block(bp);
}
void RecordManager::delete_value(SQLDelete& st) {
	Table* t = cm_->database_by_name(db_name_)->get_table(st.tb_name());
	bool has_index = false;

	int block_num = t->first_block_num();
	int b_cnt = t->block_count();
	if (!has_index) {
		for (int i = 0; i < b_cnt; ++i) {
			BlockInfo* bp = get_block_info(t, block_num);
			int r_cnt = bp->get_record_count();
			for (int j = 0; j < r_cnt; ++j) {
				vector<Tkey*> values = get_record(t, bp, j);
				if (satisfy(t, values, st.where_condition())) {
					delete_record(t, bp, j);
				}
			}
			block_num = bp->get_next_block_num();
		}
	}
	else {

	}
}
void RecordManager::update_record(Table* t, BlockInfo* bp, int offset, vector<Tkey*>& values, vector<KV>& kvs) {
	// update record
	int attr_size = t->attribute_count();
	for (int i = 0; i < attr_size; ++i) {
		string name = t->attribute(i)->attribute_name();
		for (auto kv : kvs) {
			if (name == kv.k_) {
				values[i]->read_value(kv.v_);
				break;
			}
		}
	}
	// write new record
	char* content = bp->get_content_address() + offset * t->record_length();
	for (int i = 0; i < attr_size; ++i) {
		memcpy(content, values[i]->key(), values[i]->key_length());
		content += values[i]->key_length();
	}
	bm_->write_block(bp);
}
void RecordManager::update_value(SQLUpdate& st) {
	Table* t = cm_->database_by_name(db_name_)->get_table(st.tb_name());
	bool has_index = false;

	int block_num = t->first_block_num();
	int b_cnt = t->block_count();

	if (!has_index) {
		for (int i = 0; i < b_cnt; ++i) {
			BlockInfo* bp = get_block_info(t, block_num);
			int r_cnt = bp->get_record_count();
			for (int j = 0; j < r_cnt; ++j) {
				vector<Tkey*> values = get_record(t, bp, j);
				if (satisfy(t, values, st.where_condition())) {
					update_record(t, bp, j, values, st.kvs());
				}
			}
			block_num = bp->get_next_block_num();
		}
	}
	else {

	}
}
