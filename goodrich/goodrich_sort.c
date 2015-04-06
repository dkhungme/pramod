#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/resource.h>
#include <time.h>

int M = 500, k = 12, m = 12;


int item_size = 8;

long IOread = 0;
long IOwrite = 0;

int inter_Count = 0;

long ignore = -1;
FILE ***fp;
char *special_value;
FILE ***M_fp;


int max_depth;
char ***filename;
char ***merged_filename;
/*
- N: #elements to be sorted
- M: internal memory capacity (# items)
- k: branching factor of sorting
- m: branching factor of merging
constraints: 	
+ M > 3B^4
+ M = N^(1/c)
*/




int file_size(char *filename){ 
	struct stat st;
	stat(filename, &st);
	int size = st.st_size;	

	return size;
}


int count_element(char *filename){ 
	struct stat st;
	stat(filename, &st);
	int size = st.st_size;	
	int count = size / (sizeof(char)*8);

	return count;
}

long externalSort(FILE *source, long source_offset, long problem_size, FILE *dest,  int merge_depth, int sort_depth, long *dest_offset){ 

	//dest_offset[0]: start offset in destination file
	//dest_offset[1]: end offset in destination file
	int s,e;
	
	int i = 0;
	
	if(problem_size < M){
		return internalSort(source, source_offset, problem_size, dest, merge_depth, sort_depth, dest_offset);
	
	}	
	else{	
		long *list_source_offset, *list_problem_size; 
		list_source_offset = malloc((k+1) * sizeof(long));
		list_problem_size = malloc((k+1) * sizeof(long));
		create_Sorted_Subproblem(source_offset, problem_size, list_source_offset, list_problem_size);
		long **subproblems_offset;
		subproblems_offset = malloc ((k+1)*sizeof(long *));
		for (i=0; i<k+1; i++){


			subproblems_offset[i] = malloc(sizeof(long)*2);
		}


		for (i=0; i<k+1; i++){
			externalSort(source, list_source_offset[i], list_problem_size[i], fp[merge_depth][sort_depth+1], 
				merge_depth, sort_depth+1, subproblems_offset[i]);	
			s = subproblems_offset[i][0]/item_size;
			e = subproblems_offset[i][1]/item_size;
			
		}


		
		externalMerge(fp[merge_depth][sort_depth+1], subproblems_offset, problem_size, dest,
		 	merge_depth, sort_depth, dest_offset);

		free(list_source_offset);
		free(list_problem_size);
		free(subproblems_offset);


		return 0;
		
	}
		
	

}


int cmpfunc (const void * a, const void * b ) {
	const char *pa = *(const char**)a;
	const char *pb = *(const char**)b;

	return strcmp(pa,pb);
}

int internalSort(FILE *source, long source_offset, long problem_size, FILE *dest, int merge_depth, int sort_depth, long *dest_offset){ 
	
		if (dest == NULL){
			printf("cannot open %s", filename[merge_depth][sort_depth]);
		}
		inter_Count++;


		int array_size = problem_size;
		int i = 0;
		char **array;
		array = malloc(array_size * sizeof(char*));
		for(i = 0; i < array_size; i++) {
			array[i] = (char *) malloc(36);
		}
		char *temp;
		temp = (char *) malloc(36);

		fseek(source, source_offset, SEEK_SET);
		i = 0;
		while (i<problem_size){

			if (fgets (temp, 36, source)!=NULL){
				
				if (strcmp(temp, "\n")!=0){	
					strcpy(array[i], temp);
					IOread++;
					i++;
				}
			}
		}

		qsort(array, array_size, sizeof(char *), cmpfunc);
		fseek(dest, 0, SEEK_END);
		dest_offset[0] = ftell(dest);

		for(i = 0; i<array_size; i++){

			fputs (array[i], dest);
			IOwrite++;

		}
		fseek(dest, 0, SEEK_END);
		dest_offset[1] = ftell(dest);

		
		free(temp);
		free(array);

		
		return 0;
	

}

