//
//  ActivatableButton.cpp
//  SvgTest
//
//  Created by Ryan Bartley on 10/23/14.
//
//

#include "ButtonTypes.h"
#include "JsonManager.h"
#include "InfoDisplay.h"

#include "cinder/Log.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace heartbeat {
	
const ButtonId DataPageButton::TYPE = Hash::HASHER( "DataPageButton" );
const ButtonId OverlayPageButton::TYPE = Hash::HASHER( "OverlayPageButton" );
const ButtonId ReturnButton::TYPE = Hash::HASHER( "ReturnButton" );
const ButtonId NavigableButton::TYPE = Hash::HASHER( "NavigableButton" );
const ButtonId OverlayPageSectionButton::TYPE = Hash::HASHER("OverlayPageSectionButton");
	
///////////////////////////////////////////////////////////////////////////////////////
// Activatable Button
///////////////////////////////////////////////////////////////////////////////////////
	
ActivatableButton::ActivatableButton( const std::string &buttonId )
	: Button( buttonId ), mStatus( ButtonStatus::NONACTIVE ),
	mActive( static_cast<const svg::Group*>( const_cast<svg::Group*>(mGroup)->findByIdContains<svg::Group>( "active" ) ) ),
	mNonActive( static_cast<const svg::Group*>( const_cast<svg::Group*>(mGroup)->findByIdContains<svg::Group>( "default" ) ) )
{
	if( mActive )
		CI_LOG_V(mName << ": initialized Active correctly");
	else
		CI_LOG_E(mName << ": active wasn't initialized correctly");
	
	if( mNonActive )
		CI_LOG_V(mName << ": initialized NonActive correctly");
	else
		CI_LOG_E(mName << ": nonactive wasn't initialized correctly");
	
	auto rect = mActive->getBoundingBox();
	auto point = vec2( rect.x1, rect.y1 );
	mAbsolutePosition = vec2( point.x - mOffset.x / 2.0f, point.y - mOffset.y / 2.0f );
}
	
void ActivatableButton::initializeGl()
{
	mActiveTex = Node::initializeGl( mActive );
	mNonActiveTex = Node::initializeGl( mNonActive );
}
	
void ActivatableButton::debugRender()
{
	gl::draw( mActiveTex, vec2( 0 ) );
	Rectf bounding = mActive->getBoundingBox();
	{
		gl::ScopedModelMatrix scopeModel;
		
		gl::setModelMatrix( ci::translate( vec3( bounding.getSize().x, 0, 0 ) ) );
		gl::draw( mNonActiveTex, vec2( 0 ) );
	}
	gl::draw( mActiveTex, bounding );
}
	
void ActivatableButton::render()
{
	gl::ScopedModelMatrix scopeModel;
	gl::setModelMatrix( ci::translate( vec3( mAbsolutePosition, 0 ) ) );

	if( mStatus == ButtonStatus::ACTIVE ) {
		gl::draw( mActiveTex );
	}
	else if( mStatus == ButtonStatus::NONACTIVE ) {
		gl::draw( mNonActiveTex );
	}
}
	
void ActivatableButton::render( cairo::Context &context, float alpha )
{
	if( mStatus == ButtonStatus::ACTIVE ) {
		auto style = const_cast<svg::Style*>(&mActive->getStyle());
		style->setOpacity( alpha );
		context.render( *mActive );
	}
	else if( mStatus == ButtonStatus::NONACTIVE ) {
		auto style = const_cast<svg::Style*>(&mNonActive->getStyle());
		style->setOpacity( alpha );
		context.render( *mNonActive );
	}
}
	
void ActivatableButton::changeState( InfoDisplayRef &display )
{
	auto activatedButton = display->getActivatedButton();
	if ( activatedButton ) {
		activatedButton->setStatus( ButtonStatus::NONACTIVE );
	}
	display->setActivatedButton( shared_from_this() );
	mStatus = ButtonStatus::ACTIVE;
}
	
///////////////////////////////////////////////////////////////////////////////////////
// Data Page Button
///////////////////////////////////////////////////////////////////////////////////////
	
DataPageButton::DataPageButton( const std::string &buttonId )
: ActivatableButton( buttonId )
{
}
	
DataPageButtonRef DataPageButton::create( const std::string &buttonId )
{
	return DataPageButtonRef( new DataPageButton( buttonId ) );
}
	
