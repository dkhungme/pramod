/*
 * Encryptor.h
 *
 *  Created on: 28 Mar, 2015
 *      Author: dinhtta
 */

#ifndef ENCRYPTOR_H_
#define ENCRYPTOR_H_

#include <crypto++/gcm.h>
#include <crypto++/aes.h>
#include <crypto++/osrng.h>
#include <string.h>

using std::string;
using namespace CryptoPP;


/**
 * Class that does encryption and re-encryption. According to CPP documentation,
 * encryption/decryption should be thread-safe at the class level. Hence each thread
 * uses a different Encryptor object.
 *
 * Encryption is GCM-AES, and reads its key from key_path_
 */

#define IV_SIZE 16 //in GCM, IV can be of arbitrary length
#define AES_BLOCK_SIZE 16
#define GCM_TAG_SIZE 16

namespace sober{
class Encryptor{
public:
		Encryptor();
		/**
		 * Produce <IV><ciphertext> from plaintext
		 */
		string Encrypt(byte *input, int size);

		/**
		 * Decrypt <IV><ciphertext> from ciphertext
		 */
		string Decrypt(byte *input, int size);

		/**
		 * Decrypt, then Encrypt
		 */
		string ReEncrypt(byte *input, int size);

		/**
		 * Hash
		 */
		string Hash(byte *input, int size);

		static long num_encrypts, num_decrypts; 
private:
		GCM<AES>::Encryption encryption_;
		GCM<AES>::Decryption decryption_;
		string enc_key_;
		AutoSeededRandomPool rng_;
};
}

#endif /* ENCRYPTOR_H_ */
