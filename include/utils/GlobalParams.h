/*
 * GlobalParams.h
 *
 *  Created on: Mar 25, 2015
 *      Author: dinhtta
 */

#ifndef GLOBALPARAMS_H_
#define GLOBALPARAMS_H_

#include <string.h>


/**
 * Singleton for accessing global parameters read from Config object.
 */

using std::string;

namespace sober{
class GlobalParams{
public:
	static GlobalParams* Get();
	string data_path(){return data_path_;}
	string tmp_data_path(){return tmp_data_path_;}
	string key_path(){return key_path_;}
	int num_records(){return num_records_;}
	int record_size(){return record_size_;}
	int num_records_per_block(){return num_records_per_block_;}
	int num_rounds(){return num_rounds_;}
	int num_mixer_nodes(){return num_records_/num_records_per_block_;}
	int nthreads(){return nthreads_;}

private:
	GlobalParams();
	string data_path_, key_path_, tmp_data_path_;
	int num_records_, record_size_, num_records_per_block_, num_rounds_, nthreads_;
};

} // end namespace
#endif /* GLOBALPARAMS_H_ */
