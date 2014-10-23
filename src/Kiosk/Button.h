//
//  Button.h
//  SvgTest
//
//  Created by Ryan Bartley on 10/22/14.
//
//

#pragma once

#include "Node.h"

namespace heartbeat {
	
class Button : public Node {
public:
	
	enum class State {
		ACTIVE,
		NONACTIVE
	};
	
	inline bool contains( const ci::vec2 &point ) { return mNode->containsPoint( point ); }
	
	inline void setState( State state ) { mState = state; }
	inline State getState() { return mState; }
	
private:
	Button( const std::string &name );
	
	State	mState;
	bool	mIsTouched;
	
};
	
}
