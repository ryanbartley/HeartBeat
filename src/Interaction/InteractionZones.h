//
//  InteractionZones.h
//  LibUrgTest
//
//  Created by Ryan Bartley on 10/8/14.
//
//

#pragma once

#include "Common.h"
#include "Transformation.h"
#include "InteractionData.h"

#include <deque>

namespace heartbeat {
	
class InteractionZones : public std::enable_shared_from_this<InteractionZones> {
public:
	
	enum class Zone {
		TABLE = 0,
		DEAD = 1,
		APPROACH = 2,
		FAR = 3,
		NUM_INTERACTIONS = 4
	};
	
	static InteractionZonesRef create();
	
	//! I don't know about this yet.
	std::shared_future<void> process( std::vector<long> &&newData );
	
	//! returns a reference to the Barrier vector.
	const std::vector<long>& getBarrier() { return mBarrier; }
	long getBarrierAtIndex( int index ) { return mBarrier[index]; }
	//! Returns a reference to the zone and scalar map.
	const std::map<Zone, float>& getZones() { return mZones; }
	//! Returns true or false whether the zone scalar floats have been
	//! updated.
	bool getZoneScalarsUpdated() { return mZoneScalarsUpdated; }
	
	const UrgRef& getUrg() { return mUrg; }
	
	//! Writes the current urg data to a file. 
	bool writeInteractionZone();
	//! Serializes the current barrier and zone scalars to \a file.
	void serialize( ci::DataTargetPathRef &file );
	//! Populates the current barrier and zone scalars from \a file.
	void deserialize( const ci::DataSourceRef &file );
	
	//! This processes the urg data against the interaction data
	//! contained in barrier and zone.
	void processData();
	
	//! Returns the current scalar being used for this zone
	float getZoneScalar( Zone zone );
	
	
	//! Initialize moved to public for use in the TestApp, shouldn't be
	//! called normally. Engine should handle the initialization.
	void initialize();
	
	//! This allows data to be captured into barrier
	void captureBarrier();
	
	//! Used only by InteractionDebugRenderable to be used as a callback when settings inside
	//! have been updated like Zone Scalars for instance.
	inline void setZoneUpdateCallback( const std::function<void()> &function ) { mZoneUpdateFunc = function; }
	
	//! Returns the a const transformation reference used to transform urg points into modelSpace.
	inline const Transformation& getTransform() const { return mTransform; }
	//! Returns the a transformation reference used to transform urg points into modelSpace.
	inline Transformation& getTransform() { return mTransform; }
	
	inline std::map<KioskId, ApproachData>& getApproachZoneData() { return mApproachZones; }
	
	inline void queryIgnoreIndices() { mIgnoreIndices.clear(); mSendEvents = false; }
	
	const std::vector<uint32_t>& getIgnoreIndices() const { return mIgnoreIndices; }
	
	//! Sets the current scalar of \a zone.
	void setZoneScalar( Zone zone, float scalar );
	
	int getNumIndicesThreshApproach() const { return mNumIndicesThreshApproach; }
	int getNumIndicesThreshTouch() const { return mNumIndicesThreshTouches; }
	void setNumIndicesThreshApproach( float thresh ) { mNumIndicesThreshApproach = thresh; }
	void setNumIndicesThreshTouch( float thresh ) { mNumIndicesThreshTouches = thresh; }
	
	inline void processApproach( int index, long distance );
	inline void processTouch( int index, long distance );
	
	std::vector<ci::vec2> getDrawablePoints();
	
	inline void checkAverages( int index, long &distance );
	
private:
	InteractionZones();
	
	//! This is a possible async implementation.
	void processImpl( std::vector<long> &&newData );
	
	void preProcessData();
	void postProcessData();

	inline void processApproaches();
	inline void processTouches();
	
	inline void addEvent( std::vector<Interactor> &events, int index, long dist );
	
	//! Normalized scalar to be applied to the barrier
	//! to figure out which zone the interaction is in,
	//! and the number of frames currently in that zone.
	std::map<Zone, float>		mZones;
	//! Static Barrier of the object in question
	std::vector<long>			mBarrier;
	
	std::map<KioskId, ApproachData>		mApproachZones;
	std::list<TouchData>				mCurrentTouches;
	std::vector<uint32_t>				mIgnoreIndices;
	std::vector<Interactor>				mApproachInteractors, mTouchInteractors;
	
	std::thread						mThread;
	std::future<std::vector<long>>	mFuture;
	ci::Timer						mOverallTime;
	
	int							mInBetweenThreshold,
								mNumIndicesThreshTouches,
								mNumIndicesThreshApproach,
								mAveragesThresh;
	bool						mZoneScalarsUpdated;
	bool						mSendEvents;
	
