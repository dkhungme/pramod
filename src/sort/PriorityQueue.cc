/*
 * PriorityQueue.cc
 *
 *  Created on: 4 Apr, 2015
 *      Author: dinhtta
 */


#include <sort/PriorityQueue.h>
#include <glog/logging.h>
namespace sober{

Node::Node(string input_file, Encryptor *encryptor, int record_size, int plaintext_size, data_mode_t mode){
	file_ = fopen(input_file.c_str(), "r");
	current_pos_ = 0;
	//record_size_ = GlobalParams::Get()->record_size()+GCM_TAG_SIZE+IV_SIZE;
	record_size_ = record_size;
	plaintext_size_ = plaintext_size; 
	mode_ = mode; 
	//plaintext_size_ = GlobalParams::Get()->record_size(); 

	current_record_ = (char*)malloc(record_size_*sizeof(char));
	current_plaintext_ = (char*)malloc(plaintext_size_*sizeof(char)); 
	encryptor_ = encryptor; 

	fread(current_record_, 1, record_size_,file_);
	if (mode==ENCRYPT){
		string pt = encryptor_->Decrypt((byte*)current_record_, record_size_);
		memcpy(current_plaintext_,pt.c_str(),plaintext_size_); 
	}
}

bool Node::IsEmpty(){
	return feof(file_);
}

void Node::Next(){
	fread(current_record_, 1, record_size_,file_);
	if (mode_==ENCRYPT){
		string pt = encryptor_->Decrypt((byte*)current_record_, record_size_); 
		memcpy(current_plaintext_,pt.c_str(),plaintext_size_); 
	}
}

char *Node::GetRecord(){
	return mode_==ENCRYPT ? current_plaintext_: current_record_;
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

PriorityQueue::PriorityQueue(int size, int record_size, int plaintext_size, comp comparator){
	max_size_ = size+1;
	current_size_ = 0;
	nodes_.push_back(NULL);

	record_size_ = record_size; 
	plaintext_size_ = plaintext_size; 
	comparator_ = comparator; 
}

bool PriorityQueue::lessThan(Node *a, Node *b){
	char *data_a = a->GetRecord();
	char *data_b = b->GetRecord();
	//string pt_a = encryptor_.Decrypt((byte*)data_a, ciphertext_size_);
	//string pt_b = encryptor_.Decrypt((byte*)data_b, ciphertext_size_);
	//return strncmp(data_a, data_b, plaintext_size_)<0;
	return comparator_(string(data_a,plaintext_size_), string(data_b, plaintext_size_)); 
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
	delete nodes_[1]; 
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
