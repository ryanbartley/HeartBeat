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


#include "cinder/Log.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace heartbeat {
	
static SvgManagerRef sSvgManager = nullptr;
static bool sSvgManagerInitialized = false;
	
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
				auto next = mDatas.find( connectionName );
				if( next != mDatas.end() ) {
					if( last ) {
						last->connectBack( next->second );
					}
					
					last = next->second;
				}
				else {
					auto dataPage = DataPage::create( connectionName );
					auto newPage = mDatas.insert( make_pair( connectionName, dataPage ) );
					if( last )
						last->connectBack( newPage.first->second );
					
					last = newPage.first->second;
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
		
		auto overlayPageIds = root.getChildren();
		for( auto & overlayId : overlayPageIds ) {
			auto overlayPageId = overlayId.getKey();
			CI_LOG_V("Adding overlayPage " << overlayPageId);
			auto overlayPage = OverlayPage::create( overlayPageId );
			mOverlays.insert( make_pair( overlayPageId, overlayPage ) );
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
	for( auto & dataPage : mDatas ) {
		dataPage.second->initializeGl();
	}
	for( auto & overlayPage : mOverlays ) {
		overlayPage.second->initializeGl();
	}
	for( auto & button : mButtons ) {
		button.second->initializeGl();
	}
}
	
}