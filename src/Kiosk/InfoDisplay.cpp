//
//  InfoDisplay.cpp
//  ProjectionTestApp
//
//  Created by Ryan Bartley on 10/1/14.
//
//

#include "InfoDisplay.h"

#include "JsonManager.h"
#include "InteractionEvents.h"
#include "EventManager.h"
#include "ButtonTypes.h"
#include "Engine.h"
#include "Renderer.h"
#include "cinder/svg/Svg.h"

#include "cinder/Timeline.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/Fbo.h"
#include "cinder/Log.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace heartbeat {
	
InfoDisplay::InfoDisplay( KioskId kioskId, bool shouldRenderSvgs )
: mId( kioskId ), mMasterAlpha( 0.0f ), mFadeTime( 1.0f ), mStatus( Status::HOME_SCREEN ), mIsActivated( false ),
	mCurrentSection( 0 ), mIsHalfSized( Engine::get()->getRenderer()->isHalfSize() ),
	mShouldDrawBoundingBoxes( false ), mRenderWithCairo( shouldRenderSvgs ), mStateChanged( true )
{
}
	
InfoDisplayRef InfoDisplay::create( heartbeat::KioskId kioskId, bool shouldRenderSvgs )
{
	return InfoDisplayRef( new InfoDisplay( kioskId, shouldRenderSvgs ) );
}

void InfoDisplay::update()
{
	
}
	
void InfoDisplay::renderCurrentSceneTex()
{
	if( mStatus == Status::HOME_SCREEN ) {
		renderHomeScreenTex();
	}
	else if( mStatus == Status::DATA_SCREEN ) {
		renderDataScreenTex();
	}
	renderOverlayScreenTex();
}
	
void InfoDisplay::renderHomeScreenTex()
{
	auto home = mPageCache.find("HOME");
	home->second->render();
	
	for( auto & button : mHomeButtons ) {
		button->render();
		if( mShouldDrawBoundingBoxes ) {
			button->renderBoundingBox();
		}
	}
}
	
void InfoDisplay::renderDataScreenTex()
{
	if( ! mOverlay ) {
		for( auto & page : mDataPages ) {
			page->render();
		}
	}
	
	// Variable for animation of this needed.
	gl::ScopedColor scopeColor( ColorA( 1, 1, 1, mOverlay ? .5 : 1.0 ) );
	
	for( auto & button : mDataButtons ) {
		// TODO: Why did I have this?
		// if( button )
			button->render();
			if( mShouldDrawBoundingBoxes ) {
				button->renderBoundingBox();
			}
	}
	
	mLines->render();
}
	
void InfoDisplay::renderOverlayScreenTex()
{
	if( ! mOverlay ) return;

	mOverlay->render();
	
	auto & name = mOverlay->getButtonGroup();
	auto & buttons = mOverlayButtons[name];
	if( buttons.empty() ) {
		CI_LOG_E("Couldn't find overlay buttons: " << name);
	}
	
	for( auto & button : buttons ) {
		button->render();
		if( mShouldDrawBoundingBoxes ) {
			button->renderBoundingBox();
		}
	}
}
	
void InfoDisplay::renderToFbo()
{
	auto fboSize = mPresentationFbo->getSize();
	gl::ScopedFramebuffer	scopeFbo( mPresentationFbo );
	gl::ScopedViewport		scopeView( vec2( 0 ), fboSize );
	gl::ScopedMatrices		scopeMat;
	gl::setMatricesWindow( fboSize );
	
	gl::clearColor( ColorA( 0, 0, 0, 0 ) );
	gl::clear( GL_COLOR_BUFFER_BIT );
	gl::clearColor( ColorA( 0, 0, 0, 1 ) );
	
	//	mBackGround->render();
	
	renderCurrentSceneTex();
#if defined( DEBUG )
	{
		gl::ScopedColor scopeColor( ColorA(1, 0, 0, 1) );
		for( auto & point : mPoints ) {
			gl::drawSolidCircle( point, 30 );
		}
	}
#endif
}
	
void InfoDisplay::renderCurrentSceneSvg( ci::cairo::Context &context )
{
	if( mStatus == Status::HOME_SCREEN ) {
		renderHomeScreenSvg( context );
	}
	else if( mStatus == Status::DATA_SCREEN ) {
		renderDataScreenSvg( context );
	}
	renderOverlayScreenSvg( context );
}

