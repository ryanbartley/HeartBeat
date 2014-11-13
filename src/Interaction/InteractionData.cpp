//
//  InteractionData.cpp
//  FullProject
//
//  Created by Ryan Bartley on 11/9/14.
//
//

#include "InteractionData.h"
#include "InteractionEvents.h"
#include "EventManager.h"

#include "cinder/Log.h"

namespace heartbeat {
	
uint64_t TouchData::sCurrentId = 0;
	
TouchData::TouchData( int index, long distance )
: mId( idGenerator() ), mCurrentDistance( distance ), mCurrentIndex( index ), mShouldEmit( false ),
	mNumUpdates( 0 ), mEmitType( EventTypeToEmit::BEGAN ), mExistsThisFrame( true )
{
}
	
void TouchData::createAndSendEvent()
{
	static auto eventManager = EventManagerBase::get();

	if( eventManager == nullptr ) {
		static bool notified = false;
		if( ! notified ) {
			CI_LOG_E("No event manager even though I've processed all of the data");
			notified = true;
		}
		return;
	}
	
	EventDataRef event;
	switch (mEmitType) {
		case EventTypeToEmit::BEGAN: {
			event.reset( new TouchBeganEvent( mId, mCurrentIndex, mCurrentDistance ) );
		}
		break;
		case EventTypeToEmit::MOVED: {
			event.reset( new TouchMoveEvent( mId, mCurrentIndex, mCurrentDistance ) );
		}
		break;
		case EventTypeToEmit::ENDED: {
			event.reset( new TouchEndedEvent( mId, mCurrentIndex, mCurrentDistance ) );
		}
		break;
		case EventTypeToEmit::NONE: {
		}
		break;
		default:
		break;
	}
	if( event ) {
		eventManager->queueEvent( event );
	}
	mEmitType = EventTypeToEmit::MOVED;
}

void ApproachData::createAndSendEvent()
{
	static auto eventManager = EventManagerBase::get();
	
	if( eventManager == nullptr ) {
		static bool notified = false;
		if( ! notified ) {
			CI_LOG_E("No event manager even though I've processed all of the data");
			notified = true;
		}
		return;
	}
	
	EventDataRef event;
	switch ( mEmitType ) {
		case EventTypeToEmit::APPROACH: {
			event.reset( new ApproachEvent( mKiosk ) );
		}
		break;
		case EventTypeToEmit::DEPART: {
			event.reset( new DepartEvent( mKiosk ) );
		}
		break;
		case EventTypeToEmit::NONE: {
		}
		break;
		default:
		break;
	}
	if( event ) {
		eventManager->queueEvent( event );
	}
	mEmitType = EventTypeToEmit::NONE;
}
	


}