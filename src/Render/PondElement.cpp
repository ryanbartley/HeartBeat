//
//  PondElement.cpp
//  SpringMeshGpu
//
//  Created by Ryan Bartley on 10/20/14.
//
//

#include "PondElement.h"

#include "cinder/Json.h"
#include "cinder/Log.h"
#include "cinder/gl/Shader.h"
#include "cinder/ObjLoader.h"
#include "Engine.h"
#include "Pond.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace heartbeat {
	
uint64_t PondElement::globalId = 0;
	
PondElementId PondElement::TYPE = Hash::HASHER("PondElement");
	
PondElement::PondElement( const ci::gl::GlslProgRef &shader )
: mId( getNextId() ), mRenderShader( shader )
{
}

PondElementRef PondElement::create( const ci::gl::GlslProgRef &shader )
{
	return PondElementRef( new PondElement( shader ) );
}
	
void PondElement::initialize( const ci::JsonTree &root )
{
	Renderable::initialize( root["transformation"] );
	
	// TODO: swap out the stock shaders for real shaders which allow light and texture and
	// make cases for them in the json
	try {
		auto objName = root["objName"].getValue();
		
		auto obj = ObjLoader( getFileContents( objName ) );
		
		mBatch = gl::Batch::create( obj, mRenderShader );
	}
	catch ( const JsonTree::ExcChildNotFound &ex ) {
		CI_LOG_E( "ObjName Not found " << ex.what() << " using default cube" );
		
		mBatch = gl::Batch::create( geom::Cube(), mRenderShader );
	}
	
	try {
		auto diffuse = root["diffuse"].getValue();
		
		mDiffuseTexture = gl::Texture2d::create( loadImage( getFileContents( diffuse ) ) );
	}
	catch ( const JsonTree::ExcChildNotFound &ex ) {
		CI_LOG_E(ex.what());
	}
}
	
void PondElement::draw()
{
	gl::ScopedModelMatrix scopeModel;
	gl::setModelMatrix( getModelMatrix() );
	
	mBatch->draw();
}
	
}