//
//  Button.cpp
//  SvgTest
//
//  Created by Ryan Bartley on 10/22/14.
//
//

#include "Button.h"

namespace heartbeat {
	
Button::Button( const std::string &name )
: Node( name ), mState( State::NONACTIVE ), mIsTouched( false )
{
}
	

	
}