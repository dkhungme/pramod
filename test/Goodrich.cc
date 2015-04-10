/*
 * Goodrich.c
 *
 *  Created on: Apr 6, 2015
 *      Author: dinhtta
 */

#include <gflags/gflags.h>
#include <glog/logging.h>
#include "encrypt/Encryptor.h"
#include "sort/Goodrich.h"
#include "utils/GlobalParams.h"
#include "sort/Sorter.h"
#include <iostream>
using namespace std; 
using namespace sober; 
DEFINE_string(config_file,"config","Where to read the config");
DEFINE_int32(msize,100,"Size of M"); 
/**
 * Testing Goodrich's sorting algorithm.
 */

int main(int argc, char **argv) {
	gflags::ParseCommandLineFlags(&argc, &argv, true);
	google::InitGoogleLogging(argv[0]);
	FLAGS_logtostderr = 1;
	GlobalParams * params = GlobalParams::Get();


	//Prepare input + output
	char input_file[256];
	sprintf(input_file, "%s_0", params->data_path().c_str());
	char final_output[256];
	sprintf(final_output, "%s/data_goodrich_sorted.final",
			params->tmp_data_path().c_str());
	int ciphertext_size = params->record_size()+GCM_TAG_SIZE+IV_SIZE;


	LOG(INFO)<< "Sorting " << input_file; 
	//Encryption
	double start = Now();
	Encryptor encryptor; 

	Goodrich goodrich(&encryptor, ciphertext_size, params->record_size(),1000);

	goodrich.Sort(input_file, final_output);
	LOG(INFO)<< "Finish Goodrich sort in .. " << (Now() - start);

	//Validation
	// Sorter sorter;
	// LOG(INFO)<< "  Order? " << sorter.Validate(final_output);

	return 0;
}

