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
	
class SingleTexturePage : public Page {
public:
	
	virtual ~SingleTexturePage() {}
	
	static const PageId TYPE;
	
	static SingleTexturePageRef create( const std::string &name );
	
	//! Returns ref to the underlying Texture
	ci::gl::TextureRef& getTexture() { return mTexture; }
	//! Returns const ref to the underlying Texture
	const ci::gl::TextureRef& getTexture() const { return mTexture; }
	
	//! Initializes gl elements
	virtual void initializeGl() override;
	//! Renders underlying gl elements in a debug way
	virtual void debugRender() override;
	
	virtual bool initialize( const ci::JsonTree &root ) override;
	
	virtual PageId getType() const override { return TYPE; }
	
	virtual void render() override;
	
	virtual PageRef clone() override;
	
protected:
	SingleTexturePage( const std::string &name );
	
	ci::gl::TextureRef mTexture;
};
	
/////////////////////////////////////////////////////////////////////////
/// Data Page
class DataPage : public SingleTexturePage {
public:
	
	virtual ~DataPage() {}
	
	//! The static hash Page type
	static const PageId TYPE;
	
	//! Returns the hash Type of PageId
	PageId getType() const override { return TYPE; }
	
	//! Returns new DataPageRef with id of \a name.
	static DataPageRef create( const std::string &name );
	
	//! Returns the next DataPage
	DataPageRef& next() { return mNext; }
	//! Returns the previous DataPage
	DataPageRef& prev() { return mPrev; }
	
	//! Sets the next DataPage
	void setNext( DataPageRef next ) { mNext = next; }
	//! Sets the prev DataPage
	void setPrev( DataPageRef prev ) { mPrev = prev; }
	
	//! Further initializes the DataPage
	bool initialize( const ci::JsonTree &root ) override;
	
	//! Clones the current DataPage copying the inner references
	PageRef clone() override;
	
	//! Renders the current state of the DataPage
	void render() override;
	
protected:
	DataPage( const std::string &name );
	
	DataPageRef mNext, mPrev;
	
};

/////////////////////////////////////////////////////////////////////////
/// Overlay Page
class OverlayPage : public SingleTexturePage {
public:
	
	virtual ~OverlayPage() {}
	
	//! The static hash Page type
	static const PageId TYPE;
	
	//! Returns the hash Type of PageId
	PageId getType() const override { return TYPE; }
	
	//! Returns new OverlayPageRef with id of \a name.
	static OverlayPageRef create( const std::string &name );

	//! Further initializes the OverlayPage
	virtual bool initialize( const ci::JsonTree &root ) override;
	
	//! Clones the current OverlayPage copying the inner references
	virtual PageRef clone() override;
	
	//! Renders the current state of the DataPage
	void render() override;
	
	//! Function that tests the touch against the buttons contained in the OverlayPage
	void touch( InfoDisplayRef &display, const ci::vec2 &touch );
	
	std::vector<ButtonRef>& getButtons() { return mButtons; }
	
	const std::string& getButtonGroup() const { return mButtonGroup; }
	
protected:
	OverlayPage( const std::string &name );
	
	std::vector<ButtonRef>	mButtons;
	std::string				mButtonGroup;
};
	
/////////////////////////////////////////////////////////////////////////
/// Overlay Section
class OverlaySection : public OverlayPage {
public:
	
	virtual ~OverlaySection() {}
	
	//! The static hash Page type
	static const PageId TYPE;
	
	//! Returns the hash Type of PageId
	PageId getType() const override { return TYPE; }
	
	static OverlaySectionRef create( const std::string &name );
	
	uint32_t getSection() const { return mSection; }
	
	bool initialize( const ci::JsonTree &root ) override;
	
	PageRef clone() override;
	
private:
	OverlaySection( const std::string &name );
	
	uint32_t		mSection;
};
	
/////////////////////////////////////////////////////////////////////////
/// Overlay Plus
class OverlayPlus : public OverlayPage {
public:
	
	virtual ~OverlayPlus() {}
	
	static const PageId TYPE;
	
	PageId getType() const override { return TYPE; }
	
	static OverlayPlusRef create( const std::string &name );
	
	bool initialize( const ci::JsonTree &root ) override;
	void initializeGl() override;
	
	bool nextIndex()
	{
		mCurrentIndex++;
		if( mCurrentIndex + 1 >= mTextures.size() ) {
			if( mCurrentIndex == mTextures.size() )
				mCurrentIndex = mTextures.size() - 1;
			return false;
		}
		return true;
	}
	bool prevIndex()
	{
		mCurrentIndex--;
		if( mCurrentIndex - 1 < 0 ) {
			if( mCurrentIndex < 0 )
				mCurrentIndex = 0;
			return false;
		}
		return true;
	}
	
	bool moreNext();
	bool morePrev();
	
	void debugRender() override;
	
	PageRef clone() override;
	
	void render() override;
	
private:
	OverlayPlus( const std::string &name );
	
	std::vector<const ci::svg::Group*>	mOverlays;
	std::vector<ci::gl::Texture2dRef>	mTextures;
	int32_t								mCurrentIndex;
};
	
}