void InfoDisplay::renderHomeScreenSvg( ci::cairo::Context &context )
{
	float alpha = mOverlay ? 0.5f : 1.0f;
	auto home = mPageCache.find("HOME");
	home->second->render( context, alpha );
	
	for( auto & button : mHomeButtons ) {
		if( mOverlayActiveButton ) {
			if( button == mOverlayActiveButton ) {
				button->render( context, 1.0f );
                continue;
			}
		}
        button->render( context, alpha );
	}
}

void InfoDisplay::renderDataScreenSvg( ci::cairo::Context &context )
{
	float alpha = mOverlay ? 0.5f : 1.0f;
	bool check5 = mCurrentSection == 5;
	
	if( ! mOverlay ) {
		for( auto & page : mDataPages ) {
			context.render( *page->getRootGroup() );
		}
		for( auto & button : mDataButtons ) {
			if( check5 ) {
				if( button->getType() != OverlayPageSectionButton::TYPE ) {
					button->render( context, alpha );
				}
			}
			else {
				button->render( context, alpha );
			}
		}
	}
	else {
		for( auto & button : mDataButtons ) {
			if( check5 ) {
				if( button->getType() == OverlayPageSectionButton::TYPE ) {
					break;
				}
			}
			
			if( button == mOverlayActiveButton ) {
				button->render( context, 1.0f );
			}
			else {
				if( button->getType() != NavigableButton::TYPE ) {
					button->render( context, alpha );
				}
			}
		}
	}

	mLines->render( context, alpha );
}

void InfoDisplay::renderOverlayScreenSvg( ci::cairo::Context &context )
{
	if( ! mOverlay ) return;
	mOverlay->render( context, 1.0f );
	
	auto & name = mOverlay->getButtonGroup();
	auto & buttons = mOverlayButtons[name];
	if( buttons.empty() ) {
		CI_LOG_E("Couldn't find overlay buttons: " << name);
	}
	
	for( auto & button : buttons ) {
		button->render( context, 1.0f );
	}
}
	
void InfoDisplay::renderToSvg()
{
	cairo::SurfaceImage surface( mTranslatedPresentRect.getWidth(), mTranslatedPresentRect.getHeight(), true );
	cairo::Context context( surface );
	
#if defined( DEBUG )
	auto & mM = getModelMatrix();
	mCairoMat = MatrixAffine2<float>( mM[0][0], mM[0][1], mM[1][0], mM[1][1], mM[3][0], mM[3][1] );
#endif
	context.setMatrix( mCairoMat );
	
	context.setAntiAlias( 12 );
	renderCurrentSceneSvg( context );
	
	mCairoTex->update( surface.getSurface() );
	mStateChanged = false;
}

	
void InfoDisplay::toggleStatus()
{
	if( mStatus == Status::HOME_SCREEN )
		mStatus = Status::DATA_SCREEN;
	else if ( mStatus == Status::DATA_SCREEN )
		mStatus = Status::HOME_SCREEN;
}

void InfoDisplay::draw()
{
	if( mMasterAlpha <= 0.0f ) return;
	
	if( mRenderWithCairo ) {
		if( mStateChanged ) {
			renderToSvg();
		}
		{
			gl::ScopedColor scopeColor( ColorA( 1, 1, 1, mMasterAlpha) );
			gl::draw( mCairoTex );
		}
        gl::ScopedModelMatrix scopeModel;
		gl::setModelMatrix( getModelMatrix() );
		
#if defined( DEBUG )
        {
            gl::ScopedColor scopeColor( ColorA(1, 0, 0, 1) );
            
            for( auto & point : mPoints ) {
                gl::begin(GL_LINES );
                gl::vertex( point );
                gl::vertex( point.x, point.y + 15 );
                gl::vertex( point );
                gl::vertex( point.x, point.y - 15 );
                gl::vertex( point );
                gl::vertex( point.x + 15, point.y );
                gl::vertex( point );
                gl::vertex( point.x - 15, point.y );
                gl::end();
            }
        }
#endif
		{
			gl::ScopedColor scopeColor( ColorA( 1, 1, 1, .2 ) );
			for( auto touchIt = mPointMap.begin(); touchIt != mPointMap.end();  ) {
                if( touchIt->second.mContained )
                    gl::drawSolidCircle( touchIt->second.getCachedCoordinateSpacePoint(), 45 );
                else {
                    touchIt->second.mNumNotContained++;
                }
                if( touchIt->second.mNumNotContained > 2 ) {
                    touchIt = mPointMap.erase( touchIt );
                }
                else {
                    ++touchIt;
                }
			}
		}
	}
	else {
		renderToFbo();
	
		auto tex = mPresentationFbo->getColorTexture();
	
		gl::ScopedModelMatrix scopeModel;
		gl::setModelMatrix( getModelMatrix() );
		{
			gl::ScopedColor scopeColor( ColorA( 1, 1, 1, mMasterAlpha ) );
			gl::draw( tex );
		}
#if defined( DEBUG )
		{
			gl::ScopedColor scopeColor( ColorA(1, 0, 0, 1) );
			for( auto & point : mPoints ) {
				gl::drawSolidCircle( point, 20 );
			}
		}
		mPoints.clear();
#endif
	}
}
	
