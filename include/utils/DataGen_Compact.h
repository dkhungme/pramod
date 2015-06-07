/*
 * DataGen.cc
 *
 *  Created on: Mar 26, 2015
 *      Author: dinhtta
 */

#ifndef DATAGEN_COMPACT_H_
#define DATAGEN_COMPACT_H_

#include <string.h>
#include "utils/DataGen.h"
using std::string;

/**
 * Override the DataGen method to generate data for Compaction
 *
 * Basically, the record-key will be 1,2,3... Values of 0 at random positions
 * are used to indicate deletion. It is controlled by "drop_rate" variable.
 */
 namespace sober{

 	class DataGen_Compact: public DataGen{
 	public:
 		DataGen_Compact();
 		void GenerateBuffer(char *buffer, int size);
 	};
 } 

#endif /* DATAGEN_H_ */
