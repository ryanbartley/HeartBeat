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
		mOpen( false ), mNum( -1 )
	{}
	
	const HidId		mId;
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
	
	std::map<HidId, Hid> mConnections;
	
	friend class Engine;
};
	
}
