#include "StdAfx.h"
#include "AirplaneManager.h"
#include "RealToysShared.h"

#include "NewtonMaterialManager.h"

template<> AirplaneManager* Ogre::Singleton<AirplaneManager>::ms_Singleton = 0;
AirplaneManager::AirplaneManager(bool server, Ogre::SceneManager *sceneMgr,OgreNewt::World *world)
	:mIsServer(server), mWorld(world), mSceneMgr(sceneMgr)
{	
	mAirplanesListEnd = mAirplanesList.end();
	mLocalAirplane = 0;

	mAirplaneMarkersManager = AirplaneMarkersManager::getSingletonPtr();

	
	createMaterials();
}

AirplaneManager::~AirplaneManager(void)
{
}

Airplane* AirplaneManager::createAirplane(PlayerID ownerID)
{	
	//check if it does not exist
	if(findAirplane(ownerID) != NULL)
		return NULL;

	Airplane *airplane;
	if(mIsServer)
	{	
		airplane = new Airplane(mSceneMgr, mWorld, true);		
		airplane->setOwner(ownerID);
		Ogre::Vector3 pos;
		Ogre::Quaternion ori;		

		mAirplaneMarkersManager->getMarkerPositionOrientation(pos, ori);

		airplane->createPlane(pos, ori);
			

		if(ownerID == mLocalID)
			mLocalAirplane = airplane;	
	}
	else
	{
		airplane = new Airplane(mSceneMgr, mWorld, false);	
		//since it's on the client side, do not assign a name nor create the plane,
		//it will be done from the replica system.
	}

	
	mAirplanesList.push_back(airplane);
	mAirplanesListEnd = mAirplanesList.end();

	return airplane;
}

bool AirplaneManager::canHaveMoreAirplanes()
{
	return ( (int)mAirplanesList.size() < 
		AirplaneMarkersManager::getSingletonPtr()->getMarkerCount() );
}

AirplaneManager& AirplaneManager::getSingleton(void)
{
	assert( ms_Singleton );  return ( *ms_Singleton );  
}
AirplaneManager* AirplaneManager::getSingletonPtr(void)
{
	return ms_Singleton;
}

void AirplaneManager::createMaterials()
{	
	const OgreNewt::MaterialID* matDefault;
	OgreNewt::MaterialPair* matPairDefaultAirplane;
	OgreNewt::MaterialPair* matPairAirplaneAirplane;
	AirplaneContactCallback* airplaneContCallback;

	matDefault = NewtonMaterialManager::getSingletonPtr()->getMaterialID(NME_DEFAULT);
	mMatAirplane = NewtonMaterialManager::getSingletonPtr()->getMaterialID(NME_AIRPLANE);
	

	matPairDefaultAirplane = new OgreNewt::MaterialPair( mWorld, matDefault, mMatAirplane );
	matPairAirplaneAirplane = new OgreNewt::MaterialPair( mWorld, mMatAirplane, mMatAirplane);
	airplaneContCallback = new AirplaneContactCallback();

	matPairAirplaneAirplane->setContactCallback( airplaneContCallback );

	matPairDefaultAirplane->setContactCallback( airplaneContCallback );
//	mMatPairDefaultAirplane->setDefaultSoftness( 0.8 );
	matPairDefaultAirplane->setDefaultElasticity( 0.5 );
	matPairDefaultAirplane->setDefaultFriction( 0.009, 0.008 );
}

