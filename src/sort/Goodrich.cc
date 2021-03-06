#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/resource.h>
#include <time.h>
#include <iostream>
#include "sort/Goodrich.h"
using namespace std;

//re-encryption time: 4.5 * 10^-6 second 
//in order word, 10^6 re-encryption take 4.5 sec

namespace sober{
	static int cmpfunc (const void * a, const void * b ) {
		const char *pa = *(const char**)a;
		const char *pb = *(const char**)b;

		return strcmp(pa,pb);
	}

	int internalSort_count = 0;
	int internalMerge_count = 0;
	int slidingMerge_count = 0;
	int fileopen_count = 0;
	int encrypt = 0;
	int decrypt = 0;

	
	Goodrich::Goodrich(Encryptor* encryptor_object, int cipher_record_size, int plain_record_size, int memory_capacity){
		this->encryptor = encryptor_object;
		this->M = memory_capacity;
		this->k = 4;
		this->m = 4;
		this->cipher_item_size = cipher_record_size;
		this->plain_item_size = plain_record_size;
		this->IOwrite =0;
		this->IOread = 0;
		this->ignore = -1;




	}

	int Goodrich::file_size(char *filename){
		struct stat st;
		stat(filename, &st);
		int size = st.st_size;
		return size;
	}

	long long Goodrich::count_element(char *filename){
		struct stat st;
		stat(filename, &st);
		long long size = st.st_size;
		long long count = size / (sizeof(char)*cipher_item_size);

		return count;
	}
	int Goodrich::externalSort(FILE *source, long long source_offset, long long problem_size, FILE *dest,  int merge_depth, int sort_depth, long long *dest_offset){
		//int s,e;
		int i = 0;
		if(problem_size <= M){
			return internalSort(source, source_offset, problem_size, dest, merge_depth, sort_depth, dest_offset);
		}
		else{
			long long *list_source_offset, *list_problem_size;
			list_source_offset = static_cast<long long*>(malloc((k+1) * sizeof(long long)));
			list_problem_size = static_cast<long long*>(malloc((k+1) * sizeof(long long)));
			create_Sorted_Subproblem(source_offset, problem_size, list_source_offset, list_problem_size);
			long long **subproblems_offset;
			subproblems_offset = static_cast<long long**>(malloc ((k+1)*sizeof(long long *)));
			for (i=0; i<k+1; i++){
				subproblems_offset[i] = static_cast<long long*>(malloc(sizeof(long long)*2));
			}

			fclose(fp[merge_depth][sort_depth+1]);
			unlink(filename[merge_depth][sort_depth+1]);
			fp[merge_depth][sort_depth+1] = fopen(filename[merge_depth][sort_depth+1], "w+");
			fileopen_count++;


			for (i=0; i<k+1; i++){
				externalSort(source, list_source_offset[i], list_problem_size[i], fp[merge_depth][sort_depth+1],
					merge_depth, sort_depth+1, subproblems_offset[i]);
				//s = subproblems_offset[i][0]/cipher_item_size;
				//e = subproblems_offset[i][1]/cipher_item_size;

			}
			externalMerge(fp[merge_depth][sort_depth+1], subproblems_offset, problem_size, dest, merge_depth, sort_depth, dest_offset);

			fclose(fp[merge_depth][sort_depth+1]);
			unlink(filename[merge_depth][sort_depth+1]);
			fp[merge_depth][sort_depth+1] = fopen(filename[merge_depth][sort_depth+1], "w+");
			fileopen_count++;

			free(list_source_offset);
			free(list_problem_size);
			free(subproblems_offset);
			return 0;
		}
	}

