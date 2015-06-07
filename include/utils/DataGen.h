/*
 * DataGen.cc
 *
 *  Created on: Mar 26, 2015
 *      Author: dinhtta
 */

#ifndef DATAGEN_H_
#define DATAGEN_H_

#include <string.h>
#include <crypto++/osrng.h>

using std::string;
using namespace CryptoPP;

/** Generating random data for sorting
 *  Read settings from GlobalParameters
 *  Data is encrypted with GCM, output of the form
 *  	<IV><E(key)>
 *
 *  Encryption key is store at the key_path
 */
 namespace sober{

 	class DataGen{
 	public:
 		DataGen():file_(NULL){}
 		void Generate();
 		virtual void GenerateBuffer(char *buffer, int size);
 		AutoSeededRandomPool rng_;
 	private:
 		FILE* file_;
 	};
 } 

#endif /* DATAGEN_H_ */
