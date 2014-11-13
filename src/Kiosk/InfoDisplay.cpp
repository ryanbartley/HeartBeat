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
	
InfoDisplay::InfoDisplay( KioskId kioskId, bool shouldRenderSvgs )
: mId( kioskId ), mMasterAlpha( 0.0f ), mFadeTime( 5.0f ), mStatus( Status::HOME_SCREEN ), mIsActivated( false ),
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
	auto home = mPageCache.find("HOME");
	context.render( *home->second->getRootGroup() );
	
	
	for( auto & button : mHomeButtons ) {
        if( button->getType() == OverlayPageButton::TYPE ) {
            auto cast = std::dynamic_pointer_cast<OverlayPageButton>( button );
            if( cast ) {
                if( cast->getStatus() == ButtonStatus::ACTIVE ) {
                    context.render( *cast->getActive() );
                }
                else if( cast->getStatus() == ButtonStatus::NONACTIVE ) {
                    context.render( *cast->getNonActive() );
                }
            }
        }
        else {
            context.render( *button->getRootGroup() );
        }
    }
}

void InfoDisplay::renderDataScreenSvg( ci::cairo::Context &context )
{
	if( ! mOverlay ) {
		for( auto & page : mDataPages ) {
			context.render( *page->getRootGroup() );
		}
	}
	
	// Variable for animation of this needed.
	gl::ScopedColor scopeColor( ColorA( 1, 1, 1, mOverlay ? .5 : 1.0 ) );
	
	for( auto & button : mDataButtons ) {
		// TODO: Why did I have this?
		// if( button )
            auto cast = std::dynamic_pointer_cast<ActivatableButton>( button );
            if( cast ) {
                if( cast->getStatus() == ButtonStatus::ACTIVE ) {
                    context.render( *cast->getActive() );
                }
                else {
                    context.render( *cast->getNonActive() );
                }
            }
            else {
                auto doubleCast = std::dynamic_pointer_cast<NavigableButton>( button );
                if( doubleCast ) {
                    if( doubleCast->getButtonStatus() == ButtonStatus::ACTIVE ) {
                        context.render( *doubleCast->getRootGroup() );
                    }
                }
                else {
                    context.render( *button->getRootGroup() );
                }
            }
    }
	
	context.render( *mLines->getRootGroup() );
}

void InfoDisplay::renderOverlayScreenSvg( ci::cairo::Context &context )
{
	if( ! mOverlay ) return;
	auto cast = std::dynamic_pointer_cast<OverlayPlus>(mOverlay);
    if( cast ) {
        context.render( *cast->getCurrentGroup() );
    }
    else {
        context.render( *mOverlay->getRootGroup() );
    }
	
	auto & name = mOverlay->getButtonGroup();
	auto & buttons = mOverlayButtons[name];
	if( buttons.empty() ) {
		CI_LOG_E("Couldn't find overlay buttons: " << name);
	}
	
	for( auto & button : buttons ) {
        if( button->getType() == NavigableButton::TYPE ) {
            auto cast = std::dynamic_pointer_cast<NavigableButton>(button);
            if( cast ) {
                if( cast->getButtonStatus() == ButtonStatus::ACTIVE ) {
                    context.render( *cast->getRootGroup() );
                }
            }
        }
        else {
            context.render( *button->getRootGroup() );
        }
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
#if defined( DEBUG )
	{
		gl::ScopedColor scopeColor( ColorA(1, 0, 0, 1) );
		for( auto & point : mPoints ) {
			gl::drawSolidCircle( point, 30 );
		}
	}
#endif
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
		gl::draw( mCairoTex );
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
			if( mRenderWithCairo ) {
				mStateChanged = true;
			}
			break;
		}
	}
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
        std::deque<ci::vec2> vec({vec2(event->getWorldCoordinate())});
        mPointMap.insert( make_pair( event->getTouchId(), vec ) );
		checkInteraction( twoDimPoint );
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
		if( found->second.size() > 20 ) {
			found->second.pop_front();
		}
	}
}
	
void InfoDisplay::registerTouchEnded( EventDataRef eventData )
{
	auto event = std::dynamic_pointer_cast<TouchEndedEvent>( eventData );
	if( ! event ) {
		CI_LOG_E("Couldn't cast touch event from " << eventData->getName() );
		return;
	}
	
	auto found = mPointMap.find( event->getTouchId() );
	if( found != mPointMap.end() ) {
		auto & pointVec = found->second;
		if( pointVec.size() > 5 ) {
			vec2 accum( 0.0f );
            auto middle = pointVec.size() / 2;
            auto index = middle - 2;
			while( index < middle + 2 ) {
                accum += pointVec[index++];
            }
            auto average = accum / 5.0f;
			checkInteraction( vec2( getInverseMatrix() * vec4( average.x, average.y, 0, 1 ) ) );
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
		
		auto translated1 = getModelMatrix() * vec4(mPresentRect.x1, mPresentRect.y1, 0, 1);
		auto translated2 = getModelMatrix() * vec4(mPresentRect.x2, mPresentRect.y2, 0, 1);
		cout << "Original point1: " << vec4(mPresentRect.x1, mPresentRect.y1, 0, 1) << " Translated point 1: " << translated1 << endl << " Original point2: " << vec4(mPresentRect.x2, mPresentRect.y2, 0, 1) << " Translated point 2: " << translated2 << endl;
		mTranslatedPresentRect = Rectf( -1000, -1000, 1000, 1000 );
		mTranslatedPresentRect.getCenteredFill( Rectf( translated1.x, translated1.y, translated2.x, translated2.y ), true );
		cout << "Translated rect with include: " << mTranslatedPresentRect << " width: " << mTranslatedPresentRect.getWidth() << " height: " << mTranslatedPresentRect.getHeight() << endl;

		mCairoTex = gl::Texture2d::create( mTranslatedPresentRect.getWidth(), mTranslatedPresentRect.getHeight() );
		auto & mM = getModelMatrix();
		mCairoMat = MatrixAffine2<float>( mM[0][0], mM[0][1], mM[1][0], mM[1][1], mM[3][0], mM[3][1] );
		
		cout << "Context Matrix: " << endl << mCairoMat << " Model Matrix: " << endl << getModelMatrix() << endl;
		
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