/*
 * WUpdateTimer_OSX.h
 *
 * Simple cross-platform callback timer
 *
 * Extracted from legacy W library, modified & made standalone on 2016.4.23
 * by Ben Hallstein
 *
 */

#import <Foundation/Foundation.h>
#import "WCallback.h"

typedef void (*vdfncb)(void);

@interface W_UpdateTimer : NSObject
-(id)initWithFunc:(vdfncb)fp;
-(id)initWithCallback:(WCallback<void, int>*)cb;
-(void)start:(float)interval;
-(void)stop;
-(void)callback:(NSTimer*)_t;
@end

