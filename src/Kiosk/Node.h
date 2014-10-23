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
	
	void render();
	
protected:
	Node( const std::string &group )
	: mNode( SvgManager::get()->getGroup( group ) )
	{
	}
	
	const ci::svg::Group* mNode;
};
	
}
