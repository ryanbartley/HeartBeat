//
//  InfoDisplay.cpp
//  ProjectionTestApp
//
//  Created by Ryan Bartley on 10/1/14.
//
//

#include "InfoDisplay.h"

#include "JsonManager.h"

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
			
			mSurface = cairo::SurfaceImage( size.x, size.y, true );
			mContext = cairo::Context( mSurface );
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