//
//  ActivatableButton.h
//  SvgTest
//
//  Created by Ryan Bartley on 10/23/14.
//
//

#pragma once

#include "Button.h"
#include "Data.h"
#include "Overlay.h"

namespace heartbeat {
	
class ActivatableButton : public Button {
public:
	
	inline const ci::svg::Group* getActive() { return mActive; }
	inline const ci::svg::Group* getNonActive() { return mNonActive; }
	
	inline const ci::gl::Texture2dRef& getActiveTex() { return mActiveTex; }
	inline const ci::gl::Texture2dRef& getNonActiveTex() { return mNonActiveTex; }
	
protected:
	ActivatableButton( const std::string &name );
	
	const ci::svg::Group	*mActive,	*mNonActive;
	ci::gl::Texture2dRef	mActiveTex, mNonActiveTex;
};
	
using DataPageButtonRef = std::shared_ptr<class DataPageButton>;
using OverlayButtonRef = std::shared_ptr<class OverlayButton>;
	
class DataPageButton : public ActivatableButton {
public:
	
	static const ButtonId TYPE;
	
	static DataPageButtonRef create( const std::string &name );
	
	void changeState( InfoDisplayRef &display ) override;
	ButtonId getType() const override { return TYPE; }
	
	void initializeGl() override;
	
private:
	DataPageButton( const std::string &name );
	
	DataRef		mNavigation;
};
	
class OverlayButton : public ActivatableButton {
public:
	
	static const ButtonId TYPE;
	
	static OverlayButtonRef create( const std::string &name );
	
	void changeState( InfoDisplayRef &display ) override;
	ButtonId getType() const override { return TYPE; }
	
	void initializeGl() override;
	
private:
	OverlayButton( const std::string &name );
	
	OverlayRef	mNavigation;
};
	
class PressableButton : public Button {
public:
	
	ci::gl::TextureRef& getTexture() { return mTexture; }
	
protected:
	PressableButton( const std::string &name );
	
	ci::gl::TextureRef mTexture;
};
	
using CloseButtonRef = std::shared_ptr<class CloseButton>;
using NavagableButtonRef = std::shared_ptr<class NavagableButton>;
	
class CloseButton : public PressableButton {
public:
	
	static const ButtonId TYPE;
	
	static CloseButtonRef create( const std::string &name );
	
	void changeState( InfoDisplayRef &display ) override;
	ButtonId getType() const override { return TYPE; }
	
	void initializeGl() override;
	
private:
	CloseButton( const std::string &name );
};
	
class NavagableButton : public PressableButton {
public:
	
	static const ButtonId TYPE;
	
	static CloseButtonRef create( const std::string &name );
	
	void changeState( InfoDisplayRef &display ) override;
	ButtonId getType() const override { return TYPE; }
	
	void initializeGl() override;
	
private:
	NavagableButton( const std::string &name );
};
	
}
