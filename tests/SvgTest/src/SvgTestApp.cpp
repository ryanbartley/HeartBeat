#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/gl/Texture.h"

#include "cinder/svg/Svg.h"
#include "Cairo.h"

#include "SvgManager.h"
#include "JsonManager.h"
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
	heartbeat::ButtonRef		mButton;
	heartbeat::SvgManagerRef	mManager;
	heartbeat::JsonManagerRef	mJsonManager;
	ci::gl::Texture2dRef		mTexture;
};

// Renders a given SVG group 'groupName' into a new gl::Texture
gl::TextureRef renderSvgGroupToTexture( const svg::Doc &doc, const std::string &group, bool alpha )
{
//	auto& nodes = dynamic_cast<svg::Group*>(const_cast<svg::Node*>(doc.findNode( group )))->getChildren();
//	for( auto & node : nodes ) {
//		cout << node->getId() << endl;
//	}
//	return gl::Texture2dRef();
	auto node = dynamic_cast<svg::Group*>(const_cast<svg::Node*>(doc.findNode( group )))->findByIdContains<svg::Group>("active");
	auto rect = node->getBoundingBox();
	cout << "Rect " << rect << endl;
	cairo::SurfaceImage srfImg( rect.getWidth() + 10, rect.getHeight() + 10, alpha );
	cairo::Context ctx( srfImg );
	
	ctx.setAntiAlias( 16 );
	ctx.translate( -rect.x1 + 5, -rect.y1 + 5 );
	ctx.render( *node );
	return gl::Texture::create( srfImg.getSurface() );
	
}

void SvgTestApp::setup()
{
//	mDoc = svg::Doc::create( loadAsset("Actemra_V1.svg") );
//	MatrixAffine2<float> transform = mDoc->getTransform();
//	transform.scale( vec2(getWindowSize()) / vec2(mDoc->getSize()) );
//	mDoc->setTransform( transform );
	mJsonManager = heartbeat::JsonManager::create( "test.json" );
	
	mManager = heartbeat::SvgManager::create();
	mManager->initialize();
	
//	mButton = heartbeat::Button::create( "BUTTONS" );
	mDoc = mManager->getDoc();
	mTexture = renderSvgGroupToTexture( *mDoc, "D2", true );
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
	gl::clear( Color::black() );
	gl::setMatricesWindow( getWindowSize() );
	
	
	if( mTexture ) {
		gl::enableAlphaBlending();
		gl::color( Color::white() );
		gl::draw( mTexture, vec2( 0 ) );
	}
}

CINDER_APP_NATIVE( SvgTestApp, RendererGl )
