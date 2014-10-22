//
//  Kiosk.h
//  ProjectionTestApp
//
//  Created by Ryan Bartley on 10/1/14.
//
//

#pragma once

#include "Common.h"
#include "InfoDisplay.h"

namespace heartbeat {
	
enum class HidId;
	
class Kiosk {
public:
	
	static KioskRef create();
	
	~Kiosk();
	
	void update();
	void draw();
	
private:
	Kiosk();
	
	void initialize();
	
	InfoDisplayRef	mHotSpot;
	HidId			mHid;
};
	
}