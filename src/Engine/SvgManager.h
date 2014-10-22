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
	
	SvgManager( const SvgManager & ) = delete;
	SvgManager( SvgManager && ) = delete;
	SvgManager& operator=( const SvgManager & ) = delete;
	SvgManager& operator=( SvgManager && ) = delete;
	
	~SvgManager();
	
	const ci::svg::Node& getNode( const std::string &nodeId ) { return mDoc->getChild( nodeId ); }
	

private:
	SvgManager(  const std::string &fileName );
	
	void initialize();
	
	ci::svg::DocRef mDoc;
	std::string		mFileName;
};
	
}
