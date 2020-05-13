/*
 * WTimer.h
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

#ifndef Timer_H
#define Timer_H

class WTimer {
public:
	WTimer();
	~WTimer();
	void reset();
	unsigned long getMilliseconds();	// ms since init/reset
	unsigned long getMicroseconds();	// µs since init/reset
	unsigned long getMillisecondsCPU();	// "only cpu time measured"
	unsigned long getMicrosecondsCPU();	// ~
private:
	struct TimerInt;
	TimerInt *timer;
};

#endif
