/*
 * DataGen.h
 *
 *  Created on: Mar 26, 2015
 *      Author: dinhtta
 */

#ifndef DATAGEN_H_
#define DATAGEN_H_

#include <string.h>
using std::string;

/** Generating random data for sorting
 *  Read settings from GlobalParameters
 *  Data is encrypted with GCM, output of the form
 *  	<IV><E(key)>
 *
 *  Encryption key is store at the key_path
 */

double Now();

namespace sober{

class DataGen{
public:
	DataGen():file_(NULL){}
	void Generate();

private:
	FILE* file_;
};
} // namespace sober

#endif /* DATAGEN_H_ */
