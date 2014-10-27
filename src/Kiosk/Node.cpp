//
//  Node.cpp
//  SvgTest
//
//  Created by Ryan Bartley on 10/27/14.
//
//

#include "Node.h"
#include "Cairo.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace heartbeat {
	
Node::Node( const std::string &group )
: mGroup( dynamic_cast<const svg::Group*>( SvgManager::get()->getDoc()->findNode( group ) ) ),
	mName(group)
{
}
	
ci::gl::Texture2dRef Node::initializeGl( const ci::svg::Group *group )
{
	auto rect = group->getBoundingBox();
	cairo::SurfaceImage srfImg( rect.getWidth() + mOffset.x, rect.getHeight() + mOffset.y, true );
	cairo::Context ctx( srfImg );
	
	ctx.setAntiAlias( 16 );
	ctx.translate( -rect.x1 + (mOffset.x / 2), -rect.y1 + (mOffset.y / 2) );
	ctx.render( *mGroup );
	return gl::Texture::create( srfImg.getSurface() );
}
	
Page::Page( const std::string &name )
: Node( name )
{
}
	
void Page::initializeGl()
{
	mTexture = Node::initializeGl( mGroup );
}
	
Button::Button( const std::string &name )
: Node(name), mStatus(ButtonStatus::INVISIBLE)
{
}
	
}