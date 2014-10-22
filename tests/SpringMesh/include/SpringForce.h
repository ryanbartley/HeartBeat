#pragma once

#include "MassPoint.h"

/**
 * \class SpringForceT
 *
 */
template <typename VecT>
class SpringForceT {
public:

	SpringForceT();
	SpringForceT( MassPointT<VecT>* aMassPointA, MassPointT<VecT>* aMassPointB, float aRestLength = 1.0f, float aStiffness = 1.0f );
	SpringForceT( const SpringForceT& rhs );
	~SpringForceT() {}

	SpringForceT& operator=( const SpringForceT& rhs );

	float					getStiffness() const { return mStiffness; }
	void					setStiffness( float aStiffness );
	float					getRestLength() const { return mRestLength; }
	void					setRestLength( float aRestLength );
	float					getRestLengthSquared() const { return mRestLengthSquared; }
	void					setRestLengthFromMassPoints();
	MassPointT<VecT>*		getMassPointA() { return mMassPointA; }
	const MassPointT<VecT>*	getMassPointA() const { return mMassPointA; }
	MassPointT<VecT>*		getMassPointB() { return mMassPointB; }
	const MassPointT<VecT>*	getMassPointB() const { return mMassPointB; }

	void					update();

private:
	float					mStiffness;
	float					mRestLength;
	float					mRestLengthSquared;
	MassPointT<VecT>*		mMassPointA;
	MassPointT<VecT>*		mMassPointB;
};

typedef SpringForceT<ci::vec2> SpringForce2f;
typedef SpringForceT<ci::vec3> SpringForce3f;
