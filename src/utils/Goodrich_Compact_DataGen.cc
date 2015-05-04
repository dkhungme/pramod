#include <iostream>
#include <stdio.h>
#include <time.h>
#include <crypto++/osrng.h>
#include <crypto++/files.h>
#include <crypto++/gcm.h>
#include <crypto++/hex.h>
#include <gflags/gflags.h>
#include "utils/Goodrich_Compact_DataGen.h"
#include "utils/GlobalParams.h"
#include "encrypt/Encryptor.h"
#include <glog/logging.h>

/**
 * Implementation of Goodrich_Compact_DataGen
 */


 using namespace CryptoPP;
 using namespace std;

 DECLARE_string(config_file);

 namespace sober{
 	void Goodrich_Compact_DataGen::gen_empty(byte *s) {
 		int i;
 		for (i = 0; i < 100; ++i) {
 			s[i] = '0';
 		}

 		s[100] = 0;
 	}
 	void Goodrich_Compact_DataGen::Generate(double ratio) {
 		
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
 		int value = (double)1/ratio;

 		int block_size = params->num_records_per_block();
 		start = Now();

 		char *temp;
 		temp = (char *) malloc(100);
 		

 		for (int i = 0; i < params->num_mixer_nodes(); i++) {
 			char name[256];
 			sprintf(name, "%s_%d", params->data_path().c_str(), i);
 			file_ = fopen(name, "w");
 			for (int j = 0; j < block_size; j++) {
 				int r = rand()%value;
 				
 				string iv_print, cipher;

					

 				if (r == 1){
 					gen_empty(val);
 					
 				}
 				else{
 					rng.GenerateBlock(val, vs);
 				}

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



