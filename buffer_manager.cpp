#include "buffer_manager.h"

BlockInfo* BufferManager::get_usable_block() {
	BlockInfo* block = free_blocks_->remove_from_tail();
	if (block == NULL) {
		block = busy_blocks_->remove_from_tail();
		fhandle_->remove_file_block(block);
	}

	if (block->dirty()) {
		block->write_info(path_);
		block->set_dirty(false);
	}
	block->set_record_count(0);
	return block;
}

// 如何加速file和block之间的查询？？？
// 定义 < 运算
// 文件是否存在？如不存在则读入

BlockInfo* BufferManager::get_file_block(std::string db_name, std::string file_name, std::string type, int block_num) {
	FileInfo* fp = fhandle_->get_file(db_name, file_name, type);
	if (fp) { // 文件已经被打开
		BlockInfo* bp = fhandle_->get_file_block(fp, block_num);
		if (bp) { // 文件块已经在内存了，则将该块挂到busy_blocks_的头上
			busy_blocks_->remove_block(bp);
			busy_blocks_->insert_into_head(bp);
			return bp;
		}
		else { // 未读入该块
			bp = get_usable_block();
			bp->set_block_num(block_num);
			bp->set_file(fp);
			bp->read_info(path_);
			busy_blocks_->insert_into_head(bp);
			fhandle_->insert_file_block(bp);
			return bp;
		}
	}
	else { // 文件尚未被打开
		BlockInfo* bp = get_usable_block();
		bp->set_block_num(block_num);
		fp = new FileInfo(db_name, file_name, type, 0, 0); // 什么时候释放？为什么record_amount和record_len都设置为0？
		fhandle_->insert_file(fp);
		bp->set_file(fp);
		bp->read_info(path_);
		fhandle_->insert_file_block(bp);
		busy_blocks_->insert_into_head(bp);
		return bp;
	}
	return NULL;
}

void BufferManager::write_block(BlockInfo* b) { b->set_dirty(true); }
void BufferManager::write_into_disk() {
	fhandle_->write_into_disk();
	//busy_blocks_->write_into_disk();
}