//
//  InfoDisplay.h
//  ProjectionTestApp
//
//  Created by Ryan Bartley on 10/1/14.
//
//

#pragma once

#include "cinder/svg/SvgGl.h"

namespace heartbeat {
	
using InfoDisplayRef = std::shared_ptr<class InfoDisplay>;
	
class InfoDisplay {
public:
	
	static InfoDisplayRef create();
	
	~InfoDisplay();
	
	void display();
	void update();
	
	const ci::Rectf getBoundingBox() { return mBoundingBox; }
	
	void activate();
	void deactivate();
	
	bool insideAngle( float radians );
	
private:
	InfoDisplay();
	
	void setOrientation( const ci::Quatf &orientation );
	void setScale( const ci::vec3 &scale );
	void setPosition( const ci::vec3 &scale );
	void setBoundingBox( const ci::Rectf &bounding );
	
	ci::Rectf	mBoundingBox;
	float		mMinAngle, mMaxAngle;
	
};
	
}
