/*
 * WLog.h - very simple logging
 *
 * Extracted from legacy W library, modified & made standalone on 2016.4.23
 * by Ben Hallstein
 *
 */

#include "WLog.h"
#include <cstring>
#include <cstdlib>
#include <cstdio>

#if defined _WIN32 || _WIN64
#define WTARGET_WIN
#endif

FILE *_wlog = NULL;

struct _WLogInit {
	_WLogInit() {
		// Default log: /dev/null
		#if defined WTARGET_WIN
			WLog::setPath("\\Device\\Null");
		#else
			WLog::setPath("/dev/null");
		#endif
	}
};

_WLogInit *_wlogInit = new _WLogInit();

void WLog::setPath(const char *path) {
	fclose(_wlog);
	_wlog = NULL;
	
	_wlog = fopen(path, "w");
}

void WLog::write(const char *data) {
	fwrite(data, sizeof(char), strlen(data), _wlog);
}
