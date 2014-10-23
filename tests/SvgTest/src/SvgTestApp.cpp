#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/gl/Texture.h"

#include "cinder/svg/Svg.h"
#include "Cairo.h"

#include "SvgManager.h"
#include "Button.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class SvgTestApp : public AppNative {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
	
	heartbeat::ButtonRef& getButton() { return mButton; }
	
	svg::DocRef mDoc;
	heartbeat::ButtonRef mButton;
	heartbeat::SvgManagerRef mManager;
};

// Renders a given SVG group 'groupName' into a new gl::Texture
gl::TextureRef renderSvgGroupToTexture( const svg::Doc &doc, heartbeat::ButtonRef& button, const Rectf &rect, bool alpha )
{
	cairo::SurfaceImage srfImg( rect.getWidth(), rect.getHeight(), alpha );
	cairo::Context ctx( srfImg );
	ctx.scale( rect.getWidth() / doc.getWidth(), rect.getHeight() / doc.getHeight() );
//	ctx.render( *button->getGroup()  );
	return gl::Texture::create( srfImg.getSurface() );
}

void SvgTestApp::setup()
{
//	mDoc = svg::Doc::create( loadAsset("Actemra_V1.svg") );
//	MatrixAffine2<float> transform = mDoc->getTransform();
//	transform.scale( vec2(getWindowSize()) / vec2(mDoc->getSize()) );
//	mDoc->setTransform( transform );
	
	mManager = heartbeat::SvgManager::create( "Actemra_V1.svg" );
	mManager->initialize();
	
	mButton = heartbeat::Button::create( "BUTTONS" );
	
}

void SvgTestApp::mouseDown( MouseEvent event )
{
	auto node = mDoc->nodeUnderPoint( event.getPos() );
	if( node ) {
		auto found = false;
		const svg::Node* prepNode = node;
		while ( ! found ) {
			auto id = prepNode->getId();
			if( id.empty() ) {
				prepNode = prepNode->getParent();
			}
			else {
				cout << id << endl;
				cout << prepNode->getParent()->getId() << endl;
				found = true;
			}
		}
	}
}

void SvgTestApp::update()
{
}

void SvgTestApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
	gl::setMatricesWindow( getWindowSize() );
	
	auto doc = mManager->getDoc();
	
	gl::draw( renderSvgGroupToTexture( *(doc), mButton, Rectf( vec2( 0 ), doc->getSize() ), false ) );
}

CINDER_APP_NATIVE( SvgTestApp, RendererGl )
