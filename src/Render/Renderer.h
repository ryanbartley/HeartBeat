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

class Renderer {
public:
	
	static RendererRef create();
	
	Renderer( const Renderer & ) = delete;
	Renderer( Renderer && ) = delete;
	Renderer& operator=( const Renderer & ) = delete;
	Renderer& operator=( Renderer && ) = delete;
	
	~Renderer();
	
	const ci::gl::FboRef&		getRenderTarget() const;
	const ci::gl::FboRef&		getPondTarget() const;
	const ci::gl::FboRef&		getBottomPresentationTarget() const;
	const ci::gl::FboRef&		getTopPresentationTarget() const;
	const ci::gl::TextureRef&	getImageStencil() const { return mImageStencil; }
	const float					getEdgeWidth() const { return mEdgeWidth; }
	const uint32_t				getNumPixelOverlap() const { return mNumPixelOverlap; }
	const ci::vec2&				getTotalRenderSize() const { return mTotalRenderSize; }
    
    bool isHalfSize() { return mIsHalfSized; }
	
private:
	Renderer();
	
	//! Initializes the renderer and loads Settings
	void initialize();
	//! Nullify's the holds on the windows and breaks the connections.
	void cleanup();
	
	//! Interface used by Engine to control the rendering.
	
	//! Sets up the fbo and the different flags for global rendering.
	void beginFrame();
	//! cleans up the fbo and the different flags for global rendering.
	void endFrame();
	//! Presents the render_target to the two present_targets. TODO: I should maybe look at
	//! blitting the fbo's, maybe more efficient and easier to think about?
	void presentRender();
	
	//! Sets the flag on whether the window(s) are halfSized. If false, the window(s)
	//! will be full screen.
	void setIsHalfSize( bool halfSize ) { mIsHalfSized = halfSize; }
	//! Sets the flag on whether the window should be split.
	void setIsSplit( bool isSplit ) { mIsSplitWindow = isSplit; }
	
	//! This sets and initializes the texture used for stenciling.
	void setImageStencil( const std::string &fileName );
	//! Sets the EdgeBlendWidth used in blending the two projector images together.
	void setEdgeBlendWidth( float width );
	//! Sets the number of pixels we want to overlap between the projectors. This also
	//! calculates what the final TargetRenderSize should be and the edgeWidth.
	void setNumPixelOverlap( uint32_t numPixelOverlap );
	//! Sets the Target Render Size. The size of the offscreen fbo, or 3 - RENDER_TARGET
	void setTargetRenderSize( const ci::vec2 &totalSize );
	//! This is the total area of the Individual Projector size. The size of the presentation
	//! Fbo's and the windows.
	void setIndividualProjectorSize( const ci::vec2 &individualSize );
	//! Finalizes settings and creates and caches the windows if need be. Also, configures the
	//! draw signals. There was a bug where I was calling this before I called setupGlsl and
	//! setupFbos and the draw signal was firing early. I moved it after those are setup.
	void setupPresentation();
	
	//! Creates glsl.
	void setupGlsl();
	//! Initializes Fbos. Called after initializing config.json.
	void setupFbos();
	
	//! Stencil's the stencilImage to the Render_Target Fbo.
	void stencilTargetRenderFbo();
	
	
	//! Render's the Render_Target to the seperate Present_Targets'
	void renderToPresentTarget( uint32_t target );
	//! Writes both of the Present_Targets to a single window.
	//! Used when config.json's splitWindow is false.
	void renderToSingleWindow();
	//! Render's Bottom_Present_target to bottom window.
	//! Used when config.json's splitWindow is true.
	void renderToBottomWindow();
	//! Render's Top_Present_Window to top window.
	//! Used when config.json's splitWindow is true.
	void renderToTopWindow();
	
	//! 0 - BOTTOM_PRESENT_TARGET, 1 - TOP_PRESENT_TARGET, 2 - RENDER_TARGET, 3 - POND_RENDERER
	std::array<ci::gl::FboRef, 4>				mFbos;
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