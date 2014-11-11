//
//  Output.cpp
//  ProjectionTestApp
//
//  Created by Ryan Bartley on 9/29/14.
//
//

#include "Renderer.h"
#include "JsonManager.h"
#include "cinder/Log.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Shader.h"
#include "cinder/gl/Context.h"
#include "Engine.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace heartbeat {
	

const uint32_t BOTTOM_PRESENT_TARGET = 0;
const uint32_t TOP_PRESENT_TARGET = 1;
const uint32_t RENDER_TARGET = 2;
const uint32_t POND_RENDERER = 3;

Renderer::Renderer()
: mEdgeWidth( 0.0f ), mStencilImageUpdated( false )
{
}
	
Renderer::~Renderer()
{
	cleanup();
	CI_LOG_V("Renderer being destroyed");
}
	
void Renderer::cleanup()
{
//	mWindows[0] = nullptr;
//	mWindows[1] = nullptr;
//	for( auto & connection : mDrawSignals ) {
//		connection.disconnect();
//	}
	CI_LOG_V("Renderer being cleanedup");
}
	
RendererRef Renderer::create()
{
	return RendererRef( new Renderer() );
}

void Renderer::initialize()
{
	try {
		
		auto screenAttribs = JsonManager::get()->getRoot()["screenRender"];
		
		try {
			// Grabs the image to stencil with.
			auto imageStencilFileName = screenAttribs["imageStencil"].getValue();
			setImageStencil( imageStencilFileName );
		}
		catch ( ci::JsonTree::ExcChildNotFound &ex ) {
			CI_LOG_W("No imageStencil child found");
		}
		
		try {
			// Writes whether the coordinates should be written to the screen as half size
			auto halfSize = screenAttribs["halfSize"].getValue<bool>();
			setIsHalfSize( halfSize );
		} catch ( JsonTree::ExcChildNotFound &ex ) {
			CI_LOG_W( "No halfSize child found, setting default - true" );
			setIsHalfSize( true );
		}
		
		try {
			// Individual Projector size in pixels
			auto individuals = screenAttribs["individualProjectorSize"].getChildren();
			auto individualIt = individuals.begin();
			ci::vec2 individualSize( (*individualIt++).getValue<float>(), (*individualIt).getValue<float>() );
			
			// Uses half size
			if ( mIsHalfSized )
				individualSize = individualSize / 2.0f;
			
			setIndividualProjectorSize( individualSize );
		}
		catch ( ci::JsonTree::ExcChildNotFound &ex ) {
			ci::vec2 individualSize( 1920, 1080 );
			CI_LOG_W("No individualProjectorSize child found, setting default - " << individualSize);
			
			if ( mIsHalfSized )
				individualSize = individualSize / 2.0f;
			
			setIndividualProjectorSize( individualSize );
		}
		
		try {
			mIsSplitWindow = screenAttribs["splitWindow"].getValue<bool>();
		}
		catch (ci::JsonTree::ExcChildNotFound &ex) {
			CI_LOG_W("No splitWindow child found, setting default - false");
			mIsSplitWindow = false;
		}
		
		try {
			auto numPixelOverlap = screenAttribs["numPixelOverlap"].getValue<uint32_t>();
			
			if ( mIsHalfSized )
				numPixelOverlap = numPixelOverlap / 2;
			
			setNumPixelOverlap( numPixelOverlap );
		}
		catch ( JsonTree::ExcChildNotFound &ex ) {
			uint32_t numPixelOverlap = 200;
			CI_LOG_W("No numPixelOverlap child found, setting default - " << numPixelOverlap);
			
			if ( mIsHalfSized )
				numPixelOverlap = numPixelOverlap / 2;
			
			setNumPixelOverlap( numPixelOverlap );
		}
		
		setupFbos();
		setupGlsl();
		
		// Now that I have the individual Projector Size,
		setupPresentation();
		
	}
	catch ( JsonTree::ExcChildNotFound &ex ) {
		CI_LOG_E( ex.what() );
	}
}
	
