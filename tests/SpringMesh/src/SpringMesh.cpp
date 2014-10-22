#include "SpringMesh.h"
//
#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
using namespace ci;
using namespace ci::app;

float gDiffuseBase		= 0.75f;
Color gDiffuseAdjust	= Color( 0, 61.0f/255.0f, 64.0f/255.0f );
float gSpecularContrib	= 1.05f;
float gSpecularPower	= 15.0f;
Color gSpecularColor	= Color( 0.8f, 0.95f, 1.3f );

template <typename VecT>
SpringMeshT<VecT>::SpringMeshT()
{
}

template <typename VecT>
SpringMeshT<VecT>::SpringMeshT( const ci::Rectf& aBounds, int aResX, int aResY )
{
	initialize( aBounds, aResX, aResY );
}

template <typename VecT>
void SpringMeshT<VecT>::initialize( const ci::Rectf& aBounds, int aResX, int aResY )
{
	mBounds = aBounds;
	mBounds.x1 -= 50.0f;
	mBounds.x2 += 50.0f;
	mBounds.y1 -= 50.0f;
	mBounds.y2 += 50.0f;
	mResX = aResX;
	mResY = aResY;
	mHorizAndVertSpringForceCount = 0;

	console() << "Requested mResX: " << mResX << ", mRexY: " << mResY << std::endl;

	float dx = mBounds.getWidth()/(float)(mResX - 1);
	float dy = mBounds.getHeight()/(float)(mResY - 1);

	dx = dy;
	mResX = (float)mBounds.getWidth()/dx;
	
	console() << "Actual mResX: " << mResX << ", mRexY: " << mResY << std::endl;

	mMassPoints.clear();
	for( int j = 0; j < mResY; ++j ) {
		float y = j*dy + mBounds.y1;
		for( int i = 0; i < mResX; ++i ) {			
			float x = i*dx + mBounds.x1;
			VecT P = Ops<VecT>::VecT( x, y );
			ci::vec2 uv = ci::vec2( (float)i/(float)(mResX - 1), (float)j/(float)(mResY -1 ) );
			mMassPoints.push_back( MassPointT<VecT>( P, uv ) );
			//
			mNormals.push_back( vec3( 0, -1, 0 ) );
		}
	}
	//
	mNormals4.resize( mNormals.size() );

	for( int i = 0; i < mResX; ++i ) {
		getMassPoint( i, 0 ).setNoReponse();
		getMassPoint( i, mResY - 1 ).setNoReponse();
	}

	for( int j = 0; j < mResY; ++j ) {
		getMassPoint( 0, j ).setNoReponse();
		getMassPoint( mResX - 1, j ).setNoReponse();
	}

	// Vertical springs
	for( int j = 1; j < mResY; ++j ) {
		int j0 = j - 1;
		int j1 = j;
		for( int i = 0; i < mResX; ++i ) {
			MassPointT<VecT>* a = &(getMassPoint( i, j0 ) );
			MassPointT<VecT>* b = &(getMassPoint( i, j1 ) );
			SpringForceT<VecT> sf = SpringForceT<VecT>( a, b );
			mSpringForces.push_back( sf );
		}
	}

	// Horizontal springs
	for( int i = 1; i < mResX; ++i ) {
		int i0 = i - 1;
		int i1 = i;
		for( int j = 0; j < mResY; ++j ) {
			MassPointT<VecT>* a = &(getMassPoint( i0, j ) );
			MassPointT<VecT>* b = &(getMassPoint( i1, j ) );
			SpringForceT<VecT> sf = SpringForceT<VecT>( a, b );
			mSpringForces.push_back( sf );
		}
	}

	// Mark the count here - we're going to use this to draw the grid lines
	mHorizAndVertSpringForceCount = mSpringForces.size();

	// Diagonal springs
	for( int j = 1; j < mResY; ++j ) {
		int j0 = j - 1;
		int j1 = j;
		for( int i = 1; i < mResX; ++i ) {
			int i0 = i - 1;
			int i1 = i;
			// Diagonal 1 
			{
				MassPointT<VecT>* a = &(getMassPoint( i0, j0 ) );
				MassPointT<VecT>* b = &(getMassPoint( i1, j1 ) );
				SpringForceT<VecT> sf = SpringForceT<VecT>( a, b );
				mSpringForces.push_back( sf );
			}
			// Diagonal 2 
			{
				MassPointT<VecT>* a = &(getMassPoint( i1, j0 ) );
				MassPointT<VecT>* b = &(getMassPoint( i0, j1 ) );
				SpringForceT<VecT> sf = SpringForceT<VecT>( a, b );
				mSpringForces.push_back( sf );
			}
		}
	}

	for( auto iter = mSpringForces.begin(); iter != mSpringForces.end(); ++iter ) {
		iter->setRestLengthFromMassPoints();
		iter->setStiffness( 30.0f );
	}
	
	mMesh = TriMesh::create();
	
	int perVertexPerFaceCount = 2*3*(mResX - 1)*(mResY - 1);
	mMesh->getBufferPositions().resize( perVertexPerFaceCount * 3 );
	mMesh->getIndices().resize( perVertexPerFaceCount );
	mMesh->getBufferTexCoords0().resize( perVertexPerFaceCount * 2 );
	mMesh->getNormals().resize( perVertexPerFaceCount );
}

