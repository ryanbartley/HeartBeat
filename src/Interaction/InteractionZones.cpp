//
//  InteractionZones.cpp
//  LibUrgTest
//
//  Created by Ryan Bartley on 10/8/14.
//
//

#include "InteractionZones.h"

#include "cinder/Log.h"

#include "InteractionEvents.h"
#include "EventManager.h"
#include "JsonManager.h"

#include "Urg.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace heartbeat {
	
void ApproachData::activate( bool enable )
{
	mIsActivated = enable;
}
	
InteractionZones::InteractionZones()
    : mZoneScalarsUpdated( false ), mInBetweenThreshold( 0 ), mSendEvents( true )
{
	
}
	
InteractionZonesRef	InteractionZones::create()
{
	return InteractionZonesRef( new InteractionZones() );
}
	
void InteractionZones::serialize( DataTargetPathRef &file )
{
	if( mBarrier.empty() ) {
		CI_LOG_E("Barrier is empty.");
		return;
	}
	
	size_t totalDataSize = 0;

	size_t numLongs = mBarrier.size();
	totalDataSize += ( ( mBarrier.size() * sizeof(long) ) + sizeof(size_t) );
	Buffer buf( totalDataSize );
	
	auto longNum = (size_t*) buf.getData();
	*longNum++ = numLongs;
	
	auto longData = (long*) longNum;
	for( auto & barrier : mBarrier ) {
		*longData++ = barrier;
	}
	
	file->getStream()->write( buf );
}
	
void InteractionZones::deserialize( const ci::DataSourceRef &file )
{
	auto buf = file->getBuffer();
	
	auto longNum = (size_t*) buf.getData();
	auto numLongs = *longNum++;
	
	auto longData = (long*) longNum;
	mBarrier.resize( numLongs );
	for( int i = 0; i < numLongs; ++i ) {
		mBarrier[i] = *longData++;
	}
	
	if( mZoneUpdateFunc )
		mZoneUpdateFunc();
}
	
