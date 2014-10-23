//
//  SvgManager.h
//  ProjectionTestApp
//
//  Created by Ryan Bartley on 10/1/14.
//
//

#pragma once

#include "Common.h"
#include "cinder/svg/Svg.h"

namespace heartbeat {
	
class SvgManager {
public:
	
	static SvgManagerRef create( const std::string &fileName );
	static SvgManagerRef get();
	
	
	SvgManager( const SvgManager & ) = delete;
	SvgManager( SvgManager && ) = delete;
	SvgManager& operator=( const SvgManager & ) = delete;
	SvgManager& operator=( SvgManager && ) = delete;
	
	~SvgManager();
	
	const ci::svg::Group* getGroup( const std::string &groupId ) { return mDoc->find<ci::svg::Group>( groupId ); }
	
private:
	SvgManager(  const std::string &fileName );
	
	void initialize();
	static void destroy();
	
	ci::svg::DocRef mDoc;
	std::string		mFileName;
	
	friend class Engine;
};
	
}
