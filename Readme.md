WTimer
======

WTimer is a small, standlone highish-resolution timer class, useful for quick ’n dirty profiling, getting inter-frame update times, etc.

    #include "WTimer.h"
	
	WTimer t;
	// do something
	unsigned long task1_duration = t.getMicroseconds();

	t.reset();
	// do something else
	unsigned long task2_duration = t.getMicroseconds();
	
(Repo-ising because I find I want this type of thing all the time, but always forget exactly how `gettimeofday` works.)

WTimer was modified from the Ogre source code. It was once part of my erstwhile [W](http://ben.am/w) library.

– Ben Hallstein, Oct 2014
