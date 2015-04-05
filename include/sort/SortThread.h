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
struct comp{
	int size;
	comp(int x): size(x){}

	bool operator()(string a, string b){
		return strncmp(a.c_str(), b.c_str(),size)<0;
	}
};

class SortThread{
public:
	SortThread();

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
	GlobalParams *params_;
	Encryptor encryptor_;
	int encrypted_record_size_;
};

}

#endif /* SORTTHREAD_H */
