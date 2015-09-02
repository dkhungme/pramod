#ifndef OJOIN_H_
#define OJOIN_H_


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
#include <iostream>
#include <map>
#include "encrypt/Encryptor.h"

using namespace std;

namespace sober{
	class OJoin{
	private:
		int cipher_item_size;
		int plain_item_size;
		static bool compare(std::pair<std::string ,int> i, pair<std::string, int> j);

		Encryptor* encryptor;
	public:
		int ob_Expand(FILE *fpi, FILE *fpo, std::vector<int> weights);
		OJoin(Encryptor *encryptor_object, int cipher_record_size, int plain_record_size);


	};

}
#endif /* OJOIN_H_ */