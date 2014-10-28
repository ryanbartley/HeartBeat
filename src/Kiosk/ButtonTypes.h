//
//  ActivatableButton.h
//  SvgTest
//
//  Created by Ryan Bartley on 10/23/14.
//
//

#pragma once

#include "Node.h"
#include "PageTypes.h"

namespace heartbeat {
	
class ActivatableButton : public Button {
public:
	
	const ci::svg::Group* getActive() { return mActive; }
	const ci::svg::Group* getNonActive() { return mNonActive; }
	
	const ci::gl::Texture2dRef& getActiveTex() { return mActiveTex; }
	const ci::gl::Texture2dRef& getNonActiveTex() { return mNonActiveTex; }
	
protected:
	ActivatableButton( const std::string &buttonId );
	
	const ci::svg::Group	*mActive,	*mNonActive;
	ci::gl::Texture2dRef	mActiveTex, mNonActiveTex;
};
	
using DataPageButtonRef = std::shared_ptr<class DataPageButton>;
using OverlayPageButtonRef = std::shared_ptr<class OverlayPageButton>;
	
class DataPageButton : public ActivatableButton {
public:
	
	static const ButtonId TYPE;
	
	static DataPageButtonRef create( const std::string &buttonId );
	
	void changeState( InfoDisplayRef &display ) override;
	ButtonId getType() const override { return TYPE; }
	
	
	bool initialize( const ci::JsonTree &root ) override;
	void initializeGl() override;
	
private:
	DataPageButton( const std::string &buttonId );
	
	DataPageRef		mNavigation;
};
	
class OverlayPageButton : public ActivatableButton {
public:
	
	static const ButtonId TYPE;
	
	static OverlayPageButtonRef create( const std::string &buttonId );
	
	void changeState( InfoDisplayRef &display ) override;
	ButtonId getType() const override { return TYPE; }
	
	bool initialize( const ci::JsonTree &root ) override;
	void initializeGl() override;
	
private:
	OverlayPageButton( const std::string &buttonId );
	
	OverlayPageRef	mNavigation;
};
	
class StaticButton : public Button {
public:
	
	ci::gl::TextureRef& getTexture() { return mTexture; }
	
protected:
	StaticButton( const std::string &buttonId );
	
	ci::gl::TextureRef mTexture;
};
	
using CloseButtonRef = std::shared_ptr<class CloseButton>;
using NavigableButtonRef = std::shared_ptr<class NavigableButton>;
	
class CloseButton : public StaticButton {
public:
	
	static const ButtonId TYPE;
	
	static CloseButtonRef create( const std::string &buttonId );
	
	void changeState( InfoDisplayRef &display ) override;
	ButtonId getType() const override { return TYPE; }
	
	bool initialize( const ci::JsonTree &root ) override;
	void initializeGl() override;
	
private:
	CloseButton( const std::string &buttonId );
	
	
};
	
class NavigableButton : public StaticButton {
public:
	
	static const ButtonId TYPE;
	
	static NavigableButtonRef create( const std::string &buttonId );
	
	void changeState( InfoDisplayRef &display ) override;
	ButtonId getType() const override { return TYPE; }
	
	bool initialize( const ci::JsonTree &root ) override;
	void initializeGl() override;
	
private:
	NavigableButton( const std::string &buttonId );
};
	
}
