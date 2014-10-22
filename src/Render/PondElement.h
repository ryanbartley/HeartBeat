//
//  PondElement.h
//  SpringMeshGpu
//
//  Created by Ryan Bartley on 10/20/14.
//
//

#pragma once

#include "Renderable.h"

#include "Common.h"

#include "cinder/gl/Batch.h"

namespace heartbeat {
	
using PondElementId = uint64_t;

class PondElement : public Renderable {
public:
	
	static PondElementId TYPE;
	
	virtual PondElementId getType() const { return TYPE; }
	
	static PondElementRef create();
	
	~PondElement() {}
	
	virtual void update() override {}
	virtual void draw() override;
	
protected:
	PondElement();
	
	void initialize( const ci::JsonTree &root ) override;
	
	ci::gl::BatchRef mBatch;
	
	friend class Pond;
};

}