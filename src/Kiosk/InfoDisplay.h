//
//  InfoDisplay.h
//  ProjectionTestApp
//
//  Created by Ryan Bartley on 10/1/14.
//
//

#pragma once

#include "Renderable.h"

#include "Cairo.h"

#include "Node.h"

namespace heartbeat {
	

	
class InfoDisplay : public Renderable {
public:
	
	static InfoDisplayRef create( KioskId kioskId );
	
	~InfoDisplay() {}
	
	void draw() override;
	void update() override;
	
	const ci::Rectf getBoundingBox() { return mBoundingBox; }
	
	void activate();
	void deactivate();
	
	bool isActivated() { return mIsActivated; }
	bool insideAngle( float radians ) { return mMinAngle < radians && mMaxAngle > radians; }
	
	void registerTouch( EventDataRef eventData );
	
	ci::cairo::Context& getContext() { return mContext; }
	
private:
	InfoDisplay( KioskId kioskId );
	
	void initiaize( const ci::JsonTree &root );
	
	ci::cairo::SurfaceImage	mSurface;
	ci::cairo::Context		mContext;
	
	ci::gl::Texture2dRef	mTexture;
	ci::Rectf				mBoundingBox;
	bool					mIsActivated;
	float					mMinAngle, mMaxAngle;
	const KioskId			mId;
	
};
	
}
