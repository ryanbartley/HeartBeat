//
//  JsonManager.cpp
//  ProjectionTestApp
//
//  Created by Ryan Bartley on 9/29/14.
//
//

#include "JsonManager.h"
#include "cinder/Log.h"

using namespace ci;
using namespace ci::app;

namespace heartbeat {
	
static JsonManagerRef sJsonManager = nullptr;
static bool sJsonManagerInitialized = false;
	
JsonManagerRef	JsonManager::get() {
	if( sJsonManagerInitialized ) {
		return sJsonManager;
	}
	
	CI_LOG_W("Trying to access JsonManager after it's already been destroyed()");
	return nullptr;
}
	
JsonManager::JsonManager( const std::string &fileName, const DataSourceRef &fileContents )
: mFileName( fileName ), mRoot( fileContents )
{
}
	
JsonManager::~JsonManager()
{
	CI_LOG_V("JsonManagerDestroyed");
}
	
JsonManagerRef JsonManager::create( const std::string &fileName )
{
	auto contents = getFileContents( fileName );
	sJsonManager = JsonManagerRef( new JsonManager( fileName, contents ) );
	sJsonManagerInitialized = true;
	CI_LOG_V("Created JsonManager");
	return JsonManager::get();
}
	
void JsonManager::destroy()
{
	CI_LOG_V("Destroying JsonManager");
	sJsonManager = nullptr;
	sJsonManagerInitialized = false;
}
	
const ci::JsonTree& JsonManager::getChild( const std::string &key )
{
	return mRoot[key];
}
	
}