int internalMerge(FILE *source, long **source_offset, long problem_size, int merge_depth, int sort_depth,FILE *dest, long *dest_offset){ 
	//source_offset[i][0]: starting offset of sub array i
	//source_offset[i][1]: ending offset of sub array i

	//dest[0]: starting_offset of merged array stored in dest
	//dest[1]: ending offset of merged array stored in dest
	if (dest == NULL){
			printf("cannot open %s", filename[merge_depth][sort_depth]);
		}
	
	int i = 0;
	int j = 0;
	int count = 0;
	int sub_problem_size = problem_size / k;
	int last_sub_problem_size = problem_size % k;


	char **array;
	array = malloc(problem_size * sizeof(char*));
	for(i = 0; i < problem_size; i++) {
		array[i] = (char *) malloc(36);
	}

	char *temp;
	temp = (char *) malloc(36);


	for (i=0; i<k; i++){
		count = 0;
		fseek(source, source_offset[i][0], SEEK_SET);
		while (ftell(source) < source_offset[i][1]){
			fgets(temp, 36, source);
			if (strcmp(temp, "\n")!=0){
				strcpy(array[j], temp);
				IOread++;
				j++;	
			}
		}
	}


	count = 0;
	fseek(source, source_offset[k][0], SEEK_SET);
	while (ftell(source) < source_offset[k][1]){

		fgets(temp, 36, source);
		if (strcmp(temp, "\n")!=0){
			strcpy(array[j], temp);
			IOread++;
			j++;	
		}
	}

	qsort(array, problem_size, sizeof(char *), cmpfunc);
	fseek(dest, 0, SEEK_END);
	dest_offset[0] = ftell(dest);
	for(i = 0; i<problem_size; i++){
		fputs(array[i], dest);
		IOwrite++;
	}
	fseek(dest, 0, SEEK_END);
	dest_offset[1] = ftell(dest);

	free(temp);
	free(array);
			
	return 0;
}


int externalMerge(FILE *source, long **source_offset, long problem_size, FILE *dest, int merge_depth, int sort_depth, long *dest_offset){
	
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

		subproblems_offset = malloc((m+1)* sizeof(long **));
		Merge_subproblem_size =  malloc((m+1)* sizeof(long **));
		for (i =0; i<m+1; i++){
			subproblems_offset[i] = malloc((k+1)* sizeof(long *));
			for (j = 0; j<k+1; j++){
				subproblems_offset[i][j] = malloc(2* sizeof(long));
			}
		}

		create_Merged_Subproblems(source, source_offset, Merge_subproblem_size, M_fp[merge_depth+1][sort_depth+1], subproblems_offset);

		

		long **Merged_subproblems_offset;
		Merged_subproblems_offset = malloc((m+1)*sizeof(long *));
		for (i=0; i<m+1; i++){
			Merged_subproblems_offset[i] = malloc (2*sizeof(long));
		}


		for (i = 0; i < m+1; i++) {

			externalMerge(M_fp[merge_depth+1][sort_depth+1], subproblems_offset[i], Merge_subproblem_size[i], fp[merge_depth+1][sort_depth], 
				merge_depth+1, sort_depth, Merged_subproblems_offset[i]);

			int s, e;
			s = Merged_subproblems_offset[i][0]/item_size;
			e =  Merged_subproblems_offset[i][1]/item_size;


		}
		
		slidingMerge(fp[merge_depth+1][sort_depth], Merged_subproblems_offset, fp[merge_depth][sort_depth],merge_depth, sort_depth , dest_offset);
		
		free(subproblems_offset);
		free(Merged_subproblems_offset);
		return 0;
	}
}


