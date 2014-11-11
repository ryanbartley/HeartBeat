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
#include "cinder/Timeline.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace heartbeat {
	
LilyPad::LilyPad( KioskId kioskId, const ci::gl::Texture2dRef &lightTexture, const ci::gl::Texture2dRef &darkTex )
: mKiosk( kioskId ), mLightTex( lightTexture ), mDarkTex( darkTex ), mVisibility( 0.0f )
{
}
	
LilyPadRef LilyPad::create( heartbeat::KioskId kioskId, const ci::gl::Texture2dRef &lightTexture, const ci::gl::Texture2dRef &darkTexture )
{
	return LilyPadRef( new LilyPad( kioskId, lightTexture, darkTexture ) );
}
	
void LilyPad::initialize( const ci::JsonTree &root )
{
	try {
        cout << root << endl;
		Renderable::initialize( root["transformation"] );
	}
	catch ( const JsonTree::ExcChildNotFound &ex ) {
		CI_LOG_E(ex.what());
	}
}
	
void LilyPad::draw()
{
	gl::ScopedModelMatrix scopeModel;
	gl::setModelMatrix( getModelMatrix() );
	{
		gl::ScopedColor scopeColor( ColorA( 1, 1, 1, mVisibility ) );
		gl::draw( mLightTex );
	}
	{
		gl::ScopedColor scopeColor( ColorA( 1, 1, 1, 1.0 - mVisibility ) );
		gl::draw( mDarkTex );
	}
}
	
void LilyPad::activate( bool activate )
{
	if ( mIsActivated == activate ) return;
	
	auto& timeline = app::App::get()->timeline();
	
	if( activate ) {
		timeline.applyPtr( &mVisibility, 1.0f, 1.0f );
	}
	else {
		timeline.applyPtr( &mVisibility, 0.0f, 1.0f );
	}
	mIsActivated = activate;
}
	
}