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
	
	virtual void debugRender() = 0;
	virtual void render() = 0;
	
	const ci::vec2& getScenePosition() { return mAbsolutePosition; }
	bool			isAnimating() { return mIsAnimating; }
	float			getCurrentAlpha() const { return mCurrentAlpha; }
	float&			getCurrentAlpha() { return mCurrentAlpha; }
	ci::vec2&		getCurrentPosition() { return mCurrentPosition; }
	const ci::vec2& getCurrentPosition() const { return mCurrentPosition; }
	
protected:
	Node( const std::string &name );
	
	virtual ci::gl::Texture2dRef initializeGl( const ci::svg::Group *group );
	
	const ci::svg::Group*	mGroup;
	std::string				mName;
	ci::vec2				mOffset;
	uint8_t					mAntiAlias;
	
	ci::vec2				mAbsolutePosition;
	bool					mIsAnimating;
	float					mCurrentAlpha;
	ci::vec2				mCurrentPosition;
};
	
using PageId = uint64_t;
	
class Page : public Node {
public:
	
	virtual ~Page() {}
	
	virtual PageId getType() const = 0;
	
	virtual PageRef clone() = 0;
	
	const std::string& getButtonName() const { return mButton; }
	
protected:
	Page( const std::string &name );

	std::string				mButton;
	
};

using ButtonId = uint64_t;

class Button : public Node {
public:
	
	virtual ~Button() {}
	
	//! Returns the buttonId
	virtual ButtonId		getType() const = 0;
	
	//! Returns whether the point is contained within this node.
	inline bool contains( const ci::vec2 &point ) { return mGroup->getBoundingBox().contains( point ); }
	//! Abstract method used to change the state of the infodisplay
	virtual void changeState( InfoDisplayRef &display ) = 0;
	
	//! Copies things that need to be copied.
	virtual ButtonRef clone() = 0;
	
protected:
	Button( const std::string &name );
	
};
	
class StaticElement : public Node {
public:
	
private:
	StaticElement( const std::string &name );
};
	
}
