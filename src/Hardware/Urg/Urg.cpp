//
//  Urg.cpp
//  LibUrgTest
//
//  Created by Ryan Bartley on 10/7/14.
//
//

#include "Urg.h"
#include "JsonManager.h"


#include "cinder/Serial.h"
#include "cinder/Log.h"

#include "urg_errno.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace heartbeat {
	
const long CONNECT_BAUDRATE = 115200;
	
Urg::Urg()
: mDeviceName("/dev/"), mIsOpen( false ), mIsMeasurmentStarted( false )
{
}
	
Urg::~Urg()
{
	close();
}
	
UrgRef Urg::create()
{
	return UrgRef( new Urg() );
}
	
bool Urg::open()
{
	if ( mIsOpen ) {
		close();
	}
	
	int errorVal = 0;
	
	errorVal = urg_open( &mSensor, URG_SERIAL, mDeviceName.c_str(), CONNECT_BAUDRATE );
	
	// If errorVal is less than
	if( errorVal < 0 ) {
		CI_LOG_E( urg_error(&mSensor) );
        app::App::get()->quit();
		return false;
	}
	
	// It's open
	mIsOpen = true;
	
	// Allocate the data array
	mSensorDataSize = urg_max_data_size( &mSensor );
	
	if( mSensorDataSize > 1081 ) {
		CI_LOG_E("Sensor Data Size is larger than 1081, actual number: " << mSensorDataSize);
        app::App::get()->quit();
		return false;
	}
	
	return true;
}
	
void Urg::close()
{
	urg_close( &mSensor );
	mIsOpen = false;
}
	
void Urg::startMeasurement( int numTimes )
{
	int errorVal = 0;
	
	errorVal = urg_start_measurement( &mSensor, URG_DISTANCE, numTimes, 0 );
	
	mIsMeasurmentStarted = true;
	
	if( errorVal < 0 ) {
		CI_LOG_E( urg_error(&mSensor) );
	}
}

void Urg::getDistance( std::vector<long> &data, long *timestamp )
{
	int amountOfDistanceData = urg_get_distance( &mSensor, data.data(), timestamp );
	
	mIsMeasurmentStarted = false;
	
	if( amountOfDistanceData != mSensorDataSize ) {
		CI_LOG_W("Didn't receive as much data as specified" << amountOfDistanceData );
	}
}
	
bool Urg::initialize()
{
	try {
		auto & urgAttribs = JsonManager::get()->getRoot()["urg"];
		
		try {
			auto urgAddress = urgAttribs["address"].getValue();
			
			auto devices = Serial::getDevices();
			bool found = false;
			for( auto & device : devices ) {
				auto & name = device.getName();
				if( urgAddress == name ) {
					found = true;
				}
				// If the devices name contains this fill out the rest of the
				// address with it.
				CI_LOG_I("\t" << name);
			}
		
			if( found ) {
				mDeviceName += urgAddress;
			}
			else {
				auto deviceNameContains = Serial::findDeviceByNameContains( "tty.usbmodem" );
				if( ! deviceNameContains.getName().empty() ) {
					mDeviceName += deviceNameContains.getName();
				}
			}
			
			CI_LOG_V("Device Name Found: " << mDeviceName );
		}
		catch ( JsonTree::ExcChildNotFound &ex ) {
			CI_LOG_W("Device address not found, finding first usb address");
			// Query the available serial Devices
			auto devices = Serial::getDevices();
			CI_LOG_I("Available Devices: ");
			for( auto & device : devices ) {
				auto & name = device.getName();
				// If the devices name contains this fill out the rest of the
				// address with it.
				CI_LOG_I("\t" << name);
			}
			return false;
		}
		
		try {
			auto proj1Scales = urgAttribs["proj1"]["scale"].getChildren();
			ci::vec2 proj1Scale;
			int i = 0;
			for( auto & scaleDim : proj1Scales ) {
				proj1Scale[i++] = scaleDim.getValue<float>();
			}
			
			mOffsetScales[PROJ_1_SCALE] = proj1Scale;
			
			auto proj1Offsets = urgAttribs["proj1"]["offset"].getChildren();
			ci::vec2 proj1Offset;
			i = 0;
			for( auto & offsetDim : proj1Offsets ) {
				proj1Offset[i++] = offsetDim.getValue<float>();
			}
			
			mOffsetScales[PROJ_1_OFFSET] = proj1Offset;
			
			auto proj2Scales = urgAttribs["proj2"]["scale"].getChildren();
			ci::vec2 proj2Scale;
			i = 0;
			for( auto & scaleDim : proj2Scales ) {
				proj2Scale[i++] = scaleDim.getValue<float>();
			}
			
			mOffsetScales[PROJ_2_SCALE] = proj1Scale;
			
			auto proj2Offsets = urgAttribs["proj2"]["offset"].getChildren();
			ci::vec2 proj2Offset;
			i = 0;
			for( auto & offsetDim : proj2Offsets ) {
				proj2Offset[i++] = offsetDim.getValue<float>();
			}
			
			mOffsetScales[PROJ_2_OFFSET] = proj2Offset;
		}
		catch( const JsonTree::ExcChildNotFound &ex ) {
			CI_LOG_E(ex.what());
			mOffsetScales[PROJ_1_OFFSET] = vec2( 0.0f );
			mOffsetScales[PROJ_1_SCALE] = vec2( 1.0f );
			mOffsetScales[PROJ_2_OFFSET] = vec2( 0.0f );
			mOffsetScales[PROJ_2_SCALE] = vec2( 1.0f );
		}
		
		return true;
	}
	catch( JsonTree::ExcChildNotFound &ex ) {
		CI_LOG_E(ex.what());
		return false;
	}
}
	
void Urg::readInto( std::vector<long> &writeableData )
{
	if( mCurrentData.empty() ) {
		writeableData = readOnce();
	}
	else {
		writeableData = std::move(mCurrentData);
	}
}
	
}