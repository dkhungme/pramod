/*
 * Goodrich.h
 *
 *  Created on: Apr 6, 2015
 *      Author: dinhtta
 */

#ifndef GOODRICH_H_
#define GOODRICH_H_

/**
 * Header for the Goodrich's oblivious sort algorithm.
 *
 * Hung: implement this.
 */

namespace sober{
class Goodrich{
public:

	/**
	 * Use the Encryptor object to decrypt the file.
	 * See encrypt/Encryptor.h for details.
	 *
	 * encryptor.Decrypt(byte *input, int size);
	 */
	void Sort(string input, string output, int num_records, int record_size, Encryptor encryptor);
};
}

#endif /* GOODRICH_H_ */
