//
//  Pages.cpp
//  SvgTest
//
//  Created by Ryan Bartley on 10/24/14.
//
//

#include "PageTypes.h"
#include "JsonManager.h"
#include "ButtonTypes.h"
#include "cinder/Log.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace heartbeat {
	
const PageId DataPage::TYPE = Hash::HASHER("DataPage");
const PageId OverlayPage::TYPE = Hash::HASHER("OverlayPage");
const PageId OverlaySection::TYPE = Hash::HASHER("OverlaySection");
const PageId OverlayPlus::TYPE = Hash::HASHER("OverlayPlus");
const PageId SingleTexturePage::TYPE = Hash::HASHER("SingleTexturePage");
	
SingleTexturePageRef SingleTexturePage::create( const std::string &name )
{
	return SingleTexturePageRef( new SingleTexturePage( name ) );
}
	
SingleTexturePage::SingleTexturePage( const std::string &name )
: Page( name )
{
	
}
	
void SingleTexturePage::render()
{
	gl::ScopedModelMatrix scopeModel;
	gl::setModelMatrix( ci::translate( vec3( mAbsolutePosition, 0 ) ) );
	
	gl::draw( mTexture );
}
	
void SingleTexturePage::render( ci::cairo::Context &context, float alpha )
{
	auto style = const_cast<svg::Style*>(&mGroup->getStyle());
	style->setOpacity( alpha );
	context.render( *mGroup );
}
	
PageRef SingleTexturePage::clone()
{
	auto ret = SingleTexturePage::create( mGroup->getId() );
	ret->mTexture = mTexture;
	return ret;
}
	

void SingleTexturePage::debugRender()
{
	gl::draw( mTexture, -( mOffset / 2.0f ) );
}
	
void SingleTexturePage::initializeGl()
{
	mTexture = Node::initializeGl( mGroup );
}
	
bool SingleTexturePage::initialize( const ci::JsonTree &root )
{
	try {
		
	}
	catch ( const JsonTree::ExcChildNotFound &ex ) {
		CI_LOG_E(ex.what());
		return false;
	}
	return true;
}
	
/////////////////////////////////////////////////////////////////////////
/// Data Page
/////////////////////////////////////////////////////////////////////////
	
DataPage::DataPage( const std::string &name )
: SingleTexturePage( name )
{
}
	
DataPageRef DataPage::create( const std::string &name )
{
	return DataPageRef( new DataPage( name ) );
}

bool DataPage::initialize( const ci::JsonTree &root )
{
	try {
		mButton = root["button"].getValue();
	}
	catch( const JsonTree::ExcChildNotFound &ex ) {
		CI_LOG_E(ex.what());
	}
	return true;
}
	
PageRef DataPage::clone()
{
	auto page = DataPage::create( mGroup->getId() );
	page->mButton = mButton;
	page->mTexture = mTexture;
	page->mSection = mSection;
	return page;
}
	
/////////////////////////////////////////////////////////////////////////
/// Overlay Page
/////////////////////////////////////////////////////////////////////////
	
OverlayPage::OverlayPage( const std::string &name )
: SingleTexturePage( name )
{
}
	
OverlayPageRef OverlayPage::create( const std::string &name )
{
	return OverlayPageRef( new OverlayPage( name ) );
}
	
bool OverlayPage::initialize( const ci::JsonTree &root )
{
	
	try {
		mButtonGroup = root["button"].getValue();
	}
	catch ( const JsonTree::ExcChildNotFound &ex ) {
		CI_LOG_E(ex.what());
		return false;
	}
	return true;
}
	
PageRef OverlayPage::clone()
{
	auto page = OverlayPage::create( mGroup->getId() );
	page->mButton = mButton;
	page->mTexture = mTexture;
	page->mButtonGroup = mButtonGroup;
	return page;
}
	
/////////////////////////////////////////////////////////////////////////
/// Overlay Section
/////////////////////////////////////////////////////////////////////////
	
OverlaySection::OverlaySection( const std::string &name )
: OverlayPage( name )
{
}

OverlaySectionRef OverlaySection::create( const std::string &name )
{
	return OverlaySectionRef( new OverlaySection( name ) );
}

bool OverlaySection::initialize( const ci::JsonTree &root )
{
	OverlayPage::initialize( root );
	try {
		mSection = root["section"].getValue<uint32_t>();
		mButtonGroup = root["button"].getValue();
	}
	catch ( const JsonTree::ExcChildNotFound &ex ) {
		CI_LOG_W("OverlaySection " << ex.what() );
		return false;
	}
	return true;
}
	
PageRef OverlaySection::clone()
{
	auto page = OverlaySection::create( mGroup->getId() );
	page->mTexture = mTexture;
	page->mButton = mButton;
	page->mSection = mSection;
	page->mButtonGroup = mButtonGroup;
	return page;
}
	
/////////////////////////////////////////////////////////////////////////
/// Overlay Plus
/////////////////////////////////////////////////////////////////////////

OverlayPlus::OverlayPlus( const std::string &name )
: OverlayPage( name ), mCurrentIndex( 0 )
{
}

OverlayPlusRef OverlayPlus::create( const std::string &name )
{
	return OverlayPlusRef( new OverlayPlus( name ) );
}
	
void OverlayPlus::debugRender()
{
	auto texIt = mTextures.begin();
	auto overIt = mOverlays.begin();
	auto texEnd = mTextures.end();
	auto overEnd = mOverlays.end();
	auto currentX = 0;
	for( ; texIt != texEnd && overIt != overEnd; ++texIt, ++overIt ) {
		auto size = (*overIt)->getBoundingBox().getSize() / 4.0f;
		auto point = vec2( currentX, 0 );
		gl::draw( *texIt, Rectf( point, point + size ) );
		currentX += size.x;
	}
}

bool OverlayPlus::initialize( const ci::JsonTree &root )
{
	OverlayPage::initialize( root );
	
	try {
		auto groups = root["images"].getChildren();
		auto nonConstGroup = SvgManager::get()->getDoc();
		
		CI_LOG_I("Group in Overlay");
		for( auto & groupName : groups ) {
			CI_LOG_I(groupName.getValue());
			mOverlays.push_back(  nonConstGroup->findByIdContains<svg::Group>( groupName.getValue() ) );
		}
	}
	catch( const JsonTree::ExcChildNotFound &ex ) {
		CI_LOG_E(ex.what());
		return false;
	}
	
	try {
		mButtonGroup = root["button"].getValue();
	}
	catch( const JsonTree::ExcChildNotFound &ex ) {
		CI_LOG_E(ex.what());
		return false;
	}
	
	return true;
}
	
void OverlayPlus::initializeGl()
{
	for( auto & group : mOverlays ) {
		mTextures.push_back( Node::initializeGl( group ) );
	}
}
	
PageRef OverlayPlus::clone()
{
	auto page = OverlayPlus::create( mGroup->getId() );
	page->mTextures = mTextures;
	page->mOverlays = mOverlays;
	page->mButtonGroup = mButtonGroup;
	return page;
}
	
void OverlayPlus::render()
{
	{
		gl::ScopedModelMatrix scopeModel;
		gl::setModelMatrix( ci::translate( vec3( mCurrentPosition, 0.0f ) ) );
		
		gl::draw( mTextures[mCurrentIndex] );
	}
}
	
void OverlayPlus::render( ci::cairo::Context &context, float alpha )
{
	context.render( *mOverlays[mCurrentIndex] );
}

}