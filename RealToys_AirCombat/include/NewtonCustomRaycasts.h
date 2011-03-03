#ifndef _NEWTONCUSTOMRAYCASTS_H_
#define _NEWTONCUSTOMRAYCASTS_H_

/*!
	Implementation of raycast used to find target for airplane shots
*/
class AirplaneShotRaycast : public OgreNewt::Raycast
{
public:
	AirplaneShotRaycast(void);

	//! constructor
	/*!
	performs a raycast, then returns the closest hit
	\param world pointer to the OgreNewt::World
	\param startpt starting point of the ray in global space
	\param endpt ending point of the ray in global space	
	*/
	AirplaneShotRaycast( const OgreNewt::World* world, const Ogre::Vector3& startpt, const Ogre::Vector3& endpt);

	~AirplaneShotRaycast(void);

	//! perform a raycast
	/*!
	performs a raycast, then the result can be queried  after creation.
	\param world pointer to the OgreNewt::World
	\param startpt starting point of the ray in global space
	\param endpt ending point of the ray in global space
	*/
	void go( const OgreNewt::World* world, const Ogre::Vector3& startpt, const Ogre::Vector3& endpt);

	//! the all-important custom callback function.
	bool userCallback( OgreNewt::Body* body, Ogre::Real distance, const Ogre::Vector3& normal, int collisionID );
	//! the all-important pre-filter custom callback function.
	bool userPreFilterCallback( OgreNewt::Body* body );
	
	////! get the closest body hit by the ray.
	OgreNewt::BasicRaycast::BasicRaycastInfo getClosestHit() const {return mCloseInfo;}
	
private:
	OgreNewt::BasicRaycast::BasicRaycastInfo mCloseInfo;
};

#endif
