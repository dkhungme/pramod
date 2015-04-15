/*
 * Sorter.cc
 *
 *  Created on: 5 Apr, 2015
 *      Author: dinhtta
 */

#include <vector>
#include <thread>
#include <deque>
#include <sort/Sorter.h>
#include <glog/logging.h>
using std::thread;
using std::vector;
using std::deque;
/**
 * Multi-threaded merge sort
 */

namespace sober{

Sorter::Sorter(){
	this->nthreads_ = GlobalParams::Get()->nthreads();
	this->params_= GlobalParams::Get();
		this->encrypted_record_size_ = this->params_->record_size() + GCM_TAG_SIZE
				+ IV_SIZE;
}

/**
 * Merge-sorting the entire directory, using nthreads.
 *
 * 0. Block sort the inputs.
 * 1. Create a queue of file to be merged.
 * 2. Pick merge_factor files from the queue to merge. The result is inserted back to the queue.
 * 3. Repeat until one file left on the queue.
 *
 */
void Sorter::MergeSort(){

	deque < string > sorted_file_names;
	deque < thread > sort_threads;
	vector <SortThread*> thread_objs;
	vector<string> merge_intermediates;

	int nmixers = params_->num_mixer_nodes();
	int nfiles_per_thread = (nmixers < nthreads_) ? 1 : nmixers / nthreads_;
	string inputs[nmixers], outputs[nmixers], merge_files[nmixers];

	// Block sort individual files
	// output goes to "sorted_id"
	char input_files[nmixers][256];
	char output_files[nmixers][256];

	double start = Now();
	for (int i = 0; i < nmixers; i++) {
		sprintf(input_files[i], "%s/tmp_%d_%d",
				params_->tmp_data_path().c_str(), (params_->num_rounds() - 1),
				i);
		sprintf(output_files[i], "%s/sorted_%d",
				params_->tmp_data_path().c_str(), i);
		inputs[i] = string(input_files[i]);
		outputs[i] = string(output_files[i]);
		sorted_file_names.push_back(string(output_files[i]));
	}

	// launch thread doing block sorting
	for (int i = 0; i < nthreads_ && i<nmixers; i++) {
		thread_objs.push_back(new SortThread());
		sort_threads.push_back(
				thread(&SortThread::BlockSort, thread_objs[i],
						&inputs[i * nfiles_per_thread],
						&outputs[i * nfiles_per_thread], nfiles_per_thread));
	}

	//wait for them to join
	for (int i=0; i<nthreads_ && i<nmixers; i++){
		sort_threads.front().join();
		sort_threads.pop_front();
	}

	LOG(INFO) << "Finish block sorting ... in "<< (Now()-start) << " nmixers = " <<nmixers << " nthreads = "<<nthreads_;
	// Merge sorting. Use nthreads, and sync
	int pass=0;
	char tmp_output[256];

	while (sorted_file_names.size()>1){
		int merge_file_idx=0;
		//launch nthreads
		for (int k = 0; k < nthreads_ && sorted_file_names.size() > 1; k++) {
			int nstreams =
					params_->merge_factor() < sorted_file_names.size() ?
							params_->merge_factor() : sorted_file_names.size();
			if (nstreams == sorted_file_names.size() && nstreams>2)
				nstreams = 2;

			sprintf(tmp_output, "%s/intermediate_%d_%d",
					params_->tmp_data_path().c_str(), pass,k);

			string out_file(tmp_output);
			merge_intermediates.push_back(out_file);

			string current_inputs[nstreams];
			sprintf(tmp_output, "%s/intermediate_%d",
					params_->tmp_data_path().c_str(), pass);
			int starting_idx = merge_file_idx;

			for (int i = 0; i < nstreams; i++) {
				merge_files[merge_file_idx] = sorted_file_names.front();
				sorted_file_names.pop_front();
				merge_file_idx++;
			}

			sort_threads.push_back(
					thread(&SortThread::Merge, thread_objs[k], &merge_files[starting_idx],
							nstreams, out_file));
		}

		//wait for join, push the intermediate file in the queue, then
		//move to next pass
		int k=0;
		while (!sort_threads.empty()){
			sort_threads.front().join();
			sprintf(tmp_output, "%s/intermediate_%d_%d",
								params_->tmp_data_path().c_str(), pass,k);
			sorted_file_names.push_back(string(tmp_output));
			k++;
			sort_threads.pop_front();
		}

		pass++;
	}

	//rename the remaining file to tmp/data_mixed_sorted.final
	char final_name[256];
	sprintf(final_name, "%s/data_mixed_sorted.final", params_->tmp_data_path().c_str());
	rename(merge_intermediates.back().c_str(), final_name);
	merge_intermediates.pop_back();

	//delete temp files
	while (merge_intermediates.size()>0){
		remove(merge_intermediates.back().c_str());
		merge_intermediates.pop_back();
	}
}

bool Sorter::Validate(char *input_path) {
	FILE *file = fopen(input_path, "r");
	assert(file);
	int size = this->encrypted_record_size_ * params_->num_records_per_block();

	//read the file all in.
	byte *input = (byte*) malloc(size * sizeof(byte));
	fread(input, 1, size, file);

	vector < string > sort_vector;
	for (int i = 0; i < params_->num_records_per_block(); i++)
		sort_vector.push_back(
				encryptor_.Decrypt(input + i * this->encrypted_record_size_,
						this->encrypted_record_size_));
	free(input);
	for (int i=0; i<params_->num_records_per_block()-1; i++)
		if (strncmp(sort_vector[i].c_str(), sort_vector[i+1].c_str(), params_->record_size())>0){
			fclose(file);
			return false;
		}
	fclose(file);
	return true;
}
}
