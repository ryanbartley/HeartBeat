#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "Engine.h"
#include "KioskManager.h"
#include "InfoDisplay.h"

#if defined( DEBUG )
#include "InteractionZones.h"
#include "InteractionDebugRenderable.h"
#include "InteractionEvents.h"
#include "EventManager.h"
#include "Renderer.h"
#include "LilyPadOverlay.h"
#endif

using namespace ci;
using namespace ci::app;
using namespace std;

class FullProjectApp : public AppNative {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void mouseDrag( MouseEvent event ) override;
	void update() override;
	void draw() override;
    
    void keyDown( KeyEvent event ) override;
	
	heartbeat::EngineRef mEngine;
	
	
#if defined( DEBUG )
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
	
	bool							mFarUpdated, mApproachUpdated, mDeadUpdated, mTableUpdated;
	float							mFar, mApproach, mDead, mTable;
	
	std::array<heartbeat::InfoDisplayRef, 3>	mInfoDisplays;
	std::array<heartbeat::LilyPadRef, 3>		mLilyPads;
	heartbeat::InteractionZonesRef				mInteractionZones;
	heartbeat::InteractionDebugRenderableRef	mInteractionDebug;
	heartbeat::EventManagerRef					mEventManager;
	std::array<float, 7> mRotations;
	std::array<float, 7> mScales;
	std::array<vec3, 7> mTranslations;
	params::InterfaceGlRef mParams;
	bool										mShowParams;
#endif
};

