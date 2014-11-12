//
//  Node.cpp
//  SvgTest
//
//  Created by Ryan Bartley on 10/27/14.
//
//

#include "Node.h"
#include "Cairo.h"
#include "cinder/Log.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace heartbeat {
	
Node::Node( const std::string &group )
: mGroup( dynamic_cast<const svg::Group*>( SvgManager::get()->getDoc()->findNode( group ) ) ),
	mName(group), mAntiAlias( 0 ), mOffset( vec2( 10 ) ), mCurrentAlpha( 1.0f ), mIsAnimating( false ), 
	mCurrentPosition( vec2( mGroup->getBoundingBox().x1, mGroup->getBoundingBox().y1 ) ), mAbsolutePosition( vec2( mGroup->getBoundingBox().x1 - mOffset.x / 2.0f, mGroup->getBoundingBox().y1 - mOffset.y / 2.0f ) )
{
	if( ! mGroup ) {
		CI_LOG_E(mName << " wasn't able to be found");
	}
	else {
		CI_LOG_V(mGroup->getId() << " was initialized correctly");
	}
	
	if( mName == "BUTTON-STUDY-close" ) {
		cout << "TopLines display is " << ! mGroup->isDisplayNone() << endl;
		cout << "Absolute: " << mGroup->getBoundingBox() << endl;
	}
}
	
ci::gl::Texture2dRef Node::initializeGl( const ci::svg::Group *group )
{
	auto rect = group->getBoundingBox();
	cairo::SurfaceImage srfImg( rect.getWidth() + mOffset.x, rect.getHeight() + mOffset.y, true );
	cairo::Context ctx( srfImg );
	
	ctx.setAntiAlias( mAntiAlias );
	ctx.translate( -rect.x1 + (mOffset.x / 2), -rect.y1 + (mOffset.y / 2) );
	ctx.render( *group );
	return gl::Texture::create( srfImg.getSurface() );
}
	
Page::Page( const std::string &name )
: Node( name )
{
}
	
Button::Button( const std::string &name )
: Node(name), mBoundingBox( Rectf( mGroup->getBoundingBox().x1 - 40, mGroup->getBoundingBox().y1 - 40, mGroup->getBoundingBox().x2 + 40, mGroup->getBoundingBox().y2 + 40 ) )
{
}
	
void Button::renderBoundingBox()
{
	gl::drawStrokedRect( mBoundingBox, 5 );
}
	
}