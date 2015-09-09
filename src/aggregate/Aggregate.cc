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
#include "aggregate/Aggregate.h"
using namespace std;


namespace sober{




	Aggregate::Aggregate (Encryptor *encryptor_object, int cipher_record_size, int plain_record_size){
		this->encryptor = encryptor_object;
		this->cipher_item_size = cipher_record_size;
		this->plain_item_size = plain_record_size;
	}
	
	
	long long Aggregate::count_element(char *filename){
		struct stat st;
		stat(filename, &st);
		long long size = st.st_size;
		long long count = size / (sizeof(char)*cipher_item_size);

		return count;
	}

	
	int Aggregate::Scan(char *input_file_name,  char *fileO){
		double inputsize = count_element(input_file_name);
		
		
		long long reencrypt = 0;

	
		FILE *fpi;

		FILE *fpo;

		fpi = fopen(input_file_name, "r+");
		fpo = fopen(fileO, "w+");


		

		int i,j,k;
		char temp[cipher_item_size];
		char dummy[plain_item_size];
		string plaintext;
		

		fseek (fpi , 0 , SEEK_SET );
		fseek(fpo, 0, SEEK_SET);	
		for (i=0; i<inputsize; i++){
			fread(temp, 1, cipher_item_size, fpi);
			plaintext = encryptor->Decrypt((byte *)temp, cipher_item_size);
			fwrite(encryptor->Encrypt((byte *) plaintext.c_str(), plain_item_size).c_str(), 1, cipher_item_size, fpo);
			reencrypt++;
			
			

		}
		



		fclose(fpo);
		fclose(fpi);
		cout << reencrypt << "  re-encryptions \n";
		

	}
}
