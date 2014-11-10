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
	
	explicit ApproachEvent( KioskId kioskId );
	~ApproachEvent() {}
	
	//! Returns the index of the approaching entity
	
	EventDataRef copy() { return EventDataRef( new ApproachEvent( mKiosk ) ); }
	const char* getName() { return "ApproachEvent";}
	virtual const EventType& getEventType() { return TYPE; }
	
	KioskId getKiosk() { return mKiosk; }
	
#if defined( DEBUG )
	virtual void serialize( std::ostrstream &streamOut ){ streamOut << static_cast<size_t>(mKiosk);}
	virtual void deSerialize( std::istrstream& streamIn ){ int num = -1; streamIn >> num; mKiosk = static_cast<KioskId>(num); }
#else
	// TODO: This is probably stupid, need to figure out what I actually want.
	virtual void serialize( ci::Buffer &streamOut ){;}
	virtual void deSerialize( ci::Buffer &streamIn ){;}
#endif
	
private:
	KioskId mKiosk;
};
	
class DepartEvent : public EventData {
public:
	
	static const EventType TYPE;
	
	explicit DepartEvent( KioskId kioskId );
	~DepartEvent() {}
	
	//! Returns the index of the approaching entity
	
	EventDataRef copy() { return EventDataRef( new DepartEvent( mKiosk ) ); }
	const char* getName() { return "DepartEvent";}
	virtual const EventType& getEventType() { return TYPE; }
	
	KioskId getKiosk() { return mKiosk; }
	
#if defined( DEBUG )
	virtual void serialize( std::ostrstream &streamOut ){ streamOut << static_cast<size_t>(mKiosk);}
	virtual void deSerialize( std::istrstream& streamIn ){ int num = -1; streamIn >> num; mKiosk = static_cast<KioskId>(num); }
#else
	// TODO: This is probably stupid, need to figure out what I actually want.
	virtual void serialize( ci::Buffer &streamOut ){;}
	virtual void deSerialize( ci::Buffer &streamIn ){;}
#endif
	
private:
	KioskId mKiosk;
};

class TouchEvent : public EventData {
public:
	
	static const EventType TYPE;
	
	explicit TouchEvent( uint64_t touchId, int index, float dist );
	explicit TouchEvent( uint64_t touchId, const ci::vec2 &point );
	~TouchEvent() {}
	
	int getIndex() { return mIndex; }
	float getDistance() { return mDist; }
	
	ci::vec2&	getWorldCoordinate();
	uint64_t	getTouchId() { return mTouchId; }
	
	virtual EventDataRef copy() { return EventDataRef( new TouchEvent( mTouchId, mIndex, mDist ) ); }
	virtual const char* getName() { return "TouchEvent";}
	virtual const EventType& getEventType() { return TYPE; }
	
#if defined( DEBUG )
	virtual void serialize( std::ostrstream &streamOut ){ streamOut << mIndex << mDist;}
	virtual void deSerialize( std::istrstream& streamIn ){ streamIn >> mIndex; streamIn >> mDist; }
#else
	// TODO: This is probably stupid, need to figure out what I actually want.
	virtual void serialize( ci::Buffer &streamOut ){;}
	virtual void deSerialize( ci::Buffer &streamIn ){;}
#endif
	
protected:
	void calcWorldCoord();
	
	ci::vec2		mWorldCoord;
	uint64_t		mTouchId;
	float			mDist;
	int				mIndex;
	bool			mCachedWorldCoord;
};
	
class TouchBeganEvent : public TouchEvent {
public:
	
	static const EventType TYPE;
	
	explicit TouchBeganEvent( uint64_t touchId, int index, float distance );
	explicit TouchBeganEvent( uint64_t touchId, const ci::vec2 &point );
	
	EventDataRef copy() { return EventDataRef( new TouchBeganEvent( mTouchId, mIndex, mDist ) ); }
	const char* getName() override { return "TouchBeganEvent"; }
	const EventType& getEventType() override { return TYPE; }
	
#if defined( DEBUG )
	void serialize( std::ostrstream &streamOut ) override { streamOut << mTouchId << mIndex << mDist; }
	void deSerialize( std::istrstream& streamIn ) override { streamIn >> mTouchId >> mIndex >> mDist; }
#else
	// TODO: This is probably stupid, need to figure out what I actually want.
	void serialize( ci::Buffer &streamOut ){;}
	void deSerialize( ci::Buffer &streamIn ){;}
#endif
	
private:
	
};
	
class TouchMoveEvent : public TouchEvent {
public:
	
	static const EventType TYPE;
	
	explicit TouchMoveEvent( uint64_t touchId, int index, float distance );
	explicit TouchMoveEvent( uint64_t touchId, const ci::vec2 &point );
	
	EventDataRef copy() { return EventDataRef( new TouchMoveEvent( mTouchId, mIndex, mDist ) ); }
	const char* getName() override { return "TouchMoveEvent"; }
	const EventType& getEventType() override { return TYPE; }
	
#if defined( DEBUG )
	void serialize( std::ostrstream &streamOut ) override { streamOut << mTouchId << mIndex << mDist; }
	void deSerialize( std::istrstream& streamIn ) override { streamIn >> mTouchId >> mIndex >> mDist; }
#else
	// TODO: This is probably stupid, need to figure out what I actually want.
	void serialize( ci::Buffer &streamOut ){;}
	void deSerialize( ci::Buffer &streamIn ){;}
#endif
		
private:
	
};
	
class TouchEndedEvent : public TouchEvent {
public:
	
	static const EventType TYPE;
	
	explicit TouchEndedEvent( uint64_t touchId, int index, float distance );
	explicit TouchEndedEvent( uint64_t touchId, const ci::vec2 &point );
	
	EventDataRef copy() { return EventDataRef( new TouchEndedEvent( mTouchId, mIndex, mDist ) ); }
	const char* getName() override { return "TouchEndedEvent"; }
	const EventType& getEventType() override { return TYPE; }
	
#if defined( DEBUG )
	void serialize( std::ostrstream &streamOut ) override { streamOut << mTouchId << mIndex << mDist; }
	void deSerialize( std::istrstream& streamIn ) override { streamIn >> mTouchId >> mIndex >> mDist; }
#else
	// TODO: This is probably stupid, need to figure out what I actually want.
	void serialize( ci::Buffer &streamOut ){;}
	void deSerialize( ci::Buffer &streamIn ){;}
#endif
	
private:
	
};
	
}
