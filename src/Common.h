//
//  Common.h
//  ProjectionTestApp
//
//  Created by Ryan Bartley on 9/29/14.
//
//

#pragma once

namespace heartbeat {
	
using EngineRef = std::shared_ptr<class Engine>;
using JsonManagerRef = std::shared_ptr<class JsonManager>;
using JsonManagerWeak = std::shared_ptr<class JsonManager>;
	
ci::DataSourceRef getFileContents( const std::string &fileName );
	
}
