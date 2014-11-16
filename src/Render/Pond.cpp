//
//  Pond.cpp
//  SpringMeshGpu
//
//  Created by Ryan Bartley on 10/20/14.
//
//

#include "cinder/Log.h"

#include "Pond.h"

#include "JsonManager.h"
#include "EventManager.h"
#include "InteractionEvents.h"
#include "Fish.h"
#include "PondElement.h"
#include "PrimitivePondElement.h"
#include "Engine.h"

#include "cinder/Log.h"
#include "cinder/gl/GlslProg.h"

using namespace ci;
using namespace std;
using namespace ci::app;

namespace heartbeat {

const int POSITION_INDEX	= 0;
const int VELOCITY_INDEX	= 1;
const int NORMAL_INDEX		= 2;
const int CONNECTION_INDEX	= 3;
const int TEXCOORD_INDEX	= 4;
	
PondElementFactoryMap Pond::PondElementCreators = {
	make_pair( "PondElement", std::bind( &PondElement::create, std::placeholders::_1 ) ),
	make_pair( "Fish", std::bind( &Fish::create, std::placeholders::_1 ) ),
	make_pair( "PrimitivePondElement", std::bind( &PrimitivePondElement::create, std::placeholders::_1  ) )
};
	
Pond::Pond( const ci::vec2 &pondSize )
: mPondSize( pondSize )
{
	auto eventManager = EventManagerBase::get();
	
	if( eventManager ) {
		eventManager->addListener( std::bind( &Pond::touchBeganDelegate, this, std::placeholders::_1 ), TouchBeganEvent::TYPE );
		eventManager->addListener( std::bind( &Pond::touchMovedDelegate, this, std::placeholders::_1 ), TouchMoveEvent::TYPE );
	}
}
	
PondRef Pond::create( const ci::vec2 &pondSize )
{
	return PondRef( new Pond( pondSize ) );
}
	
void Pond::initialize()
{
	loadShaders();
	
	try {
		auto pondAttribs = JsonManager::get()->getRoot()["pondAttribs"];
		
		try {
			auto pondBounds = pondAttribs["pondBounds"];
			auto mins = pondBounds["min"].getChildren();
			ci::vec3 min;
			int i = 0;
			for( auto & minDim : mins ) {
				min[i++] = minDim.getValue<float>();
			}
			
			auto maxs = pondBounds["max"].getChildren();
			ci::vec3 max;
			i = 0;
			for( auto & maxDim : maxs ) {
				max[i++] = maxDim.getValue<float>();
			}
			
			mPondBounds = PondBounds( min, max );
		}
		catch( const JsonTree::ExcChildNotFound &ex ) {
			CI_LOG_E(ex.what());
		}
		
		try {
			auto pondElements = pondAttribs["pondElements"].getChildren();
			
			for( auto & element : pondElements ) {
				
				auto type = element["type"].getValue();
				
				auto found = PondElementCreators.find( type );
				
				if( found != PondElementCreators.end() ) {
					CI_LOG_V("Pond Elements: type: " << type << " element: " << element);
					auto pondElement = found->second( mRenderGlsl );
					pondElement->initialize( element );
					mPondElements.push_back( pondElement );
				}
				else {
					CI_LOG_E("Type not found " << type );
				}
				
			}
		}
		catch ( JsonTree::ExcChildNotFound &ex ) {
			CI_LOG_W("No Pond Elements Found " << ex.what() );
		}
		
		mSpringMesh = SpringMesh::create( mRenderGlsl );
		
		try {
			auto springMeshAttribs = pondAttribs["springMesh"];
			
			mSpringMesh->initialize( springMeshAttribs, mPondSize );
		}
		catch ( JsonTree::ExcChildNotFound &ex ) {
			CI_LOG_E("Spring Mesh attribs not found, using defaults" << ex.what());
			mSpringMesh->initialize( JsonTree(), mPondSize );
		}
		
		try {
			auto cameraAttribs = pondAttribs["camera"];
			
			auto fov = cameraAttribs["fov"].getValue<float>();
			auto near = cameraAttribs["near"].getValue<float>();
			auto far = cameraAttribs["far"].getValue<float>();
			
			mCam.setPerspective( fov, mPondSize.x / mPondSize.y, near, far );
			
			auto camPos = cameraAttribs["camPos"].getChildren();
			vec3 cameraPosition;
			int i = 0;
			for( auto & pos : camPos ) {
				cameraPosition[i++] = pos.getValue<float>();
			}
			
			auto lookAtPos = cameraAttribs["lookAtPos"].getChildren();
			vec3 center;
			i = 0;
			for( auto & pos : lookAtPos ) {
				center[i++] = pos.getValue<float>();
			}
			
			mCam.lookAt( cameraPosition, center );
		}
		catch ( JsonTree::ExcChildNotFound &ex ) {
			CI_LOG_E("Couldn't find camera child, reverting to defaults" << ex.what() );
			mCam.setPerspective( 60.0f, mPondSize.x / mPondSize.y, .01f, 1000.0f );
			mCam.lookAt( vec3( 0, 5, 0 ), vec3( 0 ) );
		}
		
	}
	catch ( const JsonTree::ExcChildNotFound &ex ) {
		CI_LOG_E("Couldn't Find " << ex.what() );
	}
}
	
void Pond::loadShaders()
{
	gl::GlslProg::Format renderFormat;
	renderFormat.vertex( getFileContents( "SpringMeshrender.vert" ) )
	.fragment( getFileContents( "SpringMeshrender.frag" ) )
	.attrib( geom::Attrib::NORMAL, "normal" )
	.attrib( geom::Attrib::POSITION, "position" )
	.attrib( geom::Attrib::TEX_COORD_0, "texCoord" )
	.attribLocation( "position",	POSITION_INDEX )
	.attribLocation( "normal",		NORMAL_INDEX )
	.attribLocation( "texCoord",	TEXCOORD_INDEX );
	
	try {
		mRenderGlsl = gl::GlslProg::create( renderFormat );
	}
	catch ( const gl::GlslProgCompileExc &ex ) {
		CI_LOG_E("Render Shader Compile Exc " << ex.what());
	}
	catch ( const ci::Exception &ex ) {
		CI_LOG_E("Unknown Exception " << ex.what());
	}
}
	
void Pond::touchBeganDelegate( EventDataRef touchEvent )
{
	if( touchEvent->isHandled() )
		return;
	
	auto event = std::dynamic_pointer_cast<TouchBeganEvent>(touchEvent);
	if( !event ) {
		CI_LOG_W("Not a TouchEvent " << touchEvent->getName() );
		return;
	}
	
	mSpringMesh->registerTouchBegan( event->getWorldCoordinate() );
	
	for( auto & element : mPondElements ) {
		if( element->getType() == Fish::TYPE ) {
			auto fish = std::dynamic_pointer_cast<Fish>(element);
			fish->registerTouch( touchEvent );
		}
	}
}

void Pond::touchMovedDelegate( EventDataRef touchEvent )
{
	if( touchEvent->isHandled() )
		return;
	
	auto event = std::dynamic_pointer_cast<TouchMoveEvent>(touchEvent);
	if( !event ) {
		CI_LOG_W("Not a TouchEvent " << touchEvent->getName() );
		return;
	}
	
	mSpringMesh->registerTouchMoved( event->getWorldCoordinate() );
	
	for( auto & element : mPondElements ) {
		if( element->getType() == Fish::TYPE ) {
			auto fish = std::dynamic_pointer_cast<Fish>(element);
			fish->registerTouch( touchEvent );
		}
	}
}

void Pond::update()
{
	mSpringMesh->update();
	
	for( auto & element : mPondElements ) {
		if( element->getType() == Fish::TYPE ) {
			auto cast = std::dynamic_pointer_cast<Fish>(element);
			cast->applyBehaviors( mPondElements );
		}
		element->update();
	}
}

void Pond::renderPondElements()
{
	gl::ScopedMatrices scopeMat;
	gl::setMatrices( mCam );

	// Need to surround this with the renderers Fbo
	for( auto & element : mPondElements ) {
		element->draw();
	}
}
	
void Pond::projectPondElements( const gl::Texture2dRef &pond )
{
//	static float rot = 0.0f;
	gl::ScopedMatrices scopeMat;
	gl::setMatricesWindowPersp( mPondSize, 40.0f, .01f, 10000.0f );
//	gl::multModelMatrix( ci::rotate( rot -= .01f, vec3( 0, 1, 0 ) ) );
//	gl::multModelMatrix( ci::translate( vec3( - mPondSize.x, 0, 0 ) ) );
	if( pond ) {
		// Need to send it the fbo or texture of the rendered pond from above.
		mSpringMesh->project( pond );
	}
	
	mSpringMesh->debugRender();
}
	
}