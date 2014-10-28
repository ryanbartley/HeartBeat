#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/params/Params.h"

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
	
	void drawKey();
	void drawColorString( const ci::vec2 &size, const std::string &str, const Colorf &background, const Colorf &text );
	
	void resize() override;
	
	void updateFar()
	{
		if( mFar < mApproach ) {
			mFar = mApproach;
		}
		mFarUpdated = true;
	}
	void updateApproach()
	{
		if( mApproach < mDead ) {
			mApproach = mDead;
		}
		else if( mApproach > mFar ) {
			mApproach = mFar;
		}
		mApproachUpdated = true;
	}
	void updateDead()
	{
		if( mDead < mTable ) {
			mDead = mTable;
		}
		else if( mDead > mApproach ) {
			mDead = mApproach;
		}
		mDeadUpdated = true;
	}
	void updateTable()
	{
		if( mTable > mDead ) {
			mTable = mDead;
		}
		mTableUpdated = true;
	}
	
	void updateZones() {
		using namespace heartbeat;
		if( mFarUpdated ) {
			mInteractionZones->setZoneScalar( InteractionZones::Zone::FAR, mFar );
		}
		if( mApproachUpdated ) {
			mInteractionZones->setZoneScalar( InteractionZones::Zone::APPROACH, mApproach );
		}
		if( mDeadUpdated ) {
			mInteractionZones->setZoneScalar( InteractionZones::Zone::DEAD, mDead );
		}
		if( mTableUpdated ) {
			mInteractionZones->setZoneScalar( InteractionZones::Zone::TABLE, mTable );
		}
	}
	
	
	heartbeat::HidCommManagerRef	mHidComm;
	heartbeat::JsonManagerRef		mJsonManager;
	heartbeat::EventManagerRef		mEventManager;
	heartbeat::InteractionZonesRef	mInteractionZones;
	heartbeat::InteractionDebugRenderableRef mInteractionZonesDebug;
	heartbeat::KioskManagerRef		mKioskManager;
	
	params::InterfaceGlRef			mParams;
	bool							mFarUpdated, mApproachUpdated, mDeadUpdated, mTableUpdated;
	float							mFar, mApproach, mDead, mTable;
};

