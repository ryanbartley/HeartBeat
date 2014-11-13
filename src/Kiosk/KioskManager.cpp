//
//  KioskManager.cpp
//  SvgTest
//
//  Created by Ryan Bartley on 10/19/14.
//
//

#include "KioskManager.h"
#include "InteractionEvents.h"
#include "EventManager.h"
#include "InfoDisplay.h"
#include "HidCommManager.h"
#include "JsonManager.h"
#include "ButtonTypes.h"
#include "LilyPadOverlay.h"
#include "Engine.h"
#include "Renderer.h"


#include "cinder/gl/Fbo.h"
#include "cinder/Log.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace heartbeat {
	
KioskManager::KioskManager()
#if defined( DEBUG )
: mDebugRenderInfoDisplay( false )
#endif
{
	auto eventManager = EventManagerBase::get();
	if( eventManager ) {
		eventManager->addListener( std::bind( &KioskManager::approachDelegate, this, std::placeholders::_1 ), ApproachEvent::TYPE );
		eventManager->addListener( std::bind( &KioskManager::departDelegate, this, std::placeholders::_1 ), DepartEvent::TYPE );
		eventManager->addListener( std::bind( &KioskManager::touchBeganDelegate, this, std::placeholders::_1 ), TouchBeganEvent::TYPE );
		eventManager->addListener( std::bind( &KioskManager::touchMovedDelegate, this, std::placeholders::_1 ), TouchMoveEvent::TYPE );
		eventManager->addListener( std::bind( &KioskManager::touchEndedDelegate, this, std::placeholders::_1 ), TouchEndedEvent::TYPE );
		CI_LOG_V("KioskManager has registered it's event listeners");
	}
	else
		CI_LOG_W("KioskManager has NOT registered it's event listeners");
	
}
	
KioskManager::~KioskManager()
{

}
	
KioskManagerRef KioskManager::create()
{
	return KioskManagerRef( new KioskManager );
}
	
void KioskManager::approachDelegate( EventDataRef approachEvent )
{
	auto event = std::dynamic_pointer_cast<ApproachEvent>( approachEvent );
	
	if( ! event ) {
		CI_LOG_E("Not an ApproachEvent " << approachEvent->getName() );
		return;
	}
	auto kioskId = event->getKiosk();
	auto kioskIndex = static_cast<int>(kioskId);
	
	CI_LOG_V("Got a approachEvent for " << getKiosk( kioskId ));
	
	mHidCommManager->activate( kioskId, true );
	mLilyPads[kioskIndex]->activate( true );
	mDisplays[kioskIndex]->activate( true );
}
	
void KioskManager::departDelegate( EventDataRef departEvent )
{
	auto event = std::dynamic_pointer_cast<DepartEvent>( departEvent );
	
	if( ! event ) {
		CI_LOG_E("Not a DepartEvent " << departEvent->getName() );
		return;
	}
	auto kioskId = event->getKiosk();
	auto kioskIndex = static_cast<int>(kioskId);
	
	CI_LOG_V("Got a departEvent for " << static_cast<int>( kioskId ));
	
	mHidCommManager->activate( kioskId, false );
	mLilyPads[kioskIndex]->activate( false );
	mDisplays[kioskIndex]->activate( false );
}
	
void KioskManager::touchBeganDelegate( EventDataRef touchEvent )
{
	auto event = std::dynamic_pointer_cast<TouchBeganEvent>( touchEvent );
	
	if( ! event ) {
		CI_LOG_V("Not an touchEvent " << touchEvent->getName() );
		return;
	}
	
	for( auto & kiosk : mDisplays ) {
		if( kiosk->insideAngle( event->getIndex() ) ) {
			kiosk->registerTouchBegan( event );
		}
	}
}
	
void KioskManager::touchMovedDelegate( EventDataRef touchEvent )
{
	auto event = std::dynamic_pointer_cast<TouchMoveEvent>( touchEvent );
	
	if( ! event ) {
		CI_LOG_V("Not an touchEvent " << touchEvent->getName() );
		return;
	}
	
	for( auto & kiosk : mDisplays ) {
		if( kiosk->insideAngle( event->getIndex() ) ) {
			kiosk->registerTouchMoved( event );
		}
	}
}
	
void KioskManager::touchEndedDelegate( EventDataRef touchEvent )
{
	auto event = std::dynamic_pointer_cast<TouchEndedEvent>( touchEvent );
	
	if( ! event ) {
		CI_LOG_V("Not an touchEvent " << touchEvent->getName() );
		return;
	}
	
	for( auto & kiosk : mDisplays ) {
		if( kiosk->insideAngle( event->getIndex() ) ) {
			kiosk->registerTouchEnded( event );
		}
	}
}

#if defined( DEBUG )
void KioskManager::toggleDebugRenderInfoDisplay()
{
	mDebugRenderInfoDisplay = ! mDebugRenderInfoDisplay;
}
	
void KioskManager::toggleDebugBoundingBoxes()
{
	for( auto & kiosk : mDisplays ) {
		kiosk->enableBoundingBoxRender( ! kiosk->getBoundingBoxRender() );
	}
}
#endif
	
void KioskManager::update()
{
	for( auto & kiosk : mDisplays ) {
		kiosk->update();
	}
}
	
void KioskManager::render()
{
	gl::enableAlphaBlending();
#if defined( DEBUG )
	if( ! mDebugRenderInfoDisplay ) {
#endif
		for( auto & kiosk : mDisplays ) {
			kiosk->draw();
		}
		
		for ( auto & lilyPad : mLilyPads ) {
			lilyPad->draw();
		}
#if defined( DEBUG )
	}
	else {
		mDisplays[0]->renderToFbo();
		auto fbo = mDisplays[0]->getPresentationFbo();
		gl::ScopedMatrices scopeMatrix;
        auto scale = Engine::get()->getRenderer()->isHalfSize();
		gl::multModelMatrix( ci::scale( vec3( scale ? 0.5 : 1, scale ? 0.5 : 1, 0 ) ) );
		gl::draw( fbo->getColorTexture() );
	}
#endif
	
	gl::disableAlphaBlending();
}
	
