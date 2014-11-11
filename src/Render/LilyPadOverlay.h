//
//  LilyPadOverlay.h
//  FullProject
//
//  Created by Ryan Bartley on 11/10/14.
//
//

#pragma once

#include "Renderable.h"
#include "Common.h"

namespace heartbeat {
	
class LilyPad : public Renderable {
public:
	
	static LilyPadRef create( KioskId kioskId, const ci::gl::Texture2dRef &lightTexture, const ci::gl::Texture2dRef &darkTexture );
	
	void update() override {}
	void draw() override;
	
	void activate( bool activate );
	
	void initialize( const ci::JsonTree &root ) override;
	bool isActivated() const { return mIsActivated; }
	
private:
	LilyPad( KioskId kioskId, const ci::gl::Texture2dRef &lightTexture, const ci::gl::Texture2dRef &darkTexture );
	
	ci::gl::Texture2dRef	mLightTex, mDarkTex;
	float					mVisibility;
	const KioskId			mKiosk;
	bool					mIsActivated;
};
	
}