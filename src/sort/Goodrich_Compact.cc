#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/resource.h>
#include <time.h>
#include <iostream>
#include "sort/Goodrich_Compact.h"
using namespace std;

//re-encryption time: 4.5 * 10^-6 second 
//in order word, 10^6 re-encryption take 4.5 sec

namespace sober{
	static int cmpfunc (const void * a, const void * b ) {
		const char *pa = *(const char**)a;
		const char *pb = *(const char**)b;

		return strcmp(pa,pb);
	}

	int empty_cell = 0;

	int B = 65536; //64KB
	int item_size = 132;
	string* String_compact;
	int consolidating_index = 0;
	long *distance_label;
	long no_cells;
	Goodrich_Compact::Goodrich_Compact(Encryptor* encryptor_object, int cipher_record_size, int plain_record_size){
		this->encryptor = encryptor_object;
		
		
		this->cipher_item_size = cipher_record_size;
		this->plain_item_size = plain_record_size;
		this->IOwrite =0;
		this->IOread = 0;
		this->ignore = -1;




	}

	int Goodrich_Compact::file_size(char *filename){
		struct stat st;
		stat(filename, &st);
		int size = st.st_size;
		return size;
	}

	bool Goodrich_Compact::double_is_int(double trouble) {
		double absolute = abs( trouble );
		return absolute == floor(absolute);
	}

	long long Goodrich_Compact::count_element(char *filename){
		struct stat st;
		stat(filename, &st);
		long long size = st.st_size;
		long long count = size / (sizeof(char)*cipher_item_size);

		return count;
	}

	void Goodrich_Compact::gen_empty(char *s) {
		int i;
		for (i = 0; i < plain_item_size; ++i) {
			s[i] = '0';
		}

		s[plain_item_size] = 0;
	}

	void Goodrich_Compact::Gen_Sample_Data(char *input, char *output, long no_item, double ratio){
		FILE *source, *dest;
		source = fopen(input, "r+");
		dest = fopen(output, "w+");
		int i, r;
		int value = (double)1/ratio;
		char *temp;
		temp = (char *) malloc(plain_item_size);
		byte *copy;
		copy = (byte *) malloc(cipher_item_size);
		for(i =0; i<no_item; i++){
			r = rand()%value;
			if (r == 1){
				gen_empty(temp);
				fwrite((encryptor->Encrypt((byte*)temp, plain_item_size)).c_str() , 1 , cipher_item_size , dest );

			}
			else{
				fread (copy,1, cipher_item_size,source);
				fwrite(copy , 1 , cipher_item_size , dest );				

			}
			
		}

	}




	void Goodrich_Compact::consolidate(char *input, char *output, char *del){
		printf("in consolidation\n");

		int distance_label_index = 0;
		FILE *fp_in, *fp_out;


		fp_in = fopen(input, "r");
		cout<< fp_in <<endl;
		fp_out = fopen(output, "w+");
		cout<< fp_out <<endl;
		long input_size = count_element(input);
		no_cells = 1 + input_size/B;
		printf("no cells: %ld\n", no_cells);




		distance_label = static_cast<long*>(malloc (no_cells * sizeof(long )));


		fseek(fp_in, 0, SEEK_END);
		long last_index = ftell(fp_in);
		fseek(fp_in, 0, SEEK_SET);
		String_compact = new string [2*B];

		byte *temp;
		temp = (byte *) malloc(item_size);
		int i;

		string decrypted_temp;

		while(ftell(fp_in)<last_index){


			

			for (i=0; i<B; i++){
				if(ftell(fp_in) < last_index){
					fread (temp,1, item_size,fp_in);
					decrypted_temp = encryptor->Decrypt(temp, cipher_item_size);
					if(decrypted_temp!=del){
						String_compact[consolidating_index] = encryptor->Decrypt(temp, cipher_item_size);
						consolidating_index++;
					}
					else{

					}
				}
			}




			if (consolidating_index >= B){
				for (i=0; i<B; i++){

					fwrite((encryptor->Encrypt((byte*)String_compact[i].c_str(), plain_item_size)).c_str() , 1 , cipher_item_size , fp_out );
				}
				for (i = B; i <= consolidating_index; i++){
					String_compact[i-B] = String_compact[i];
				}
				distance_label[distance_label_index] = empty_cell;
				consolidating_index = consolidating_index-B;


			}
			else{
				for (i=0; i<B; i++){
					fwrite((encryptor->Encrypt((byte*)del, plain_item_size)).c_str() , 1 , cipher_item_size , fp_out );

				}
				distance_label[distance_label_index] = -1;
				empty_cell++;
			}

			distance_label_index++;

		}
		distance_label[distance_label_index] = -1;
		
		for (i = 0; i<B; i++){
			if(i<consolidating_index){

				
				fwrite((encryptor->Encrypt((byte*)String_compact[i].c_str(), plain_item_size)).c_str() , 1 , cipher_item_size , fp_out );
			}
			else{

				fwrite((encryptor->Encrypt((byte*)del, plain_item_size)).c_str() , 1 , cipher_item_size , fp_out );
			}

		}
		distance_label[distance_label_index] = empty_cell;


		printf("about to exit consolidation\n");
		cout<< fp_in <<endl;
		cout<< fp_out <<endl;

		fclose(fp_in);
		cout<<"closed fp_in"<<endl;
		if(fp_out == NULL){
			cout<<"fp_out NULL"<<endl;
		}
		else{
			cout<<"fp_out NOT NULL"<<endl;
		}
		fclose(fp_out);
		cout<<"closed fp_out"<<endl;
		free(temp);

		cout<<"DONE with CONSOLIDATION"<<endl;
		

	}

	
	
