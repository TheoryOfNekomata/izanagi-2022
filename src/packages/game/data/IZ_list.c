#include <stdio.h>
#include "IZ_list.h"

void IZ_ListDoDeleteNode(IZ_ListNode* node) {
	if (!(node && node->list)) {
		// should we raise warnings here?
		return;
	}

	if (node->previous) {
		node->previous->next = node->next;
	} else {
		if (node->next) {
			node->next->previous = NULL;
		}
		node->list->root = node->next;
	}

	if (node->list->length > 0) {
		node->list->length -= 1;
	}

	IZ_free(node);
}

void IZ_ListDoAppendNode(IZ_List* list, void* node_value, IZ_ListNode** new_node_ref) {
	IZ_ListNode* new_node = IZ_malloc(sizeof(IZ_ListNode));
	new_node->value = node_value;
	new_node->next = NULL;
	new_node->list = list;
	new_node->list->length += 1;

	if (!(new_node->list->root)) {
		new_node->previous = NULL;
		new_node->list->root = new_node;
		if (new_node_ref) {
			*new_node_ref = new_node;
		}
		return;
	}

	IZ_ListNode** cursor_node = &new_node->list->root;
	while ((*cursor_node)->next) {
		cursor_node = &(*cursor_node)->next;
	}

	new_node->previous = *cursor_node;
	(*cursor_node)->next = new_node;
	if (new_node_ref) {
		*new_node_ref = new_node;
	}
}

#ifdef IZ_DEBUG
void IZ_ListPrintNodeValues(IZ_List* list) {
	list->iterator = &list->root;
	u64 index = 0;
	printf("\nlist@%p\n", list);
	do {
		printf("  %llu@%p:%u\n", index, *list->iterator, *((unsigned int*)(*list->iterator)->value));
		index += 1;
		list->iterator = &(*list->iterator)->next;
	} while (*list->iterator);
	list->iterator = NULL;
}
#endif

/**
 * Initializes a list.
 * @param list - The list to initialize.
 */
void IZ_ListInitialize(IZ_List* list) {
	list->root = NULL;
	list->length = 0;
	list->iterator = NULL;
}

/**
 * Performs cleanup on a list.
 * @param list - The list to clean up.
 */
void IZ_ListTeardown(IZ_List* list) {
	while (list->length > 0) {
		IZ_ListDoDeleteNode(list->root);
	}
}

/**
 * Appends a node to the end of the list.
 * @param list - The list to append to.
 * @param node_value - The value of the node to append.
 * @return Pointer to the newly created node.
 */
void IZ_ListAppendNode(IZ_List* list, void* node_value, IZ_ListNode** new_node) {
	IZ_ListDoAppendNode(list, node_value, new_node);
}

/**
 * Deletes the first node in the list that matches the filter.
 * @param node - The node to delete.
 */
void IZ_ListDeleteNode(IZ_ListNode* node) {
	IZ_ListDoDeleteNode(node);
}

/**
 * Finds the first node in the list that matches the filter.
 * @param list - The list to search.
 * @param filter - The filter to use to find the node.
 * @return Pointer to the node that matches the filter.
 * @see IZ_ListFindPredicate
 * @see IZ_ListFindAllNodes
 * @note This function will set the list's iterator to the node that was found. Ensure that the iterator is previously
 * set to an existing node in the list before calling this function to know where to begin the search.
 */
void IZ_ListFindFirstNode(IZ_List* list, IZ_ListFindPredicate filter, IZ_ListNode** found_node) {
	if (!(list && list->root)) {
		return;
	}

	u64 index = 0;
	do {
		if (!filter(list->iterator, index, list)) {
			list->iterator = &((*list->iterator)->next);
			index += 1;
			continue;
		}
		*found_node = *list->iterator;
		return;
	} while (*list->iterator);

}

/**
 * Finds all nodes in the list that match the filter.
 * @param list - The list to search.
 * @param filter - The filter to use to find the node.
 * @param found_nodes - The list to append the found nodes to.
 * @return New list containing nodes that match the filter.
 * @see IZ_ListFindPredicate
 * @see IZ_ListFindFirstNode
 * @note This function will set the list's iterator to the node that was found. Ensure that the iterator is previously
 * set to an existing node in the list before calling this function to know where to begin the search.
 */
void IZ_ListFindAllNodes(IZ_List* list, IZ_ListFindPredicate filter, IZ_List* found_nodes) {
	if (!(list && list->root)) {
		return;
	}

	list->iterator = &list->root;
	u64 index;
	for (index = 0; index < list->length; index += 1) {
		if (filter(list->iterator, index, list)) {
			IZ_ListDoAppendNode(found_nodes, (*list->iterator)->value, NULL);
		}

		list->iterator = &((*list->iterator)->next);
	}
}

/**
 * Inserts a node at the specified index.
 * @param list - The list to append to.
 * @param node_value - The value of the node to append.
 * @param dest_index - The index to insert the node at.
 * @return Pointer to the newly created node.
 */
void IZ_ListInsertNodeAtIndex(IZ_List* list, void* node_value, u64 dest_index, IZ_ListNode** new_node_ref) {
	if (dest_index > list->length) {
		// to consumer: check your bounds first!
		return;
	}

	if (dest_index == list->length) {
		IZ_ListDoAppendNode(list, node_value, new_node_ref);
		return;
	}

	IZ_ListNode* new_node = IZ_malloc(sizeof(IZ_ListNode));
	new_node->value = node_value;
	new_node->list = list;

	if (!(new_node->list->root)) {
		new_node->previous = NULL;
		new_node->next = NULL;
		new_node->list->root = new_node;
		if (new_node_ref) {
			*new_node_ref = new_node;
		}
		return;
	}

	IZ_ListNode** cursor_node = NULL;
	u64 index;
	for (
		index = 0, cursor_node = &new_node->list->root;
		index < dest_index;
		index += 1, cursor_node = &((*cursor_node)->next)
	);

	new_node->next = *cursor_node;
	new_node->previous = (*cursor_node)->previous;

	if (dest_index == 0) {
		new_node->list->root = new_node;
	} else if (dest_index < list->length) {
		(*cursor_node)->previous->next = new_node;
	} else {
		(*cursor_node)->next = new_node;
	}

	new_node->list->length += 1;
	if (new_node_ref) {
		*new_node_ref = new_node;
	}
}
