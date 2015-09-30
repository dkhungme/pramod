
#include <iostream>
#include <gflags/gflags.h>
#include <glog/logging.h>
#include <sort/Sorter.h>
#include <mixer/Mixnet.h>
#include <utils/DataGen_Compact.h>
#include <utils/GlobalParams.h>
#include <encrypt/Encryptor.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/resource.h>
 DEFINE_string(config_file,"config","Where to read the config");

 using namespace std;
 using namespace sober;
 



 void gen_random(char *s, const int len) {
 	static const char alphanum[] =
 	"0123456789"
 	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
 	"abcdefghijklmnopqrstuvwxyz";

 	for (int i = 0; i < len; ++i) {
 		s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
 	}

 	s[len] = 0;
 }

 int main(int argc, char **argv) {


 	
 	gflags::ParseCommandLineFlags(&argc, &argv, true);
 	google::InitGoogleLogging(argv[0]);
 	FLAGS_logtostderr = 1;

	//work out the prefix to block that have been mixed.
 	GlobalParams *params = GlobalParams::Get();
 	DataGen_Compact gen;
 	//gen.Generate();
 	//LOG(INFO) << "Data generated for testing encryptor performance." << endl;
 	sleep(2);


 	FILE  *finput;
 	char input[256];
 	sprintf(input, "%s/data_0", params->tmp_data_path().c_str());
 	finput = fopen(input, "r+");

 	byte *cipher;
	cipher = (byte *) malloc(132);
 	fread(cipher, 1, 132,finput);

 	Encryptor encryptor; 
 	int i;
 	double start = Now();
 	for (i=0; i<10000000; i++){
 		encryptor.ReEncrypt((byte*)cipher, 132);
 	}
 	LOG(INFO)  << "reencrypt 10^7 items in .. "<< (Now()-start) << endl;

 	
 	//string pt = encryptor.Decrypt((byte*)cipher, 132);

 	//start = Now();
 	//LOG(INFO)  << "reset NOW in .. "<< (Now()-start) << endl;

 	//for (i=0; i<1000000; i++){
 	//	encryptor.Hash((byte*)pt.c_str(), 132);
 	//}
 	//LOG(INFO)  << "hash 10^6 items in .. "<< (Now()-start) << endl;


 	return 0;
 }

