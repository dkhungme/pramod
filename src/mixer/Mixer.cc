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

	int new_size=0; 
	if (app_func!=NULL){//work_out the size
		string pt = encryptor_.Decrypt((*input)+index[0]*ciphertext_record_size_, ciphertext_record_size_);
		byte *new_pt;
		int out_size, new_pt_size;
		app_func((byte*)pt.c_str(), pt.length(), &new_pt, &out_size, &new_pt_size);

		new_size = out_size + IV_SIZE + GCM_TAG_SIZE;
		//outsize is size of the actual value (not counting the tage/key)
		//new_size is the size of a new ciphertext (e.g in compacting; new_size is size of encrypted value without the tag)
		free(new_pt); 
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
			// if there is no user defined function, re-encrypt the whole thing
		}
		else{
			string pt = encryptor_.Decrypt((*input)+index[i]*ciphertext_record_size_, ciphertext_record_size_);
			//pt is plaintext
			byte *new_pt;
			// new_pt is the "key" part of the record left in plaintext
			int output_size, new_pt_size;
			app_func((byte*)pt.c_str(), pt.length(), &new_pt, &output_size, &new_pt_size);
			/*
			copy the entire plain record (key+value) to new_pt
			outputsize's value is the length of new_pt
			new_pt_size is a length of "key"
			"output_size-new_pt_size" is length of "value"
			*/
			string ct = encryptor_.Encrypt(new_pt+new_pt_size, output_size-new_pt_size);
			/*
			only encrypt the "value" part of the record, which is starting from "new_pt+new_pt_size".
			*/
			
			memcpy(output+i*new_size, new_pt, new_pt_size);
			//write the first "new_pt_size" bytes of new_pt. these bytes are "key" part of the records
			memcpy(output+i*new_size+new_pt_size, ct.c_str(), ct.length());
			// append encrypted "value", which is ct
			free(new_pt); 
			
			
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