void KioskManager::initialize()
{
	auto svgManager = SvgManager::get();
	
	if( ! svgManager ) {
		CI_LOG_W("No SvgManager, quitting initializing KioskManager");
		return;
	}
	
	mHidCommManager = HidCommManager::create();
	mHidCommManager->initialize();
	
	try {
		auto kioskAttribs = JsonManager::get()->getRoot()["kioskManagerAttribs"];
		
		try {
			auto infoDisplays = kioskAttribs["infoDisplays"];
			auto topIndex = static_cast<int>(KioskId::TOP_KIOSK);
			auto middleIndex = static_cast<int>(KioskId::MIDDLE_KIOSK);
			auto bottomIndex = static_cast<int>(KioskId::BOTTOM_KIOSK);
			
			ci::gl::Texture2dRef lightTex, darkTex;
			
			try {
				auto lilyTexLightName = infoDisplays["lilyTexLightName"].getValue();
				lightTex = gl::Texture2d::create( loadImage( getFileContents( lilyTexLightName ) ) );
				auto lilyTexDarkName = infoDisplays["lilyTexDarkName"].getValue();
				darkTex = gl::Texture2d::create( loadImage( getFileContents( lilyTexDarkName ) ) );
			}
			catch( const JsonTree::ExcChildNotFound &ex ) {
				CI_LOG_E(ex.what());
			}
			
			gl::Fbo::Format format;
			format.colorTexture( gl::Texture2d::Format()
								.mipmap()
								.minFilter( GL_LINEAR_MIPMAP_LINEAR ) 
            .maxAnisotropy( gl::Texture2d::getMaxMaxAnisotropy() ) );
			
			
			auto size = svgManager->getDoc()->getSize();
			CI_LOG_V("Size of doc is: " << size);
			auto globalFbo = gl::Fbo::create( size.x, size.y, format );
			
			try {
				auto top = infoDisplays["top"];
				try {
					auto topDisplay = InfoDisplay::create( KioskId::TOP_KIOSK );
					topDisplay->initiaize( top );
					topDisplay->setPresentFbo( globalFbo );
					
					mDisplays[topIndex] = topDisplay;
				}
				catch ( const JsonTree::ExcChildNotFound &ex ) {
					CI_LOG_E(ex.what());
				}
				
				try {
					auto topLilyPad = LilyPad::create( KioskId::TOP_KIOSK, lightTex, darkTex );
					topLilyPad->initialize( top["lilyPad"] );
					mLilyPads[topIndex] = topLilyPad;
				}
				catch ( const JsonTree::ExcChildNotFound &ex ) {
					CI_LOG_E(ex.what());
				}
			
			}
			catch ( const JsonTree::ExcChildNotFound &ex ) {
				CI_LOG_E(ex.what());
			}
			
			try {
				auto middle = infoDisplays["middle"];
				
				try {
					auto middleDisplay = InfoDisplay::create( KioskId::TOP_KIOSK );
					middleDisplay->initiaize( middle );
					middleDisplay->setPresentFbo( globalFbo );
					
					mDisplays[middleIndex] = middleDisplay;
				}
				catch ( const JsonTree::ExcChildNotFound &ex ) {
					CI_LOG_E(ex.what());
				}
				
				try {
					auto middleLilyPad = LilyPad::create( KioskId::MIDDLE_KIOSK, lightTex, darkTex );
					middleLilyPad->initialize( middle["lilyPad"] );
					mLilyPads[middleIndex] = middleLilyPad;
				}
				catch ( const JsonTree::ExcChildNotFound &ex ) {
					CI_LOG_E(ex.what());
				}
			}
			catch( const JsonTree::ExcChildNotFound &ex ) {
				CI_LOG_E(ex.what());
			}
			
			try {
				auto bottom = infoDisplays["bottom"];
				
				try {
					auto bottomDisplay = InfoDisplay::create( KioskId::TOP_KIOSK );
					bottomDisplay->initiaize( bottom );
					bottomDisplay->setPresentFbo( globalFbo );
					
					mDisplays[bottomIndex] = bottomDisplay;
				}
				catch ( const JsonTree::ExcChildNotFound &ex ) {
					CI_LOG_E(ex.what());
				}
				
				try {
					auto bottomLilyPad = LilyPad::create( KioskId::MIDDLE_KIOSK, lightTex, darkTex );
					bottomLilyPad->initialize( bottom["lilyPad"] );
					mLilyPads[bottomIndex] = bottomLilyPad;
				}
				catch ( const JsonTree::ExcChildNotFound &ex ) {
					CI_LOG_E(ex.what());
				}
			}
			catch( const JsonTree::ExcChildNotFound &ex ) {
				CI_LOG_E(ex.what());
			}
			
			auto svgPages = svgManager->getPages();
			
			for( auto & svgPage : svgPages ) {
				auto currentPage = svgPage.second;
				if( currentPage->getType() == DataPage::TYPE ) {
					auto dataPage = std::dynamic_pointer_cast<DataPage>(currentPage);
					auto nextDataPage = dataPage->next();
				}
			}
		}
		catch ( const JsonTree::ExcChildNotFound &ex ) {
			CI_LOG_E(ex.what());
		}
	}
	catch( const JsonTree::ExcChildNotFound &ex ) {
		CI_LOG_E(ex.what());
	}
	
}
	
}