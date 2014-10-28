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

bool DataPage::initialize( const ci::JsonTree &root )
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
	
bool OverlayPage::initialize( const ci::JsonTree &root )
{
	try {
		
	}
	catch( const JsonTree::ExcChildNotFound &ex ) {
		
	}
}
	
OverlaySection::OverlaySection( const std::string &name )
: OverlayPage( name )
{
}

OverlaySectionRef OverlaySection::create( const std::string &name )
{
	return OverlaySectionRef( new OverlaySection( name ) );
}

bool OverlaySection::initialize( const ci::JsonTree &root )
{
	try {
		
	} catch ( const JsonTree::ExcChildNotFound &ex ) {
		
	}
}

OverlayPlus::OverlayPlus( const std::string &name )
: OverlayPage( name )
{
}

OverlayPlusRef OverlayPlus::create( const std::string &name )
{
	return OverlayPlusRef( new OverlayPlus( name ) );
}

bool OverlayPlus::initialize( const ci::JsonTree &root )
{
	try {
		
	}
	catch( const JsonTree::ExcChildNotFound &ex ) {
		
	}
}

}