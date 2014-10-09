//
//  Renderable.cpp
//  LibUrgTest
//
//  Created by Ryan Bartley on 10/7/14.
//
//

#include "Renderable.h"

#include "JsonManager.h"

#include "cinder/Log.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace heartbeat {
	
Renderable::Renderable() : mTransformation()
{
	
}
	
void Renderable::initialize( const ci::JsonTree &root )
{
	try {
		
		auto translationRoot = root["translations"];
		
		if( translationRoot.hasChild( "coordinates" ) ) {
			CI_LOG_V("Using coordinate version of translation");
			auto translations = translationRoot["coordinates"].getChildren();
			int i = 0;
			vec2 translation;
			for( auto & position : translations ) {
				translation[i++] = position.getValue<float>();
			}
			setTranslation( translation );
		}
		else if( translationRoot.hasChild( "center" ) ) {
			CI_LOG_V("Using center version of translation");
			auto center = translationRoot["center"].getValue<bool>();
			if( center ) {
				auto center = vec2( app::App::get()->getWindowSize() ) / 2.0f;
				setTranslation( center );
			}
		}
	}
	catch ( JsonTree::ExcChildNotFound &ex ) {
		CI_LOG_W("translations child not found, using default 0, 0, 0");
	}
	
	try {
		auto scales = root["scales"].getChildren();
		int i = 0;
		vec2 scale;
		for( auto & scalar : scales ) {
			scale[i++] = scalar.getValue<float>();
		}
		setScale( scale );
		CI_LOG_V(scale);
	}
	catch ( JsonTree::ExcChildNotFound &ex ) {
		CI_LOG_W("Scale child not found, using default 1, 1, 1");
		setScale( vec3( 1 ) );
	}
	
	try {
		auto rotationRoot = root["rotations"];
		if( rotationRoot.hasChild( "quat" ) ) {
			CI_LOG_V("Using quat version of rotation");
			auto quaternion = rotationRoot["quat"].getChildren();
			int i = 0;
			quat rotation;
			for( auto & rotationStep : quaternion ) {
				rotation[i++] = rotationStep.getValue<float>();
			}
			setRotation( rotation );
		}
		else if( rotationRoot.hasChild( "angle" ) ) {
			CI_LOG_V("Using angle axis version of rotation");
			auto angle = rotationRoot["angle"].getValue<float>();
			setRotation( toRadians( angle ) );
		}
		
	}
	catch ( JsonTree::ExcChildNotFound &ex ) {
		CI_LOG_V("Rotation child not found, using default 0, 0, 0, 1");
	}
}
	
}