void AirplaneManager::updateAirplanes(Ogre::Real timeSinceLastFrame)
{
	// Update all airplanes (both in server and client)
	// search for deleted airplanes and remove them (server dont need this, because its the server that deletes them)

	Airplane *plane;
	std::list<Airplane*>::iterator deletedIt;
	bool toDelete = false;
	
	mAirplanesListIt = mAirplanesList.begin();

	if(mLocalAirplane == 0)	
	{
		//try to find the local airplane
		for( ;mAirplanesListIt != mAirplanesListEnd; mAirplanesListIt++)
		{
			plane = *mAirplanesListIt;
			if(plane->isDestructed())
			{
				toDelete = true;
				deletedIt = mAirplanesListIt;
			}
			else
			{				
				if(mIsServer && plane->isDead() && plane->decreaseTimeToBornAgain(timeSinceLastFrame) <= 0)
				{
					Ogre::Vector3 pos;
					Ogre::Quaternion ori;		

					mAirplaneMarkersManager->getMarkerPositionOrientation(pos, ori);
					plane->born(pos, ori);
				}
				plane->update(timeSinceLastFrame);
				if(plane->getOwner() == mLocalID)
				{
					mLocalAirplane = plane;
					Ogre::LogManager::getSingletonPtr()
						->logMessage(RealToys::logMessagePrefix + "Local Airplane assigned" );
					break;
				}
			}
		}
	}

	// Since the last cycle stopped when it found the local airplane, process the rest of the airplanes
	for( ; mAirplanesListIt != mAirplanesListEnd; mAirplanesListIt++)
	{
		plane = *mAirplanesListIt;
		if(plane->isDestructed())
		{
			toDelete = true;
			deletedIt = mAirplanesListIt;
		}		
		else
		{
			if(mIsServer && plane->isDead() && plane->decreaseTimeToBornAgain(timeSinceLastFrame) <= 0)
			{
				Ogre::Vector3 pos;
				Ogre::Quaternion ori;		

				mAirplaneMarkersManager->getMarkerPositionOrientation(pos, ori);
				plane->born(pos, ori);
			}
			plane->update(timeSinceLastFrame);
		}
	}

	if(toDelete)
	{
		delete (*deletedIt);
		mAirplanesList.erase(deletedIt);	
		mAirplanesListEnd = mAirplanesList.end();
	}
}

void AirplaneManager::setPlayerID(PlayerID ID)
{
	mLocalID = ID;
}

Airplane* AirplaneManager::getLocalAirplane()
{
	return mLocalAirplane;
}
void AirplaneManager::applyInput(PlayerID ownerID, InputActions action, bool pressed)
{
	Airplane* plane = findAirplane(ownerID);
	if(plane == 0)
		return;
	switch(action)
	{
	case INPUTACTION_PITCHUP:
		if(pressed)
			plane->addPitchForce(PITCHFORCE_2);	
		else
			plane->addPitchForce(-PITCHFORCE_2);	
		break;
	case INPUTACTION_PITCHDOWN:
		if(pressed)
			plane->addPitchForce(-PITCHFORCE_2);			
		else
			plane->addPitchForce(PITCHFORCE_2);
		break;
	case INPUTACTION_ROLLLEFT:
		if(pressed)
			plane->addRollForce(-ROLLFORCE_2);			
		else
			plane->addRollForce(ROLLFORCE_2);
		break;
	case INPUTACTION_ROLLRIGHT:
		if(pressed)
			plane->addRollForce(ROLLFORCE_2);			
		else
			plane->addRollForce(-ROLLFORCE_2);
		break;
	case INPUTACTION_INCSPEED:
		if(pressed)
			plane->increaseThrust();
		break;
	case INPUTACTION_DECSPEED:
		if(pressed)
			plane->decreaseThrust();
		break;
	case INPUTACTION_SPEED0:
		if(pressed)
			plane->setThrustForce(THRUSTFORCE_0);
		break;
	case INPUTACTION_SPEED1:
		if(pressed)
			plane->setThrustForce(THRUSTFORCE_1);
		break;
	case INPUTACTION_SPEED2:
		if(pressed)
			plane->setThrustForce(THRUSTFORCE_2);
		break;
	case INPUTACTION_SPEED3:	
		if(pressed)
			plane->setThrustForce(THRUSTFORCE_3);
		break;
	case INPUTACTION_SHOT1:
		if(pressed)
			plane->setShot1On(true);
		else
			plane->setShot1On(false);
	default:
		break;
	}
}
Airplane* AirplaneManager::findAirplane(PlayerID ownerID)
{
	for (std::list<Airplane*>::iterator it = mAirplanesList.begin();
		it != mAirplanesList.end(); it++)
	{
		if((*it)->getOwner() == ownerID)
			return (*it);
	}
	return NULL;
}

void AirplaneManager::destroyAirplane(PlayerID ownerID)
{
	for (std::list<Airplane*>::iterator it = mAirplanesList.begin();
		it != mAirplanesList.end(); it++)
	{
		if((*it)->getOwner() == ownerID)
		{
			Airplane* plane = (*it);
			mAirplanesList.erase(it);
			delete plane;
			mAirplanesListEnd = mAirplanesList.end();
			return;
		}
	}
}