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
#include "InteractionEvents.h"

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
<<<<<<< HEAD
=======
	
>>>>>>> A few debug tweaks also a non-working macro define for allowing large allocations
#if defined( DEBUG )
void Engine::touchBeganDelegate( EventDataRef event )
{
	
	auto touchBegan = std::dynamic_pointer_cast<TouchBeganEvent>( event );
	if( ! touchBegan ) {
		CI_LOG_E("PROBLEM WITH TOUCH BEGAN" << event->getName() );
		return;
	}
	
	mTouchesBegan.push_back( touchBegan->getWorldCoordinate() );
}

void Engine::touchMovedDelegate( EventDataRef event )
{
	auto touchMove = std::dynamic_pointer_cast<TouchMoveEvent>( event );
	if( ! touchMove ) {
		CI_LOG_E("PROBLEM WITH TOUCH Move" << event->getName() );
		return;
	}
	
	mTouchesMoved.push_back( touchMove->getWorldCoordinate() );
}

void Engine::touchEndedDelegate( EventDataRef event )
{
	auto touchEnded = std::dynamic_pointer_cast<TouchEndedEvent>( event );
	
	if( ! touchEnded ) {
		CI_LOG_E("PROBLEM WITH TOUCH Ended" << event->getName() );
		return;
	}
	
	mTouchesEnded.push_back( touchEnded->getWorldCoordinate() );
}
<<<<<<< HEAD
#endif
void Engine::initialize()
{
	mEventManager = heartbeat::EventManager::create( "Global", true );

//	mEventManager->addListener( std::bind( &Engine::touchBeganDelegate, this, std::placeholders::_1 ), TouchBeganEvent::TYPE );
//	mEventManager->addListener( std::bind( &Engine::touchMovedDelegate, this, std::placeholders::_1 ), TouchMoveEvent::TYPE );
//	mEventManager->addListener( std::bind( &Engine::touchEndedDelegate, this, std::placeholders::_1 ), TouchEndedEvent::TYPE );
=======

#endif
	
void Engine::initialize()
{
	mEventManager = heartbeat::EventManager::create( "Global", true );
	
#if defined( DEBUG )
	mEventManager->addListener( std::bind( &Engine::touchBeganDelegate, this, std::placeholders::_1 ), TouchBeganEvent::TYPE );
	mEventManager->addListener( std::bind( &Engine::touchMovedDelegate, this, std::placeholders::_1 ), TouchMoveEvent::TYPE );
	mEventManager->addListener( std::bind( &Engine::touchEndedDelegate, this, std::placeholders::_1 ), TouchEndedEvent::TYPE );
#endif
>>>>>>> A few debug tweaks also a non-working macro define for allowing large allocations
	
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
	
	app->getSignalUpdate().connect( std::bind( &Engine::update, this ) );
	app->getSignalShutdown().connect( std::bind( &Engine::cleanup, this ) );
}
	
void Engine::cleanup()
{
	mRenderer.reset();
	mJsonManager.reset();
	mEventManager.reset();
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
        gl::clearColor( ColorA( 0.11f, 0.32, 0.58f, 1.0f ) );
		gl::clear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		gl::clearColor( ColorA( 1, 1, 1, 1 ) );
	}
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
	
	for( auto & point : mTouchesBegan ) {
		gl::ScopedColor scopeColor( ColorA( 1.0, 0.0, 0.0, 1.0f ) );
		gl::drawSolidCircle( point, 15 );
	}
	mTouchesBegan.clear();
	for ( auto & point : mTouchesMoved ) {
		gl::ScopedColor scopeColor( ColorA( 0.0, 1.0, 1.0, 1.0f ) );
		gl::drawSolidCircle( point, 15 );
	}
	mTouchesMoved.clear();
	for ( auto & point : mTouchesEnded ) {
		gl::ScopedColor scopeColor( ColorA( 1.0, 1.0, 1.0, 1.0f ) );
		gl::drawSolidCircle( point, 15 );
	}
	mTouchesEnded.clear();
#endif


}
	
void Engine::postDraw()
{
	mRenderer->endFrame();
	mRenderer->presentRender();
	auto app = app::App::get();
	mEndingFrame = app->getElapsedSeconds();
	CI_LOG_I("Time for this frame: " << mEndingFrame - mBeginningFrame);
	app->getWindow()->setTitle( to_string( app->getAverageFps() ) );
    
    auto window = mRenderer->getPrimaryWindow();
    window->getRenderer()->makeCurrentContext();
}

}