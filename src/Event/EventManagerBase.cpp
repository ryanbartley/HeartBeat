//
//  EventManagerBase.cpp
//  RendererTest
//
//  Created by Ryan Bartley on 8/17/14.
//
//

#include "EventManagerBase.h"
#include "cinder/Log.h"

namespace heartbeat {
	
static EventManagerBase* kEventManager = nullptr;
	
EventManagerBase* EventManagerBase::get()
{
//	CI_ASSERT(kEventManager);
	return kEventManager;
}
	
EventManagerBase::EventManagerBase( const std::string &name, bool setAsGlobal )
{
	if ( setAsGlobal ) {
		if ( kEventManager ) {
			CI_LOG_E("Attempting to make two Event Managers. Deleting the old one and replacing with the new one");
			delete kEventManager;
		}
		kEventManager = this;
	}
}
	
EventManagerBase::~EventManagerBase()
{
//	if ( kEventManager ) {
//		delete kEventManager;
//	}
}
	
}