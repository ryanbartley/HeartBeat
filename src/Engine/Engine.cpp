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
#include "EventManager.h"
#include "HidCommManager.h"
#include "InteractionZones.h"
#include "Renderable.h"
#include "SvgManager.h"
#include "KioskManager.h"
#include "Pond.h"
#include "InteractionDebugRenderable.h"

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
    
    auto window = mRenderer->getPrimaryWindow();
    window->getRenderer()->makeCurrentContext();
    
	mInteractionManager = heartbeat::InteractionZones::create();
#if defined( DEBUG )
	mInteractionDebug = heartbeat::InteractionDebugRenderable::create( mInteractionManager );
#endif
    
	mInteractionManager->initialize();
    
#if defined( DEBUG )
	mInteractionDebug->initialize();
#endif
	
	mSvgManager = heartbeat::SvgManager::create();
	mSvgManager->initialize();
	
	mKioskManager = heartbeat::KioskManager::create();
	mKioskManager->initialize();
	
	mPond = heartbeat::Pond::create( mRenderer->getTotalRenderSize() );
	mPond->initialize();
	
	auto app = app::App::get();
	
	mConnections.push_back( app->getSignalUpdate().connect( std::bind( &Engine::update, this ) ) );
	mConnections.push_back( app->getWindow()->connectKeyDown( &Engine::keyDown, this ) );
	mConnections.push_back( app->getSignalShutdown().connect( std::bind( &Engine::cleanup, this ) ) );
}
	
void Engine::cleanup()
{
//	for( auto & connection : mConnections ) {
//		connection.disconnect();
//	}
//	mRenderer.reset();
//	mJsonManager.reset();
//	mEventManager.reset();
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
	mBeginningFrame = app::App::get()->getElapsedSeconds();
    
    auto window = mRenderer->getPrimaryWindow();
    window->getRenderer()->makeCurrentContext();
    
	mEventManager->update();
	mInteractionManager->preProcessData();
	mInteractionManager->processData();
	mPond->update();
	mKioskManager->update();
}
	
void Engine::preDraw()
{
	mRenderer->beginFrame();
}
	
void Engine::draw()
{
    auto window = mRenderer->getPrimaryWindow();
    window->getRenderer()->makeCurrentContext();
    
	gl::setMatricesWindow( getRenderer()->getTotalRenderSize() );
	auto pondRenderer = getRenderer()->getPondTarget();
	{
		gl::ScopedFramebuffer scopeFBO( pondRenderer );
		gl::clearColor( ColorA( 0.30196f, 0.49019f, 0.72941f, 1.0f ) );
		gl::clear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		mPond->renderPondElements();
		gl::clearColor( ColorA( 1, 1, 1, 1 ) );
	}
//    gl::draw( pondRenderer->getColorTexture() );
	mPond->projectPondElements( pondRenderer->getColorTexture() );
	
	mKioskManager->render();
	
#if defined( DEBUG )
	if( mInteractionDebug ) {
		mInteractionDebug->draw();
	}
	if( mParams ) {
		gl::ScopedViewport scopeView( vec2( 0 ), getWindowSize() );
		gl::ScopedMatrices scopeMatrices;
		gl::setMatricesWindow( getWindowSize() );
		mParams->draw();
	}
#endif


}
	
void Engine::postDraw()
{
	mRenderer->endFrame();
	mRenderer->presentRender();
	auto app = app::App::get();
	mEndingFrame = app->getElapsedSeconds();
	cout << "Time for this frame: " << mEndingFrame - mBeginningFrame << endl;
	app->getWindow()->setTitle( to_string( app->getAverageFps() ) );
    
    auto window = mRenderer->getPrimaryWindow();
    window->getRenderer()->makeCurrentContext();
}

}