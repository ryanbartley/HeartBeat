//
//  Output.cpp
//  ProjectionTestApp
//
//  Created by Ryan Bartley on 9/29/14.
//
//

#include "ScreenRenderer.h"
#include "JsonManager.h"
#include "cinder/Log.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Shader.h"
#include "cinder/gl/Context.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace heartbeat {
	
const uint32_t RENDER_TARGET = 0;
const uint32_t BOTTOM_PRESENT_TARGET = 1;
const uint32_t TOP_PRESENT_TARGET = 2;

ScreenRenderer::ScreenRenderer()
: mEdgeWidth( 0.0f ), mStencilImageUpdated( false )
{
}
	
ScreenRendererRef ScreenRenderer::create()
{
	return ScreenRendererRef( new ScreenRenderer() );
}

void ScreenRenderer::initialize()
{
	try {
		
		auto screenAttribs = JsonManager::get()->getRoot()["screenRender"];
		
		try {
			auto imageStencilFileName = screenAttribs["imageStencil"].getValue();
			setImageStencil( imageStencilFileName );
		}
		catch ( ci::JsonTree::ExcChildNotFound &ex ) {
			CI_LOG_W("No imageStencil child found");
		}
		
		try {
			auto halfSize = screenAttribs["halfSize"].getValue<bool>();
			setHalfSize( halfSize );
		} catch ( JsonTree::ExcChildNotFound &ex ) {
			CI_LOG_W( "No halfSize child found, setting default - true" );
			setHalfSize( true );
		}
		
		try {
			auto individuals = screenAttribs["individualProjectorSize"].getChildren();
			auto individualIt = individuals.begin();
			ci::vec2 individualSize( (*individualIt++).getValue<float>(), (*individualIt).getValue<float>() );
			
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
		
		app::App::get()->setWindowSize( ivec2( mIndividualProjectorSize.x, mIndividualProjectorSize.y * 2 ) );
		setupFbos();
		setupGlsl();
		
	}
	catch ( JsonTree::ExcChildNotFound &ex ) {
		CI_LOG_E( ex.what() );
	}
}
	
void ScreenRenderer::setTotalRenderSize( const ci::vec2 &totalSize )
{
	mTotalRenderSize = totalSize;
}
	
void ScreenRenderer::setIndividualProjectorSize( const ci::vec2 &individualSize )
{
	mIndividualProjectorSize = individualSize;
}
	
void ScreenRenderer::setNumPixelOverlap( uint32_t numPixelOverlap )
{
	if( numPixelOverlap == mNumPixelOverlap ) return;
	
	mNumPixelOverlap = numPixelOverlap;
	
	mEdgeWidth = mNumPixelOverlap / mIndividualProjectorSize.y;
	
	auto halfTotal = (mIndividualProjectorSize.y - mNumPixelOverlap);
	ci::vec2 totalSize( mIndividualProjectorSize.x, halfTotal * 2.0f );
	
	setTotalRenderSize( totalSize );
}
	
void ScreenRenderer::setupFbos()
{
	auto fboFormat = gl::Fbo::Format().depthBuffer().stencilBuffer().samples( 4 );
	
	mFbos[RENDER_TARGET] = gl::Fbo::create( mTotalRenderSize.x, mTotalRenderSize.y, fboFormat );
	CI_LOG_V("Setup render Target");
	mFbos[TOP_PRESENT_TARGET] = gl::Fbo::create( mIndividualProjectorSize.x, mIndividualProjectorSize.y, fboFormat );
	CI_LOG_V("Setup left target");
	mFbos[BOTTOM_PRESENT_TARGET] = gl::Fbo::create( mIndividualProjectorSize.x, mIndividualProjectorSize.y, fboFormat );
	CI_LOG_V("Setup right target");
}
	
void ScreenRenderer::setupGlsl()
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
	
void ScreenRenderer::setImageStencil( const std::string &fileName )
{
	auto image = loadImage( getFileContents( fileName ) );
	mImageStencil = gl::Texture2d::create( image );
	CI_LOG_V( "Created ImageStencil from " << fileName );
	mStencilImageUpdated = true;
}
	
void ScreenRenderer::setEdgePercentages( float width )
{
	if( width == mEdgeWidth ) return;
	
	mEdgeWidth = width;
}
	
const gl::FboRef& ScreenRenderer::getRenderTarget()
{
	return mFbos[RENDER_TARGET];
}
	
const gl::FboRef& ScreenRenderer::getBottomPresentationTarget()
{
	return mFbos[BOTTOM_PRESENT_TARGET];
}
	
const gl::FboRef& ScreenRenderer::getTopPresentationTarget()
{
	return mFbos[TOP_PRESENT_TARGET];
}
	
void ScreenRenderer::stencilTargetRenderFbo()
{
	if( ! mStencilImageUpdated ) return;
	
	// bind the fbo
	gl::ScopedFramebuffer fboScope( mFbos[RENDER_TARGET] );
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
	gl::stencilFunc( GL_KEEP, GL_KEEP, GL_KEEP );
	// specifies that we don't want to write to any bits,
	// essentially turns off the stencil writer.
	gl::stencilMask( 0x00 );
	
	// Turn the color and depth masks back on
	gl::colorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
	gl::depthMask( GL_TRUE );
	
	mStencilImageUpdated = false;
}
	
void ScreenRenderer::beginFrame()
{
	// Setup stencil area
	mFbos[RENDER_TARGET]->bindFramebuffer();
	gl::enable( GL_STENCIL_TEST );
	gl::clear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	auto ctx = gl::context();
	ctx->pushViewport( make_pair( vec2( 0 ), mTotalRenderSize ) );
	
	stencilTargetRenderFbo();
}
	
void ScreenRenderer::endFrame()
{
	auto ctx = gl::context();
	ctx->popViewport();
	
	gl::disable( GL_STENCIL_TEST );
	mFbos[RENDER_TARGET]->unbindFramebuffer();
}
	
void ScreenRenderer::presentRender()
{
	renderToPresent( BOTTOM_PRESENT_TARGET );
	renderToPresent( TOP_PRESENT_TARGET );
	
	renderToWindow();
}
	
void ScreenRenderer::renderToPresent( uint32_t target )
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
	
	gl::setMatricesWindow( mIndividualProjectorSize, false );
	
	if( target == BOTTOM_PRESENT_TARGET ) {
		auto offset = 340.0f;
		gl::drawSolidRect( Rectf( ivec2( 0, - offset ),
								 ivec2(texture->getSize().x, texture->getSize().y - offset ) ) );
	}
	else if( target == TOP_PRESENT_TARGET ) {
		gl::drawSolidRect( Rectf( vec2( 0 ), vec2( texture->getSize() ) ) );
	}
}
	
void ScreenRenderer::renderToWindow()
{
	gl::clear();
	gl::ScopedViewport scopeView( vec2( 0 ), getWindowSize() );
	gl::ScopedMatrices scopeMat;
	gl::setMatricesWindow( getWindowSize() );
	gl::ScopedGlslProg	  scopeGlsl( mEdgeBlendGlsl );
	
	mEdgeBlendGlsl->uniform( "edges", vec4( 0.0, mEdgeWidth, 0.0, 0.0 ) );
	{
		auto tex = getBottomPresentationTarget()->getColorTexture();
		gl::ScopedTextureBind scopeTex( tex );

		gl::drawSolidRect( Rectf( vec2( 0 ), tex->getSize() ) );
	}
	
	mEdgeBlendGlsl->uniform( "edges", vec4( 0.0, 0.0, 0.0, mEdgeWidth ) );
	{
		auto tex = getTopPresentationTarget()->getColorTexture();
		
		gl::ScopedModelMatrix scopeMat;
		gl::ScopedTextureBind scopeTex( tex );
		gl::translate( vec2( 0, getWindowHeight() ) / 2.0f );
		gl::drawSolidRect( Rectf( vec2( 0 ), tex->getSize() ) );
	}
}
	
}