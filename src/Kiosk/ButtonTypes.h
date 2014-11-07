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
	
enum class ButtonStatus {
	ACTIVE,
	NONACTIVE
};
	
//! A further implementation of a "Button" that is just a pressable button with one texture
class StaticButton : public Button {
public:
	
	//! Returns a ref to underlying Button Texture
	ci::gl::TextureRef& getTexture() { return mTexture; }
	//! Returns a const ref to the underlying Button Texture
	const ci::gl::TextureRef& getTexture() const { return mTexture; }
	
	//! Further initializes this button type and returns true if it was able to
	//! fully intialize
	virtual bool initialize( const ci::JsonTree &root ) override;
	//! Initializes the Gl components of this node
	virtual void initializeGl() override;
	//! Renders Gl components in a debug manner
	virtual void debugRender() override;
	//! Render Final Gl component
	virtual void render() override;
	
protected:
	StaticButton( const std::string &buttonId );
	
	ci::gl::TextureRef mTexture;
};

using ReturnButtonRef = std::shared_ptr<class ReturnButton>;
using NavigableButtonRef = std::shared_ptr<class NavigableButton>;

class ReturnButton : public StaticButton {
public:
	
	enum class ReturnStatus {
		DATA_PAGE,
		HOME_PAGE,
		CLOSE_OVERLAY
	};
	
	//! The hashed id of this Button
	static const ButtonId TYPE;
	
	//! Create method, returns a ReturnButtonRef, and taking the \a buttonId
	static ReturnButtonRef create( const std::string &buttonId );
	
	ButtonRef clone() override;
	
	//! Changes the State of the InfoDisplay when this button is pressed
	void changeState( InfoDisplayRef &display ) override;
	
	//! Further initializes this button type and returns true if it was able to
	//! fully intialize
	bool initialize( const ci::JsonTree &root ) override;
	//! Returns the type of this Button
	ButtonId getType() const override { return TYPE; }
	
private:
	ReturnButton( const std::string &buttonId );
	
	std::string		mNavigation;
	std::string		mActiveButtonName;
	ReturnStatus	mStatus;
	uint32_t		mSection;
};
	
class CloseButton : public StaticButton {
	
};

class NavigableButton : public StaticButton {
public:
	
	enum class NavigationStatus {
		NEXT,
		PREV
	};
	
	//! The hashed id of this Button
	static const ButtonId TYPE;
	
	//! Create method, returns a ReturnButtonRef, and taking the \a buttonId
	static NavigableButtonRef create( const std::string &buttonId );
	
	ButtonRef clone() override;
	
	//! Changes the State of the InfoDisplay when this button is pressed
	void changeState( InfoDisplayRef &display ) override;
	
	//! Further initializes this button type and returns true if it was able to
	//! fully intialize
	bool initialize( const ci::JsonTree &root ) override;
	//! Returns the type of this Button
	ButtonId getType() const override { return TYPE; }
	
	const std::string& getOppositeName() const { return mOppositeName; }
	void setOpposite( NavigableButtonRef &button ) { mOpposite = button; }
	NavigableButtonRef getOpposite() { return mOpposite; }
	ButtonStatus getButtonStatus() const { return mButtonStatus; }
	void setButtonStatus( ButtonStatus status ) { mButtonStatus = status; }
	NavigationStatus getNavigationStatus() { return mNavigationStatus; }
	
private:
	NavigableButton( const std::string &buttonId );

	NavigationStatus	mNavigationStatus;
	std::string			mOppositeName;
	NavigableButtonRef	mOpposite;
	ButtonStatus		mButtonStatus;
};

//! A further implementation of a "Button" that stores an Active and NonActive state
class ActivatableButton : public Button, public std::enable_shared_from_this<ActivatableButton> {
public:
	
	//! Returns a const pointer to the "Active" Group
	const ci::svg::Group* getActive() { return mActive; }
	//! Returns a const pointer to the "NonActive" Group
	const ci::svg::Group* getNonActive() { return mNonActive; }
	
