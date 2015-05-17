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
#include <mix/Mixnet.h>
#include <utils/DataGen_Compact.h>
#include <utils/GlobalParams.h>
#include <encrypt/Encryptor.h>
DEFINE_string(config_file,"config","Where to read the config");

using namespace std;
using namespace sober;

void compact_mix_fn(byte *data, int inputsize, byte **output, int *output_size, int *plaintext_size){
	*output = (byte*) malloc(inputsize*sizeof(byte)); 
	memcpy(*output, data, inputsize); 
	*output_size = input_size; 
 
	*plaintext_size = sizeof(int); // 4 bytes of integer as plaintext	
}

int main(int argc, char **argv) {
	gflags::ParseCommandLineFlags(&argc, &argv, true);
	google::InitGoogleLogging(argv[0]);
	FLAGS_logtostderr = 1;

	//work out the prefix to block that have been mixed.
	GlobalParams *params = GlobalParams::Get();

	sober::DataGen_Compact gen;
	gen.Generate();
	cout << "Data generated for Compaction." << endl;
	sleep(2);


	sober::Mixnet mixnet(&compact_mix_fn);
	mixnet.StartMixing();
	cout << "Mixing successful. "<<endl;
	sleep(2); 

	//compute the encrypted record size after Mixing phase
	// record_size = plaintext tag size + (original_size - plaintext tag + IV + GCM TAG)
	//		= original size + IV + GCM TAG
	int plaintext_size = sizeof(int); 

	Sorter sorter(record_size, plaintext_size, NO_ENCRYPT);
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


