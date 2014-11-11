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
	void departDelegate( EventDataRef departEvent );
	void touchDelegate( EventDataRef touchEvent );
	
	void update();
	void render();
	
	void initialize();
	
	std::array<InfoDisplayRef, 3>& getInfoDisplays() { return mDisplays; }
	std::array<LilyPadRef, 3>& getLilyPads() { return mLilyPads; }
	
private:
	KioskManager();
	
	std::array<InfoDisplayRef, 3>	mDisplays;
	std::array<LilyPadRef, 3>		mLilyPads;
	HidCommManagerRef				mHidCommManager;
	
	friend class Engine;
};
	
}
