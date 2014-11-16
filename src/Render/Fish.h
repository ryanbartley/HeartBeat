//
//  Fish.h
//  SpringMeshGpu
//
//  Created by Ryan Bartley on 10/20/14.
//
//

#pragma once

#include "PondElement.h"
#include "cinder/Log.h"

namespace heartbeat {
	
using FishRef = std::shared_ptr<class Fish>;

class Fish : public PondElement {
public:
	
	static const float MAX_SPEED;
	static const float MAX_FORCE;
	static const float NEAR_DISTANCE;
	
	static FishRef create( const ci::gl::GlslProgRef &shader );
	
	static PondElementId TYPE;
	
	PondElementId getType() const override { return TYPE; }
	
	void update() override;
	void draw() override;
	
	inline void applyBehaviors( std::vector<PondElementRef> &pondElements );
	void updateTarget( const ci::vec3 &target ) { mCurrentTarget = target; }
	bool hasArrived();
	inline void addForce( const ci::vec3 &force ) {mAcceleration += force * mInverseMass;}
	void registerTouch( EventDataRef touchEvent );
	
private:
	Fish( const ci::gl::GlslProgRef &shader );
	
	void initialize( const ci::JsonTree &root ) override;
	
	inline ci::vec3 seek( const ci::vec3 &target );
	inline ci::vec3 separate( std::vector<PondElementRef> &pondElements );
	inline ci::vec3 cohesion( std::vector<PondElementRef> &pondElements );
	inline bool	isNearTarget();
	
	inline void calcAndSetUpdatedTranform();
	
	
	ci::vec3	mCalcLocation;
	ci::vec3	mAcceleration;
	ci::vec3	mVelocity;
	ci::vec3	mCurrentTarget;
	float		mInverseMass;
	
	friend class Pond;
};
	
bool Fish::isNearTarget()
{
	auto dist = glm::distance( mCalcLocation, mCurrentTarget );
	CI_LOG_V("distance: " << dist << " NEAR_DISTANCE: " << NEAR_DISTANCE );
	return dist < NEAR_DISTANCE;
}
	
void Fish::applyBehaviors( std::vector<PondElementRef> &pondElements )
{
	auto separateForce = separate( pondElements );
	auto seekForce = seek( mCurrentTarget );
	addForce(separateForce);
	addForce(seekForce);
}
	
ci::vec3 Fish::seek( const ci::vec3& target ) {
	CI_LOG_V("mCalcLocation: " << mCalcLocation);
	CI_LOG_V("target: " << target);
	ci::vec3 desired = ci::normalize(target - mCalcLocation);
	CI_LOG_V("desired: " << desired);
	desired *= .5;
	CI_LOG_V("desired * .5: " << desired);
	ci::vec3 steer = desired - mVelocity;
	CI_LOG_V("Steer: " << steer);
	auto clampedSteering = glm::clamp(steer,
									  ci::vec3( -MAX_FORCE ),
									  ci::vec3( MAX_FORCE ) );
	CI_LOG_V("clampedSteering: " << clampedSteering);
	
	return clampedSteering;
}
	
ci::vec3 Fish::separate( std::vector<PondElementRef>& pondElements ) {
	static const float desiredSeparation = 5.;
	ci::vec3 sum( 0.0f );
	int count = 0;
	
	for ( auto & pondElement : pondElements ){
		if( mId != pondElement->getId() ){
			auto otherPos = pondElement->getTranslation();
			float d = glm::distance( mCalcLocation, otherPos );
			if ( ( d > 0 ) && ( d < desiredSeparation ) ) {
				auto diff = glm::normalize( mCalcLocation - otherPos );
				diff /= d;
				sum += diff;
				++count;
			}
		}
	}
	
	if (count > 0) {
		sum /= count;
		glm::normalize(sum);
		sum *= MAX_SPEED;
		sum -= mVelocity;
		sum = glm::clamp(sum,
					   ci::vec3( 0.0f ),
					   ci::vec3( MAX_FORCE ) );
	}
		CI_LOG_V("Sum: " << sum << " Velocity: " << mVelocity << " Translation: " << getTranslation() << " Rotation: " << getRotation() );
	return sum;
}
	
void Fish::calcAndSetUpdatedTranform()
{
	auto axis = glm::normalize( glm::cross( mCalcLocation, mCurrentTarget ) );
	float theta = acos( glm::dot( mCalcLocation, mCurrentTarget ) /
					   ( glm::length2( mCalcLocation ) * glm::length2( mCurrentTarget ) ) );
//	setRotation( glm::angleAxis( ci::toRadians( theta ), axis ) );
	setTranslation( mCalcLocation );
}

}