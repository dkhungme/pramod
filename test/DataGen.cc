/*
 * DataGen.cc
 *
 *  Created on: 31 Mar, 2015
 *      Author: dinhtta
 */


#include <gflags/gflags.h>
#include <iostream>
#include <utils/DataGen.h>

using namespace std;

DEFINE_string(config_file,"config","Where to read the config");
int main(int argc, char **argv){
	 gflags::ParseCommandLineFlags(&argc, &argv, true);
	 sober::DataGen gen;
	 gen.Generate();
	 cout << "DataGen test successful" << endl;
	 return 0;
}