	int Goodrich::internalSort(FILE *source, long long source_offset, long long problem_size, FILE *dest, int merge_depth, int sort_depth, long long *dest_offset){
		if (dest == NULL){
			printf("cannot open %s", filename[merge_depth][sort_depth]);
		}
		internalSort_count++;
		long array_size = problem_size;
		long i = 0;
		string* String = new string[problem_size];
		byte *temp;
		temp = (byte *) malloc(cipher_item_size);

		fseek(source, source_offset, SEEK_SET);
		i = 0;
		while (i<problem_size){

			if (fread (temp,1, cipher_item_size,source)==(cipher_item_size)){
				IOread++;

				String[i] = encryptor->Decrypt(temp, cipher_item_size);
				decrypt++;
				
			}
			i++;
		}
		qsort(String, array_size, sizeof(string), cmpfunc);

		fseek(dest, 0, SEEK_END);
		dest_offset[0] = ftell(dest);

		for(i = 0; i<array_size; i++){
			fwrite((encryptor->Encrypt((byte*)String[i].c_str(), plain_item_size)).c_str() , 1 , cipher_item_size , dest );
			encrypt++;
			IOwrite++;

		}
		fseek(dest, 0, SEEK_END);
		dest_offset[1] = ftell(dest);
		free(temp);
		delete[] String;
		return 0;
	}

	int Goodrich::internalMerge(FILE *source, long long **source_offset, long long problem_size, int merge_depth, int sort_depth,FILE *dest, long long *dest_offset){
		if (dest == NULL){
			printf("cannot open %s", filename[merge_depth][sort_depth]);
		}
		internalMerge_count++;
		int i = 0;
		int j = 0;
		int count = 0;
		//int sub_problem_size = problem_size / k;
		//int last_sub_problem_size = problem_size % k;
		string* String = new string[problem_size];
		byte *temp;
		temp = (byte *) malloc(cipher_item_size);
		j = 0;
		for (i=0; i<k+1; i++){

			count = 0;
			fseek(source, source_offset[i][0], SEEK_SET);

			while (ftell(source) < source_offset[i][1]){

				if (fread (temp,1, cipher_item_size,source)==(cipher_item_size)){


					String[j] = encryptor->Decrypt(temp, cipher_item_size);
					decrypt++;
					j++;
				}
			}

		}
		qsort(String, problem_size, sizeof(string), cmpfunc);
		fseek(dest, 0, SEEK_END);
		dest_offset[0] = ftell(dest);
		for(i = 0; i<problem_size; i++){
			fwrite((encryptor->Encrypt((byte*)String[i].c_str(), plain_item_size)).c_str() , 1 , cipher_item_size , dest );
			encrypt++;
			IOwrite++;
		}
		fseek(dest, 0, SEEK_END);
		dest_offset[1] = ftell(dest);
		free(temp);
		delete[] String;
		return 0;
	}

