# pramod

------------------------------
Configuration file: config
+ data path: 			where the encrypted data is
+ tmp_data_path: 		where the sorted/mixed/intermediate data is
+ key_path:			store encryption key
+ num_records: 			total number of records
+ record_size: 			size of a plaintext record
+ key_size: 			size of plaintext record key (only key is sorted) 
+ num_records_per_block:	how many records per block
+ nthreads:			threads to run (= number of cores)
+ merge_factor:			how many to merge at the same time (k-way)


----------------------------
Encrypted file format:
	[<iv><GCM AES ciphertext>]

Each encrypted record is of size:
	record_size + GCM_TAG_SIZE (16) + IV_SIZE (16)




   

