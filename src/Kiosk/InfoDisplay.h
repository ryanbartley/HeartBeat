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
#include "Cairo.h"
#include "deque"

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
	
	static InfoDisplayRef create( KioskId kioskId, bool shouldRenderSvgs );
	
	~InfoDisplay() {}
	
	void draw() override;
	void update() override;
	
	const ci::Rectf getPresentationRect() { return mPresentRect; }
	
	void activate( bool activate );
	void finished();
	
	bool isActivated() { return mIsActivated; }
	bool insideAngle( int index ) { return mMinIndex < index && mMaxIndex > index; }
	
	void renderCurrentSceneTex();
	void renderHomeScreenTex();
	void renderDataScreenTex();
	void renderOverlayScreenTex();
	
	void renderCurrentSceneSvg( ci::cairo::Context &context );
	void renderHomeScreenSvg( ci::cairo::Context &context );
	void renderDataScreenSvg( ci::cairo::Context &context );
	void renderOverlayScreenSvg( ci::cairo::Context &context );
	
	void renderToFbo();
	void renderToSvg();
	
	void toggleStatus();
    void toggleRenderSvgs() { mRenderWithCairo = !mRenderWithCairo; }
	
	void registerTouchBegan( EventDataRef eventData );
	void registerTouchMoved( EventDataRef eventData );
	void registerTouchEnded( EventDataRef eventData );
	
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
	void setOverlayActivatedButton( const ActivatableButtonRef &button ) { mOverlayActiveButton = button; }
	ActivatableButtonRef& getOverlayActivatedButton() { return mOverlayActiveButton; }
	std::map<std::string, std::vector<ButtonRef>>& getOverlayButtons() { return mOverlayButtons; }
	
	void enableBoundingBoxRender( bool enable ) { mShouldDrawBoundingBoxes = enable; }
	bool getBoundingBoxRender() {return mShouldDrawBoundingBoxes; }
	void removeFront();
	void checkInteraction( const ci::vec2 &point );
	ci::gl::FboRef& getPresentationFbo() { return mPresentationFbo; }
	
	inline ci::vec2 getCoordinateSpacePoint( const ci::vec2 &point );
	
	struct TouchData {
		
		TouchData( ci::vec2 &point, bool contained )
		{
			mHistory.push_back( point );
		}
		
		inline bool valid()
		{
			return mHistory.size() > 2;
		}
		
		inline ci::vec2 getPointOfInterest()
		{
			ci::vec2 ret;
			int historySize = mHistory.size();
			if( historySize < 6 ) {
				ret = mHistory[3];
			}
			else {
				ret = mHistory[(historySize - 1) - 3];
			}
			return ret;
		}
		
		bool mContained;
		std::vector<ci::vec2> mHistory;
	};
	
private:
	InfoDisplay( KioskId kioskId, bool shouldRenderSvgs );

	ci::gl::FboRef					mPresentationFbo;
	Status							mStatus;
	uint32_t						mCurrentSection;
	
	ci::gl::TextureRef				mCairoTex;
	ci::Rectf						mTranslatedPresentRect;
	ci::MatrixAffine2<float>		mCairoMat;
	bool							mRenderWithCairo, mStateChanged;
	
	std::deque<DataPageRef>			mDataPages;
	std::map<std::string, PageRef>	mPageCache;
	OverlayPageRef					mOverlay;
	ActivatableButtonRef			mOverlayActiveButton;
	ActivatableButtonRef			mActivatedButton;
	std::vector<ButtonRef>			mHomeButtons,
									mDataButtons;
	std::map<std::string,
	std::vector<ButtonRef>>			mOverlayButtons;

#if defined( DEBUG )
    std::vector<ci::vec2>			mPoints;
#endif
	std::map<uint64_t,TouchData>	mPointMap;
	
	PageRef							mBackGround,
									mLines;
	
	ci::Rectf						mPresentRect;
	float							mMasterAlpha, mFadeTime;
	int								mMinIndex, mMaxIndex;
	bool							mIsHalfSized, mIsActivated,
									mShouldDrawBoundingBoxes;
	
	const KioskId					mId;
};
	
ci::vec2 InfoDisplay::getCoordinateSpacePoint( const ci::vec2 &point )
{
	ci::vec2 ret;
	auto modelSpacePoint = getInverseMatrix() * vec4( point, 0, 1 );
	ret = vec2( modelSpacePoint.x, modelSpacePoint.y );
	return ret;
}
	
}
