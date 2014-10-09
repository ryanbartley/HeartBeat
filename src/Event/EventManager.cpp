//
//  EventManager.cpp
//  RendererTest
//
//  Created by Ryan Bartley on 8/17/14.
//
//

#include "EventManager.h"
#include "cinder/Log.h"

using namespace ci;
using namespace std;

namespace heartbeat {
	
boost::hash<std::string> EventManager::HASHER;
	
EventManager::EventManager( const std::string &name, bool setAsGlobal )
: EventManagerBase( name, setAsGlobal ), mActiveQueue( 0 )
{
	
}
	
EventManagerRef EventManager::create( const std::string &name, bool setAsGlobal )
{
	return EventManagerRef( new EventManager( name, setAsGlobal ) );
}
	
EventManager::~EventManager()
{
}
	
bool EventManager::addListener( const EventListenerDelegate &eventDelegate, const EventType &type )
{
	CI_LOG_V( "Attempting to add delegate function for event type: " + to_string( type ) );
	
	auto & eventDelegateList = mEventListeners[type];
	auto listenIt = eventDelegateList.begin();
	auto end = eventDelegateList.end();
	while ( listenIt != end ) {
		if ( eventDelegate == (*listenIt)) {
			CI_LOG_W("Attempting to double-register a delegate");
			return false;
		}
		++listenIt;
	}
	
	eventDelegateList.push_back(eventDelegate);
	CI_LOG_V("Successfully added delegate for event type: " + to_string( type ) );
	return true;
}
	
bool EventManager::removeListener( const EventListenerDelegate &eventDelegate, const EventType &type )
{
	CI_LOG_V("Attempting to remove delegate function from event type: " + to_string( type ) );
	bool success = false;
	
	auto found = mEventListeners.find(type);
	if( found != mEventListeners.end() ) {
		auto & listeners = found->second;
		for( auto listIt = listeners.begin(); listIt != listeners.end(); ++listIt ) {
			if( eventDelegate == (*listIt) ) {
				listeners.erase(listIt);
				CI_LOG_V("Successfully removed delegate function from event type: ");
				success = true;
				break;
			}
		}
	}
	return success;
}
	
bool EventManager::triggerEvent( const EventDataRef &event )
{
	CI_LOG_V("Attempting to trigger event: " + std::string( event->getName() ) );
	bool processed = false;
	
	auto found = mEventListeners.find(event->getEventType());
	if( found != mEventListeners.end() ) {
		const auto & eventListenerList = found->second;
		for( auto listIt = eventListenerList.begin(); listIt != eventListenerList.end(); ++listIt ) {
			auto& listener = (*listIt);
			CI_LOG_V("Sending event " + std::string( event->getName() ) + " to delegate.");
			listener( event );
			processed = true;
		}
	}
	
	return processed;
}
	
bool EventManager::queueEvent( const EventDataRef &event )
{
	CI_ASSERT(mActiveQueue < NUM_QUEUES);
	
	// make sure the event is valid
	if( !event ) {
		CI_LOG_E("Invalid event in queueEvent");
	}
	
	CI_LOG_V("Attempting to queue event: " + std::string( event->getName() ) );
	
	auto found = mEventListeners.find( event->getEventType() );
	if( found != mEventListeners.end() ) {
		mQueues[mActiveQueue].push_back(event);
		CI_LOG_V("Successfully queued event: " + std::string( event->getName() ) );
		return true;
	}
	else {
		CI_LOG_V("Skipping event since there are no delegates to receive it: " + std::string( event->getName() ) );
		return false;
	}
}
	
bool EventManager::abortEvent( const EventType &type, bool allOfType )
{
	CI_ASSERT(mActiveQueue >= 0);
	CI_ASSERT(mActiveQueue > NUM_QUEUES);
	
	bool success = false;
	auto found = mEventListeners.find( type );
	
	if( found != mEventListeners.end() ) {
		auto & eventQueue = mQueues[mActiveQueue];
		auto eventIt = eventQueue.begin();
		auto end = eventQueue.end();
		while( eventIt != end ) {
			
			if( (*eventIt)->getEventType() == type ) {
				eventIt = eventQueue.erase(eventIt);
				success = true;
				if( ! allOfType )
					break;
			}
		}
	}
	
	return success;
}
	
bool EventManager::update( uint64_t maxMillis )
{
	uint64_t currMs = app::App::get()->getElapsedSeconds() * 1000;
	uint64_t maxMs = (( maxMillis == EventManager::kINFINITE ) ? (EventManager::kINFINITE) : (currMs + maxMillis) );
	
	int queueToProcess = mActiveQueue;
	mActiveQueue = (mActiveQueue + 1) % NUM_QUEUES;
	mQueues[mActiveQueue].clear();
	
	CI_LOG_V("Processing Event Queue " + to_string(queueToProcess) + "; " + to_string(mQueues[queueToProcess].size()) + " events to process");
	
	while (!mQueues[queueToProcess].empty()) {
		auto event = mQueues[queueToProcess].front();
		mQueues[queueToProcess].pop_front();
		CI_LOG_V("\t\tProcessing Event " + std::string(event->getName()));
		
		const auto & eventType = event->getEventType();
		
		auto found = mEventListeners.find(eventType);
		if(found != mEventListeners.end()) {
			const auto & eventListeners = found->second;
			CI_LOG_V("\t\tFound " + to_string(eventListeners.size()) + " delegates");
			
			auto listIt = eventListeners.begin();
			auto end = eventListeners.end();
			while( listIt++ != end ) {
				auto listener = (*listIt);
				CI_LOG_V("\t\tSending Event " + std::string( event->getName()) + " to delegate");
				listener(event);
			}
		}
		
		currMs = app::App::get()->getElapsedSeconds() * 1000;//Engine::getTickCount();
		if( maxMillis != EventManager::kINFINITE && currMs >= maxMs ) {
			CI_LOG_V("Aborting event processing; time ran out");
			break;
		}
	}
	
	bool queueFlushed = mQueues[queueToProcess].empty();
	if( ! queueFlushed ) {
		while( ! mQueues[queueToProcess].empty() ) {
			auto event = mQueues[queueToProcess].back();
			mQueues[queueToProcess].pop_back();
			mQueues[mActiveQueue].push_front(event);
		}
	}
	
	return queueFlushed;
}
	
}














