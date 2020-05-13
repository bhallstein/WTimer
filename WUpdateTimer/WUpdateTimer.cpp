/*
 * WUpdateTimer.cpp
 *
 * Simple cross-platform callback timer
 *
 * Extracted from legacy W library, modified & made standalone on 2016.4.23
 * by Ben Hallstein
 *
 */

#include "WUpdateTimer.h"
#include <cstdlib>

/* Target identification */

#ifdef __APPLE__
#include "TargetConditionals.h"
#if TARGET_OS_IPHONE
#define WTARGET_IOS
#elif TARGET_OS_MAC
#define WTARGET_MAC
#endif

#elif defined _WIN32 || _WIN64
#define WTARGET_WIN

#endif


WUpdateTimer::WUpdateTimer(vdfncb _fp) :
	objs(NULL),
	running(false),
	fp(_fp)
{
	createTimer();
}
WUpdateTimer::~WUpdateTimer()
{
	destroyTimer();
}



/***  UpdateTimer: Mac- & iOS-specific implementation  ***/


#if defined WTARGET_MAC || defined WTARGET_IOS
#pragma mark - Mac/iOS impl

#ifdef WTARGET_MAC
	#include "WUpdateTimer_OSX.h"
#else
	#include "WUpdateTimer_iOS.h"
#endif

struct WUpdateTimer::Objs {
	W_UpdateTimer *timer;
};

void WUpdateTimer::createTimer() {
	if (objs) {
		return;
	}
	objs = new Objs;
	objs->timer = [[W_UpdateTimer alloc] initWithCallback:fp];
}
void WUpdateTimer::destroyTimer() {
	if (objs) {
		delete objs;
		objs = NULL;
	}
}
void WUpdateTimer::start(float interval) { [objs->timer start:interval]; }
void WUpdateTimer::stop() { [objs->timer stop]; }



/***  UpdateTimer: Win-specific implementation  ***/

#elif defined WTARGET_WIN
#pragma mark - Windows impl

struct WUpdateTimer::Objs {
	UINT timer;
};

typedef void (*vdfncb)(void);
vdfncb _controllerUpdateFn = NULL;
void CALLBACK _mmtCallback(UINT timerid, UINT uMsg, DWORD userdata, DWORD d1, DWORD d2) {
	if (_controllerUpdateFn)
		(*_controllerUpdateFn)();
}
struct WUpdateTimer::Init {
	Init() {
		MMRESULT mmr = timeBeginPeriod(2);
		if (mmr != TIMERR_NOERROR)
			throw Exception("UpdateTimer: Error setting timer resolution");
	}
};
WUpdateTimer::Init *WUpdateTimer::init = new WUpdateTimer::Init();

void WUpdateTimer::createTimer() {
	if (objs) {
		W::log << "UpdateTimer: createTimer() called but 'objs' already exists";
		return;
	}
	objs = new Objs();
	_controllerUpdateFn = fp;
}
void WUpdateTimer::destroyTimer() {
	if (objs) {
		delete objs;
		objs = NULL;
	}
	_controllerUpdateFn = NULL;
}
void WUpdateTimer::start(float interval) {
	// Create mmt
	MMRESULT mmr = timeSetEvent(20, 40, &_mmtCallback, NULL, TIME_PERIODIC);
	if (mmr == NULL)
		throw Exception("UpdateTimer: Error starting multimedia timer");
	objs->timer = mmr;
}
void WUpdateTimer::stop() {
	timeKillEvent(objs->timer);
}

#endif
