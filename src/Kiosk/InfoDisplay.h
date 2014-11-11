//
//  InfoDisplay.h
//  ProjectionTestApp
//
//  Created by Ryan Bartley on 10/1/14.
//
//

#pragma once

#include "Renderable.h"
#include "Node.h"

namespace heartbeat {
	
class InfoDisplay : public Renderable, public std::enable_shared_from_this<InfoDisplay> {
public:
	
	enum class Status {
		HOME_SCREEN,
		DATA_SCREEN
	};
	
	enum class AnimateType {
		RIGHT_TO_LEFT,
		LEFT_TO_RIGHT,
		FADE_OUT_FADE_IN,
		CUT
	};
	
	static InfoDisplayRef create( KioskId kioskId );
	
	~InfoDisplay() {}
	
	void draw() override;
	void update() override;
	
	const ci::Rectf getPresentationRect() { return mPresentRect; }
	
	void activate( bool activate );
	void reset();
	
	bool isActivated() { return mIsActivated; }
	bool insideAngle( int index ) { return mMinIndex < index && mMaxIndex > index; }
	
	void renderCurrentScene();
	void renderHomeScreen();
	void renderDataScreen();
	void renderOverlayScreen();
	
	void toggleStatus();
	
	void registerTouchBegan( EventDataRef eventData );
	void registerTouchMoved( EventDataRef eventData );
	
	void addDataPage( DataPageRef &page, AnimateType type );
	void addOverlayPage( OverlayPageRef &page );
	
	std::vector<ButtonRef>& getHomeButtons() { return mHomeButtons; }
	std::vector<ButtonRef>& getDataButtons() { return mDataButtons; }
	
	void initiaize( const ci::JsonTree &root );
	
	void setPresentFbo( const ci::gl::FboRef &fbo ) { mPresentationFbo = fbo; }
	void setStatus( Status status ) { mStatus = status; }
	
	std::map<std::string, PageRef>& getPageCache() { return mPageCache; }
	std::deque<DataPageRef>& getCurrentPages() { return mDataPages; }
	OverlayPageRef& getOverlayPage() { return mOverlay; }
	
	void setSection( uint32_t section ) { mCurrentSection = section; }
	uint32_t getSection() { return mCurrentSection; }
	
	ActivatableButtonRef& getActivatedButton() { return mActivatedButton; }
	void setActivatedButton( const ActivatableButtonRef &button ) { mActivatedButton = button; }
	
	void removeFront();
	
	void renderToFbo();
	
	ci::gl::FboRef& getPresentationFbo() { return mPresentationFbo; }
	
private:
	InfoDisplay( KioskId kioskId );

	ci::gl::FboRef					mPresentationFbo;
	Status							mStatus;
	uint32_t						mCurrentSection;
	
	std::deque<DataPageRef>			mDataPages;
	std::map<std::string, PageRef>	mPageCache;
	OverlayPageRef					mOverlay;
	ActivatableButtonRef			mActivatedButton;
	std::vector<ButtonRef>			mHomeButtons,
									mDataButtons;
	std::map<std::string,
	std::vector<ButtonRef>>			mOverlayButtons;

#if defined( DEBUG )
    std::vector<ci::vec2>			mPoints;
#endif
	
	PageRef							mBackGround,
									mLines;
	
	ci::Rectf						mPresentRect;
	float							mMasterAlpha;
	bool							mIsActivated;
	int								mMinIndex, mMaxIndex;
	bool							mIsHalfSized;
	
	const KioskId					mId;
};
	
}
