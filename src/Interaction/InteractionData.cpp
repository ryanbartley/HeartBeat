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
#include "Engine.h"
#include "InteractionZones.h"

#include "cinder/Log.h"

namespace heartbeat {
	
uint64_t TouchData::sCurrentId = 0;
	
TouchData::TouchData( int index, long distance )
: mId( idGenerator() ), mCurrentIndex( index ), mCurrentDistance( distance ),
	mClosestDistance( distance ), mEmitType( EventTypeToEmit::BEGAN ),
	mExistsThisFrame( true )
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
}
	
ApproachData::ApproachData( InteractionZonesRef interZones, KioskId kiosk, int lowestIndex, int highestIndex )
: mKiosk( kiosk ), mLowestIndex( lowestIndex ), mHighestIndex( highestIndex ),
mIsActivated( false ), mNumEvents( 0 ), mInteractionZone( interZones ),
	mCurrentClosestDistance( 100000 ), mCurrentClosestIndex( 1082 )
{
	auto shared = mInteractionZone.lock();
	float approach = 1.4f, dead = 1.1f ;
	if( shared ) {
	 approach = shared->getZoneScalar( InteractionZones::Zone::APPROACH );
	 dead = shared->getZoneScalar( InteractionZones::Zone::DEAD );
	}
	float mid = (approach + dead) / 2.0f;
	float total = approach - dead;
	mDepartThresh = mid + .1 * total;
    CI_LOG_V("approach: " << approach << " dead: " << dead << " mid: " << mid);
	mApproachThresh = mid - .1 * total;
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
            CI_LOG_I("emitting approachEvent");
            
		}
		break;
		case EventTypeToEmit::DEPART: {
			event.reset( new DepartEvent( mKiosk ) );
            CI_LOG_I("emitting departEvent");
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
	
void ApproachData::checkDistanceForSend()
{
	auto shared = mInteractionZone.lock();
	auto barrierDistance = shared->getBarrierAtIndex( mCurrentClosestIndex );
	CI_LOG_I("barrierWithDepartThresh " << (barrierDistance * mDepartThresh) << " barrierWithApproachThresh " << (barrierDistance * mApproachThresh) << " CurrendtClosestDistance " << mCurrentClosestDistance << " activated " << mIsActivated );
	if( mIsActivated ) {
		if( mCurrentClosestDistance > barrierDistance * mDepartThresh ) {
			mEmitType = EventTypeToEmit::DEPART;
            mIsActivated = false;
            CI_LOG_I("I'm emitting depart");
		}
		else {
			mEmitType = EventTypeToEmit::NONE;
            CI_LOG_I("I'm emitting none");
		}
	}
	else {
		if ( mCurrentClosestDistance < barrierDistance * mApproachThresh ) {
			mEmitType = EventTypeToEmit::APPROACH;
            mIsActivated = true;
            CI_LOG_I("I'm emitting approach");
		}
		else {
			mEmitType = EventTypeToEmit::NONE;
            CI_LOG_I("I'm emitting none");
		}
	}
	
	createAndSendEvent();
}
	


}