void Renderer::setupPresentation()
{
	if( ! mIsSplitWindow ) {
		auto app = app::App::get();
        mWindows[BOTTOM_PRESENT_TARGET] = app->getWindow();
		app->setWindowSize( ivec2( mIndividualProjectorSize.x, mIndividualProjectorSize.y * 2 ) );
		auto engine = Engine::get();
		mDrawSignals.push_back( app->getWindow()->connectDraw( &Engine::preDraw, engine.get() ) );
		mDrawSignals.push_back( app->getWindow()->connectDraw( &Engine::draw, engine.get() ) );
		mDrawSignals.push_back( app->getWindow()->connectDraw( &Engine::postDraw, engine.get() ) );
		mDrawSignals.push_back( app->getWindow()->connectPostDraw( &Renderer::renderToSingleWindow, this ) );
	}
	else {
		auto app = app::AppBasic::get();
		
		// get the one that's already been created
		// this one is the default given to us by the
		// app class
		auto window = app->getWindow();
		window->setSize( mIndividualProjectorSize );
		window->setTitle( "BOTTOM_PRESENT_TARGET" );
//		window->setAlwaysOnTop();
		
		// Hook up the engine draw methods to the "main window"
		auto engine = Engine::get();
		mDrawSignals.push_back( window->connectDraw( &Engine::preDraw, engine.get() ) );
		mDrawSignals.push_back( window->connectDraw( &Engine::draw, engine.get() ) );
		mDrawSignals.push_back( window->connectDraw( &Engine::postDraw, engine.get() ) );
		mDrawSignals.push_back( window->connectPostDraw( &Renderer::renderToBottomWindow, this ) );
		window->setUserData( this );
		mWindows[BOTTOM_PRESENT_TARGET] = window;
		
		// now create another from scratch.
		
		// TODO: Fix this. It's a hack. Need something better like
		// checking the display id from the config file.
		DisplayRef secondDisplay;
		for( auto & display : Display::getDisplays() ) {
			if ( display != window->getDisplay() )
				secondDisplay = display;
		}
		
		Window::Format format;
		format.size( mIndividualProjectorSize ).title( "TOP_PRESENT_TARGET" ).display( secondDisplay ? secondDisplay : window->getDisplay() );//.alwaysOnTop();
		window = app->createWindow( format );
		
		mDrawSignals.push_back( window->connectPostDraw( &Renderer::renderToTopWindow, this ) );
		window->setUserData( this );
		mWindows[TOP_PRESENT_TARGET] = window;
		
		if( ! mIsHalfSized ) {
			mWindows[TOP_PRESENT_TARGET]->setFullScreen();
			mWindows[BOTTOM_PRESENT_TARGET]->setFullScreen();
		}
	}
}
    
ci::app::WindowRef Renderer::getPrimaryWindow() const
{
    return mWindows[BOTTOM_PRESENT_TARGET];
}
	
void Renderer::setTargetRenderSize( const ci::vec2 &totalSize )
{
	mTotalRenderSize = totalSize;
}
	
void Renderer::setIndividualProjectorSize( const ci::vec2 &individualSize )
{
	mIndividualProjectorSize = individualSize;
}
	
void Renderer::setNumPixelOverlap( uint32_t numPixelOverlap )
{
	if( numPixelOverlap == mNumPixelOverlap ) return;
	
	mNumPixelOverlap = numPixelOverlap;
	
	setEdgeBlendWidth( (mNumPixelOverlap / mIndividualProjectorSize.y) * 2.0f );
	
	auto halfTotal = (mIndividualProjectorSize.y - mNumPixelOverlap);
	ci::vec2 totalSize( mIndividualProjectorSize.x, halfTotal * 2.0f );
	
	setTargetRenderSize( totalSize );
}
	
void Renderer::setupFbos()
{
	auto fboFormat = gl::Fbo::Format().depthBuffer().stencilBuffer().samples( 4 );
	
	mFbos[RENDER_TARGET] = gl::Fbo::create( mTotalRenderSize.x, mTotalRenderSize.y, fboFormat );
	CI_LOG_V("Setup render Target");
	mFbos[TOP_PRESENT_TARGET] = gl::Fbo::create( mIndividualProjectorSize.x, mIndividualProjectorSize.y, fboFormat );
	CI_LOG_V("Setup left target");
	mFbos[BOTTOM_PRESENT_TARGET] = gl::Fbo::create( mIndividualProjectorSize.x, mIndividualProjectorSize.y, fboFormat );
	CI_LOG_V("Setup right target");
	mFbos[POND_RENDERER] = gl::Fbo::create( mTotalRenderSize.x, mTotalRenderSize.y, fboFormat.samples( 0 ) );
	CI_LOG_V("Setup pond target");
}
	
void Renderer::setupGlsl()
{
	try {
		auto glsl = gl::GlslProg::create( gl::GlslProg::Format()
											  .vertex( getFileContents( "EdgeBlend.vert" ) )
											  .fragment( getFileContents( "EdgeBlend.frag" ) ) );
		mEdgeBlendGlsl = glsl;
		
		mEdgeBlendGlsl->uniform( "exponent", 2.0f );
		mEdgeBlendGlsl->uniform( "luminance", vec3( 0.5 ) );
		mEdgeBlendGlsl->uniform( "gamma", vec3(1.8, 1.5, 1.2) );
		mEdgeBlendGlsl->uniform( "tex0", 0 );
		
		mAlphaDiscard = gl::GlslProg::create( gl::GlslProg::Format()
											 .vertex( getFileContents( "StencilAlpha.vert" ) )
											 .fragment( getFileContents( "StencilAlpha.frag" ) ) );
		
		mAlphaDiscard->uniform( "alphaThreshold", 0.0f );
	} catch ( gl::GlslProgCompileExc &ex ) {
		CI_LOG_E("Compile error" << ex.what() );
	}
	
}
	
