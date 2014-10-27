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

#include "cinder/Log.h"

namespace heartbeat {
	
KioskManager::KioskManager()
{
	auto eventManager = EventManagerBase::get();
	if( eventManager ) {
		eventManager->addListener( std::bind( &KioskManager::approachDelegate, this, std::placeholders::_1 ), ApproachEvent::TYPE );
		eventManager->addListener( std::bind( &KioskManager::departDelegate, this, std::placeholders::_1 ), DepartEvent::TYPE );
		eventManager->addListener( std::bind( &KioskManager::touchDelegate, this, std::placeholders::_1 ), TableEvent::TYPE );
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
	
	CI_LOG_V("Got a approachEvent for " << static_cast<int>(event->getKiosk()) );
	mHidCommManager->activate( event->getKiosk(), true );
	// Implement this.
}
	
void KioskManager::departDelegate( EventDataRef departEvent )
{
	auto event = std::dynamic_pointer_cast<DepartEvent>( departEvent );
	
	if( ! event ) {
		CI_LOG_E("Not a DepartEvent " << departEvent->getName() );
		return;
	}
	
	CI_LOG_V("Got a departEvent for " << static_cast<int>(event->getKiosk()) );
	mHidCommManager->activate( event->getKiosk(), false );
	// Implement this.
}
	
void KioskManager::touchDelegate( EventDataRef touchEvent )
{
	auto event = std::dynamic_pointer_cast<TableEvent>( touchEvent );
	
	if( ! event ) {
		CI_LOG_V("Not an touchEvent " << touchEvent->getName() );
		return;
	}
	
	// Implement this.
}
	
void KioskManager::update()
{
	
}
	
void KioskManager::render()
{
	
}
	
void KioskManager::initialize()
{
	mHidCommManager = HidCommManager::create();
	mHidCommManager->initialize();
}
	
}