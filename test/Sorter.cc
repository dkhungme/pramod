/*
 * Sorter.cc
 *
 *  Created on: 4 Apr, 2015
 *      Author: dinhtta
 */

/**
 * Testing sorting of encrypted file
 */
#include <iostream>
#include <gflags/gflags.h>
#include <glog/logging.h>
#include <sort/Sorter.h>
#include <utils/GlobalParams.h>
#include <encrypt/Encryptor.h>
DEFINE_string(config_file,"config","Where to read the config");

using namespace std;
using namespace sober;
int main(int argc, char **argv) {
	gflags::ParseCommandLineFlags(&argc, &argv, true);
	google::InitGoogleLogging(argv[0]);
	FLAGS_logtostderr = 1;

	//work out the prefix to block that have been mixed.
	GlobalParams *params = GlobalParams::Get();

	Sorter sorter;
	double start = Now();
	sorter.MergeSort();
	cout << "Finish merge-sort in .. "<< (Now()-start) << endl;

	LOG(INFO) << "Number of encryptions = " << Encryptor::num_encrypts; 
	LOG(INFO) << "Number of decryptions = " << Encryptor::num_decrypts; 

	char final_output[256];
	sprintf(final_output, "%s/data_mixed_sorted.final",params->tmp_data_path().c_str());
	cout << "  Order? "<<sorter.Validate(final_output)<<endl;
	
	return 0;
}


