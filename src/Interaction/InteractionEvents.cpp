//
//  InteractionEvents.cpp
//  LibUrgTest
//
//  Created by Ryan Bartley on 10/8/14.
//
//

#include "InteractionEvents.h"

#include "EventManager.h"
#include "Urg.h"
#include "InteractionZones.h"
#include "cinder/Log.h"
#include "Engine.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace heartbeat {
	
const EventType ApproachEvent::TYPE = Hash::HASHER("ApproachEvent");
const EventType TouchEvent::TYPE = Hash::HASHER("TouchEvent");
const EventType DepartEvent::TYPE = Hash::HASHER("DepartEvent");
	
ApproachEvent::ApproachEvent( KioskId kioskId )
: EventData( ci::app::getElapsedSeconds() ), mKiosk( kioskId )
{
}

DepartEvent::DepartEvent( KioskId kioskId )
: EventData( ci::app::getElapsedSeconds() ), mKiosk( kioskId )
{
}
	
TouchEvent::TouchEvent( int index, float dist )
: EventData( ci::app::getElapsedSeconds() ), mIndex( index ),
	mDist( dist ), mCachedWorldCoord( false )
{
}
	
TouchEvent::TouchEvent( const ci::vec2 &point )
: mIndex( 100 ), mDist( 0 ), mWorldCoord( point ), mCachedWorldCoord( true )
{
}
	
ci::vec2& TouchEvent::getWorldCoordinate()
{
	if( ! mCachedWorldCoord ) {
		calcWorldCoord();
	}
	return mWorldCoord;
}
	
void TouchEvent::calcWorldCoord()
{
	static auto sInteractionZones = Engine::get()->getInteractionZones();
	static auto sUrg = sInteractionZones->getUrg();
	if( ! sUrg ) {
		CI_LOG_E("Setting world coord without a working urg");
		mWorldCoord = vec2( 0 );
		return;
	}
	
	auto temp = sInteractionZones->getTransform().getModelMatrix() * vec4( sUrg->getPoint( mIndex, mDist ), 0, 1 );
	cout << "temp: " << temp << " original point: " << sUrg->getPoint( mIndex, mDist ) << endl;
	mWorldCoord = vec2( temp.x, temp.y );
}
	
}