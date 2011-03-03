#include "StdAfx.h"
#include "NewtonCustomRaycasts.h"
#include "RealToysShared.h"

AirplaneShotRaycast::AirplaneShotRaycast(void) : Raycast()
{
	mCloseInfo.mBody = NULL;
	mCloseInfo.mDistance = -1;
	mCloseInfo.mNormal = Ogre::Vector3::ZERO;
	mCloseInfo.mCollisionID = -1;
}
AirplaneShotRaycast::~AirplaneShotRaycast(void) {}

AirplaneShotRaycast::AirplaneShotRaycast(const OgreNewt::World* world, const Ogre::Vector3& startpt, const Ogre::Vector3& endpt)
: Raycast()
{
	go( world, startpt, endpt );
}
void AirplaneShotRaycast::go(const OgreNewt::World* world, const Ogre::Vector3& startpt, const Ogre::Vector3& endpt)
{
	mCloseInfo.mBody = NULL;
	mCloseInfo.mDistance = -1;
	mCloseInfo.mNormal = Ogre::Vector3::ZERO;
	mCloseInfo.mCollisionID = -1;

	Raycast::go( world, startpt, endpt );	
}
bool AirplaneShotRaycast::userCallback( OgreNewt::Body* body, Ogre::Real distance, const Ogre::Vector3& normal, int collisionID )
{
	//if( distance < mCloseInfo.mDistance)
	//{
		// replace closest info object.
		mCloseInfo.mBody = body;
		mCloseInfo.mDistance = distance;
		mCloseInfo.mNormal = normal;
		mCloseInfo.mCollisionID = collisionID;	
	//}
	return true;
}

bool AirplaneShotRaycast::userPreFilterCallback( OgreNewt::Body* body )
{
	if(body->getType() == RealToys::BODYTYPE_SHOT1)
		return false;
	return true;
}