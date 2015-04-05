/*
 * PriorityQueue.cc
 *
 *  Created on: 4 Apr, 2015
 *      Author: dinhtta
 */


#include <sort/PriorityQueue.h>
#include <utils/GlobalParams.h>
#include <glog/logging.h>
namespace sober{

Node::Node(string input_file){
	file_ = fopen(input_file.c_str(), "r");
	current_pos_ = 0;
	record_size_ = GlobalParams::Get()->record_size()+GCM_TAG_SIZE+IV_SIZE;
	current_record_ = (char*)malloc(record_size_*sizeof(char));
	fread(current_record_, 1, record_size_,file_);
}

bool Node::IsEmpty(){
	return feof(file_);
}

void Node::Next(){
	fread(current_record_, 1, record_size_,file_);
}

char *Node::GetRecord(){
	return current_record_;
}


void Node::DumpToFile(FILE *output){
	//current position
	long cur = ftell(file_);
	fseek(file_,0,SEEK_END);
	long last = ftell(file_);
	fseek(file_,cur,SEEK_SET);//rewind

	char *remainder = (char*)malloc((last-cur)*sizeof(char));
	fread(remainder,1,(last-cur),file_);
	fwrite(remainder,1,(last-cur),output);
	free(remainder);
}

/**
 * Close the file handle.
 */
Node::~Node(){
	fclose(file_);
}

PriorityQueue::PriorityQueue(int size){
	max_size_ = size+1;
	current_size_ = 0;
	ciphertext_size_ = GlobalParams::Get()->record_size()+GCM_TAG_SIZE+IV_SIZE;
	nodes_.push_back(NULL);
}
bool PriorityQueue::lessThan(Node *a, Node *b){
	char *data_a = a->GetRecord();
	char *data_b = b->GetRecord();
	string pt_a = encryptor_.Decrypt((byte*)data_a, ciphertext_size_);
	string pt_b = encryptor_.Decrypt((byte*)data_b, ciphertext_size_);
	return strncmp(pt_a.c_str(), pt_b.c_str(), pt_a.length())<0;
}

void PriorityQueue::upHeap(){
	int i = current_size_;
	Node *tmp;
	while (i>1){
		int j = i>>1; //i's parent
		if (lessThan(nodes_[j], nodes_[i])){
			//swap
			tmp = nodes_[i];
			nodes_[i] = nodes_[j];
			nodes_[j] = tmp;

			i=j;//continue
		}
		else
			break;
	}
}

void PriorityQueue::downHeap(){
	int i=1;
	int j=0;
	while (i<current_size_){
		int jl = i << 1;

		if (jl>current_size_) break; // no more children

		//j is smallest of two children
		if ((jl+1)<current_size_ && lessThan(nodes_[jl+1], nodes_[jl]))
			j = jl+1;
		else
			j = jl;

		if (lessThan(nodes_[i], nodes_[j]))
			break;
		else{
			//swap with smallest children
			Node *tmp = nodes_[i];
			nodes_[i] = nodes_[j];
			nodes_[j] = tmp;
			i = j;
		}
	}
}

bool PriorityQueue::Insert(Node *node){
	current_size_++;
	assert(current_size_<max_size_);
	nodes_.push_back(node);
	upHeap();
	return true;
}

char * PriorityQueue::Next(){
	return nodes_[1]->GetRecord();
}

void PriorityQueue::DumpToFile(FILE *file){
	nodes_[1]->DumpToFile(file);
}

void PriorityQueue::AdjustQueue(){
	//if the top node is empty, remove
	if (nodes_[1]->IsEmpty()){
		Node *tmp = nodes_[1];
		nodes_[1] = nodes_[current_size_];
		nodes_[current_size_] = tmp;
		nodes_.pop_back();
		delete tmp; //close the file.
		current_size_--;
		downHeap();
	}
	else{// move to next record on the top node, and downheap
		nodes_[1]->Next();
		downHeap();
	}
}

}
