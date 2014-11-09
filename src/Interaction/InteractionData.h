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
	: mDistance( distance ), mIndex( index )
	{}
	
	int			mIndex;
	long		mDistance;
};
	
struct TouchData {
	
};
	
struct ApproachData {
	
	ApproachData( KioskId kiosk, int lowestIndex, int highestIndex )
	: mKiosk( kiosk ), mLowestIndex( lowestIndex ), mHighestIndex( highestIndex ),
	mIsActivated( false ), mNumEvents( 0 ), mActivationTime( 0.0 )
	{}
	
	inline bool contains( int index ) { return mLowestIndex < index && mHighestIndex > index; }
	inline void addEvent() { mNumEvents++; }
	bool	getIsActivated() const { return mIsActivated; }
	void	activate( bool enable ) { mActivationTime = enable; }
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
	
	
}
