//
//  Button.h
//  SvgTest
//
//  Created by Ryan Bartley on 10/22/14.
//
//

#pragma once

#include "Common.h"
#include "SvgManager.h"

namespace heartbeat {
	
using ButtonId = uint64_t;
	
class Button {
public:
	
	static ButtonRef create( const std::string &name );
	
	inline bool contains( const ci::vec2 &point ) { return mParent->containsPoint( point ); }
	
	inline DataRef	getNavigation() { return mNavigation; }
	
	const std::string& getParentName() const { return mName; }
	
protected:
	Button( const std::string &name );
	
	void setNavigation( const DataRef &navigation ) { mNavigation = navigation; }
	
	const ci::svg::Group*	mParent;
	DataRef					mNavigation;
	std::string				mName;
};
	
}
