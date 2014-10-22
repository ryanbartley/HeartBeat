//
//  Fish.h
//  SpringMeshGpu
//
//  Created by Ryan Bartley on 10/20/14.
//
//

#pragma once

#include "PondElement.h"

namespace heartbeat {
	
using FishRef = std::shared_ptr<class Fish>;

class Fish : public PondElement {
public:
	
	static FishRef create();
	
	static PondElementId TYPE;
	
	PondElementId getType() const override { return TYPE; }
	
	void update() override;
	
private:
	Fish();
	
	void initialize( const ci::JsonTree &root ) override;
	
	void registerTouch( EventDataRef touchEvent );
	
	ci::vec2	mCurrentVelocity;
	float		mDistance;
	
	friend class Pond;
};

}