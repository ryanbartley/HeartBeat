//
//  Engine.cpp
//  ProjectionTestApp
//
//  Created by Ryan Bartley on 9/29/14.
//
//

#include "Engine.h"
#include "cinder/Log.h"
#include "Urg.h"

#include "JsonManager.h"
#include "Renderer.h"
#include "Kiosk.h"
#include "EventManager.h"
#include "HidCommManager.h"
#include "InteractionZones.h"
#include "Renderable.h"

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
		sEngineInitialized = true;
		sEngine->initialize();
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
	sEngine->cleanup();
	sEngine = nullptr;
	sEngineInitialized = false;
}
	
void Engine::initialize()
{
	mEventManager = heartbeat::EventManager::create( "Global", true );
	mJsonManager = heartbeat::JsonManager::create( "test.json" );
	
	mRenderer = heartbeat::Renderer::create();
	mRenderer->initialize();
	
	mHidCommManager = heartbeat::HidCommManager::create();
	mHidCommManager->initialize();
	
	auto app = app::App::get();
	
	mConnections.push_back( app->getSignalUpdate().connect( std::bind( &Engine::update, this ) ) );
	mConnections.push_back( app->getWindow()->connectKeyDown( &Engine::keyDown, this ) );
	mConnections.push_back( app->getSignalShutdown().connect( std::bind( &Engine::cleanup, this ) ) );
}
	
void Engine::cleanup()
{
	for( auto & connection : mConnections ) {
		connection.disconnect();
	}
	mRenderer.reset();
	mJsonManager.reset();
	mEventManager.reset();
	mHidCommManager.reset();
}
	
void Engine::keyDown( ci::app::KeyEvent event )
{
	if( ! event.isShiftDown() ) return;
	
	if( event.getChar() == 's' )
		mRenderer->setupGlsl();
	
	if( event.getChar() == ' ') {
		
	}
	
	if( event.getChar() == 'c' )
		mInteractionManager->captureBarrier();
	
	if( event.getChar() == 'b' )
		mInteractionManager->writeInteractionZone();
		
}
	
void Engine::update()
{
	for( auto & kiosk : mKiosks ) {
		
	}
}
	
void Engine::preDraw()
{
	mRenderer->beginFrame();
}
	
void Engine::draw()
{
	cout << "I'm in draw" << endl;
	
	CameraPersp		mCamera;
	auto aspect = getRenderer()->getTotalRenderSize();
	mCamera.setPerspective( 60.0f, aspect.x / aspect.y , .01f, 1000.0f );
	mCamera.lookAt( vec3( 0, 0, 6 ), vec3( 0 ) );
	
	{
		gl::ScopedMatrices scopeMat;
		gl::setMatricesWindow( getRenderer()->getTotalRenderSize() );
		gl::color( 1, 0, 0 );
		gl::drawSolidRect( Rectf( vec2( 0 ), getRenderer()->getTotalRenderSize() ) );
	}
	{
		gl::enableDepthRead();
		gl::enableDepthWrite();
		
		static float rotation = 0.0f;
		gl::ScopedMatrices scopeMat;
		gl::setMatrices( mCamera );
		gl::color( 1, 1, 1 );
		gl::multModelMatrix( rotate( rotation += 0.01f, vec3( 0, 1, 0 ) ) );
		gl::drawColorCube( vec3( 0 ), vec3( 4 ) );
		
		gl::disableDepthRead();
		gl::disableDepthWrite();
	}
	
//	for( auto & renderable : mRenderables ) {
//		renderable->draw();
//	}
	
}
	
void Engine::postDraw()
{
	mRenderer->endFrame();
	mRenderer->presentRender();
}

}