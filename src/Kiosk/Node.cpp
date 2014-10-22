//
//  Node.cpp
//  SvgTest
//
//  Created by Ryan Bartley on 10/20/14.
//
//

#include "Node.h"

#include "SvgManager.h"

namespace heartbeat {
	
Node::Node( const std::string& group )
{
//	SvgManager::get();
}

NodeRef Node::create( const std::string &group )
{
	return NodeRef( new Node( group ) );
}
	
}