template <typename VecT>
int SpringMeshT<VecT>::calcIndex( int x, int y ) const
{
	int result = y*mResX + x;
	return result;
}

template <typename VecT>
MassPointT<VecT>& SpringMeshT<VecT>::getMassPoint( int x, int y )
{
	int idx = calcIndex( x, y );
	return mMassPoints[idx];
}

template <typename VecT>
const MassPointT<VecT>& SpringMeshT<VecT>::getMassPoint( int x, int y ) const
{
	int idx = calcIndex( x, y );
	return mMassPoints[idx];
}

template <typename VecT>
MassPointT<VecT>* SpringMeshT<VecT>::getClosestMassPoint( const VecT& aPos )
{
	MassPointT<VecT>* result = nullptr;
	float minDistSq = std::numeric_limits<float>::max();
	for( auto iter = mMassPoints.begin(); iter != mMassPoints.end(); ++iter ) {
		VecT dv = aPos - iter->getPos();
		float lengthSquared = length2( dv );
		if( lengthSquared < minDistSq ) {
			minDistSq = lengthSquared;
			result = &(*iter);
		}
	}
	return result;
}

template <typename VecT>
void SpringMeshT<VecT>::applyCircleForce( const float aForceAmt, const ci::vec2& aPos, float aRadius )
{
	//const VecT inP = Ops<VecT>::Vec( aPos );

	const float kRadiusSquared = aRadius*aRadius;
	for( auto iter = mMassPoints.begin(); iter != mMassPoints.end(); ++iter ) {
		//VecT dv = iter->getPos() - inP;
		VecT P = iter->getPos();
		vec2 dv = vec2( P.x - aPos.x, P.y - aPos.y );
		float lengthSquared = ci::length2( dv );
		if( lengthSquared < kRadiusSquared ) {
			float falloff = 1.0f - lengthSquared/kRadiusSquared;
			falloff = falloff*falloff;
			//VecT ndv = dv.normalized();
			//VecT force = falloff*aForceAmt*ndv;
			VecT force = falloff*Ops<VecT>::DownForce( aForceAmt, dv );
			iter->addForce( force );
		}
	}

/*
	const float kRadiusSquared = aRadius*aRadius;
	for( auto iter = mMassPoints.begin(); iter != mMassPoints.end(); ++iter ) {
		VecT dv = iter->getPos() - inP;
		float lengthSquared = dv.lengthSquared();
		if( lengthSquared < kRadiusSquared ) {
			float falloff = 1.0f - lengthSquared/kRadiusSquared;
			falloff = falloff*falloff;
			VecT ndv = dv.normalized();
			VecT force = falloff*aForceAmt*ndv;
			iter->addForce( force );
		}
	}
*/
}

