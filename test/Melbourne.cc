
#include <gflags/gflags.h>
#include <glog/logging.h>
#include "encrypt/Encryptor.h"
#include "utils/GlobalParams.h"
#include <iostream>
#include <scrambler/Melbourne.h>
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

 	char T1[256];
 	sprintf(T1, "%s_T1", params->data_path().c_str());

 	char T2[256];
 	sprintf(T2, "%s_T2", params->data_path().c_str());

 	char O[256];
 	sprintf(O, "%s_output", params->data_path().c_str());

	sober::DataGen gen;
	gen.Generate();
	cout << "Data generated." << endl;
	sleep(2);

	double start = Now();


	Encryptor encryptor; 
	Melbourne Melbourne(&encryptor,ciphertext_size,params->record_size());
	
	Melbourne.Scramble(input_file,T1, T2, O, 3,4);
	
	cout << "Scramble in  (s)" << (Now()-start) <<endl;
	unlink(T1);
	unlink(T2);
	return 0;
}
