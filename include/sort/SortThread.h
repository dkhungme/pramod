/*
 * Sorter.h
 *
 *  Created on: 2 Apr, 2015
 *      Author: dinhtta
 */

#ifndef SORTTHREAD_H_
#define SORTTHREAD_H_

#include "utils/GlobalParams.h"
#include "encrypt/Encryptor.h"
#include <string.h>
#include <iostream>
/**
 * One thread that does sorting.It provides two main function:
 * (1) sort encrypted block via quick sort
 * (2) k-way merge sort. k is specified as merge-factor in GlobalParams
 *
 */
namespace sober{

/**
 * Comparator function for std::sort() function
 */

typedef bool (*comp)(string a, string b); 

class SortThread{
public:
	SortThread(int num_records, int record_size, int plaintext_size, data_mode_t mode, comp comparator):
		num_records_(num_records), record_size_(record_size), 
		plaintext_size_(plaintext_size), mode_(mode), comparator_(comparator){}

	/**
	 * Internal sort of n blocks, each given in a file.
	 * Data is decrypted, then sort, then re-encrypted again.
	 * Output is to be written to another file.
	 */
	void BlockSort(string *input, string *output, int n);

	/**
	 * Helper function, to merge nstreams files into the output file
	 */
	void Merge(string *input_files, int nstreams, string output_file);


private:
	Encryptor encryptor_;
	int record_size_, num_records_, plaintext_size_;
	data_mode_t mode_;
	comp comparator_; 
};

}

#endif /* SORTTHREAD_H */
