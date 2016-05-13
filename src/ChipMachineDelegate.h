#ifndef ChipMachineDelegate_h
#define ChipMachineDelegate_h

#include <Cocoa/Cocoa.h>

@class SPMediaKeyTap;

namespace chipmachine {
class ChipMachine;
	
class ChipMachineDelegate {
public:
	void mediaKeyTapReceivedMediaKeyEvent(SPMediaKeyTap* keyTap, NSEvent* event);
	
	ChipMachine* mChipMachine;
	
};
};
#endif