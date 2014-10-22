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
	
const uint32_t HidMessage::MAX_PACKET_SIZE = 64;
	
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
		
		try {
			auto topId = hidAttribs["top"].getValue<int16_t>();
			
			setTop( topId );
		}
		catch ( JsonTree::ExcChildNotFound &ex ) {
			CI_LOG_W("'top' child not found, using default -1, communication will be suspended");
			setTop( -1 );
		}
		
		try {
			auto bottomId = hidAttribs["bottom"].getValue<int16_t>();
			
			setBottom( bottomId );
		}
		catch ( JsonTree::ExcChildNotFound &ex) {
			CI_LOG_W("'bottom' child not found, using default -1, communication will be suspended");
			setBottom( -1 );
		}
		
		try {
			auto middleId = hidAttribs["middle"].getValue<int16_t>();
			
			setMiddle( middleId );
		}
		catch ( JsonTree::ExcChildNotFound &ex) {
			CI_LOG_W("'middle' child not found, using default -1, communication will be suspended");
			setMiddle( -1 );
		}
		
		int num = 0;
		for( auto & connection : mConnections ) {
			auto & hid = connection.second;
			hid.mNum = num;
			openConnections( hid );
			++num;
		}
		
	} catch ( JsonTree::ExcChildNotFound &ex ) {
		CI_LOG_W("'hidAttribs' child from root not found, using default -1, communication with all will be suspended");
		setTop( -1 );
		setBottom( -1 );
		setTop( -1 );
	}
}
	
void HidCommManager::setTop( int16_t topAddress )
{
	mConnections.insert( std::make_pair( HidId::TOP_KIOSK, std::move( Hid( HidId::TOP_KIOSK, topAddress ) ) ) );
}
	
void HidCommManager::setBottom( int16_t bottomAddress )
{
	mConnections.insert( std::make_pair( HidId::BOTTOM_KIOSK, std::move( Hid( HidId::BOTTOM_KIOSK, bottomAddress ) ) ) );
}
	
void HidCommManager::setMiddle( int16_t middleAddress )
{
	mConnections.insert( std::make_pair( HidId::MIDDLE_KIOSK, std::move( Hid( HidId::MIDDLE_KIOSK, middleAddress ) ) ) );
}
	
void HidCommManager::openConnections( Hid &hid )
{
	// Open up from top to Bottom
	
	if( hid.mAddress != -1 ) {
		
		int32_t numOpened = rawhid_open( 1, 0x16C0, 0x0480, 0xFFAB, hid.mAddress );
		if( numOpened == 1 ) {
			hid.mOpen = true;
			
			auto message = HidMessage();
			
			message.mBuffer = { HidProtocol::HANDSHAKE };
			message.mHid = &hid;
			
			write( message );
		}
			CI_LOG_W("HID " << static_cast<size_t>(hid.mId) << ": could not be opened");
	}
	else
		CI_LOG_W("HID " << static_cast<size_t>(hid.mId) << ": address is -1 and will not be opened");
}
	
void HidCommManager::write( const HidMessage &packet )
{
	auto bytesWritten = rawhid_send( packet.mHid->mNum,
				static_cast<void*>( const_cast<uint8_t*>( packet.mBuffer.data() ) ),
				packet.mBuffer.size(),
				10 );
	if( bytesWritten == -1 )
		CI_LOG_E("Returned -1 from: " << static_cast<int>(packet.mHid->mId) );
}
	
HidMessage HidCommManager::recv( const Hid *hid )
{
	HidMessage ret;
	// TODO: Figure out if I actually need that much info
	ret.mBuffer.resize( HidMessage::MAX_PACKET_SIZE );
	auto bytesRead = rawhid_recv( hid->mNum,
								 static_cast<void*>( const_cast<uint8_t*>( ret.mBuffer.data() ) ),
								 ret.mBuffer.size(),
								 2 );
	if( bytesRead == -1 )
		CI_LOG_E("Returned -1 from: " << static_cast<int>(hid->mId) );
	
	return ret;
}
	
void HidCommManager::update()
{
	for( auto & connection : mConnections ) {
		auto message = recv( &connection.second );
		
	}
}

	
}