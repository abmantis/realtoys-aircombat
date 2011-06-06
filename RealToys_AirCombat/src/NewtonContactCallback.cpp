#include "StdAfx.h"

#include "NewtonContactCallback.h"
#include "RealToysShared.h"
#include "Airplane.h"
#include "Shots.h"


AirplaneContactCallback::AirplaneContactCallback()
{
	mPlaneType = RealToys::BODYTYPE_AIRPLANE;
	mScoresManager = ScoresManager::getSingletonPtr();
}

AirplaneContactCallback::~AirplaneContactCallback(void)
{
}

int AirplaneContactCallback::onAABBOverlap( OgreNewt::Body* body0, OgreNewt::Body* body1, int threadIndex )
{
	return 1;
}
void AirplaneContactCallback::contactsProcess( OgreNewt::ContactJoint &contactJoint, Ogre::Real timeStep, int threadIndex )
{
	
	OgreNewt::Body* body0 = contactJoint.getBody0();
	OgreNewt::Body* body1 = contactJoint.getBody1();
	Airplane *plane = 0;

	OgreNewt::Contact contact = contactJoint.getFirstContact();
	Ogre::Real speed = 0;
	while(contact)
	{
		if(contact.getNormalSpeed() > speed)
			speed = contact.getNormalSpeed();

		contact = contact.getNext();
	}
	speed = Ogre::Math::Abs(speed);

	// each airplane has a speed limit of ~83, so there
	// should not be a colision at more than 2*83
	if(speed > 166)	speed = 166;
						

	//std::cout << "colision: " << timeStep << " - " << speed <<  std::endl;

	if(body0->getType() == mPlaneType)
	{
		plane = Ogre::any_cast<Airplane*>(body0->getUserData());
		plane->setCollisionActions();
		mScoresManager->addDamage(plane->getOwner(), speed*0.15f);
	
	}
	if(body1->getType() == mPlaneType)
	{
		plane = Ogre::any_cast<Airplane*>(body1->getUserData());
		plane->setCollisionActions();
		mScoresManager->addDamage(plane->getOwner(), speed*0.15f);
	}	

	//if(plane)
	//{
	//	//it's a plane!
	//	plane->setCollisionActions();
	//	
	//	////Ogre::Vector3 thedir = belt->getGlobalDir();
	//	//for( OgreNewt::Contact contact = contactJoint.getFirstContact(); contact; contact = contact.getNext() )
 // //	  {
	//	//	
	// //	   contact.rotateTangentDirections( thedir );
 // //		  Ogre::Vector3 contactPos, contactNorm;
 // //		  contact.getPositionAndNormal(contactPos, contactNorm);
 // //		  Ogre::Vector3 objectPos;
 // //		  Ogre::Quaternion objectOri;
 // //		  object->getPositionOrientation(objectPos, objectOri);
 // //		  Ogre::Vector3 objectContactPointVel = object->getVelocity() + (contactPos - objectPos)*object->getOmega();
 // //  		Ogre::Real result_accel = belt->getSpeed() - thedir.dotProduct(objectContactPointVel);
 // //		  result_accel *= 10; // looks nicer
	// //	   contact.setTangentAcceleration( result_accel, 0 );
 // //	  }
	//}	
}

/* Shot1ContactCallback	*/
Shot1ContactCallback::Shot1ContactCallback(Ogre::String namesPrefix)
{
	/* initialize random seed: */
	srand ( time(NULL) );

	mDynObjMan = DynamicObjectsManager::getSingletonPtr();
	mScoresManager = ScoresManager::getSingletonPtr();
	mParticlesManager = ParticlesManager::getSingletonPtr();

	mSoundMgr = OgreOggSound::OgreOggSoundManager::getSingletonPtr();
	if(mShotSound1 = mSoundMgr->createSound(namesPrefix + "shot1hitsound1", "bullethitmetal.wav") )
	{
		mShotSound1->setMaxDistance(300);
		mShotSound1->setReferenceDistance(100);
		mShotSound1->setRolloffFactor(1);
	}
	if(mShotSound2 = mSoundMgr->createSound(namesPrefix + "shot1hitsound2", "bullethitmetal2.wav") )
	{
		mShotSound2->setMaxDistance(300);
		mShotSound2->setReferenceDistance(100);
		mShotSound2->setRolloffFactor(1);
	}
}

