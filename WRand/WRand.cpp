/*
 * WRand.cpp
 *
 * - Simple Mersenne Twister pseudo-random number generator
 * - Auto-initializes
 * - Based on mtrand
 *
 * Extracted from legacy W library, modified & made standalone on 2016.4.23
 * by Ben Hallstein
 *
 */

#include "WRand.h"

// MTRand, modified for simple WRand wrapper:
//  - made single-file
//  - 'twiddle' function now a #define
//  - removed unneeded float subclasses
//  - removed some boilerplate not reqd here
// 2016.4.23

// mtrand.h
// C++ include file for MT19937, with initialization improved 2002/1/26.
// Coded by Takuji Nishimura and Makoto Matsumoto.
// Ported to C++ by Jasper Bedaux 2003/1/1 (see http://www.bedaux.net/mtrand/).
// The generators returning floating point numbers are based on
// a version by Isaku Wada, 2002/01/09
//
// Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// 3. The names of its contributors may not be used to endorse or promote
//    products derived from this software without specific prior written
//    permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Any feedback is very welcome.
// http://www.math.keio.ac.jp/matumoto/emt.html
// email: matumoto@math.keio.ac.jp
//
// Feedback about the C++ port should be sent to Jasper Bedaux,
// see http://www.bedaux.net/mtrand/ for e-mail address and info.

#define MT_TWIDDLE(u, v) \
  ((((u & 0x80000000UL) | (v & 0x7FFFFFFFUL)) >> 1) \
  ^ ((v & 1UL) ? 0x9908B0DFUL : 0x0UL))

class MTRand_int32 {
public:
	// constructor with 32 bit int as seed
	MTRand_int32(unsigned long s) { seed(s); init = true; }
	// constructor with array of size 32 bit ints as seed
	MTRand_int32(const unsigned long* array, int size) { seed(array, size); init = true; }
	
	// seed with 32 bit integer
	void seed(unsigned long s) {
		state[0] = s & 0xFFFFFFFFUL; // for > 32 bit machines
		for (int i = 1; i < n; ++i) {
			state[i] = 1812433253UL * (state[i - 1] ^ (state[i - 1] >> 30)) + i;
			// see Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier
			// in the previous versions, MSBs of the seed affect only MSBs of the array state
			// 2002/01/09 modified by Makoto Matsumoto
			state[i] &= 0xFFFFFFFFUL; // for > 32 bit machines
		}
		p = n; // force gen_state() to be called for next random number
	}
	
	// seed with array
	void seed(const unsigned long *array, int size) {
		seed(19650218UL);
		int i = 1, j = 0;
		for (int k = ((n > size) ? n : size); k; --k) {
			state[i] = (state[i] ^ ((state[i - 1] ^ (state[i - 1] >> 30)) * 1664525UL))
			+ array[j] + j; // non linear
			state[i] &= 0xFFFFFFFFUL; // for > 32 bit machines
			++j; j %= size;
			if ((++i) == n) { state[0] = state[n - 1]; i = 1; }
		}
		for (int k = n - 1; k; --k) {
			state[i] = (state[i] ^ ((state[i - 1] ^ (state[i - 1] >> 30)) * 1566083941UL)) - i;
			state[i] &= 0xFFFFFFFFUL; // for > 32 bit machines
			if ((++i) == n) { state[0] = state[n - 1]; i = 1; }
		}
		state[0] = 0x80000000UL; // MSB is 1; assuring non-zero initial array
		p = n; // force gen_state() to be called for next random number
	}
	
	unsigned long operator()() { return rand_int32(); }
	
private:
	// generate 32 bit random integer
	unsigned long rand_int32(){
		// generate 32 bit random int
		if (p == n) gen_state(); // new state vector needed
		// gen_state() is split off to be non-inline, because it is only called once
		// in every 624 calls and otherwise irand() would become too big to get inlined
		unsigned long x = state[p++];
		x ^= (x >> 11);
		x ^= (x << 7) & 0x9D2C5680UL;
		x ^= (x << 15) & 0xEFC60000UL;
		return x ^ (x >> 18);
	}
	static const int n = 624, m = 397; // compile time constants
	static unsigned long state[n]; // state vector array
	static int p;     // position in state array
	static bool init; // true if init function is called
	// private functions used to generate the pseudo random numbers
	// generate new state
	void gen_state() {
		for (int i = 0; i < (n - m); ++i)
			state[i] = state[i + m] ^ MT_TWIDDLE(state[i], state[i + 1]);
		for (int i = n - m; i < (n - 1); ++i)
			state[i] = state[i + m - n] ^ MT_TWIDDLE(state[i], state[i + 1]);
		state[n - 1] = state[m - 1] ^ MT_TWIDDLE(state[n - 1], state[0]);
		p = 0; // reset position
	};
};

// init statics
unsigned long MTRand_int32::state[n] = {0x0UL};
int MTRand_int32::p = 0;
bool MTRand_int32::init = false;

/**  end mtrand  **/


// Our WRand wrapper class
#include <ctime>

MTRand_int32 *_mt_twister;

struct _WRand_Init {
	_WRand_Init() {
		time_t thetime;
		time(&thetime);
		_mt_twister = new MTRand_int32(thetime);
	}
};
_WRand_Init _w_init;

int WRand::intUpTo(int x) {
	if (x <= 0) return 0;
	return (*_mt_twister)()%x;
}
float WRand::floatUpTo(float x) {
	return (*_mt_twister)() * x / 4294967295.;
}

