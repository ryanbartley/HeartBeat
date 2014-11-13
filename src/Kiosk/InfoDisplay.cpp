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

#include "cinder/Timeline.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/Fbo.h"
#include "cinder/Log.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace heartbeat {
	
InfoDisplay::InfoDisplay( KioskId kioskId )
: mId( kioskId ), mMasterAlpha( 0.0f ), mFadeTime( 5.0f ), mStatus( Status::HOME_SCREEN ), mIsActivated( false ),
	mCurrentSection( 0 ), mIsHalfSized( Engine::get()->getRenderer()->isHalfSize() ),
	mShouldDrawBoundingBoxes( false ), mRenderWithCairo( false )
{
}
	
InfoDisplayRef InfoDisplay::create( heartbeat::KioskId kioskId )
{
	return InfoDisplayRef( new InfoDisplay( kioskId ) );
}

void InfoDisplay::update()
{
	
}
	
void InfoDisplay::renderCurrentScene()
{
	if( mStatus == Status::HOME_SCREEN ) {
		renderHomeScreen();
	}
	else if( mStatus == Status::DATA_SCREEN ) {
		renderDataScreen();
	}
	renderOverlayScreen();
}
	
void InfoDisplay::renderHomeScreen()
{
	auto home = mPageCache.find("HOME");
	if( mRenderWithCairo ) {
		mContext.render( *home->second->getRootGroup() );
	}
	else {
		home->second->render();
	}
	
	
	for( auto & button : mHomeButtons ) {
		if( mRenderWithCairo ) {
			mContext.render( *home->second->getRootGroup() );
		}
		else {
			button->render();
			if( mShouldDrawBoundingBoxes ) {
				button->renderBoundingBox();
			}
		}
	}
}
	
void InfoDisplay::renderDataScreen()
{
	if( ! mOverlay ) {
		for( auto & page : mDataPages ) {
			if( mRenderWithCairo ) {
				mContext.render( *page->getRootGroup() );
			}
			else {
				page->render();
			}
		}
	}
	
	// Variable for animation of this needed.
	gl::ScopedColor scopeColor( ColorA( 1, 1, 1, mOverlay ? .5 : 1.0 ) );
	
	for( auto & button : mDataButtons ) {
		// TODO: Why did I have this?
		// if( button )
		if( mRenderWithCairo ) {
			auto cast = std::dynamic_pointer_cast<DataPageButton>( button );
			if( cast ) {
				if( cast->getStatus() == ButtonStatus::ACTIVE ) {
					mContext.render( *cast->getActive() );
				}
				else {
					mContext.render( *cast->getNonActive() );
				}
			}
		}
		else {
			button->render();
			if( mShouldDrawBoundingBoxes ) {
				button->renderBoundingBox();
			}
		}
	}
	
	if( mRenderWithCairo ) {
		mContext.render( *mLines->getRootGroup() );
	}
	else {
		mLines->render();
	}
}
	
void InfoDisplay::renderOverlayScreen()
{
	if( ! mOverlay ) return;
	
	if( mRenderWithCairo ) {
		mContext.render( *mOverlay->getRootGroup() );
	}
	else {
		mOverlay->render();
	}
	
	auto & name = mOverlay->getButtonGroup();
	auto & buttons = mOverlayButtons[name];
	if( buttons.empty() ) {
		CI_LOG_E("Couldn't find overlay buttons: " << name);
	}
	
	for( auto & button : buttons ) {
		if( mRenderWithCairo ) {
			mContext.render( *button->getRootGroup() );
		}
		else {
			button->render();
			if( mShouldDrawBoundingBoxes ) {
				button->renderBoundingBox();
			}
		}
	}
}
	
void InfoDisplay::toggleStatus()
{
	if( mStatus == Status::HOME_SCREEN )
		mStatus = Status::DATA_SCREEN;
	else if ( mStatus == Status::DATA_SCREEN )
		mStatus = Status::HOME_SCREEN;
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
	
	renderCurrentScene();
#if defined( DEBUG )
	{
		gl::ScopedColor scopeColor( ColorA(1, 0, 0, 1) );
		for( auto & point : mPoints ) {
			gl::drawSolidCircle( point, 30 );
		}
	}
#endif
}

void InfoDisplay::draw()
{
	if( mMasterAlpha <= 0.0f ) return;
	
	if( mRenderWithCairo ) {
		
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
				cout << "I should be drawing this point" << endl;
				gl::drawSolidCircle( point, 20 );
			}
		}
		mPoints.clear();
