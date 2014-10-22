#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "Pond.h"
#include "JsonManager.h"
#include "InteractionEvents.h"
#include "cinder/gl/Fbo.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class SpringMeshGpuApp : public AppNative {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void mouseDrag( MouseEvent event ) override;
	void update() override;
	void draw() override;
	
	heartbeat::PondRef mPond;
	heartbeat::JsonManagerRef mJsonManager;
	
	gl::FboRef mPing;
};

void SpringMeshGpuApp::setup()
{
	
	gl::Fbo::Format fboFormat;
	
	fboFormat.depthBuffer();

	mPing = gl::Fbo::create( getWindowWidth(), getWindowHeight(), fboFormat );
	
	mJsonManager = heartbeat::JsonManager::create("test.json");
	mPond = heartbeat::Pond::create( getWindowSize() );
	mPond->initialize();
}

void SpringMeshGpuApp::mouseDown( MouseEvent event )
{
	mPond->getSpringMesh()->registerTouch( event.getPos() );
}

void SpringMeshGpuApp::mouseDrag(cinder::app::MouseEvent event)
{
	mPond->getSpringMesh()->registerTouch( event.getPos() );
}

void SpringMeshGpuApp::update()
{
	mPond->update();
}

void SpringMeshGpuApp::draw()
{
	{
		gl::ScopedFramebuffer scopeFbo( mPing );
		gl::clear( Color( 0, 0, 0 ) );
		mPond->renderPondElements();
	}
	
	gl::clear( Color( 0, 0, 0 ) );
	mPond->projectPondElements( mPing->getColorTexture() );
}

CINDER_APP_NATIVE( SpringMeshGpuApp, RendererGl )
