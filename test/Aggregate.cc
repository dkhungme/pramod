#include <gflags/gflags.h>
#include <glog/logging.h>
#include "encrypt/Encryptor.h"
#include "utils/GlobalParams.h"
#include <iostream>
#include <aggregate/Aggregate.h>
#include <utils/DataGen.h>
#include <unistd.h>
DEFINE_string(config_file,"config","Where to read the config");

using namespace std;
using namespace sober; 
/**
 * Generate data, sleep 1 sec, then Start mixing
 */
int main(int argc, char **argv){
	gflags::ParseCommandLineFlags(&argc, &argv, true);
    google::InitGoogleLogging(argv[0]);
    FLAGS_logtostderr = 1;
    GlobalParams * params = GlobalParams::Get();
	int ciphertext_size = params->record_size()+GCM_TAG_SIZE+IV_SIZE;

	char input_file[256];
 	sprintf(input_file, "%s_0", params->data_path().c_str());

 	

 	char O[256];
 	sprintf(O, "%s_output", params->data_path().c_str());

	sober::DataGen gen;
	gen.Generate();
	cout << "Data generated." << endl;
	sleep(2);

	double start = Now();


	Encryptor encryptor; 
	Aggregate Aggregate(&encryptor,ciphertext_size,params->record_size());
	
	Aggregate.Scan(input_file,O);
	
	cout << "Aggregate in  (s)" << (Now()-start) <<endl;
	
	return 0;
}
