#if defined IZ_MACOS
#include <SDL.h>
#endif
#include <bdd-for-c.h>
#include <stdinc/IZ_stdlib.mock.h>
#include <common/IZ_common.h>
#include <game/data/IZ_list.h>

bool NodeExists(IZ_ListNode** node, u64 _index, IZ_List* list) {
	return *((u64*) (*node)->value) == 42069;
}

bool NodeExists2(IZ_ListNode** node, u64 _index, IZ_List* list) {
	return *((u64*) (*node)->value) == 69420;
}

bool NodeExists3(IZ_ListNode** node, u64 _index, IZ_List* list) {
	return *((u64*) (*node)->value) == 69069;
}

bool NodeDoesNotExist(IZ_ListNode** node, u64 _index, IZ_List* list) {
	return *((u64*) (*node)->value) == 55555;
}

bool NodeHasOddValue(IZ_ListNode** node, u64 _index, IZ_List* list) {
	return *((u64*) (*node)->value) % 2 == 1;
}

spec("data") {
	describe("list") {
		describe("Initialize") {
			static IZ_List list;

			after_each() {
				mock_mode(IZ_free, IZ_FREE_CALLS_UNTRACKED);
				IZ_ListTeardown(&list);
			}

			it("sets root to NULL") {
				IZ_ListInitialize(&list);

				check(
					list.root == NULL,
					"List not properly initialized. Root: %p", list.root
				);
			}
		}

		describe("Teardown") {
			static IZ_List list;

			before_each() {
				static u64 value1 = 69420u;
				static u64 value2 = 42069u;
				static u64 value3 = 69069u;

				IZ_ListInitialize(&list);
				IZ_ListAppendNode(&list, &value1, NULL);
				IZ_ListAppendNode(&list, &value2, NULL);
				IZ_ListAppendNode(&list, &value3, NULL);

				mock_mode(IZ_free, IZ_FREE_CALLS_TRACKED);
			}

			after_each() {
				mock_reset(IZ_free);
			}

			it("removes all nodes from the list") {
				mock_set_expected_calls(IZ_free, 3);
				IZ_ListTeardown(&list);
				unsigned int expected_calls = mock_get_expected_calls(IZ_free);
				unsigned int actual_calls = mock_get_actual_calls(IZ_free);

				check(
					expected_calls == actual_calls,
					"Deallocator function call count mismatch. Expected %u, got %u.", expected_calls, actual_calls
				);
			}
		}

		describe("AppendNode") {
			static IZ_List list;
			static IZ_List list2;

			before_each() {
				IZ_ListInitialize(&list);
			}

			before_each() {
				IZ_ListInitialize(&list2);
				static u64 existing_value = 69420u;
				static IZ_ListNode* existing_node;
				existing_node = IZ_malloc(sizeof(IZ_ListNode));
				existing_node->list = &list2;
				existing_node->previous = NULL;
				existing_node->value = &existing_value;
				existing_node->next = NULL;
				list2.root = existing_node;
				list2.length = 1;
			}

			after_each() {
				mock_reset(IZ_malloc);
			}

			after_each() {
				mock_mode(IZ_free, IZ_FREE_CALLS_UNTRACKED);
				IZ_ListTeardown(&list);
			}

			after_each() {
				mock_mode(IZ_free, IZ_FREE_CALLS_UNTRACKED);
				IZ_ListTeardown(&list2);
			}

			it("appends new node to empty list and sets it as root") {
				static u64 value = 69420u;
				static IZ_ListNode* check_inserted_node;
				IZ_ListAppendNode(&list, &value, &check_inserted_node);
				u64 added_value = *((u64*) list.root->value);

				check(added_value == 69420u, "Node not properly appended. Value: %u", added_value);
				check(*((u64*) check_inserted_node->value) == added_value, "Node value not properly set. Value: %u", added_value);
				check(
					mock_is_called(IZ_malloc),
					"Allocator function not called."
				);
				check(list.length == 1, "Length mismatch.");
			}

			it("appends new node to non-empty list") {
				static u64 value1 = 42069u;
				static IZ_ListNode* check_inserted_node;
				IZ_ListAppendNode(&list2, &value1, &check_inserted_node);
				check(*((u64*) check_inserted_node->value) == value1, "Node value not properly set. Value: %u", value1);

				check(*((u64*) list2.root->next->value) == 42069u, "Node not properly appended.");
				check(
					mock_is_called(IZ_malloc),
					"Allocator function not called."
				);
				check(list2.length == 2, "Length mismatch. Length: %u", list2.length);
			}
		}

		describe("FindFirstNode") {
			static IZ_List list;
			static u64 value1 = 69420u;
			static u64 value2 = 42069u;

			before_each() {
				IZ_ListInitialize(&list);
				IZ_ListAppendNode(&list, &value1, NULL);
				IZ_ListAppendNode(&list, &value2, NULL);
			}

			after_each() {
				mock_mode(IZ_free, IZ_FREE_CALLS_UNTRACKED);
				IZ_ListTeardown(&list);
			}

			it("retrieves first node satisfying the filter condition") {
				static IZ_ListNode* node;
				list.iterator = &list.root;
				IZ_ListFindFirstNode(&list, NodeExists, &node);
				check(*((u64*) node->value) == 42069u, "Existing node not found.");
			}

			it("returns NULL when all nodes do not satisfy the filter condition") {
				static IZ_ListNode* node;
				list.iterator = &list.root;
				IZ_ListFindFirstNode(&list, NodeDoesNotExist, &node);
				check(node == NULL, "Non-existing node found.");
			}
		}

		describe("DeleteNode") {
			static IZ_List list;

			before_each() {
				static u64 value1 = 69420u;
				static u64 value2 = 42069u;
				static u64 value3 = 69069u;

				IZ_ListInitialize(&list);
				IZ_ListAppendNode(&list, &value1, NULL);
				IZ_ListAppendNode(&list, &value2, NULL);
				IZ_ListAppendNode(&list, &value3, NULL);

				mock_mode(IZ_free, IZ_FREE_CALLS_TRACKED);
			}

			after_each() {
				mock_reset(IZ_free);
			}

			it("removes first node satisfying the filter condition") {
				list.iterator = &list.root;
				static IZ_ListNode* existing_node;
				static IZ_ListNode* check_node;
				IZ_ListFindFirstNode(&list, NodeExists, &existing_node);
				IZ_ListDeleteNode(existing_node);

				check(
					mock_is_called(IZ_free),
					"Deallocator function not called."
				);

				check_node = NULL;
				list.iterator = &list.root;
				IZ_ListFindFirstNode(&list, NodeExists2, &check_node);
				check(check_node, "Node supposed to be present in list is absent.");

				list.iterator = &list.root;
				check_node = NULL;
				IZ_ListFindFirstNode(&list, NodeExists, &check_node);
				check(!check_node, "Deleted node still present in list.");

				check(list.length == 2, "Length mismatch.");
			}
		}

		describe("FindAllNodes") {
			static IZ_List list;
			static IZ_List out;

			before_each() {
				static u64 value1 = 69420u;
				static u64 value2 = 42069u;
				static u64 value3 = 69069u;

				IZ_ListInitialize(&list);
				IZ_ListAppendNode(&list, &value1, NULL);
				IZ_ListAppendNode(&list, &value2, NULL);
				IZ_ListAppendNode(&list, &value3, NULL);

				mock_mode(IZ_free, IZ_FREE_CALLS_UNTRACKED);
			}

			before_each() {
				IZ_ListInitialize(&out);
			}

			after_each() {
				IZ_ListTeardown(&out);
			}

			after_each() {
				IZ_ListTeardown(&list);
			}

			it("finds all nodes satisfying the filter condition") {
				list.iterator = &list.root;
				IZ_ListFindAllNodes(&list, NodeHasOddValue, &out);

				check(out.length == 2, "Length mismatch. Length: %u", out.length);

				out.iterator = &out.root;
				static IZ_ListNode* check_node;
				check_node = NULL;
				IZ_ListFindFirstNode(&out, NodeExists, &check_node);
				check(check_node, "Node supposed to be present in list is absent.");

				out.iterator = &out.root;
				check_node = NULL;
				IZ_ListFindFirstNode(&out, NodeExists3, &check_node);
				check(check_node, "Node supposed to be present in list is absent.");

				out.iterator = &out.root;
				check_node = NULL;
				IZ_ListFindFirstNode(&out, NodeExists2, &check_node);
				check(!check_node, "Node not supposed to be present in list is present.");
			}
		}

		describe("InsertNodeAtIndex") {
			static IZ_List list;

			before_each() {
				static u64 value1 = 69420u;
				static u64 value2 = 42069u;
				static u64 value3 = 69069u;

				IZ_ListInitialize(&list);
				IZ_ListAppendNode(&list, &value1, NULL);
				IZ_ListAppendNode(&list, &value2, NULL);
				IZ_ListAppendNode(&list, &value3, NULL);

				mock_mode(IZ_free, IZ_FREE_CALLS_UNTRACKED);
			}

			after_each() {
				IZ_ListTeardown(&list);
			}

			it("inserts node at the beginning of the list") {
				static u64 value = 1337u;
				static IZ_ListNode* check_node;
				IZ_ListInsertNodeAtIndex(&list, &value, 0, &check_node);
				check(*((u64*) check_node->value) == value, "Incorrect value of inserted node.");
				static IZ_ListNode* check_other_node;

				list.iterator = &list.root;
				check_other_node = NULL;
				IZ_ListFindFirstNode(&list, NodeExists, &check_other_node);
				check(check_other_node, "Node supposed to be present in list is absent.");

				list.iterator = &list.root;
				check_other_node = NULL;
				IZ_ListFindFirstNode(&list, NodeExists2, &check_other_node);
				check(check_other_node, "Node supposed to be present in list is absent.");

				list.iterator = &list.root;
				check_other_node = NULL;
				IZ_ListFindFirstNode(&list, NodeExists3, &check_other_node);
				check(check_other_node, "Node supposed to be present in list is absent.");

				check(list.length == 4, "Length mismatch.");
				//IZ_ListPrintNodeValues(&list);
			}

			it("inserts node at the end of the list") {
				static u64 value = 1337u;
				static IZ_ListNode* check_node;
				IZ_ListInsertNodeAtIndex(&list, &value, 3, &check_node);
				check(*((u64*) check_node->value) == value, "Incorrect value of inserted node.");

				static IZ_ListNode* check_other_node;
				list.iterator = &list.root;
				check_other_node = NULL;
				IZ_ListFindFirstNode(&list, NodeExists, &check_other_node);
				check(check_other_node, "Node supposed to be present in list is absent.");

				list.iterator = &list.root;
				check_other_node = NULL;
				IZ_ListFindFirstNode(&list, NodeExists2, &check_other_node);
				check(check_other_node, "Node supposed to be present in list is absent.");

				list.iterator = &list.root;
				check_other_node = NULL;
				IZ_ListFindFirstNode(&list, NodeExists3, &check_other_node);
				check(check_other_node, "Node supposed to be present in list is absent.");

				check(list.length == 4, "Length mismatch.");
				//IZ_ListPrintNodeValues(&list);
			}

			it("inserts node in the middle of the list") {
				static u64 value = 1337u;
				static IZ_ListNode* check_node;
				IZ_ListInsertNodeAtIndex(&list, &value, 1, &check_node);
				check(*((u64*) check_node->value) == value, "Incorrect value of inserted node.");

				static IZ_ListNode* check_other_node;
				list.iterator = &list.root;
				check_other_node = NULL;
				IZ_ListFindFirstNode(&list, NodeExists, &check_other_node);
				check(check_other_node, "Node supposed to be present in list is absent.");

				list.iterator = &list.root;
				check_other_node = NULL;
				IZ_ListFindFirstNode(&list, NodeExists2, &check_other_node);
				check(check_other_node, "Node supposed to be present in list is absent.");

				list.iterator = &list.root;
				check_other_node = NULL;
				IZ_ListFindFirstNode(&list, NodeExists3, &check_other_node);
				check(check_other_node, "Node supposed to be present in list is absent.");

				check(list.length == 4, "Length mismatch.");
				//IZ_ListPrintNodeValues(&list);
			}
		}
	}
}
