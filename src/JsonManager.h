//
//  JsonManager.h
//  ProjectionTestApp
//
//  Created by Ryan Bartley on 9/29/14.
//
//

#pragma once

#include "cinder/Json.h"

namespace heartbeat {
	
using JsonManagerRef = std::shared_ptr<class JsonManager>;
using JsonManagerWeak = std::shared_ptr<class JsonManager>;
	
class JsonManager {
public:
	
	static JsonManagerRef create( const std::string &fileName );
	static JsonManagerRef get();
	static void destroy();
	
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
