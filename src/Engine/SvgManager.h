//
//  SvgManager.h
//  ProjectionTestApp
//
//  Created by Ryan Bartley on 10/1/14.
//
//

#pragma once

#include "Common.h"
#include "cinder/svg/Svg.h"

namespace cinder {
class JsonTree;
}



namespace heartbeat {
	
using OverlayPageCreator	= std::function<OverlayPageRef(const std::string&)>;
using OverlayPageCreators	= std::map<std::string, OverlayPageCreator>;
using ButtonCreator			= std::function<ButtonRef(const std::string&)>;
using ButtonCreators		= std::map<std::string, ButtonCreator>;
	
class SvgManager {
public:
	
	static SvgManagerRef create();
	static SvgManagerRef get();
	static void destroy();
	static OverlayPageCreators	OverlayCreators;
	static ButtonCreators		ButtonCreators;
	
	SvgManager( const SvgManager & ) = delete;
	SvgManager( SvgManager && ) = delete;
	SvgManager& operator=( const SvgManager & ) = delete;
	SvgManager& operator=( SvgManager && ) = delete;
	
	~SvgManager();
	
	const ci::svg::Group* getGroup( const std::string &groupId ) { return mDoc->find<ci::svg::Group>( groupId ); }
	const ci::svg::Node* getNode( const std::string &groupId ) { return mDoc->findNode( groupId ); }
	const ci::svg::DocRef& getDoc() { return mDoc; }
	
	void initialize();
	
	ButtonRef		getButton( const std::string &name );
	DataPageRef		getData( const std::string &name );
	OverlayPageRef	getOverlay( const std::string &name );
	
private:
	SvgManager();
	
	void initializeDataPages( const ci::JsonTree &root );
	void initializeOverlayPages( const ci::JsonTree &root );
	void initializeButtons( const ci::JsonTree &root );
	
	void initializeGl();
	
	ci::svg::DocRef mDoc;
	std::string		mFileName;
	
	std::map<std::string, ButtonRef>		mButtons;
	std::map<std::string, PageRef>			mPages;
	
	friend class Engine;
};
	
}
