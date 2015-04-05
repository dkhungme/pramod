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
 * Carry out external sorting by using multiple SortThread
 */

namespace sober{
class Sorter{
public:
	Sorter();

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
	int nthreads_;
	GlobalParams *params_;
	Encryptor encryptor_;
	int encrypted_record_size_;
};
}
#endif /* SORTER_H_ */