void InfoDisplay::activate( bool activate )
{
    CI_LOG_V("activate: " << activate << " mIsActivated; " << mIsActivated);
//	if( mIsActivated == activate ) return;

	auto app = App::get();
	auto shared = shared_from_this();
	
	if( activate ) {
        CI_LOG_V("activating");
		app->timeline().applyPtr( &mMasterAlpha, 1.0f, 1.0f ).easeFn( EaseInCubic() ).finishFn( std::bind( &InfoDisplay::started, shared ) );
        mIsActivated = true;
	}
	else {
		app->timeline().applyPtr( &mMasterAlpha, 0.0f, 5.0f ).easeFn( EaseInCubic() ).finishFn( std::bind( &InfoDisplay::finished, shared ) );
		CI_LOG_V("deactivating");
        mIsActivated = false;
	}
}
	
void InfoDisplay::started()
{
    CI_LOG_V("started");
    mMasterAlpha = 1.0f;
    
}
	
void InfoDisplay::finished()
{
    CI_LOG_V("finished");
	mStatus = Status::HOME_SCREEN;
	mMasterAlpha = 0.0f;
	mDataPages.clear();
	mOverlay = nullptr;
	if( mActivatedButton ) {
		mActivatedButton->setStatus( ButtonStatus::NONACTIVE );
		mActivatedButton = nullptr;
	}
	if( mOverlayActiveButton ) {
		mOverlayActiveButton->setStatus( ButtonStatus::NONACTIVE );
		mOverlayActiveButton = nullptr;
	}
	mCurrentSection = 0;
	CI_LOG_I(getKiosk( mId ) << " Reset the infoDisplay: " << app::getElapsedSeconds());
	renderToSvg();
	mStateChanged = false;
	mPointMap.clear();
#if defined( DEBUG )
	mPoints.clear();
#endif
	CI_LOG_I(getKiosk( mId ) << " Reset the infoDisplay: " << app::getElapsedSeconds());
}
	
void InfoDisplay::addDataPage( DataPageRef &nextPage, AnimateType type )
{
	auto app = App::get();
	
	if( AnimateType::CUT == type ) {
		if( mDataPages.size() > 0 ) {
			mDataPages.pop_front();
		}
	}
	else {
		vec2 incomingOffset;
		vec2 outgoingOffset ;
		if( AnimateType::LEFT_TO_RIGHT == type ) {
			incomingOffset = vec2( -200, 0 );
			outgoingOffset = vec2(  200, 0 );
		}
		else if( AnimateType::RIGHT_TO_LEFT == type ) {
			incomingOffset = vec2(  200, 0 );
			outgoingOffset = vec2( -200, 0 );
		}
		
		nextPage->getCurrentAlpha() = 0.0f;
		auto finalDestination = nextPage->getScenePosition();
		nextPage->getCurrentPosition() = finalDestination + incomingOffset;
		app->timeline().applyPtr( &nextPage->getCurrentAlpha(), 1.0f, 1.0f );
		app->timeline().applyPtr( &nextPage->getCurrentPosition(), finalDestination, 1.0f );
		
		auto establishedPage = mDataPages.front();
		auto establishedPagefinalDest = establishedPage->getScenePosition() + outgoingOffset;
		app->timeline().applyPtr( &establishedPage->getCurrentPosition(), establishedPagefinalDest, 1.0f ).finishFn( std::bind( &InfoDisplay::removeFront, this ) );
		app->timeline().applyPtr( &establishedPage->getCurrentAlpha(), 0.0f, 1.0 );
	}
	
	mDataPages.push_back( nextPage );
}
	
