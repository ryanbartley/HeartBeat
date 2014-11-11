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
	
	static LilyPadRef create( const ci::gl::Texture2dRef &texture );
	
	void update() override;
	void draw() override;
	
	void activate( bool activate );
	
	void initialize( const ci::JsonTree &root ) override;
	
private:
	LilyPad( KioskId kiosk, const ci::gl::Texture2dRef &texture );
	
	ci::gl::Texture2dRef	mLilyPad;
	float					mVisibility;
	const KioskId			mKiosk;
};
	
}