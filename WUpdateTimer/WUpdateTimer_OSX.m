/*
 * WUpdateTimer_OSX.m
 *
 * Simple cross-platform callback timer
 *
 * Extracted from legacy W library, modified & made standalone on 2016.4.23
 * by Ben Hallstein
 *
 */

#import "WUpdateTimer_OSX.h"

@interface W_UpdateTimer () {
	NSTimer *t;
	vdfncb fp;
}
@end

@implementation W_UpdateTimer

-(id)initWithCallback:(vdfncb)_fp {
	if (self = [super init]) {
		fp = _fp;
	}
	return self;
}

-(void)start:(float)interval {
	t = [NSTimer scheduledTimerWithTimeInterval:interval
										 target:self
									   selector:@selector(callback:)
									   userInfo:nil
										repeats:YES];
}
-(void)stop {
	[t invalidate];
	t = nil;
}
-(void)callback:(NSTimer*)_t {
	(*fp)();
}

@end
