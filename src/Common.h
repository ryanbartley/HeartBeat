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
using RendererRef = std::shared_ptr<class Renderer>;
using JsonManagerRef = std::shared_ptr<class JsonManager>;
using EventManagerRef = std::shared_ptr<class EventManager>;
using HidCommManagerRef = std::shared_ptr<class HidCommManager>;
using KioskRef = std::shared_ptr<class Kiosk>;
using UrgRef = std::shared_ptr<class Urg>;
using InteractionZonesRef = std::shared_ptr<class InteractionZones>;
using RenderableRef = std::shared_ptr<class Renderable>;
using InteractionDebugRenderableRef = std::shared_ptr<class InteractionDebugRenderable>;

//! Returns the Resource Path for tests and applications, Note: NOT FOR
//! USE WITH ci::loadResource.
boost::filesystem::path& getWorkingResourcePath();
//! Returns a DataSourceRef from the readable file of \a fileName or null
//! if it doesn't exist. Note: Uses ci::loadResource in release.
ci::DataSourceRef getFileContents( const std::string &fileName );
//! Returns a DataTargetPathRef for the writeable file of \a fileName or
//! null if it doesn't exist.
ci::DataTargetPathRef getWriteablePath( const std::string &fileName );

}
