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
#include "JsonManager.h"


using namespace ci;
using namespace ci::app;
using namespace std;

namespace heartbeat {

const float Fish::MAX_SPEED = 0.5f;
const float Fish::MAX_FORCE = 4.0f;
const float Fish::NEAR_DISTANCE = 0.50f;
	
PondElementId Fish::TYPE = Hash::HASHER("Fish");
	
Fish::Fish( const ci::gl::GlslProgRef &shader )
: PondElement( shader ), mAcceleration( 0.0f ), mVelocity( 0.0f ), mInverseMass( 1.0 )
{
	
}
	
FishRef Fish::create( const ci::gl::GlslProgRef &shader )
{
	return FishRef( new Fish( shader ) );
}

void Fish::update()
{
	static auto pondBounds = Engine::get()->getPond()->getPondBounds();
	CI_LOG_V("velocity " << mAcceleration);
	mVelocity += mAcceleration*0.01f;
	mVelocity *= 0.99;
	CI_LOG_V("velocity " << mVelocity);
	mVelocity = glm::clamp( mVelocity, ci::vec3( -.02f, -.02f, -.02f), ci::vec3( .02f, .02f, .02f ) );
	CI_LOG_V("pre velocity location" << mCalcLocation);
	mCalcLocation += mVelocity;
	CI_LOG_V("post velocity location" << mCalcLocation);
	mAcceleration = ci::vec3( 0, 0, 0 );
	if( ! pondBounds.contained( mCalcLocation ) || isNearTarget() ) {
		cout << "Updating Target" << endl;
		updateTarget( pondBounds.getRandomPointWithin() );
	}
	calcAndSetUpdatedTranform();
}
	
void Fish::initialize( const ci::JsonTree &root )
{
	PondElement::initialize( root );
	mCalcLocation = getTranslation();
	mCurrentTarget = Engine::get()->getPond()->getPondBounds().getRandomPointWithin();
	CI_LOG_V( "mCurrentTarget: " << mCurrentTarget );
}

void Fish::registerTouch( EventDataRef touchEvent )
{
	
}
	
void Fish::draw()
{
	static float rot = 0.0f;
	gl::ScopedModelMatrix scopeMatrix;
	gl::setModelMatrix( getModelMatrix() );
	gl::multModelMatrix( ci::scale( vec3( .1, .1, .1 ) ) );
	
	
	gl::ScopedTextureBind scopeTexture( mDiffuseTexture, 0 );
	
	mBatch->draw();
}
	
}