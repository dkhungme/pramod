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
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/resource.h>
 DEFINE_string(config_file,"config","Where to read the config");

 using namespace std;
 using namespace sober;



 bool str_comp(string a, string b){
 	return strncmp(a.c_str(), b.c_str(),64)<0; 
 }
 bool str_gt_comp(string a, string b){
 	return strncmp(a.c_str(), b.c_str(),64)>0; 
 }
/*
+ copy "inputsize" bytes of "data" to "output"
+ assign a value of input_size to output_size
+ assign a length of "key" (which is left in plaintext) to plaintext_size
*/
void group_mix_fn(byte *data, int inputsize, byte **output, int *output_size, int *plaintext_size){
	Encryptor encryptor; 
	int key_length = 64;
	byte *key;
	key = (byte *) malloc(key_length);
	memcpy(key, data, key_length);
	string hashed_key  = encryptor.Hash((byte*)key, key_length);
	int hashed_key_length = hashed_key.length();

	*output = (byte*) malloc((inputsize - key_length + hashed_key_length)*sizeof(byte)); 

	

	memcpy(*output, (byte *) hashed_key.c_str(), hashed_key_length); 
	memcpy(*output + hashed_key_length, data+key_length, inputsize-key_length); 


	*output_size = inputsize - key_length + hashed_key_length;
	*plaintext_size = hashed_key_length; // original key size is 10 bytes. hashed key size is 19 bytes. bytes of integer as plaintext	
	
	free(key);
	//delete &hashed_key;
	//delete &encryptor;
	//cout<< hashed_key <<endl;
	

	//memcpy(*output, data, inputsize); 
	//*output_size = inputsize; 

	//*plaintext_size = sizeof(int); // 4 bytes of integer as plaintext	




}

int main(int argc, char **argv) {
	
	
	
	gflags::ParseCommandLineFlags(&argc, &argv, true);
	google::InitGoogleLogging(argv[0]);
	FLAGS_logtostderr = 1;
	
	//work out the prefix to block that have been mixed.
	GlobalParams *params = GlobalParams::Get();
	DataGen_Compact gen;
	gen.Generate();
	LOG(INFO) << "Data generated for Grouping." << endl;
	sleep(2);
	double start = Now();
	LOG(INFO) << "Start mixing "; 

	Mixnet mixnet(&group_mix_fn);
	mixnet.StartMixing();
	LOG(INFO) << "Finish mixing in .. "<< (Now()-start) << endl;
	sleep(2); 

	start = Now();

	int nfile = params->merge_factor();
	int n_mixound = params->num_rounds();

	FILE  *foutput;
	char output[256];
	sprintf(output, "%s/data_mixed_key_hashed.final", params->tmp_data_path().c_str());
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


