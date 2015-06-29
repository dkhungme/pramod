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
#include <mixer/Mixnet.h>
#include <utils/DataGen_Compact.h>
#include <utils/GlobalParams.h>
#include <encrypt/Encryptor.h>
DEFINE_string(config_file,"config","Where to read the config");

using namespace std;
using namespace sober;

bool int_comp(string a, string b){
		int val_a, val_b; 
		memcpy(&val_a, a.c_str(), sizeof(int)); 
		memcpy(&val_b, b.c_str(), sizeof(int)); 
		return val_a<val_b; 
	}

bool gt_comp(string a, string b){
		int val_a, val_b; 
		memcpy(&val_a, a.c_str(), sizeof(int)); 
		memcpy(&val_b, b.c_str(), sizeof(int)); 
		return val_a>val_b; 

}
/*
+ copy "inputsize" bytes of "data" to "output"
+ assign a value of input_size to output_size
+ assign a length of "key" (which is left in plaintext) to plaintext_size
*/
void compact_mix_fn(byte *data, int inputsize, byte **output, int *output_size, int *plaintext_size){
	*output = (byte*) malloc(inputsize*sizeof(byte)); 
	memcpy(*output, data, inputsize); 
	*output_size = inputsize; 
 
	*plaintext_size = sizeof(int); // 4 bytes of integer as plaintext	
}

int main(int argc, char **argv) {
	
	gflags::ParseCommandLineFlags(&argc, &argv, true);
	google::InitGoogleLogging(argv[0]);
	FLAGS_logtostderr = 1;
	
	//work out the prefix to block that have been mixed.
	GlobalParams *params = GlobalParams::Get();
	DataGen_Compact gen;
	gen.Generate();
	LOG(INFO) << "Data generated for Compaction." << endl;
	sleep(2);
	double start = Now();
	LOG(INFO) << "Start mixing "; 

	Mixnet mixnet(&compact_mix_fn);
	mixnet.StartMixing();
	
	LOG(INFO)  << "Finish mixing in .. "<< (Now()-start) << endl;
	sleep(2); 

	// //compute the encrypted record size after Mixing phase
	// // record_size = plaintext tag size + (original_size - plaintext tag + IV + GCM TAG)
	// //		= original size + IV + GCM TAG
	// int plaintext_size = sizeof(int); 
	// int record_size = params->record_size() + IV_SIZE + GCM_TAG_SIZE; 

	// Sorter sorter(record_size, plaintext_size, NO_ENCRYPT, int_comp);
	// read from file each record of record_size, decrypt it if mode==ENCRYPT or directly process it if mode==NO_ENCRYPT. 
	// Perform the sorting only on plaintext_size bytes.
	//  + in sorting, read-in a record of record_size, decrypt it to get a plaintext of plain_text size, sort based on the entire string
	//  + in compacting, read-in a record of record_size, the first plain_text size of it is plain, so no need to decrypt, only perform 
	//  sorting with respect to these bytes
	
	// double start = Now();
	// sorter.MergeSort();
	// cout << "Finish merge-sort in .. "<< (Now()-start) << endl;

	// LOG(INFO) << "Number of encryptions = " << Encryptor::num_encrypts; 
	// LOG(INFO) << "Number of decryptions = " << Encryptor::num_decrypts; 

	// char final_output[256];
	// sprintf(final_output, "%s/data_mixed_sorted.final",params->tmp_data_path().c_str());
	// cout << "  Order? "<<sorter.Validate(final_output,gt_comp)<<endl;
	
	// return 0;


	start = Now();

	int nfile = params->merge_factor();
	int n_mixound = params->num_rounds();

	FILE  *foutput;
	char output[256];
	sprintf(output, "%s/data_mixed_compact.final", params->tmp_data_path().c_str());
	foutput = fopen(output, "w+");

	char input[nfile][256];
	FILE *finput[nfile];

	int i;
	char scan[132];
	for (i=0; i<nfile; i++){
		
		sprintf(input[i], "%s/tmp_%d_%d", params->tmp_data_path().c_str(), (n_mixound - 1), i);
		finput[i] = fopen(input[i], "r+");
		while(fread (scan, 1, 132,finput[i])){
			fwrite(scan, 1, 132, foutput);
		}
		fclose(finput[i]);
		//free(finput[i]);

	}

	LOG(INFO)  << "Finish linear scan in .. "<< (Now()-start) << endl;

	return 0;
}


