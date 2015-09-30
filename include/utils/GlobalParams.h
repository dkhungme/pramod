#ifndef GLOBALPARAMS_H_
#define GLOBALPARAMS_H_
#include <crypto++/osrng.h>

#include <string>


/**
 * Singleton for accessing global parameters read from Config object.
 */

using std::string;

namespace sober{

double Now();

enum data_mode_t {ENCRYPT, NO_ENCRYPT};  

/**
 * Application function, to execute at the last mixer round. This has to be passed as input to the Sorter object. 
 * (default value is NULL). 
 *
 * Input is a byte array representing the plaintext, output is another byte array in which:
 *  [0..plaintext_size): plaintext
 *  [plaintext_size, output_size): to be encrypted
 *
 * This user-defined function process plaintext data at the last mixer round, before writting out the data. 
 * Output of this function [plaintext_size, output_size) are encrypted before writting, whereas the [0..plaintext_size)
 * are left in the clear. 
 *
 * In the sorting application, this is NULL, i.e. encrypt the whole thing, no need for extra processing.  
 * In the compact application, this function strip off the tag and return only the content to be encrypted. 
 * In the grouping application, this function transform the plaintext to its hash value. 
 *
 * See compact_mix_fn() inside test/Compact.cc for example. 
 */
typedef void (*application_fn_t)(byte * data, int inputsize, byte **output, int *ouput_size, int *plaintext_size);


/**
 * Comparator function. 
 * It is passed as parameter for initializing Sorter object, and use for std::sort. In this case, 
 * the funciton must implement strict ordering (<, or >)
 *
 * Another used is for validating sort order in Validate() method. In this case, it implements
 * "greater-or-equal than" logic.  
 *
 * See test/Compact.cc, test/Sorter.cc for examples of integer and string comparators. 
 */

typedef bool (*comp)(string a, string b); 

class GlobalParams{
public:
	static GlobalParams* Get();
	string data_path(){return data_path_;}
	string tmp_data_path(){return tmp_data_path_;}
	string key_path(){return key_path_;}
	int num_records(){return num_records_;}
	int record_size(){return record_size_;}
	int key_size(){return key_size_;}
	int num_records_per_block(){return num_records_per_block_;}
	int num_rounds(){return num_rounds_;}
	int num_mixer_nodes(){return num_records_/num_records_per_block_;}
	int nthreads(){return nthreads_;}
	int merge_factor(){return merge_factor_;}

private:
	GlobalParams();
	string data_path_, key_path_, tmp_data_path_;
	int num_records_, record_size_, num_records_per_block_, num_rounds_, nthreads_, key_size_, merge_factor_;
};

} // end namespace
#endif /* GLOBALPARAMS_H_ */
