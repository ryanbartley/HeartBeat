#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "Engine.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class hidTestApp : public AppNative {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
	
	heartbeat::EngineRef mEngine;
};

void hidTestApp::setup()
{
	mEngine = heartbeat::Engine::create();
}

void hidTestApp::mouseDown( MouseEvent event )
{
}

void hidTestApp::update()
{
}

void hidTestApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP_NATIVE( hidTestApp, RendererGl )