bool DataPageButton::initialize( const ci::JsonTree &root )
{
	try {
		
		mDataPageName = root["dataPage"].getValue();
		mSection = root["section"].getValue<uint32_t>();
		
		auto svgManager = SvgManager::get();
		if( svgManager ) {
			mNavigation = svgManager->getData( mDataPageName );
			cout << "Navigation: " << mNavigation->getGroupName() << endl;
		}
		else {
			CI_LOG_E("Trying to access svgManger but doesn't exist");
		}
	}
	catch ( const JsonTree::ExcChildNotFound & ex) {
		CI_LOG_E(mName << " Couldn't find data page " << ex.what() );
		return false;
	}
	
	return true;
}
	
void DataPageButton::changeState( InfoDisplayRef &infoDisplay )
{
	cout << "DATA_PAGE_BUTTON: " << mName << endl;
	ActivatableButton::changeState( infoDisplay );
	auto buttons = infoDisplay->getDataButtons();
	
	for( auto & button : buttons ) {
		if( button->getType() == NavigableButton::TYPE ) {
			auto cast = std::dynamic_pointer_cast<NavigableButton>( button );
			if( cast ) {
				if( cast->getNavigationStatus() == NavigableButton::NavigationStatus::PREV ) {
					cout << "Changing state of " << cast->getGroupName() << " to none active because it's prev" << endl;
					if( mSection == 1 ) {
						cast->setButtonStatus( ButtonStatus::NONACTIVE );
					}
					else {
						cast->setButtonStatus( ButtonStatus::ACTIVE );
					}
					break;
				}
			}
		}
	}
	infoDisplay->addDataPage( mNavigation, InfoDisplay::AnimateType::CUT );
	infoDisplay->setSection( mSection );
}
	
ButtonRef DataPageButton::clone()
{
	auto button = DataPageButton::create( mGroup->getId() );
	button->mActiveTex = mActiveTex;
	button->mNonActiveTex = mNonActiveTex;
	button->mDataPageName = mDataPageName;
	button->mSection = mSection;
	return button;
}
	
///////////////////////////////////////////////////////////////////////////////////////
// Overlay Page Button
///////////////////////////////////////////////////////////////////////////////////////
	
OverlayPageButton::OverlayPageButton( const std::string &buttonId )
: ActivatableButton( buttonId )
{
}
	
OverlayPageButtonRef OverlayPageButton::create( const std::string &buttonId )
{
	return OverlayPageButtonRef( new OverlayPageButton( buttonId ) );
}
	
bool OverlayPageButton::initialize( const ci::JsonTree &root )
{
	try {
		auto overlayPage = root["overlayPage"].getValue();
		
		auto svg = SvgManager::get();
		if( svg ) {
			auto page = svg->getOverlay( overlayPage );
			if( page ) {
				mNavigation = page;
			}
			else {
				CI_LOG_E("Couldn't find overlayPage: " << overlayPage);
				return false;
			}
		}
		else {
			CI_LOG_E("SvgManger not instantiated");
			return false;
		}
	}
	catch ( const JsonTree::ExcChildNotFound &ex ) {
		CI_LOG_E(mName << " Couldn't find overlay page " << ex.what() );
		return false;
	}
	
	return true;
}

void OverlayPageButton::changeState( InfoDisplayRef &display )
{
	cout << "Overlay_PAGE_BUTTON: " << mName << endl;
	
	mStatus = ButtonStatus::ACTIVE;
	auto overlayButtonMap = display->getOverlayButtons();
	auto found = overlayButtonMap.find( mNavigation->getButtonGroup() );
	if( found != overlayButtonMap.end() ) {
		for( auto & button : found->second ) {
			if( button->getType() == NavigableButton::TYPE ) {
				auto cast = std::dynamic_pointer_cast<NavigableButton>( button );
				if( cast ) {
					if( cast->getNavigationStatus() == NavigableButton::NavigationStatus::PREV ) {
						cout << "Changing state of " << cast->getGroupName() << " to none active because it's prev" << endl;
						cast->setButtonStatus( ButtonStatus::NONACTIVE );
					}
					else if( cast->getNavigationStatus() == NavigableButton::NavigationStatus::NEXT ) {
						cout << "Changing state of " << cast->getGroupName() << " to active because it's next" << endl;
						cast->setButtonStatus( ButtonStatus::ACTIVE );
					}
				}
			}
		}
	}
	
	display->setOverlayActivatedButton( shared_from_this() );
	display->addOverlayPage( mNavigation );
}
	
ButtonRef OverlayPageButton::clone()
{
	auto button = OverlayPageButton::create( mGroup->getId() );
	button->mActiveTex = mActiveTex;
	button->mNonActiveTex = mNonActiveTex;
	button->mNavigation = mNavigation;
	return button;
}
	