void hidTestApp::setup()
{
	mJsonManager				= heartbeat::JsonManager::create("test.json");
	mEventManager				= heartbeat::EventManager::create( "global", true );
	mInteractionZones			= heartbeat::InteractionZones::create();
	mInteractionZonesDebug		= heartbeat::InteractionDebugRenderable::create( mInteractionZones );
	mKioskManager				= heartbeat::KioskManager::create();
	mKioskManager->initialize();
	
	cout << "Initialized Interaction Zones" << endl;
	mInteractionZones->initialize();
	mInteractionZonesDebug->initialize();
	
	mInteractionZonesDebug->enableDistance();
	mInteractionZonesDebug->enableZone();
	
	using namespace heartbeat;
	
	mFar = mInteractionZones->getZoneScalar( InteractionZones::Zone::FAR );
	mApproach = mInteractionZones->getZoneScalar( InteractionZones::Zone::APPROACH );
	mDead = mInteractionZones->getZoneScalar( InteractionZones::Zone::DEAD );
	mTable = mInteractionZones->getZoneScalar( InteractionZones::Zone::TABLE );
	
	mParams = params::InterfaceGl::create( "Interaction Zone Debug", ivec2( 300, 400 ) );
	
	// Unsafe but needed.
	mParams->addButton( "Capture Table Barrier", [&]() {
		mInteractionZones->captureBarrier();
		mInteractionZonesDebug->enableZone();
	});
	mParams->addButton( "Write Table Barrier", [&]() {
		mInteractionZones->writeInteractionZone();
	});
	
	mParams->addButton( "Toggle Live Distance", [&](){
		mInteractionZonesDebug->enableDistance( ! mInteractionZonesDebug->isDistanceEnabled() );
	});
	mParams->addButton( "Toggle Table Barrier", [&](){
		mInteractionZonesDebug->enableZone( ! mInteractionZonesDebug->isZoneEnabled() );
	});
	mParams->addSeparator();
	mParams->addButton( "Query/Store Ignore Indices(Poles)", std::bind( &InteractionZones::queryIgnoreIndices, mInteractionZones ) );
	mParams->addButton( "Print Current Ignore Indices(Poles)", [&](){
		std::cout << "Printing ignore indices: ";
		auto ignoreIndices = mInteractionZones->getIgnoreIndices();
		int last = 0;
		for( auto & indice : ignoreIndices ) {
			if( last == 0 ) {
				last = indice;
			}
			if( last != indice-1 ) {
				cout << endl << "  New Ignore Zone: ";
			}
			cout << indice << ", ";
			last = indice;
		}
		std::cout << std::endl;
	});
	mParams->addButton( "Clear Current Ignore Indices(Poles)", [&](){
		const_cast<std::vector<uint32_t>&>(mInteractionZones->getIgnoreIndices()).clear();
	});
	mParams->addSeparator();
	mParams->addText("Zone Scalars (No Overlapping)");
	mParams->addParam("Set Far Zone", &mFar).updateFn( std::bind( &hidTestApp::updateFar, this ) );
	mParams->addParam("Set Approach Zone", &mApproach).updateFn( std::bind( &hidTestApp::updateApproach, this ) );
	mParams->addParam("Set Dead Zone", &mDead).updateFn( std::bind( &hidTestApp::updateDead, this ) );
	mParams->addParam("Set Table Zone", &mTable).updateFn( std::bind( &hidTestApp::updateTable, this ) );
	mParams->addButton("Submit Zone Changes", std::bind( &hidTestApp::updateZones, this ) );
	mParams->addSeparator();
	mParams->addText("Teensy Settings");
	mParams->addButton("Send Top Teensy Approach", [&](){
		mEventManager->queueEvent( EventDataRef( new ApproachEvent( KioskId::TOP_KIOSK ) ) );
	});
	mParams->addButton("Send Top Teensy Depart", [&](){
		mEventManager->queueEvent( EventDataRef( new DepartEvent( KioskId::TOP_KIOSK ) ) );
	});
	mParams->addButton("Send Middle Teensy Approach", [&](){
		mEventManager->queueEvent( EventDataRef( new ApproachEvent( KioskId::MIDDLE_KIOSK ) ) );
	});
	mParams->addButton("Send Middle Teensy Depart", [&](){
		mEventManager->queueEvent( EventDataRef( new DepartEvent( KioskId::MIDDLE_KIOSK ) ) );
	});
	mParams->addButton("Send Bottom Teensy Approach", [&](){
		mEventManager->queueEvent( EventDataRef( new ApproachEvent( KioskId::BOTTOM_KIOSK ) ) );
	});
	mParams->addButton("Send Bottom Teensy Depart", [&](){
		mEventManager->queueEvent( EventDataRef( new DepartEvent( KioskId::BOTTOM_KIOSK ) ) );
	});
	
//	mParams->addButton("Set Teensy TOP", );
}

void hidTestApp::resize()
{
	mInteractionZonesDebug->setTranslation( getWindowCenter() );
}

void hidTestApp::mouseDown( MouseEvent event )
{
//	mEventManager->queueEvent( EventDataRef(  ) );
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
	
	if( event.getChar() == 'q' ) {
		mInteractionZones->queryIgnoreIndices();
	}
	
	if( event.getChar() == 'p' ) {
		std::cout << "Printing ignore indices: ";
		auto ignoreIndices = mInteractionZones->getIgnoreIndices();
		int last = 0;
		for( auto & indice : ignoreIndices ) {
			if( last == 0 ) {
				last = indice;
			}
			if( last != indice-1 ) {
				cout << endl << "  New Ignore Zone: ";
			}
			cout << indice << ", ";
			last = indice;
		}
		std::cout << std::endl;
	}
}

