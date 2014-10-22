#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "JsonManager.h"
#include "InteractionZones.h"
#include "InteractionDebugRenderable.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class LibUrgTestApp : public AppNative {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void keyDown( KeyEvent event ) override;
	void update() override;
	void draw() override;
	
	heartbeat::InteractionZonesRef mInteractionZone;
	heartbeat::InteractionDebugRenderableRef mInteractionDebug;
	heartbeat::JsonManagerRef		mJsonManager;
};

void LibUrgTestApp::setup()
{
	mJsonManager		= heartbeat::JsonManager::create( "test.json" );
	mInteractionZone	= heartbeat::InteractionZones::create();
	mInteractionDebug	= heartbeat::InteractionDebugRenderable::create( mInteractionZone );
	
	mInteractionZone->initialize();
	mInteractionDebug->initialize();
	
	mInteractionDebug->enableZone();
	mInteractionDebug->enableDistance();
}

void LibUrgTestApp::mouseDown( MouseEvent event )
{
	mInteractionZone->writeInteractionZone();
}

void LibUrgTestApp::keyDown( KeyEvent event )
{
	if( event.getChar() == ' ' ) {
		mInteractionZone->captureBarrier();
		mInteractionDebug->enableZone();
	}
	
	if( event.getChar() == 'd' ) {
		mInteractionDebug->enableDistance( ! mInteractionDebug->isDistanceEnabled() );
	}
	
	if( event.getChar() == 'z' ) {
		mInteractionDebug->enableZone( ! mInteractionDebug->isZoneEnabled() );
	}
}

void LibUrgTestApp::update()
{
	mInteractionZone->process();
}

void LibUrgTestApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
	
	gl::setMatricesWindow( getWindowSize() );
	
	mInteractionDebug->draw();
}

CINDER_APP_NATIVE( LibUrgTestApp, RendererGl )
