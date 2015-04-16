/**

 * Encryptor.cc
 *
 *  Created on: 28 Mar, 2015
 *      Author: dinhtta
 */

#include "crypto++/files.h"
#include <crypto++/hex.h>
#include "encrypt/Encryptor.h"
#include "utils/GlobalParams.h"
#include <glog/logging.h>

#include <iostream>
#include <stdlib.h>

namespace sober{

Encryptor::Encryptor(){
	GlobalParams *params = GlobalParams::Get();
	FileSource(params->key_path().c_str(), true, new StringSink(enc_key_));

}

string Encryptor::Encrypt(byte *input, int size){
	//init IV
	byte iv[IV_SIZE];
	rng_.GenerateBlock(iv, IV_SIZE);
	string iv_string((char*)iv, IV_SIZE);

	string cipher;
	//set key + value
	encryption_.SetKeyWithIV((byte*)enc_key_.c_str(), AES_BLOCK_SIZE, iv, IV_SIZE);
	//init AuthenticatedEncryption
	AuthenticatedEncryptionFilter authen_enc(encryption_, new StringSink(cipher));
	authen_enc.ChannelPut(DEFAULT_CHANNEL, input, size);
	authen_enc.ChannelMessageEnd(DEFAULT_CHANNEL);
	iv_string+=cipher;
	return iv_string;
}

string Encryptor::Decrypt(byte *input, int size) {
	//extract IV
	string decrypt;
	try {
		decryption_.SetKeyWithIV((byte*) enc_key_.c_str(), AES_BLOCK_SIZE,
				input, IV_SIZE);
		AuthenticatedDecryptionFilter authen_dec(decryption_,
				new StringSink(decrypt));

		authen_dec.ChannelPut(DEFAULT_CHANNEL, input + IV_SIZE,
				size - (IV_SIZE));
		authen_dec.ChannelMessageEnd(DEFAULT_CHANNEL);

	} catch (Exception e) {
		LOG(ERROR) << "Error ... size = " << size << e.what();
		exit(1);
	}

	return decrypt;
}

string Encryptor::ReEncrypt(byte *input, int size){
	string pt = this->Decrypt(input, size);
	string ree =  this->Encrypt((byte*)pt.c_str(), pt.length());
	return ree;
}
}



