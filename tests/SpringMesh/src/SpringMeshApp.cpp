#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/Rand.h"

#include "SpringMesh.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class SpringMeshApp : public AppNative {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void mouseDrag( MouseEvent event ) override;
	void keyDown( KeyEvent event ) override;
	void update() override;
	void draw() override;
	
	void applyCircleForce( float forceAmt, const ci::vec2 &forcePos, float forceRadius );
	
	ci::gl::TextureRef				mBgTex;
	ci::gl::GlslProgRef				mShader;
	ci::Rectf						mInputRect;
	SpringMesh3						mSpringMesh;
	
	double							mLastTouchTime;
	bool							mSplatActive;
	bool							mDrawGrid;
};

void SpringMeshApp::setup()
{
	float height = getWindowHeight();
	float width = getWindowWidth();
	
	mSplatActive = true;
	mLastTouchTime = 0;
	
	float border = 10.0f;
	mInputRect = Rectf( border, border, width - border, height - border );
	
//	loadBg( "springmesh/anemone.png" );
	
	try {
		mShader = gl::GlslProg::create( gl::GlslProg::Format()
									   .vertex( loadAsset( "springmesh/springwave_vert.glsl" ) )
									   .fragment(  loadAsset( "springmesh/springwave_frag.glsl" ) ) );
	}
	catch( const std::exception& e ) {
		console() << "SpringWave Shader Error: " << e.what() << std::endl;
	}
	
	Rectf r = Rectf( 0, 0, width, height );
//#if defined( _DEBUG )
//	int numRows = 20;
//#else
//#if defined( RELEASE_HAI )
//	int numRows = 200;
//#else
//	int numRows = 175;
//#endif
//#endif
	int numRows = 20;
	int numCols = numRows * 2 * ( width / height ) + 2;
	mSpringMesh.initialize( r, numCols, numRows );
	
	mDrawGrid = true;

}

void SpringMeshApp::applyCircleForce( float aForceAmt, const ci::vec2& aForceP, float aForceRadius )
{
	if( mInputRect.contains( aForceP ) ) {
		mSpringMesh.applyCircleForce( aForceAmt, aForceP, aForceRadius );
	}
}

void SpringMeshApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 'g' ) {
		mDrawGrid = ! mDrawGrid;
	}
}

void SpringMeshApp::mouseDown( MouseEvent event )
{
	if( mSplatActive ) {
		mSplatActive = false;
	}
	
	applyCircleForce( 2*25000.0f, event.getPos(), 1.5f*50.0f );
	
	mLastTouchTime = getElapsedSeconds();
}

void SpringMeshApp::mouseDrag( MouseEvent event )
{
	applyCircleForce( 4*1500.0f, event.getPos(), 1.5f*30.0f );
	
	mLastTouchTime = getElapsedSeconds();
}

void SpringMeshApp::update()
{
	{
		double dt = getElapsedSeconds() - mLastTouchTime;
		if( ! mSplatActive && dt > 8.0f ) {
			mSplatActive = true;
		}
	}
	
	
	static float prevTime = (float)app::getElapsedSeconds();
	float curTime = (float)app::getElapsedSeconds();
	float dt = curTime - prevTime;
	prevTime = curTime;
	
	static float sLastSplatTime = 0;
	float splatDt = (curTime - sLastSplatTime );
	if( mSplatActive && splatDt > 1.25f ) {
		const float kBorder = 100.0f;
		float x = ci::randFloat( kBorder, getWindowHeight() - kBorder );
		float y = ci::randFloat( kBorder, getWindowHeight() - kBorder );
		
		float force  = ci::randFloat( 1.0f, 3.0f )*25000.0f;
		float radius = ci::randFloat( 1.0f, 2.5f )*50.0f;
		applyCircleForce( force, vec2( x, y ), radius );
		//
		sLastSplatTime = curTime;
	}
	//
	//	float timescale = getTimescale();
	mSpringMesh.update( 1.0f/60.0f * 3 );
}

void SpringMeshApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
	
	gl::viewport( getWindowSize() );
	gl::setMatricesWindowPersp( getWindowSize(), 15.0f, 1.0f, 10000.0f );
	gl::scale( 1, -1 );
	gl::translate( 0, (float)-getWindowHeight() );
	
	gl::enableDepthRead();
	gl::enableDepthWrite();
	
	gl::disableAlphaBlending();
	if( mDrawGrid ) {
		mSpringMesh.draw();
	}
	else {
		mSpringMesh.draw( mBgTex, mShader );
	}
}

CINDER_APP_NATIVE( SpringMeshApp, RendererGl )
