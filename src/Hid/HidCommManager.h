//
//  HidCommunication.h
//  hidTest
//
//  Created by Ryan Bartley on 10/2/14.
//
//

#pragma once

#include "hid.h"

namespace heartbeat {
	
enum class HidId {
	TOP_KIOSK = 0,
	MIDDLE_KIOSK = 1,
	BOTTOM_KIOSK = 2
};
	
struct Hid {
	
	Hid( HidId hidId, int16_t address )
	: mId( hidId ), mAddress( address ),
		mOpen( false )
	{}
	
	const HidId		mId;
	const int16_t	mAddress;
	bool			mOpen;
};
	
struct HidMessage {
	
	Hid* mHid;
	std::array<uint8_t, 64> mBuffer;
};
	
struct HidProtocol {
	
	static const uint8_t HANDSHAKE;
};
	
using HidCommManagerRef = std::shared_ptr<class HidCommManager>;
	
class HidCommManager {
public:
	
	static HidCommManagerRef create();
	static HidCommManagerRef get();
	static void destroy();
	
	void update();
	
	void write( const HidMessage &packet );
	HidMessage recv();
	
private:
	HidCommManager();
	
	void initialize();
	
	void openConnections( Hid &hid );

	void setTop( int16_t topAddress );
	void setBottom( int16_t bottomAddress );
	void setMiddle( int16_t middleAddress );
	
	std::map<HidId, Hid> mConnections;
	
	friend class Engine;
};
	
}
