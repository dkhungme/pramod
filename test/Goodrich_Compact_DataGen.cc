#include <gflags/gflags.h>
#include <iostream>
#include <utils/Goodrich_Compact_DataGen.h>

using namespace std;

DEFINE_string(config_file,"config","Where to read the config");
int main(int argc, char **argv){
	 gflags::ParseCommandLineFlags(&argc, &argv, true);
	 sober::Goodrich_Compact_DataGen GC_gen;
	 GC_gen.Generate(0.3);
	 cout << "Complete generating data for compaction" << endl;
	 return 0;
}
