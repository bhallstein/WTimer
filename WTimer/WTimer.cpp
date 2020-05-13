/*
 * WTimer.cpp
 *
 * Simple timer implementation from the OGRE source code, combined into standalone
 * windows/mac-compatible file 14.8.12 - Ben Hallstein
 *
 * Extracted from legacy W library, modified & made standalone on 2.10.14
 *
 */

/*
 * -----------------------------------------------------------------------------
 * This source file is part of OGRE
 *     (Object-oriented Graphics Rendering Engine)
 * For the latest info, see http://www.ogre3d.org/
 * 
 * Copyright (c) 2000-2011 Torus Knot Software Ltd
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 * -----------------------------------------------------------------------------
 */

#ifdef __APPLE__

#include "WTimer.h"
#include <sys/time.h>

struct WTimer::TimerInt {
	struct timeval start;
	clock_t zeroClock;
};


WTimer::WTimer()
{
	timer = new TimerInt;
	reset();
}

WTimer::~WTimer()
{
	delete timer;
}

void WTimer::reset() {
	timer->zeroClock = clock();
	gettimeofday(&timer->start, NULL);
}

unsigned long WTimer::getMilliseconds() {
	struct timeval now;
	gettimeofday(&now, NULL);
	return (now.tv_sec - timer->start.tv_sec) * 1000 + (now.tv_usec - timer->start.tv_usec)/1000;
}

unsigned long WTimer::getMicroseconds() {
	struct timeval now;
	gettimeofday(&now, NULL);
	return (now.tv_sec - timer->start.tv_sec) * 1000000 + (now.tv_usec - timer->start.tv_usec);
}

unsigned long WTimer::getMillisecondsCPU() {
	clock_t newClock = clock();
	return (unsigned long)((float)(newClock-timer->zeroClock) / ((float)CLOCKS_PER_SEC/1000.0)) ;
}

unsigned long WTimer::getMicrosecondsCPU() {
	clock_t newClock = clock();
	return (unsigned long)((float)(newClock-timer->zeroClock) / ((float)CLOCKS_PER_SEC/1000000.0)) ;
}


#elif defined _WIN32 || _WIN64

#include <time.h>
#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif
#if !defined(NOMINMAX) && defined(_MSC_VER)
	#define NOMINMAX // required to stop windows.h messing up std::min
#endif
#include "windows.h"

#include "WTimer.h"
#include <algorithm>

struct WTimer::TimerInt {
	clock_t mZeroClock;
	DWORD mStartTick;
	LONGLONG mLastTime;
	LARGE_INTEGER mStartTime;
	LARGE_INTEGER mFrequency;
	DWORD_PTR mTimerMask;
};

WTimer::WTimer()
{
	timer = new TimerInt;
	timer->mTimerMask = 0;
	reset();
}

WTimer::~WTimer()
{
	delete timer;
}

void WTimer::reset() {
	// Get the current process core mask
	DWORD_PTR procMask;
	DWORD_PTR sysMask;
	GetProcessAffinityMask(GetCurrentProcess(), &procMask, &sysMask);

	// If procMask is 0, consider there is only one core available
	// (using 0 as procMask will cause an infinite loop below)
	if (procMask == 0) procMask = 1;

	// Find the lowest core that this process uses
	if (timer->mTimerMask == 0) {
		timer->mTimerMask = 1;
		while((timer->mTimerMask & procMask) == 0)
			timer->mTimerMask <<= 1;
	}
	
	HANDLE thread = GetCurrentThread();
	
	// Set affinity to the first core
	DWORD_PTR oldMask = SetThreadAffinityMask(thread, timer->mTimerMask);

	// Get the constant frequency
	QueryPerformanceFrequency(&timer->mFrequency);

	// Query the timer
	QueryPerformanceCounter(&timer->mStartTime);
	timer->mStartTick = GetTickCount();

	// Reset affinity
	SetThreadAffinityMask(thread, oldMask);

	timer->mLastTime = 0;
	timer->mZeroClock = clock();
}

unsigned long WTimer::getMilliseconds() {
	LARGE_INTEGER curTime;

	HANDLE thread = GetCurrentThread();

	// Set affinity to the first core
	DWORD_PTR oldMask = SetThreadAffinityMask(thread, timer->mTimerMask);

	// Query the timer
	QueryPerformanceCounter(&curTime);

	// Reset affinity
	SetThreadAffinityMask(thread, oldMask);
	
	LONGLONG newTime = curTime.QuadPart - timer->mStartTime.QuadPart;
    
	// scale by 1000 for milliseconds
	unsigned long newTicks = (unsigned long) (1000 * newTime / timer->mFrequency.QuadPart);

	// detect and compensate for performance counter leaps
	// (surprisingly common, see Microsoft KB: Q274323)
	unsigned long check = GetTickCount() - timer->mStartTick;
	signed long msecOff = (signed long)(newTicks - check);
	if (msecOff < -100 || msecOff > 100) {
		// We must keep the timer running forward :)
		LONGLONG adjust = (std::min)(msecOff * timer->mFrequency.QuadPart / 1000, newTime - timer->mLastTime);
		timer->mStartTime.QuadPart += adjust;
		newTime -= adjust;

		// Re-calculate milliseconds
		newTicks = (unsigned long) (1000 * newTime / timer->mFrequency.QuadPart);
	}

	// Record last time for adjust
	timer->mLastTime = newTime;
	return newTicks;
}

unsigned long WTimer::getMicroseconds() {
	LARGE_INTEGER curTime;

	HANDLE thread = GetCurrentThread();

	// Set affinity to the first core
	DWORD_PTR oldMask = SetThreadAffinityMask(thread, timer->mTimerMask);

	// Query the timer
	QueryPerformanceCounter(&curTime);

	// Reset affinity
	SetThreadAffinityMask(thread, oldMask);

	LONGLONG newTime = curTime.QuadPart - timer->mStartTime.QuadPart;

	// get milliseconds to check against GetTickCount
	unsigned long newTicks = (unsigned long) (1000 * newTime / timer->mFrequency.QuadPart);

	// detect and compensate for performance counter leaps
	// (surprisingly common, see Microsoft KB: Q274323)
	unsigned long check = GetTickCount() - timer->mStartTick;
	signed long msecOff = (signed long)(newTicks - check);
	if (msecOff < -100 || msecOff > 100) {
		// We must keep the timer running forward :
		LONGLONG adjust = (std::min)(msecOff * timer->mFrequency.QuadPart / 1000, newTime - timer->mLastTime);
		timer->mStartTime.QuadPart += adjust;
		newTime -= adjust;
	}

	// Record last time for adjust
	timer->mLastTime = newTime;

	// scale by 1000000 for microseconds
	unsigned long newMicro = (unsigned long) (1000000 * newTime / timer->mFrequency.QuadPart);

	return newMicro;
}

unsigned long WTimer::getMillisecondsCPU() {
	clock_t newClock = clock();
	return (unsigned long)( (float)( newClock - timer->mZeroClock ) / ( (float)CLOCKS_PER_SEC / 1000.0 ) ) ;
}

unsigned long WTimer::getMicrosecondsCPU() {
	clock_t newClock = clock();
	return (unsigned long)( (float)( newClock - timer->mZeroClock ) / ( (float)CLOCKS_PER_SEC / 1000000.0 ) ) ;
}

#endif
