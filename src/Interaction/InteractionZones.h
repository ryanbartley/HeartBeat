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
	mIsActivated( false ), mNumEvents( 0 ), mActivationTime( 0.0 )
	{}
	
	inline bool contains( int index ) { return mLowestIndex < index && mHighestIndex > index; }
	inline void addEvent() { mNumEvents++; }
	bool	getIsActivated() const { return mIsActivated; }
	void	activate( bool enable );
	size_t	getNumDistances() const { return mNumEvents; }
	KioskId getKiosk() const { return mKiosk; }
	void	reset() { mNumEvents = 0; }
	inline const int getLowest() const { return mLowestIndex; }
	inline const int getHighest() const { return mHighestIndex; }
	
	const KioskId		mKiosk;
	const int			mLowestIndex, mHighestIndex;
	int					mNumEvents;
	bool				mIsActivated;
	double				mActivationTime;
};
	
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
	
	//! Returns 
	
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
    inline ci::quat& getFlip() { return mFlip; }
	
	inline std::map<KioskId, ApproachData>& getApproachZoneData() { return mApproachZones; }
	
	inline void queryIgnoreIndices() { mIgnoreIndices.clear(); mSendEvents = false; }
	
	const std::vector<uint32_t>& getIgnoreIndices() const { return mIgnoreIndices; }
	
	//! Sets the current scalar of \a zone.
	void setZoneScalar( Zone zone, float scalar );
	
private:
	InteractionZones();
	
	//! This is a possible async implementation.
	void processImpl( std::vector<long> &&newData );
	
	
	
	inline void processApproaches( const std::vector<Interactor> &events );
	
	inline void addEvent( std::vector<Interactor> &events, int index, long dist );
	
	//! Normalized scalar to be applied to the barrier
	//! to figure out which zone the interaction is in,
	//! and the number of frames currently in that zone.
	std::map<Zone, float>		mZones;
	//! Static Barrier of the object in question
	std::vector<long>			mBarrier;
	
	std::map<KioskId, ApproachData>		mApproachZones;
	std::vector<uint32_t>				mIgnoreIndices;
	
	int							mInBetweenThreshold;
	bool						mZoneScalarsUpdated;
	bool						mSendEvents;
	
	//! Pointer to the urg
	UrgRef						mUrg;
	
	std::function<void()>		mZoneUpdateFunc;
	
	Transformation				mTransform;
    ci::quat                    mFlip;
	
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
