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

#include <deque>

namespace heartbeat {
	
struct Interactor {
	Interactor( int index, long distance )
	: mDistance( distance ), mIndex( index )
	{}
	
	int			mIndex;
	long		mDistance;
};
	
struct ApproachData {
	
	ApproachData( KioskId kiosk, int lowestIndex, int highestIndex )
	: mKiosk( kiosk ), mLowestIndex( lowestIndex ), mHighestIndex( highestIndex ),
	mIsActivated( false ), mNumEvents( 0 )
	{}
	
	inline bool contains( int index ) { return mLowestIndex < index && mHighestIndex > index; }
	inline void addEvent() { mNumEvents++; }
	bool	getIsActivated() const { return mIsActivated; }
	void	activate( bool enable ) { mIsActivated = enable; }
	size_t	getNumDistances() const { return mNumEvents; }
	KioskId getKiosk() const { return mKiosk; }
	void	reset() { mNumEvents = 0; }
	inline const int getLowest() const { return mLowestIndex; }
	inline const int getHighest() const { return mHighestIndex; }
	
	const KioskId		mKiosk;
	const int			mLowestIndex, mHighestIndex;
	int					mNumEvents;
	bool				mIsActivated;
	
};
	
class InteractionZones {
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
	void process();
	
	//! Returns the current scalar being used for this zone
	float getZoneScalar( Zone zone );
	
	//! Initialize moved to public for use in the TestApp, shouldn't be
	//! called normally. Engine should handle the initialization.
	void initialize();
	
	//! This allows data to be captured into barrier
	void captureBarrier();
	
	inline void setZoneUpdateCallback( const std::function<void()> &function ) { mZoneUpdateFunc = function; }
	
	inline const Transformation& getTransform() { return mTransform; }
	
private:
	InteractionZones();
	
	//! This is a possible async implementation.
	void processImpl( std::vector<long> &&newData );
	
	//! Sets the current scalar of \a zone.
	void setZoneScalar( Zone zone, float scalar );
	
	inline void processApproaches( const std::vector<Interactor> &events );
	
	inline void addEvent( std::vector<Interactor> &events, int index, long dist );
	
	//! Normalized scalar to be applied to the barrier
	//! to figure out which zone the interaction is in,
	//! and the number of frames currently in that zone.
	std::map<Zone, float>		mZones;
	//! Static Barrier of the object in question
	std::vector<long>			mBarrier;
	
	std::map<KioskId, ApproachData>		mApproachZones;
	std::vector<uint32_t>				mPoleIndices;
	
	int							mInBetweenThreshold;
	bool						mZoneScalarsUpdated;
	
	//! Pointer to the urg
	UrgRef						mUrg;
	
	std::function<void()>		mZoneUpdateFunc;
	
	Transformation				mTransform;
	
	friend class InteractionManager;
};
	
void InteractionZones::addEvent( std::vector<Interactor> &events, int index, long dist )
{
	if( events.empty() )
		events.emplace_back( index, dist );
	else {
		auto & back = events.back();
		// if we're one more and our distance is less than, this is a more precise
		// middle point
		if( index == back.mIndex + 1 ) {
			if(  dist <= back.mDistance ) {
				back.mIndex = index;
				back.mDistance = dist;
			}
		}
		// otherwise if there's some space between this index and the last then
		// it's another object
		else if( index > back.mIndex + mInBetweenThreshold ){
			events.emplace_back( index, dist );
		}
	}
}
	
void InteractionZones::processApproaches( const std::vector<Interactor> &events )
{
	for( auto & event : events ) {
		for( auto & approachZone : mApproachZones ) {
			if( approachZone.second.contains( event.mIndex ) ) {
				approachZone.second.addEvent();
			}
		}
	}
}
	
}
