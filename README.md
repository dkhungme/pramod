# sober
Data oblivious sorting over encrypted data

------------------------------
Configuration file: config
+ data path: 			where the encrypted data is
+ tmp_data_path: 		where the sorted/mixed/intermediate data is
+ key_path:			store encryption key
+ num_records: 			total number of records
+ record_size: 			size of a plaintext record
+ key_size: 			size of plaintext record key (only key is sorted) 
+ mixnet_round: 		number of mixing
+ num_records_per_block:	how many records per block
+ nthreads:			threads to run (= number of cores)
+ merge_factor:			how many to merge at the same time (k-way)


----------------------------
Encrypted file format:
	[<iv><GCM AES ciphertext>]

Each encrypted record is of size:
	record_size + GCM_TAG_SIZE (16) + IV_SIZE (16)


---------------------------
1. Generate encrypted data:
	make test_gen -> build/test/datagen

   Data is encrypted in block, indexed from 0

2. Generate and Mix:
	make test_mix -> build/test/mixer

   Data is encrypted in block, indexed from 0
   Mixing is done on encrypted block, final output is at tmp_data_path/tmp_i 
	(one encrypted, mixed block for one original block)

3. Sort: done after mixing (take tmp_i files as input)
	make test_sort -> build/test/sorter
   
   First, sort individual blocks, store results in tmp_data_path/sorted_i
   Next, merge them
        Intermediate file is of the form: tmp_data_path/intermediate_i_j. 
   Final intermediate file:

	data_mixed_sorted.final 



   