	int Goodrich::externalMerge(FILE *source, long long **source_offset, long long problem_size, FILE *dest, int merge_depth, int sort_depth, long long *dest_offset){
		int i = 0, j=0;
		if (dest == NULL){
			printf("cannot open %s", filename[merge_depth][sort_depth]);
		}
		if (problem_size <= M ){
			return internalMerge(source,  source_offset, problem_size,merge_depth,sort_depth, dest, dest_offset);
		}
		else {
			long long ***subproblems_offset;
			long long *Merge_subproblem_size;
			subproblems_offset = static_cast<long long***>(malloc((m+1)* sizeof(long long **)));
			Merge_subproblem_size =  static_cast<long long*>(malloc((m+1)* sizeof(long long **)));
			for (i =0; i<m+1; i++){
				subproblems_offset[i] = static_cast<long long**>(malloc((k+1)* sizeof(long long *)));
				for (j = 0; j<k+1; j++){
					subproblems_offset[i][j] = static_cast<long long*>(malloc(2* sizeof(long long)));
				}
			}
			fclose(M_fp[merge_depth+1][sort_depth+1]);
			unlink(merged_filename[merge_depth+1][sort_depth+1]);
			M_fp[merge_depth+1][sort_depth+1] = fopen(merged_filename[merge_depth+1][sort_depth+1], "w+");
			fileopen_count++;

			create_Merged_Subproblems(source, source_offset, Merge_subproblem_size, M_fp[merge_depth+1][sort_depth+1], subproblems_offset);
			long long **Merged_subproblems_offset;
			Merged_subproblems_offset = static_cast<long long**>(malloc((m+1)*sizeof(long long *)));
			for (i=0; i<m+1; i++){
				Merged_subproblems_offset[i] = static_cast<long long*>(malloc (2*sizeof(long long)));
			}

			for (i = 0; i < m+1; i++) {

				externalMerge(M_fp[merge_depth+1][sort_depth+1], subproblems_offset[i], Merge_subproblem_size[i], fp[merge_depth+1][sort_depth],
					merge_depth+1, sort_depth, Merged_subproblems_offset[i]);

				//int s, e;
				//s = Merged_subproblems_offset[i][0]/cipher_item_size;
				//e =  Merged_subproblems_offset[i][1]/cipher_item_size;
			}
			slidingMerge(fp[merge_depth+1][sort_depth], Merged_subproblems_offset, fp[merge_depth][sort_depth],merge_depth, sort_depth , dest_offset);

			fclose(M_fp[merge_depth+1][sort_depth+1]);
			unlink(merged_filename[merge_depth+1][sort_depth+1]);
			M_fp[merge_depth+1][sort_depth+1] = fopen(merged_filename[merge_depth+1][sort_depth+1], "w+");
			fileopen_count++;

			fclose(fp[merge_depth+1][sort_depth]);
			unlink(filename[merge_depth+1][sort_depth]);
			fp[merge_depth+1][sort_depth] = fopen(filename[merge_depth+1][sort_depth], "w+");
			fileopen_count++;

			free(subproblems_offset);
			free(Merged_subproblems_offset);
			return 0;
		}
	}

	int Goodrich::slidingMerge(FILE *source, long long **Merged_subproblems_offset, FILE *dest, int merge_depth, int sort_depth, long long *dest_offset){
		if (dest == NULL){
			printf("cannot open %s", filename[merge_depth][sort_depth]);
		}
		slidingMerge_count++;
		long consumed_item[m+1];
		long i, j, p, q;
		char *special_value;
		string cptext;

		special_value  = (char *) malloc(plain_item_size);
		for (i = 0; i < plain_item_size; ++i) {
			special_value[i] = 'Z';
		}

		special_value[plain_item_size] = 0;

		for (i=0; i<m+1; i++){
			consumed_item[i] = 0;
		}
		byte *temp;
		temp = (byte *) malloc(cipher_item_size);
		
		string* String = new string[2*(m+1)*(k+1)];
		p = 0;
		long consumed = 0;
		for (i=0; i<m+1; i++){
			fseek(source, Merged_subproblems_offset[i][0] + consumed_item[i]*cipher_item_size, SEEK_SET);
			j = 0;
			while (j<2*(k+1) && ftell(source) < Merged_subproblems_offset[i][1]){



				if (fread (temp,1, cipher_item_size,source)==(cipher_item_size)){
					consumed_item[i]++;

					String[p] = encryptor->Decrypt(temp, cipher_item_size);
					decrypt++;
					IOread++;
					p++;
					j++;
				}
			}
			if (ftell(source) >= Merged_subproblems_offset[i][1]){
				consumed++;

			}
		}
		fseek(dest, 0, SEEK_END);
		dest_offset[0] = ftell(dest);
		qsort(String, p, sizeof(string), cmpfunc);

		for (q = 0; q<(k+1)*(m+1); q++){
			if (String[q]!=special_value){


				fwrite((encryptor->Encrypt((byte*)String[q].c_str(), plain_item_size)).c_str() , 1 , cipher_item_size , dest );
				encrypt++;
				String[q]=special_value;
				IOwrite++;
			}
		}
		fseek(dest, 0, SEEK_END);
		dest_offset[1] = ftell(dest);
		while (consumed<m+1){
			consumed = 0;
			p = 0;
			for (i=0; i<m+1; i++){

				fseek(source, Merged_subproblems_offset[i][0] + consumed_item[i]*cipher_item_size, SEEK_SET);
				j = 0;
				while (j<k+1 && ftell(source) < Merged_subproblems_offset[i][1]){
					if (fread (temp,1, cipher_item_size,source)==cipher_item_size){
						consumed_item[i]++;		
						String[p] = encryptor->Decrypt(temp, cipher_item_size);
						decrypt++;
						IOread++;
						p++;
						j++;
					}
				}

				if (ftell(source) >= Merged_subproblems_offset[i][1]){
					consumed++;
				}
			}
			qsort(String,2*(k+1)*(m+1), sizeof(string), cmpfunc);
			for (q = 0; q<(k+1)*(m+1); q++){
				
				if (String[q]!=special_value){

					cptext = encryptor->Encrypt((byte*)String[q].c_str(), plain_item_size);
					encrypt++;
					
					fwrite(cptext.c_str() , 1 , cipher_item_size , dest );
					
					String[q]=special_value;

				}
			}
		}
		for (q = 0; q<2*(k+1)*(m+1); q++){
			if (String[q]!=special_value){
				cptext = encryptor->Encrypt((byte*)String[q].c_str(), plain_item_size);
				encrypt++;
				fwrite(cptext.c_str() , 1 , cipher_item_size , dest );
				
			}
			else {
			}
		}
		fseek(dest, 0, SEEK_END);
		dest_offset[1] = ftell(dest);
		free(temp);
		delete[] String;
		return 0;
	}

