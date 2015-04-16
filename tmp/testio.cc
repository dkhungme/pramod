#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <glog/logging.h>
#include <gflags/gflags.h>
#include <assert.h>
using namespace std; 


#define FILE_NAME "/data/sober/data_goodrich_sorted.final"
#define OUT_NAME "out"

DEFINE_int32(blocksize,1,"block size to read"); 
DEFINE_int32(filesize,1,"filesize"); 
int main(int argc, char **argv){
	gflags::ParseCommandLineFlags(&argc, &argv, true);
	google::InitGoogleLogging(argv[0]);
	FLAGS_logtostderr = 1;

	int n= FLAGS_filesize/FLAGS_blocksize; 
	FILE *file = fopen(FILE_NAME, "r"); 
	FILE *out = fopen(OUT_NAME, "w"); 

	assert(file);
	char *buffer = (char*)malloc(FLAGS_blocksize*sizeof(char));  
	LOG(INFO) << "Reading " << n << " times"; 
	for (int i=0; i<n; i++){
		fread(buffer,1,FLAGS_blocksize,file);
		fwrite(buffer,1,FLAGS_blocksize,out); 
	}	
	fclose(file); 
	fclose(out); 
	LOG(INFO) << "Read whole file in and out write" ; 
	return 0; 
}
