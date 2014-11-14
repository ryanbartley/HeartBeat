//
//  SvgManager.cpp
//  ProjectionTestApp
//
//  Created by Ryan Bartley on 10/1/14.
//
//

#include "SvgManager.h"

#include "JsonManager.h"
#include "PageTypes.h"
#include "ButtonTypes.h"

#include "cinder/Log.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace heartbeat {
	
static SvgManagerRef sSvgManager = nullptr;
static bool sSvgManagerInitialized = false;
	
PageCreators SvgManager::OverlayCreators = {
	make_pair( "overlay", &OverlayPage::create ),
	make_pair( "overlaySection", &OverlaySection::create ),
	make_pair( "overlayPlus", &OverlayPlus::create )
};
	
ButtonCreators SvgManager::ButtonCreators = {
	make_pair( "dataPageButton", &DataPageButton::create ),
	make_pair( "overlayPageButton", &OverlayPageButton::create ),
	make_pair( "returnButton", &ReturnButton::create ),
	make_pair( "navigableButton", &NavigableButton::create ),
	make_pair( "overlayPageSectionButton", &OverlayPageSectionButton::create )
};
	
SvgManagerRef	SvgManager::get() {
	if( sSvgManagerInitialized ) {
		return sSvgManager;
	}
	
	CI_LOG_W("Trying to access SvgManager after it's already been destroyed()");
	return nullptr;
}

SvgManager::SvgManager()
{
	
}

SvgManager::~SvgManager()
{
	CI_LOG_V("SvgManagerDestroyed");
}

SvgManagerRef SvgManager::create()
{
	sSvgManager = SvgManagerRef( new SvgManager );
	sSvgManagerInitialized = true;
	CI_LOG_V("Created SvgManager");
	return SvgManager::get();
}

void SvgManager::destroy()
{
	CI_LOG_V("Destroying SvgManager");
	sSvgManager = nullptr;
	sSvgManagerInitialized = false;
}
	
void SvgManager::initialize()
{
	try {
		auto svgAttribs = JsonManager::get()->getRoot()["svgAttribs"];
	
		try {
			auto fileName = svgAttribs["fileName"].getValue();
			mDoc = svg::Doc::create( getFileContents( fileName ) );
		}
		catch ( const JsonTree::ExcChildNotFound &ex ) {
			CI_LOG_E("FileName couldn't be found, using default");
			mDoc = svg::Doc::create( getFileContents( "ACTEMRA_PondCharts_102114.svg" ) );
		}
		
		auto pageAttribs = svgAttribs["pageAttribs"];
		
		try {
			preInitializeOverlayPages( pageAttribs["overlayPages"] );
			initializeDataPages( pageAttribs["dataPages"] );
		}
		catch( const JsonTree::ExcChildNotFound &ex ) {
			CI_LOG_E(ex.what());
		}
		
		try {
			initializeButtons( svgAttribs["buttonAttribs"] );
		}
		catch( const JsonTree::ExcChildNotFound &ex ) {
			CI_LOG_E(ex.what());
		}
		
		try {
			postInitializeOverlayPages( pageAttribs["overlayPages"] );
		}
		catch( const JsonTree::ExcChildNotFound &ex ) {
			CI_LOG_E(ex.what());
		}
		
		try {
			initializeStaticPages( pageAttribs["staticPages"] );
		} catch ( const JsonTree::ExcChildNotFound &ex ) {
			CI_LOG_E(ex.what());
		}
	
		initializeGl();
		
	}
	catch( const JsonTree::ExcChildNotFound &ex ) {
		CI_LOG_E(ex.what());
	}
}
	
void SvgManager::initializeDataPages( const ci::JsonTree &root )
{
	try {
		auto connections = root.getChildren();
		
		std::vector<DataPageRef> mDataPages;
		DataPageRef prev;
		
		int i = 1;
		for( auto & connectionList : connections ) {
			for( auto & connection : connectionList ) {
				auto connectedPages = connection.getChildren();
				for( auto & page : connectedPages ) {
					auto pageName = page.getValue();
					auto dataPage = DataPage::create( pageName );
					mPages.insert( make_pair( pageName, dataPage ) );
					dataPage->setSection( i );
					if( dataPage->getGroupName() != "HOME" ) {
						mDataPages.push_back( dataPage );
						if( prev ) {
							prev->setNext( dataPage );
						}
					
						prev = dataPage;
					}
				}
				++i;
			}
		}
		DataPageRef next;
		for( auto dataIt = mDataPages.rbegin(); dataIt != mDataPages.rend(); ++dataIt ) {
			if( next ) {
				next->setPrev( *dataIt );
			}
			
			next = *dataIt;
		}
	}
	catch (const JsonTree::ExcChildNotFound &ex ) {
		CI_LOG_E(ex.what());
	}
}
	
