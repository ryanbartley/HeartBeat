//
//  Fish.cpp
//  SpringMeshGpu
//
//  Created by Ryan Bartley on 10/20/14.
//
//

#include "Fish.h"
#include "Common.h"

namespace heartbeat {
	
PondElementId Fish::TYPE = Hash::HASHER("Fish");
	
Fish::Fish()
{
	
}
	
FishRef Fish::create()
{
	return FishRef( new Fish );
}

void Fish::update()
{
	
}
	
void Fish::initialize( const ci::JsonTree &root )
{
	PondElement::initialize( root );
}

void Fish::registerTouch( EventDataRef touchEvent )
{
	
}
	
	
}