/*
 * Goodrich.h
 *
 *  Created on: Apr 6, 2015
 *      Author: dinhtta
 */

#ifndef GOODRICH_COMPACT_H_
#define GOODRICH_COMPACT_H_

#include "encrypt/Encryptor.h"
/**
 * Header for the Goodrich's oblivious sort algorithm.
 *
 * Hung: implement this.
 */

 namespace sober{
 	class Goodrich_Compact{
 	private:
 		int B;


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


 		void consolidate(char *input, char *output, char *del);
 		void read_block_in(FILE *in, char *del, long last_index);
 		void write_block_out(FILE *out,  char *del, int index);
 		void one_step_compact(FILE *src, FILE* dest, int level, long *distance_label, long *distance_label2, char *candidate);
 		bool double_is_int(double trouble);



 	public:
 		Goodrich_Compact(Encryptor *encryptor_object, int cipher_record_size, int plain_record_size);


 		void Sort(char *input, char *output);


 		void Gen_Sample_Data(char *input, char *output, long no_item, double ratio);
 		void gen_empty(char *s);
 		void do_compaction(char *input, char *candidate);

 		





	
	};
}

#endif /* GOODRICH_COMPACT_H_ */
