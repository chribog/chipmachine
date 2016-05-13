#ifndef SPMediaKeyTap_h
#define SPMediaKeyTap_h

#include <Cocoa/Cocoa.h>
#import <IOKit/hidsystem/ev_keymap.h>
#import <Carbon/Carbon.h>

#ifdef __cplusplus
#include "../ChipMachineDelegate.h"
#endif
// http://overooped.com/post/2593597587/mediakeys

#define SPSystemDefinedEventMediaKeys 8

@interface SPMediaKeyTap : NSObject {
	EventHandlerRef _app_switching_ref;
	EventHandlerRef _app_terminating_ref;
	CFMachPortRef _eventPort;
	CFRunLoopSourceRef _eventPortSource;
	CFRunLoopRef _tapThreadRL;
	BOOL _shouldInterceptMediaKeyEvents;

	chipmachine::ChipMachineDelegate* _delegate;
	// The app that is frontmost in this list owns media keys
	NSMutableArray *_mediaKeyAppList;
}
+ (NSArray*)defaultMediaKeyUserBundleIdentifiers;

-(id)initWithDelegate:(void*)delegate;

+(BOOL)usesGlobalMediaKeyTap;
-(void)startWatchingMediaKeys;
-(void)stopWatchingMediaKeys;
-(void)handleAndReleaseMediaKeyEvent:(NSEvent *)event;
@end

@interface NSObject (SPMediaKeyTapDelegate)
-(void)mediaKeyTap:(SPMediaKeyTap*)keyTap receivedMediaKeyEvent:(NSEvent*)event;
@end

#ifdef __cplusplus
extern "C" {
#endif

extern NSString *kMediaKeyUsingBundleIdentifiersDefaultsKey;
extern NSString *kIgnoreMediaKeysDefaultsKey;

#ifdef __cplusplus
}
#endif
#endif