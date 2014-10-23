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
: mParent( SvgManager::get()->getGroup( name ) ), mName(name)
{
}

ButtonRef Button::create( const std::string &name )
{
	return ButtonRef( new Button( name ) );
}


	
}