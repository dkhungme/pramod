/*
 * test_sort.cc
 *
 *  Created on: 3 Apr, 2015
 *      Author: dinhtta
 */


#include <iostream>
#include <algorithm>
#include <string.h>
#include <vector>
#include <time.h>
#include <crypto++/osrng.h>
using CryptoPP::AutoSeededRandomPool;

using namespace std;

/**
 * Test sorting of char **. Assume that each (char *) has the same size
 */
#define RECORD_SIZE 100
#define MAX 2000000

// strict less than comparison
bool comp(const char *x, const char *y){
	return strncmp(x,y,RECORD_SIZE) < 0;
}

double Now() {
        timespec tp;
        clock_gettime(CLOCK_MONOTONIC, &tp);
        return tp.tv_sec + 1e-9 * tp.tv_nsec;
}

typedef char cipher_t[16];

bool gen_and_sort() {

	vector<const char*> vec;
	cipher_t *input = (cipher_t*)malloc(MAX*sizeof(cipher_t));
	AutoSeededRandomPool rng;
	for (int i = 0; i < MAX; i++)
		rng.GenerateBlock((byte*) input[i], RECORD_SIZE);

	double start = Now();
	vec.assign(input, input + MAX);
	cout << "convert time = " << (Now() - start) << endl;
	start = Now();
	sort(vec.begin(), vec.end(), comp);
	cout << "sorting time = " << (Now() - start) << endl;


	for (int i = 0; i < MAX - 1; i++)
		if (strncmp(vec[i], vec[i + 1], RECORD_SIZE) > 0){
			free(input);
			return false;
	}
	free(input);
	return true;
}

int main(int argc, char **argv){
	bool ret = gen_and_sort();
	cout << "Sort result = " << ret << endl;
	return 0;
}
