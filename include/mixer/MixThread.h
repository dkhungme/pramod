/*
 * MixThread.h
 *
 *  Created on: Apr 1, 2015
 *      Author: dinhtta
 */

#ifndef MIXTHREAD_H_
#define MIXTHREAD_H_

#include <vector>
#include "mixer/Mixer.h"
#include "utils/GlobalParams.h"
#include "mixer/Mixnet.h"

using std::vector;

/**
 * A thread executing multiple mixer nodes in sequential order.
 */

namespace sober{

class MixThread{
public:
	MixThread(int start_idx, int end_idx);

	/**
	 * Thread entry. At the given round, each mixer pulls data, mixes, re-encrypts and
	 * outputs. The main thread is in charge of advancing the round.
	 */
	void StartMixing(Mixnet *mixnet, int round);

private:
	vector<Mixer*> mixers_;
	GlobalParams *params_;
	int ciphertext_block_size_;
	int start_idx_, end_idx_;
};

} // namespace sober

#endif /* MIXTHREAD_H_ */
