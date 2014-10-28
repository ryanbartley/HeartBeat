//
//  InteractionDebug.cpp
//  LibUrgTest
//
//  Created by Ryan Bartley on 10/7/14.
//
//

#include "InteractionDebugRenderable.h"

#include "JsonManager.h"
#include "InteractionZones.h"
#include "InteractionEvents.h"
#include "EventManager.h"

#include "cinder/Log.h"
#include "cinder/gl/Shader.h"
#include "cinder/gl/Context.h"

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
	
	auto eventManager = EventManagerBase::get();
	if( eventManager ) {
		eventManager->addListener( std::bind( &InteractionDebugRenderable::touchDelegate, this, std::placeholders::_1 ), TouchEvent::TYPE );
	}
}
	
InteractionDebugRenderableRef InteractionDebugRenderable::create( const InteractionZonesRef &interactionZones )
{
	return InteractionDebugRenderableRef( new InteractionDebugRenderable( interactionZones ) );
}
	
void InteractionDebugRenderable::initialize()
{
	try {
		auto interactionDebugAttribs = JsonManager::get()->getRoot()["interactionZones"];
		
		Renderable::initialize( interactionDebugAttribs["transforms"] );
	}
	catch ( JsonTree::ExcChildNotFound &ex ) {
		CI_LOG_W("interactionDebug child not Found, Setting up with defaults");
		Renderable::initialize( JsonTree() );
	}
}
	
void InteractionDebugRenderable::setupZoneGlBuffers()
{
	CI_LOG_V("Setting Up the Buffer Zones");
	auto urg = mInteractionZones->getUrg();
	if( ! urg ) {
		CI_LOG_W("URG Not created");
		return;
	}
		
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
	
void InteractionDebugRenderable::touchDelegate( EventDataRef touchEvent )
{
	auto event = std::dynamic_pointer_cast<TouchEvent>( touchEvent );
	
	if( ! event ) {
		CI_LOG_V("Not an touchEvent " << touchEvent->getName() );
		return;
	}
	
	// Implement this.
	mTouchIndices.push_back( event->getIndex() );
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
		drawZones();
	}
	
	if( mDrawDistanceData ) {
		gl::begin( GL_LINES );
		gl::enableAlphaBlending();
		
		auto urg = mInteractionZones->getUrg();
		if( ! urg ) {
			static bool notified = false;
			if( !notified ) {
				CI_LOG_W("Urg not connected");
				notified = true;
			}
			return;
		}
		auto points = urg->getDrawablePoints();
		auto approachZones = mInteractionZones->getApproachZoneData();

		int i = 0;
		int k = 10;
		for( auto & point : points ) {
			auto isTouched = false;
			auto isApproached = false;
			// First check if this index is inside the TouchedIndices, if it is get rid of it.
			for( auto indexIt = mTouchIndices.begin(); indexIt != mTouchIndices.end(); ++indexIt ) {
				if( *indexIt == i ) {
					isTouched = true;
					mTouchIndices.erase(indexIt);
					break;
				}
			}
			// If it isn't touched and k is 0, or in other words we're through showing the area of touch
			// check to see if this is inside an activated approach zone.
			if( ! isTouched && k == 0 ) {
				for ( auto & approachZone : approachZones ) {
					if( approachZone.second.contains( i ) ) {
						isApproached = approachZone.second.getIsActivated();
					}
				}
			}
			// If this point isTouched provide a Red color and start the countDown at ten to show
			// an area.
			if( isTouched ) {
				gl::color( 1, 0, .5, 0.5f );
				k = 10;
			}
			else if( k <= 0 && !isApproached ) {
				gl::color( 1, 1, 1, 0.5f );
			}
			else if( k <= 0 && isApproached ) {
				gl::color( 1.0, 1.0, 0.0, 0.5f );
			}
			else {
				k--;
			}
			gl::vertex( vec2( 0 ) );
			gl::vertex( point );
			++i;
		}
		
		gl::disableAlphaBlending();
		gl::end();
	}
}
	
}