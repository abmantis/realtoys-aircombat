#include "StdAfx.h"
#include "RealToysShared.h"
#include "InterSenseManager.h"

const Ogre::Real InterSenseManager::mMinElapsedTime = 0.018f;
template<> InterSenseManager* Ogre::Singleton<InterSenseManager>::ms_Singleton = 0;

InterSenseManager::InterSenseManager(void)
{
	mHandle = -1;	
	mActive = false;
}

InterSenseManager::~InterSenseManager(void)
{
	ISD_CloseTracker( mHandle );
}

InterSenseManager& InterSenseManager::getSingleton(void)
{
	assert( ms_Singleton );  return ( *ms_Singleton );  
}
InterSenseManager* InterSenseManager::getSingletonPtr(void)
{
	return ms_Singleton;
}

bool InterSenseManager::init()
{
	WORD station = 1;
	DWORD maxStations = 4;


	// Detect first tracker. If you have more than one InterSense device and
	// would like to have a specific tracker, connected to a known port, 
	// initialized first, then enter the port number instead of 0. Otherwise, 
	// tracker connected to the rs232 port with lower number is found first 

	mHandle = ISD_OpenTracker( (Hwnd) NULL, 0, FALSE, TRUE );

	// Check value of mHandle to see if tracker was located 
	if( mHandle < 1 )
	{
		Ogre::LogManager::getSingletonPtr()->logMessage(RealToys::logMessagePrefix 
			+ "Failed to detect InterSense tracking device");
		mHandle = -1;
		mActive = false;
		return false;
	}

	Ogre::LogManager::getSingletonPtr()->logMessage(RealToys::logMessagePrefix 
		+ "InterSense tracking device detected :)");

	// Get tracker configuration info 
	ISD_GetTrackerConfig( mHandle, &mTracker, TRUE );

	memset((void *) &mHwInfo, 0, sizeof(mHwInfo));

	if( ISD_GetSystemHardwareInfo( mHandle, &mHwInfo ) )
	{
		if( mHwInfo.Valid )
		{
			maxStations = mHwInfo.Capability.MaxStations;
		}
	}

	// Clear station configuration info to make sure GetAnalogData and other flags are FALSE 
	memset( (void *) mStations, 0, sizeof(mStations) );

	// General procedure for changing any setting is to first retrieve current 
	// configuration, make the change, and then apply it. Calling 
	// ISD_GetStationConfig is important because you only want to change 
	// some of the settings, leaving the rest unchanged. 

	if( mTracker.TrackerType == ISD_PRECISION_SERIES )
	{
		for( station = 1; station <= maxStations; station++ )
		{         
			// fill ISD_STATION_INFO_TYPE structure with current station configuration 
			if( !ISD_GetStationConfig( mHandle, 
				&mStations[station-1], station, TRUE ) ) break;

			if( !ISD_GetStationHardwareInfo( mHandle, 
				&mStationsHwInfo[station-1], station ) ) break;
		}
	}


	mElapsedTime = mMinElapsedTime+1;
	mActive = true;

	return true;
}

InterSenseCube InterSenseManager::capture( Ogre::Real timeSinceLastFrame )
{
	mElapsedTime += timeSinceLastFrame;

	if(mElapsedTime > mMinElapsedTime)
	{
		mElapsedTime = 0;

		ISD_GetTrackingData( mHandle, &mRawData );

		mISenseCube.mYaw = mRawData.Station[0].Euler[0] - mISenseCubeAdjustment.mYaw;
		mISenseCube.mPitch = mRawData.Station[0].Euler[1] - mISenseCubeAdjustment.mPitch;
		mISenseCube.mRoll = mRawData.Station[0].Euler[2] - mISenseCubeAdjustment.mRoll;
	}

	return mISenseCube;
}

void InterSenseManager::resetAngles()
{
	//ISD_Boresight(mHandle, 1, TRUE);
	
	ISD_GetTrackingData( mHandle, &mRawData );
	mISenseCubeAdjustment.mYaw = mRawData.Station[0].Euler[0];
	mISenseCubeAdjustment.mPitch = mRawData.Station[0].Euler[1];
	mISenseCubeAdjustment.mRoll = mRawData.Station[0].Euler[2];
}

bool InterSenseManager::isActive()
{
	return (mHandle > 0) && mActive;
}

bool InterSenseManager::setActive( bool active )
{
	if(mHandle > 0)
	{
		mActive = active;
	}
	else
	{
		mActive = false;
	}
	return mActive;
}

bool InterSenseManager::toggleActive()
{
	if(mHandle > 0)
	{
		mActive = !mActive;
	}
	else
	{
		mActive = false;
	}

	return mActive;
}

bool InterSenseManager::isInited()
{
	return (mHandle > 0);
}


