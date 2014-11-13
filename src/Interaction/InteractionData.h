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
	: mCurrentDistance( distance ), mMinDistance( distance ), mMaxDistance( distance ),
		mIndex( index ), mNumIndicesPast( 0 )
	{}
	
	inline void checkMinMax( long distance )
	{
		mCurrentDistance = distance;
		if( distance < mMinDistance ) {
			mMinDistance = distance;
			mMinIndex = mIndex + mNumIndicesPast;
		}
		else if( distance > mMaxDistance ) {
			mMaxDistance = distance;
		}
	}
	
	inline void getCenterIndexDistance( int & index, long & distance )
	{
		index = (mIndex + (mIndex + mNumIndicesPast)) / 2.0f;
		distance = mMinDistance;
	}
	
	int			mNumIndicesPast;
	int			mIndex, mMinIndex;
	long		mCurrentDistance, mMinDistance, mMaxDistance;
};
	
const uint32_t NUM_UPDATES_TO_EMIT = 2;
	
struct TouchData {
public:
	enum class EventTypeToEmit {
		BEGAN,
		MOVED,
		ENDED,
		NONE
	};
	
	~TouchData() = default;
	
	TouchData( int index, long distance );
	inline bool contains( int index, long distance );
	inline void update( int index, long distance );
	inline bool reset();
	void		createAndSendEvent();
	
	inline bool existsThisFrame() { return mExistsThisFrame; }
	inline uint64_t getId() const { return mId; }
	
private:
	const uint64_t		mId;
	long				mCurrentDistance;
	int					mCurrentIndex, mNumUpdates;
	EventTypeToEmit		mEmitType;
	bool				mExistsThisFrame, mShouldEmit;
	
	static uint64_t sCurrentId;
	inline static uint64_t idGenerator()	{ return sCurrentId++; }
};
	
bool TouchData::contains( int index, long distance )
{
	if( ( index > mCurrentIndex - 16 && index < mCurrentIndex + 16 ) &&
	   ( distance > mCurrentDistance - 200 && distance < mCurrentDistance + 200 ) ) {
		mExistsThisFrame = true;
		update( index, distance );
		return true;
	}
	else
		return false;
}
	
void TouchData::update( int index, long distance )
{
	static const float scalar = 1.0f / 2.0f;
	if( index != mCurrentIndex || distance != mCurrentDistance ) {
		mCurrentIndex = (index + mCurrentIndex) * scalar;
		mCurrentDistance = (distance + mCurrentDistance) * scalar;
	}
	mNumUpdates++;
	if( mNumUpdates > 1 ) {
		mShouldEmit = true;
	}
}
	
bool TouchData::reset()
{
	if( mExistsThisFrame ) {
		if( mShouldEmit ) {
			mNumUpdates = 0;
			mShouldEmit = false;
		}
		mExistsThisFrame = false;
		return true;
	}
	else {
		mShouldEmit = true;
		mEmitType = EventTypeToEmit::ENDED;
		return false;
	}
}
	
struct ApproachData {
public:
	
	ApproachData( KioskId kiosk, int lowestIndex, int highestIndex )
	: mKiosk( kiosk ), mLowestIndex( lowestIndex ), mHighestIndex( highestIndex ),
	mIsActivated( false ), mNumEvents( 0 )
	{}
	~ApproachData() = default;
	
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
	void endFrame();
	
private:
	
	EventTypeToEmit		mEmitType;
	const KioskId		mKiosk;
	const int			mLowestIndex, mHighestIndex;
	int					mNumEvents;
	bool				mIsActivated;
	
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