void InteractionZones::initialize()
{
	try {
		auto interactionAttribs = JsonManager::get()->getRoot()["interactionZones"];
		
		try {
			auto runUrg = interactionAttribs["runUrg"].getValue<bool>();
			
			if( runUrg ) {
				auto urg = Urg::create();
				if( urg->initialize() ) {
					if( urg->open() )
						mUrg = urg;
					else
						CI_LOG_W("Couldn't open Urg");
				}
				else {
					CI_LOG_W("Couldn't initialize Urg");
				}
			}
			else
				CI_LOG_V("Not running urg");
		}
		catch( JsonTree::ExcChildNotFound &ex ) {
			CI_LOG_W("runUrg child not found, defaulting to false");
		}
		
		try {
			auto approachZones = interactionAttribs["approachZones"];
		
			auto topZone = approachZones["top"];
			mApproachZones.insert( make_pair( KioskId::TOP_KIOSK, ApproachData( KioskId::TOP_KIOSK, topZone.getChild(0).getValue<int>(), topZone.getChild(1).getValue<int>() ) ) );
			auto middleZone = approachZones["middle"];
			mApproachZones.insert( make_pair( KioskId::MIDDLE_KIOSK, ApproachData( KioskId::MIDDLE_KIOSK, middleZone.getChild(0).getValue<int>(), middleZone.getChild(1).getValue<int>() ) ) );
			auto bottomZone = approachZones["bottom"];
			mApproachZones.insert( make_pair( KioskId::BOTTOM_KIOSK, ApproachData( KioskId::BOTTOM_KIOSK, bottomZone.getChild(0).getValue<int>(), bottomZone.getChild(1).getValue<int>() ) ) );
		}
		catch( const JsonTree::ExcChildNotFound &ex ) {
			CI_LOG_E("Approach Divisions problem, using default top - 330, middle - 660, bottom - 1080 " << ex.what());
			mApproachZones.insert( make_pair( KioskId::TOP_KIOSK, ApproachData( KioskId::TOP_KIOSK, 0, 330 ) ) );
			mApproachZones.insert( make_pair( KioskId::MIDDLE_KIOSK, ApproachData( KioskId::MIDDLE_KIOSK, 331, 660 ) ) );
			mApproachZones.insert( make_pair( KioskId::BOTTOM_KIOSK, ApproachData( KioskId::BOTTOM_KIOSK, 661, 1080 ) ) );
		}
		
		try {
			auto poleIndices = interactionAttribs["poleIndices"];
			
			for( auto & index : poleIndices ) {
				mIgnoreIndices.push_back( index.getValue<uint32_t>() );
			}
		}
		catch( const JsonTree::ExcChildNotFound &ex ) {
			CI_LOG_E("PoleIndices not found, using default");
			mSendEvents = false;
		}
		
		// Now cache scales for barrier zones
		try {
			auto scales = interactionAttribs["scales"];
			
			// Table scale
			try {
				auto tableScale = scales["table"].getValue<float>();
				mZones[Zone::TABLE] = tableScale;
			}
			catch ( JsonTree::ExcChildNotFound &ex ) {
				CI_LOG_W("no table child found, using default - .85f");
				mZones.insert( make_pair( Zone::TABLE, .85f ) );
			}
			
			// Dead scale
			try {
				auto deadScale = scales["dead"].getValue<float>();
				mZones[Zone::DEAD] = deadScale;
			}
			catch ( JsonTree::ExcChildNotFound &ex ) {
				CI_LOG_W("no dead child found, using default - 1.2f");
				mZones.insert( make_pair( Zone::DEAD, 1.0f ) );
			}
			
			// Approach scale
			try {
				auto approachScale = scales["approach"].getValue<float>();
				mZones[Zone::APPROACH] = approachScale;
			}
			catch ( JsonTree::ExcChildNotFound &ex ) {
				CI_LOG_W("no approach child found, using default - 1.5f");
				mZones.insert( make_pair( Zone::APPROACH, 1.5f ) );
			}
			
			// Far Scale
			try {
				auto farScale = scales["far"].getValue<float>();
				mZones[Zone::FAR] = farScale;
			}
			catch ( JsonTree::ExcChildNotFound &ex ) {
				CI_LOG_W("no far child found, using default - 1.8f");
				mZones.insert( make_pair( Zone::FAR, 1.8f ) );
			}
			
		}
		catch ( JsonTree::ExcChildNotFound &ex ) {
			CI_LOG_W(ex.what() << "using default scales\n\tTABLE - 0.85f\n\tDEAD - 1.2f\n\tAPPROACH - 1.5f\n\tFAR - 1.8f");
			mZones.insert( make_pair( Zone::TABLE, .85f ) );
			mZones.insert( make_pair( Zone::DEAD, 1.0f ) );
			mZones.insert( make_pair( Zone::APPROACH, 1.5f ) );
			mZones.insert( make_pair( Zone::FAR, 1.8f ) );
		}
		
		try {
			auto transform = interactionAttribs["transforms"];
			try {
				
				auto translationRoot = transform["translations"];
				
				if( translationRoot.hasChild( "coordinates" ) ) {
					CI_LOG_V("Using coordinate version of translation");
					auto translations = translationRoot["coordinates"].getChildren();
					int i = 0;
					vec2 translation;
					for( auto & position : translations ) {
						translation[i++] = position.getValue<float>();
					}
					mTransform.setTranslation( vec3(translation, 0) );
				}
				else if( translationRoot.hasChild( "center" ) ) {
					CI_LOG_V("Using center version of translation");
					auto center = translationRoot["center"].getValue<bool>();
					if( center ) {
						auto center = vec2( app::App::get()->getWindowSize() ) / 2.0f;
						mTransform.setTranslation( vec3(center, 0) );
					}
				}
			}
			catch ( JsonTree::ExcChildNotFound &ex ) {
				CI_LOG_W("translations child not found, using default 0, 0, 0");
			}
			
			try {
				auto scales = transform["scales"].getChildren();
				int i = 0;
				vec2 scale;
				for( auto & scalar : scales ) {
					scale[i++] = scalar.getValue<float>();
				}
				mTransform.setScale( vec3(scale, 1.0f) );
				CI_LOG_V(scale);
			}
			catch ( JsonTree::ExcChildNotFound &ex ) {
				CI_LOG_W("Scale child not found, using default 1, 1, 1");
				mTransform.setScale( vec3( 1 ) );
			}
			
			try {
				auto rotationRoot = transform["rotations"];
				if( rotationRoot.hasChild( "quat" ) ) {
					CI_LOG_V("Using quat version of rotation");
					auto quaternion = rotationRoot["quat"].getChildren();
					int i = 0;
					quat rotation;
					for( auto & rotationStep : quaternion ) {
						rotation[i++] = rotationStep.getValue<float>();
					}
					mTransform.setRotation( rotation );
				}
				else if( rotationRoot.hasChild( "angle" ) ) {
					CI_LOG_V("Using angle axis version of rotation");
					auto angle = rotationRoot["angle"].getValue<float>();
					mTransform.setRotation( quat( rotate( toRadians(angle), vec3( 0, 0, 1 ) ) ) );
				}
				
			}
			catch ( JsonTree::ExcChildNotFound &ex ) {
				CI_LOG_V("Rotation child not found, using default 0, 0, 0, 1");
			}
		}
		catch ( JsonTree::ExcChildNotFound &ex ) {
			CI_LOG_E("Transform Child not found " << ex.what() );
		}
		
		try {
			auto inBetweenThresh = interactionAttribs["inBetweenThresh"].getValue<uint32_t>();
			mInBetweenThreshold = inBetweenThresh;
		}
		catch ( JsonTree::ExcChildNotFound &ex ) {
			CI_LOG_W("no inBetweenThresh child found, defaulting to 50");
			mInBetweenThreshold = 50;
		}
		
		// Now try to find the file open and buffer it.
		try {
			auto barrierFileName = interactionAttribs["barrierFile"].getValue();
			
			try {
				auto file = getFileContents( barrierFileName );
				
				deserialize( file );
			}
			catch ( StreamExc &ex ) {
				CI_LOG_E(ex.what() << " File: " << barrierFileName << " most likely not found." );
			}
			
			
		}
		catch ( JsonTree::ExcChildNotFound &ex ) {
			CI_LOG_E("barrierFile child not found");
		}
		
	}
	catch ( JsonTree::ExcChildNotFound & ex	) {
		CI_LOG_E("interactionZones child not found");
	}
}
	
