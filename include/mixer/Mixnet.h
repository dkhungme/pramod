/*
 * Mixnet.h
 *
 *  Created on: 29 Mar, 2015
 *      Author: dinhtta
 */

#ifndef MIXNET_H_
#define MIXNET_H_

#include <vector>
#include "mixer/Mixer.h"
#include "utils/GlobalParams.h"


using std::vector;

/**
 * Network of Mixer node. Number of mixer nodes is the same as number of blocks (nblocks)
 *
 * At round i:
 * 1. If (i=0), read from file data_j (0<=j<=nblocks)
 * 	  else, read from file tmp_(i-1)_j (0<=j<=nblocks)
 *
 * 2. Mixer node m reads the m^(th) partition of the input.
 *
 * 3. A block is created from the read input, to be fed into Mixer::Mix() function.
 *
 * 4. Output from Mixer::Mix() is written to file tmp_i_m, into nblocks partitions.
 * 
 * 5. At the final round, apply the user-given function before encrypted. Default = NULL, 
 * means just doing encryption.  
 *
 * Partition index and size can be worked out from nblocks, record_size and block_size
 *
 * Run sequentially for now. Multi-threading later
 */
namespace sober{
class Mixnet{

public:
	Mixnet(application_fn_t func=NULL);
	void StartMixing();
	application_fn_t GetFunction(){ return func_;}
	char *input_file_name(int i);
	char *output_file_name(int i);
private:
	application_fn_t func_;
	vector<Mixer*> mixers_;
	GlobalParams *params_;
	int ciphertext_block_size_;
	vector <char *> input_file_names_;
	vector <char *> output_file_names_;
};
}

#endif /* MIXNET_H_ */
