//
//  JsonManager.h
//  ProjectionTestApp
//
//  Created by Ryan Bartley on 9/29/14.
//
//

#pragma once

#include "Common.h"

#include "cinder/Json.h"

namespace heartbeat {
	
class JsonManager {
public:
	
	static JsonManagerRef create( const std::string &fileName );
	static JsonManagerRef get();
	static void destroy();
	
	JsonManager( const JsonManager & ) = delete;
	JsonManager( JsonManager && ) = delete;
	JsonManager& operator=( const JsonManager & ) = delete;
	JsonManager& operator=( JsonManager && ) = delete;
	
	~JsonManager() {}
	
	const ci::JsonTree& getRoot() { return mRoot["root"]; }
	const std::string& getFileName() { return mFileName; }
	
	const ci::JsonTree& getChild( const std::string &key );
	
private:
	JsonManager( const std::string &fileName, const ci::DataSourceRef &fileContents  );
	
	std::string		mFileName;
	ci::JsonTree	mRoot;
};
	
}
