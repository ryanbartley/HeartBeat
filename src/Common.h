//
//  Common.h
//  ProjectionTestApp
//
//  Created by Ryan Bartley on 9/29/14.
//
//

#pragma once

#include <boost/functional/hash.hpp>

namespace heartbeat {
	
enum class KioskId {
	TOP_KIOSK = 0,
	MIDDLE_KIOSK = 1,
	BOTTOM_KIOSK = 2
};
	
std::string getKiosk( KioskId kioskId );
	
using EngineRef = std::shared_ptr<class Engine>;
using RendererRef = std::shared_ptr<class Renderer>;
using JsonManagerRef = std::shared_ptr<class JsonManager>;
using SvgManagerRef = std::shared_ptr<class SvgManager>;
using EventManagerRef = std::shared_ptr<class EventManager>;
using HidCommManagerRef = std::shared_ptr<class HidCommManager>;
using KioskRef = std::shared_ptr<class Kiosk>;
using UrgRef = std::shared_ptr<class Urg>;
using InteractionZonesRef = std::shared_ptr<class InteractionZones>;
using RenderableRef = std::shared_ptr<class Renderable>;
using InteractionDebugRenderableRef = std::shared_ptr<class InteractionDebugRenderable>;
	
using KioskManagerRef = std::shared_ptr<class KioskManager>;
	using InfoDisplayRef = std::shared_ptr<class InfoDisplay>;
	
using SpringMeshRef = std::shared_ptr<class SpringMesh>;
using PondElementRef = std::shared_ptr<class PondElement>;

using ButtonRef		= std::shared_ptr<class Button>;
using DataRef		= std::shared_ptr<class Data>;
using OverlayRef	= std::shared_ptr<class Overlay>;
	
using EventType		= uint64_t;
using EventDataRef	= std::shared_ptr<class EventData>;
	

//! Returns the Resource Path for tests and applications, Note: NOT FOR
//! USE WITH ci::loadResource.
boost::filesystem::path& getWorkingResourcePath();
//! Returns a DataSourceRef from the readable file of \a fileName or null
//! if it doesn't exist. Note: Uses ci::loadResource in release.
ci::DataSourceRef getFileContents( const std::string &fileName );
//! Returns a DataTargetPathRef for the writeable file of \a fileName or
//! null if it doesn't exist.
ci::DataTargetPathRef getWriteablePath( const std::string &fileName );
	
class Hash {
public:
	static boost::hash<std::string> HASHER;
};

}