template <typename VecT>
void SpringMeshT<VecT>::update( float dt )
{
	const int kNumIters = 1;
	const float subDt = dt/(float)kNumIters;

	for( int i = 0; i < kNumIters; ++i ) {
		for( auto iter = mSpringForces.begin(); iter != mSpringForces.end(); ++iter ) {
			iter->update();
		}

		for( auto iter = mMassPoints.begin(); iter != mMassPoints.end(); ++iter ) {
			iter->update( subDt );
		}
	}

	for( int j = 1; j < (mResY - 1); ++j ) {
		for( int i = 1; i < (mResX - 1); ++i ) {

			vec3 Q = Ops<VecT>::Vec3( getMassPoint( i - 1, j - 1 ).getPos() );
			vec3 T = Ops<VecT>::Vec3( getMassPoint( i, j - 1 ).getPos() );
			vec3 L = Ops<VecT>::Vec3( getMassPoint( i - 1, j ).getPos() );
			vec3 C = Ops<VecT>::Vec3( getMassPoint( i, j ).getPos() );
			vec3 R = Ops<VecT>::Vec3( getMassPoint( i + 1, j ).getPos() );
			vec3 B = Ops<VecT>::Vec3( getMassPoint( i, j + 1 ).getPos() );
			vec3 W = Ops<VecT>::Vec3( getMassPoint( i + 1, j + 1 ).getPos() );
			//
			vec3 V0 = T - C;
			vec3 V1 = Q - C;
			vec3 V2 = L - C;
			vec3 V3 = B - C;
			vec3 V4 = W - C;
			vec3 V5 = R - C;
			vec3 N0 = ci::cross( V1, V0 );
			vec3 N1 = ci::cross( V2, V1 );
			vec3 N2 = ci::cross( V3, V2 );
			vec3 N3 = ci::cross( V4, V3 );
			vec3 N4 = ci::cross( V5, V4 );
			vec3 N5 = ci::cross( V0, V5 );
			//
			vec3 N = (N0 + N1 + N2 + N3 + N4 + N5)/6.0f;

/*
			vec3 T = Ops<VecT>::Vec3( getMassPoint( i, j - 1 ).getPos() );
			vec3 L = Ops<VecT>::Vec3( getMassPoint( i - 1, j ).getPos() );
			vec3 C = Ops<VecT>::Vec3( getMassPoint( i, j ).getPos() );
			vec3 R = Ops<VecT>::Vec3( getMassPoint( i + 1, j ).getPos() );
			vec3 B = Ops<VecT>::Vec3( getMassPoint( i, j + 1 ).getPos() );
			//
			vec3 V0 = T - C;
			vec3 V1 = L - C;
			vec3 V2 = R - C;
			vec3 V3 = B - C;
			vec3 N0 = V1.cross( V0 );
			vec3 N1 = V0.cross( V2 );
			vec3 N2 = V3.cross( V1 );
			vec3 N3 = V2.cross( V3 );
			vec3 N = (N0 + N1 + N2 + N3)/4.0f;
			//N.normalize();
*/

			//
			int idx = calcIndex( i, j );
			mNormals[idx] = N;
			mNormals4[idx] = vec4( N, 0.0f );
		}
	}

	for( int i = 0; i < mNormals4.size(); ++i ) {
		mNormals4[i] = ci::normalize( mNormals4[i] );
	}
	//
	for( int i = 0; i < mNormals.size(); ++i ) {
		mNormals[i] = ci::normalize( vec3( mNormals4[i] ) );
	}

	int faceIdx = 0;
	for( int j = 1; j < mResY; ++j ) {
		int j0 = j - 1;
		int j1 = j;
		for( int i = 1; i < mResX; ++i, ++faceIdx ) {
			int i0 = i - 1;
			int i1 = i;
			//
			const MassPointT<VecT>& mp0 = getMassPoint( i0, j0 );
			const MassPointT<VecT>& mp1 = getMassPoint( i0, j1 );
			const MassPointT<VecT>& mp2 = getMassPoint( i1, j1 );
			const MassPointT<VecT>& mp3 = getMassPoint( i1, j0 );
			vec3 P0 = Ops<VecT>::Vec3( mp0.getPos() );
			vec3 P1 = Ops<VecT>::Vec3( mp1.getPos() );
			vec3 P2 = Ops<VecT>::Vec3( mp2.getPos() );
			vec3 P3 = Ops<VecT>::Vec3( mp3.getPos() );
			//
			//P0.z *= 1.25f;
			//P1.z *= 1.25f;
			//P2.z *= 1.25f;
			//P3.z *= 1.25f;
			//
			int idx0 = calcIndex( i0, j0 );
			int idx1 = calcIndex( i0, j1 );
			int idx2 = calcIndex( i1, j1 );
			int idx3 = calcIndex( i1, j0 );
			vec3 N0 = mNormals[idx0];
			vec3 N1 = mNormals[idx1];
			vec3 N2 = mNormals[idx2];
			vec3 N3 = mNormals[idx3];
			//vec3 N0 = (P3 - P0).cross( P1 - P0 );
			//vec3 N1 = (P0 - P1).cross( P2 - P1 );
			//vec3 N2 = (P1 - P2).cross( P3 - P2 );
			//vec3 N3 = (P2 - P3).cross( P0 - P3 );
			// Triangle 1
			{
				int v0 = 6*faceIdx + 0;
				int v1 = 6*faceIdx + 1;
				int v2 = 6*faceIdx + 2;
				mMesh->getPositions<3>()[v0] = P0;
				mMesh->getPositions<3>()[v1] = P1;
				mMesh->getPositions<3>()[v2] = P2;
				mMesh->getTexCoords0<2>()[v0] = mp0.getUv();
				mMesh->getTexCoords0<2>()[v1] = mp1.getUv();
				mMesh->getTexCoords0<2>()[v2] = mp2.getUv();
				mMesh->getIndices()[v0] = v0;
				mMesh->getIndices()[v1] = v1;
				mMesh->getIndices()[v2] = v2;
				mMesh->getNormals()[v0] = N0;
				mMesh->getNormals()[v1] = N1;
				mMesh->getNormals()[v2] = N2;
			}
			// Triangle 2
			{
				int v0 = 6*faceIdx + 3;
				int v1 = 6*faceIdx + 4;
				int v2 = 6*faceIdx + 5;
				mMesh->getPositions<3>()[v0] = P0;
				mMesh->getPositions<3>()[v1] = P2;
				mMesh->getPositions<3>()[v2] = P3;
				mMesh->getTexCoords0<2>()[v0] = mp0.getUv();
				mMesh->getTexCoords0<2>()[v1] = mp2.getUv();
				mMesh->getTexCoords0<2>()[v2] = mp3.getUv();
				mMesh->getIndices()[v0] = v0;
				mMesh->getIndices()[v1] = v1;
				mMesh->getIndices()[v2] = v2;
				mMesh->getNormals()[v0] = N0;
				mMesh->getNormals()[v1] = N2;
				mMesh->getNormals()[v2] = N3;
			}
		}
	}

	{
		if( mLinePoints.empty() ) {
			size_t n = 2*mHorizAndVertSpringForceCount;
			mLinePoints.resize( n );
		}

		for( size_t i = 0; i < mHorizAndVertSpringForceCount; ++i ) {
			VecT P0 = mSpringForces[i].getMassPointA()->getPos();
			VecT P1 = mSpringForces[i].getMassPointB()->getPos();
			//
			size_t lpIdx = 2*i;
			mLinePoints[lpIdx + 0] = P0;
			mLinePoints[lpIdx + 1] = P1;
		}

		if( ! mLineVbo && ! mLinePoints.empty() ) {
			mLineVbo = ci::gl::Vbo::create( GL_ARRAY_BUFFER );
		}

		if( mLineVbo ) {
			size_t dataSizeBytes = mLinePoints.size()*sizeof(VecT);
			mLineVbo->bufferData( dataSizeBytes, &mLinePoints[0], GL_STATIC_DRAW );
		}
	}
}

