/*
 * Mixnet.cc
 *
 *  Created on: 29 Mar, 2015
 *      Author: dinhtta
 */

#include <string.h>
#include <stdio.h>
#include "mixer/Mixnet.h"
#include "mixer/MixThread.h"
#include "utils/GlobalParams.h"
#include "utils/DataGen.h"
#include <glog/logging.h>
#include <assert.h>
#include <thread>
#include <vector>

using std::thread;
using std::string;
using std::vector;

/**
 * Implementation of Mixnet
 */

namespace sober{

Mixnet::Mixnet(){
	this->params_ = GlobalParams::Get();
	ciphertext_block_size_ = (params_->record_size()+GCM_TAG_SIZE+IV_SIZE)*params_->num_records_per_block();
	for (int i=0; i<params_->num_mixer_nodes(); i++)
		mixers_.push_back(new Mixer(i));
}

char *Mixnet::input_file_name(int i){ return input_file_names_.at(i);}
char *Mixnet::output_file_name(int i){ return output_file_names_[i];}

void Mixnet::StartMixing(){
	int round=0;
	int nmixers = this->params_->num_mixer_nodes();

	for (int i=0; i<nmixers; i++){
		char *ip = (char*)malloc(256*sizeof(char));
		sprintf(ip, "%s_%d",this->params_->data_path().c_str(), i);
		input_file_names_.push_back(ip);
	}

	vector<thread> mixer_threads;
	vector<MixThread*> thread_objs;

	//create MixThread objects
	int mixers_per_thread = (nmixers<params_->nthreads()) ? 1: (nmixers/params_->nthreads()); 
	for (int i=0; i<params_->nthreads() && i<nmixers; i++)
		thread_objs.push_back(new MixThread(i*mixers_per_thread, (i+1)*mixers_per_thread));

	double round_start, round_end;
	// at round 0, files name are data_0, data_1,...,
	// at end of round i, mixed blocks are written to tmp_i_0, tmp_i_1,..
	// if round >= 1, delete tmp_(i-1)_0,..tmp_(i-1)_1,..
	while (round < this->params_->num_rounds()){
		round_start = Now();
		for (int i=0; i<nmixers; i++){
			char *op = (char*)malloc(256*sizeof(char));
			sprintf(op, "%s/tmp_%d_%d", this->params_->tmp_data_path().c_str(), round, i);
			output_file_names_.push_back(op);
		}

		//start threads and wait for them to join
		for (int i = 0; i < params_->nthreads() && i<nmixers; i++) {
			mixer_threads.push_back(
					thread(&MixThread::StartMixing, thread_objs[i], this,
							round));
		}

		for (int i=0; i<params_->nthreads() && i<nmixers; i++)
			mixer_threads[i].join();

		for (int i=0; i<params_->nthreads() && i<nmixers; i++)
			mixer_threads.pop_back();

		for (int i = 0; i < nmixers; i++) {
			if (round > 0) //delete the input
				remove(input_file_name(i));

			// output file of this round become input of the next
			strcpy(input_file_name(i), output_file_name(i));
		}

		for (int i=0; i<nmixers; i++)
			output_file_names_.pop_back();

		round_end = Now();
		LOG(INFO) << "Mixing round " << round << " done in "<<(round_end-round_start);
		round++;
	}
	for (int i=0; i<params_->nthreads() && i<nmixers; i++)
			thread_objs.pop_back();

	LOG(INFO) << "Done mixing ...";
	LOG(INFO) << "Number of encryptions = " << Encryptor::num_encrypts; 
	LOG(INFO) << "Number of decryptions = " << Encryptor::num_decrypts; 

}

}

