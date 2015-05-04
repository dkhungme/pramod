#ifndef GOODRICH_COMPACT_DATAGEN_H_
#define GOODRICH_COMPACT_DATAGEN_H_

#include <string.h>
using std::string;

/** Generating random data for compacting
 * with to-be-delete item is a string of 100 zeros.
 *  Read settings from GlobalParameters
 *  Data is encrypted with GCM, output of the form
 *  	<IV><E(key)>
 *
 *  Encryption key is store at the key_path
 */

 namespace sober{

 	class Goodrich_Compact_DataGen{
 	public:
 		Goodrich_Compact_DataGen():file_(NULL){}
 		void gen_empty(byte *s);
 		void Generate(double ratio);

 	private:
 		FILE* file_;
 	};
 } 

#endif /* GOODRICH_COMPACT_DATAGEN_H_ */
