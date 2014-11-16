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

const float Fish::MAX_SPEED = 0.15f;
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
	mVelocity += mAcceleration*0.01f;
	mVelocity *= 0.99;
	mVelocity = glm::clamp( mVelocity, ci::vec3( -.005f, -.005f, -.005f), ci::vec3( .005f, .005f, .005f ) );
	mCalcLocation += mVelocity;
	mAcceleration = ci::vec3( 0, 0, 0 );
	if( ! pondBounds.contained( mCalcLocation ) || isNearTarget() ) {
		ci::randSeed(mId+ci::app::getElapsedFrames());
		updateTarget( pondBounds.getRandomPointWithin(mCalcLocation) );
	}
	calcAndSetUpdatedTranform();
}
	
void Fish::initialize( const ci::JsonTree &root )
{
	PondElement::initialize( root );
	mCalcLocation = getTranslation();
	mCurrentTarget = Engine::get()->getPond()->getPondBounds().getRandomPointWithin(mCalcLocation);
}

void Fish::registerTouch( EventDataRef touchEvent )
{
	
}
	
void Fish::draw()
{
	static float rot = 0.0f;
	gl::ScopedModelMatrix scopeMatrix;
	gl::setModelMatrix( getModelMatrix() );
	gl::multModelMatrix( ci::scale( vec3( .075, .075, .075 ) ) );
//	ci::rotate( ci::toRadians( mTheta ), mAxis );
	//gl::multModelMatrix( ci::rotate( toRadians( 180.0f ), vec3( 0, 1, 0 ) ) );
	gl::multModelMatrix( ci::rotate( toRadians( -90.0f ), vec3( 1, 0, 0 ) ) );
	
//	mRenderShader->uniform( "shininess", 30.0f );
//    mRenderShader->uniform( "lightIntensity", vec3( 0 ) );
//	gl::ScopedTextureBind scopeTexture( mDiffuseTexture, 0 );
    gl::ScopedColor scopeColor( ColorA( 0, 0, 0, 1 ) );
	mBatch->draw();
}
	
}