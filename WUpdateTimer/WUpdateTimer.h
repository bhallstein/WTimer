/*
 * WUpdateTimer.h
 *
 * Simple cross-platform callback timer
 *
 * Extracted from legacy W library, modified & made standalone on 2016.4.23
 * by Ben Hallstein
 *
 */


#ifndef __W__UpdateTimer
#define __W__UpdateTimer

class WUpdateTimer {
	typedef void (*vdfncb)(void);
public:
	WUpdateTimer(vdfncb _fp);
	~WUpdateTimer();
	
	void start(float interval_s);
	void stop();
		// NB: interval is only honoured on OS X
	
private:
	void createTimer();
	void destroyTimer();
	
	bool running;
	
	struct Objs;
	Objs *objs;
	
	struct Init;
	static Init *init;
	
	void (*fp)(void);
};

#endif