#endif
	}
}
	
void InfoDisplay::activate( bool activate )
{
	if( mIsActivated == activate ) return;

	auto app = App::get();
	auto shared = shared_from_this();
	
	if( activate ) {
		app->timeline().applyPtr( &mMasterAlpha, 1.0f, mFadeTime ).easeFn( EaseInCubic() );
		mIsActivated = true;
	}
	else {
		app->timeline().applyPtr( &mMasterAlpha, 0.0f, mFadeTime ).easeFn( EaseInCubic() ).finishFn( std::bind( &InfoDisplay::finished, shared ) );
		mIsActivated = false;
	}
}
	
void InfoDisplay::finished()
{
	mStatus = Status::HOME_SCREEN;
	mIsActivated = false;
	mMasterAlpha = 0.0f;
	mDataPages.clear();
	mOverlay = nullptr;
	if( mActivatedButton ) {
		mActivatedButton->setStatus( ButtonStatus::NONACTIVE );
		mActivatedButton = nullptr;
	}
	if( mOverlayActiveButton ) {
		mOverlayActiveButton->setStatus( ButtonStatus::ACTIVE );
		mOverlayActiveButton = nullptr;
	}
	mCurrentSection = 0;
	cout << getKiosk( mId ) << " Reset the infoDisplay: " << app::getElapsedSeconds() << endl;
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
	
void InfoDisplay::registerTouchBegan( EventDataRef eventData )
{
	auto event = std::dynamic_pointer_cast<TouchBeganEvent>( eventData );
	if( ! event ) {
		CI_LOG_E("Couldn't cast touch event from " << eventData->getName() );
		return;
	}
	
	auto modelSpacePoint = getInverseMatrix() * vec4( event->getWorldCoordinate(), 0, 1 );
	auto twoDimPoint = vec2( modelSpacePoint.x, modelSpacePoint.y );
    cout << "About to check this point: " << twoDimPoint << endl;
	if( mPresentRect.contains( twoDimPoint ) ) {
		mPointMap[event->getTouchId()].push_back( twoDimPoint );
	}
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
	for( auto & button : *buttonSet ) {
		cout << "checking button: " << button->getGroupName() << " bounding:  " << button->getRootGroup()->getBoundingBox() << " point: " << point << endl;
		if( button->contains( point ) ) {
			cout << "it's this button: " << button->getGroupName() << endl;
			auto shared = shared_from_this();
			button->changeState( shared );
			break;
		}
	}
}

void InfoDisplay::registerTouchMoved( EventDataRef eventData )
{
	auto event = std::dynamic_pointer_cast<TouchMoveEvent>( eventData );
	if( ! event ) {
		CI_LOG_E("Couldn't cast touch event from " << eventData->getName() );
		return;
	}
	
	auto found = mPointMap.find( event->getTouchId() );
	if( found != mPointMap.end() ) {
		found->second.push_back( event->getWorldCoordinate() );
	}
}
	
void InfoDisplay::registerTouchEnded( EventDataRef eventData )
{
	auto event = std::dynamic_pointer_cast<TouchMoveEvent>( eventData );
	if( ! event ) {
		CI_LOG_E("Couldn't cast touch event from " << eventData->getName() );
		return;
	}
	
	auto found = mPointMap.find( event->getTouchId() );
	if( found != mPointMap.end() ) {
		auto & pointVec = found->second;
		if( pointVec.size() > 5 ) {
			vec2 accum( 0.0f );
			int i = 0;
			for( auto pointIt = pointVec.rbegin(); pointIt != pointVec.rend(); ++pointIt ) {
				if( i < 5 ) {
					accum += *pointIt;
				}
				else {
					break;
				}
			}
			checkInteraction( accum / 5.0f );
		}
		mPointMap.erase( found );
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
								cout << "Adding DataPages to InfoDisplay" << endl;
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
									cout << "\t" << current->getGroupName() << endl;
									// Since we're interested in the SvgManager map for the clones we make the next the current
									current = current->next();
								}
								DataPageRef next;
								cout << "Reversing" << endl;
								// Now reverse the above vector to set Previous
								for( auto revIt = forwardDataPages.rbegin(); revIt != forwardDataPages.rend(); ++revIt ) {
									cout << "\t" << (*revIt)->getGroupName() << endl;
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