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

/**
 * For each file:
 * Read from the given file to one block.
 * Store in array
 * Use C++ sort
 * Write to output
 */
void SortThread::BlockSort(string *input_path, string *output_path, int n){
	int size = num_records_*record_size_;
	byte *input = (byte*) malloc(size * sizeof(byte));

	for (int k = 0; k < n; k++) {
		FILE *file = fopen(input_path[k].c_str(), "r");
		assert(file);

		//read the file all in.

		fread(input, 1, size, file);

		vector < string > sort_vector;
		for (int i = 0; i < num_records_; i++)
			sort_vector.push_back(
					mode_==ENCRYPT ? encryptor_.Decrypt(input + i * record_size_,
							record_size_)
					: string(input+i*record_size_, record_size_));

		struct comp test(plaintext_size_);
		sort(sort_vector.begin(), sort_vector.end(), test);
		fclose(file);
		for (int i = 0; i < num_records_; i++) {
			string cipher = mode_==ENCRYPT ? encryptor_.Encrypt((byte*) sort_vector[i].c_str(),
					params_->record_size())
					: sort_vector[i];
			memcpy(input + i * record_size_, cipher.c_str(), record_size_);
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
	PriorityQueue merge_heap(GlobalParams::Get()->merge_factor(), record_size_, plaintext_size_);
	for (int i=0; i<nstreams; i++){
		merge_heap.Insert(new Node(input_files[i].c_str(), &encryptor_, record_size_, plaintext_size_, mode_));
		LOG(INFO) << "Merging file ... "<<input_files[i] << " to " << output_file;
	}

	FILE *file = fopen(output_file.c_str(), "w");
	assert(file);

	while (merge_heap.GetCurrentSize()>1){
		char *data = merge_heap.Next();
		string cipher = mode==ENCRYPT ? encryptor_.Encrypt((byte*)data, plaintext_size_)
				: string(data, record_size_);

		fwrite(cipher.c_str(), 1, cipher.length(), file);
		merge_heap.AdjustQueue();
	}
	//copy the rest to the file
	merge_heap.DumpToFile(file);
	fclose(file);
}


}


