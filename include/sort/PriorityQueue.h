/*
 * PriorityQueue.h
 *
 *  Created on: 4 Apr, 2015
 *      Author: dinhtta
 */

#ifndef PRIORITYQUEUE_H_
#define PRIORITYQUEUE_H_

#include <stdio.h>
#include <encrypt/Encryptor.h>
#include <vector>
using std::vector;

/**
 * Priority Queue for k-way merge. Each node is the FILE handle. The property is that:
 * parent node > all child nodes.
 *
 * Where "<" is comparison on the next record pointed by FILE handle.
 *
 * Taken idea from Hadoop's Merger implementation.
 */

namespace sober{

/**
 * A node is defined by the FILE handle to a file on disk.
 */
class Node{
public:
	Node(string input_file);
	~Node();

	/**
	 * If no more to read
	 */
	bool IsEmpty();

	/**
	 * Read the next record. Automatically move the file pointer.
	 * Must check that it's not EOF before calling this.
	 */
	void Next();

	/**
	 * Return the current read record.
	 * It retains the owership (to avoid memory copying.
	 */
	char *GetRecord();

	/**
	 * Dump the remaining records to file. When there's only 1 node left
	 * on the priority queue.
	 */
	void DumpToFile(FILE *file);

private:
	FILE *file_;
	char *current_record_;
	int current_pos_, record_size_;
};

class PriorityQueue{
public:
	PriorityQueue(int size);

	/**
	 * push a new node in the queue. Return true if successful.
	 */
	bool Insert(Node *node);

	/**
	 * Adjust the queue from the top node. Called after a record
	 * is read from the top. Delete the top node if it reaches the end of file.
	 */
	void AdjustQueue();

	/**
	 * Return the next record, of the size globally known
	 */
	char *Next();

	bool HasNext() { return current_size_>0; } /** true if there's still node with record */

	int GetCurrentSize(){ return current_size_;}

	/**
	 * Dump the head node to file.
	 */
	void DumpToFile(FILE *file);

private:
	int max_size_;
	int current_size_;
	int ciphertext_size_;

	vector<Node*> nodes_; /**< content only start from 1 */
	Encryptor encryptor_;

	/**
	 * Move current node up the tree. If less than its parent, swap
	 */
	void upHeap();

	/**
	 * Move the current node down the tree. Swap with its smallest child
	 */
	void downHeap();

	bool lessThan(Node *a, Node *b); /** true if a<b */
};
}


#endif /* PRIORITYQUEUE_H_ */
