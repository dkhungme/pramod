/*
 * GlobalParams.cc
 *
 *  Created on: Mar 25, 2015
 *      Author: dinhtta
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <gflags/gflags.h>
#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include "utils/GlobalParams.h"
#include "proto/config.pb.h"

DECLARE_string(config_file); /*< path to the file to read Config object */

using namespace google::protobuf::io;
using google::protobuf::TextFormat;

namespace sober{

double Now() {
        timespec tp;
        clock_gettime(CLOCK_MONOTONIC, &tp);
        return tp.tv_sec + 1e-9 * tp.tv_nsec;
}

GlobalParams* GlobalParams::Get(){
	static GlobalParams *gp = new GlobalParams();
	return gp;
}

/**
 * Read from the config file. Initialize members.
 */
GlobalParams::GlobalParams(){
	int fd = open(FLAGS_config_file.c_str(), O_RDONLY);
	assert(fd!=-1);
	Config config;
	TextFormat::Parse(new FileInputStream(fd), &config);
	data_path_ = config.data_path();
	key_path_ = config.key_path();
	tmp_data_path_ = config.tmp_data_path();
	num_records_ = config.num_records();
	record_size_ = config.record_size();
	key_size_ = config.key_size();
	num_records_per_block_ = config.num_records_per_block();
	num_rounds_ = config.mixnet_rounds();
	nthreads_ = config.nthreads();
	merge_factor_ = config.merge_factor();
}
}





