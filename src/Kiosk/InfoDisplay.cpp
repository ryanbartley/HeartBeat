//
//  InfoDisplay.cpp
//  ProjectionTestApp
//
//  Created by Ryan Bartley on 10/1/14.
//
//

#include "InfoDisplay.h"

#include "JsonManager.h"
#include "InteractionEvents.h"

#include "cinder/gl/Texture.h"
#include "cinder/Log.h"


using namespace ci;
using namespace ci::app;
using namespace std;

namespace heartbeat {
	
InfoDisplay::InfoDisplay( KioskId kioskId )
: mId( kioskId )
{
	
}
	
InfoDisplayRef InfoDisplay::create( heartbeat::KioskId kioskId )
{
	return InfoDisplayRef( new InfoDisplay( kioskId ) );
}

void InfoDisplay::update()
{
	 
}

void InfoDisplay::draw()
{
	
}
	
void InfoDisplay::registerTouch( EventDataRef eventData )
{
	static ci::mat4 inverse = ci::inverse( getModelMatrix() );
	
	auto event = std::dynamic_pointer_cast<TouchEvent>( eventData );
	if( ! event ) {
		CI_LOG_E("Couldn't cast touch event from " << eventData->getName() );
		return;
	}
	
	auto modelSpacePoint = inverse * vec4( event->getWorldCoordinate(), 0, 1 );
	auto twoDimPoint = vec2( modelSpacePoint.x, modelSpacePoint.y );
	if( mBoundingBox.contains( twoDimPoint ) ) {
		mDisplayState->touch( twoDimPoint  );
	}
	
}
	
void InfoDisplay::initiaize(const ci::JsonTree &root)
{
	try {
		Renderable::initialize( root["transformation"] );
		
		try {
			auto surfaceAttribs = root["surfaceAttribs"];
			
			auto dimensions = surfaceAttribs["dimensions"].getChildren();
			
			vec2 size;
			int i = 0;
			for( auto & dimension : dimensions ) {
				size[i++] = dimension.getValue<float>();
			}
			
		}
		catch ( const JsonTree::ExcChildNotFound &ex ) {
			CI_LOG_W("Couldn't find surface Attribs " << ex.what());
		}
		
	}
	catch ( const JsonTree::ExcChildNotFound &ex ) {
		CI_LOG_W(ex.what());
	}
}
	
}