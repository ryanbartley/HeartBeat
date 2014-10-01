//
//  Common.cpp
//  ProjectionTestApp
//
//  Created by Ryan Bartley on 9/29/14.
//
//

#include "Common.h"
#include "cinder/Log.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace heartbeat {
	
ci::DataSourceRef getFileContents( const std::string &fileName )
{
	ci::DataSourceRef ret;
	// This is dirty but works with the structure.
	auto path = cinder::app::getAppPath() / ".." / ".." / ".."
				/ ".." / ".." / ".." / "resources" / fileName;
	try {
#ifdef DEBUG
		ret = ci::loadFile( path );
#else
		ret = loadResource( fileName );
#endif
		
	} catch ( std::exception &e ) {
		CI_LOG_E( e.what() << " PATH: " << path );
	}
	
	return ret;
}

}