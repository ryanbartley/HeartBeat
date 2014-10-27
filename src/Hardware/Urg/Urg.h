//
//  Urg.h
//  LibUrgTest
//
//  Created by Ryan Bartley on 10/7/14.
//
//

#pragma once

#include "urg_sensor.h"
#include "urg_utils.h"

#include "Common.h"

namespace heartbeat {
	
struct DistanceData {
	
	long mCurrentReading, mLastReading;
	bool mGettingCloser;
};

class Urg {
public:
	
	static UrgRef create();
	
	~Urg();
	
	//! Opens the Urg using mDeviceName. Returns true if opened, false if something went wrong.
	bool open();
	//! Closes the Urg.
	void close();
	
	//! Reads in one full revolution of data from the laser.
	void read();
	void readInto( std::vector<long> &outData );
	
	//! Returns a boolean of whether the connection is open.
	bool isOpen() { return mIsOpen; }
	//! Returns the name of the device.
	const std::string& getName() { return mDeviceName; }
	
	inline double getRadian( int index ) { return urg_index2deg( &(mSensor), index); }
	
	//! Returns a vector of points to draw from \a urg. These locations expect a 0,0 origin. You can return two types of points, TABLE_DATA, and DISTANCE_DATA.
	std::vector<ci::vec2> getDrawablePoints();
	std::vector<ci::vec2> getDrawablePoints( const std::vector<long> &data, float scalar = 1.0f );
	
	inline ci::vec2 getPoint( int index, long dist, float scale = 1.0f );
	
	const std::vector<long>& getCurrentData() { return mCurrentData; }
	
private:
	Urg();
	
	//! Loads settings from JsonManager.
	bool initialize();
	
	urg_t				mSensor;
	std::string			mDeviceName;
	std::vector<long>	mCurrentData;
	bool				mIsOpen;
	
	friend class InteractionZones;
};
	
ci::vec2 Urg::getPoint( int index, long length, float scalar )
{
	ci::vec2 ret;
	double x;
	double y;
	double radian;
	
	radian = urg_index2rad( &(mSensor), index );
	// \ Todo check length is valid
	
	x = (length * cos (radian)) * scalar;
	y = (length * sin (radian)) * scalar;
	ret = ci::vec2( x, y );
	return ret;
}

}