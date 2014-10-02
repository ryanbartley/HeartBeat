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
using EventManagerRef = std::shared_ptr<class EventManager>;
using HidCommManagerRef = std::shared_ptr<class HidCommManager>;
	
ci::DataSourceRef getFileContents( const std::string &fileName );
	
}
