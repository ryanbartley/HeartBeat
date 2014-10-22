#pragma once

#include "SpringMesh.h"

class ModSpringMesh {
public:

	enum {
		BUTTON_0,
		BUTTON_1,
		BUTTON_2,
		BUTTON_3,
		BUTTON_4,
		TOTAL_BUTTONS
	};

    ModSpringMesh();
	virtual ~ModSpringMesh();

	static std::shared_ptr<ModSpringMesh> create();
    
	void							applyCircleForce( float aForceAmt, const ci::vec2& aForceP, float aForceRadius );

	virtual void					setup();

	virtual void					touchBegan( int32_t touchId, const ci::vec2& touchPos, const ci::vec2& touchPrevPos );
	virtual void					touchMoved( int32_t touchId, const ci::vec2& touchPos, const ci::vec2& touchPrevPos );
	virtual void					touchEnded( int32_t touchId, const ci::vec2& touchPos, const ci::vec2& touchPrevPos );
    
	virtual void					show();
	virtual void					update();
	virtual void					render();

private:
	void							loadBg( const std::string& relAssetPath );
	void							toggleDrawGrid( bool val );
	void							toggleBg0( bool val );
	void							toggleBg1( bool val );
	void							toggleBg2( bool val );
	void							toggleBg3( bool val );
	
	ci::gl::TextureRef				mBgTex;
	ci::gl::GlslProgRef				mShader;
	ci::Rectf						mInputRect;
	SpringMesh3					mSpringMesh;

	double							mLastTouchTime;
	bool							mSplatActive;
	bool							mDrawGrid;
};