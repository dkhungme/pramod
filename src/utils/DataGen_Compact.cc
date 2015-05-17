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
 * Implementation of DataGen
 */


using namespace CryptoPP;
using namespace std;

DECLARE_string(config_file);
DEFINE_double(drop_rate, 0.001, "rate of data element marked as deleted");
namespace sober{

DataGen_Compact::DataGen_Compact(): DataGen(){
	srand(time(NULL));
}
DataGen_Compact::DataGen(char *buffer, int size){
	static int starting_ctr=1;
	rng_.GenerateBlock(buffer, size); 
	memset(buffer,0,sizeof(int));
	if ((double)rand()%((double)RAND_MAX) >= FLAGS_drop_rate)
		memcpy(buffer,&starting_ctr, sizeof(int));
	starting_ctr++;
}
}



