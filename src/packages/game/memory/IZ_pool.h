#ifndef IZ_POOL_H
#define IZ_POOL_H

#include "../../common/IZ_common.h"
#include "../../log/IZ_log.h"
#include "../../stdinc/IZ_string.h"
#include "../../stdinc/IZ_stdlib.h"
#include "../data/IZ_list.h"

#define POOL_MAX_SIZE (1llu << 26) // 64MB

struct IZ_Pool;

typedef struct {
	size_t size;
	u64 priority;
	u64 timestamp;
} IZ_PoolAllocationArgs;

typedef struct {
	void* pointer;
	IZ_PoolAllocationArgs args;
	struct IZ_Pool* pool;
} IZ_PoolItem;

typedef struct IZ_Pool {
	IZ_List items;
	u64 next_address;
	size_t allocated_memory;
	size_t max_size;
	void* memory;
} IZ_Pool;

void IZ_PoolInitialize(IZ_Pool*, size_t);

IZ_PoolItem* IZ_PoolAllocate(IZ_Pool*, IZ_PoolAllocationArgs);

void IZ_PoolDeallocate(IZ_PoolItem*);

void IZ_PoolTeardown(IZ_Pool*);

#endif
