/*
 * Goodrich.h
 *
 *  Created on: Apr 6, 2015
 *      Author: dinhtta
 */

#ifndef GOODRICH_H_
#define GOODRICH_H_

#include "encrypt/Encryptor.h"
/**
 * Header for the Goodrich's oblivious sort algorithm.
 *
 * Hung: implement this.
 */

namespace sober{
class Goodrich{
private:
	int M, k, m;


	int plain_item_size, cipher_item_size;

	long IOread ;
	long IOwrite;



	long ignore;
	FILE ***fp;

	FILE ***M_fp;


	int max_depth;
	char ***filename;
	char ***merged_filename;
	Encryptor* encryptor;


	int file_size(char *filename);
	int count_element(char *filename);
	int externalSort(FILE *source, long source_offset, long problem_size, FILE *dest,  int merge_depth, int sort_depth, long *dest_offset);
	int internalSort(FILE *source, long source_offset, long problem_size, FILE *dest, int merge_depth, int sort_depth, long *dest_offset);
	int internalMerge(FILE *source, long **source_offset, long problem_size, int merge_depth, int sort_depth,FILE *dest, long *dest_offset);
	int externalMerge(FILE *source, long **source_offset, long problem_size, FILE *dest, int merge_depth, int sort_depth, long *dest_offset);
	int slidingMerge(FILE *source, long **Merged_subproblems_offset, FILE *dest, int merge_depth, int sort_depth, long *dest_offset);
	int create_Sorted_Subproblem(long source_offset, long problem_size, long *list_source_offset, long *list_problem_size);
	int create_Merged_Subproblems(FILE *source, long **source_offset, long *Merge_subproblem_size, FILE *dest, long ***subproblems_offset);



public:
	Goodrich(Encryptor *encryptor_object, int cipher_record_size, int plain_record_size, int memory_capacity);


	void Sort(char *input, char *output);





	/**
	 * Use the Encryptor object to decrypt the file.
	 * See encrypt/Encryptor.h for details.
	 *
	 * encryptor.Decrypt(byte *input, int size);
	 */
	//void Sort(string input, string output, int num_records, int record_size, Encryptor encryptor);
};
}

#endif /* GOODRICH_H_ */