	int Goodrich::create_Sorted_Subproblem(long long source_offset, long long problem_size, long long *list_source_offset, long long *list_problem_size){

		int i;
		long sub_problem_size;
		sub_problem_size = problem_size / k;
		for (i = 0; i<k; i++){
			list_source_offset[i] = i * sub_problem_size * cipher_item_size+ source_offset;
			list_problem_size[i] = sub_problem_size;
		}
		if ((problem_size % k) == 0 ){
			list_source_offset[k] = ignore;
			list_problem_size[i] = 0;
		}
		else{
			list_source_offset[k] = k * sub_problem_size * cipher_item_size+ source_offset;;
			list_problem_size[i] = problem_size % k;
		}

	}

	int Goodrich::create_Merged_Subproblems(FILE *source, long long **source_offset, long long *Merge_subproblem_size, FILE *dest, long long ***subproblems_offset){

		if (dest==NULL){
			printf("create Merge Subproblem null\n");
		}
		
		int i, j;
		char *line;
		line = (char *) malloc(plain_item_size);
		byte *temp;
		temp = (byte *) malloc(cipher_item_size);

		int jump = m * cipher_item_size;
		for (i = 0; i<m+1; i++){
			for (j = 0; j<k+1; j++){


				long long p = source_offset[j][0] + i * cipher_item_size;
				fseek(source, p, SEEK_SET);
				fseek(dest, 0, SEEK_END);
				subproblems_offset[i][j][0] = ftell(dest);
				while(ftell(source)<source_offset[j][1]){
					fread (temp,1, cipher_item_size,source);
					fwrite(temp , 1 , cipher_item_size , dest );				
					fseek(source, jump, SEEK_CUR);
				}
				fseek(dest, 0, SEEK_END);
				subproblems_offset[i][j][1] = ftell(dest);

			}
			long s, e;
			s = subproblems_offset[i][0][0]/cipher_item_size;
			e = subproblems_offset[i][k][1]/cipher_item_size;
			Merge_subproblem_size[i] = e - s;
		}
		free(line);
		free(temp);
		return(0);
	}

	bool double_is_int(double trouble) {
		double absolute = abs( trouble );
		return absolute == floor(absolute);
	}

