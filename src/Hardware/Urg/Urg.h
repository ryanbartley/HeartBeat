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

#include "cinder/Log.h"

#include "Common.h"

namespace heartbeat {

class Urg {
public:
	
	static UrgRef create();
	
	~Urg();
	
	//! Opens the Urg using mDeviceName. Returns true if opened, false if something went wrong.
	bool open();
	//! Closes the Urg.
	void close();
	
	//! Reads in one full revolution of data from the laser.
	inline std::vector<long> readOnce();
	inline void readMultiple( int numTimes );
	void readInto( std::vector<long> &outData );
	
	void start();
	void stop();
	
	void readIntoThread( std::promise<std::vector<long>> &&promise, int numTimes );
	
	//! Returns a boolean of whether the connection is open.
	bool isOpen() { return mIsOpen; }
	//! Returns the name of the device.
	const std::string& getName() { return mDeviceName; }
	
	inline double getRadian( int index ) { return urg_index2deg( &(mSensor), index); }
	
	inline ci::vec2 getPoint( int index, long dist, float scale = 1.0f );
	
	inline void processAverages( int numTimes );
	
	void asyncReadMany();
	
private:
	Urg();
	
	//! Loads settings from JsonManager.
	bool initialize();
	
	void startMeasurement( int numTimes );
	void getDistance( std::vector<long> &data, long * timestamp = nullptr );
	
	urg_t				mSensor;
	std::string			mDeviceName;
	std::vector<long>	mCurrentData;
	uint32_t			mSensorDataSize;
	bool				mIsOpen;
	bool				mIsMeasurmentStarted;
	
	friend class InteractionZones;
};
	
std::vector<long> Urg::readOnce()
{
	if( !mIsOpen ) return std::vector<long>();
	std::vector<long> ret( mSensorDataSize );
	if( ! mIsMeasurmentStarted ) {
		startMeasurement( 1 );
	}
	getDistance( ret );
	return ret;
}
	
ci::vec2 Urg::getPoint( int index, long length, float scalar )
{
	ci::vec2 ret;
	double x;
	double y;
	double radian;
	
	radian = urg_index2rad( &(mSensor), index );
	// \ Todo check length is valid
	
	x = ((length * scalar) * sin (radian)) ;
	y = ((length * scalar) * cos (radian));
	ret = ci::vec2( x, y );
	return ret;
}
	

}