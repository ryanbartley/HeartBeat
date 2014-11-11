//
//  SpringMesh.cpp
//  SpringMeshGpu
//
//  Created by Ryan Bartley on 10/20/14.
//
//

#include "SpringMesh.h"

#include "cinder/gl/Vbo.h"
#include "cinder/gl/Vao.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/Log.h"
#include "cinder/Rand.h"
#include "JsonManager.h"
#include "Engine.h"
#include "Renderer.h"
#include "cinder/app/Window.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace heartbeat {
	
const int POSITION_INDEX	= 0;
const int VELOCITY_INDEX	= 1;
const int NORMAL_INDEX		= 2;
const int CONNECTION_INDEX	= 3;
const int TEXCOORD_INDEX	= 4;

	
SpringMesh::SpringMesh()
: mIterationsPerFrame(1), mIterationIndex( 0 ), mLineIndices( 0 ), mTriangleIndices( 0 ), mDrawDebug( true ), mDrawTexture( true )
{
	
}
	
SpringMeshRef SpringMesh::create()
{
	return SpringMeshRef( new SpringMesh );
}
	
void SpringMesh::update()
{
	gl::ScopedGlslProg	updateScope( mUpdateGlsl );
	gl::ScopedState		stateScope( GL_RASTERIZER_DISCARD, true );
	
	if( ! mTouchesBegan.empty() ) {
		mUpdateGlsl->uniform( "touchesBegan", mTouchesBegan.data(), mTouchesBegan.size() );
	}
	if( ! mTouchesMoved.empty() ) {
		mUpdateGlsl->uniform( "touchesMoved", mTouchesMoved.data(), mTouchesMoved.size() );
	}
	
	mUpdateGlsl->uniform( "numTouchesBegan", (int)mTouchesBegan.size() );
	mUpdateGlsl->uniform( "numTouchesMoved", (int)mTouchesMoved.size() );
	
	// This for loop allows iteration on the gpu of solving the
	// physics of the cloth.
	// Change mIterationsPerFrame to see the difference it makes
	for( int i = mIterationsPerFrame; i != 0; --i ) {
		ci::Timer updatetime;
		updatetime.start();
		// Pick using the mouse if it's pressed
		mUpdateGlsl->uniform( "elapsedSeconds", float(getElapsedSeconds()) );
		
		gl::ScopedVao			vaoScope( mVaos[mIterationIndex & 1] );
		gl::ScopedTextureBind	textureBind( mPosBufferTextures[mIterationIndex & 1]->getTarget(),
											mPosBufferTextures[mIterationIndex & 1]->getId() );
		
		mIterationIndex++;
		
		mFeedbackObjs[mIterationIndex & 1]->bind();
		
		gl::beginTransformFeedback( GL_POINTS );
		gl::drawArrays( GL_POINTS, 0, mPointTotal );
		gl::endTransformFeedback();
	}
	
	mTouchesMoved.clear();
	mTouchesBegan.clear();
}

void SpringMesh::project( const ci::gl::Texture2dRef &tex )
{
	// Need to figure this out.
	gl::ScopedGlslProg	scopeGlsl( mRenderGlsl );
	gl::ScopedBuffer	scopeElements( mTriangleElementBuffer );
	gl::ScopedTextureBind scopeTexture( tex, 0 );
	
	mRenderGlsl->uniform( "tex", 0 );
	
	gl::setDefaultShaderVars();
	
	gl::drawElements( GL_TRIANGLES, mTriangleIndices, GL_UNSIGNED_INT, 0 );
}
	
void SpringMesh::debugRender()
{
	if( ! mDrawDebug )
		return;
	
	// Render the Line and/or Point version of the flag
	gl::ScopedGlslProg	scopeGlsl( mDebugRenderGlsl );
	gl::ScopedVao		scopeVao( mVaos[mIterationIndex & 1] );
//	gl::multModelMatrix( ci::rotate( toRadians(90.0f), vec3( 0, 1, 0 ) ) );
	
	gl::setDefaultShaderVars();
	
	glPointSize( 6.0f );
	gl::drawArrays( GL_POINTS, 0, mPointTotal );
	gl::ScopedBuffer bufferScope( mLineElementBuffer );
	gl::drawElements( GL_LINES, mLineIndices, GL_UNSIGNED_INT, nullptr );
}

