/*
 * WUpdateTimer_iOS.m
 *
 * Simple cross-platform callback timer
 *
 * Extracted from legacy W library, modified & made standalone on 2016.4.23
 * by Ben Hallstein
 *
 */

#import "WUpdateTimer_iOS.h"


@interface W_UpdateTimer () {
	CADisplayLink *displayLink;	  // NB: iOS 3.1 or higher req'd to use displaylink
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
	displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(callback:)];
	displayLink.frameInterval = 1;
	[displayLink addToRunLoop:[NSRunLoop mainRunLoop] forMode:NSDefaultRunLoopMode];
}
-(void)stop {
	[displayLink invalidate];
}
-(void)callback:(id)sender {
	(*fp)();
}

@end
