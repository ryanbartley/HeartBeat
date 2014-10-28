//
//  DisplayState.h
//  SvgTest
//
//  Created by Ryan Bartley on 10/23/14.
//
//

#pragma once

#include "Node.h"
#include "PageTypes.h"

namespace heartbeat {
	
class DisplayState {
public:
	
	void touch( const ci::vec2 &point );
	
	void update();
	
	void render();
	
private:
	DisplayState();
	
	InfoDisplayRef			mParent;
	ButtonRef				mCurrentActiveButton;
	std::vector<ButtonRef>	mButtons;
	DataPageRef				mCurrentPage, mNextPage;
	OverlayPageRef			mCurrentOverlay;
};
	
}