///////////////////////////////////////////////////////////////////////////////////////
// Overlay Section Page Button
///////////////////////////////////////////////////////////////////////////////////////
	
OverlayPageSectionButton::OverlayPageSectionButton( const std::string &name )
: OverlayPageButton( name )
{
}

OverlayPageSectionButtonRef OverlayPageSectionButton::create( const std::string &name )
{
	return OverlayPageSectionButtonRef( new OverlayPageSectionButton( name ) );
}
	
ButtonRef OverlayPageSectionButton::clone()
{
	auto ret = OverlayPageSectionButton::create( mGroup->getId() );
	ret->mActiveTex = mActiveTex;
	ret->mNonActiveTex = mNonActiveTex;
	ret->mPrefix = mPrefix;
	return ret;
}
	
bool OverlayPageSectionButton::initialize( const ci::JsonTree &root )
{
	try {
		mPrefix = root["prefix"].getValue();
	}
	catch( const JsonTree::ExcChildNotFound &ex ) {
		CI_LOG_E(ex.what());
		return false;
	}
	return true;
}
	
void OverlayPageSectionButton::changeState( InfoDisplayRef &display )
{
	auto navigateSection = mPrefix + std::to_string( display->getSection() );
	auto pageCache = display->getPageCache();
	auto found = pageCache.find( navigateSection );
	if( found != pageCache.end() ) {
		auto cast = std::dynamic_pointer_cast<OverlayPage>(found->second);
		if( cast ) {
			display->addOverlayPage( cast );
		}
		else {
			CI_LOG_E(navigateSection << " wasn't able to be cast");
		}
		mStatus = ButtonStatus::ACTIVE;
		display->setOverlayActivatedButton( shared_from_this() );
	}
	else {
		CI_LOG_V(navigateSection << " wasn't found");
	}
}
	
///////////////////////////////////////////////////////////////////////////////////////
// Pressable Button
///////////////////////////////////////////////////////////////////////////////////////
	
StaticButton::StaticButton( const std::string &buttonId )
: Button( buttonId )
{
}
	
void StaticButton::initializeGl()
{
	mTexture = Node::initializeGl( mGroup );
}
	
bool StaticButton::initialize( const ci::JsonTree &root )
{
	try {
		
		
	}
	catch ( const JsonTree::ExcChildNotFound &ex ) {
		CI_LOG_E(ex.what());
		return false;
	}
	return true;
}
	
void StaticButton::debugRender()
{
	gl::draw( mTexture, vec2( 0 ) );
}
	
void StaticButton::render()
{
	gl::ScopedModelMatrix scopModel;
	gl::setModelMatrix( ci::translate( vec3( mAbsolutePosition, 0 ) ) );
	
	gl::draw( mTexture );
}
	
void StaticButton::render( ci::cairo::Context &context, float alpha )
{
	auto style = const_cast<svg::Style*>(&mGroup->getStyle());
	style->setOpacity( alpha );
	context.render( *mGroup );
}
	
///////////////////////////////////////////////////////////////////////////////////////
// Close Button
///////////////////////////////////////////////////////////////////////////////////////
	
ReturnButton::ReturnButton( const std::string &buttonId )
: StaticButton( buttonId )
{
}
	
	
ReturnButtonRef ReturnButton::create( const std::string &buttonId )
{
	return ReturnButtonRef( new ReturnButton( buttonId ) );
}

	
bool ReturnButton::initialize( const ci::JsonTree &root )
{
	StaticButton::initialize( root );
	
	try {
		mNavigation = root["navigation"].getValue();
		mActiveButtonName = root["buttonActive"].getValue();
		mSection = root["section"].getValue<uint32_t>();
		
		auto status = root["statusChange"].getValue();
		
		if( status == "home" ) {
			mStatus = ReturnStatus::HOME_PAGE;
		}
		else if( status == "data" ) {
			mStatus = ReturnStatus::DATA_PAGE;
		}
		else if( status == "close" ) {
			mStatus = ReturnStatus::CLOSE_OVERLAY;
		}
		
	}
	catch ( const JsonTree::ExcChildNotFound &ex ) {
		CI_LOG_E(ex.what());
		return false;
	}
	
	return true;
}

