//
//  SpringMesh.h
//  SpringMeshGpu
//
//  Created by Ryan Bartley on 10/20/14.
//
//

#pragma once

#include "Common.h"
#include "Renderable.h"

#include "cinder/gl/TransformFeedbackObj.h"
#include "cinder/gl/BufferTexture.h"

namespace heartbeat {
	
class SpringMesh {
public:
	
	static SpringMeshRef create();
	
	void update();
	//! This may need to ping pong.
	void project( const ci::gl::Texture2dRef &tex );
	void debugRender();
	
	~SpringMesh() {}
	
	void registerTouchBegan( ci::vec2 touch );
	void registerTouchMoved( ci::vec2 touch );
	
    void enableDrawDebug( bool enable ) { mDrawDebug = enable; }
    bool drawDebug() { return mDrawDebug; }
	
	float getTouchesMovedDistThresh() const { return mTouchesMovedDistThresh; }
	float getTouchesBeganDistThresh() const { return mTouchesBeganDistThresh; }
	
	void setTouchesMovedDistThresh( float thresh ) { mTouchesMovedDistThresh = thresh;}
	void setTouchesBeganDistThresh( float thresh ) { mTouchesBeganDistThresh = thresh; }
	
	void loadShaders();
    
    void setSpringConstant( float constant ) { mSpringConstant = constant; }
    void setRestLength( float restLength ) { mRestLength = restLength; }
    void setGlobalDampening( float globalDampening ) { mGlobalDampening = globalDampening; }
    
    float getSpringConstant() { return mSpringConstant; }
    float getRestLength() { return mRestLength; }
    float getGlobalDampening() { return mGlobalDampening; }
	
private:
	SpringMesh();
	
	void initialize( const ci::JsonTree &root, const ci::vec2 &size );
	
	
	void loadBuffers( const ci::vec2 &size );

	ci::gl::GlslProgRef				mUpdateGlsl, mDebugRenderGlsl, mRenderGlsl;
	ci::gl::VaoRef					mVaos[2];
	ci::gl::VboRef					mPositions[2], mVelocities[2], mNormals, mTexCoords,
									mConnections, mTriangleElementBuffer,
									mLineElementBuffer;
	ci::gl::TransformFeedbackObjRef mFeedbackObjs[2];
	ci::gl::BufferTextureRef		mPosBufferTextures[2];
	uint32_t						mIterationsPerFrame, mIterationIndex,
									mLineIndices, mTriangleIndices, mPointTotal;
	bool							mDrawDebug, mDrawTexture;
	ci::Timer						mUpdateTimer;
    
    float                           mSpringConstant, mRestLength, mGlobalDampening;
	
	uint32_t						mNumRows, mNumColumns, mNumIterations;
	float							mTouchesBeganDistThresh, mTouchesMovedDistThresh;
	
	std::vector<ci::vec2>			mTouchesBegan, mTouchesMoved;
	
	friend class Pond;
};
	
}
