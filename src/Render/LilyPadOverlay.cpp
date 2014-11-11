//
//  LilyPadOverlay.cpp
//  FullProject
//
//  Created by Ryan Bartley on 11/10/14.
//
//

#include "LilyPadOverlay.h"
#include "JsonManager.h"
#include "cinder/Log.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace heartbeat {
	
LilyPad::LilyPad( KioskId kioskId, const ci::gl::Texture2dRef &texture )
: mKiosk( kioskId ), mLilyPad( texture ), mVisibility( 0.0f )
{
}
	
void LilyPad::initialize( const ci::JsonTree &root )
{
	try {
		Renderable::initialize( root["transformation"] );
	}
	catch ( const JsonTree::ExcChildNotFound &ex ) {
		CI_LOG_E(ex.what());
	}
}
	
void LilyPad::update()
{
	
}
	
void LilyPad::draw()
{
	gl::ScopedModelMatrix scopeModel;
	gl::setModelMatrix( getModelMatrix() );
	{
		gl::ScopedColor scopeColor( ColorA( 1, 1, 1, mVisibility ) );
		gl::draw( mLilyPad );
	}
	{
		gl::ScopedColor scopeColor( ColorA( 0, 0, 0, 1.0 - mVisibility ) );
		gl::draw( mLilyPad );
	}
}
	
void LilyPad::activate( bool activate )
{
	
}
	
}