	//! Returns a const Ref to the Active Texture
	const ci::gl::Texture2dRef& getActiveTex() { return mActiveTex; }
	//! Returns a const Ref to the NonActive Texture
	const ci::gl::Texture2dRef& getNonActiveTex() { return mNonActiveTex; }
	
	//! Initializes the Gl components of this node
	void initializeGl() override;
	//! Renders Gl components in a debug manner
	void debugRender() override;
	
	void render() override;
	
	void setStatus( ButtonStatus status ) { mStatus = status; }
	ButtonStatus			getStatus() const { return mStatus; }
	
	virtual void changeState( InfoDisplayRef &display ) override;
	
protected:
	ActivatableButton( const std::string &buttonId );
	
	const ci::svg::Group	*mActive,	*mNonActive;
	ci::gl::Texture2dRef	mActiveTex, mNonActiveTex;
	ButtonStatus			mStatus;
};
	
using DataPageButtonRef = std::shared_ptr<class DataPageButton>;
using OverlayPageButtonRef = std::shared_ptr<class OverlayPageButton>;
using OverlayPageSectionButtonRef = std::shared_ptr<class OverlayPageSectionButton>;

//! A further implementation of an "ActivatableButton" which knows how to initialize
//! and cleanup a DataPage
class DataPageButton : public ActivatableButton {
public:
	
	//! The hashed id of this Button
	static const ButtonId TYPE;
	
	//! Create method, returns a DataPageButtonRef, and taking the \a buttonId
	static DataPageButtonRef create( const std::string &buttonId );
	
	ButtonRef clone() override;
	
	//! Changes the State of the InfoDisplay when this button is pressed
	void changeState( InfoDisplayRef &display ) override;
	//! Returns the type of this Button
	ButtonId getType() const override { return TYPE; }
	
	//! Further initializes this button type and returns true if it was able to
	//! fully intialize
	bool initialize( const ci::JsonTree &root ) override;
	
	//! Returns a reference to the page this button navigates to
	DataPageRef getNavigation() { return mNavigation; }
	
	const std::string& getDataPageName() { return mDataPageName; }
	
	void setDataPage( const DataPageRef &dataPage ) { mNavigation = dataPage; }
	
private:
	DataPageButton( const std::string &buttonId );
	
	DataPageRef		mNavigation;
	uint32_t		mSection;
	std::string		mDataPageName;
};
	
//! A further implementation of an "ActivatableButton" which knows how to initialize
//! and cleanup an OverlayPage
class OverlayPageButton : public ActivatableButton {
public:
	
	//! The hashed id of this Button
	static const ButtonId TYPE;
	
	//! Create method, returns a OverlayPageButtonRef, and taking the \a buttonId
	static OverlayPageButtonRef create( const std::string &buttonId );
	
	virtual ButtonRef clone() override;
	
	//! Changes the State of the InfoDisplay when this button is pressed
	virtual void changeState( InfoDisplayRef &display ) override;
	//! Returns the type of this Button
	virtual ButtonId getType() const override { return TYPE; }
	
	//! Further initializes this button type and returns true if it was able to
	//! fully intialize
	virtual bool initialize( const ci::JsonTree &root ) override;
	
protected:
	OverlayPageButton( const std::string &buttonId );
	
	OverlayPageRef				 mNavigation;
	std::vector<StaticButtonRef> mButtons;
};
	
class OverlayPageSectionButton : public OverlayPageButton {
public:
	
	static const ButtonId TYPE;
	
	static OverlayPageSectionButtonRef create( const std::string &name );
	
	ButtonRef clone() override;
	
	ButtonId getType() { return TYPE; }
	
	bool initialize( const ci::JsonTree &root ) override;
	
	void changeState( InfoDisplayRef &display ) override;
	
private:
	OverlayPageSectionButton( const std::string &buttonId );
	
	std::string mPrefix;
};
	
}
