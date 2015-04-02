/*
 * DataGen.cc
 *
 *  Created on: Mar 26, 2015
 *      Author: dinhtta
 */

#include <iostream>
#include <stdio.h>
#include <time.h>
#include <crypto++/osrng.h>
#include <crypto++/files.h>
#include <crypto++/gcm.h>
#include <crypto++/hex.h>
#include <gflags/gflags.h>
#include "utils/DataGen.h"
#include "utils/GlobalParams.h"
#include "encrypt/Encryptor.h"
#include <glog/logging.h>

/**
 * Implementation of DataGen
 */


using namespace CryptoPP;
using namespace std;

DECLARE_string(config_file);

double Now() {
        timespec tp;
        clock_gettime(CLOCK_MONOTONIC, &tp);
        return tp.tv_sec + 1e-9 * tp.tv_nsec;
}

namespace sober{
void DataGen::Generate() {
	GlobalParams *params = GlobalParams::Get();

	AutoSeededRandomPool rng;
	//generate encryption key
	byte enckey[AES_BLOCK_SIZE];
	rng.GenerateBlock(enckey, AES_BLOCK_SIZE);
	StringSource(enckey, AES_BLOCK_SIZE, true,
			new FileSink(params->key_path().c_str()));

	GCM<AES>::Encryption encryption;

	int vs = params->record_size();
	byte val[vs];
	byte iv[IV_SIZE];
	double start, end;

	int block_size = params->num_records_per_block();
	start = Now();

	for (int i = 0; i < params->num_mixer_nodes(); i++) {
		char name[256];
		sprintf(name, "%s_%d", params->data_path().c_str(), i);
		file_ = fopen(name, "w");
		for (int j = 0; j < block_size; j++) {
			string iv_print, cipher;

			//generate record
			rng.GenerateBlock(val, vs);

			//init encryption
			rng.GenerateBlock(iv, IV_SIZE);


			string gcm_cipher;

			encryption.SetKeyWithIV(enckey, AES_BLOCK_SIZE, iv, IV_SIZE);

			AuthenticatedEncryptionFilter aenc(encryption,
					new StringSink(gcm_cipher));

			aenc.ChannelPut(DEFAULT_CHANNEL, val, vs);
			aenc.ChannelMessageEnd(DEFAULT_CHANNEL);


			fwrite(iv, 1, IV_SIZE, file_);
			fwrite(gcm_cipher.c_str(), 1, gcm_cipher.length(), file_);

		}
		fclose(file_);
	}
	end = Now();
	cout << "Time = " << (end - start) << endl;

	string keystring;

	FileSource(params->key_path().c_str(), true, new StringSink(keystring));

}
}