bool InteractionZones::writeInteractionZone()
{
	auto writeableFile = getWriteablePath("tableData.bin");
	if( !writeableFile )
		return false;
	
	serialize( writeableFile );
	
	return true;
}
	
void InteractionZones::captureBarrier()
{
	if( !mUrg ) {
		CI_LOG_V("Attempting to capture Barrier from Urg Data without an Urg");
		return;
	}
	
	CI_LOG_V("Capturing Barrier");
	
	mUrg->read();
	auto & data = mUrg->getCurrentData();
		
	if( data.size() != mBarrier.size() )
		mBarrier.resize( data.size() );
	
	memcpy( mBarrier.data(), data.data(), data.size() * sizeof( long ) );
	
	if( mZoneUpdateFunc )
		mZoneUpdateFunc();
}
	
void InteractionZones::setZoneScalar( Zone zone, float scalar )
{
	auto found = mZones.find( zone );
	
	if( found != mZones.end() ) {
		found->second = scalar;
		mZoneScalarsUpdated = true;
	}
	if( mZoneUpdateFunc )
		mZoneUpdateFunc();
}
	
float InteractionZones::getZoneScalar( Zone zone )
{
	auto found = mZones.find( zone );
	
	if( found != mZones.end() )
		return found->second;
	else
		return 0.0f;
}
	
