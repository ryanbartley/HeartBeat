#include "ModSpringMesh.h"
//
#include "cinder/app/App.h"
#include "cinder/ImageIo.h"
#include "cinder/Rand.h"
using namespace ci;
using namespace ci::app;

extern int gWindowWidth;
extern int gWindowHeight;

extern Color gDiffuseAdjust;
extern Color gSpecularColor;

/**
 * \class ModSpringMesh
 *
 */
ModSpringMesh::ModSpringMesh()
: mDrawGrid( false )
{
	mSplatActive = true;
	mLastTouchTime = 0;

	float border = 10.0f;
	mInputRect = Rectf( border, border, gWindowWidth - border, gWindowHeight - border );

	loadBg( "springmesh/anemone.png" );

	try {
		mShader = gl::GlslProg::create( gl::GlslProg::Format()
									   .vertex( loadAsset( "springmesh/springwave_vert.glsl" ) )
									   .fragment(  loadAsset( "springmesh/springwave_frag.glsl" ) ) );
	}
	catch( const std::exception& e ) {
		console() << "SpringWave Shader Error: " << e.what() << std::endl;
	}

	Rectf r = Rectf( 0, 0, (float)gWindowWidth, (float)gWindowHeight );
#if defined( _DEBUG )
	int numRows = 20; 
#else
  #if defined( RELEASE_HAI )
	int numRows = 200;
  #else
	int numRows = 175;
  #endif
#endif 
	int numCols = numRows*2*((float)gWindowWidth/(float)gWindowHeight) + 2;
	mSpringMesh.initialize( r, numCols, numRows );
}

ModSpringMesh::~ModSpringMesh()
{
    
}

std::shared_ptr<ModSpringMesh> ModSpringMesh::create()
{
	std::shared_ptr<ModSpringMesh> result = std::shared_ptr<ModSpringMesh>( new ModSpringMesh() );
	return result;
}

void ModSpringMesh::loadBg( const std::string& relAssetPath )
{
	ci::Surface surf = loadImage( loadAsset( relAssetPath ) );
	if( surf ) {
		mBgTex = ci::gl::Texture::create( surf );
	}
}


void ModSpringMesh::applyCircleForce( float aForceAmt, const ci::vec2& aForceP, float aForceRadius )
{
	if( mInputRect.contains( aForceP ) ) {
		mSpringMesh.applyCircleForce( aForceAmt, aForceP, aForceRadius );
	}
}

void ModSpringMesh::setup()
{
//	mUiPanel = UiButtonPanelPtr( new UiButtonPanel() );
//	mUiPanel->setStyle( UiButtonPanel::Style::HORIZONTAL );
//	mUiPanel->setPos( getFoldedUiPos() );
//	mUiPanel->setSize( vec2( kDefaultUiSizeX*TOTAL_BUTTONS, kDefaultUiSizeY ) );
//	mUiPanel->setButtonCount( TOTAL_BUTTONS);
//	//
//	mUiPanel->getButton( BUTTON_0 )->setClickable( true );
//	mUiPanel->getButton( BUTTON_1 )->setClickable( true );
//	mUiPanel->getButton( BUTTON_2 )->setClickable( true );
//	mUiPanel->getButton( BUTTON_3 )->setClickable( true );
//	mUiPanel->getButton( BUTTON_4 )->setClickable( true );
//	//
//	mUiPanel->getButton( BUTTON_0 )->setActiveChangeCallback( std::bind( &ModSpringMesh::toggleDrawGrid, this, std::placeholders::_1 ) );
//	mUiPanel->getButton( BUTTON_1 )->setActiveChangeCallback( std::bind( &ModSpringMesh::toggleBg0, this, std::placeholders::_1 ) );
//	mUiPanel->getButton( BUTTON_2 )->setActiveChangeCallback( std::bind( &ModSpringMesh::toggleBg1, this, std::placeholders::_1 ) );
//	mUiPanel->getButton( BUTTON_3 )->setActiveChangeCallback( std::bind( &ModSpringMesh::toggleBg2, this, std::placeholders::_1 ) );
//	mUiPanel->getButton( BUTTON_4 )->setActiveChangeCallback( std::bind( &ModSpringMesh::toggleBg3, this, std::placeholders::_1 ) );
//	//
//	mUiPanel->getButton( BUTTON_0 )->setText( "Grid" );
//	mUiPanel->getButton( BUTTON_1 )->setText( "Bg 0" );
//	mUiPanel->getButton( BUTTON_2 )->setText( "Bg 1" );
//	mUiPanel->getButton( BUTTON_3 )->setText( "Bg 2" );
//	mUiPanel->getButton( BUTTON_4 )->setText( "Bg 3" );

//	mParams = params::InterfaceGl::create( "Oil Paint", Vec2i( 200, 500 ) );
//	mParams->addParam( "Glow Radius", &mGlowRadius, "min=0 max=10 step=0.01" );
//	mParams->addSeparator();
//	mParams->addParam( "Diffuse Adjust", &gDiffuseAdjust );
}

