#include "IZ_pool.h"

void IZ_PoolInitialize(IZ_Pool* pool, size_t size) {
	IZ_LogInfo(IZ_LOG_CATEGORY_GLOBAL, "memory", "Setting up pool...");
	IZ_ListInitialize(&pool->items);
	pool->memory = IZ_malloc(size);
	IZ_memset(pool->memory, 0, size);
	pool->allocated_memory = 0;
	pool->next_address = 0;
	pool->max_size = size;
}

IZ_PoolItem* IZ_PoolAllocate(IZ_Pool* pool, IZ_PoolAllocationArgs args) {
	// 1. check next free allocation for size
	// 2. if 1. returns non-null,

//	u64 alloc_end = pool->next_address + size;
//
//	for (u64 i = 0; i < POOL_MAX_ALLOCATIONS; i += 1) {
//		if (pool->allocation[i].length == 0) {
//			continue;
//		}
//	}

	if (pool->max_size - pool->allocated_memory < args.size) {
		// TODO deallocate memory based from priority
	}

	void* pointer = &((u8*) pool->memory)[pool->next_address];
	IZ_ListNode* new_item;
	IZ_ListAppendNode(&pool->items, &(IZ_PoolItem) {
		.pointer = pointer,
		.args = args,
		.pool = pool,
	}, &new_item);

	pool->next_address = (pool->next_address + args.size) % POOL_MAX_SIZE;
	pool->allocated_memory += args.size;
	return new_item->value;
}

bool IZ_PoolGetSameItem(IZ_ListNode** node, u64 _index, IZ_List* list) {
	return (*node)->value == (*list->iterator)->value;
}

void IZ_PoolDeallocate(IZ_PoolItem* item) {
	IZ_ListNode* node;
	IZ_ListFindFirstNode(&item->pool->items, IZ_PoolGetSameItem, &node);
	if (node) {
		IZ_ListDeleteNode(node);
	}
}

void IZ_PoolTeardown(IZ_Pool* pool) {
	IZ_LogInfo(IZ_LOG_CATEGORY_GLOBAL, "memory", "Shutting down pool...");
	IZ_free(pool->memory);
	pool->memory = NULL;
}
