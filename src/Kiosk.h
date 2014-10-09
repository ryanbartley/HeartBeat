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
	
class Kiosk {
public:
	
	static KioskRef create();
	
	~Kiosk();
	
private:
	Kiosk();
	
	InfoDisplayRef	mHotSpot;
};
	
}