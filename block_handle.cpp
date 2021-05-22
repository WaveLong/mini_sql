#include "block_handle.h"

BlockHandle::BlockHandle(std::string path, int left) {
	left_ = left;
	path_ = path;
	if (left == 0) {
		first_block_ = last_block_ = NULL;
		return;
	}
	BlockInfo* p = new BlockInfo(-1), * tmp;
	first_block_ = p;
	for (int i = 1; i < left_; ++i) {
		tmp = new BlockInfo(-1);
		p->set_next(tmp);
		tmp->set_prev(p);
		p = tmp;
	}
	last_block_ = p;
}

BlockHandle::~BlockHandle() {
	BlockInfo* p = first_block_, * tmp;
	while (left_) {
		tmp = p->next();
		if (p->dirty())p->write_info(path_);
		delete p;
		p = tmp;
		left_--;
	}
}

void BlockHandle::insert_into_head(BlockInfo* b) {
	if (b == NULL)return;
	b->set_next(first_block_);
	b->set_prev(NULL);

	if (left_ == 0) { //链表为空
		first_block_ = last_block_ = b;
	}
	else { // 链表非空
		first_block_->set_prev(b);
		first_block_ = b;
	}
	left_++;
}

// 认为b肯定在链表中
void BlockHandle::remove_block(BlockInfo* b) {
	// 移除头 
	if (b == first_block_) {
		first_block_ = first_block_->next();
	}
	// 移除尾
	if (b == last_block_) {
		last_block_ = last_block_->prev();
	}
	// 有头
	if (b->prev())b->prev()->set_next(b->next());
	// 有尾
	if (b->next())b->next()->set_prev(b->prev());
	// 摘除块
	b->set_next(NULL);
	b->set_prev(NULL);
	left_--;
}

BlockInfo* BlockHandle::remove_from_tail() {
	if (last_block_ == NULL)return NULL;
	BlockInfo* res = last_block_;
	if (last_block_->prev() != NULL) { // 不止一个节点
		last_block_->prev()->set_next(NULL); 
		last_block_ = res->prev();
	}
	else { // 只剩下一个节点
		first_block_ = last_block_ = NULL;
	}
	
	res->set_prev(NULL);
	res->set_next(NULL);
	left_--;
	return res;
}

void BlockHandle::write_into_disk() {
	auto b = first_block_;
	while (b) {
		if (b->dirty()) {
			b->write_info(path_);
			b->set_dirty(false);
		}
		b = b->next();
	}
}