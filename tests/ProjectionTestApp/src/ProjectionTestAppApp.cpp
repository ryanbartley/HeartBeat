#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "JsonManager.h"
#include "ScreenRenderer.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ProjectionTestAppApp : public AppNative {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void fileDrop( FileDropEvent event ) override;
	void keyDown( KeyEvent event ) override;
	void update() override;
	void draw() override;
	
	void renderTopPresentationTarget();
	void renderBottomPresentationTarget();
	
	heartbeat::JsonManagerRef mJsonManager;
	heartbeat::ScreenRendererRef mScreenRenderer;
	
	CameraPersp		mCamera;
	bool			mShowTop;
};

void ProjectionTestAppApp::setup()
{
	using namespace heartbeat;
	mJsonManager = heartbeat::JsonManager::create( "test.json" );
	mScreenRenderer = heartbeat::ScreenRenderer::create();
	mScreenRenderer->initialize();
	
	auto aspect = mScreenRenderer->getTotalRenderSize();
	
	mCamera.setPerspective( 60.0f, aspect.x / aspect.y , .01f, 1000.0f );
	mCamera.lookAt( vec3( 0, 0, 5 ), vec3( 0 ) );
	
	mShowTop = true;
}

void ProjectionTestAppApp::mouseDown( MouseEvent event )
{
	mScreenRenderer->setupGlsl();
}

void ProjectionTestAppApp::keyDown( KeyEvent event )
{
	if( event.getCode() == KeyEvent::KEY_SPACE )
		mShowTop = !mShowTop;
}

void ProjectionTestAppApp::fileDrop( FileDropEvent event )
{
	mScreenRenderer->setImageStencil( event.getFile( 0 ).filename().string() );
}

void ProjectionTestAppApp::update()
{
	
}

void ProjectionTestAppApp::draw()
{
	mScreenRenderer->beginFrame();
	{
		gl::ScopedMatrices scopeMat;
		gl::setMatricesWindow( mScreenRenderer->getTotalRenderSize() );
		gl::color( 1, 0, 0 );
		gl::drawSolidRect( Rectf( vec2( 0 ), mScreenRenderer->getTotalRenderSize() ) );
	}
	
	
	{
		gl::enableDepthRead();
		gl::enableDepthWrite();
		
		static float rotation = 0.0f;
		gl::ScopedMatrices scopeMat;
		gl::setMatrices( mCamera );
		gl::color( 1, 1, 1 );
		gl::multModelMatrix( rotate( rotation += 0.01f, vec3( 0, 1, 0 ) ) );
		gl::drawColorCube( vec3( 0 ), vec3( 2 ) );
		
		gl::disableDepthRead();
		gl::disableDepthWrite();
	}
	mScreenRenderer->endFrame();
	mScreenRenderer->presentRender();
}

void ProjectionTestAppApp::renderTopPresentationTarget()
{
	
}

void ProjectionTestAppApp::renderBottomPresentationTarget()
{
	
}

CINDER_APP_NATIVE( ProjectionTestAppApp, RendererGl )
