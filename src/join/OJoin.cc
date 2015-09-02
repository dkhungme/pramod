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
#include "join/OJoin.h"
#include <map>
using namespace std;


namespace sober{
	OJoin::OJoin (Encryptor *encryptor_object, int cipher_record_size, int plain_record_size){
		this->encryptor = encryptor_object;
		this->cipher_item_size = cipher_record_size;
		this->plain_item_size = plain_record_size;
	}
	

	// static bool OJoin::compare(std::pair<std::string ,int> i, pair<std::string, int> j) {
	// 	return i.second < j.second;
	// }

	typedef std::pair<std::string, int> MyPairType;
	struct CompareSecond
	{
		bool operator()(const MyPairType& left, const MyPairType& right) const
		{
			return left.second < right.second;
		}
	};

	int OJoin::ob_Expand(FILE *fpi, FILE *fpo, std::vector<int> weights){
		double reencrypt = 0;
		int i,j,k;
		double sum = 0;
		double avg;


		for (i=0; i<weights.size(); i++){
			sum += weights[i]; 
		}
		avg = sum / weights.size();

		map<string, int> counter;
		char record[cipher_item_size];
		double curr;

		string output="";

		string rk;
		fseek(fpi, 0, SEEK_SET);
		string plaintext;
		for (i=0; i<weights.size(); i++){

			fread(record, 1, cipher_item_size, fpi);
			plaintext = encryptor->Decrypt((byte *)record, cipher_item_size);



			curr = (int) (i*avg -(i-1)*avg);
			if (weights[i] <= curr){
				for (j=0; j<weights[i]; j++){
					output.append(encryptor->ReEncrypt((byte *)record, cipher_item_size));
					reencrypt++;
				}

				curr = curr - weights[i];
			}
			else{
				counter[string(record)] = weights[i];
			}
			
			while(curr > 0){
				pair<string, int> min = *min_element(counter.begin(), counter.end(), CompareSecond());
				rk = min.first;

				if (counter[rk] > curr){
					for (j=0; j<curr; j++){
						output.append(encryptor->Encrypt((byte *)rk.c_str(), plain_item_size));
						reencrypt++;
					}
					
					counter[rk] = counter[rk] - curr;
					curr = 0;

				}
				
				if (counter[rk] <= curr) {
					for (j=0; j<counter[rk]; j++){
						output.append(encryptor->Encrypt((byte *)rk.c_str(), plain_item_size));
						reencrypt++;
					}
					curr = curr - counter[rk];
					counter.erase(rk);
				}
			}

			fwrite(output.c_str(), 1, output.size(), fpo);
			output = "";

		}

		cout << reencrypt << "  re-encryptions \n";

	}

}