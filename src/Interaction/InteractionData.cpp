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
	mEmitType( EventTypeToEmit::BEGAN ),
	mExistsThisFrame( true )
{
}
	
void TouchData::createAndSendEvent()
{
    if ( mCurrentDistance < 0 ) {
        return;
    }
    
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
mIsActivated( false ), mInteractionZone( interZones )
{
	auto shared = mInteractionZone.lock();
	float approach = 1.4f, dead = 1.1f ;
	if( shared ) {
	 approach = shared->getZoneScalar( InteractionZones::Zone::APPROACH );
	 dead = shared->getZoneScalar( InteractionZones::Zone::DEAD );
	}
	float mid = (approach + dead) / 2.0f;
	float total = approach - dead;
	mDepartThresh = mid + .2 * total;
    CI_LOG_V("approach: " << approach << " dead: " << dead << " mid: " << mid);
	mApproachThresh = mid - .2 * total;
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
            mIsActivated = true;
		}
		break;
		case EventTypeToEmit::DEPART: {
			event.reset( new DepartEvent( mKiosk ) );
            CI_LOG_I("emitting departEvent");
            mIsActivated = false;
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
    if( mInteractors.empty() && ! mIsActivated ) return;
    
    bool emitEvent = false;
    int numInApproach = 0;
    int numInDepart = 0;
 	auto shared = mInteractionZone.lock();
    for( auto & interactor : mInteractors ) {
        auto barrierDistance = shared->getBarrierAtIndex( interactor.mIndex );
        if (!mIsActivated) {
            if ( interactor.mDistance < (barrierDistance * mApproachThresh)) {
                numInApproach++;
            }
        } else {
            if ( interactor.mDistance < (barrierDistance * mDepartThresh)) {
                numInApproach++;
            }
        }
    }
    
    if( numInApproach > 0 && ! mIsActivated ) {
        mEmitType = EventTypeToEmit::APPROACH;
        emitEvent = true;
    }
    else if( (numInApproach == 0) && mIsActivated ) {
        mEmitType = EventTypeToEmit::DEPART;
        emitEvent = true;
    }
	
    if( emitEvent ) {
        createAndSendEvent();
    }
}
	


}