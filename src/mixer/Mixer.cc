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

byte* Mixer::Mix(byte** input, int size){
	int nrecords = size/ciphertext_record_size_;

	//mix the index
	int index[nrecords];
	for (int i=0; i<nrecords; i++) index[i] = i;


	int j,tmp;
	srandom(time(NULL));
	for (int i=nrecords-1; i>1; i--){
		j=rand()%i;
		tmp=index[i];
		index[i]=index[j];
		index[j]=tmp;
	}


	//create new array
	byte *output = (byte*)malloc(size*sizeof(byte));

	string newcipher;
	for (int i=0; i<nrecords; i++){
		newcipher = encryptor_.ReEncrypt((*input)+index[i]*ciphertext_record_size_, ciphertext_record_size_);
		memcpy(output+i*ciphertext_record_size_, newcipher.c_str(), ciphertext_record_size_);
	}

	//delete original input at the end
	free(*input);
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
