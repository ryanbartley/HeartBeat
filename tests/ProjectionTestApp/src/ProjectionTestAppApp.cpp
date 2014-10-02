#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "Engine.h"
#include "JsonManager.h"
#include "Renderer.h"

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
	
	heartbeat::EngineRef mEngine;
	
	
	bool			mShowTop;
};

void ProjectionTestAppApp::setup()
{
	using namespace heartbeat;
	
	mEngine = heartbeat::Engine::create();
	
	mShowTop = true;
}

void ProjectionTestAppApp::mouseDown( MouseEvent event )
{
	
}

void ProjectionTestAppApp::keyDown( KeyEvent event )
{
	if( event.getCode() == KeyEvent::KEY_SPACE )
		mShowTop = !mShowTop;
}

void ProjectionTestAppApp::fileDrop( FileDropEvent event )
{
//	mEngine->getRenderer()->setImageStencil( event.getFile( 0 ).filename().string() );
}

void ProjectionTestAppApp::update()
{
	
}

CINDER_APP_NATIVE( ProjectionTestAppApp, RendererGl )
