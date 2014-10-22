#pragma once

template <typename VecT> struct Ops {};

template <> struct Ops<ci::vec2> {
	static ci::vec2 VecT( float x, float y ) { return ci::vec2( x, y ); }
	static ci::vec2 VecT( const ci::vec2& v ) { return v; }
	static ci::vec3 Vec3( const ci::vec2& v ) { return ci::vec3( v.x, v.y, 0 ); }
	static ci::vec2 DownForce( float amt, const ci::vec2& dv ) { return amt * ci::normalize( dv ); }
	//static ci::vec3 Cross( const ci::vec2& a, const ci::vec2& b ) { return ci::vec3( 0, 0, 1 ); }
};

template <> struct Ops<ci::vec3> {
	static ci::vec3 VecT( float x, float y ) { return ci::vec3( x, y, 0 ); }
	static ci::vec3 VecT( const ci::vec2& v ) { return ci::vec3( v.x, v.y, 0 ); }
	static ci::vec3 Vec3( const ci::vec3& v ) { return v; }
	static ci::vec3 DownForce( float amt, const ci::vec2& dv ) { return ci::vec3( 0, 0, -amt ); }
	//static ci::vec3 Cross( const ci::vec3& a, const ci::vec3& b ) { return a.cross( b ); }
};

/**
 * \class MassPointT
 *
 */
template <typename VecT>
class MassPointT {
public:

	MassPointT();
	MassPointT( const VecT& aPos, const ci::vec2& aUv = ci::vec2( 0, 0 ) );
	~MassPointT() {}

	float				getMass() const { return mMass; }
	void				setMass( float aMass );

	float				getResponse() const { return mResponse; }
	void				setResponse( float aResponse );
	void				setNoReponse() { setResponse( 0.0f ); }

	const VecT&			getPrevPos() const { return mPrevPos; }
	const VecT&			getPos() const { return mPos; }
	void				setPos( const VecT& aPos );
	const VecT&			getVel() const { return mVel; }
	const VecT&			getAccel() const { return mAccel; }
	void				addForce( const VecT& aForce );
	void				clearAccel();

	const ci::vec2&	getUv() const { return mUv; }
	void				setUv( const ci::vec2& aUv ) { mUv = aUv; }

	void				update( float dt, float dampen = 0.0f );

private:
	float				mMass;
	float				mInvMass;
	float				mResponse;
	VecT				mPrevPos;
	VecT				mPos;
	VecT				mVel;
	VecT				mAccel;
	float				mConserve;
	ci::vec2			mUv;
};

typedef MassPointT<ci::vec2> MassPoint2f;
typedef MassPointT<ci::vec3> MassPoint3f;