void ReturnButton::changeState( InfoDisplayRef &display )
{
	cout << "Return_BUTTON: " << mName << endl;
	switch ( mStatus ) {
		case ReturnStatus::DATA_PAGE: {
			auto pages = display->getPageCache();
			auto foundPage = pages.find( mNavigation );
			if ( foundPage != pages.end() ) {
				auto dataPage = std::dynamic_pointer_cast<DataPage>(foundPage->second);
				if( dataPage ) {
					display->addDataPage( dataPage, InfoDisplay::AnimateType::CUT );
				}
			}
			bool check = mSection == 1;
			DataPageButtonRef foundButton;
			for( auto & button : display->getDataButtons() ) {
				if( button->getGroupName() == mActiveButtonName ) {
					foundButton = std::dynamic_pointer_cast<DataPageButton>( button );
				}
				if( check && button->getType() == NavigableButton::TYPE ) {
					auto cast = std::dynamic_pointer_cast<NavigableButton>( button );
					if( cast ) {
						if( cast->getNavigationStatus() == NavigableButton::NavigationStatus::PREV ) {
							cout << "Changing state of " << cast->getGroupName() << " to none active because it's prev" << endl;
							cast->setButtonStatus( ButtonStatus::NONACTIVE );
						}
					}
				}
			}
			foundButton->setStatus( ButtonStatus::ACTIVE );
			display->setActivatedButton( foundButton );
			display->setStatus( InfoDisplay::Status::DATA_SCREEN );
			display->setSection( mSection );
		}
		break;
		case ReturnStatus::HOME_PAGE: {
			auto activeButton = display->getActivatedButton();
			if( activeButton ) {
				activeButton->setStatus( ButtonStatus::NONACTIVE );
			}
			display->setStatus( InfoDisplay::Status::HOME_SCREEN );
			display->setSection( mSection );
		}
		break;
		case ReturnStatus::CLOSE_OVERLAY: {
			auto null = OverlayPageRef();
			auto overlay = display->getOverlayPage();
			if( overlay->getType() == OverlayPlus::TYPE ) {
				auto cast = std::dynamic_pointer_cast<OverlayPlus>(overlay);
				if( cast ) {
					cast->reset();
				}
			}
			auto overlayButton = display->getOverlayActivatedButton();
			overlayButton->setStatus(ButtonStatus::NONACTIVE);
			display->setOverlayActivatedButton( nullptr );
			display->addOverlayPage( null );
		}
		default:
		break;
	}
	
}
	
ButtonRef ReturnButton::clone()
{
	auto button = ReturnButton::create( mGroup->getId() );
	button->mTexture = mTexture;
	button->mNavigation = mNavigation;
	button->mActiveButtonName = mActiveButtonName;
	button->mSection = mSection;
	button->mStatus = mStatus;
	return button;
}
	
///////////////////////////////////////////////////////////////////////////////////////
// Navigable Button
///////////////////////////////////////////////////////////////////////////////////////
	
NavigableButton::NavigableButton( const std::string &buttonId )
: StaticButton( buttonId )
{
}
	
NavigableButtonRef NavigableButton::create( const std::string &buttonId )
{
	return NavigableButtonRef( new NavigableButton( buttonId ) );
}
	
bool NavigableButton::initialize( const ci::JsonTree &root )
{
	try {
		auto navigation = root["navigation"].getValue();
		
		if( navigation == "next" ) {
			mNavigationStatus = NavigationStatus::NEXT;
		}
		else if( navigation == "prev" ) {
			mNavigationStatus = NavigationStatus::PREV;
		}
		
		mOppositeName = root["opposite"].getValue();
	}
	catch ( const JsonTree::ExcChildNotFound &ex ) {
		CI_LOG_E(ex.what());
		return false;
	}
	
	return true;
}

