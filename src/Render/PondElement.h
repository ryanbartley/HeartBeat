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
	
	static PondElementRef create( const ci::gl::GlslProgRef &shader );
	
	~PondElement() {}
	
	virtual void update() override {}
	virtual void draw() override;
	
	uint64_t getId() const { return mId; }
	
protected:
	PondElement( const ci::gl::GlslProgRef &shader );
	
	void initialize( const ci::JsonTree &root ) override;
	
	ci::gl::TextureRef	mDiffuseTexture;
	ci::gl::BatchRef	mBatch;
	ci::gl::GlslProgRef	mRenderShader;
	const uint64_t		mId;
	
	static uint64_t globalId;
	static uint64_t getNextId() { return globalId++; }
	
	friend class Pond;
};

}