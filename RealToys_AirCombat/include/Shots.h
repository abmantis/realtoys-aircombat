#ifndef _SHOTS_H_
#define _SHOTS_H_

#include "StdAfx.h"
#include "DynamicObjectsManager.h"
#include "ParticlesManager.h"
#include <stack>

class Shot1: public DynamicObject
{
public:
	Shot1(Ogre::SceneManager *sceneMgr, OgreNewt::World *world, Ogre::Vector3 position, 
		Ogre::Quaternion direction, OgreNewt::CollisionPtr collision, Ogre::Vector3 inertia, const OgreNewt::MaterialID* materialID, 
		std::stack<Ogre::Entity*> * shotStackPtr, PlayerID ownerID, ParticlesManager *particlesMgr)
		:DynamicObject(), mSceneMgr(sceneMgr), mShotStackPtr(shotStackPtr), mWorld(world), mOwner(ownerID), mParticlesMgr(particlesMgr)
	{

		mNew = true;
		mDamage = 2;

		mShotNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
		mEntity = mShotStackPtr->top();
		mShotStackPtr->pop();
		mEntity->setVisible(true);
		mShotNode->attachObject(mEntity);
		//mShotNode->setOrientation(direction);
		//mShotNode->setPosition(position);

		Ogre::Real mass = 0.01f;
		mBody = new OgreNewt::Body(mWorld, collision);
		mBody->attachNode(mShotNode);
		mBody->setOgreUpdateScaleFactor(RealToys::OgreNewtonFactor);
		mBody->setUserData(Ogre::Any(this));	
		mBody->setMassMatrix(mass, inertia*mass);
		mBody->setPositionOrientation(RealToys::ToNewton(position), RealToys::ToNewton(direction));
		mBody->setType(RealToys::BODYTYPE_SHOT1);
		mBody->setMaterialGroupID(materialID);
		mBody->setLinearDamping(0.0001f);
		mBody->setAutoSleep(0);		
		mBody->setVelocity(RealToys::ToNewton(direction*Ogre::Vector3(0, 0, 1000)));

		//particles
		mParticles = mParticlesMgr->requestShotBlueFlameParticles(mShotNode);		
	}
	Shot1::~Shot1()
 	{
		if(mBody)
			delete mBody;		
		mEntity->setVisible(false);	
		mShotStackPtr->push(mEntity);

		mShotNode->detachAllObjects();
		
		if(mParticles)
			mParticlesMgr->deliverShotBlueFlameParticles(mParticles);

		mSceneMgr->getRootSceneNode()->removeAndDestroyChild(mShotNode->getName());	
	}

	PlayerID mOwner;
	int mDamage;
	bool mNew;
private:
	Ogre::SceneManager *mSceneMgr;
	Ogre::SceneNode* mShotNode;
	Ogre::Entity *mEntity;
	std::stack<Ogre::Entity*> *mShotStackPtr;
	OgreNewt::World *mWorld;
	OgreNewt::Body *mBody;
	Ogre::ParticleSystem *mParticles;
	ParticlesManager *mParticlesMgr;
};

#endif