void FullProjectApp::setup()
{
	mEngine = heartbeat::Engine::create();
#if defined( DEBUG )
	auto kioskMan = mEngine->getKioskManager();
	mInfoDisplays = kioskMan->getInfoDisplays();
	mLilyPads = kioskMan->getLilyPads();
	
	mScales.fill( 1.0f );
	mRotations.fill( 0 );
	
	mParams = params::InterfaceGl::create( "fullProject", ivec2( 400, 400 ) );
	mParams->setPosition( getWindowCenter() );
	mTranslations[0] = mInfoDisplays[0]->getTranslation();
	mTranslations[1] = mInfoDisplays[1]->getTranslation();
	mTranslations[2] = mInfoDisplays[2]->getTranslation();
	
	mParams->addText("Kiosk 1 Transform");
	mParams->addParam( "Rotation Kiosk 1", &mRotations[0] ).updateFn( [&](){ mInfoDisplays[0]->setRotationDegree( mRotations[0] ); } );
	mParams->addParam( "Scale Kiosk 1", &mScales[0] ).updateFn( [&](){ mInfoDisplays[0]->setScale( vec2(mScales[0]) ); } );
	mParams->addParam( "Translation Kiosk 1", &mTranslations[0] ).updateFn( [&](){ mInfoDisplays[0]->setTranslation( vec2(mTranslations[0]) ); } );
	mParams->addSeparator();
	mParams->addText("Kiosk 2 Transform");
	mParams->addParam( "Rotation Kiosk 2", &mRotations[1] ).updateFn( [&](){ mInfoDisplays[1]->setRotationDegree( mRotations[1] ); } );
	mParams->addParam( "Scale Kiosk 2", &mScales[1] ).updateFn( [&](){ mInfoDisplays[1]->setScale( vec2(mScales[1]) ); } );
	mParams->addParam( "Translation Kiosk 2", &mTranslations[1] ).updateFn( [&](){ mInfoDisplays[1]->setTranslation( vec2(mTranslations[1]) ); } );
	mParams->addSeparator();
	mParams->addText("Kiosk 3 Transform");
	mParams->addParam( "Rotation Kiosk 3", &mRotations[2] ).updateFn( [&](){ mInfoDisplays[2]->setRotationDegree( mRotations[2] ); } );
	mParams->addParam( "Scale Kiosk 3", &mScales[2] ).updateFn( [&](){ mInfoDisplays[2]->setScale( vec2(mScales[2]) ); } );
	mParams->addParam( "Translation Kiosk 3", &mTranslations[2] ).updateFn( [&](){ mInfoDisplays[2]->setTranslation( vec2(mTranslations[2]) ); } );
	mParams->addSeparator();
	mParams->addText("Interaction Zone Transform");
	mParams->addParam( "Rotation Interaction Zone", &mRotations[3] ).updateFn( [&](){ mInteractionDebug->setRotationDegree( mRotations[3] ); } );   
	mParams->addParam( "Scale Interaction Zone", &mScales[3] ).updateFn( [&](){ mInteractionDebug->setScale( vec2(mScales[3]) ); } );
	mParams->addParam( "Translation Interaction Zone", &mTranslations[3] ).updateFn( [&](){ mInteractionDebug->setTranslation( vec2(mTranslations[3]) ); } );
	mParams->addButton( "Submit Interaction Zone Transform", [&]() {
		auto & transform = mInteractionZones->getTransform();
		transform.setRotation( mInteractionDebug->getRotation() );
		transform.setScale( mInteractionDebug->getScale() );
		transform.setTranslation( mInteractionDebug->getTranslation() );
	});
	mParams->addSeparator();
	mParams->addText("LilyPad 1 Transform");
	mParams->addParam( "Rotation LilyPad 1", &mRotations[4] ).updateFn( [&](){ mLilyPads[0]->setRotationDegree( mRotations[4] ); } );
	mParams->addParam( "Scale LilyPad 1", &mScales[4] ).updateFn( [&](){ mLilyPads[0]->setScale( vec2(mScales[4]) ); } );
	mParams->addParam( "Translation LilyPad 1", &mTranslations[4] ).updateFn( [&](){ mLilyPads[0]->setTranslation( vec2(mTranslations[4]) ); } );
	mParams->addSeparator();
	mParams->addText("LilyPad 2 Transform");
	mParams->addParam( "Rotation LilyPad 2", &mRotations[5] ).updateFn( [&](){ mLilyPads[1]->setRotationDegree( mRotations[5] ); } );
	mParams->addParam( "Scale LilyPad 2", &mScales[5] ).updateFn( [&](){ mLilyPads[1]->setScale( vec2(mScales[5]) ); } );
	mParams->addParam( "Translation LilyPad 2", &mTranslations[5] ).updateFn( [&](){ mLilyPads[1]->setTranslation( vec2(mTranslations[5]) ); } );
	mParams->addSeparator();
	mParams->addText("LilyPad 3 Transform");
	mParams->addParam( "Rotation LilyPad 3", &mRotations[6] ).updateFn( [&](){ mLilyPads[2]->setRotationDegree( mRotations[6] ); } );
	mParams->addParam( "Scale LilyPad 3", &mScales[6] ).updateFn( [&](){ mLilyPads[2]->setScale( vec2(mScales[6]) ); } );
	mParams->addParam( "Translation LilyPad 3", &mTranslations[6] ).updateFn( [&](){ mLilyPads[2]->setTranslation( vec2(mTranslations[6]) ); } );
	
	mParams->addSeparator();
	using namespace heartbeat;
	
	mInteractionDebug = mEngine->getInteractionDebug();
	mInteractionZones = mEngine->getInteractionZones();
	mEventManager = mEngine->getEventManager();
	mTranslations[3] = mInteractionDebug->getTranslation();
	
	mFar = mInteractionZones->getZoneScalar( InteractionZones::Zone::FAR );
	mApproach = mInteractionZones->getZoneScalar( InteractionZones::Zone::APPROACH );
	mDead = mInteractionZones->getZoneScalar( InteractionZones::Zone::DEAD );
	mTable = mInteractionZones->getZoneScalar( InteractionZones::Zone::TABLE );
	
	// Unsafe but needed.
	mParams->addButton( "Capture Table Barrier", [&]() {
		mInteractionZones->captureBarrier();
		mInteractionDebug->enableZone();
	});
	mParams->addButton( "Write Table Barrier", [&]() {
		mInteractionZones->writeInteractionZone();
	});
	
	mParams->addButton( "Toggle Live Distance", [&](){
		mInteractionDebug->enableDistance( ! mInteractionDebug->isDistanceEnabled() );
	});
	mParams->addButton( "Toggle Table Barrier", [&](){
		mInteractionDebug->enableZone( ! mInteractionDebug->isZoneEnabled() );
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
	mParams->addParam("Set Far Zone", &mFar).updateFn( std::bind( &FullProjectApp::updateFar, this ) );
	mParams->addParam("Set Approach Zone", &mApproach).updateFn( std::bind( &FullProjectApp::updateApproach, this ) );
	mParams->addParam("Set Dead Zone", &mDead).updateFn( std::bind( &FullProjectApp::updateDead, this ) );
	mParams->addParam("Set Table Zone", &mTable).updateFn( std::bind( &FullProjectApp::updateTable, this ) );
	mParams->addButton("Submit Zone Changes", std::bind( &FullProjectApp::updateZones, this ) );
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
	mShowParams = true;
	mParams->show( mShowParams );
	
	mEngine->setParams( mParams );
#endif
}

void FullProjectApp::keyDown(cinder::app::KeyEvent event)
{

    if( event.getCode() == KeyEvent::KEY_ESCAPE ) {
        quit();
    }
#if defined( DEBUG )
	if( event.getChar() == KeyEvent::KEY_p ) {
		mShowParams = ! mShowParams;
		mParams->show( mShowParams );
	}
#endif
}

void FullProjectApp::mouseDown( MouseEvent event )
{
#if defined( DEBUG )
	if( mShowParams ) {
		mParams->setPosition( event.getPos() );
	} 
	else {
		ci::vec2 eventPosition = event.getPos();
		auto renderer = mEngine->getRenderer();
		if( event.getPos().y > renderer->getBottomPresentationTarget()->getSize().y  ) {
			eventPosition.y = eventPosition.y - renderer->getNumPixelOverlap();
		}
		mEventManager->queueEvent( heartbeat::EventDataRef( new heartbeat::TouchBeganEvent( 1, eventPosition ) ) );
	}
#endif
}

void FullProjectApp::mouseDrag( cinder::app::MouseEvent event )
{
#if defined( DEBUG )
	if( ! mShowParams ) {
		ci::vec2 eventPosition = event.getPos();
		auto renderer = mEngine->getRenderer();
		if( event.getPos().y > renderer->getBottomPresentationTarget()->getSize().y  ) {
			eventPosition.y = eventPosition.y - renderer->getNumPixelOverlap();
		}
		mEventManager->queueEvent( heartbeat::EventDataRef( new heartbeat::TouchMoveEvent( 1, eventPosition ) ) );
	}
#endif
}

void FullProjectApp::update()
{
}

void FullProjectApp::draw()
{
//	gl::clear( Color( 0, 0, 0 ) );
//	gl::setMatricesWindow( getWindowSize() );
//	mParams->draw();
}

CINDER_APP_NATIVE( FullProjectApp, RendererGl )
