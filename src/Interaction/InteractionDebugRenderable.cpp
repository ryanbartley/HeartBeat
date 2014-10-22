//
//  InteractionDebug.cpp
//  LibUrgTest
//
//  Created by Ryan Bartley on 10/7/14.
//
//

#include "InteractionDebugRenderable.h"

#include "JsonManager.h"

#include "cinder/Log.h"
#include "InteractionZones.h"

#include "cinder/gl/Shader.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace heartbeat {
	
struct Vertex {
	Vertex( const vec2 &position, const Color &color )
	: mPosition( position ), mColor( color ) {}
	
	vec2 mPosition;
	Color mColor;
};
	
InteractionDebugRenderable::InteractionDebugRenderable( const InteractionZonesRef &interactionZones )
: Renderable(), mInteractionZones( interactionZones ), mDrawDistanceData( false ),
	mDrawZoneData( false ), mZoneCached( false )
{
	mInteractionZones->setZoneUpdateCallback( std::bind( &InteractionDebugRenderable::setupZoneGlBuffers, this ) );

	mBarrierVao = gl::Vao::create();
	mBarrierVbo = gl::Vbo::create( GL_ARRAY_BUFFER );
	
	gl::ScopedVao scopeVao( mBarrierVao );
	
	gl::ScopedBuffer scopeBuffer( mBarrierVbo );
	
	gl::enableVertexAttribArray( 0 );
	gl::vertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)nullptr );
	
	gl::enableVertexAttribArray( 1 );
	gl::vertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(vec2) );
}
	
InteractionDebugRenderableRef InteractionDebugRenderable::create( const InteractionZonesRef &interactionZones )
{
	return InteractionDebugRenderableRef( new InteractionDebugRenderable( interactionZones ) );
}
	
void InteractionDebugRenderable::initialize()
{
	try {
		auto interactionDebugAttribs = JsonManager::get()->getRoot()["interactionDebug"];
		
		Renderable::initialize( interactionDebugAttribs["transformation"] );
	}
	catch ( JsonTree::ExcChildNotFound &ex ) {
		CI_LOG_W("interactionDebug child not Found, Setting up with defaults");
		Renderable::initialize( JsonTree() );
	}
}
	
void InteractionDebugRenderable::setupZoneGlBuffers()
{
	CI_LOG_V("Setting Up the Buffer Zones");
	auto & barrier = mInteractionZones->getBarrier();
	auto & zones = mInteractionZones->getZones();
	
	std::vector<std::pair<float, ci::Color>> scalars;
	
	// Add the first one at origin
	scalars.push_back( make_pair( 0.0f, ci::Color( 1, 0, 0 ) ) );
	
	auto found = zones.find( InteractionZones::Zone::TABLE );
	
	if( found != zones.end() ) {
		scalars.push_back( make_pair( found->second, ci::Color( 1, 0, 0 ) ) );
		scalars.push_back( make_pair( found->second, ci::Color( .5, .7, .3 ) ) );
	}
	
	found = zones.find( InteractionZones::Zone::DEAD );
	
	if( found != zones.end() ) {
		scalars.push_back( make_pair( found->second, ci::Color( .5, .7, .3 ) ) );
		scalars.push_back( make_pair( found->second, ci::Color( 0, 1, .5 ) ) );
	}
	
	found = zones.find( InteractionZones::Zone::APPROACH );
	
	if( found != zones.end() ) {
		scalars.push_back( make_pair( found->second, ci::Color( 0, 1, .5 ) ) );
		scalars.push_back( make_pair( found->second, ci::Color( .6, .1, .8 ) ) );
	}
	
	found = zones.find( InteractionZones::Zone::FAR );
	
	if( found != zones.end() ) {
		scalars.push_back( make_pair( found->second, ci::Color( .6, .1, .8 ) ) );
	}
	
	auto urg = mInteractionZones->getUrg();
	std::vector<Vertex> buffer;
	
	int i = 0;
	for( auto & value : barrier ) {
		int currentIndex = i++;
		for( auto & scalar : scalars ) {
			buffer.emplace_back( vec2( urg->getPoint( currentIndex, value, scalar.first ) ), scalar.second );
		}
	}
	
	mNumBarrierVertices = buffer.size();
	
	mBarrierVbo->bufferData( buffer.size() * sizeof(Vertex), buffer.data(), GL_DYNAMIC_DRAW );
	
	mZoneCached = true;
}
	
void InteractionDebugRenderable::drawZones()
{
	if( ! mZoneCached ) return;
	
	gl::ScopedVao scopeVao( mBarrierVao );
	gl::ScopedGlslProg scopeGlsl( gl::getStockShader( gl::ShaderDef().color() ) );
	
	gl::setDefaultShaderVars();
	
	gl::drawArrays( GL_LINES, 0, mNumBarrierVertices );
}
	
void InteractionDebugRenderable::draw()
{
	if( ! mDrawDistanceData && ! mDrawZoneData ) return;
	
	gl::ScopedModelMatrix scopeModel;
	gl::setModelMatrix( getModelMatrix() );
	
	if( mDrawZoneData ) {
		cout << mDrawZoneData << endl;
		drawZones();
	}
	
	if( mDrawDistanceData ) {
		gl::begin( GL_LINES );
		gl::enableAlphaBlending();
		
		auto points = mInteractionZones->getUrg()->getDrawablePoints();

		gl::color( 1, 1, 1, 0.5f );
		for( auto & point : points ) {
			gl::vertex( vec2( 0 ) );
			gl::vertex( point );
		}
		
		gl::disableAlphaBlending();
		gl::end();
	}
}
	
}