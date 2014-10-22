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
	
boost::hash<std::string> Hash::HASHER;
	
boost::filesystem::path& getWorkingResourcePath()
{
	// This is dirty but works with the structure.
	static boost::filesystem::path path = cinder::app::getAppPath() / ".." / ".." / ".."
	/ ".." / ".." / ".." / "resources";
	return path;
}
	
ci::DataSourceRef getFileContents( const std::string &fileName )
{
	try {
		ci::DataSourceRef ret;
#ifdef DEBUG
		auto path = getWorkingResourcePath() / fileName;
		ret = ci::loadFile( path );
#else
		ret = loadResource( fileName );
#endif
		return ret;
	}
	catch ( std::exception &e ) {
		CI_LOG_E( e.what() << " PATH: " << fileName );
		return ci::DataSourceRef();
	}
	
	
}
	
ci::DataTargetPathRef getWriteablePath( const std::string &fileName )
{
	
	try {
		ci::DataTargetPathRef ret;
#ifdef DEBUG
		auto path = getWorkingResourcePath() / fileName;
		ret = ci::writeFile( path );
#else
		// We shouldn't be writing files in release
		CI_ASSERT(0);
#endif
		return ret;
	}
	catch ( std::exception &ex ) {
		CI_LOG_E(ex.what());
		return ci::DataTargetPathRef();
	}
}

}