void Renderer::setImageStencil( const std::string &fileName )
{
	auto image = loadImage( getFileContents( fileName ) );
	mImageStencil = gl::Texture2d::create( image );
	CI_LOG_V( "Created ImageStencil from " << fileName );
	mStencilImageUpdated = true;
}
	
void Renderer::setEdgeBlendWidth( float width )
{
	if( width == mEdgeWidth ) return;
	
	mEdgeWidth = width;
}
	
const gl::FboRef& Renderer::getRenderTarget() const
{
	return mFbos[RENDER_TARGET];
}
	
const gl::FboRef& Renderer::getBottomPresentationTarget() const
{
	return mFbos[BOTTOM_PRESENT_TARGET];
}
	
const gl::FboRef& Renderer::getTopPresentationTarget() const
{
	return mFbos[TOP_PRESENT_TARGET];
}
	
const gl::FboRef& Renderer::getPondTarget() const
{
	return mFbos[POND_RENDERER];
}
	
void Renderer::stencilTargetRenderFbo()
{
	if( ! mStencilImageUpdated ) return;
	
    for( int i = 0; i < 2; i++ ) {
	// bind the fbo
        uint32_t renderer = 0;
        if( i == 0 ) {
            renderer = RENDER_TARGET;
        }
        else if( i == 1 ){
            renderer = POND_RENDERER;
        }
	gl::ScopedFramebuffer fboScope( mFbos[renderer] );
	gl::ScopedGlslProg	  glslScope( mAlphaDiscard );
	
	// The texture should have alpha
	gl::ScopedAlphaBlend scopeAlpha( false );
	gl::ScopedState stencilScope( GL_STENCIL_TEST, true );
	
	// All pixels drawn to will be given this mask value
	gl::stencilFunc( GL_ALWAYS, 1, 1 );

	// Keep the value if stencil fails, disabled depth, and Replace
	// if stencil passes
	gl::stencilOp( GL_KEEP, GL_KEEP, GL_REPLACE );
	// specifies that we want to write to any bit
	gl::stencilMask( 0xFF );
	// turn off colormask
	gl::colorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
	// turn off depthmask
	gl::depthMask( GL_FALSE );
	
	// clear the buffer of what it has.
	gl::clear( GL_STENCIL_BUFFER_BIT );
	
	// consider changing the viewport to the size of the Image
	// probably not though because we should be using an image
	// that is the size of our presentation.
	gl::ScopedMatrices scopeMat;
	gl::setMatricesWindow( mImageStencil->getSize() );
	
	gl::ScopedTextureBind scopeTex( mImageStencil );
	
	gl::drawSolidRect( Rectf( vec2( 0 ), mImageStencil->getSize() ) );

	// now we only let pass those that are equal to 0xFF
	gl::stencilFunc( GL_EQUAL, 1, 1 );
	gl::stencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
	// specifies that we don't want to write to any bits,
	// essentially turns off the stencil writer.
	gl::stencilMask( 0x00 );
	
	// Turn the color and depth masks back on
	gl::colorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
	gl::depthMask( GL_TRUE );
    }
	
	mStencilImageUpdated = false;
}
	
void Renderer::beginFrame()
{
	// Setup stencil area
	mFbos[RENDER_TARGET]->bindFramebuffer();
	gl::enable( GL_STENCIL_TEST );
	gl::clear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	auto ctx = gl::context();
	ctx->pushViewport( make_pair( vec2( 0 ), mTotalRenderSize ) );
	
	stencilTargetRenderFbo();
}
	
void Renderer::endFrame()
{
	auto ctx = gl::context();
	ctx->popViewport();
	gl::disable( GL_STENCIL_TEST );
	mFbos[RENDER_TARGET]->unbindFramebuffer();
}
	
void Renderer::presentRender()
{
	renderToPresentTarget( BOTTOM_PRESENT_TARGET );
	renderToPresentTarget( TOP_PRESENT_TARGET );
}
	
