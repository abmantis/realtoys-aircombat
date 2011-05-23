#ifndef InterSenseManager_h__
#define InterSenseManager_h__

#pragma once

#include "isense.h"

class InterSenseManager;

struct InterSenseCube
{
public:
	Ogre::Real GetYaw()		{ return mYaw; }
	Ogre::Real GetPitch()	{ return mPitch; }
	Ogre::Real GetRoll()	{ return mRoll; }
private:
	Ogre::Real mYaw, mPitch, mRoll;

	friend class InterSenseManager;
};

class InterSenseManager : public Ogre::Singleton<InterSenseManager>
{
public:
	InterSenseManager(void);
	~InterSenseManager(void);

	bool init();

	bool setActive(bool active);
	bool toggleActive();
	bool isActive();
	bool isInited();
	void resetAngles();
	InterSenseCube capture(Ogre::Real timeSinceLastFrame);

private:	
	ISD_TRACKER_HANDLE              mHandle;
	ISD_TRACKING_DATA_TYPE          mData;
	ISD_STATION_INFO_TYPE           mStations[ISD_MAX_STATIONS];
	ISD_TRACKER_INFO_TYPE           mTracker;
	ISD_HARDWARE_INFO_TYPE          mHwInfo;
	ISD_STATION_HARDWARE_INFO_TYPE  mStationsHwInfo[ISD_MAX_STATIONS];
	bool							mActive;

	InterSenseCube					mISenseCube;
	
	static const Ogre::Real			mMinElapsedTime;
	Ogre::Real						mElapsedTime; 

public:
	static InterSenseManager& getSingleton(void);
	static InterSenseManager* getSingletonPtr(void);

};
#endif // InterSenseManager_h__
