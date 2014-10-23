//
//  KioskManager.h
//  SvgTest
//
//  Created by Ryan Bartley on 10/19/14.
//
//

#pragma once

#include "Common.h"

namespace heartbeat {
	
class KioskManager {
public:
	
	~KioskManager();
	
	static KioskManagerRef create();
	
	void approachDelegate( EventDataRef approachEvent );
	void touchDelegate( EventDataRef touchEvent );
	
	void update();
	void render();
	
private:
	KioskManager();
	
	void initialize();
	
	std::array<InfoDisplayRef, 3>			mDisplays;
	std::array<std::shared_future<void>, 3> mLocks;
	HidCommManagerRef						mHidCommManager;
	
	friend class Engine;
};
	
}
