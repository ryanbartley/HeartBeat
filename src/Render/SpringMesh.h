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
	
	void registerTouch( ci::vec2 touch );
	
private:
	SpringMesh();
	
	void initialize( const ci::JsonTree &root, const ci::vec2 &size );
	
	void loadShaders();
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
	bool							mDrawDebug, mTouchMoving, mDrawTexture;
	ci::vec2						mCurrentTouch;
	
	uint32_t						mNumRows, mNumColumns, mNumIterations;
	
	friend class Pond;
};
	
}
