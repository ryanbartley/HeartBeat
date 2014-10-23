//
//  DisplayState.cpp
//  SvgTest
//
//  Created by Ryan Bartley on 10/23/14.
//
//

#include "DisplayState.h"
#include "ActivatableButton.h"

namespace heartbeat {
	
DisplayState::DisplayState()
{
	
}
	
void DisplayState::update()
{
	if( mNextPage ) {
		mCurrentPage = mNextPage;
		mNextPage = nullptr;
	}
	
}
	
void DisplayState::renderToSurface()
{
	auto ctx = mParent->getContext();
	ctx.flush();
	
//	ctx.render( mCurrentPage-> )
	
	
	for( auto & button : mButtons ) {
		if( button->getType() == ActivatableButton::TYPE ) {
			
		}
	}
}
	
}