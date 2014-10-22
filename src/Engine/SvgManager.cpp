//
//  SvgManager.cpp
//  ProjectionTestApp
//
//  Created by Ryan Bartley on 10/1/14.
//
//

#include "SvgManager.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace heartbeat {
	
SvgManager::SvgManager( const std::string &fileName )
: mFileName( fileName )
{
}
	
SvgManager::~SvgManager()
{
}
	
SvgManagerRef SvgManager::create( const std::string &fileName )
{
	return SvgManagerRef( new SvgManager( fileName ) );
}
	
void SvgManager::initialize()
{
	mDoc = svg::Doc::create( getFileContents( mFileName ) );
//	mDoc->Group::
}
	

	
}