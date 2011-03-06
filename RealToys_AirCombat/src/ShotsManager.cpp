#include "StdAfx.h"
#include "ShotsManager.h"
#include "RealToysShared.h"
#include "NewtonMaterialManager.h"
#include "Shots.h"
#include "NewtonContactCallback.h"


/*
	ShotsManager
*/
ShotsManager::ShotsManager(int initialShotStackCount, float fireInterval,
						   std::vector<Ogre::Vector3> airplaneGunPositions, Ogre::String shotsNamePrefix,
						   Ogre::SceneManager *sceneMgr, OgreNewt::World *world, PlayerID ownerID, Ogre::SceneNode *planeNode)
:mFireInterval(fireInterval), mAirplaneGunPositions(airplaneGunPositions), 
	mSceneMgr(sceneMgr), mShotsNamePrefix(shotsNamePrefix), mWorld(world), mOwnerID(ownerID), mPlaneNode(planeNode)
{	
	mParticlesManager = ParticlesManager::getSingletonPtr();
	mTimeSinceLastShot = fireInterval;
	mAirplaneGunCount = mAirplaneGunPositions.size();
	mAirplaneGunIndex = 0;
	mIsSoundPlaying = false;

	//fill stack
	Ogre::Entity *baseShotEntity = mSceneMgr->createEntity(shotsNamePrefix+"baseShot", "Cylinderbullet.mesh");
	Ogre::String entName;
	for (int t = 0; t < initialShotStackCount; t++)
	{		
		entName = shotsNamePrefix + Ogre::StringConverter::toString(t);
		Ogre::Entity* newShotEntity = baseShotEntity->clone(entName);
		newShotEntity->setVisible(false);
		newShotEntity->setQueryFlags(RealToys::MASK_NONE);
		mShotEntitiesStack.push(newShotEntity);
	}

	Ogre::Quaternion collOri;
	collOri.FromAngleAxis(Ogre::Degree(-90), Ogre::Vector3::UNIT_Y);
	//create shared newton collision
	Ogre::AxisAlignedBox bbox = baseShotEntity->getBoundingBox();
	OgreNewt::CollisionPrimitives::Cylinder *cylColl = 	new OgreNewt::CollisionPrimitives::Cylinder(
		mWorld, bbox.getSize().x*0.5f, bbox.getSize().z, 0, collOri );
	mShotCollision = OgreNewt::CollisionPtr( cylColl );
	cylColl->calculateInertialMatrix(mCollisionInertialMatrix, Ogre::Vector3());

	createMaterials();	

	//push gun positions forward so that shots start on the right place
	for(unsigned int i = 0; i < airplaneGunPositions.size(); i++)
	{
		mAirplaneGunPositions[i].z += bbox.getSize().z * 0.5f;
	}


	//create sounds
	mSoundMgr = OgreOggSound::OgreOggSoundManager::getSingletonPtr();
	if(mShotSound = mSoundMgr->createSound(shotsNamePrefix + "_sound", "Shot1.wav", false, true, true))
	{
		mShotSound->setPitch(1);
		mShotSound->setMaxDistance(650);
		mShotSound->setReferenceDistance(300);
		mShotSound->setRolloffFactor(1);
		mPlaneNode->attachObject(mShotSound);
	}
	
	

}
ShotsManager::~ShotsManager(void)
{
	while(mShotEntitiesStack.empty() == false)
	{
		Ogre::Entity* shotEntity = mShotEntitiesStack.top();
		mShotEntitiesStack.pop();

		shotEntity->detachFromParent();
		mSceneMgr->destroyEntity(shotEntity);
	}

	mShotSound->stop();
	mSoundMgr->destroySound(mShotSound->getName());
}

void ShotsManager::fire(float timeSinceLastCall, Ogre::Vector3 position, Ogre::Quaternion orientation, Ogre::Vector3 targetPos)
{	
	mTimeSinceLastShot += timeSinceLastCall;	

	if(mTimeSinceLastShot >= mFireInterval)
	{
		mTimeSinceLastShot = 0;
		if(!mShotEntitiesStack.empty())
		{			
			mAirplaneGunIndex++;
			if(mAirplaneGunIndex == mAirplaneGunCount)
				mAirplaneGunIndex = 0;

			position += orientation*mAirplaneGunPositions[mAirplaneGunIndex];

			Ogre::Vector3 targetDir = targetPos-position;
			targetDir.normalise();

			new Shot1(mSceneMgr, mWorld, position, Ogre::Vector3::UNIT_Z.getRotationTo( targetDir ),
				mShotCollision, mCollisionInertialMatrix, mMaterialID, &mShotEntitiesStack, mOwnerID, mParticlesManager);	

			if(!mIsSoundPlaying)
			{
				mShotSound->play();
				mIsSoundPlaying = true;		
			}				

			return;
		}
		else
		{
			std::cout << "Shot stack empty!" << std::endl;
			//if it has come to this point is because it didn't fire for some reason
			if(mIsSoundPlaying)
			{				
				mShotSound->pause();
				mIsSoundPlaying = false;		
			}
		}
	}


}

void ShotsManager::createMaterials()
{	
	mMaterialID = NewtonMaterialManager::getSingletonPtr()->getMaterialID(NME_SHOT1);

	Shot1ContactCallback *shotCallback = new Shot1ContactCallback(mShotsNamePrefix+"_Collision");

	OgreNewt::MaterialPair *matPairShotAirplane = new OgreNewt::MaterialPair( mWorld, 
		mMaterialID, NewtonMaterialManager::getSingletonPtr()->getMaterialID(NME_AIRPLANE) );
		
	matPairShotAirplane->setContinuousCollisionMode(1);
	matPairShotAirplane->setContactCallback(shotCallback);


	OgreNewt::MaterialPair *matPairShotDefault = new OgreNewt::MaterialPair( mWorld, 
		mMaterialID, NewtonMaterialManager::getSingletonPtr()->getMaterialID(NME_DEFAULT) );
	
	matPairShotDefault->setContinuousCollisionMode(1);
	matPairShotDefault->setContactCallback(shotCallback);
}

void ShotsManager::stopFireSound()
{
	if(mIsSoundPlaying)
		mShotSound->pause();
	mIsSoundPlaying = false;
}