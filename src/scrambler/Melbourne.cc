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
#include "scrambler/Melbourne.h"
using namespace std;


namespace sober{



	Melbourne::Melbourne (Encryptor *encryptor_object, int cipher_record_size, int plain_record_size){
		this->encryptor = encryptor_object;
		this->cipher_item_size = cipher_record_size;
		this->plain_item_size = plain_record_size;
	}
	
	
	long long Melbourne::count_element(char *filename){
		struct stat st;
		stat(filename, &st);
		long long size = st.st_size;
		long long count = size / (sizeof(char)*cipher_item_size);

		return count;
	}

	int Melbourne::myrandom (int i) { 
		return std::rand()%i;
	}

	std::vector<int> Melbourne::random_permutation(double n){
		std::vector<int> myvector;
		for (int i=0; i<n; ++i) 
			myvector.push_back(i);
		std::random_shuffle ( myvector.begin(), myvector.end() );
		return myvector;
	}


	void Melbourne::gen_item(char *s) {
		int i;
		for (i = 0; i < plain_item_size; ++i) {
			s[i] = '0';
		}
		s[plain_item_size] = 0;
	}

	void Melbourne::gen_dummy(char *s) {
		int i;
		for (i = 0; i < plain_item_size; ++i) {
			s[i] = '1';
		}
		s[plain_item_size] = 0;
	}



	int Melbourne::Scramble(char *input_file_name, char *fileT1, char *fileT2, char *fileO, double p1, double p2){
		double inputsize = count_element(input_file_name);
		int i, j, k, o,p,q;
		double  m;
		double chunks,  chunk_size;
		double buckets, bucket_size;
		double element_per_bucket, buckets_per_chunk;

		int real_element, no_dummy1,no_dummy2;

		char plaintext[plain_item_size];



		m = sqrt(sqrt(inputsize));
		buckets = sqrt(inputsize);
		chunks = sqrt(buckets);	
		element_per_bucket = sqrt(inputsize);	
		chunk_size = element_per_bucket*chunks;
		buckets_per_chunk = sqrt(sqrt(inputsize));

		int max1 = (int) p1*m;
		int max2 = (int) p2*m;



		
		FILE *fpi;

		FILE *fpt1, *fpt2, *fpo;

		fpi = fopen(input_file_name, "r+");
		fpt1 = fopen(fileT1, "w+");
		fpt2 = fopen(fileT2, "w+");
		fpo = fopen(fileO, "w+");


		std::vector<int> myvector = random_permutation(inputsize);

		char *memory;
		memory = (char *) malloc(cipher_item_size*element_per_bucket);

		string* segments;
		segments = new string [(int) chunks];
		for (i = 0; i<chunks; i++){
			segments[i] = "";
		}


		int c;
		char temp[cipher_item_size];
		char dummy[plain_item_size];
		gen_dummy(dummy);

		fseek (fpi , 0 , SEEK_SET );
		fseek(fpt1, 0, SEEK_SET);	
		for (i = 0; i<buckets; i++){
			fseek (fpi , i*cipher_item_size*element_per_bucket , SEEK_SET );
			
			for (j=0; j<element_per_bucket; j++){
				c = myvector[i*buckets+j]/chunk_size;
				fread(temp,1,cipher_item_size, fpi);
				segments[c].append(encryptor->ReEncrypt((byte *)temp, cipher_item_size));
				

			}

			for (j=0; j<chunks; j++){
				real_element = segments[j].size()/cipher_item_size;

				if (real_element>max1){
					printf("%s\n", "FAIL1");
					cout << real_element << ">" << max1 << "\n";
					return 0;
				}
				no_dummy1 = max1 - real_element;
			
				for (k=0; k<no_dummy1; k++){
					segments[j].append(encryptor->Encrypt((byte *)dummy, plain_item_size));
				}

				fwrite(segments[j].c_str(), 1, segments[j].size(), fpt1);
				//cout << "writing";
				segments[j] = "";
			}
		}


		cout << "done with 1st distribution" << "\n";

		
		fseek(fpt1, 0, SEEK_SET);	
		fseek(fpt2, 0, SEEK_SET);	
		char temp_memory [max1*cipher_item_size];

		string s_plaintext;
		

		char *memory2;
		memory2 = (char *) malloc(cipher_item_size*p1*element_per_bucket);
		//myvector = random_permutation(inputsize); 
		for (i=0; i<chunks; i++){
			for (j=0; j<buckets_per_chunk; j++){
				for (k=0; k<chunks; k++){
					fseek(fpt1, (j*buckets_per_chunk + k*chunks + i*max1)*cipher_item_size  ,SEEK_SET);
					for (o=0; o<max1; o++){
						c = myvector[i*buckets+k]/chunk_size;
						fread(temp, 1, cipher_item_size, fpt1);
						s_plaintext = encryptor->Decrypt((byte *)temp, cipher_item_size);

						if (s_plaintext.compare(string(dummy))!=0){
							segments[c].append(encryptor->Encrypt((byte *)s_plaintext.c_str(), plain_item_size));

						}
					}

					for (k=0; k<chunks; k++){
						real_element = segments[k].size()/cipher_item_size;

						if (real_element>max2){
							printf("%s\n", "FAIL2");
							cout << real_element << ">" << max2 << "\n";
							return 0;
						}
						no_dummy2 = max2 - real_element;


						for (o=0; o<no_dummy2; o++){
							segments[k].append(encryptor->Encrypt((byte *)dummy, plain_item_size ));	
						}

						fwrite(segments[k].c_str(), 1, segments[k].size(), fpt2);
						segments[k] = "";
					}
				}
			}
		}

		cout << "done with 2nd distribution" << "\n";

		
		myvector = random_permutation(inputsize); 
		fseek(fpt2, 0, SEEK_SET);	
		fseek(fpo, 0, SEEK_SET);	

		for (i = 0; i<buckets; i++){
			for (j=0; j<buckets_per_chunk; j++){
				fseek(fpt2, (j*buckets_per_chunk + i*max2)*cipher_item_size , SEEK_SET);
				for (o=0; o<max2; o++){
					fread(temp, 1, cipher_item_size, fpt1);
					s_plaintext = encryptor->Decrypt((byte *)temp, cipher_item_size);
					c = myvector[i*buckets+k]/chunk_size;

					if (s_plaintext.compare(string(dummy))!=0){
						segments[0].append(encryptor->Encrypt((byte *)s_plaintext.c_str(), plain_item_size));
					}
				}
			}


			fwrite(segments[0].c_str(), 1, segments[0].size(), fpo);
			segments[0] ="";
		}
		fclose(fpt1);
		fclose(fpt2);
		fclose(fpo);
		fclose(fpi);


	}
}