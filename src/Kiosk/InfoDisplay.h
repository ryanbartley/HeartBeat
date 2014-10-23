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
	
using InfoDisplayRef = std::shared_ptr<class InfoDisplay>;
	
class InfoDisplay : public Renderable {
public:
	
	static InfoDisplayRef create();
	
	~InfoDisplay();
	
	void draw() override;
	void update() override;
	
	const ci::Rectf getBoundingBox() { return mBoundingBox; }
	
	void activate();
	void deactivate();
	
	bool isActivated() { return mIsActivated; }
	bool insideAngle( float radians );
	
	void initialize();
	
private:
	InfoDisplay();
	
	ci::cairo::SurfaceSvg	mSurface;

		
	
	ci::Rectf				mBoundingBox;
	bool					mIsActivated;
	float					mMinAngle, mMaxAngle;
	KioskId					mId;
	
};
	
}
