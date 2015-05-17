/*
 * Sorter.h
 *
 *  Created on: 5 Apr, 2015
 *      Author: dinhtta
 */

#ifndef SORTER_H_
#define SORTER_H_

#include "sort/SortThread.h"
#include "utils/GlobalParams.h"
#include "encrypt/Encryptor.h"
#include <string.h>

using std::string;

/**
 * Carry out external sorting by using multiple SortThread. 
 * @param record_size: size of elements after the mixer phase 
 * @param plaintex_size: plaintext portion of each element
 * @param mode: ENCRYPT or NO_ENCRYPT
 */

namespace sober{
class Sorter{
public:
	Sorter(int record_size, int plaintext_size, int mode);

	/**
	 * Merge-sort all files resulted from the mixing. This assumes that
	 * the file content are already sorted.
	 */
	void MergeSort();

	/**
	 * Check if the encrypted input file is in sorted order.
	 */
	bool Validate(char *input);

private:
	int nthreads_, record_size_, plaintext_size_, mode_;
	GlobalParams *params_;
	Encryptor encryptor_;
	int encrypted_record_size_;
};
}
#endif /* SORTER_H_ */
