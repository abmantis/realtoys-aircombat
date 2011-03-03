#ifndef _SHOTS_MANAGER_H_
#define _SHOTS_MANAGER_H_

#include <stack>
#include "RealToysShared.h"
#include "ParticlesManager.h"

class ShotsManager
{
public:
	ShotsManager(int initialShotStackCount, float fireInterval, std::vector<Ogre::Vector3> airplaneGunPositions,
		Ogre::String shotsNamePrefix, Ogre::SceneManager *sceneMgr, OgreNewt::World *world, PlayerID ownerID, Ogre::SceneNode *planeNode);
	~ShotsManager();

	virtual void fire(float timeSinceLastCall, Ogre::Vector3 planePosition, Ogre::Quaternion planeOrientation, Ogre::Vector3 targetPos);
	virtual void stopFireSound();

private:	
	std::stack<Ogre::Entity*> mShotEntitiesStack;		//stores all shots entities
	Ogre::SceneManager *mSceneMgr;						//scene manager
	OgreOggSound::OgreOggSoundManager *mSoundMgr;		//Sound manager
	OgreOggSound::OgreOggISound *mShotSound;			//shot sounds
	Ogre::SceneNode *mPlaneNode;
	Ogre::String mShotsNamePrefix;						//prefix to name the bullets
	std::vector<Ogre::Vector3> mAirplaneGunPositions;	//vector of gun positions to fire bullets from
	int mAirplaneGunCount;								//number of guns to fire bullets from
	int mAirplaneGunIndex;								//index of the current gun in use to fire
	OgreNewt::World *mWorld;							//newton world
	OgreNewt::CollisionPtr mShotCollision;				//newton collision to apply to all shots
	const OgreNewt::MaterialID *mMaterialID;			//newton material to apply to all shots
	Ogre::Vector3 mCollisionInertialMatrix;				//newton inertia to apply to all shots
	float mFireInterval;								//fire rate (number of seconds between shots)
	float mTimeSinceLastShot;							//time passed since last shot
	PlayerID mOwnerID;

	bool mIsSoundPlaying;

	ParticlesManager *mParticlesManager;

	void createMaterials();
};

#endif