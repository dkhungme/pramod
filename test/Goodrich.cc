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
/**
 * Testing Goodrich's sorting algorithm.
 */

int main(int argc, int argv) {
	gflags::ParseCommandLineFlags(&argc, &argv, true);
	google::InitGoogleLogging(argv[0]);
	FLAGS_logtostderr = 1;
	GlobalParams * params = GlobalParams::Get();


	//Prepare input + output
	char input_file[256];
	sprintf(input_file, "%s_0", params->data_path());
	char final_output[256];
	sprintf(final_output, "%s/data_goodrich_sorted.final",
			params->tmp_data_path().c_str());
	int ciphertext_size = params->record_size()+GCM_TAG_SIZE+IV_SIZE;


	//Encryption
	double start = Now();
	Goodrich goodrich;
	Encryptor encryptor;
	goodrich.Sort(string(input_file), string(final_output), params->num_records(), ciphertext_size, encryptor);
	cout << "Finish Goodrich sort in .. " << (Now() - start) << endl;

	//Validation
	Sorter sorter;
	cout << "  Order? " << sorter.Validate(final_output) << endl;

	return 0;
}