void hidTestApp::drawColorString( const ci::vec2 &size, const std::string &str, const Colorf &background, const Colorf &text )
{
	gl::color( background );
	gl::drawSolidRect( Rectf( 0, 0, size.x, size.y ) );
	gl::color( 1, 1, 1 );
	gl::drawStringCentered( str, vec2( size.x / 2.0f, size.y / 2.0f ), text );
}

void hidTestApp::update()
{
	mEventManager->update();
	mInteractionZones->process();
	mInteractionZonesDebug->update();
}

void hidTestApp::drawKey()
{
	gl::enableAlphaBlending();
	auto size = vec2( 200, 40 );
	{
		gl::ScopedModelMatrix scopeModel;
		gl::multModelMatrix( ci::translate( vec3( getWindowWidth() - size.x, 0 * size.y, 0 )  ) );
		drawColorString( vec2( 100, 10 ), "Zone Data Key", Color( 0, 0, 0 ), Color::white()  );
	}
	{
		gl::ScopedModelMatrix scopeModel;
		gl::multModelMatrix( ci::translate( vec3( getWindowWidth() - size.x, 15 + size.y * 0, 0 )  ) );
		drawColorString( size, "Touch Area", Color( 1.0, 0.0, 0.0 ), Color::black() );
	}
	{
		gl::ScopedModelMatrix scopeModel;
		gl::multModelMatrix( ci::translate( vec3( getWindowWidth() - size.x, 15 + size.y * 1, 0 )  ) );
		drawColorString( size, "Dead Area", Color( .5, .7, .3 ), Color::black() );
	}
	{
		gl::ScopedModelMatrix scopeModel;
		gl::multModelMatrix( ci::translate( vec3( getWindowWidth() - size.x, 15 + size.y * 2, 0 )  ) );
		drawColorString( size, "Approach Area", Color( 0, 1, .5 ), Color::black() );
	}
	{
		gl::ScopedModelMatrix scopeModel;
		gl::multModelMatrix( ci::translate( vec3( getWindowWidth() - size.x, 15 + size.y * 3, 0 )  ) );
		drawColorString( size, "Far Area", Color( .6, .1, .8 ), Color::black() );
	}
	
	{
		gl::ScopedModelMatrix scopeModel;
		gl::multModelMatrix( ci::translate( vec3( getWindowWidth() - size.x, 50 + size.y * 4, 0 )  ) );
		drawColorString( vec2( 100, 10 ), "Live Distance Key", Color( 0, 0, 0 ), Color::white() );
	}
	{
		gl::ScopedModelMatrix scopeModel;
		gl::multModelMatrix( ci::translate( vec3( getWindowWidth() - size.x, 30 + size.y * 5, 0 )  ) );
		drawColorString( size, "Registered Touch", Color( 1, 0, .5 ), Color::black() );
	}
	{
		gl::ScopedModelMatrix scopeModel;
		gl::multModelMatrix( ci::translate( vec3( getWindowWidth() - size.x, 30 + size.y * 6, 0 )  ) );
		drawColorString( size, "Reg. Approach", Color( 1.0, 1.0, 0.0 ), Color::black() );
	}
	{
		gl::ScopedModelMatrix scopeModel;
		gl::multModelMatrix( ci::translate( vec3( getWindowWidth() - size.x, 30 + size.y * 7, 0 )  ) );
		drawColorString( size, "Unreg. Index", Color( 1, 1, 1 ), Color::black() );
	}
	gl::disableAlphaBlending();
}

void hidTestApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
	
	gl::setMatricesWindow( getWindowSize() );
	
	mInteractionZonesDebug->draw();
	
	drawKey();
	
	mParams->draw();
}

CINDER_APP_NATIVE( hidTestApp, RendererGl )
