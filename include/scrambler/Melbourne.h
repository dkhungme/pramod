#ifndef MELBOURNE_H_
#define MELBOURNE_H_




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
	class Melbourne{
	private:
		std::vector<int> random_permutation(double n);
		void gen_item(char *s);
		void gen_dummy(char *s);
		//void gen_data(FILE *fpi, double inputsize);
		
		Encryptor* encryptor;

		long long count_element(char *filename);
		int myrandom (int i);

		int cipher_item_size;
		int plain_item_size;

	public:
		Melbourne(Encryptor *encryptor_object, int cipher_record_size, int plain_record_size);
		int Scramble(char *input_file_name, char *T1, char *T2,char *O, double p1, double p2);

		



	};

}
#endif /* MELBOURNE_H_ */