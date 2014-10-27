#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "JsonManager.h"
#include "HidCommManager.h"
#include "EventManager.h"
#include "InteractionZones.h"
#include "InteractionDebugRenderable.h"
#include "KioskManager.h"
#include "InteractionEvents.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class hidTestApp : public AppNative {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void keyDown( KeyEvent event ) override;
	void update() override;
	void draw() override;
	
	heartbeat::HidCommManagerRef	mHidComm;
	heartbeat::JsonManagerRef		mJsonManager;
	heartbeat::EventManagerRef		mEventManager;
	heartbeat::InteractionZonesRef	mInteractionZones;
	heartbeat::InteractionDebugRenderableRef mInteractionZonesDebug;
	heartbeat::KioskManagerRef		mKioskManager;
	
};

void hidTestApp::setup()
{
	mJsonManager				= heartbeat::JsonManager::create("test.json");
	mEventManager				= heartbeat::EventManager::create( "global", true );
	mInteractionZones			= heartbeat::InteractionZones::create();
	mInteractionZonesDebug		= heartbeat::InteractionDebugRenderable::create( mInteractionZones );
	mKioskManager				= heartbeat::KioskManager::create();
	mKioskManager->initialize();
	
	mInteractionZones->initialize();
	mInteractionZonesDebug->initialize();
	
	mInteractionZonesDebug->enableDistance();
	mInteractionZonesDebug->enableZone();
}

void hidTestApp::mouseDown( MouseEvent event )
{
	auto division = getWindowHeight() / 3.0f;
	if( event.getPos().y < division )
		mEventManager->queueEvent( heartbeat::EventDataRef( new heartbeat::ApproachEvent( heartbeat::KioskId::TOP_KIOSK)  ) );
	else if( event.getPos().y < (division * 2) )
		mEventManager->queueEvent( heartbeat::EventDataRef( new heartbeat::DepartEvent( heartbeat::KioskId::TOP_KIOSK)  ) );
	else
		mEventManager->queueEvent( heartbeat::EventDataRef( new heartbeat::TableEvent( 100, 200 )  ) );
}

void hidTestApp::keyDown( cinder::app::KeyEvent event )
{
	if( event.getChar() == ' ' ) {
		mInteractionZones->captureBarrier();
		mInteractionZonesDebug->enableZone();
	}
	if( event.getChar() == 'w' ) {
		mInteractionZones->writeInteractionZone();
	}
	
	if( event.getChar() == 'd' ) {
		mInteractionZonesDebug->enableDistance( ! mInteractionZonesDebug->isDistanceEnabled() );
	}
	
	if( event.getChar() == 'z' ) {
		mInteractionZonesDebug->enableZone( ! mInteractionZonesDebug->isZoneEnabled() );
	}
}

void hidTestApp::update()
{
	mEventManager->update();
	mInteractionZones->process();
	mInteractionZonesDebug->update();
}

void hidTestApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
	
	gl::setMatricesWindow( getWindowSize() );
	
	mInteractionZonesDebug->draw();
}

CINDER_APP_NATIVE( hidTestApp, RendererGl )
