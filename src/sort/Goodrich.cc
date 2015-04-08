#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/resource.h>
#include <time.h>
#include <iostream>
using namespace std;

namespace sober{
static int cmpfunc (const void * a, const void * b ) {
	const char *pa = *(const char**)a;
	const char *pb = *(const char**)b;

	return strcmp(pa,pb);
}
Goodrich::Goodrich(Encryptor encryptor_object, int cipher_record_size, int plain_record_size, int memory_capacity){
	this->encryptor = encryptor_object;
	this->M = memory_capacity;
	this->k = 8;
	this->m = 8;
	this->cipher_item_size = cipher_record_size;
	this->plain_item_size = plain_record_size;
	//Note this part
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

int Goodrich::count_element(char *filename){
	struct stat st;
	stat(filename, &st);
	int size = st.st_size;
	int count = size / (sizeof(char)*cipher_item_size);

	return count;
}
int Goodrich::externalSort(FILE *source, long source_offset, long problem_size, FILE *dest,  int merge_depth, int sort_depth, long *dest_offset){

	int s,e;




	int i = 0;

	if(problem_size < M){
		return internalSort(source, source_offset, problem_size, dest, merge_depth, sort_depth, dest_offset);

	}
	else{
		long *list_source_offset, *list_problem_size;
		list_source_offset = static_cast<long*>(malloc((k+1) * sizeof(long)));
		list_problem_size = static_cast<long*>(malloc((k+1) * sizeof(long)));
		create_Sorted_Subproblem(source_offset, problem_size, list_source_offset, list_problem_size);
		long **subproblems_offset;
		subproblems_offset = static_cast<long**>(malloc ((k+1)*sizeof(long *)));
		for (i=0; i<k+1; i++){


			subproblems_offset[i] = static_cast<long*>(malloc(sizeof(long)*2));
		}


		for (i=0; i<k+1; i++){
			externalSort(source, list_source_offset[i], list_problem_size[i], fp[merge_depth][sort_depth+1],
					merge_depth, sort_depth+1, subproblems_offset[i]);
			s = subproblems_offset[i][0]/cipher_item_size;
			e = subproblems_offset[i][1]/cipher_item_size;

		}



		externalMerge(fp[merge_depth][sort_depth+1], subproblems_offset, problem_size, dest, merge_depth, sort_depth, dest_offset);



		free(list_source_offset);
		free(list_problem_size);
		free(subproblems_offset);


		return 0;

	}



}


int Goodrich::internalSort(FILE *source, long source_offset, long problem_size, FILE *dest, int merge_depth, int sort_depth, long *dest_offset){

	if (dest == NULL){
		printf("cannot open %s", filename[merge_depth][sort_depth]);
	}



	int array_size = problem_size;
	int i = 0;
	char **array;
	array = static_cast<char**>(malloc(array_size * sizeof(char*)));
	for(i = 0; i < array_size; i++) {
		array[i] = (char *) malloc(plain_item_size);
	}
	char *temp;
	temp = (char *) malloc(cipher_item_size);

	fseek(source, source_offset, SEEK_SET);
	i = 0;
	while (i<problem_size){

		if (fread (temp,1, cipher_item_size,source)==(cipher_item_size)){
			IOread++;
			if (strcmp(temp, "\n")!=0){
				array[i] = encryptor.Decrypt(temp, cipher_item_size);
				//strcpy(array[i], temp);

			}
		}
		i++;
	}

	qsort(array, array_size, sizeof(char *), cmpfunc);

	fseek(dest, 0, SEEK_END);
	dest_offset[0] = ftell(dest);

	for(i = 0; i<array_size; i++){
		//encryptor.Encrypt(array[i], plain_item_size)
		//fwrite(array[i] , 1 , item_size , dest );
		fwrite(encryptor.Encrypt(array[i], plain_item_size) , 1 , cipher_item_size , dest );


		IOwrite++;

	}
	fseek(dest, 0, SEEK_END);
	dest_offset[1] = ftell(dest);


	free(temp);
	free(array);

	return 0;


}

int Goodrich::internalMerge(FILE *source, long **source_offset, long problem_size, int merge_depth, int sort_depth,FILE *dest, long *dest_offset){
	if (dest == NULL){
		printf("cannot open %s", filename[merge_depth][sort_depth]);
	}


	int i = 0;
	int j = 0;
	int count = 0;
	int sub_problem_size = problem_size / k;
	int last_sub_problem_size = problem_size % k;


	char **array;
	array = static_cast<char**>(malloc(problem_size * sizeof(char*)));
	for(i = 0; i < problem_size; i++) {
		array[i] = (char *) malloc(plain_item_size);
	}

	char *temp;
	temp = (char *) malloc(cipher_item_size);


	for (i=0; i<k+1; i++){

		count = 0;
		fseek(source, source_offset[i][0], SEEK_SET);

		while (ftell(source) < source_offset[i][1]){

			fread (temp,1, cipher_item_size,source);
			if (temp!=NULL && strcmp(temp, "\n")!=0){
				//strcpy(array[j], temp);
				array[j] = encryptor.Decrypt(temp, cipher_item_size);
				j++;
			}
		}

	}

	qsort(array, problem_size, sizeof(char *), cmpfunc);
	fseek(dest, 0, SEEK_END);
	dest_offset[0] = ftell(dest);
	for(i = 0; i<problem_size; i++){

		//fwrite(array[i] , 1 , item_size , dest );
		fwrite(encryptor.Encrypt(array[i], plain_item_size) , 1 , cipher_item_size , dest );
		IOwrite++;
	}
	fseek(dest, 0, SEEK_END);
	dest_offset[1] = ftell(dest);

	free(temp);
	free(array);


	return 0;
}


int Goodrich::externalMerge(FILE *source, long **source_offset, long problem_size, FILE *dest, int merge_depth, int sort_depth, long *dest_offset){

	int i = 0, j=0;
	if (dest == NULL){
		printf("cannot open %s", filename[merge_depth][sort_depth]);
	}
	if (problem_size <= M ){
		return internalMerge(source,  source_offset, problem_size,merge_depth,sort_depth, dest, dest_offset);
	}
	else {

		long ***subproblems_offset;
		long *Merge_subproblem_size;

		subproblems_offset = static_cast<long***>(malloc((m+1)* sizeof(long **)));
		Merge_subproblem_size =  static_cast<long*>(malloc((m+1)* sizeof(long **)));
		for (i =0; i<m+1; i++){
			subproblems_offset[i] = static_cast<long**>(malloc((k+1)* sizeof(long *)));
			for (j = 0; j<k+1; j++){
				subproblems_offset[i][j] = static_cast<long*>(malloc(2* sizeof(long)));
			}
		}

		create_Merged_Subproblems(source, source_offset, Merge_subproblem_size, M_fp[merge_depth+1][sort_depth+1], subproblems_offset);



		long **Merged_subproblems_offset;
		Merged_subproblems_offset = static_cast<long**>(malloc((m+1)*sizeof(long *)));
		for (i=0; i<m+1; i++){
			Merged_subproblems_offset[i] = static_cast<long*>(malloc (2*sizeof(long)));
		}


		for (i = 0; i < m+1; i++) {

			externalMerge(M_fp[merge_depth+1][sort_depth+1], subproblems_offset[i], Merge_subproblem_size[i], fp[merge_depth+1][sort_depth],
					merge_depth+1, sort_depth, Merged_subproblems_offset[i]);

			int s, e;
			s = Merged_subproblems_offset[i][0]/cipher_item_size;
			e =  Merged_subproblems_offset[i][1]/cipher_item_size;


		}

		slidingMerge(fp[merge_depth+1][sort_depth], Merged_subproblems_offset, fp[merge_depth][sort_depth],merge_depth, sort_depth , dest_offset);

		free(subproblems_offset);
		free(Merged_subproblems_offset);
		return 0;
	}
}


int Goodrich::slidingMerge(FILE *source, long **Merged_subproblems_offset, FILE *dest, int merge_depth, int sort_depth, long *dest_offset){

	if (dest == NULL){
		printf("cannot open %s", filename[merge_depth][sort_depth]);
	}

	int consumed_item[m+1];
	int i, j, p, q;
	char *special_value;

	special_value  = (char *) malloc(plain_item_size);
	for (i = 0; i < plain_item_size; ++i) {
		special_value[i] = 'Z';
	}

	special_value[plain_item_size] = 0;

	for (i=0; i<m+1; i++){
		consumed_item[i] = 0;
	}
	char *temp;
	temp = (char *) malloc(cipher_item_size);
	char **array;
	array = static_cast<char**>(malloc( 2*(m+1)*(k+1)*sizeof(char*)));
	for(i = 0; i < 2*(k+1)*(m+1); i++) {
		array[i] = (char *) malloc(plain_item_size);
	}
	p = 0;
	int consumed = 0;

	for (i=0; i<m+1; i++){
		fseek(source, Merged_subproblems_offset[i][0] + consumed_item[i]*cipher_item_size, SEEK_SET);
		j = 0;
		while (j<2*(k+1) && ftell(source) < Merged_subproblems_offset[i][1]){

			fread (temp,1, cipher_item_size,source);
			consumed_item[i]++;
			if (strcmp(temp, "\n")!=0){
				//strcpy(array[p], temp);
				array[p] = encryptor.Decrypt(temp, cipher_item_size);
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
	qsort(array,p, sizeof(char *), cmpfunc);
	for (q = 0; q<(k+1)*(m+1); q++){
		if (strcmp(array[q],special_value)!=0){

			//fwrite(array[q] , 1 , item_size , dest );
			fwrite(encryptor.Encrypt(array[q], plain_item_size) , 1 , cipher_item_size , dest );
			strcpy(array[q],special_value);
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

				fread (temp,1, cipher_item_size,source);

				consumed_item[i]++;
				if (strcmp(temp, "\n")!=0){

					//strcpy(array[p], temp);
					array[p] = encryptor.Decrypt(temp, cipher_item_size);

					IOread++;
					p++;
					j++;
				}

			}
			if (ftell(source) >= Merged_subproblems_offset[i][1]){
				consumed++;
			}
		}


		qsort(array,2*(k+1)*(m+1), sizeof(char *), cmpfunc);
		for (q = 0; q<(k+1)*(m+1); q++){
			if (strcmp(array[q],special_value)!=0){

				//fwrite(array[q] , 1 , item_size , dest );
				fwrite(encryptor.Encrypt(array[q], plain_item_size) , 1 , cipher_item_size , dest );
				strcpy(array[q],special_value);

			}
		}

	}

	for (q = 0; q<2*(k+1)*(m+1); q++){
		if (strcmp(array[q],special_value)!=0){

			//fwrite(array[q] , 1 , item_size , dest );
			fwrite(encryptor.Encrypt(array[q], plain_item_size) , 1 , cipher_item_size , dest );
		}
		else {

		}
	}
	fseek(dest, 0, SEEK_END);
	dest_offset[1] = ftell(dest);





	free(array);
	free(temp);

	return 0;



}

int Goodrich::create_Sorted_Subproblem(long source_offset, long problem_size, long *list_source_offset, long *list_problem_size){

	int i;
	int sub_problem_size;
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

int Goodrich::create_Merged_Subproblems(FILE *source, long **source_offset, long *Merge_subproblem_size, FILE *dest, long ***subproblems_offset){
	//source_offset[i][0]: start offset of sorted array i
	//source_offset[i][1]: end offset of sorted array i

	if (dest==NULL){
		printf("create Merge Subproblem null\n");
	}

	int i, j;
	char *line;
	line = (char *) malloc(cipher_item_size);
	char *temp;
	temp = (char *) malloc(cipher_item_size);

	int jump = m * cipher_item_size;
	for (i = 0; i<m+1; i++){
		for (j = 0; j<k+1; j++){


			long p = source_offset[j][0] + i * cipher_item_size;
			fseek(source, p, SEEK_SET);
			fseek(dest, 0, SEEK_END);
			subproblems_offset[i][j][0] = ftell(dest);
			while(ftell(source)<source_offset[j][1]){
				fread (temp,1, cipher_item_size,source);
				if (strcmp(temp, "\n")!=0){

					fwrite(temp , 1 ,cipher_item_size , dest );
					fseek(source, jump, SEEK_CUR);
				}

			}
			fseek(dest, 0, SEEK_END);
			subproblems_offset[i][j][1] = ftell(dest);

		}

		int s, e;
		s = subproblems_offset[i][0][0]/cipher_item_size;
		e = subproblems_offset[i][k][1]/cipher_item_size;

		Merge_subproblem_size[i] = e - s;

	}

	free(line);
	free(temp);

	return(0);
}



void Goodrich::Sort(char *input, char *output){

	int depth, i, j;

	FILE *source;
	source = fopen(input, "r+");
	int problem_size;

	problem_size = count_element(input);
	depth = 1+ log((double) (problem_size / M)) / log((double)k);
	max_depth = depth;
	printf("%d\n", depth);



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
			merged_filename[i][j] = static_cast<char*>(malloc (sizeof (char)*66));
			filename[i][j] = static_cast<char*>(malloc (sizeof (char)*66));
		}
	}

	strcpy(filename[0][0], output);
	for (i=1; i<depth+1; i++){
		sprintf(filename[0][i], "%s_x", filename[0][i-1]);
	}

	for (j = 0 ; j<depth+1; j++){
		for(i=1; i<depth+1; i++){
			sprintf(filename[i][j], "x_%s", filename[i-1][j]);
		}
	}


	for (j = depth ; j>-1; j--){
		for(i=0; i<depth+1; i++){
			unlink(filename[i][j]);
			fp[i][j] = fopen(filename[i][j], "a+");
		}
	}

	for (j = 0 ; j<depth+1; j++){
		for(i=0; i<depth+1; i++){
			sprintf(merged_filename[i][j], "M_%s", filename[i][j]);


		}
	}
	for (j = depth ; j>-1; j--){
		for(i=0; i<depth+1; i++){
			unlink(merged_filename[i][j]);
			M_fp[i][j] = fopen(merged_filename[i][j], "a+");
		}
	}


	// printf("Finish initializing\n");
	long *dest_offset;
	dest_offset = static_cast<long*>(malloc (2 * sizeof(long)));
	externalSort(source, 0, problem_size, fp[0][0], 0, 0,dest_offset);



	for (j = depth ; j>-1; j--){
		for(i=0; i<depth+1; i++){
			fclose(fp[i][j]);
		}
	}

	printf("HEY, IT'S DONE\n");
	printf("sorted to R with %d items\n", count_element(output));
	strcpy(filename[0][0] , "aaa");
	for (j = depth ; j>-1; j--){
		for(i=0; i<depth+1; i++){
			unlink(filename[i][j]);
			unlink(merged_filename[i][j]);
		}
	}





}




}
