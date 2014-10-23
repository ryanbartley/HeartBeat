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
	
enum class ButtonStatus {
	ACTIVE,
	VISIBLE,
	INVISIBLE
};
	
class Button {
public:
	
	virtual void changeState( InfoDisplayRef &display ) = 0;
	
	const std::string&		getGroupName() const { return mName; }
	const ci::svg::Group*	getRoot() const { return mGroup; }
	ButtonStatus			getStatus() const { return mStatus; }
	virtual ButtonId		getType() const = 0;
	
	void setStatus( ButtonStatus status ) { mStatus = status; }
	
	inline bool contains( const ci::vec2 &point ) { return mGroup->containsPoint( point ); }
	
	virtual void initializeGl() = 0;
	
protected:
	Button( const std::string &name );
	
	const ci::svg::Group*	mGroup;
	std::string				mName;
	ButtonStatus			mStatus;
};
	
}
