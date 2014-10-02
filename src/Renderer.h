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
	
using RendererRef = std::shared_ptr<class Renderer>;

class Renderer {
public:
	
	static RendererRef create();
	
	Renderer( const Renderer & ) = delete;
	Renderer( Renderer && ) = delete;
	Renderer& operator=( const Renderer & ) = delete;
	Renderer& operator=( Renderer && ) = delete;
	
	~Renderer() {}
	
	const ci::gl::FboRef&		getRenderTarget() const;
	const ci::gl::FboRef&		getBottomPresentationTarget() const;
	const ci::gl::FboRef&		getTopPresentationTarget() const;
	const ci::gl::TextureRef&	getImageStencil() const { return mImageStencil; }
	const float					getEdgeWidth() const { return mEdgeWidth; }
	const uint32_t				getNumPixelOverlap() const { return mNumPixelOverlap; }
	const ci::vec2&				getTotalRenderSize() const { return mTotalRenderSize; }
	
private:
	Renderer();
	
	void initialize();
	void beginFrame();
	void endFrame();
	void presentRender();
	
	void setIsHalfSize( bool halfSize ) { mIsHalfSized = halfSize; }
	void setIsSplit( bool isSplit ) { mIsSplitWindow = isSplit; }
	
	void setImageStencil( const std::string &fileName );
	void setEdgeBlendWidth( float width );
	void setNumPixelOverlap( uint32_t numPixelOverlap );
	void setTargetRenderSize( const ci::vec2 &totalSize );
	void setIndividualProjectorSize( const ci::vec2 &individualSize );
	void setupPresentation();
	
	void setupGlsl();
	void setupFbos();
	
	void stencilTargetRenderFbo();
	
	//! Render's the Render_Target to the seperate Present_Targets'
	void renderToPresentTarget( uint32_t target );
	//! Writes the Present_Targets to the Window
	void renderToSingleWindow();
	void renderToBottomWindow();
	void renderToTopWindow();
	
	//! 0 - BOTTOM_PRESENT_TARGET, 1 - TOP_PRESENT_TARGET, 2 - RENDER_TARGET
	std::array<ci::gl::FboRef, 3>				mFbos;
	//! 0 - BOTTOM_PRESENT_TARGET, 1 - TOP_PRESENT_TARGET
	std::array<ci::app::WindowRef, 2>			mWindows;
	
	std::vector<boost::signals2::connection>	mDrawSignals;
	ci::gl::Texture2dRef						mImageStencil;
	ci::gl::GlslProgRef							mEdgeBlendGlsl, mAlphaDiscard;
	ci::vec2									mTotalRenderSize, mIndividualProjectorSize;
	float										mEdgeWidth;
	uint32_t									mNumPixelOverlap;
	bool										mStencilImageUpdated, mIsHalfSized,
												mIsSplitWindow;
	
	friend class Engine;
};

}