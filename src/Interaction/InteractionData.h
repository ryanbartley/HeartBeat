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
	: mCurrentDistance( distance ), mMinDistance( distance ),
		mMaxDistance( distance ), mIndex( index ), mNumIndicesPast( 0 ),
        mMinIndex( index )
	{}
	
	inline void checkMinMax( long distance )
	{
		mCurrentDistance = distance;
		mDistances.push_back( distance );
		if( distance < mMinDistance ) {
			mMinDistance = distance;
			mMinIndex = mIndex + mNumIndicesPast;
		}
		else if( distance > mMaxDistance ) {
			mMaxDistance = distance;
		}
	}
	
	double average()
	{
		double accum = 0.0f;
		int numAccum = 0;
		for( auto & distance : mDistances ) {
			if( distance < mMinDistance + 50 ) {
				accum += distance;
				numAccum++;
			}
		}
		return accum / numAccum;
	}
	
	inline void getCenterIndexDistance( int & index, long & distance )
	{
		index = (mIndex + (mIndex + mNumIndicesPast)) / 2.0f;
		distance = average();
	}
	
	int			mNumIndicesPast;
	std::vector<long>	mDistances;
	int			mIndex, mMinIndex;
	long		mCurrentDistance, mMinDistance, mMaxDistance;
};
	
const uint32_t NUM_UPDATES_TO_EMIT = 2;
const uint32_t UPDATE_DISTANCE_THRESH = 300;
const uint32_t UPDATE_INDEX_THRESH = 16;
	
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
	long				mClosestDistance, mCurrentDistance;
	int					mCurrentIndex;
	EventTypeToEmit		mEmitType;
	bool				mExistsThisFrame;
	
	static uint64_t sCurrentId;
	inline static uint64_t idGenerator()	{ return sCurrentId++; }
};
	
bool TouchData::contains( int index, long distance )
{
	if( ( index > mCurrentIndex - UPDATE_INDEX_THRESH &&
		  index < mCurrentIndex + UPDATE_INDEX_THRESH ) &&
		( distance > mCurrentDistance - UPDATE_DISTANCE_THRESH &&
		  distance < mCurrentDistance + UPDATE_DISTANCE_THRESH ) )
	{
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
	if( distance < mCurrentDistance + 50 ) {
		mCurrentIndex = (index + mCurrentIndex) * scalar;
		mCurrentDistance = (distance + mCurrentDistance) * scalar;
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
	
	ApproachData( InteractionZonesRef interZones, KioskId kiosk, int lowestIndex, int highestIndex );
	~ApproachData() = default;
	
	enum class EventTypeToEmit {
		APPROACH,
		DEPART,
		NONE
	};
	
	inline bool contains( int index ) { return mLowestIndex < index && mHighestIndex > index; }
	inline void addEvent( int index, long distance )
	{
		if( distance < mCurrentClosestDistance ) {
			mCurrentClosestDistance = distance;
			mCurrentClosestIndex = index;
		}
		mNumEvents++;
	}
	inline bool	getIsActivated() const { return mIsActivated; }
	inline size_t	getNumDistances() const { return mNumEvents; }
	inline KioskId getKiosk() const { return mKiosk; }
	inline void	reset() { mNumEvents = 0; mEmitType = EventTypeToEmit::NONE; }
	inline const int getLowest() const { return mLowestIndex; }
	inline const int getHighest() const { return mHighestIndex; }
	void checkDistanceForSend();
	void createAndSendEvent();
	void endFrame();
	
private:
	
	std::weak_ptr<InteractionZones>	mInteractionZone;
	EventTypeToEmit		mEmitType;
	const KioskId		mKiosk;
	const int			mLowestIndex, mHighestIndex;
	long				mCurrentClosestDistance, mCurrentClosestIndex;
	float				mApproachThresh, mDepartThresh;
	int					mNumEvents;
	bool				mIsActivated;
	
};
	
}
