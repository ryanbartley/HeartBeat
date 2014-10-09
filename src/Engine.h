//
//  Engine.h
//  ProjectionTestApp
//
//  Created by Ryan Bartley on 9/29/14.
//
//

#pragma once

#include "Common.h"

#include "cinder/params/Params.h"

namespace heartbeat {
	
class Engine {
public:
	
	static EngineRef	create();
	static EngineRef	get();
	static void			destroy();
	
	Engine( const Engine & ) = delete;
	Engine( Engine && ) = delete;
	Engine& operator=( const Engine & ) = delete;
	Engine& operator=( Engine && ) = delete;
	
	~Engine() {}
	
	void update();
	void preDraw();
	void draw();
	void postDraw();
	
	void keyDown( ci::app::KeyEvent event );
	
	const RendererRef& getRenderer() { return mRenderer; }
	const JsonManagerRef& getJsonManager() { return mJsonManager; }
	
private:
	Engine();
	
	void initialize();
	void cleanup();
	
	RendererRef					mRenderer;
	JsonManagerRef				mJsonManager;
	EventManagerRef				mEventManager;
	HidCommManagerRef			mHidCommManager;
	InteractionZonesRef			mInteractionManager;
	std::array<KioskRef, 3>		mKiosks;
	
	ci::params::InterfaceGlRef	mParams;
	
	std::vector<RenderableRef>  mRenderables;
	
	std::vector<boost::signals2::connection> mConnections;

};
	
}