	std::vector<long>			mAverageBuffer, mCurrentFrameData;
	
	//! Pointer to the urg
	UrgRef						mUrg;
	
	std::function<void()>		mZoneUpdateFunc;
	
	Transformation				mTransform;
	
	friend class Engine;
};
	
void InteractionZones::processApproach( int index, long distance )
{
	if( mSendEvents ) {
		// emit touching event.
		addEvent( mApproachInteractors, index, distance );
	}
	else {
//		auto found = std::find( mIgnoreIndices.begin(), mIgnoreIndices.end(), index );
//		if ( found == mIgnoreIndices.end() ) {
//			mIgnoreIndices.push_back( index );
//			std::sort( mIgnoreIndices.begin(), mIgnoreIndices.end(), []( int i, int j ) {
//				return i < j;
//			});
//		}
	}
}
	
void InteractionZones::processTouch( int index, long distance )
{
	if( mSendEvents ) {
		// emit touching event.
		addEvent( mTouchInteractors, index, distance );
	}
	else {
		auto found = std::find( mIgnoreIndices.begin(), mIgnoreIndices.end(), index );
		if ( found == mIgnoreIndices.end() ) {
			mIgnoreIndices.push_back( index );
			std::sort( mIgnoreIndices.begin(), mIgnoreIndices.end(), []( int i, int j ) {
				return i < j;
			});
		}
	}
}
	
void InteractionZones::checkAverages( int index, long &distance )
{
	if( (distance < mAverageBuffer[index] + 10) || (distance > mAverageBuffer[index] - 10) ) {
		distance = mAverageBuffer[index] = (mAverageBuffer[index] + distance) / 2;
	}
	else {
		mAverageBuffer[index] = distance;
	}
}
	
void InteractionZones::addEvent( std::vector<Interactor> &events, int index, long dist )
{
	if( events.empty() )
		events.emplace_back( index, dist );
	else {
		auto & back = events.back();
		// if we're one more and our distance is less than, this is a more precise
		// middle point
		if( index == back.mIndex + back.mNumIndicesPast + 1 ) {
			if(  dist < back.mCurrentDistance + 40 && dist > back.mCurrentDistance - 40 ) {
				back.mNumIndicesPast++;
				back.checkMinMax( dist );
			}
		}
		// otherwise if there's some space between this index and the last then
		// it's another object
		else {
			events.emplace_back( index, dist );
		}
	}
}
	
void InteractionZones::processApproaches()
{
	for( auto & event : mApproachInteractors ) {
		for( auto & approachZone : mApproachZones ) {
			if( approachZone.second.contains( event.mIndex ) ) {
				approachZone.second.addEvent( event.mMinIndex, event.mMinDistance );
			}
		}
	}
	
	for( auto & zone : mApproachZones ) {
		auto & approachZone = zone.second;
		auto activated = approachZone.getIsActivated();
		auto numDistances = approachZone.getNumDistances();
		if( activated && numDistances == 0 ) {
			approachZone.checkDistanceForSend();
		}
		else if( !activated && numDistances > 0 ) {
			approachZone.checkDistanceForSend();
		}
		approachZone.reset();
	}
	
	mApproachInteractors.clear();
}
	
void InteractionZones::processTouches()
{
	for( auto & touchIt : mTouchInteractors ) {
		int currentCenterIndex = 0;
		long currentCenterDistance = 0;
		touchIt.getCenterIndexDistance( currentCenterIndex, currentCenterDistance );
		bool touchHandled = false;
		bool inNonActiveArea = false;
		if( ! mCurrentTouches.empty() ) {
			for( auto & currentTouch : mCurrentTouches ) {
				if( currentTouch.contains( currentCenterIndex, currentCenterDistance ) ) {
					//&& touchIt.mNumIndicesPast < mNumIndicesThreshTouches ) {
					touchHandled = true;
					break;
				}
			}
			for( auto & approachZone : mApproachZones ) {
				if( approachZone.second.contains( currentCenterIndex ) ) {
					if( ! approachZone.second.getIsActivated() ) {
						inNonActiveArea = true;
					}
				}
			}
		}
		if( ! touchHandled && ! inNonActiveArea ) {
			mCurrentTouches.emplace_back( currentCenterIndex, currentCenterDistance );
		}
	}
	for( auto currentTouch = mCurrentTouches.begin(); currentTouch != mCurrentTouches.end(); ) {
		bool deleteCurrentTouch = false;
		if( ! currentTouch->reset() ) {
			deleteCurrentTouch = true;
		}
		
		currentTouch->createAndSendEvent();
		
		if( deleteCurrentTouch ) {
			currentTouch = mCurrentTouches.erase( currentTouch );
		}
		else {
			++currentTouch;
		}
	}
	
	mTouchInteractors.clear();
}
	
}
