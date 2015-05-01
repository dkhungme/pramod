/*
 * Goodrich.c
 *
 *  Created on: Apr 6, 2015
 *      Author: dinhtta
 */

#include <gflags/gflags.h>
#include <glog/logging.h>
#include "encrypt/Encryptor.h"
#include "sort/Goodrich_Compact.h"
#include "utils/GlobalParams.h"
#include "sort/Sorter.h"
#include <iostream>
 using namespace std; 
 using namespace sober; 
 DEFINE_string(config_file,"config","Where to read the config");
 DEFINE_int32(choice,1,"Size of M"); 
 DEFINE_int32(Bsize,2048,"Size of M"); 
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


 	LOG(INFO)<< "Compacting ";
	//Encryption
 	


 	

 	Encryptor encryptor; 


 	double start = Now();



 	Goodrich_Compact goodrich(&encryptor, ciphertext_size, params->record_size(), FLAGS_Bsize);

 	

 	if(FLAGS_choice == 0){
 		goodrich.Gen_Sample_Data(input_file, "data/sample_file", 134217728, 0.3);
 		cout<< "Finish in generate in " << (Now() - start);
 	}
 	else{
 		
 		char *candidate;
 		candidate =  (char *) malloc (params->record_size());
 		goodrich.gen_empty(candidate);


 		goodrich.do_compaction("data/sample_file", candidate);

 		printf("DONE with compacting and hello from test\n");






 		cout<< "Finish in compacting in .. " << (Now() - start) <<endl;
 		cout << "Number of encryptions = " << Encryptor::num_encrypts <<endl; 
 		cout << "Number of decryptions = " << Encryptor::num_decrypts <<endl; 
 	}


 	return 0;
 }

