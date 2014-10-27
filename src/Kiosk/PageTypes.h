//
//  Pages.h
//  SvgTest
//
//  Created by Ryan Bartley on 10/24/14.
//
//

#pragma once

#include "Node.h"

namespace heartbeat {
	
class DataPage : public Page {
public:
	
	static DataPageRef create( const std::string &name );
	
	DataPageRef& next() { return mNext; }
	void connectBack( DataPageRef dataPage ) { mNext = dataPage; }
	
	void initialize( const ci::JsonTree &root ) override;
	
protected:
	DataPage( const std::string &name );
	
	DataPageRef mNext;
};
	
class OverlayPage : public Page {
public:
	
	static OverlayPageRef create( const std::string &name );

	void initialize( const ci::JsonTree &root ) override;
	
protected:
	OverlayPage( const std::string &name );
	
	std::string		mButton;
};
	
class OverlaySection : public OverlayPage {
public:
	
	static OverlaySectionRef create( const std::string &name );
	
	std::string& getSection() { return mSection; }
	
	
private:
	
	std::string		mSection;
};
	
class OverlayPlus : public OverlayPage {
public:
	
	static OverlayPlusRef create( const std::string &name );
	
private:
	std::vector<const ci::svg::Group*> mOverlays;
	std::vector<ci::gl::Texture2dRef>  mTextures;
	uint32_t mCurrentIndex;
};
	
}