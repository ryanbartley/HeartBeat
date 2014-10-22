#include "SpringForce.h"
using namespace ci;

template <typename VecT>
SpringForceT<VecT>::SpringForceT()
	: mStiffness( 1.0f ),
	  mRestLength( 10.0f ),
	  mRestLengthSquared( 100.0f ),
	  mMassPointA( nullptr ),
	  mMassPointB( nullptr )
{
}

template <typename VecT>
SpringForceT<VecT>::SpringForceT( MassPointT<VecT>* aMassPointA, MassPointT<VecT>* aMassPointB, float aRestLength, float aStiffness )
	: mStiffness( aStiffness ),
	  mRestLength( aRestLength ),
	  mRestLengthSquared( aRestLength*aRestLength ),
	  mMassPointA( aMassPointA ),
	  mMassPointB( aMassPointB )
{
}

template <typename VecT>
SpringForceT<VecT>::SpringForceT( const SpringForceT<VecT>& rhs )
	: mStiffness( rhs.mStiffness ),
	  mRestLength( rhs.mRestLength ),
	  mRestLengthSquared( rhs.mRestLengthSquared ),
	  mMassPointA( rhs.mMassPointA ),
	  mMassPointB( rhs.mMassPointB )
{
}

template <typename VecT>
SpringForceT<VecT>& SpringForceT<VecT>::operator=( const SpringForceT<VecT>& rhs )
{
	if( &rhs != this ) {
		mStiffness			= rhs.mStiffness;
		mRestLength			= rhs.mRestLength;
		mRestLengthSquared	= rhs.mRestLengthSquared;
		mMassPointA			= rhs.mMassPointA;
		mMassPointB			= rhs.mMassPointB;
	}
	return *this;
}

template <typename VecT>
void SpringForceT<VecT>::setStiffness( float aStiffness )
{
	mStiffness = aStiffness;
}

template <typename VecT>
void SpringForceT<VecT>::setRestLength( float aRestLength )
{
	mRestLength = aRestLength;
	mRestLengthSquared = mRestLength*mRestLength;
}

template <typename VecT>
void SpringForceT<VecT>::update()
{
	if( nullptr == mMassPointA || nullptr == mMassPointB ) {
		return;
	}

	VecT springVector = mMassPointB->getPos() - mMassPointA->getPos();
	float springLength = springVector.length();
	VecT adjVec = (springVector/mRestLength)*(mRestLength - springLength)*(-mStiffness);
	VecT forceA = -adjVec;
	VecT forceB =  adjVec;
	mMassPointA->addForce( forceB );
	mMassPointB->addForce( forceA );

/*
	vec2 dv = mMassPointB->getPos() - mMassPointA->getPos();
	float lenSquared = dv.lengthSquared();
	float scale = 1.0f - (mRestLengthSquared/lenSquared);
	vec2 adjVec = 0.5f*scale*(-mStiffness)*dv;
	vec2 forceA =  adjVec;
	vec2 forceB = -adjVec;
	mMassPointA->addForce( forceB );
	mMassPointB->addForce( forceA );
*/
}

template <typename VecT>
void SpringForceT<VecT>::setRestLengthFromMassPoints()
{
	if( nullptr == mMassPointA || nullptr == mMassPointB ) {
		return;
	}

	VecT dv = mMassPointB->getPos() - mMassPointA->getPos();
	setRestLength( dv.length() );
}

template class SpringForceT<vec2>;
template class SpringForceT<vec3>;
