//
//  Kiosk.h
//  ProjectionTestApp
//
//  Created by Ryan Bartley on 10/1/14.
//
//

#pragma once

#include "InfoDisplay.h"

namespace heartbeat {
	
using KioskRef = std::shared_ptr<class Kiosk>;
	
class Kiosk {
public:
	
	static KioskRef create();
	
	~Kiosk();
	
private:
	Kiosk();
	
	std::array<InfoDisplayRef, 3> mHotSpots;
};
	
}