int slidingMerge(FILE *source, long **Merged_subproblems_offset, FILE *dest, int merge_depth, int sort_depth, long *dest_offset){
	if (dest == NULL){
			printf("cannot open %s", filename[merge_depth][sort_depth]);
		}
	
	int consumed_item[m+1];
	
	
	
	int i, j, p, q;
	for (i=0; i<m+1; i++){
		consumed_item[i] = 0;
	}
	char *temp;
	temp = (char *) malloc(36);
	char **array;
	array = malloc( 2*(m+1)*(k+1)*sizeof(char*));
	for(i = 0; i < 2*(k+1)*(m+1); i++) {
		array[i] = (char *) malloc(36);
	}
	p = 0;
	int consumed = 0;

	for (i=0; i<m+1; i++){
		fseek(source, Merged_subproblems_offset[i][0] + consumed_item[i]*item_size, SEEK_SET);
		j = 0;
		while (j<2*(k+1) && ftell(source) < Merged_subproblems_offset[i][1]){

			fgets(temp, 36, source);	
			consumed_item[i]++;
			if (strcmp(temp, "\n")!=0){
				strcpy(array[p], temp);
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
			fputs (array[q], dest);
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

			fseek(source, Merged_subproblems_offset[i][0] + consumed_item[i]*item_size, SEEK_SET);
			j = 0;
			while (j<k+1 && ftell(source) < Merged_subproblems_offset[i][1]){

				fgets(temp, 36, source);	

				consumed_item[i]++;	
				if (strcmp(temp, "\n")!=0){

					strcpy(array[p], temp);

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
				fputs (array[q], dest);
				strcpy(array[q],special_value);
				IOwrite++;

			}
		}

	}

	for (q = 0; q<2*(k+1)*(m+1); q++){
		if (strcmp(array[q],special_value)!=0){
			fputs (array[q], dest);
			IOwrite++;
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

int create_Sorted_Subproblem(long source_offset, long problem_size, long *list_source_offset, long *list_problem_size){ 

	int i;
	int sub_problem_size;
	sub_problem_size = problem_size / k;
	for (i = 0; i<k; i++){
		list_source_offset[i] = i * sub_problem_size * item_size+ source_offset;
		list_problem_size[i] = sub_problem_size;
	}
	if ((problem_size % k) == 0 ){
		list_source_offset[k] = ignore;
		list_problem_size[i] = 0;
	}
	else{
		list_source_offset[k] = k * sub_problem_size * item_size+ source_offset;;
		list_problem_size[i] = problem_size % k;
	}

}

int create_Merged_Subproblems(FILE *source, long **source_offset, long *Merge_subproblem_size, FILE *dest, long ***subproblems_offset){
	//source_offset[i][0]: start offset of sorted array i
	//source_offset[i][1]: end offset of sorted array i
	if (dest==NULL){
		printf("create Merge Subproblem null\n");
	}

	int i, j;
	char *line;
	line = (char *) malloc(36);
	char *temp;
	temp = (char *) malloc(36);

	int jump = m * item_size;
	for (i = 0; i<m+1; i++){
		for (j = 0; j<k+1; j++){
				

				long p = source_offset[j][0] + i * item_size;
				fseek(source, p, SEEK_SET);
				fseek(dest, 0, SEEK_END);
				subproblems_offset[i][j][0] = ftell(dest);
				while(ftell(source)<source_offset[j][1]){
					fgets (temp, 36, source);
					if (strcmp(temp, "\n")!=0){
						IOread++;
						fputs(temp, dest);
						fseek(source, jump, SEEK_CUR);
						IOwrite++;
					}

				}
				fseek(dest, 0, SEEK_END);
				subproblems_offset[i][j][1] = ftell(dest);
			
		}

		int s, e;
		s = subproblems_offset[i][0][0]/item_size;
		e = subproblems_offset[i][k][1]/item_size;

		Merge_subproblem_size[i] = e - s;
	
	}
	
	free(line);
	free(temp);
	return(0);
}

int count_occurence(const char *pString, char c){ 
	int i=-1, count = 0;
	while(pString[++i])
	{
		if(pString[i] == c) count++;
	}  
	return count;
}



int main ()
{

	int depth, i, j;
	FILE *source;
	source = fopen("file", "r+");
	int problem_size;
	problem_size = count_element("file");
	depth = 1+ log((double) (problem_size / M)) / log((double)k);
	max_depth = depth;
	special_value  = (char *) malloc(36);
	strcpy(special_value, "ZZZZZZZ");
	
	fp = malloc (sizeof (FILE **)*(depth+1));
	M_fp = malloc (sizeof (FILE **)*(depth+1));
	filename = malloc (sizeof (char **) * (depth +1));
	merged_filename = malloc (sizeof (char **) * (depth +1));
	for (i = 0; i<depth + 1; i++){
		fp[i] = malloc (sizeof (FILE *) * (depth +1));
		filename[i] = malloc (sizeof (char *) * (depth +1));
		merged_filename[i] = malloc (sizeof (char *) * (depth +1));
		M_fp[i] = malloc (sizeof (FILE *) * (depth +1));
		for (j = 0; j<depth + 1; j++){
			merged_filename[i][j] = malloc (sizeof (char)*66);
			filename[i][j] = malloc (sizeof (char)*66);
		}
	}

	

	strcpy(filename[0][0], "R");
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


	long *dest_offset;
	dest_offset = malloc (2 * sizeof(long));
	externalSort(source, 0, problem_size, fp[0][0], 0, 0,dest_offset);



	for (j = depth ; j>-1; j--){
		for(i=0; i<depth+1; i++){
			fclose(fp[i][j]);
		}
	}
	
	printf("Recurrence Depth %d\n", depth);
	printf("HEY, IT'S DONE\n");
	printf("sorted to R with %d items\n", count_element("R"));
	strcpy(filename[0][0] , "aaa");
	for (j = depth ; j>-1; j--){
		for(i=0; i<depth+1; i++){
			unlink(filename[i][j]);
			unlink(merged_filename[i][j]);
		}
	}

	printf("IOwrite: %ld\nIOread: %ld\n", IOwrite, IOread);
	return 0;

}