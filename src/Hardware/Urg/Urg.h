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
	
	enum ScaleOffsets {
		PROJ_1_OFFSET = 0,
		PROJ_1_SCALE = 1,
		PROJ_2_OFFSET = 2,
		PROJ_2_SCALE = 3
	};
	
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
	
	inline ci::vec2& getProj1Offset() { return mOffsetScales[PROJ_1_OFFSET]; }
	inline ci::vec2& getProj1Scale() { return mOffsetScales[PROJ_1_SCALE]; }
	inline ci::vec2& getProj2Offset() { return mOffsetScales[PROJ_2_OFFSET]; }
	inline ci::vec2& getProj2Scale() { return mOffsetScales[PROJ_2_SCALE]; }
	
	inline void setProj1OffsetX( float x ) { mOffsetScales[PROJ_1_OFFSET].x = x; }
	inline void setProj1OffsetY( float y ) { mOffsetScales[PROJ_1_OFFSET].y = y; }
	inline void setProj2OffsetX( float x ) { mOffsetScales[PROJ_2_OFFSET].x = x; }
	inline void setProj2OffsetY( float y ) { mOffsetScales[PROJ_2_OFFSET].y = y; }
	inline void setProj1ScaleX( float x )  { mOffsetScales[PROJ_1_SCALE].x = x; }
	inline void setProj1ScaleY( float y )  { mOffsetScales[PROJ_1_SCALE].y = y; }
	inline void setProj2ScaleX( float x )  { mOffsetScales[PROJ_2_SCALE].x = x; }
	inline void setProj2ScaleY( float y )  { mOffsetScales[PROJ_2_SCALE].y = y; }
	
	inline void offsetAndScaleIndexPoint( ci::vec2 &point, int index );
	
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
	
	int						mProjectorIndex;
	std::array<ci::vec2, 4> mOffsetScales;
	
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
	double radian;
	
	radian = urg_index2rad( &(mSensor), index );
	// \ Todo check length is valid
	
	ret.x = ((length * scalar) * sin (radian)) ;
	ret.y = ((length * scalar) * cos (radian));
	offsetAndScaleIndexPoint( ret, index );
	return ret;
}

void Urg::offsetAndScaleIndexPoint( ci::vec2 &point, int index )
{
	if( index <= mProjectorIndex ) {
		auto & scale = mOffsetScales[PROJ_1_SCALE];
		auto & offset = mOffsetScales[PROJ_1_OFFSET];
		point = point + offset * scale;
	}
	else {
		auto & scale = mOffsetScales[PROJ_2_SCALE];
		auto & offset = mOffsetScales[PROJ_2_OFFSET];
		point = point + offset * scale;
	}
}

}