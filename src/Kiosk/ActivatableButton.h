//
//  ActivatableButton.h
//  SvgTest
//
//  Created by Ryan Bartley on 10/23/14.
//
//

#pragma once

#include "Button.h"

namespace heartbeat {
	
using ActivatableButtonRef = std::shared_ptr<class ActivatableButton>;
	
class ActivatableButton : public Button {
public:
	
	static ActivatableButtonRef create( const std::string &name );
	
	inline const ci::svg::Group* getActive() { return mActive; }
	inline const ci::svg::Group* getNonActive() { return mNonActive; }
	
private:
	ActivatableButton( const std::string &name );
	
	const ci::svg::Group	*mActive, *mNonActive;
};
	
}