ButtonRef SvgManager::getButton( const std::string &name )
{
	auto found = mButtons.find( name );
	if( found != mButtons.end() ) {
		return found->second;
	}
	return ButtonRef();
}

DataPageRef SvgManager::getData( const std::string &name )
{
	auto found = mPages.find( name );
	if( found != mPages.end() ) {
		auto cast = std::dynamic_pointer_cast<DataPage>( found->second );
		if( cast )
			return cast;
	}
	return DataPageRef();
}

OverlayPageRef SvgManager::getOverlay( const std::string &name )
{
	auto found = mPages.find( name );
	if( found != mPages.end() ) {
		auto cast = std::dynamic_pointer_cast<OverlayPage>( found->second );
		if( cast )
			return cast;
	}
	return OverlayPageRef();
}
	
PageRef SvgManager::getPage( const std::string &name )
{
	auto found = mPages.find( name );
	if( found != mPages.end() ) {
		return found->second;
	}
	return PageRef();
}
	
void SvgManager::preInitializeOverlayPages( const ci::JsonTree &root )
{
	try {
		auto overlayPageInfos = root.getChildren();
		
		for( auto & overlayInfo : overlayPageInfos ) {
			auto overlayPageId = overlayInfo.getKey();
			
			auto type = overlayInfo["type"].getValue();
			auto found = OverlayCreators.find( type );
			
			if( found != OverlayCreators.end() ) {
				auto overlayPage = found->second( overlayPageId );
				mPages.insert( make_pair( overlayPageId, overlayPage ) );
			}
			else {
				CI_LOG_W("Creator wasn't found, " << type);
			}
			
		}
	}
	catch ( const JsonTree::ExcChildNotFound &ex ) {
		CI_LOG_E(ex.what());
	}
}
	
void SvgManager::postInitializeOverlayPages( const ci::JsonTree &root )
{
	try {
		auto overlayPageInfos = root.getChildren();
		
		for( auto & overlayInfo : overlayPageInfos ) {
			auto overlayPageId = overlayInfo.getKey();
			
			auto found = mPages.find( overlayPageId );
			if( found != mPages.end() ) {
				if( found->second->initialize( overlayInfo ) ) {
					CI_LOG_V(overlayPageId << ": Initialized correctly");
				}
				else {
					CI_LOG_E(overlayPageId << ": wasn't initialized correctly");
				}
			}
			else {
				CI_LOG_W("OverlayPage wasn't found, " << overlayPageId);
			}
			
		}
	}
	catch ( const JsonTree::ExcChildNotFound &ex ) {
		CI_LOG_E(ex.what());
	}
}
	
void SvgManager::initializeStaticPages( const ci::JsonTree &root )
{
	try {
		auto staticPages = root["pages"].getChildren();
		
		for( auto & staticPage : staticPages ) {
			auto staticPageId = staticPage.getValue();
			auto page = SingleTexturePage::create( staticPageId );
			if( page->initialize( JsonTree() ) ) {
				mPages.insert( make_pair( staticPageId, page ) );
			}
			else {
				CI_LOG_E("Error initializing " << staticPageId);
			}
		}
	}
	catch ( const JsonTree::ExcChildNotFound &ex ) {
		CI_LOG_E(ex.what());
	}
}
	
void SvgManager::initializeButtons( const ci::JsonTree &root )
{
	try {
		auto buttons = root.getChildren();
		
		for( auto & button : buttons ) {
			auto buttonName = button.getKey();
			auto type = button["type"].getValue();
			auto creator = ButtonCreators.find( type );
			if( creator != ButtonCreators.end() ) {
				auto tempButton = creator->second( buttonName );
				if( tempButton->initialize( button ) ) {
					CI_LOG_V(tempButton->getGroupName() << ": initialized correctly");
					mButtons.insert( make_pair( buttonName, tempButton ) );
				}
				else {
					CI_LOG_E("Error Initializing " << buttonName );
				}
			}
			else {
				CI_LOG_E("Button Creator doesn't exist " << type);
			}
		}
	}
	catch( const JsonTree::ExcChildNotFound &ex ) {
		CI_LOG_E(ex.what());
	}
}
	
void SvgManager::initializeGl()
{
	for( auto & page : mPages ) {
		page.second->initializeGl();
	}
	for( auto & button : mButtons ) {
		button.second->initializeGl();
	}
}
	
}