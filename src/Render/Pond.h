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

namespace heartbeat {
	
using PondRef = std::shared_ptr<class Pond>;
using PondElementFactoryMap = std::map<std::string, std::function<PondElementRef()>>;
	
class Pond {
public:
	
	static PondElementFactoryMap PondElementCreators;
	
	static PondRef create( const ci::vec2 &pondSize );
	
	~Pond() {}
	
	void update();
	void renderPondElements();
	void projectPondElements( const ci::gl::Texture2dRef &pond );
	
	void touchBeganDelegate( EventDataRef touchEvent );
	void touchMovedDelegate( EventDataRef touchEvent );
	
	void initialize();
	
	SpringMeshRef& getSpringMesh() { return mSpringMesh; }
	
private:
	Pond( const ci::vec2 &pondSize );
	
	SpringMeshRef				mSpringMesh;
	std::vector<PondElementRef> mPondElements;
	ci::CameraPersp				mCam;
	ci::vec2					mPondSize;
};
	
}
