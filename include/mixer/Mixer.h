/*
 * Mixer.h
 *
 *  Created on: 29 Mar, 2015
 *      Author: dinhtta
 */

#ifndef MIXER_H_
#define MIXER_H_

#include <stdio.h>
#include "encrypt/Encryptor.h"

/**
 * Mixing functionality, take as input M records to mix.
 */

namespace sober{
class Mixer{
public:
	Mixer(int id);

	/**
	 * pull data from a given number of input files (from start_fidx file index) and store data to the output buffer.
	 * Assume that each mixer node takes an equal number of records. Thus, the size of the output is
	 * nfiles*sub_block_size*ciphertext_size
	 *
	 * where sub_block_size = num_records_per_block/num_mixer_nodes
	 */
	void GetData(FILE** input, int nfiles, int start_fidx, byte *output);

	/**
	 * return the newly mixed input. Take ownership, i.e. delete input.
	 */
	byte* Mix(byte** input, int size);
private:
	Encryptor encryptor_;
	int id_; /**< the ID determines which sub-block to pull from input files */
	int sub_block_size_;
	int ciphertext_record_size_;
};

}

#endif /* MIXER_H_ */
