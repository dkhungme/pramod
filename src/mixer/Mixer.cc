/*
 * Mixer.cc
 *
 *  Created on: 29 Mar, 2015
 *      Author: dinhtta
 */



#include "mixer/Mixer.h"
#include "utils/GlobalParams.h"
#include <cstdlib>
#include <stdio.h>
#include <glog/logging.h>

/**
 * Implementing Mixer function
 */

namespace sober{

Mixer::Mixer(int id){
	id_ = id;
	GlobalParams *param = GlobalParams::Get();
	sub_block_size_ = param->num_records_per_block()/param->num_mixer_nodes();
	ciphertext_record_size_ = param->record_size()+GCM_TAG_SIZE+IV_SIZE;
}

byte* Mixer::Mix(byte** input, int size, application_fn_t app_func, int *output_size){
	int nrecords = size/ciphertext_record_size_;

	//mix the index
	//int index[nrecords];
	int *index = (int*)malloc(nrecords*sizeof(int)); 
	
	for (int i=0; i<nrecords; i++) index[i] = i;


	int j,tmp;
	srandom(time(NULL));
	for (int i=nrecords-1; i>1; i--){
		j=rand()%i;
		tmp=index[i];
		index[i]=index[j];
		index[j]=tmp;
	}

	byte *output;

	if (app_func!=NULL){//work_out the size
		string pt = encryptor_.Decrypt((*input)+index[0]*ciphertext_record_size_, ciphertext_record_size_);
		byte *new_pt;
		int output_size, new_pt_size;
		app_func((byte*)pt.c_str(), pt.length(), &new_pt, &output_size, &new_pt_size);

		int new_size = output_size + IV_SIZE + GCM_TAG_SIZE;
		VLOG(3) << "new size = " << new_size;
		output = (byte*)malloc(new_size*sizeof(byte));
		*ouput_size = new_size*sizeof(byte);
	}
	
	//create new array
	output = (byte*)malloc(size*sizeof(byte));
	*output_size = size*sizeof(byte);
	assert(output);
	
	string newcipher;
	for (int i=0; i<nrecords; i++){
		if (app_func==NULL){
			newcipher = encryptor_.ReEncrypt((*input)+index[i]*ciphertext_record_size_, ciphertext_record_size_);
			memcpy(output+i*ciphertext_record_size_, newcipher.c_str(), ciphertext_record_size_);
		}
		else{
			string pt = encryptor_.Decrypt((*input)+index[i]*ciphertext_record_size_, ciphertext_record_size_);
			byte *new_pt;
			int output_size, new_pt_size;
			app_func((byte*)pt.c_str(), pt.length(), &new_pt, &output_size, &new_pt_size);
			string ct = encryptor_.Encrypt(new_pt+new_pt_size, output_size-new_pt_size);

			memcpy(output+i*new_size, new_pt, new_pt_size);
			memcpy(output+i*new_size+new_pt_size, ct.c_str(), ct.length());
		}
	}

	//delete original input at the end
	free(*input);
	free(index); 
	return output;
}

void Mixer::GetData(FILE** inputs, int nfiles, int start_fidx,  byte *output){
	//for each file, pull a sub-block of size
	//sub_block_size = num_records_per_block/num_mixer_nodes
	//seek to the index (sub_block_size*id) and reads

	//allocating data
	//byte *ret = (byte*)malloc(sub_block_size_*nfiles*ciphertext_record_size_*sizeof(byte));
	for (int i=0; i<nfiles; i++){
		//seek and read
		fseek(inputs[i+start_fidx], id_*ciphertext_record_size_*sub_block_size_, SEEK_SET);
		fread(output+i*sub_block_size_*ciphertext_record_size_,1,ciphertext_record_size_*sub_block_size_,inputs[i+start_fidx]);
	}

}
}
