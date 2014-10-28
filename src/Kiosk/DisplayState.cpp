//
//  DisplayState.cpp
//  SvgTest
//
//  Created by Ryan Bartley on 10/23/14.
//
//

#include "DisplayState.h"
#include "ButtonTypes.h"
#include "InfoDisplay.h"

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
	
void DisplayState::render()
{

}
	
}