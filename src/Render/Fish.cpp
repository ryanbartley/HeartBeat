//
//  Fish.cpp
//  SpringMeshGpu
//
//  Created by Ryan Bartley on 10/20/14.
//
//

#include "Fish.h"
#include "Common.h"
#include "Engine.h"
#include "Pond.h"
#include "cinder/AxisAlignedBox.h"
#include "cinder/gl/Context.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace heartbeat {

const float Fish::MAX_SPEED = 0.5f;
const float Fish::MAX_FORCE = 5.0f;
	
PondElementId Fish::TYPE = Hash::HASHER("Fish");
	
Fish::Fish()
: mAcceleration( 0.0f ), mVelocity( 0.0f ), mInverseMass( 1.0 )
{
	
}
	
FishRef Fish::create()
{
	return FishRef( new Fish );
}

void Fish::update()
{
	static auto pondBounds = Engine::get()->getPond()->getPondBounds();
	mVelocity += mAcceleration;
	glm::clamp( mVelocity, ci::vec3( 0, 0, 0 ), ci::vec3( 5, 5, 5 ) );
	mCalcLocation += mVelocity;
	mAcceleration = ci::vec3( 0, 0, 0 );
	if( ! pondBounds.contained( mCalcLocation ) ) {
		updateTarget( pondBounds.getRandomPointWithin() );
	}
	calcAndSetUpdatedTranform();
}
	
void Fish::initialize( const ci::JsonTree &root )
{
	PondElement::initialize( root );
}

void Fish::registerTouch( EventDataRef touchEvent )
{
	
}
	
void Fish::draw()
{
	gl::ScopedModelMatrix scopeMatrix;
	gl::setModelMatrix( getModelMatrix() );
	gl::ScopedTextureBind scopeTexture( mTexture, 0 );
	
	mBatch->draw();
}
	
}