#include <bdd-for-c.h>
#include "../../../../__mocks__/src/packages/log/IZ_log.mock.h"
#include "../../../../__mocks__/src/packages/stdinc/IZ_stdlib.mock.h"
#include "../../../../__mocks__/src/packages/stdinc/IZ_string.mock.h"
#include "../../../../src/packages/game/memory/IZ_pool.h"

struct DummyInnerStruct {
	i8 f;
	i8 g;
};

struct DummyStruct {
	i8 a;
	i16 b;
	i32 c;
	i64 d;
	struct DummyInnerStruct e;
};

spec("memory") {
	describe("pool") {
		describe("Initialize") {
			static IZ_Pool pool;

			after_each() {
				mock_reset(IZ_memset);
			}

			after_each() {
				mock_reset(IZ_malloc);
			}

			after_each() {
				IZ_PoolTeardown(&pool);
			}

			it("initializes the pool values") {
				mock_set_expected_calls(IZ_memset, 1);

				IZ_PoolInitialize(&pool, POOL_MAX_SIZE);

				check(
					mock_get_expected_calls(IZ_memset) == mock_get_actual_calls(IZ_memset),
					"Call count mismatch."
				);
			}

			it("initializes the pool memory") {
				IZ_PoolInitialize(&pool, POOL_MAX_SIZE);

				check(
					mock_is_called(IZ_malloc),
					"Memory not allocated."
				);
			}
		}

		describe("Allocate") {
			static IZ_Pool pool;

			after_each() {
				IZ_PoolTeardown(&pool);
			}

			it("assigns contiguous memory") {
				IZ_PoolInitialize(&pool, POOL_MAX_SIZE);
				void* p1 = IZ_PoolAllocate(&pool, (IZ_PoolAllocationArgs){
					.size = sizeof(struct DummyStruct),
					.priority = 0,
					.timestamp = 0,
				})->pointer;
				void* p2 = IZ_PoolAllocate(&pool, (IZ_PoolAllocationArgs){
					.size = sizeof(u8),
					.priority = 0,
					.timestamp = 0,
				})->pointer;

				check(
					p2 - p1 == sizeof(struct DummyStruct),
					"Allocated memory mismatch."
				);
			}

			it("ignores previously deallocated items") {
				IZ_PoolInitialize(&pool, POOL_MAX_SIZE);

				void* p1 = IZ_PoolAllocate(&pool, (IZ_PoolAllocationArgs){
					.size = sizeof(struct DummyStruct),
					.priority = 0,
					.timestamp = 0,
				})->pointer;
				void* p2 = IZ_PoolAllocate(&pool, (IZ_PoolAllocationArgs){
					.size = sizeof(u8),
					.priority = 0,
					.timestamp = 0,
				})->pointer;
				void* p3 = IZ_PoolAllocate(&pool, (IZ_PoolAllocationArgs){
					.size = sizeof(u8),
					.priority = 0,
					.timestamp = 0,
				})->pointer;

				IZ_PoolDeallocate(p2);

				check(
					p3 - p1 == sizeof(struct DummyStruct) + sizeof(u8),
					"Allocated memory mismatch."
				);
			}

			it("deallocates old items to make way for new ones when the memory is full") {
				IZ_PoolInitialize(&pool, sizeof(u32));

				void* p1 = IZ_PoolAllocate(&pool, (IZ_PoolAllocationArgs){
					.size = sizeof(u16),
					.priority = 0,
					.timestamp = 0,
				});
				IZ_PoolAllocate(&pool, (IZ_PoolAllocationArgs){
					.size = sizeof(u8),
					.priority = 0,
					.timestamp = 1,
				});
				IZ_PoolAllocate(&pool, (IZ_PoolAllocationArgs){
					.size = sizeof(u8),
					.priority = 0,
					.timestamp = 2,
				});

				void* p2 = IZ_PoolAllocate(&pool, (IZ_PoolAllocationArgs){
					.size = sizeof(u8),
					.priority = 0,
					.timestamp = 3,
				});

				check(
					p1 == p2,
					"Old memory not properly deallocated."
				);

				void* p3 = IZ_PoolAllocate(&pool, (IZ_PoolAllocationArgs){
					.size = sizeof(u8),
					.priority = 0,
					.timestamp = 4,
				});

				// FIXME here onwards

				check(
					p3 == p2 + sizeof(u8),
					"Free memory not properly utilized."
				);

				void* p4 = IZ_PoolAllocate(&pool, (IZ_PoolAllocationArgs) {
					.size = sizeof(u8),
					.priority = 0,
					.timestamp = 5,
				});

				check(
					p4 == p3 + sizeof(u8),
					"Free memory not properly allocated."
				);
			}
		}

		describe("Deallocate") {}

		describe("Teardown") {
			static IZ_Pool pool;

			before_each() {
				IZ_PoolInitialize(&pool, POOL_MAX_SIZE);
			}

			after_each() {
				mock_reset(IZ_free);
			}

			it("frees memory") {
				IZ_PoolTeardown(&pool);

				check(
					mock_is_called(IZ_free),
					"Memory not freed."
				);
			}
		}
	}
}
