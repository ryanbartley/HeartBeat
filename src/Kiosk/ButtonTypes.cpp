//
//  ActivatableButton.cpp
//  SvgTest
//
//  Created by Ryan Bartley on 10/23/14.
//
//

#include "ButtonTypes.h"
#include "JsonManager.h"
#include "cinder/Log.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace heartbeat {
	
const ButtonId DataPageButton::TYPE = Hash::HASHER( "DataPageButton" );
const ButtonId OverlayPageButton::TYPE = Hash::HASHER( "OverlayPageButton" );
const ButtonId CloseButton::TYPE = Hash::HASHER( "CloseButton" );
const ButtonId NavigableButton::TYPE = Hash::HASHER( "NavigableButton" );
	
///////////////////////////////////////////////////////////////////////////////////////
// Activatable Button
///////////////////////////////////////////////////////////////////////////////////////
	
ActivatableButton::ActivatableButton( const std::string &buttonId )
	: Button( buttonId ),
	mActive( static_cast<const svg::Group*>( const_cast<svg::Group*>(mGroup)->findByIdContains<svg::Group>( "active" ) ) ),
	mNonActive( static_cast<const svg::Group*>( const_cast<svg::Group*>(mGroup)->findByIdContains<svg::Group>( "default" ) ) )
{
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
		
		auto dataPage = root["dataPage"].getValue();
		
		
	}
	catch ( const JsonTree::ExcChildNotFound & ex) {
		CI_LOG_E(mName << " Couldn't find data page " << ex.what() );
		return false;
	}
	
	return true;
}
	
void DataPageButton::initializeGl()
{
	
}
	
void DataPageButton::changeState( InfoDisplayRef &display )
{
	
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
		
	}
	catch ( const JsonTree::ExcChildNotFound &ex ) {
		CI_LOG_E(mName << " Couldn't find overlay page " << ex.what() );
		return false;
	}
	
	return true;
}

void OverlayPageButton::initializeGl()
{
	
}

void OverlayPageButton::changeState( InfoDisplayRef &display )
{
	
}
	
///////////////////////////////////////////////////////////////////////////////////////
// Pressable Button
///////////////////////////////////////////////////////////////////////////////////////
	
StaticButton::StaticButton( const std::string &buttonId )
: Button( buttonId )
{
}
	
///////////////////////////////////////////////////////////////////////////////////////
// Close Button
///////////////////////////////////////////////////////////////////////////////////////
	
CloseButton::CloseButton( const std::string &buttonId )
: StaticButton( buttonId )
{
}
	
CloseButtonRef CloseButton::create( const std::string &buttonId )
{
	return CloseButtonRef( new CloseButton( buttonId ) );
}
	
bool CloseButton::initialize( const ci::JsonTree &root )
{
	try {
		// need to create the removal of buttons and
	}
	catch ( const JsonTree::ExcChildNotFound &) {
		
		return false;
	}
	
	return true;
}

void CloseButton::initializeGl()
{
	
}

void CloseButton::changeState( InfoDisplayRef &display )
{
	
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
		// need to talk about direction.
	}
	catch ( const JsonTree::ExcChildNotFound &ex ) {
		
		return false;
	}
	
	return true;
}

void NavigableButton::initializeGl()
{
	
}

void NavigableButton::changeState( InfoDisplayRef &display )
{
	
}
	
}