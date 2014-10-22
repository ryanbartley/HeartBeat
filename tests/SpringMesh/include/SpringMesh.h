#pragma once

#include "MassPoint.h"
#include "SpringForce.h"
#include "AlignedAllocator.h"
//
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/Vbo.h"
#include "cinder/Rect.h"
#include "cinder/TriMesh.h"
//
#include <vector>

/**
 * \class SpringMeshT
 *
 */
template <typename VecT>
class SpringMeshT {
public:

	SpringMeshT();
	SpringMeshT( const ci::Rectf& aBounds, int aResX, int aResY );
	~SpringMeshT() {}

	void							initialize( const ci::Rectf& aBounds, int aResX, int aResY );

	int								calcIndex( int x, int y ) const;
	MassPointT<VecT>&				getMassPoint( int x, int y ) ;
	const MassPointT<VecT>&			getMassPoint( int x, int y ) const;

	MassPointT<VecT>*				getClosestMassPoint( const VecT& aPos );

	void							applyCircleForce( const float aForceAmt, const ci::vec2& aPos, float aRadius );

	void							update( float dt );
	void							draw();
	void							draw( ci::gl::Texture2dRef& aTex, ci::gl::GlslProgRef& aShader );

private:
	
	
	ci::Rectf						mBounds;
	int								mResX;
	int								mResY;
	std::vector<MassPointT<VecT>>	mMassPoints;
	std::vector<SpringForceT<VecT>>	mSpringForces;
	int								mHorizAndVertSpringForceCount;

	std::vector<ci::vec4>			mNormals4;
	std::vector<ci::vec3>			mNormals;
	ci::TriMeshRef					mMesh;

	std::vector<VecT>				mLinePoints;
	ci::gl::VboRef					mLineVbo;
};

typedef SpringMeshT<ci::vec2>	SpringMesh2;
typedef SpringMeshT<ci::vec3>	SpringMesh3;
