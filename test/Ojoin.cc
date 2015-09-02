
#include <gflags/gflags.h>
#include <glog/logging.h>
#include "encrypt/Encryptor.h"
#include "utils/GlobalParams.h"
#include <iostream>
#include <join/OJoin.h>
#include <utils/DataGen.h>
#include <unistd.h>
DEFINE_string(config_file,"config","Where to read the config");

using namespace std;
using namespace sober; 
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
	OJoin OJoin(&encryptor,ciphertext_size,params->record_size());

	FILE *input, *output;
	input = fopen(input_file, "r+");
	output = fopen(O, "w+");

	vector <int> weights;

	int num_r = params->num_records();
	// double num_r = atof(num_records);
	for (int i = num_r; i>0; i--){
		if (i>(2*num_r/3))
			weights.push_back(4);
		else if (i>(num_r/3))
			weights.push_back(3);
		else 
			weights.push_back(2);

	}

	OJoin.ob_Expand(input, output, weights);

	fclose(input);
	fclose(output);
	cout << "finished in " << Now() - start << "\n";

	
}