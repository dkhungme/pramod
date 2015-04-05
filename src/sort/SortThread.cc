/*
 * Sorter.cc
 *
 *  Created on: 3 Apr, 2015
 *      Author: dinhtta
 */

#include "sort/SortThread.h"
#include "sort/PriorityQueue.h"
#include <stdio.h>
#include <assert.h>
#include <vector>
#include <algorithm>
#include <deque>
#include <glob.h>
#include <glog/logging.h>
using std::sort;
using std::vector;
using std::deque;

/**
 * Implementation of Sorter.h
 */

namespace sober{

SortThread::SortThread(){
	this->params_= GlobalParams::Get();
	this->encrypted_record_size_ = this->params_->record_size() + GCM_TAG_SIZE
			+ IV_SIZE;
}

/**
 * For each file:
 * Read from the given file to one block.
 * Store in array
 * Use C++ sort
 * Write to output
 */
void SortThread::BlockSort(string *input_path, string *output_path, int n){
	int size = this->encrypted_record_size_*params_->num_records_per_block();
	byte *input = (byte*) malloc(size * sizeof(byte));

	for (int k = 0; k < n; k++) {
		FILE *file = fopen(input_path[k].c_str(), "r");
		assert(file);

		//read the file all in.

		fread(input, 1, size, file);

		vector < string > sort_vector;
		for (int i = 0; i < params_->num_records_per_block(); i++)
			sort_vector.push_back(
					encryptor_.Decrypt(input + i * this->encrypted_record_size_,
							this->encrypted_record_size_));

		struct comp test(params_->key_size());
		sort(sort_vector.begin(), sort_vector.end(), test);
		fclose(file);
		for (int i = 0; i < params_->num_records_per_block(); i++) {
			string cipher = encryptor_.Encrypt((byte*) sort_vector[i].c_str(),
					params_->record_size());
			memcpy(input + i * this->encrypted_record_size_, cipher.c_str(),
					this->encrypted_record_size_);
		}
		file = fopen(output_path[k].c_str(), "w");
		assert(file);
		fwrite(input, 1, size, file);
		fclose(file);
	}
	free(input);
}


/**
 * 1. Create a Priority queue and inset nodes representing input file.
 * While (queue.size>1)
 * 2. Pick up first record, write to FILE
 * 3. Adjust Queue
 *
 * At the end (queue.size = 1), dump to file
 */
void SortThread::Merge(string *input_files, int nstreams, string output_file){
	PriorityQueue merge_heap(params_->merge_factor());
	for (int i=0; i<nstreams; i++){
		merge_heap.Insert(new Node(input_files[i].c_str()));
		LOG(INFO) << "Merging file ... "<<input_files[i];
	}

	FILE *file = fopen(output_file.c_str(), "w");
	assert(file);

	while (merge_heap.GetCurrentSize()>1){
		char *data = merge_heap.Next();
		fwrite(data, 1, this->encrypted_record_size_, file);
		merge_heap.AdjustQueue();
	}
	//copy the rest to the file
	merge_heap.DumpToFile(file);
	fclose(file);
}


}


