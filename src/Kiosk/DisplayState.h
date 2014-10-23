//
//  DisplayState.h
//  SvgTest
//
//  Created by Ryan Bartley on 10/23/14.
//
//

#pragma once

#include "Button.h"
#include "Data.h"
#include "Overlay.h"
#include "InfoDisplay.h"

namespace heartbeat {
	
class DisplayState {
public:
	
	void touch( const ci::vec2 &point );
	
	void update();
	
	void renderToSurface();
	
private:
	DisplayState();
	
	InfoDisplayRef			mParent;
	ButtonRef				mCurrentActiveButton;
	std::vector<ButtonRef>	mButtons;
	DataRef					mCurrentPage, mNextPage;
	OverlayRef				mCurrentOverlay;
};
	
}
