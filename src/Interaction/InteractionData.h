//
//  InteractionData.h
//  FullProject
//
//  Created by Ryan Bartley on 11/9/14.
//
//

#pragma once

#include "Common.h"

namespace heartbeat {
	
struct Interactor {
	Interactor( int index, long distance )
	: mDistance( distance ), mIndex( index ), mNumIndicesPast( 0 )
	{}
	
	int			mNumIndicesPast;
	int			mIndex;
	long		mDistance;
};
	

	
struct TouchData {
public:
	enum class EventTypeToEmit {
		BEGAN,
		MOVED,
		ENDED,
		NONE
	};
	
	TouchData( int index, long distance );
	inline bool contains( int index, long distance );
	inline void update( int index, long distance );
	inline bool reset();
	void		createAndSendEvent();
	
	inline bool existsThisFrame() { return mExistsThisFrame; }
	uint64_t getId() const { return mId; }
	
private:
	const uint64_t		mId;
	int					mCurrentIndex;
	long				mCurrentDistance;
	bool				mExistsThisFrame;
	EventTypeToEmit		mEmitType;
	
	static uint64_t sCurrentId;
	static uint64_t idGenerator()	{ return sCurrentId++; }
	
};
	
bool TouchData::contains( int index, long distance )
{
	if( (index > mCurrentIndex - 5 || index < mCurrentIndex + 5) &&
	   ( distance > mCurrentDistance - 40 || distance < mCurrentDistance + 40 ) ) {
		mExistsThisFrame = true;
		update( index, distance );
		return true;
	}
	else
		return false;
}
	
void TouchData::update( int index, long distance )
{
	if( index != mCurrentIndex || distance != mCurrentDistance ) {
		mCurrentIndex = index;
		mCurrentDistance = distance;
		mEmitType = EventTypeToEmit::MOVED;
	}
	else {
		mEmitType = EventTypeToEmit::NONE;
	}
}
	
bool TouchData::reset()
{
	if( mExistsThisFrame ) {
		mExistsThisFrame = false;
		return true;
	}
	else {
		mEmitType = EventTypeToEmit::ENDED;
		return false;
	}
}
	
struct ApproachData {
public:
	
	ApproachData( KioskId kiosk, int lowestIndex, int highestIndex )
	: mKiosk( kiosk ), mLowestIndex( lowestIndex ), mHighestIndex( highestIndex ),
	mIsActivated( false ), mNumEvents( 0 ), mActivationTime( 0.0 )
	{}
	
	enum class EventTypeToEmit {
		APPROACH,
		DEPART,
		NONE
	};
	
	inline bool contains( int index ) { return mLowestIndex < index && mHighestIndex > index; }
	inline void addEvent() { mNumEvents++; }
	inline bool	getIsActivated() const { return mIsActivated; }
	inline void	activate( bool enable );
	inline size_t	getNumDistances() const { return mNumEvents; }
	inline KioskId getKiosk() const { return mKiosk; }
	inline void	reset() { mNumEvents = 0; mEmitType = EventTypeToEmit::NONE; }
	inline const int getLowest() const { return mLowestIndex; }
	inline const int getHighest() const { return mHighestIndex; }
	void createAndSendEvent();
	
private:
	const KioskId		mKiosk;
	const int			mLowestIndex, mHighestIndex;
	int					mNumEvents;
	bool				mIsActivated;
	double				mActivationTime;
	EventTypeToEmit		mEmitType;
};
	
void ApproachData::activate( bool enable )
{
	mIsActivated = enable;
	if( mIsActivated ) {
		mEmitType = EventTypeToEmit::APPROACH;
		createAndSendEvent();
	}
	else {
		mEmitType = EventTypeToEmit::DEPART;
		createAndSendEvent();
	}
}
	
	
}
