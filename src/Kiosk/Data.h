//
//  StaticPage.h
//  SvgTest
//
//  Created by Ryan Bartley on 10/23/14.
//
//

#pragma once

#include "Common.h"
#include "SvgManager.h"

namespace heartbeat {
	
class Data {
public:
	
	DataRef& next() { return mNext; }
	
	void activate();
	
	
protected:
	Data( const std::string &name );
	
	void initialize();
	
	DataRef mNext;
	const ci::svg::Group* mRoot;
};
	
}
