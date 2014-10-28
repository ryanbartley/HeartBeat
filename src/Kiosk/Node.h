//
//  Node.h
//  SvgTest
//
//  Created by Ryan Bartley on 10/20/14.
//
//

#pragma once

#include "Common.h"
#include "SvgManager.h"

#include "cinder/svg/Svg.h"

namespace heartbeat {
	
class Node {
public:
	
	virtual ~Node() {}
	
	const ci::svg::Group*	getRootGroup() { return mGroup; }
	const ci::svg::Node&	getChild( const std::string &childId ) { return mGroup->getChild( childId ); }
	ci::vec2&			getOffset() { return mOffset; }
	const ci::vec2&		getOffset() const { return mOffset; }
	uint8_t				getAntiAlias() const { return mAntiAlias; }
	std::string&		getGroupName() { return mName; }
	const std::string&	getGroupName() const { return mName; }
	
	virtual void initializeGl() = 0;
	virtual bool initialize( const ci::JsonTree &root ) = 0;
	
protected:
	Node( const std::string &name );
	
	virtual ci::gl::Texture2dRef initializeGl( const ci::svg::Group *group );
	
	const ci::svg::Group*	mGroup;
	std::string				mName;
	ci::vec2				mOffset;
	uint8_t					mAntiAlias;
};
	
class Page : public Node {
public:
	
	virtual ~Page() {}
	
	const ci::gl::Texture2dRef& getTexture() const { return mTexture; }
	ci::gl::Texture2dRef& getTexture() { return mTexture; }
	
	virtual void initializeGl() override;
	
protected:
	Page( const std::string &name );

	ci::gl::Texture2dRef	mTexture;
};

using ButtonId = uint64_t;

enum class ButtonStatus {
	ACTIVE,
	VISIBLE,
	INVISIBLE
};

class Button : public Node {
public:
	
	virtual ~Button() {}
	
	virtual void changeState( InfoDisplayRef &display ) = 0;
	
	ButtonStatus			getStatus() const { return mStatus; }
	virtual ButtonId		getType() const = 0;
	
	void setStatus( ButtonStatus status ) { mStatus = status; }
	
	inline bool contains( const ci::vec2 &point ) { return mGroup->containsPoint( point ); }
	
protected:
	Button( const std::string &name );
	
	ButtonStatus			mStatus;
};
	
}
