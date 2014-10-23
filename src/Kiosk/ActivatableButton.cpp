//
//  ActivatableButton.cpp
//  SvgTest
//
//  Created by Ryan Bartley on 10/23/14.
//
//

#include "ActivatableButton.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace heartbeat {
	
ActivatableButton::ActivatableButton( const std::string &name )
: Button( name ), mActive( static_cast<const svg::Group*>( &mParent->getChild( "active" ) ) ),
	mNonActive( static_cast<const svg::Group*>( &mParent->getChild( "default" ) ) )
{
}
	
ActivatableButtonRef ActivatableButton::create( const std::string &name )
{
	return ActivatableButtonRef( new ActivatableButton( name ) );
}
	
}