	void Goodrich::Sort(char *input, char *output){

		int depth, i, j;
		double d_v;
		FILE *source;
		source = fopen(input, "r+");
		int problem_size;
		//check number of item in input file
		problem_size = count_element(input);
		//calculate recurrent depth
		double num, denominator;
		num = (double) problem_size / (double)M;
		d_v = log(num) / log((double)k);
		//printf("%.12f \n%.12f \n", num, d_v);
		if (double_is_int(d_v)){
			depth = (int) d_v;
			printf("%d\n", depth);
		}
		else {
			depth = 1 + d_v;
			printf("%d\n", depth);

		}




		max_depth = depth;
		depth = 1 + d_v;

		//prepare temporary files
		fp = static_cast<FILE***>(malloc (sizeof (FILE **)*(depth+1)));
		M_fp = static_cast<FILE***>(malloc (sizeof (FILE **)*(depth+1)));
		filename = static_cast<char***>(malloc (sizeof (char **) * (depth +1)));
		merged_filename = static_cast<char***>(malloc (sizeof (char **) * (depth +1)));
		for (i = 0; i<depth + 1; i++){
			fp[i] = static_cast<FILE**>(malloc (sizeof (FILE *) * (depth +1)));
			filename[i] = static_cast<char**>(malloc (sizeof (char *) * (depth +1)));
			merged_filename[i] = static_cast<char**>(malloc (sizeof (char *) * (depth +1)));
			M_fp[i] = static_cast<FILE**>(malloc (sizeof (FILE *) * (depth +1)));
			for (j = 0; j<depth + 1; j++){
				merged_filename[i][j] = static_cast<char*>(malloc (sizeof (char)*256));
				filename[i][j] = static_cast<char*>(malloc (sizeof (char)*256));
			}
		}
		strcpy(filename[0][0], output);
		for (i=1; i<depth+1; i++){
			sprintf(filename[0][i], "%s_x", filename[0][i-1]);
		}

		for (j = 0 ; j<depth+1; j++){
			for(i=1; i<depth+1; i++){
				sprintf(filename[i][j], "%s_t", filename[i-1][j]);
				//sprintf(merged_filename[i][j], "%s_M", filename[i][j]);
			}
		}
		for (j = 0 ; j<depth+1; j++){
			for(i=0; i<depth+1; i++){
				sprintf(merged_filename[i][j], "%s_M", filename[i][j]);
			}
		}
		for (j = depth ; j>-1; j--){
			for(i=0; i<depth+1; i++){
				unlink(filename[i][j]);
				fp[i][j] = fopen(filename[i][j], "w+");
				fileopen_count++;
				unlink(merged_filename[i][j]);
				M_fp[i][j] = fopen(merged_filename[i][j], "w+");
				fileopen_count++;
			}
		}

		// perform sorting
		long long *dest_offset;
		dest_offset = static_cast<long long*>(malloc (2 * sizeof(long long)));
		externalSort(source, 0, problem_size, fp[0][0], 0, 0,dest_offset);

		//close files
		for (j = depth ; j>-1; j--){
			for(i=0; i<depth+1; i++){
				fclose(fp[i][j]);
			}
		}

		printf("HEY, IT'S DONE\n");
		
		printf("sorted to R with %d items\n", count_element(output));
		printf("-internal Sort invoked %d times\n-internal Merge invoked %d times\n-sliding Merge invoked %d times\n-file opened %d times\n",
			internalSort_count, internalMerge_count, slidingMerge_count, fileopen_count);
		printf("#encryption: %d \n#decryption: %d \n", encrypt, decrypt);


		//remove temp files
		strcpy(filename[0][0] , "aaa");
		for (j = depth ; j>-1; j--){
			for(i=0; i<depth+1; i++){
				unlink(filename[i][j]);
				unlink(merged_filename[i][j]);
			}
		}

	}


	



}
