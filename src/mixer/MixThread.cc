/*
 * MixThread.cc
 *
 *  Created on: Apr 1, 2015
 *      Author: dinhtta
 */


#include "mixer/MixThread.h"
#include <stdio.h>
#include <glog/logging.h>
#include "utils/DataGen.h"

namespace sober{

MixThread::MixThread(int start_idx, int end_idx) {
	start_idx_ = start_idx;
	end_idx_ = end_idx;
	this->params_ = GlobalParams::Get();
	ciphertext_block_size_ = (params_->record_size() + GCM_TAG_SIZE + IV_SIZE)
			* params_->num_records_per_block();
	for (int i = start_idx; i < end_idx; i++)
		mixers_.push_back(new Mixer(i));
}

void MixThread::StartMixing(Mixnet *mixnet, int round){

	int nmixers = params_->num_mixer_nodes();

	FILE* input_descriptors[nmixers];
	FILE* output_descriptors[nmixers];

	double io_start, io_end, io_total=0, mix_start, mix_end, mix_total=0;
	//open input files to read
	for (int i = 0; i < nmixers; i++) {
		input_descriptors[i] = fopen(mixnet->input_file_name(i), "r");
		assert(input_descriptors[i]);
	}

	for (int i=start_idx_; i<end_idx_; i++){
		output_descriptors[i-start_idx_] = fopen(mixnet->output_file_name(i), "w");
		assert(output_descriptors[i-start_idx_]);
	}

	for (int i = start_idx_; i < end_idx_; i++) {
		//pull data from all input files in 1 round
		io_start = Now();
		byte *pulled_data = (byte*) malloc(
				ciphertext_block_size_ * sizeof(byte));
		mixers_[i-start_idx_]->GetData(input_descriptors, nmixers, 0, pulled_data);
		io_end = Now();
		io_total += (io_end-io_start);
		mix_start = Now();
		byte *mixed_data = mixers_[i-start_idx_]->Mix(&pulled_data,
				ciphertext_block_size_);
		mix_end = Now();
		mix_total += (mix_end-mix_start);
		//write to file
		fwrite(mixed_data, 1, ciphertext_block_size_, output_descriptors[i-start_idx_]);
		free(mixed_data); //clean memory
	}

	//close file descriptor
	for (int i = 0; i < nmixers; i++) {
		fclose(input_descriptors[i]);
		if (i>=start_idx_ && i<end_idx_){
			fclose(output_descriptors[i-start_idx_]);
		}
	}
	LOG(INFO) << "IO time = " << io_total << " mix time = "<<mix_total << " over "<<(end_idx_-start_idx_) << " mixers";
}

}

