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
 		long long count_element(char *filename);
 		int externalSort(FILE *source, long long source_offset, long long problem_size, FILE *dest,  int merge_depth, int sort_depth, long long *dest_offset);
 		int internalSort(FILE *source, long long source_offset, long long problem_size, FILE *dest, int merge_depth, int sort_depth, long long *dest_offset);
 		int internalMerge(FILE *source, long long **source_offset, long long problem_size, int merge_depth, int sort_depth,FILE *dest, long long *dest_offset);
 		int externalMerge(FILE *source, long long **source_offset, long long problem_size, FILE *dest, int merge_depth, int sort_depth, long long *dest_offset);
 		int slidingMerge(FILE *source, long long **Merged_subproblems_offset, FILE *dest, int merge_depth, int sort_depth, long long *dest_offset);
 		int create_Sorted_Subproblem(long long source_offset, long long problem_size, long long *list_source_offset, long long *list_problem_size);
 		int create_Merged_Subproblems(FILE *source, long long **source_offset, long long *Merge_subproblem_size, FILE *dest, long long ***subproblems_offset);



 		void consolidate(char *input, char *output, char *del);
 		void read_block_in(FILE *in, char *del, long last_index);
 		void write_block_out(FILE *out,  char *del, int index);
 		void one_step_compact(FILE *src, FILE* dest, int level, long *distance_label, long *distance_label2, char *candidate);



 	public:
 		Goodrich(Encryptor *encryptor_object, int cipher_record_size, int plain_record_size, int memory_capacity);


 		void Sort(char *input, char *output);

//////////////////////////////////////////////////////////////////////////////////
 		
 	};
 }

#endif /* GOODRICH_H_ */
