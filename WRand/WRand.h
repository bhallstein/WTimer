/*
 * WRand.h
 *
 * - Simple Mersenne Twister pseudo-random number generator
 * - Auto-initializes
 * - Based on mtrand
 *
 * Extracted from legacy W library, modified & made standalone on 2016.4.23
 * by Ben Hallstein
 *
 */

#ifndef __WRand
#define __WRand

namespace WRand {
	int intUpTo(int x);        // -> int in the interval [0...x-1]
	float floatUpTo(float x);  // -> float in the interval [0..x]
};

#endif
