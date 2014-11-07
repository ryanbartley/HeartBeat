//
//  TestPondElement.cpp
//  SpringMeshGpu
//
//  Created by Ryan Bartley on 10/20/14.
//
//

#include "PrimitivePondElement.h"
#include "JsonManager.h"

#include "cinder/Log.h"
#include "cinder/gl/Shader.h"

#include "cinder/GeomIo.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace heartbeat {
	
PrimitivePondElement::PrimitivePondElement()
{
	
}
	
PrimitivePondElementRef PrimitivePondElement::create()
{
	return PrimitivePondElementRef( new PrimitivePondElement );
}
	
void PrimitivePondElement::initialize( const ci::JsonTree &root )
{
	Renderable::initialize( root["transformation"] );
	
	geom::SourceRef mSource;
	
	try {
		auto primitive = root["primitive"].getValue();
		
		if( primitive == "sphere" ) {
			mSource.reset( new geom::Sphere() );
		}
		else if( primitive == "cube" ) {
			mSource.reset( new geom::Cube );
		}
	}
	catch ( JsonTree::ExcChildNotFound &ex ) {
		CI_LOG_W("Couldn't find type to load, using default cube" << ex.what() );
		mSource.reset( new geom::Cube );
	}
	mSource->enable( geom::Attrib::COLOR );
	mBatch = gl::Batch::create( mSource, gl::getStockShader( gl::ShaderDef().color() ) );
}
	
void PrimitivePondElement::update()
{
//	static float i = 0.0f;
	auto appElapsed = App::get()->getElapsedSeconds();
	mTransformation.setTranslation( ci::vec3( 0, sin(appElapsed), 0  ) );
//	mTransformation.setRotation( ci::quat( ci::rotate( i += .05f, vec3( 0, 0, 1 ) ) ) );
}
	
	
}