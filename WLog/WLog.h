/*
 * WLog.h - very simple logging
 *
 * Extracted from legacy W library, modified & made standalone on 2016.4.23
 * by Ben Hallstein
 *
 */

#ifndef __WLog
#define __WLog

struct WLog {
	static void setPath(const char *);
	static void write(const char *);
};


#endif
