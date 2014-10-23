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

#include "cinder/Log.h"

namespace heartbeat {
	
KioskManager::KioskManager()
{
	auto eventManager = EventManagerBase::get();
	if( eventManager ) {
		eventManager->addListener( EventListenerDelegate( this, &KioskManager::approachDelegate ), ApproachEvent::TYPE );
		eventManager->addListener( EventListenerDelegate( this, &KioskManager::touchDelegate ), TableEvent::TYPE );
		CI_LOG_V("KioskManager has registered it's event listeners");
	}
	else
		CI_LOG_W("KioskManager has NOT registered it's event listeners");
	
}
	
KioskManager::~KioskManager()
{
	for( auto & lock : mLocks ) {
		lock.wait();
	}
	
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
	
	// Implement this.
}
	
void KioskManager::touchDelegate( EventDataRef touchEvent )
{
	auto event = std::dynamic_pointer_cast<TableEvent>( touchEvent );
	
	if( ! event ) {
		CI_LOG_E("Not an ApproachEvent " << touchEvent->getName() );
		return;
	}
	
	// Implement this.
}
	
void KioskManager::update()
{
	uint32_t lock = 0;
//	for( auto & kiosk : mKiosks ) {
//		mLocks[lock++] = std::async( std::launch::async, std::bind( &Kiosk::update, kiosk ) );
//	}
}
	
void KioskManager::render()
{
	for( auto & lock : mLocks ) {
		lock.wait();
	}
	
//	for( auto & kiosk : mKiosks ) {
//		kiosk->draw();
//	}
}
	
void KioskManager::initialize()
{
	
}
	
}