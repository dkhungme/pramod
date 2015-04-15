#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

using namespace std; 

//Test on file descriptors. 
//Usage: test #num_of_open_operations &
//       lsof -p <processID>

string FILE_NAME="../data/data_0"; 

int main(int argc, char **argv){
	if (argc!=2){
		cout << "test num " << endl; 
		exit(1); 
	}
	int n = atoi(argv[1]); 
	for (int i=0; i<n; i++){
		FILE *file = fopen(FILE_NAME.c_str(), "rw"); 
		assert(file); 
		sleep(1); 
		fclose(file); 
	}
	cout << "Bye!"<< endl; 
	return 0; 
}