void InfoDisplay::removeFront()
{
	auto front = mDataPages.front();
	mDataPages.pop_front();
}
	
void InfoDisplay::addOverlayPage( OverlayPageRef &page )
{
	mOverlay = page;
}
	
   void InfoDisplay::checkInteraction( const ci::vec2 &point )
{
	std::vector<ButtonRef> *buttonSet;
	switch ( mStatus) {
		case Status::HOME_SCREEN: {
			if( mOverlay ) {
				auto & name = mOverlay->getButtonGroup();
				buttonSet = &mOverlayButtons[name];
				if( buttonSet->empty() ) {
					CI_LOG_E("Couldn't find button set: " << name);
				}
			}
			else {
				buttonSet = &mHomeButtons;
			}
		}
		break;
		case Status::DATA_SCREEN: {
			if( mOverlay ) {
				auto & name = mOverlay->getButtonGroup();
				buttonSet = &mOverlayButtons[name];
				if( buttonSet->empty() ) {
					CI_LOG_E("Couldn't find button set: " << name);
				}
			}
			else {
				buttonSet = &mDataButtons;
			}
		}
		break;
		default:
			CI_LOG_E("Status is incorrect " << static_cast<int>(mStatus));
			return;
			break;
	}
	
#if defined( DEBUG )
    mPoints.push_back( point );
#endif
	
	float minDistance = 100000.0f;
	ButtonRef closestButton;
	for( auto & button : *buttonSet ) {
		auto buttonCenter = button->getCenter();
		auto distance = ci::distance( point, buttonCenter );
		if( distance < minDistance ) {
			closestButton = button;
			minDistance = distance;
		}
		CI_LOG_I("checking button: " << button->getGroupName() << " bounding:  " << button->getRootGroup()->getBoundingBox() << " point: " << point << " distance: " << distance);
	}
	
<<<<<<< HEAD
	if( minDistance < 250 ) {
		cout << "it's this button: " << closestButton->getGroupName() << endl;
=======
	if( minDistance < 150 ) {
		CI_LOG_I("it's this button: " << closestButton->getGroupName() << " and closest is: " << minDistance);
>>>>>>> Removed all instances of cout except for the main full app page
		auto shared = shared_from_this();
		closestButton->changeState( shared );
		mStateChanged = true;
	}
}
    
void InfoDisplay::registerTouchBegan( EventDataRef eventData )
{
//    if( mMasterAlpha != 1.0f ) return;
    
    auto event = std::dynamic_pointer_cast<TouchBeganEvent>( eventData );

    if( ! event ) {
        CI_LOG_E("Couldn't cast touch event from " << eventData->getName() );
        return;
    }
	
	auto eventWorldCoord = event->getWorldCoordinate();
	
	auto twoDimPoint = getCoordinateSpacePoint( eventWorldCoord );
<<<<<<< HEAD
    cout << "About to check this point: " << twoDimPoint << endl;
    if( mTouchRect.contains( twoDimPoint ) ) {
        mPointMap.insert( make_pair( event->getTouchId(), TouchData( eventWorldCoord, twoDimPoint, true ) ) );
=======
	
    CI_LOG_I("About to check this translated point: " << twoDimPoint);
	
    if( mPresentRect.contains( twoDimPoint ) ) {
        mPointMap.insert( make_pair( event->getTouchId(),
									TouchData( eventWorldCoord, twoDimPoint, true ) ) );
>>>>>>> Removed all instances of cout except for the main full app page
    }
	else {
		mPointMap.insert( make_pair( event->getTouchId(),
									TouchData( eventWorldCoord, twoDimPoint, false ) ) );
	}
}

void InfoDisplay::registerTouchMoved( EventDataRef eventData )
{
//    if( mMasterAlpha != 1.0f ) return;
    
	auto event = std::dynamic_pointer_cast<TouchMoveEvent>( eventData );
	if( ! event ) {
		CI_LOG_E("Couldn't cast touch event from " << eventData->getName() );
		return;
	}
<<<<<<< HEAD
    
	cout << "Finding in point map" << endl;
	auto world = event->getWorldCoordinate();
=======
>>>>>>> Removed all instances of cout except for the main full app page
	
	CI_LOG_I("Finding in point map");
	
	auto world = event->getWorldCoordinate();
	auto found = mPointMap.find( event->getTouchId() );
	if( found != mPointMap.end() ) {
		auto twoDimPoint = getCoordinateSpacePoint( world );
		if( ! found->second.mContained ) {
			if( mPresentRect.contains( twoDimPoint ) ) {
				found->second.mContained = true;
				found->second.mCachedCoordinateSpacePoint = twoDimPoint;
				found->second.mHistory.push_back( world );
			}
			else {
				mPointMap.erase( found );
			}
		}
		else {
			found->second.mHistory.push_back( world );
			found->second.mCachedCoordinateSpacePoint = twoDimPoint;
		}
	}
}
	
void InfoDisplay::registerTouchEnded( EventDataRef eventData )
{
//    if( mMasterAlpha != 1.0f ) return;
    
	auto event = std::dynamic_pointer_cast<TouchEndedEvent>( eventData );
	if( ! event ) {
		CI_LOG_E("Couldn't cast touch event from " << eventData->getName() );
		return;
	}
	
	auto found = mPointMap.find( event->getTouchId() );
	if( found != mPointMap.end() ) {
		if( found->second.mContained ) {
			found->second.mHistory.push_back( event->getWorldCoordinate() );
			if( found->second.valid() ) {
				checkInteraction( getCoordinateSpacePoint( found->second.getPointOfInterest() ) );
			}
			mPointMap.erase( found );
		}
	}
}
	
void InfoDisplay::initiaize(const ci::JsonTree &root)
{
	try {
		auto svgManager = SvgManager::get();
        
		
		if( mIsHalfSized ) {
			Renderable::initialize( root["halfSize"]["transformation"] );
		}
		else {
			Renderable::initialize( root["fullSize"]["transformation"] );
		}
		
		mPresentRect = svgManager->getDoc()->getBoundingBox();
		mTouchRect = Rectf( mPresentRect.x1 - 1000.0f, mPresentRect.y1 - 1000.0f, mPresentRect.x2 + 1000.0f, mPresentRect.y2 + 1000.0f );
		auto translated1 = getModelMatrix() * vec4(mPresentRect.x1, mPresentRect.y1, 0, 1);
		auto translated2 = getModelMatrix() * vec4(mPresentRect.x2, mPresentRect.y2, 0, 1);
		CI_LOG_I("Original point1: " << vec4(mPresentRect.x1, mPresentRect.y1, 0, 1) << " Translated point 1: " << translated1 << "\n" << " Original point2: " << vec4(mPresentRect.x2, mPresentRect.y2, 0, 1) << " Translated point 2: " << translated2);
		mTranslatedPresentRect = Rectf( -1000, -1000, 1000, 1000 );
		mTranslatedPresentRect.getCenteredFill( Rectf( translated1.x, translated1.y, translated2.x, translated2.y ), true );
		CI_LOG_I("Translated rect with include: " << mTranslatedPresentRect << " width: " << mTranslatedPresentRect.getWidth() << " height: " << mTranslatedPresentRect.getHeight());

		mCairoTex = gl::Texture2d::create( mTranslatedPresentRect.getWidth(), mTranslatedPresentRect.getHeight() );
		auto & mM = getModelMatrix();
		mCairoMat = MatrixAffine2<float>( mM[0][0], mM[0][1], mM[1][0], mM[1][1], mM[3][0], mM[3][1] );
		
		CI_LOG_I("Context Matrix:\n" << mCairoMat << "\nModel Matrix:\n" << getModelMatrix());
		
		try {
			mMinIndex = root["minIndex"].getValue<int>();
			mMaxIndex = root["maxIndex"].getValue<int>();
		}
		catch ( const JsonTree::ExcChildNotFound &ex ) {
			CI_LOG_W(ex.what() << ", setting defaults of 0 and 1080");
			mMinIndex = 0;
			mMaxIndex = 1080;
		}
		
		if( svgManager ) {
			auto masterButtons = svgManager->getButtons();
			
			try {
				auto structures = JsonManager::get()->getRoot()["kioskManagerAttribs"]["infoDisplays"]["structure"];
				
				try {
					auto pages = structures["staticPages"];
					
					auto backgroundName = pages["background"].getValue();
					auto backgroundFound = svgManager->getPage( backgroundName );
					if( backgroundFound ) {
						mBackGround = backgroundFound->clone();
					}
					else {
						CI_LOG_E("Couldn't find background, " << backgroundName);
					}
					
					auto lineName = pages["lines"].getValue();
					auto lineFound = svgManager->getPage( lineName );
					if( lineFound ) {
						mLines = lineFound->clone();
					}
					else {
						CI_LOG_E("Couldn't find lines, " << lineName);
					}
				}
				catch ( const JsonTree::ExcChildNotFound &ex ) {
					CI_LOG_E(ex.what());
				}
				
				// Attach HomePage Buttons
				try {
					auto home = structures["homePage"];
					auto homeButtons = home["buttons"].getChildren();
					
					for( auto & button : homeButtons ) {
						auto found = masterButtons.find( button.getValue() );
						if( found != masterButtons.end() ) {
							mHomeButtons.push_back( found->second->clone() );
						}
						else {
							CI_LOG_E("Couldn't find homePage Button " << button.getValue() );
						}
					}
					
					auto homePages = home["pages"].getChildren();
					
					for( auto & page : homePages ) {
						auto pageName = page.getValue();
						auto found = mPageCache.find( pageName );
						if( found == mPageCache.end() ) {
							auto page = svgManager->getPage( pageName );
							if( page ) {
								mPageCache.insert(make_pair( pageName, page->clone() ) );
							}
							else {
								CI_LOG_E(pageName << ": not found in the SvgManager");
							}
						}
						else {
							CI_LOG_V(pageName << ": already cached");
						}
					}
				}
				catch ( const JsonTree::ExcChildNotFound &ex ) {
					CI_LOG_E(ex.what());
				}
				
				try {
					auto overlay = structures["overlayPage"];
					auto overlayPages = overlay["pages"].getChildren();
					
					for( auto & page : overlayPages ) {
						auto pageName = page.getValue();
						auto found = mPageCache.find( pageName );
						if( found == mPageCache.end() ) {
							auto page = svgManager->getPage( pageName );
							if( page ) {
								mPageCache.insert(make_pair( pageName, page->clone() ) );
							}
							else {
								CI_LOG_E(pageName << ": not found in the SvgManager");
							}
						}
						else {
							CI_LOG_V(pageName << ": already cached");
						}
					}
					
					auto overlayButtons = overlay["buttons"];
					
					auto studyButtons = overlayButtons["STUDY"].getChildren();
					std::vector<ButtonRef> studyButtonVector;
					for( auto & studyButton : studyButtons ) {
						auto buttonName = studyButton.getValue();
						auto button = svgManager->getButton( buttonName );
						if( button ) {
							studyButtonVector.push_back( button->clone() );
						}
						else {
							CI_LOG_E("Couldn't find button " << buttonName);
						}
					}
					mOverlayButtons.insert( make_pair( "STUDY", std::move(studyButtonVector) ) );
					
					auto referenceButtons = overlayButtons["REFERENCE"].getChildren();
					std::vector<ButtonRef> referenceButtonVector;
					for( auto & referenceButton : referenceButtons ) {
						auto buttonName = referenceButton.getValue();
						auto button = svgManager->getButton( buttonName );
						if( button ) {
							referenceButtonVector.push_back( button->clone() );
						}
						else {
							CI_LOG_E("Couldn't find button " << buttonName);
						}
					}
					mOverlayButtons.insert( make_pair( "REFERENCE", std::move(referenceButtonVector) ) );
					
					auto isiButtons = overlayButtons["ISI"].getChildren();
					NavigableButtonRef next, prev;
					std::vector<ButtonRef> isiButtonVector;
					for( auto & isiButton : isiButtons ) {
						auto buttonName = isiButton.getValue();
						auto button = svgManager->getButton( buttonName );
						if( button ) {
							auto clonedButton = button->clone();
							isiButtonVector.push_back( clonedButton );
							if( clonedButton->getType() == NavigableButton::TYPE ) {
								auto castButton = std::dynamic_pointer_cast<NavigableButton>(clonedButton);
								if( castButton->getNavigationStatus() == NavigableButton::NavigationStatus::NEXT ) {
									next = castButton;
								}
								else if( castButton->getNavigationStatus() == NavigableButton::NavigationStatus::PREV ) {
									prev = castButton;
								}
							}
						}
						else {
							CI_LOG_E("Couldn't find button " << buttonName);
						}
					}
					if( next && prev ) {
						next->setOpposite( prev );
						prev->setOpposite( next );
					}
					else {
						CI_LOG_E("NEXT and PREV weren't able to be opposites, next: " << next << "" << prev );
					}
					mOverlayButtons.insert( make_pair( "ISI", std::move(isiButtonVector) ) );
				}
				catch( const JsonTree::ExcChildNotFound &ex ) {
					CI_LOG_E(ex.what());
				}
				
				// Attach DataPage Buttons
				try {
					auto data = structures["dataPage"];
					auto dataPageButtons = data["buttons"].getChildren();
					
					NavigableButtonRef next, prev;
					for( auto & button : dataPageButtons ) {
						auto found = masterButtons.find( button.getValue() );
						if( found != masterButtons.end() ) {
							auto buttonNode = found->second->clone();
							mDataButtons.push_back( buttonNode );
							if( buttonNode->getType() == NavigableButton::TYPE ) {
								auto navButton = std::dynamic_pointer_cast<NavigableButton>(buttonNode);
								if( navButton->getNavigationStatus() == NavigableButton::NavigationStatus::NEXT ) {
									next = navButton;
								}
								else if( navButton->getNavigationStatus() == NavigableButton::NavigationStatus::PREV ) {
									prev = navButton;
								}
							}
						}
						else {
							CI_LOG_E("Couldn't find dataPage Button " << button.getValue() );
						}
					}
					
					next->setOpposite( prev );
					prev->setOpposite( next );
					
					auto dataPages = data["pages"].getChildren();
					
					for( auto & page : dataPages ) {
						auto pageName = page.getValue();
						auto found = mPageCache.find( pageName );
						if( found == mPageCache.end() ) {
							// This is where we find the first page, e.g. "D1-1"
							auto page = svgManager->getPage( pageName );
							// If that page exists, we'll further process
							if( page ) {
								// Here I make placeholders for the two current and last
								DataPageRef current, last;
								// I also get ready to reverse with this vector
								std::vector<DataPageRef> forwardDataPages;
								// Now that I know page is a thing, I cast it
								current = std::dynamic_pointer_cast<DataPage>(page);
								// Acknowledging the start
								CI_LOG_I("Adding DataPages to InfoDisplay");
								while ( current ) {
									// now I clone current page and set it to the groupName
									auto cached = mPageCache.insert( make_pair( current->getGroupName(), current->clone() ) );
									// this returns a cached page and I cast it again to DataPage
									auto cachedPage = std::dynamic_pointer_cast<DataPage>(cached.first->second);
									// if there's already been a last, I use cached and add that to the next
									if( last ) {
										last->setNext( cachedPage );
									}
									// Add the cachedPage to the back of the "reversing" vector
									forwardDataPages.push_back( cachedPage );
									// make last the cachedPage
									last = cachedPage;
									CI_LOG_I("\t" << current->getGroupName());
									// Since we're interested in the SvgManager map for the clones we make the next the current
									current = current->next();
								}
								DataPageRef next;
								CI_LOG_I("Reversing");
								// Now reverse the above vector to set Previous
								for( auto revIt = forwardDataPages.rbegin(); revIt != forwardDataPages.rend(); ++revIt ) {
									CI_LOG_I("\t" << (*revIt)->getGroupName());
									if( next ) {
										next->setPrev( *revIt );
									}
									next = *revIt;
								}
							}
							else {
								CI_LOG_E(pageName << ": not found in the SvgManager");
							}
						}
						else {
							CI_LOG_V(pageName << ": already cached");
						}
					}
					
					// Initialize all of the navigation pages for the data pages
					for( auto & button : mDataButtons ) {
						if( button->getType() == DataPageButton::TYPE ) {
							auto dataPageButton = std::dynamic_pointer_cast<DataPageButton>(button);
							auto & dataPageName = dataPageButton->getDataPageName();
							auto found = mPageCache.find( dataPageName );
							if( found != mPageCache.end() ) {
								auto dataPage = std::dynamic_pointer_cast<DataPage>(found->second);
								dataPageButton->setDataPage( dataPage );
							}
						}
					}
					
				}
				catch ( const JsonTree::ExcChildNotFound &ex ) {
					CI_LOG_E(ex.what());
				}
				
			}
			catch( const JsonTree::ExcChildNotFound &ex ) {
				CI_LOG_E(ex.what());
			}
		}
		else {
			CI_LOG_E("No svgManager");
		}
	}
	catch ( const JsonTree::ExcChildNotFound &ex ) {
		CI_LOG_W(ex.what());
	}
}
	
}