void SpringMesh::initialize( const ci::JsonTree &root, const ci::vec2 &size )
{
	// TODO: Figure out exactly what stuff we want to initialize with
	try {
		mNumRows = root["numRows"].getValue<uint32_t>();
		mNumColumns = root["numColumns"].getValue<uint32_t>();
		mNumIterations = root["numIterations"].getValue<uint32_t>();
	}
	catch ( JsonTree::ExcChildNotFound &ex ) {
		CI_LOG_E("No SpringMesh attributes found" << ex.what() );
	}
	loadShaders();
	loadBuffers( size );
}
	
void SpringMesh::registerTouchBegan( ci::vec2 currentTouch )
{
	mTouchesBegan.push_back( currentTouch );
}
	
void SpringMesh::registerTouchMoved( ci::vec2 touch )
{
	mTouchesMoved.push_back( touch );
}

void SpringMesh::loadBuffers( const ci::vec2 &size )
{
	int n = 0;
	
    auto renderer = Engine::get()->getRenderer();
    auto window = renderer->getPrimaryWindow();
    window->getRenderer()->makeCurrentContext();
    
	const int POINTS_X			= mNumRows + 1;
	const int POINTS_Y			= mNumColumns + 1;
	const int POINTS_TOTAL		= (POINTS_X * POINTS_Y);
	const int CONNECTIONS_TOTAL	= (POINTS_X - 1) * POINTS_Y + (POINTS_Y - 1) * POINTS_X;
	
	mPointTotal = POINTS_TOTAL;
	
	vector<vec4> positions( mPointTotal );
	vector<vec3> velocities( mPointTotal );
	vector<vec2> texCoords( mPointTotal );
	vector<vec3> normals( mPointTotal, vec3( 0, 0, 1.0 ) );
	// We set all connections to -1, because these will only be updated
	// if there are connection indices. Explanation below.
	vector<ivec4> connections( mPointTotal, ivec4( -1 ) );
	cout << "THE SIZE OF THE POND: " <<size << endl;
	for( int j = 0; j < POINTS_Y; j++ ) {
		float fj = (float)j / (float)(POINTS_Y - 1);
		for( int i = 0; i < POINTS_X; i++ ) {
			float fi = (float)i / (float)(POINTS_X - 1);
			
			// This fills the position buffer data, basically makes a grid
			positions[n] = vec4((fi) * (float)size.x,	// x coordinate
								(fj) * (float)size.y,	// y coordinate
								0,						// z coordinate
								1.0f);					// mass
			texCoords[n] = vec2( fi, 1.0f - fj );
			// This allows us to figure out the indices of the four points
			// surrounding the current point. This will be used to index
			// into the texture buffer.
			if( j != (POINTS_Y - 1) && i != (POINTS_X - 1) && j != 0 && i != 0 ) {	// if it's not one of the top row, don't move
				if( i != 0 )
					connections[n][0] = n - 1;
				if( j != 0 )
					connections[n][1] = n - POINTS_X;
				if( i != (POINTS_X - 1) )
					connections[n][2] = n + 1;
				if( j != (POINTS_Y - 1) )
					connections[n][3] = n + POINTS_X;
			}
			n++;
		}
	}
	
	// Create the Position Buffer with the intial position data
	mPositions[0] = gl::Vbo::create( GL_ARRAY_BUFFER, positions.size() * sizeof(vec4), positions.data(), GL_STATIC_DRAW );
	// Create another Position Buffer that is null, for ping-ponging
	mPositions[1] = gl::Vbo::create( GL_ARRAY_BUFFER, positions.size() * sizeof(vec4), nullptr, GL_STATIC_DRAW );
	
	// Create the Velocity Buffer with the intial velocity data
	mVelocities[0] = gl::Vbo::create( GL_ARRAY_BUFFER, velocities.size() * sizeof(vec3), velocities.data(), GL_STATIC_DRAW );
	// Create another Velocity Buffer that is null, for ping-ponging
	mVelocities[1] = gl::Vbo::create( GL_ARRAY_BUFFER, velocities.size() * sizeof(vec3), nullptr, GL_STATIC_DRAW );
	
	// Create the tex coords buffer.
	mTexCoords = gl::Vbo::create( GL_ARRAY_BUFFER, texCoords.size() * sizeof(vec2), texCoords.data(), GL_STATIC_DRAW );
	
	mNormals = gl::Vbo::create( GL_ARRAY_BUFFER, normals.size() * sizeof(vec3), normals.data(), GL_STATIC_DRAW );
	
	// Create Connection Buffer to index into the Texture Buffer
	mConnections = gl::Vbo::create( GL_ARRAY_BUFFER, connections.size() * sizeof(ivec4), connections.data(), GL_STATIC_DRAW );
	
	for( int i = 0; i < 2; i++ ) {
		// Initialize the Vao's holding the info for each buffer
		mVaos[i] = gl::Vao::create();
		
		// Bind the vao to capture index data for the glsl
		mVaos[i]->bind();
		mPositions[i]->bind();
		gl::vertexAttribPointer( POSITION_INDEX, 4, GL_FLOAT, GL_FALSE, 0, nullptr );
		gl::enableVertexAttribArray( POSITION_INDEX );
		
		mVelocities[i]->bind();
		gl::vertexAttribPointer( VELOCITY_INDEX, 3, GL_FLOAT, GL_FALSE, 0, nullptr );
		gl::enableVertexAttribArray( VELOCITY_INDEX );
		
		mNormals->bind();
		gl::vertexAttribPointer( NORMAL_INDEX, 3, GL_FLOAT, GL_FALSE, 0,  nullptr );
		gl::enableVertexAttribArray( NORMAL_INDEX );
		
		mConnections->bind();
		gl::vertexAttribIPointer( CONNECTION_INDEX, 4, GL_INT, 0, nullptr );
		gl::enableVertexAttribArray( CONNECTION_INDEX );
		
		mTexCoords->bind();
		gl::vertexAttribPointer( TEXCOORD_INDEX, 2, GL_FLOAT, GL_FALSE, 0, nullptr );
		gl::enableVertexAttribArray( TEXCOORD_INDEX );
		
		
		
		// Create a TransformFeedbackObj, which is similar to Vao
		// It's used to capture the output of a glsl and uses the
		// index of the feedback's varying variable names.
		mFeedbackObjs[i] = gl::TransformFeedbackObj::create();
		
		// Bind the TransformFeedbackObj and bind each corresponding buffer
		// to it's index.
		mFeedbackObjs[i]->bind();
		gl::bindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, POSITION_INDEX, mPositions[i] );
		gl::bindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, VELOCITY_INDEX, mVelocities[i] );
		gl::bindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, NORMAL_INDEX, mNormals );
		mFeedbackObjs[i]->unbind();
		
		// Create Texture buffers to gain access to the lookup tables for
		// calculations in the update shader
		mPosBufferTextures[i] = gl::BufferTexture::create( mPositions[i], GL_RGBA32F );
	}
	
	// Create an element buffer to draw the lines (connections) between the points
	vector<uint32_t> lineIndices(CONNECTIONS_TOTAL*2);
	uint32_t * e = lineIndices.data();
	for( int j = 0; j < POINTS_Y; j++ ) {
		for( int i = 0; i < POINTS_X - 1; i++ ) {
			*e++ = i + j * POINTS_X;
			*e++ = 1 + i + j * POINTS_Y;
		}
	}
	
	for( int i = 0; i < POINTS_X; i++ ) {
		for( int j = 0; j < POINTS_Y - 1; j++ ) {
			*e++ = i + j * POINTS_X;
			*e++ = POINTS_X + i + j * POINTS_X;
		}
	}
	
	mLineIndices = lineIndices.size();
	mLineElementBuffer = gl::Vbo::create( GL_ELEMENT_ARRAY_BUFFER, lineIndices.size() * sizeof(uint32_t), lineIndices.data(), GL_STATIC_DRAW );
	
	vector<uint32_t> triangleIndices;
	int x, y;
	for( x = 0; x < mNumRows; x++ ) {
		for( y = 0; y < mNumColumns; y++ ) {
			uint32_t i = x * ( mNumColumns + 1 ) + y;
			triangleIndices.push_back( i );
			triangleIndices.push_back( i + 1 );
			triangleIndices.push_back( i + mNumColumns + 1 );
			
			triangleIndices.push_back( i + mNumColumns + 1 );
			triangleIndices.push_back( i + 1 );
			triangleIndices.push_back( i + mNumColumns + 2 );
		}
	}
	mTriangleIndices = triangleIndices.size();
	mTriangleElementBuffer = gl::Vbo::create( GL_ELEMENT_ARRAY_BUFFER, triangleIndices.size() * sizeof(uint32_t), triangleIndices.data(), GL_STATIC_DRAW );
}

