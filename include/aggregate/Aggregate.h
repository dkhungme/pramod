#ifndef AGGREGATE_H_
#define AGGREGATE_H_




#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/resource.h>
#include <time.h>
#include <iostream>
#include <algorithm>    // std::random_shuffle
#include <vector>       // std::vector
#include <ctime>        // std::time
#include <cstdlib>      // std::rand, std::srand
#include "encrypt/Encryptor.h"
using namespace std;

namespace sober{
	class Aggregate{
	private:
		
		
		
		Encryptor* encryptor;

		long long count_element(char *filename);
		
		int cipher_item_size;
		int plain_item_size;

	public:
		Aggregate(Encryptor *encryptor_object, int cipher_record_size, int plain_record_size);
		int Scan(char *input_file_name, char *O);

		



	};

}
#endif /* AGGREGATE_H_ */
