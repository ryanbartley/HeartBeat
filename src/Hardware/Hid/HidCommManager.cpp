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
	
Hid::Hid( KioskId kioskId, int hidId )
: mKioskId( kioskId ), mId( hidId ), mActivated( false ),
mOpen( true )
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
			mConnections.insert( make_pair( KioskId::TOP_KIOSK, std::move( Hid( KioskId::TOP_KIOSK, hidAttribs["top"].getValue<int>() ) ) ) );
			mConnections.insert( make_pair( KioskId::MIDDLE_KIOSK, std::move( Hid( KioskId::MIDDLE_KIOSK, hidAttribs["middle"].getValue<int>() ) ) ) );
			mConnections.insert( make_pair( KioskId::BOTTOM_KIOSK, std::move( Hid( KioskId::BOTTOM_KIOSK, hidAttribs["bottom"].getValue<int>() ) ) ) );
		}
		catch ( JsonTree::ExcChildNotFound &ex ) {
			CI_LOG_W("'top' child not found, using default -1, communication will be suspended" << ex.what() );
			
		}
		
		
	} catch ( JsonTree::ExcChildNotFound &ex ) {
		CI_LOG_W("'hidAttribs' child from root not found, using default -1, communication with all will be suspended");
	}
}
	
void HidCommManager::send( const Hid &hid, HidMessage &packet )
{
	packet.mBytesUsed = rawhid_send_by_id( hid.mId,
									static_cast<void*>( &(packet.mBuffer[0]) ),
									packet.mBytesUsed,
									0 );
	if( packet.mBytesUsed == 0 )
		CI_LOG_E("Transmitted nothing" << getKiosk( hid.mKioskId ) );
	else if( packet.mBytesUsed == -1 )
		CI_LOG_E("Returned -1 from: " << getKiosk( hid.mKioskId ) );
}
	
HidMessage HidCommManager::recv( const Hid &hid )
{
	HidMessage ret;
	// TODO: Figure out if I actually need that much info
	ret.mBytesUsed = rawhid_recv_by_id( hid.mId,
										static_cast<void*>( &(ret.mBuffer[0]) ),
										ret.mBytesUsed,
										0 );
	if( ret.mBytesUsed == 0 )
		CI_LOG_W("Received 0 bytes");
	if( ret.mBytesUsed == -1 )
		CI_LOG_E("Returned -1 from: " << getKiosk( hid.mKioskId ) );
	
	return ret;
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
		if( found == mConnections.end() ) {
			CI_LOG_E("Couldn't find not sending to " << getKiosk( kioskId ) );
		}
		else {
		CI_LOG_E("Already Activated " << activate);
		}
	}
}
	
}