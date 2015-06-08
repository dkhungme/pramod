/*
 * DataGen.cc
 *
 *  Created on: Mar 26, 2015
 *      Author: dinhtta
 */

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <crypto++/osrng.h>
#include <crypto++/files.h>
#include <crypto++/gcm.h>
#include <crypto++/hex.h>
#include <gflags/gflags.h>
#include "utils/DataGen_Compact.h"
#include "utils/GlobalParams.h"
#include "encrypt/Encryptor.h"
#include <glog/logging.h>
#include <time.h>

/**
 * Implementation of DataGen for Compact function. 
 * Each plaintext record is generated as follows:
 *      + First 4 bytes is an incrementing integer value. 
 *      + Remaining (record_size-4) bytes are randoms. 
 *
 * A total number drop_rate*num_records records will have the first 4 bytes with value 0, 
 * representing this record to be dropped/removed. 
 *
 * Then the entire record is encrypted, as with Sorting application. But after the final mixing round, 
 * the 4-byte values will be revealed. 
 */


using namespace CryptoPP;
using namespace std;

DECLARE_string(config_file);
DEFINE_double(drop_rate, 0.01, "rate of data element marked as deleted");
namespace sober{

DataGen_Compact::DataGen_Compact(): DataGen(){
	srand(time(NULL));
}
void DataGen_Compact::GenerateBuffer(char *buffer, int size){
	static int starting_ctr=1;
	rng_.GenerateBlock((byte*) buffer, size); 
	memset(buffer,0,sizeof(int));
	if ((double)rand()/((double)RAND_MAX) >= FLAGS_drop_rate)
		memcpy(buffer,&starting_ctr, sizeof(int));
	starting_ctr++;

	int val; 
	memcpy(&val, buffer, sizeof(int)); 
}
}



