//
//  SvgManager.cpp
//  ProjectionTestApp
//
//  Created by Ryan Bartley on 10/1/14.
//
//

#include "SvgManager.h"

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

SvgManager::SvgManager( const std::string &fileName )
: mFileName( fileName )
{
}

SvgManager::~SvgManager()
{
	CI_LOG_V("SvgManagerDestroyed");
}

SvgManagerRef SvgManager::create( const std::string &fileName )
{
	sSvgManager = SvgManagerRef( new SvgManager( fileName ) );
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
	
}