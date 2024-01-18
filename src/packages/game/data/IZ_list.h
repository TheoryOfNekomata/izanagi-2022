#ifndef IZ_LIST_H
#define IZ_LIST_H

#include "../../common/IZ_common.h"
#include "../../stdinc/IZ_stdlib.h"

struct IZ_List;

/**
 * A node in a linked list.
 */
typedef struct IZ_ListNode {
	/**
	 * The list that the node belongs to.
	 */
	struct IZ_List* list;
	/**
	 * The previous node in the list.
	 */
	struct IZ_ListNode* previous;
	/**
	 * The value of the node.
	 */
	void* value;
	/**
	 * The next node in the list.
	 */
	struct IZ_ListNode* next;
} IZ_ListNode;

/**
 * A doubly-linked list.
 */
typedef struct IZ_List {
	/**
	 * The first node in the list.
	 */
	IZ_ListNode* root;
	/**
	 * The number of nodes in the list.
	 */
	u64 length;
	/**
	 * The iterator for traversing the list.
	 */
	IZ_ListNode** iterator;
} IZ_List;

typedef bool IZ_ListFindPredicate(IZ_ListNode**, u64, IZ_List*);

#ifdef IZ_DEBUG
void IZ_ListPrintNodeValues(IZ_List*);
#endif

/**
 * Initializes a list.
 */
void IZ_ListInitialize(IZ_List*);

/**
 * Performs cleanup on a list.
 */
void IZ_ListTeardown(IZ_List*);

/**
 * Appends a node to the end of the list.
 * @return Pointer to the newly created node.
 */
void IZ_ListAppendNode(IZ_List*, void*, IZ_ListNode**);

/**
 * Deletes the first node in the list that matches the filter.
 */
void IZ_ListDeleteNode(IZ_ListNode*);

/**
 * Finds the first node in the list that matches the filter.
 * @return Pointer to the node that matches the filter.
 * @see IZ_ListFindPredicate
 * @see IZ_ListFindAllNodes
 */
void IZ_ListFindFirstNode(IZ_List*, IZ_ListFindPredicate, IZ_ListNode**);

/**
 * Finds all nodes in the list that match the filter.
 * @return New list containing nodes that match the filter.
 * @see IZ_ListFindPredicate
 * @see IZ_ListFindFirstNode
 */
void IZ_ListFindAllNodes(IZ_List*, IZ_ListFindPredicate, IZ_List*);

/**
 * Inserts a node at the specified index.
 * @return Pointer to the newly created node.
 */
void IZ_ListInsertNodeAtIndex(IZ_List*, void*, u64, IZ_ListNode**);

#endif
