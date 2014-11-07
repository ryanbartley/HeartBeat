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
: mDeviceName("/dev/"), mIsOpen( false )
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
		return false;
	}
	
	
	// It's open
	mIsOpen = true;
	
	// Allocate the data array
	auto sensorDataSize = urg_max_data_size( &mSensor );
	mCurrentData.resize( sensorDataSize );
	
	return true;
}
	
void Urg::close()
{
	urg_close( &mSensor );
	mIsOpen = false;
}
	
void Urg::read()
{
	if( !mIsOpen ) return;
	
	int errorVal = 0;
	
	errorVal = urg_start_measurement( &mSensor, URG_DISTANCE, 1, 0 );
	
	if( errorVal < 0 ) {
		CI_LOG_E( urg_error(&mSensor) );
	}
	
	int amountOfDistanceData = urg_get_distance( &mSensor, mCurrentData.data(), nullptr );
	
	if( amountOfDistanceData != mCurrentData.size() ) {
		CI_LOG_W("Didn't receive as much data as specified");
	}
}
	
bool Urg::initialize()
{
	try {
		auto & urgAttribs = JsonManager::get()->getRoot()["urg"];
		
		try {
			auto urgAddress = urgAttribs["address"].getValue();
			mDeviceName = urgAddress;
			
			auto devices = Serial::getDevices();  
			for( auto & device : devices ) {
				auto & name = device.getName();
				// If the devices name contains this fill out the rest of the
				// address with it.
				cout << "\t" << name << endl;
			}
			
			CI_LOG_V("Device Name Found: " << mDeviceName );
		}
		catch ( JsonTree::ExcChildNotFound &ex ) {
			CI_LOG_W("Device address not found, finding first usb address");
			// Query the available serial Devices
			auto devices = Serial::getDevices();
			cout << "Available Devices: " << endl;
			for( auto & device : devices ) {
				auto & name = device.getName();
				// If the devices name contains this fill out the rest of the
				// address with it.
				cout << "\t" << name << endl;
			}
			return false;
		}
		return true;
	}
	catch( JsonTree::ExcChildNotFound &ex ) {
		CI_LOG_E(ex.what());
		return false;
	}
}
	
std::vector<ci::vec2> Urg::getDrawablePoints()
{
	std::vector<ci::vec2> ret(mCurrentData.size());
	auto retIt = ret.begin();
	int i = 0;
	for( auto & length : mCurrentData ) {
		(*retIt++) = getPoint( i++, length );
	}

	return ret;
}
	
std::vector<ci::vec2> Urg::getDrawablePoints( const std::vector<long> &data, float scalar )
{
	std::vector<ci::vec2> ret(data.size());
	auto retIt = ret.begin();
	int i = 0;
	for( auto & length : mCurrentData ) {
		(*retIt++) = getPoint( i++, length, scalar );
	}
	return ret;
}
	
}