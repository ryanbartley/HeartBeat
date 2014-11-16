//
//  Pond.h
//  SpringMeshGpu
//
//  Created by Ryan Bartley on 10/20/14.
//
//

#pragma once

#include "Renderable.h"

#include "Common.h"
#include "SpringMesh.h"
#include "PondElement.h"
#include "cinder/Rand.h"

namespace heartbeat {
	
using PondRef = std::shared_ptr<class Pond>;
using PondElementFactoryMap = std::map<std::string, std::function<PondElementRef( const ci::gl::GlslProgRef &)>>;
	
class Pond {
public:
	
	static PondElementFactoryMap PondElementCreators;
	
	static PondRef create( const ci::vec2 &pondSize );
	
	~Pond() {}
	
	void update();
	void renderPondElements();
	void projectPondElements( const ci::gl::Texture2dRef &pond );
	ci::gl::GlslProgRef& getRenderShader() { return mRenderGlsl; }
	
	void touchBeganDelegate( EventDataRef touchEvent );
	void touchMovedDelegate( EventDataRef touchEvent );
	void enableDebug( bool enable ) { mSpringMesh->enableDrawDebug( enable ); }
    bool isDebugEnabled() { return mSpringMesh->drawDebug(); }
	void initialize();
	
	SpringMeshRef& getSpringMesh() { return mSpringMesh; }
	
	struct PondBounds {
		
		PondBounds() = default;
		PondBounds( ci::vec3 min, ci::vec3 max )
		: mMin(min), mMax(max) {}
		
		inline bool contained( const ci::vec3 & point )
		{
			return point.x > mMin.x && point.x < mMax.x &&
					point.y > mMin.y && point.y < mMax.y &&
					point.z > mMin.z && point.z < mMax.z;
		}
		
		ci::vec3 getRandomPointWithin(ci::vec3 currentPosition)
		{
			ci::vec3 ret = currentPosition;
			
			while (cinder::distance(ret, currentPosition) < 2) {
				ret.x = ci::randFloat( mMin.x, mMax.x );
				ret.y = ci::randFloat( mMin.y, mMax.y );
				ret.z = ci::randFloat( mMin.z, mMax.z );
			}	
			return ret;
		}
		
		ci::vec3 mMin, mMax;
	};
	
	PondBounds& getPondBounds() { return mPondBounds; }
	
	
private:
	Pond( const ci::vec2 &pondSize );
	
	void loadShaders();
	
	SpringMeshRef				mSpringMesh;
	std::vector<PondElementRef> mPondElements;
	ci::gl::GlslProgRef			mRenderGlsl;
	PondBounds					mPondBounds;
	ci::CameraPersp				mCam;
	ci::vec2					mPondSize;
};
	
}
