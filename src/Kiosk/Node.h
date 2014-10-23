//
//  Node.h
//  SvgTest
//
//  Created by Ryan Bartley on 10/20/14.
//
//

#pragma once

#include "Common.h"
#include "SvgManager.h"

#include "cinder/svg/Svg.h"

namespace heartbeat {
	
class Node {
public:
	
	virtual ~Node() {}
	
	virtual void render() = 0;
	
	const ci::svg::Group*	getRootGroup() { return mGroup; }
	const ci::svg::Node&	getChild( const std::string &childId ) { return mGroup->getChild( childId ); }

protected:
	Node( const std::string &group )
	: mGroup( SvgManager::get()->getGroup( group ) )
	{
	}
	
	const ci::svg::Group* mGroup;
};
	
}