void Renderer::renderToPresentTarget( uint32_t target )
{
	auto & fbo = mFbos[target];
	auto texture = mFbos[RENDER_TARGET]->getColorTexture();
	
	gl::ScopedFramebuffer scopeFbo( fbo );
	gl::ScopedTextureBind scopeTex( texture );
	gl::ScopedGlslProg	  scopeGlsl( gl::getStockShader( gl::ShaderDef().color().texture() ) );
	gl::ScopedViewport	  scopeView( vec2( 0 ), mIndividualProjectorSize );
	gl::ScopedMatrices	  scopeMat;

	gl::clearColor( ColorA( 1, 1, 1, 1 ) );
	gl::clear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	gl::setMatricesWindow( mIndividualProjectorSize, true );
	
	if( target == BOTTOM_PRESENT_TARGET ) {
		auto offset = mIndividualProjectorSize.y - mNumPixelOverlap * 2;
		auto offsetBottomLeft = vec2( 0, -offset );
		auto offsetUpperRight = vec2( mTotalRenderSize.x, mTotalRenderSize.y - offset );
		gl::drawSolidRect( Rectf( offsetBottomLeft, offsetUpperRight ) );
	}
	else if( target == TOP_PRESENT_TARGET ) {
		gl::drawSolidRect( Rectf( vec2( 0 ), vec2( mTotalRenderSize ) ) );
	}
}
	
void Renderer::renderToSingleWindow()
{
	if( ! mIsSplitWindow ) {
		gl::clear();
		gl::ScopedViewport scopeView( vec2( 0 ), getWindowSize() );
		gl::ScopedMatrices scopeMat;
		gl::setMatricesWindow( getWindowSize() );
		gl::ScopedGlslProg	  scopeGlsl( mEdgeBlendGlsl );
		
		// This is the bottom Presentation Target using the edge Width on the top
		mEdgeBlendGlsl->uniform( "edges", vec4( 0.0, mEdgeWidth, 0.0, 0.0 ) );
		{
			
			auto tex = getTopPresentationTarget()->getColorTexture();
			gl::ScopedTextureBind scopeTex( tex );
			
			gl::drawSolidRect( Rectf( vec2( 0 ), tex->getSize() ) );
		}
		
		// This is the top Presentation Target using the edge Width on the bottom
		mEdgeBlendGlsl->uniform( "edges", vec4( 0.0, 0.0, 0.0, mEdgeWidth ) );
		{
			auto tex = getBottomPresentationTarget()->getColorTexture();
			
			gl::ScopedModelMatrix scopeMat;
			gl::translate( vec2( 0, getWindowHeight() ) / 2.0f );
			gl::ScopedTextureBind scopeTex( tex );
			gl::drawSolidRect( Rectf( vec2( 0 ), tex->getSize() ) );
		}
	}
	else {
		CI_LOG_E("Calling single Window Render, when mIsSplit is true");
	}
}
	
void Renderer::renderToTopWindow()
{
	auto window = mWindows[TOP_PRESENT_TARGET];
	if( mIsSplitWindow && window ) {
		auto renderer = window->getUserData<Renderer>();
		gl::clear();
		gl::ScopedViewport scopeView( vec2( 0 ), window->getSize() );
		gl::ScopedMatrices scopeMat;
		gl::setMatricesWindow( window->getSize() );
		gl::ScopedGlslProg	  scopeGlsl( renderer->mEdgeBlendGlsl );
		
		// This is the top Presentation Target using the edge Width on the bottom
		renderer->mEdgeBlendGlsl->uniform( "edges", vec4( 0.0, 0.0, 0.0, renderer->mEdgeWidth ) );
		{
			auto tex = renderer->getBottomPresentationTarget()->getColorTexture();
			gl::ScopedTextureBind scopeTex( tex );
			
			gl::drawSolidRect( Rectf( vec2( 0 ), tex->getSize() ) );
		}
	}
	else {
		CI_LOG_E("Trying to render to seperate window, when mIsSplit is false");
	}
}

void Renderer::renderToBottomWindow()
{
	auto window = mWindows[BOTTOM_PRESENT_TARGET];
	if( mIsSplitWindow && window ) {
		auto renderer = window->getUserData<Renderer>();
		gl::clear();
		gl::ScopedViewport scopeView( vec2( 0 ), window->getSize() );
		gl::ScopedMatrices scopeMat;
		gl::setMatricesWindow( window->getSize() );
		gl::ScopedGlslProg	  scopeGlsl( renderer->mEdgeBlendGlsl );
		
		// This is the bottom Presentation Target using the edge Width on the top
		renderer->mEdgeBlendGlsl->uniform( "edges", vec4( 0.0, renderer->mEdgeWidth, 0.0, 0.0 ) );
		{
			auto tex = renderer->getTopPresentationTarget()->getColorTexture();
			gl::ScopedTextureBind scopeTex( tex );
			
			gl::drawSolidRect( Rectf( vec2( 0 ), tex->getSize() ) );
		}
	}
	else {
		CI_LOG_E("Trying to render to seperate window, when mIsSplit is false");
	}
}
	
}