void SpringMesh::loadShaders()
{
	// Create a vector of Transform Feedback "Varyings".
	// These strings tell OpenGL what to look for when capturing
	// Transform Feedback data. For instance, tf_position_mass,
	// and tf_velocity are variables in the update.vert that we
	// write our calculations to.
	std::vector<std::string> varyings(3);
	varyings[POSITION_INDEX] = "tf_position_mass";
	varyings[VELOCITY_INDEX] = "tf_velocity";
	varyings[NORMAL_INDEX] = "tf_normal";
	
	gl::GlslProg::Format updateFormat;
	// Notice that we don't offer a fragment shader. We don't need
	// one because we're not trying to write pixels while updating
	// the position, velocity, etc. data to the screen.
	updateFormat.vertex( getFileContents( "SpringMeshupdate.vert" ) )
	// This option will be either GL_SEPARATE_ATTRIBS or GL_INTERLEAVED_ATTRIBS,
	// depending on the structure of our data, below. We're using multiple
	// buffers. Therefore, we're using GL_SEPERATE_ATTRIBS
	.feedbackFormat( GL_SEPARATE_ATTRIBS )
	// Pass the varyings to the glsl
	.feedbackVaryings( varyings )
	.attribLocation( "position_mass",	POSITION_INDEX )
	.attribLocation( "velocity",		VELOCITY_INDEX )
	.attribLocation( "normal",			NORMAL_INDEX )
	.attribLocation( "connection",		CONNECTION_INDEX )
	.attribLocation( "tex_coord",		TEXCOORD_INDEX );
	
	try {
		mUpdateGlsl = gl::GlslProg::create( updateFormat );
	}
	catch( const gl::GlslProgCompileExc &ex ) {
		CI_LOG_E("Update Shader Compile Exc " << ex.what());
	}
	catch( const ci::Exception &ex ) {
		CI_LOG_E("Unknown exception " << ex.what() );
	}
	
//	std::array<float, 8> amplitude{ 1, 1, .4, .03, .05, .1, .5, .3 };
//	std::array<float, 8> wavelength{ .8, .2, .1, .01, .1, .4, .5, .8 };
//	std::array<float, 8> speed{ 1, 1, .4, .01, .6, .9, .1, .4 };
//	std::array<vec2, 8> direction{ randVec2f(), randVec2f(), randVec2f(), randVec2f(), randVec2f(), randVec2f(), randVec2f(), randVec2f() };
//
//	mUpdateGlsl->uniform( "amplitude", amplitude.data(), amplitude.size() );
//	mUpdateGlsl->uniform( "wavelength", wavelength.data(), wavelength.size() );
//	mUpdateGlsl->uniform( "speed", speed.data(), speed.size() );
//	mUpdateGlsl->uniform( "direction", direction.data(), direction.size() * 2 );
//	mUpdateGlsl->uniform( "mouse_pos", vec2(640, 480) );
	
	gl::GlslProg::Format debugRenderFormat;
	debugRenderFormat.vertex( getFileContents( "SpringMeshDebugrender.vert" ) )
	.fragment( getFileContents( "SpringMeshDebugrender.frag" ) )
	.attribLocation( "position", POSITION_INDEX );
	
	try {
		mDebugRenderGlsl = gl::GlslProg::create( debugRenderFormat );
	}
	catch( const gl::GlslProgCompileExc &ex ) {
		CI_LOG_E("Debug Render Shader Compile Exc " << ex.what());
	}
	catch( const ci::Exception &ex ) {
		CI_LOG_E("Unknown exception " << ex.what() );
	}
	
	gl::GlslProg::Format renderFormat;
	renderFormat.vertex( getFileContents( "SpringMeshrender.vert" ) )
	.fragment( getFileContents( "SpringMeshrender.frag" ) )
	.attribLocation( "position",	POSITION_INDEX )
	.attribLocation( "normal",		NORMAL_INDEX )
	.attribLocation( "texCoord",	TEXCOORD_INDEX );
	
	try {
		mRenderGlsl = gl::GlslProg::create( renderFormat );
	}
	catch ( const gl::GlslProgCompileExc &ex ) {
		CI_LOG_E("Render Shader Compile Exc " << ex.what());
	}
	catch ( const ci::Exception &ex ) {
		CI_LOG_E("Unknown Exception " << ex.what());
	}
}
	
	
	
	
}