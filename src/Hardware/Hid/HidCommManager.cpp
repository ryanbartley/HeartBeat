//
//  HidCommunication.cpp
//  hidTest
//
//  Created by Ryan Bartley on 10/2/14.
//
//

#include "HidCommManager.h"
#include "JsonManager.h"

#include "cinder/Log.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace heartbeat {
	
/////////////////////////////////////////////////////////////////////////////
// PROTCOL Definitions
/////////////////////////////////////////////////////////////////////////////
	
const uint8_t HidProtocol::HANDSHAKE = 255;
	
Hid::Hid( KioskId hidId, int index )
: mId( hidId ), mActivated( false ),
mOpen( true ), mNum( index )
{}
	
HidCommManager::HidCommManager()
{
	
}
	
HidCommManager::~HidCommManager()
{
	
}
	
HidCommManagerRef HidCommManager::create()
{
	return HidCommManagerRef( new HidCommManager );
}
	
void HidCommManager::initialize()
{
	try {
		auto hidAttribs = JsonManager::get()->getRoot()["hid"];
		
		int numToOpen = hidAttribs["numToOpen"].getValue<int>();
		
		int32_t numOpened = rawhid_open( numToOpen, 0x16C0, 0x0480, 0xFFAB, 0 );
		if( numOpened != numToOpen ) {
			CI_LOG_E("Didn't open 3 hid's, instead opened " << numOpened);
		}
		
		try {
			auto positions = hidAttribs["positions"].getChildren();
			
			int i = 0;
			
			for( auto & pos : positions ) {
				KioskId firstPosKiosk;
				auto position = pos.getValue();
				if( position == "top" ) {
					cout << i << ": top" << endl;
					firstPosKiosk = KioskId::TOP_KIOSK;
				}
				else if ( position == "middle" ) {
					cout << i << ": middle" << endl;
					firstPosKiosk = KioskId::MIDDLE_KIOSK;
				}
				else if ( position == "bottom" ) {
					cout << i << ": bottom" << endl;
					firstPosKiosk = KioskId::BOTTOM_KIOSK;
				}
				else {
					CI_LOG_E("Couldn't find the Id, defaulting to TOP");
					firstPosKiosk = KioskId::TOP_KIOSK;
				}
				
				mConnections.insert( make_pair( firstPosKiosk, std::move( Hid( firstPosKiosk, i++ ) ) ) );
			}
			
			if( numToOpen != i ) {
				CI_LOG_E("NumToOpen is not how many were opened " << "numToOpen: " << numToOpen << " i: " << i );
			}
		}
		catch ( JsonTree::ExcChildNotFound &ex ) {
			CI_LOG_W("'top' child not found, using default -1, communication will be suspended");
			
		}
		
		
	} catch ( JsonTree::ExcChildNotFound &ex ) {
		CI_LOG_W("'hidAttribs' child from root not found, using default -1, communication with all will be suspended");
	}
}
	
void HidCommManager::send( const Hid &hid, HidMessage &packet )
{
	if( hid.mNum != -1 ) {
		ci::Timer time;
		time.start();
		packet.mBytesUsed = rawhid_send( hid.mNum,
										static_cast<void*>( &(packet.mBuffer[0]) ),
										packet.mBytesUsed,
										0 );
		time.stop();
		cout << "TIME: " << time.getSeconds() << endl;
		if( packet.mBytesUsed == 0 )
			CI_LOG_E("Transmitted nothing" << getReadableHid( hid.mId ) );
		else if( packet.mBytesUsed == -1 )
			CI_LOG_E("Returned -1 from: " << getReadableHid( hid.mId ) );
	}
	else {
		CI_LOG_E("DIDN'T SEND");
	}
}
	
HidMessage HidCommManager::recv( const Hid &hid )
{
	if( hid.mNum != -1 ) {
		HidMessage ret;
		// TODO: Figure out if I actually need that much info
		ret.mBytesUsed = rawhid_recv( hid.mNum,
											static_cast<void*>( &(ret.mBuffer[0]) ),
											ret.mBytesUsed,
											0 );
		if( ret.mBytesUsed == 0 )
			CI_LOG_W("Received 0 bytes");
		if( ret.mBytesUsed == -1 )
			CI_LOG_E("Returned -1 from: " << getReadableHid( hid.mId ) );
		
		return ret;
	}
	else {
		return HidMessage();
	}
}
	
void HidCommManager::activate( heartbeat::KioskId kioskId, bool activate )
{
	auto found = mConnections.find( kioskId );
	if( found != mConnections.end() && found->second.mActivated != activate ) {
		found->second.mActivated = activate;
		HidMessage mess;
		if( activate )
			mess.mBuffer.fill( 255 );
		else
			mess.mBuffer.fill( 0 );
		
		send( found->second, mess );
	}
	else {
		CI_LOG_E("DIDN'T SEND");
	}
}

std::string HidCommManager::getReadableHid( KioskId kioskId )
{
	return " Hid " + std::to_string( static_cast<size_t>(kioskId) ) + " ";
}

	
}