void drawTexturedQuad( 
	const vec2& P0, const vec2& P1, const vec2& P2, const vec2& P3, 
	const vec2& uv0, const vec2& uv1, const vec2& uv2, const vec2& uv3, 
	const Color& color0, const Color& color1, const Color& color2, const Color& color3, 
	bool bQuadInterp = true )
{

	vec3 uvq0 = vec3( uv0.x, uv0.y, 1 );
	vec3 uvq1 = vec3( uv1.x, uv1.y, 1 );
	vec3 uvq2 = vec3( uv2.x, uv2.y, 1 );
	vec3 uvq3 = vec3( uv3.x, uv3.y, 1 );

	if( bQuadInterp ) {
		float x1 = P0.x;
		float y1 = P0.y;
		float x2 = P2.x;
		float y2 = P2.y;
		float x3 = P1.x;
		float y3 = P1.y;
		float x4 = P3.x;
		float y4 = P3.y;

		float m0 = (x1*y2 - y1*x2);
		float m1 = (x3*y4 - y3*x4);
		float a = m0*(x3 - x4) - (x1 - x2)*m1;
		float b = m0*(y3 - y4) - (y1 - y2)*m1;
		float invDenom = 1.0f/( (x1 - x2)*(y3 - y4) - (y1 - y2)*(x3 - x4) );
		vec2 c = vec2( a*invDenom, b*invDenom );
		float d0 = (P0 - c).length();
		float d1 = (P1 - c).length();
		float d2 = (P2 - c).length();
		float d3 = (P3 - c).length();
		float s0 = (d0 + d2)/d2;
		float s1 = (d1 + d3)/d3;
		float s2 = (d2 + d0)/d0;
		float s3 = (d3 + d1)/d1;

		uvq0 = s0*uvq0;
		uvq1 = s1*uvq1;
		uvq2 = s2*uvq2;
		uvq3 = s3*uvq3;
	}


	gl::begin( GL_TRIANGLES );

	// Triangle 1
	gl::color( color0 );
	gl::texCoord( uvq0.x, uvq0.y, 0, uvq0.z );
	gl::vertex( P0 );

	gl::color( color1 );
	gl::texCoord( uvq1.x, uvq1.y, 0, uvq1.z );
	gl::vertex( P1 );

	gl::color( color2 );
	gl::texCoord( uvq2.x, uvq2.y, 0, uvq2.z );
	gl::vertex( P2 );

	// Triangle 2
	gl::color( color0 );
	gl::texCoord( uvq0.x, uvq0.y, 0, uvq0.z );
	gl::vertex( P0 );

	gl::color( color2 );
	gl::texCoord( uvq2.x, uvq2.y, 0, uvq2.z );
	gl::vertex( P2 );

	gl::color( color3 );
	gl::texCoord( uvq3.x, uvq3.y, 0, uvq3.z );
	gl::vertex( P3 );

	//glEnd();
}