void InteractionZones::process()
{
	static int captureFrames = 50;
	if( ! mUrg ) {
		static bool runOnce = false;
		if( ! runOnce ) {
			runOnce = true;
			CI_LOG_V("Attempting to process Urg Interaction Data without an Urg");
		}
		return;
	}
	
		
	mUrg->read();
	auto & data = mUrg->getCurrentData();
	
	if( mBarrier.empty() ) {
		CI_LOG_V("Barrier empty, must capture data");
		return;
	}
	
	static auto FAR_SCALAR = mZones[Zone::FAR];
	static auto APPROACH_SCALAR = mZones[Zone::APPROACH];
	static auto DEAD_SCALAR = mZones[Zone::DEAD];
	static auto TABLE_SCALAR = mZones[Zone::TABLE];
	
	if( mZoneScalarsUpdated ) {
		FAR_SCALAR = mZones[Zone::FAR];
		APPROACH_SCALAR = mZones[Zone::APPROACH];
		DEAD_SCALAR = mZones[Zone::DEAD];
		TABLE_SCALAR = mZones[Zone::TABLE];
		mZoneScalarsUpdated = false;
	}
	
	std::vector<Interactor> approachEvents, touchEvents;
	bool checkPoleIndices = mSendEvents;
	int i = 0, k = 0;
	for( auto barrierIt = mBarrier.cbegin(); barrierIt != mBarrier.cend(); ++barrierIt, ++i ) {
		bool emitEvents = true;
		if( checkPoleIndices ) {
			if( mIgnoreIndices[k] == i ) {
				emitEvents = false;
				++k;
				if( mIgnoreIndices.size() - 1 < k )
					checkPoleIndices = false;
			}
		}
		if( emitEvents ) {
			if( data[i] == 1 ) data[i] = 100000;
			if( data[i] > *barrierIt * APPROACH_SCALAR ) {
				// do nothing
			}
			else if( data[i] < *barrierIt * APPROACH_SCALAR && data[i] > *barrierIt * DEAD_SCALAR ) {
				// emit approaching event.
				if( mSendEvents ) {
                    // emit touching event.
					addEvent( approachEvents, i, data[i] );
				}
				else {
					auto found = std::find( mIgnoreIndices.begin(), mIgnoreIndices.end(), i );
					if ( found == mIgnoreIndices.end() ) {
						mIgnoreIndices.push_back( i );
						std::sort( mIgnoreIndices.begin(), mIgnoreIndices.end(), []( int i, int j ) {
							return i < j;
						});
					}
				}

			}
			else if( data[i] > *barrierIt * DEAD_SCALAR ) {
				// do nothing
			}
			// I did something weird here. I care about the events below the
			// table scalar value. If it gets to this place and it's lower than
			// the scalar then I know someone's touching the table.
			else if( data[i] < *barrierIt * TABLE_SCALAR ) {
				if( mSendEvents ) {
				// emit touching event.
					addEvent( touchEvents, i, data[i] );
				}
				else {
					auto found = std::find( mIgnoreIndices.begin(), mIgnoreIndices.end(), i );
					if ( found == mIgnoreIndices.end() ) {
						mIgnoreIndices.push_back( i );
						std::sort( mIgnoreIndices.begin(), mIgnoreIndices.end(), []( int i, int j ) {
							return i < j;
						});
					}
				}
			}
		}
	}
	
	if( ! mSendEvents && ! ( captureFrames-- ) ) {
		mSendEvents = true;
		captureFrames = 50;
		return;
	}
	
	auto eventManager = EventManagerBase::get();
	
	if( eventManager == nullptr ) {
		static bool notified = false;
		if( ! notified ) {
			CI_LOG_E("No event manager even though I've processed all of the data");
			notified = true;
		}
		return;
	}
	
	if( ! approachEvents.empty() ) {
		processApproaches( approachEvents );
	}
	
	for( auto & zone : mApproachZones ) {
		auto & approachZone = zone.second;
		auto activated = approachZone.getIsActivated();
		auto numDistances = approachZone.getNumDistances();
		if( activated && numDistances == 0 ) {
			eventManager->queueEvent( EventDataRef( new DepartEvent( approachZone.getKiosk() ) ) );
			approachZone.activate( false );
		}
		else if( !activated && numDistances > 0 ) {
			eventManager->queueEvent( EventDataRef( new ApproachEvent( approachZone.getKiosk() ) ) );
			approachZone.activate( true );
		}
		approachZone.reset();
	}
	
	for( auto touchIt = touchEvents.begin(); touchIt != touchEvents.end(); ++touchIt ) {
		eventManager->queueEvent( EventDataRef( new TouchEvent( touchIt->mIndex, touchIt->mDistance, shared_from_this() ) ) );
	}
	
}

	
}