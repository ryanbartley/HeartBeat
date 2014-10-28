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
	
	Hid( KioskId kioskId, int hidId );
	
	const KioskId	mKioskId;
	const int		mId;
	bool			mOpen;
	bool			mActivated;
};
	
struct HidMessage {

	static constexpr uint32_t MAX_PACKET_SIZE = 64;

	int  mBytesUsed = 64;
	std::array<uint8_t, MAX_PACKET_SIZE> mBuffer;
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
	
	void send( const Hid &hid, HidMessage &packet );
	HidMessage recv( const Hid & hid );
	
	void activate( KioskId kioskId, bool activate );
	
	void initialize();
	
private:
	HidCommManager();
	
	std::map<KioskId, Hid> mConnections;
	
	friend class Engine;
};
	
}
