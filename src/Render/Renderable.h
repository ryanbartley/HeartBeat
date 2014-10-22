//
//  Renderable.h
//  LibUrgTest
//
//  Created by Ryan Bartley on 10/7/14.
//
//

#pragma once

#include "Transformation.h"

namespace cinder {
	class JsonTree;
}

namespace heartbeat {

class Renderable {
public:
	virtual ~Renderable() {}
	
	virtual void update() = 0;
	virtual void draw() = 0;
	
	inline void setTranslation( const ci::vec3 &position ) { mTransformation.setTranslation( position ); }
	inline void setScale( const ci::vec3 &scale ) {  mTransformation.setScale( scale ); }
	inline void setRotation( const ci::quat &rotation ) { mTransformation.setRotation( rotation ); }
	
	inline void setTranslation( const ci::vec2 &position ) { mTransformation.setTranslation( ci::vec3( position, 0 ) ); }
	inline void setScale( const ci::vec2 &scale ) {  mTransformation.setScale( ci::vec3( scale, 1 ) ); }
	inline void setRotation( float radian ) { mTransformation.setRotation( ci::quat( radian, ci::vec3( 0, 0, 1 ) ) ); }
	
	inline const ci::vec3& getTranslation() const { return mTransformation.getTranslation(); }
	inline const ci::vec3& getScale() const { return mTransformation.getScale(); }
	inline const ci::quat& getRotation() const { return mTransformation.getRotation(); }
	inline const ci::mat4& getModelMatrix() const {
		return mTransformation.getModelMatrix(); }
	
	virtual void initialize( const ci::JsonTree &root );
	
protected:
	Renderable();
	
	Transformation mTransformation;
};
	
}