void NavigableButton::changeState( InfoDisplayRef &display )
{
	cout << "Navigable_BUTTON: " << mName << endl;
	
	if( mButtonStatus == ButtonStatus::NONACTIVE ) return;
	
	auto currentPages = display->getCurrentPages();
	auto overlay = display->getOverlayPage();
	if( overlay ) {
		if( overlay->getType() == OverlayPlus::TYPE ) {
			auto plus = std::dynamic_pointer_cast<OverlayPlus>(overlay);
			if( mNavigationStatus == NavigationStatus::NEXT ) {
				if( ! plus->nextIndex() ) {
					mButtonStatus = ButtonStatus::NONACTIVE;
				}
			}
			else if( mNavigationStatus == NavigationStatus::PREV ) {
				if( ! plus->prevIndex() ) {
					mButtonStatus = ButtonStatus::NONACTIVE;
				}
			}
			if( mOpposite ) {
				if ( mOpposite->getButtonStatus() == ButtonStatus::NONACTIVE ) {
					mOpposite->setButtonStatus( ButtonStatus::ACTIVE );
				}
			}
			else {
				CI_LOG_E(mName << ": has No Opposite");
			}
		}
	}
	else if( currentPages.size() > 0 ) {
		auto dataPage = currentPages.back();
		if( dataPage ) {
			if( mNavigationStatus == NavigationStatus::NEXT ) {
				auto next = dataPage->next();
				// TODO: THis is a hack that I can't figure out.
				cout << getGroupName() << " is going to the next page " << next << endl;
				if( next ) {
					cout << "There's a next datapage called " << next->getGroupName() << endl;
					display->addDataPage( next, InfoDisplay::AnimateType::CUT );
					cout << "Section of display: " << display->getSection() << " Next's section: " << next->getSection() << endl;
					if( display->getSection() != next->getSection() ) {
						auto buttons = display->getDataButtons();
						for( auto & button : buttons ) {
							cout << "Checking: " << button->getGroupName() << endl;
							if( button->getType() == DataPageButton::TYPE ) {
								auto cast = std::dynamic_pointer_cast<DataPageButton>(button);
								cout << "Section: " << cast->getSection() << " Display: " << display->getSection() << endl;
								if( cast && cast->getSection() == display->getSection() + 1 ) {
									cout << "Setting activated button " << endl;
									display->getActivatedButton()->setStatus( ButtonStatus::NONACTIVE );
									cast->setStatus( ButtonStatus::ACTIVE );
									display->setActivatedButton( cast );
									display->setSection( cast->getSection() );
									break;
								}
							}
						}
					}
					auto another = next->next();
					if( ! another ) {
						mButtonStatus = ButtonStatus::NONACTIVE;
					}
				}
				else {
					cout << "There wasn't a next button resetting to nonActive" << endl;
					mButtonStatus = ButtonStatus::NONACTIVE;
				}
				if( mOpposite->getButtonStatus() == ButtonStatus::NONACTIVE ) {
					mOpposite->setButtonStatus( ButtonStatus::ACTIVE );
				}
			}
			else if( mNavigationStatus == NavigationStatus::PREV ) {
				auto prev = dataPage->prev();
				cout << getGroupName() << " is going to the next page " << prev << endl;
				if( prev ) {
					cout << "There's a next button called " << prev->getGroupName() << endl;
					display->addDataPage( prev, InfoDisplay::AnimateType::CUT );
					cout << "Section of display: " << display->getSection() << " Next's section: " << prev->getSection() << endl;
					if( display->getSection() != prev->getSection() ) {
						auto buttons = display->getDataButtons();
						for( auto & button : buttons ) {
							cout << "Checking: " << button->getGroupName() << endl;
							if( button->getType() == DataPageButton::TYPE ) {
								auto cast = std::dynamic_pointer_cast<DataPageButton>(button);
								cout << "Section: " << cast->getSection() << " Display: " << display->getSection() << endl;
								if( cast && cast->getSection() == display->getSection() - 1 ) {
									cout << "Setting activated button " << endl;
									display->getActivatedButton()->setStatus( ButtonStatus::NONACTIVE );
									cast->setStatus( ButtonStatus::ACTIVE );
									display->setActivatedButton( cast );
									display->setSection( cast->getSection() );
									break;
								}
							}
						}
					}
					auto another = prev->prev();
					cout << "Going to see if it should be inactive" << another << endl;
					if( ! another ) {
						mButtonStatus = ButtonStatus::NONACTIVE;
					}
				}
				else {
					cout << "There wasn't a next button resetting to nonActive" << endl;
					mButtonStatus = ButtonStatus::NONACTIVE;
				}
				if( mOpposite->getButtonStatus() == ButtonStatus::NONACTIVE ) {
					mOpposite->setButtonStatus( ButtonStatus::ACTIVE );
				}
			}
		}
	}
}
	
void NavigableButton::render()
{
	if( mButtonStatus == ButtonStatus::NONACTIVE ) return;
	
	gl::ScopedModelMatrix scopModel;
	gl::setModelMatrix( ci::translate( vec3( mAbsolutePosition, 0 ) ) );
	
	gl::draw( mTexture );
}
	
void NavigableButton::render( ci::cairo::Context &context, float alpha )
{
	if( mButtonStatus == ButtonStatus::NONACTIVE ) return;
	
	context.render( *mGroup );
}
	
ButtonRef NavigableButton::clone()
{
	auto button = NavigableButton::create( mGroup->getId() );
	button->mTexture = mTexture;
	button->mNavigationStatus = mNavigationStatus;
	button->mOppositeName = mOppositeName;
	return button;
}
	
}