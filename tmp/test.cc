#include <iostream>
#include <string.h>
#include <crypto++/filters.h>
#include <crypto++/hex.h>
#include <crypto++/modes.h>
#include <crypto++/aes.h>
#include <stdio.h>
#include <crypto++/osrng.h>
#include <crypto++/gcm.h>

#define INPUT "Hello world"
#define AAD_INPUT "add test"
#define AAD_INPUT_WRONG "add test"

#define AES_BLOCK_SIZE 16

using namespace std; 
using namespace CryptoPP; 

void printHex(const byte* s){
	string output; 
	StringSource(s,AES_BLOCK_SIZE, true, new HexEncoder(new StringSink(output))); 
	cout << output << " .. size =  " << output.length() << endl; 
}

string printHex(string s){
	string output; 
	StringSource(s,true,new HexEncoder(new StringSink(output))); 
	return output; 	
}
int main(int argc, char** argv){
		
	//generate random key
	AutoSeededRandomPool rng; 
	byte key[AES_BLOCK_SIZE], iv[AES_BLOCK_SIZE]; 
	rng.GenerateBlock(key,AES_BLOCK_SIZE); 
	rng.GenerateBlock(iv,AES_BLOCK_SIZE); 
	cout << "key:"; 
	printHex(key); 
	cout << "iv:";
	printHex(iv); 

	CBC_Mode<AES>::Encryption enc; 
	enc.SetKeyWithIV(key,AES_BLOCK_SIZE,iv,AES_BLOCK_SIZE); 

	string input(INPUT); 
	string output; 
	StreamTransformationFilter *enc_filter = 
		new StreamTransformationFilter(enc, new StringSink(output)); 

	StringSource(input, true, enc_filter); 

	cout << "ciphertext: "<<printHex(output) << endl; 


	CBC_Mode<AES>::Decryption dec; 
	dec.SetKeyWithIV(key,AES_BLOCK_SIZE,iv,AES_BLOCK_SIZE); 
	string plaintext; 
	StreamTransformationFilter *dec_filter = 
		new StreamTransformationFilter(dec, new StringSink(plaintext)); 

	StringSource(output, true, dec_filter); 
	cout << "ciphertext: "<<plaintext << "size = " << plaintext.length() << endl; 


	//GCM mode
	GCM<AES>::Encryption gcm_enc; 
	gcm_enc.SetKeyWithIV(key, AES_BLOCK_SIZE, iv, AES_BLOCK_SIZE); 
	GCM<AES>::Decryption gcm_dec; 
	rng.GenerateBlock(key,AES_BLOCK_SIZE); 
	gcm_dec.SetKeyWithIV(key, AES_BLOCK_SIZE, iv, AES_BLOCK_SIZE); 


	string gcm_cipher,gcm_pt; 

	AuthenticatedEncryptionFilter *aenc = new AuthenticatedEncryptionFilter(
			gcm_enc, new StringSink(gcm_cipher)); 		
	
	aenc->ChannelPut(AAD_CHANNEL, (byte*)AAD_INPUT, strlen(AAD_INPUT)); 
	aenc->ChannelMessageEnd(AAD_CHANNEL); 

	aenc->ChannelPut(DEFAULT_CHANNEL, (byte*)INPUT, strlen(INPUT)); 
	aenc->ChannelMessageEnd(DEFAULT_CHANNEL); 
	
	cout << "gcm cipher: "<<printHex(gcm_cipher) << " of length = "<<gcm_cipher.length()<<endl; 


	AuthenticatedDecryptionFilter *adec = new AuthenticatedDecryptionFilter(
			gcm_dec, new StringSink	(gcm_pt)); 
	adec->ChannelPut(AAD_CHANNEL, (byte*)AAD_INPUT_WRONG, strlen(AAD_INPUT_WRONG)); 
	adec->ChannelMessageEnd(AAD_CHANNEL); 

	//gcm_cipher[0]=1; 
	adec->ChannelPut(DEFAULT_CHANNEL, (byte*)gcm_cipher.c_str(), gcm_cipher.length()); 
	adec->ChannelMessageEnd(DEFAULT_CHANNEL); 

	cout << "result: "<<adec->GetLastResult() << " pt = " << gcm_pt << endl; 
}
