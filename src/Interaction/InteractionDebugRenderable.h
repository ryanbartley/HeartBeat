//
//  UrgDebug.h
//  LibUrgTest
//
//  Created by Ryan Bartley on 10/7/14.
//
//

#pragma once

#include "Common.h"
#include "Renderable.h"

#include "Urg.h"

#include "cinder/gl/Vbo.h"
#include "cinder/gl/Vao.h"

namespace heartbeat {

class InteractionDebugRenderable : public Renderable {
public:
	
	static InteractionDebugRenderableRef create( const InteractionZonesRef &interactionZones );
	
	~InteractionDebugRenderable() {}
	
	void update() {}
	void draw();
	
	void initialize();
	
	void setupZoneGlBuffers();
	void drawZones();
	
	void enableDistance( bool enable = true ) { mDrawDistanceData = enable; }
	void enableZone( bool enable = true ) { mDrawZoneData = enable; }
	
	bool isDistanceEnabled() { return mDrawDistanceData; }
	bool isZoneEnabled() { return mDrawZoneData; }

private:
	InteractionDebugRenderable( const InteractionZonesRef &interactionZones );
	
	ci::gl::VaoRef			mBarrierVao;
	ci::gl::VboRef			mBarrierVbo;
	size_t					mNumBarrierVertices;
	
	InteractionZonesRef		mInteractionZones;
	
	bool					mDrawDistanceData, mDrawZoneData, mZoneCached;
};

}