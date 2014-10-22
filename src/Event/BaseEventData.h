//
//  EventDataBase.h
//  RendererTest
//
//  Created by Ryan Bartley on 8/16/14.
//
//

#pragma once

#include "Common.h"
#include <strstream>

namespace heartbeat {
	
class EventData {
public:
	explicit EventData( float timestamp = 0.0f ) : mTimeStamp( timestamp ), mIsHandled( false ) {}
	virtual ~EventData() {}
	
	virtual EventDataRef copy() = 0;
	virtual const char* getName() = 0;
	virtual const EventType& getEventType() = 0;
	float getTimeStamp() { return mTimeStamp; }
	
	bool isHandled() { return mIsHandled; }
	void setIsHandled( bool handled = true ) { mIsHandled = handled; }
	
#if defined( DEBUG )
	virtual void serialize( std::ostrstream &streamOut ) = 0;
	virtual void deSerialize( std::istrstream& streamIn ) = 0;
#else
	// TODO: This is probably stupid, need to figure out what I actually want.
	virtual void serialize( ci::Buffer &streamOut ) = 0;
	virtual void deSerialize( ci::Buffer &streamIn ) = 0;
#endif
	
private:
	const float mTimeStamp;
	bool		mIsHandled;
};

	
}