Shot1ContactCallback::~Shot1ContactCallback(void)
{	
}

int Shot1ContactCallback::onAABBOverlap( OgreNewt::Body* body0, OgreNewt::Body* body1, int threadIndex )
{
	//check if it's a collision with another shot
	if(body0->getType() == RealToys::BODYTYPE_SHOT1)
	{
		if(body1->getType() == RealToys::BODYTYPE_SHOT1)
			return 0;
		else if(body1->getType() == RealToys::BODYTYPE_AIRPLANE) 
		{
			if(  ( Ogre::any_cast<Airplane*>(body1->getUserData()) )->getOwner() 
				== ( Ogre::any_cast<Shot1*>(body0->getUserData()) )->mOwner  )
				return 0;
		}
	}
	else if(body1->getType() == RealToys::BODYTYPE_SHOT1) 
	{
		if(body0->getType() == RealToys::BODYTYPE_SHOT1)
			return 0;
		else if(body0->getType() == RealToys::BODYTYPE_AIRPLANE) 
		{
			if(  ( Ogre::any_cast<Airplane*>(body0->getUserData()) )->getOwner() 
				== ( Ogre::any_cast<Shot1*>(body1->getUserData()) )->mOwner  )
				return 0;
		}
	}
	
	return 1;
}
void Shot1ContactCallback::contactsProcess( OgreNewt::ContactJoint &contactJoint, Ogre::Real timeStep, int threadIndex )
{	
	//---- not new shots are not ignored... some times collision happens...

	OgreNewt::Body* body0 = contactJoint.getBody0();
	OgreNewt::Body* body1 = contactJoint.getBody1();
	Shot1 *shot = 0;
	Airplane *plane = 0;
	
	if(body0->getType() == RealToys::BODYTYPE_SHOT1)
	{
		shot = Ogre::any_cast<Shot1*>(body0->getUserData());
		
		if(shot->mNew && body1->getType() == RealToys::BODYTYPE_AIRPLANE)
		{
			plane = Ogre::any_cast<Airplane*>(body1->getUserData());
		}
	}
	else if(body1->getType() == RealToys::BODYTYPE_SHOT1)
	{
		shot = Ogre::any_cast<Shot1*>(body1->getUserData());
		
		if(shot->mNew && body0->getType() == RealToys::BODYTYPE_AIRPLANE)
		{
			plane = Ogre::any_cast<Airplane*>(body0->getUserData());
		}
	}	
	
	if(shot)
	{	
		Ogre::Vector3 pos, norm;

		OgreNewt::Contact nextContact = contactJoint.getFirstContact();
		OgreNewt::Contact actualContact = nextContact;


		if(shot->mNew)
		{
			shot->mNew = false;
			mDynObjMan->addToDeleteList(shot);	

			actualContact.getPositionAndNormal(pos,norm);

			mParticlesManager->requestShotHitParticles(pos, norm);
			

			if(plane)
			{			
				OgreOggSound::OgreOggISound *soundToPlay;
				int soundID = rand()%2;

				mScoresManager->addDamage(shot->mOwner, plane->getOwner(), shot->mDamage);
				switch (soundID)
				{
				case 0:
					soundToPlay = mShotSound1;
					break;
				default:
					soundToPlay = mShotSound2;
					break;
				}
				if(soundToPlay->isPlaying())
				{
					soundToPlay->stop();
				}
					
				soundToPlay->setPosition(pos);
				soundToPlay->play();				
			}		
		}
		while(actualContact)
		{
			nextContact = nextContact.getNext();
			actualContact.remove();
			actualContact = nextContact;
		}
	}	
}