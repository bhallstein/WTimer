#ifndef __W__UpdateTimer
#define __W__UpdateTimer

#include "WCallback.h"

struct WUpdateTimer {
	typedef void (*vdfncb)(void);

	WUpdateTimer(vdfncb _fp);
	~WUpdateTimer();

	void start(float interval_s);  // NB: interval is only honoured on OS X
	void stop();

	struct Objs;
	struct Init;

private:
	void createTimer();
	void destroyTimer();

	bool running;

	Objs *objs;
	static Init *init;

	void (*fp)(void);
};

struct WUpdateTimer_Callback {
	typedef WCallback<void, int> callback;

	WUpdateTimer_Callback(callback *_cb);
	~WUpdateTimer_Callback();

	void start(float interval_s);  // NB: interval is only honoured on OS X
	void stop();

private:
	void createTimer();
	void destroyTimer();

	callback *cb;
	bool running;
	WUpdateTimer::Objs *objs;
};

#endif