void drawTexturedQuad( 
	const vec3& P0, const vec3& P1, const vec3& P2, const vec3& P3, 
	const vec2& uv0, const vec2& uv1, const vec2& uv2, const vec2& uv3, 
	const Color& color0, const Color& color1, const Color& color2, const Color& color3, 
	bool bQuadInterp = true )
{
	// Triangle 1
	//glColor3f( color0 );
	gl::texCoord( uv0.x, uv0.y );
	gl::vertex( P0 );

	//glColor3f( color1 );
	gl::texCoord( uv1.x, uv1.y );
	gl::vertex( P1 );

	//glColor3f( color2 );
	gl::texCoord( uv2.x, uv2.y );
	gl::vertex( P2 );

	// Triangle 2
	//glColor3f( color0 );
	gl::texCoord( uv0.x, uv0.y );
	gl::vertex( P0 );

	//glColor3f( color2 );
	gl::texCoord( uv2.x, uv2.y );
	gl::vertex( P2 );

	//glColor3f( color3 );
	gl::texCoord( uv3.x, uv3.y );
	gl::vertex( P3 );
}

template <typename VecT>
void SpringMeshT<VecT>::draw()
{
	if( mLineVbo ) {
		gl::disableAlphaBlending();

//		glLineWidth( 0.5f );
		gl::color( Color( 1, 1, 0 ) );

		// TODO: FIx this.
		
//		mLineVbo->bind();
//		glEnableClientState( GL_VERTEX_ARRAY );
//		glVertexPointer( VecT::DIM, GL_FLOAT, 0, 0 );
		//
//		glDrawArrays( GL_LINES, 0, mLinePoints.size() );
		//
//		glDisableClientState( GL_VERTEX_ARRAY );
//		mLineVbo.unbind();
	}

/*
	glLineWidth( 0.75f );
	gl::color( Color( 1, 1, 0 ) );


	if( mLineVbo ) {
		mLineVbo.bind();
		glEnableClientState( GL_VERTEX_ARRAY );
		glVertexPointer( VecT::DIM, GL_FLOAT, 0, 0 );
		//
		glDrawArrays( GL_POINTS, 0, mLinePoints.size() );
		//
		glDisableClientState( GL_VERTEX_ARRAY );
		mLineVbo.unbind();
	}
*/


	for( auto iter = mSpringForces.begin(); iter != mSpringForces.end(); ++iter ) {
		VecT P0 = iter->getMassPointA()->getPos();
		VecT P1 = iter->getMassPointB()->getPos();
		gl::drawLine( P0, P1 );
	}


	glPointSize( 5.0f );
	gl::begin( GL_POINTS );
	for( auto iter = mMassPoints.begin(); iter != mMassPoints.end(); ++iter ) {
		VecT P = iter->getPos();
		gl::color( Color( 1, 0, 0 ) );
		gl::vertex( P );
	}
	gl::end();

/*
	for( auto iter = mMassPoints.begin(); iter != mMassPoints.end(); ++iter ) {
		vec2 N = iter->getVel();
		float length = 100.0f; //N.length();
		//length = std::min( length, 50.0f );

		vec2 P0 = iter->getPos();
		vec2 P1 = P0 + length*N;

		gl::color( Color( 0, 1, 0 ) );
		gl::drawLine( P0, P1 );
	}
*/
}

