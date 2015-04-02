/*
 * Mixnet.cc
 *
 *  Created on: 31 Mar, 2015
 *      Author: dinhtta
 */


#include <gflags/gflags.h>
#include <iostream>
#include <glog/logging.h>
#include <mixer/Mixnet.h>
#include <utils/DataGen.h>
#include <unistd.h>
DEFINE_string(config_file,"config","Where to read the config");

using namespace std;
/**
 * Generate data, sleep 1 sec, then Start mixing
 */
int main(int argc, char **argv){
	gflags::ParseCommandLineFlags(&argc, &argv, true);
    google::InitGoogleLogging(argv[0]);
    FLAGS_logtostderr = 1;
	sober::DataGen gen;
	gen.Generate();
	cout << "Data generated." << endl;
	sleep(2);

	sober::Mixnet mixnet;
	mixnet.StartMixing();
	cout << "Mixing successful. "<<endl;
	return 0;
}

