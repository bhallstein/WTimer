/*
 * WUpdateTimer_iOS.h
 *
 * Simple cross-platform callback timer
 *
 * Extracted from legacy W library, modified & made standalone on 2016.4.23
 * by Ben Hallstein
 *
 */

#import <Foundation/Foundation.h>

typedef void (*vdfncb)(void);

@interface W_UpdateTimer : NSObject
-(id)initWithCallback:(vdfncb)fp;
-(void)start:(float)interval;
-(void)stop;
-(void)callback:(id)sender;
@end
