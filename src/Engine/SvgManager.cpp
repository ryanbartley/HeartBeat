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
	
OverlayPageCreators SvgManager::OverlayCreators = {
	make_pair( "overlay", &OverlayPage::create ),
	make_pair( "overlaySection", &OverlaySection::create ),
	make_pair( "overlayPlus", &OverlayPlus::create ),
};
	
ButtonCreators SvgManager::ButtonCreators = {
	make_pair( "dataPageButton", &DataPageButton::create ),
	make_pair( "overlayPageButton", &OverlayPageButton::create ),
	make_pair( "closeButton", &CloseButton::create ),
	make_pair( "navigableButton", &NavigableButton::create )
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
		
		try {
			auto pageAttribs = svgAttribs["pageAttribs"];
		
			initializeOverlayPages( pageAttribs["overlayPages"] );
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
		
		initializeGl();
		
	}
	catch( const JsonTree::ExcChildNotFound &ex ) {
		CI_LOG_E(ex.what());
	}
}
	
void SvgManager::initializeDataPages( const ci::JsonTree &root )
{
	try {
		
		auto connections = root["dataPageConnections"].getChildren();
		for( auto & connectionList : connections ) {
			CI_LOG_V("Adding DataPage Connections " << connectionList );
			DataPageRef last;
			for( auto & connection : connectionList ) {
				auto connectionName = connection.getValue();
				auto next = mPages.find( connectionName );
				if( next != mPages.end() ) {
					auto dataPage = std::dynamic_pointer_cast<DataPage>( next->second );
					if( dataPage ) {
						if( last ) {
							last->connectBack( dataPage );
						}
					
						last = dataPage;
					}
					else {
						CI_LOG_W("Data Page couldn't be cast" << next->second->getGroupName());
					}
				}
				else {
					auto dataPage = DataPage::create( connectionName );
					mPages.insert( make_pair( connectionName, dataPage ) );
					if( last )
						last->connectBack( dataPage );
					
					last = dataPage;
				}
			}
		}
	}
	catch (const JsonTree::ExcChildNotFound &ex ) {
		CI_LOG_E(ex.what());
	}
}
	
void SvgManager::initializeOverlayPages( const ci::JsonTree &root )
{
	try {
		
		auto overlayPageInfos = root.getChildren();
		for( auto & overlayInfo : overlayPageInfos ) {
			auto overlayPageId = overlayInfo.getKey();
			CI_LOG_V("Adding overlayPage " << overlayPageId);
			auto type = overlayInfo["type"].getValue();
			auto found = OverlayCreators.find( type );
			if( found != OverlayCreators.end() ) {
				auto overlayPage = found->second( overlayPageId );
				if( overlayPage->initialize( overlayInfo ) ) {
					mPages.insert( make_pair( overlayPageId, overlayPage ) );
				}
				else {
					CI_LOG_W("Overlay Page didn't initialize " << overlayPage->getGroupName() );
				}
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
	
void SvgManager::initializeButtons( const ci::JsonTree &root )
{
	try {
		
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