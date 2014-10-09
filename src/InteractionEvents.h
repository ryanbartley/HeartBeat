//
//  InteractionEvents.h
//  LibUrgTest
//
//  Created by Ryan Bartley on 10/8/14.
//
//

#pragma once

#include "BaseEventData.h"

namespace heartbeat {
	
class ApproachEvent : public EventData {
public:
	
	static const EventType TYPE;
	
	explicit ApproachEvent( int index, float dist )
	: mIndex( index ), mDist( dist )
	{}
	~ApproachEvent() {}
	
	//! Returns the index of the approaching entity
	int getIndex() { return mIndex; }
	float getDistance() { return mDist; }
	
	EventDataRef copy() { return EventDataRef( new ApproachEvent( mIndex, mDist ) ); }
	const char* getName() { return "ApproachEvent";}
	virtual const EventType& getEventType() { return TYPE; }
	
#if defined( DEBUG )
	virtual void serialize( std::ostrstream &streamOut ){ streamOut << mIndex << mDist;}
	virtual void deSerialize( std::istrstream& streamIn ){ streamIn >> mIndex; streamIn >> mDist; }
#else
	// TODO: This is probably stupid, need to figure out what I actually want.
	virtual void serialize( ci::Buffer &streamOut ){;}
	virtual void deSerialize( ci::Buffer &streamIn ){;}
#endif
	
private:
	int		mIndex;
	float	mDist;
};

class TableEvent : public EventData {
public:
	
	static const EventType TYPE;
	
	explicit TableEvent( int index, float dist )
	: mIndex( index ), mDist( dist )
	{}
	~TableEvent() {}
	
	int getIndex() { return mIndex; }
	float getDistance() { return mDist; }
	
	EventDataRef copy() { return EventDataRef( new ApproachEvent( mIndex, mDist ) ); }
	const char* getName() { return "ApproachEvent";}
	virtual const EventType& getEventType() { return TYPE; }
	
#if defined( DEBUG )
	virtual void serialize( std::ostrstream &streamOut ){ streamOut << mIndex << mDist;}
	virtual void deSerialize( std::istrstream& streamIn ){ streamIn >> mIndex; streamIn >> mDist; }
#else
	// TODO: This is probably stupid, need to figure out what I actually want.
	virtual void serialize( ci::Buffer &streamOut ){;}
	virtual void deSerialize( ci::Buffer &streamIn ){;}
#endif
	
private:
	int		mIndex;
	float	mDist;
};
	
}
