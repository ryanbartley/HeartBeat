//
//  Node.h
//  SvgTest
//
//  Created by Ryan Bartley on 10/20/14.
//
//

#pragma once

#include "Common.h"

#include "cinder/svg/Svg.h"

namespace heartbeat {
	
class Node {
public:

	static NodeRef create( const std::string &group );
	
	virtual ~Node() {}
	
private:
	Node( const std::string &group );
	
	ci::svg::Group* mNode;
};
	
}
