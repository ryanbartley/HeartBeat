//
//  Pages.cpp
//  SvgTest
//
//  Created by Ryan Bartley on 10/24/14.
//
//

#include "PageTypes.h"
#include "JsonManager.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace heartbeat {
	
DataPage::DataPage( const std::string &name )
: Page( name )
{
}
	
DataPageRef DataPage::create( const std::string &name )
{
	return DataPageRef( new DataPage( name ) );
}

void DataPage::initialize( const ci::JsonTree &root )
{
	
}
	
OverlayPage::OverlayPage( const std::string &name )
: Page( name )
{
}
	
OverlayPageRef OverlayPage::create( const std::string &name )
{
	return OverlayPageRef( new OverlayPage( name ) );
}
	
void OverlayPage::initialize( const ci::JsonTree &root )
{
	
}

}