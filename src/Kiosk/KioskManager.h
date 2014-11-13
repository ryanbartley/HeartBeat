//
//  KioskManager.h
//  SvgTest
//
//  Created by Ryan Bartley on 10/19/14.
//
//

#pragma once

#include "Common.h"
#include "Cairo.h"

namespace heartbeat {
	
class KioskManager {
public:
	
	~KioskManager();
	
	static KioskManagerRef create();
	
	void approachDelegate( EventDataRef approachEvent );
	void departDelegate( EventDataRef departEvent );
	void touchBeganDelegate( EventDataRef touchEvent );
	void touchMovedDelegate( EventDataRef touchEvent );
	void touchEndedDelegate( EventDataRef touchEvent );
#if defined( DEBUG )
	void toggleDebugRenderInfoDisplay();
	void toggleDebugBoundingBoxes();
#endif
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
	
#if defined( DEBUG )
	bool							mDebugRenderInfoDisplay;
	
#endif
	
	friend class Engine;
};
	
}
