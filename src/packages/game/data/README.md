# `data`

Fundamental functions for various data structures used in the project.

## `list`

A doubly-linked list is used on certain parts of the game, such as the memory pool's entries of currently allocated game
objects.

Here is how to properly use a list:

```c
#include <stdbool.h>
#include "IZ_list.h"

// find predicates
bool FindNodeWithValueOne(IZ_ListNode**, unsigned long long, IZ_List*);

bool FindNodeWithValueTwo(IZ_ListNode**, unsigned long long, IZ_List*);

int main() {
	IZ_List list;
	
	// important to initialize lists first, else there may be garbage data in the list members.
	IZ_ListInitialize(&list);
	
	// lists should accept any type of value (void*), so the implementor is free to use whatever type they want
	int node1_value = 1;
	IZ_ListAppendNode(&list, &node1_value, NULL);
	
	int node2_value = 2;
	IZ_ListAppendNode(&list, &node2_value, NULL);
	
	// pass predicate functions for finding nodes 
	ListNode* find_node = NULL;
	IZ_ListFindFirstNode(&list, &FindNodeWithValueTwo, &find_node);
	if (find_node != NULL) {
		printf("Found node with value 2!\n");
	}
	
	ListNode* delete_node;
	IZ_ListFindFirstNode(&list, &FindNodeWithValueOne, &delete_node);
	// deletions are done only on one node at a time
	IZ_ListDeleteNode(delete_node);
	
	// teardown takes care of de-allocating nodes and making sure data cannot be accessed sensibly anymore.
	IZ_ListTeardown(&list);
	
	return 0;
}

bool FindNodeWithValueOne(IZ_ListNode** node, unsigned long long index, IZ_List* list) {
	return (*node)->value == 1;
}

bool FindNodeWithValueTwo(IZ_ListNode** node, unsigned long long index, IZ_List* list) {
	return (*node)->value == 2;
}
```
