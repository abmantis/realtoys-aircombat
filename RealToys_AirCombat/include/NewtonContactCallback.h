#ifndef _AIRPLANECONTACTCALL_H_
#define _AIRPLANECONTACTCALL_H_

#include "DynamicObjectsManager.h"
#include "ScoresManager.h"
#include "ParticlesManager.h"

class AirplaneContactCallback: public OgreNewt::ContactCallback
{
public:
	AirplaneContactCallback();
	~AirplaneContactCallback(void);

	int onAABBOverlap( OgreNewt::Body* body0, OgreNewt::Body* body1, int threadIndex );
	void contactsProcess( OgreNewt::ContactJoint &contactJoint, Ogre::Real timeStep, int threadIndex );

private:
	int mPlaneType;	
	ScoresManager *mScoresManager;
};


class Shot1ContactCallback: public OgreNewt::ContactCallback
{
public:
	Shot1ContactCallback(Ogre::String namesPrefix);
	~Shot1ContactCallback(void);

	int onAABBOverlap( OgreNewt::Body* body0, OgreNewt::Body* body1, int threadIndex );
	void contactsProcess( OgreNewt::ContactJoint &contactJoint, Ogre::Real timeStep, int threadIndex );

private:
	DynamicObjectsManager *mDynObjMan;
	ScoresManager *mScoresManager;
	ParticlesManager *mParticlesManager;
	OgreOggSound::OgreOggSoundManager *mSoundMgr;
	OgreOggSound::OgreOggISound *mShotSound1;
	OgreOggSound::OgreOggISound *mShotSound2;

};

#endif