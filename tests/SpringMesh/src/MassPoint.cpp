#include "MassPoint.h"

using namespace ci;

const float kDefaultConserve = 0.985f;

template <typename VecT>
MassPointT<VecT>::MassPointT()
	: mMass( 1.0f ),
	  mInvMass( 1.0f ),
	  mResponse( 1.0f ),
	  mVel( VecT( 0 ) ),
	  mAccel( VecT( 0 ) ),
	  mConserve( kDefaultConserve ),
	  mUv( ci::vec2( 0, 0 ) )
{
}

template <typename VecT>
MassPointT<VecT>::MassPointT( const VecT& aPos, const ci::vec2& aUv  )
	: mMass( 1.0f ),
	  mInvMass( 1.0f ),
      mResponse( 1.0f ),
	  mPrevPos( aPos ),
	  mPos( aPos ),
	  mVel( VecT( 0 ) ),
	  mAccel( VecT( 0 ) ),
	  mConserve( kDefaultConserve ),
	  mUv( aUv )
{
}

template <typename VecT>
void MassPointT<VecT>::setMass( float aMass )
{
	mMass = aMass;
	mInvMass = 1.0f/mMass;
}

template <typename VecT>
void MassPointT<VecT>::setResponse( float aResponse )
{
	mResponse = std::max( 0.0f, std::min( 1.0f, aResponse ) );
}

template <typename VecT>
void MassPointT<VecT>::setPos( const VecT& aPos )
{
	mPrevPos = mPos = aPos;
}

template <typename VecT>
void MassPointT<VecT>::addForce( const VecT& aForce )
{
	VecT accel = aForce*mInvMass;
	mAccel += accel;
}

template <typename VecT>
void MassPointT<VecT>::clearAccel()
{
	mAccel = VecT( 0 );
}

template <typename VecT>
void MassPointT<VecT>::update( float dt, float dampen )
{
	mVel = mPos - mPrevPos;
	mPrevPos = mPos;
	VecT velContrib = 0.5f*dt*mVel*(1.0f - dampen);
	VecT accelContrib = dt*dt*mAccel;
	mPos += mResponse*velContrib;
	mPos += mResponse*accelContrib;
	mAccel *= mConserve;
	//mAccel = VecT::zero();
}

template class MassPointT<vec2>;
template class MassPointT<vec3>;