	void Goodrich_Compact::one_step_compact(FILE *src, FILE* dest, int level, long *distance_label, long *distance_label2, char *candidate){
		fseek(src, 0, SEEK_SET);

		long cur, until;
		empty_cell = 0;
		int i, j;
		char temp[item_size*B];
		char dummy[item_size];
		int d;
		long dest_index;
		
		long remaining;



		for (i=0; i<no_cells+1; i++){
			distance_label2[i] = -1;
			if (distance_label[i]!=-1){
				fseek(src, i*item_size*B, SEEK_SET);


				long current = ftell(src);

				fseek(src, 0, SEEK_END);
				long end = ftell(src);

				fseek(src, i*item_size*B, SEEK_SET);

				remaining = (end - current)/B;


				d = (int) (pow (2.0, (double)(level+1)));
				long mod = distance_label[i]%d;
				dest_index = (i - mod)*item_size*B;

				cur = ftell(dest);
				fseek(dest, dest_index, SEEK_SET);
				until = ftell(dest);
				fseek(dest, cur, SEEK_SET);

				while(ftell(dest)<until){

					fwrite((encryptor->Encrypt((byte*)candidate, plain_item_size)).c_str() , 1 , cipher_item_size , dest );
				}

				fseek(dest, dest_index, SEEK_SET);
				int re_encrypt;
				

				
				distance_label2[i-mod] = distance_label[i] - mod;
				fread(temp, 1, item_size*B, src);
				fwrite(temp, 1, item_size*B, dest);

				fread(dummy, 1, item_size, src);

				// for(re_encrypt = 0; re_encrypt < B; re_encrypt++){
				// 	//encryptor->ReEncrypt((byte*)dummy, cipher_item_size);
				// }

				
			}
		}
		long count_empty = 0;
		for (i=0; i<no_cells; i++){
			if (distance_label2[i]==-1){
				count_empty++;
			}
		}

		printf("emptycells: %ld\n", count_empty);
		printf("exiting one_step_compact level %d\n", level);

	}

	void Goodrich_Compact::do_compaction(char *input, char *candidate){

		FILE **fp;
		B = 65536;
			// B here is not a size of block in mix-then-sort or M in Goodrich sorting
			// it is just # items per each I/O unit

		long input_size = count_element(input);
		no_cells = 1 + input_size/B;

		double d_v = log((double) no_cells) / log((double)2);
		int depth;
		if (double_is_int(d_v)){
			depth = (int) d_v;
		}
		else {
			depth = 1 + d_v;
		}
		//printf("%d", depth);
		int i;
		char **filename;
		filename = static_cast<char**>(malloc (sizeof (char **) * (depth +1)));
		fp = static_cast<FILE**>(malloc (sizeof (FILE *) * (depth +1)));
		for (i = 0; i<depth + 1; i++){
			filename[i] =static_cast<char*>(malloc (sizeof (char)*66));
			sprintf(filename[i], "data/R%d", i);
			fp[i] = fopen(filename[i], "w+");

		}



		consolidate(input, filename[0], candidate);
		printf("finish with consolidation \n");

		long **d_label;
		d_label = static_cast<long **>(malloc ((depth+1)*sizeof(long *)));


		for(i = 0; i<depth+1; i++){
			d_label[i]= static_cast<long*>(malloc (no_cells * sizeof(long )));
		}
		d_label[0] = distance_label;
		

		long count_empty = 0;
		for (i=0; i<no_cells; i++){
			if (d_label[0][i]==-1){
				count_empty++;
			}
		}

		printf("emptycells: %ld\n", count_empty);

		for (i=0; i<depth; i++){

			printf("%s - %s \n", filename[i], filename[i+1]);


			one_step_compact(fp[i], fp[i+1], i, d_label[i], d_label[i+1], candidate);

			if(i>1){
				fclose(fp[i-1]);	
				unlink(filename[i-1]);

			}

		}
		
		printf("Compacted file is saved in %s\n", filename[depth]);
		free(filename);
		free (fp);
	}

}