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
: mGroup( SvgManager::get()->getGroup( name ) ), mName(name), mStatus(ButtonStatus::INVISIBLE)
{

}

	
}