//
//  Engine.cpp
//  ProjectionTestApp
//
//  Created by Ryan Bartley on 9/29/14.
//
//

#include "Engine.h"
#include "cinder/Log.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace heartbeat {

static EngineRef sEngine = nullptr;
static bool sEngineInitialized = false;

Engine::Engine()
{
	
}
	
EngineRef Engine::create()
{
	if( ! sEngineInitialized ) {
		sEngine = EngineRef( new Engine() );
		sEngine->initialize();
		sEngineInitialized = true;
		return sEngine;
	}
	else
		return sEngine;
}
	
EngineRef Engine::get()
{
	if( sEngineInitialized )
		return sEngine;
	else {
		CI_LOG_E("Accessing an engine before it's been initialized");
		return EngineRef();
	}
}
	
void Engine::destroy()
{
	sEngine = nullptr;
	sEngineInitialized = false;
}
	
void Engine::initialize()
{
	mJsonManager = heartbeat::JsonManager::create( "test.json" );
	mRenderer = heartbeat::Renderer::create();
	mRenderer->initialize();
	
	auto app = app::App::get();
	
	mConnections.push_back( app->getSignalUpdate().connect( std::bind( &Engine::update, this ) ) );
	mConnections.push_back( app->getWindow()->connectKeyDown( &Engine::keyDown, this ) );
	mConnections.push_back( app->getWindow()->connectDraw( &Engine::preDraw, this ) );
	mConnections.push_back( app->getWindow()->connectPostDraw( &Engine::postDraw, this ) );
}
	
void Engine::keyDown( ci::app::KeyEvent event )
{
	if( ! event.isShiftDown() ) return;
	
	if( event.getChar() == 's' )
		mRenderer->setupGlsl();
}
	
void Engine::update()
{
	
}
	
void Engine::preDraw()
{
	mRenderer->beginFrame();
	
	
}
	
void Engine::postDraw()
{
	mRenderer->endFrame();
	
	mRenderer->presentRender();
}

}