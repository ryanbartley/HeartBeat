//
//  Output.h
//  ProjectionTestApp
//
//  Created by Ryan Bartley on 9/29/14.
//
//

#pragma once

#include "Common.h"

#include "cinder/gl/Fbo.h"
#include "cinder/gl/Texture.h"

namespace heartbeat {
	
using ScreenRendererRef = std::shared_ptr<class ScreenRenderer>;
using Viewport = std::pair<ci::vec2, ci::vec2>;

class ScreenRenderer {
public:
	
	static ScreenRendererRef create();
	
	~ScreenRenderer() {}
	
	void setImageStencil( const std::string &fileName );
	void setEdgePercentages( float width );
	void setNumPixelOverlap( uint32_t numPixelOverlap );
	void setTotalRenderSize( const ci::vec2 &totalSize );
	void setIndividualProjectorSize( const ci::vec2 &individualSize );
	void setHalfSize( bool halfSize ) { mIsHalfSized = halfSize; }
	void setAppWindowSize( const ci::vec2 &size );
	
	const ci::gl::FboRef&		getRenderTarget();
	const ci::gl::FboRef&		getBottomPresentationTarget();
	const ci::gl::FboRef&		getTopPresentationTarget();
	const Viewport&				getViewport() { return mViewport; }
	const ci::gl::TextureRef&	getImageStencil() { return mImageStencil; }
	const float					getEdgeWidth() { return mEdgeWidth; }
	const uint32_t				getNumPixelOverlap() { return mNumPixelOverlap; }
	
	void initialize();
	void beginFrame();
	void endFrame();
	void presentRender();
	void renderToWindow();

	void setupGlsl();
	
	const ci::vec2& getTotalRenderSize() { return mTotalRenderSize; }
	
private:
	ScreenRenderer();
	
	void setupFbos();
	
	void stencilTargetRenderFbo();
	
	void renderToPresent( uint32_t target );
	
	std::array<ci::gl::FboRef, 3>	mFbos;
	ci::gl::Texture2dRef			mImageStencil;
	ci::gl::GlslProgRef				mEdgeBlendGlsl, mAlphaDiscard;
	Viewport						mViewport;
	float							mEdgeWidth;
	uint32_t						mNumPixelOverlap;
	bool							mStencilImageUpdated, mIsHalfSized;
	ci::vec2						mTotalRenderSize, mIndividualProjectorSize;
	
};

}