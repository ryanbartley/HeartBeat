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

#include "cinder/gl/Fbo.h"
#include "cinder/Log.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace heartbeat {
	
KioskManager::KioskManager()
{
	auto eventManager = EventManagerBase::get();
	if( eventManager ) {
		eventManager->addListener( std::bind( &KioskManager::approachDelegate, this, std::placeholders::_1 ), ApproachEvent::TYPE );
		eventManager->addListener( std::bind( &KioskManager::departDelegate, this, std::placeholders::_1 ), DepartEvent::TYPE );
		eventManager->addListener( std::bind( &KioskManager::touchDelegate, this, std::placeholders::_1 ), TouchEvent::TYPE );
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
	
	CI_LOG_V("Got a approachEvent for " << getKiosk( kioskId ) );
	
	mHidCommManager->activate( kioskId, true );
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
	
	CI_LOG_V("Got a departEvent for " << static_cast<int>( kioskId ) );
	
	mHidCommManager->activate( kioskId, false );
	mDisplays[kioskIndex]->activate( false );
}
	
void KioskManager::touchDelegate( EventDataRef touchEvent )
{
	auto event = std::dynamic_pointer_cast<TouchEvent>( touchEvent );
	
	if( ! event ) {
		CI_LOG_V("Not an touchEvent " << touchEvent->getName() );
		return;
	}
	
	for( auto & kiosk : mDisplays ) {
		if( kiosk->insideAngle( event->getIndex() ) ) {
			kiosk->registerTouch( event );
		}
	}
}
	
void KioskManager::update()
{
	for( auto & kiosk : mDisplays ) {
		kiosk->update();
	}
}
	
void KioskManager::render()
{
	
	for( auto & kiosk : mDisplays ) {
		kiosk->draw();
	}
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
			
			gl::Fbo::Format format;
			format.colorTexture( gl::Texture2d::Format()
								.mipmap()
								.maxAnisotropy( gl::Texture2d::getMaxMaxAnisotropy() )
								.minFilter( GL_LINEAR_MIPMAP_LINEAR ) );
			
			auto size = svgManager->getDoc()->getSize();
			cout << "Fbo size: " << size << endl;
			auto globalFbo = gl::Fbo::create( size.x, size.y, format );
			
			try {
				auto top = infoDisplays["top"];
				
				auto topDisplay = InfoDisplay::create( KioskId::TOP_KIOSK );
				topDisplay->initiaize( top );
				topDisplay->setPresentFbo( globalFbo );
				
				mDisplays[topIndex] = topDisplay;
			}
			catch ( const JsonTree::ExcChildNotFound &ex ) {
				CI_LOG_E(ex.what());
			}
			
			try {
				auto middle = infoDisplays["middle"];
				
				auto middleDisplay = InfoDisplay::create( KioskId::TOP_KIOSK );
				middleDisplay->initiaize( middle );
				middleDisplay->setPresentFbo( globalFbo );
				
				mDisplays[middleIndex] = middleDisplay;
			}
			catch ( const JsonTree::ExcChildNotFound &ex ) {
				CI_LOG_E(ex.what());
			}
			
			try {
				auto bottom = infoDisplays["bottom"];
				
				auto bottomDisplay = InfoDisplay::create( KioskId::TOP_KIOSK );
				bottomDisplay->initiaize( bottom );
				bottomDisplay->setPresentFbo( globalFbo );
				
				mDisplays[bottomIndex] = bottomDisplay;
			}
			catch ( const JsonTree::ExcChildNotFound &ex ) {
				CI_LOG_E(ex.what());
			}
			
			auto svgPages = svgManager->getPages();
			
			cout << "Checking SvgManager Pages: " << endl;
			for( auto & svgPage : svgPages ) {
				auto currentPage = svgPage.second;
				cout << "\t" << currentPage->getGroupName();
				if( currentPage->getType() == DataPage::TYPE ) {
					auto dataPage = std::dynamic_pointer_cast<DataPage>(currentPage);
					auto nextDataPage = dataPage->next();
					if( nextDataPage ) {
						cout << " -\t" << nextDataPage->getGroupName();
					}
				}
				cout << endl;
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