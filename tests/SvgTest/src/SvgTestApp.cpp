#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/gl/Texture.h"

#include "cinder/svg/Svg.h"
#include "Cairo.h"

#include "SvgManager.h"
#include "JsonManager.h"
#include "EventManager.h"
#include "Node.h"
#include "KioskManager.h"
#include "InfoDisplay.h"
#include "InteractionEvents.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class SvgTestApp : public AppNative {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void keyDown( KeyEvent event ) override;
	void update() override;
	void draw() override;

	heartbeat::EventManagerRef  mEventManager;
	heartbeat::SvgManagerRef	mManager;
	heartbeat::JsonManagerRef	mJsonManager;
	heartbeat::KioskManagerRef	mKioskManager;
	ci::gl::Texture2dRef		mTexture;
	std::map<std::string, heartbeat::PageRef> mPages;
	std::map<std::string, heartbeat::ButtonRef> mButtons;
	int mIndex;
	bool debugRenderPages;
};

void SvgTestApp::setup()
{
	mEventManager = heartbeat::EventManager::create( "Global", true );
	mJsonManager = heartbeat::JsonManager::create( "test.json" );
	
	mManager = heartbeat::SvgManager::create();
	mManager->initialize();
	mPages = mManager->getPages();
	mButtons = mManager->getButtons();
	mIndex = 0;
	debugRenderPages = true;
	
	mKioskManager = heartbeat::KioskManager::create();
	mKioskManager->initialize();
}

void SvgTestApp::mouseDown( MouseEvent event )
{
	mEventManager->queueEvent( heartbeat::EventDataRef( new heartbeat::TouchEvent( event.getPos() ) ) );
}

void SvgTestApp::keyDown( cinder::app::KeyEvent event )
{
	auto displays = mKioskManager->getInfoDisplays();
	displays[0]->toggleStatus();
}

void SvgTestApp::update()
{
	mEventManager->update();
}

void SvgTestApp::draw()
{
	static int lastIndex = mIndex;
	gl::clear( Color::black() );
	gl::setMatricesWindow( getWindowSize() );
	
//	if( debugRenderPages ) {
//		auto page = mPages.begin();
//		
//		if( mIndex < mPages.size() ) {
//			for( int i = 0; i < mIndex; i++ ) {
//				page++;
//			}
//		}
//		gl::enableAlphaBlending();
//		gl::color( Color::white() );
//		page->second->debugRender();
//		
//		if( mIndex != lastIndex ) {
//			cout << page->second->getGroupName() << endl;
//			lastIndex = mIndex;
//		}
//	}
//	else {
//		auto button = mButtons.begin();
//		
//		if( mIndex < mButtons.size() ) {
//			for( int i = 0; i < mIndex; i++ ) {
//				button++;
//			}
//		}
//		
//		gl::enableAlphaBlending();
//		gl::color( Color::white() );
//		button->second->debugRender();
//		
//		if( mIndex != lastIndex ) {
//			cout << button->second->getGroupName() << endl;
//			lastIndex = mIndex;
//		}
//	}
	mKioskManager->render();
}

CINDER_APP_NATIVE( SvgTestApp, RendererGl )
