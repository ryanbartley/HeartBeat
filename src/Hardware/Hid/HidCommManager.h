//
//  HidCommunication.h
//  hidTest
//
//  Created by Ryan Bartley on 10/2/14.
//
//

#pragma once

#include "hid.h"
#include "Common.h"

namespace heartbeat {
	
struct Hid {
	
	Hid( KioskId hidId, int16_t address )
	: mId( hidId ), mAddress( address ),
		mOpen( false ), mNum( -1 )
	{}
	
	const KioskId	mId;
	const int16_t	mAddress;
	int				mNum;
	bool			mOpen;
};
	
struct HidMessage {

	static const uint32_t MAX_PACKET_SIZE;
	
	Hid* mHid;
	std::vector<uint8_t> mBuffer;
};
	
struct HidProtocol {
	
	static const uint8_t HANDSHAKE;
};
	
using HidCommManagerRef = std::shared_ptr<class HidCommManager>;
	
class HidCommManager {
public:
	
	~HidCommManager();
	
	static HidCommManagerRef create();
	static HidCommManagerRef get();
	static void destroy();
	
	void update();
	
	void write( const HidMessage &packet );
	HidMessage recv( const Hid* hid );
	
private:
	HidCommManager();
	
	void initialize();
	
	void openConnections( Hid &hid );

	void setTop( int16_t topAddress );
	void setBottom( int16_t bottomAddress );
	void setMiddle( int16_t middleAddress );
	
	std::map<KioskId, Hid> mConnections;
	
	friend class Engine;
};
	
}