template <typename VecT>
void SpringMeshT<VecT>::draw( ci::gl::Texture2dRef& aTex, ci::gl::GlslProgRef& aShader )
{
	if( ! aTex ) {
		return;
	}

	float t = (float)app::getElapsedSeconds();
	gl::ScopedTextureBind scopeTex( aTex );
	gl::ScopedGlslProg		scopeGlsl( aShader );
	
	aShader->uniform( "tex0", 0 );
	aShader->uniform( "lightPos", vec3( 3000, -360, 60 ) );
	//aShader.uniform( "specularPower", 6.0f );
	aShader->uniform( "diffuseBase", gDiffuseBase );
	aShader->uniform( "diffuseAdjust", vec3( gDiffuseAdjust.r, gDiffuseAdjust.g, gDiffuseAdjust.b ) );
	aShader->uniform( "specularContrib", gSpecularContrib );
	aShader->uniform( "specularPower", gSpecularPower );
	aShader->uniform( "specularColor", vec3( gSpecularColor.r, gSpecularColor.g, gSpecularColor.b ) );
	aShader->uniform( "time", 4.0f*t );
	gl::draw( *mMesh );

	// Draw the mesh
	//draw();

/*
	//gl::color( Color( 0.7f, 0.7f, 0.7f ) );

	glBegin( GL_TRIANGLES );

	for( int j = 1; j < mResY; ++j ) {
		int j0 = j - 1;
		int j1 = j;
		for( int i = 1; i < mResX; ++i ) {
			int i0 = i - 1;
			int i1 = i;

			const MassPointT<VecT>& mp0 = getMassPoint( i0, j0 );
			const MassPointT<VecT>& mp1 = getMassPoint( i0, j1 );
			const MassPointT<VecT>& mp2 = getMassPoint( i1, j1 );
			const MassPointT<VecT>& mp3 = getMassPoint( i1, j0 );
			VecT P0 = mp0.getPos();
			VecT P1 = mp1.getPos();
			VecT P2 = mp2.getPos();
			VecT P3 = mp3.getPos();
			ci::vec2 uv0 = mp0.getUv();
			ci::vec2 uv1 = mp1.getUv();
			ci::vec2 uv2 = mp2.getUv();
			ci::vec2 uv3 = mp3.getUv();

			float invS = 0.5f; //1.0f / 1000000.0f;
			float a0 = 0.6f + mp0.getVel().lengthSquared()*invS;
			float a1 = 0.6f + mp1.getVel().lengthSquared()*invS;
			float a2 = 0.6f + mp2.getVel().lengthSquared()*invS;
			float a3 = 0.6f + mp3.getVel().lengthSquared()*invS;
			a0 = ci::math<float>::clamp( a0 );
			a1 = ci::math<float>::clamp( a1 );
			a2 = ci::math<float>::clamp( a2 );
			a3 = ci::math<float>::clamp( a3 );
			Color color0 = Color( a0*0.7f, a0*1.25f, a0*1.5f );
			Color color1 = Color( a1*0.7f, a1*1.25f, a1*1.5f );
			Color color2 = Color( a2*0.7f, a2*1.25f, a2*1.5f );
			Color color3 = Color( a3*0.7f, a3*1.25f, a3*1.5f );

			drawTexturedQuad( P0, P1, P2, P3, uv0, uv1, uv2, uv3, color0, color1, color2, color3 );
		}
	}

	glEnd();
*/
}

template class SpringMeshT<vec2>;
template class SpringMeshT<vec3>;