void ModSpringMesh::toggleDrawGrid( bool val )
{
	mDrawGrid = ! mDrawGrid;
//	if( mDrawGrid ) {
//		enableGlow( 1.4f );
//	}
//	else {
//		enableGlow( 4.0f );
//	}
}

void ModSpringMesh::toggleBg0( bool val )
{
	loadBg( "springmesh/anemone.png" );
	gDiffuseAdjust	= Color( 0, 61.0f/255.0f, 64.0f/255.0f );
	gSpecularColor	= Color( 0.8f, 0.95f, 1.5f );
}

void ModSpringMesh::toggleBg1( bool val )
{
	loadBg( "springmesh/cartier-shadows_1920x1080.png" );
	gDiffuseAdjust	= Color( 61.0f/255.0f, 61.0f/255.0f, 61.0f/255.0f );
	gSpecularColor	= Color( 0.8f, 0.8f, 0.8f );
}

void ModSpringMesh::toggleBg2( bool val )
{
	loadBg( "springmesh/kertez-eye_1920x1080.png" );
	gDiffuseAdjust	= Color( 61.0f/255.0f, 61.0f/255.0f, 61.0f/255.0f );
	gSpecularColor	= Color( 0.8f, 0.8f, 0.8f );
}

void ModSpringMesh::toggleBg3( bool val )
{
	loadBg( "springmesh/kertez-pool_1920x1080.png" );
	gDiffuseAdjust	= Color( 61.0f/255.0f, 61.0f/255.0f, 61.0f/255.0f );
	gSpecularColor	= Color( 0.8f, 0.8f, 0.8f );
}

void ModSpringMesh::touchBegan( int32_t touchId, const ci::vec2& touchPos, const ci::vec2& touchPrevPos )
{
	if( mSplatActive ) {
		mSplatActive = false;
	}

	applyCircleForce( 2*25000.0f, touchPos, 1.5f*50.0f );

	mLastTouchTime = getElapsedSeconds();
}

void ModSpringMesh::touchMoved( int32_t touchId, const ci::vec2& touchPos, const ci::vec2& touchPrevPos )
{
    applyCircleForce( 4*1500.0f, touchPos, 1.5f*30.0f );

	mLastTouchTime = getElapsedSeconds();
}

void ModSpringMesh::touchEnded( int32_t touchId, const ci::vec2& touchPos, const ci::vec2& touchPrevPos )
{
    
}

void ModSpringMesh::show()
{
//	Module::show();
//
//	glEnable( GL_LINE_SMOOTH );
//	glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
}

void ModSpringMesh::update()
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
		float x = ci::randFloat( kBorder, gWindowWidth - kBorder );
		float y = ci::randFloat( kBorder, gWindowHeight - kBorder );

		float force  = ci::randFloat( 1.0f, 3.0f )*25000.0f;
		float radius = ci::randFloat( 1.0f, 2.5f )*50.0f;		
		applyCircleForce( force, vec2( x, y ), radius );
		//
		sLastSplatTime = curTime;
	}
//
//	float timescale = getTimescale();
//	mSpringMesh.update( 1.0f/60.0f*timescale );    
}

void ModSpringMesh::render()
{
//	gl::setViewport( mRenderTarget1.getBounds() );
//	gl::setMatricesWindowPersp( mRenderTarget1.getSize(), 15.0f, 1.0f, 10000.0f );
//	gl::scale( 1, -1 );
//	gl::translate( 0, (float)-mRenderTarget1.getHeight() );
//	//
//	gl::clear( ColorA( 0, 0, 0, 0 ) );    
//
//	gl::enableDepthRead();
//	gl::enableDepthWrite();
//
//	gl::disableAlphaBlending();
//	if( mDrawGrid ) {
//		mSpringMesh.draw();
//	}
//	else {
//		mSpringMesh.draw( *mBgTex, mShader );
//	}
}
