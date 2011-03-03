#ifndef AIRPLANESTARTPOSITIONSMANAGER_H
#define AIRPLANESTARTPOSITIONSMANAGER_H

#include "RealToysShared.h"

class AirplaneMarkersManager: public Ogre::Singleton<AirplaneMarkersManager>
{
public:
	AirplaneMarkersManager(Ogre::SceneManager *sceneManager);
	~AirplaneMarkersManager(void);

	bool addMarker(Ogre::Vector3 position, Ogre::Quaternion orientation);	

	void clearAll();

	bool isValidPosition(Ogre::Vector3 position);

	int getMarkerCount();

	bool getMarkerPositionOrientation(int index, Ogre::Vector3 &position, Ogre::Quaternion &orientation);

	/*
		override to return a random marker that has no airplane on the surroundings
	*/
	bool getMarkerPositionOrientation(Ogre::Vector3 &position, Ogre::Quaternion &orientation);

	bool isMarkerPositionValid(Ogre::Vector3 position);


	/** Override standard Singleton retrieval.
	@remarks
	Why do we do this? Well, it's because the Singleton
	implementation is in a .h file, which means it gets compiled
	into anybody who includes it. This is needed for the
	Singleton template to work, but we actually only want it
	compiled into the implementation of the class based on the
	Singleton, not all of them. If we don't change this, we get
	link errors when trying to use the Singleton-based class from
	an outside dll.
	*/
	static AirplaneMarkersManager& getSingleton(void);
	/** Override standard Singleton retrieval.
	@remarks
	Why do we do this? Well, it's because the Singleton
	implementation is in a .h file, which means it gets compiled
	into anybody who includes it. This is needed for the
	Singleton template to work, but we actually only want it
	compiled into the implementation of the class based on the
	Singleton, not all of them. If we don't change this, we get
	link errors when trying to use the Singleton-based class from
	an outside dll.
	*/
	static AirplaneMarkersManager* getSingletonPtr(void);

private:
	Ogre::SceneManager *mSceneMgr;
	std::vector<std::pair<Ogre::Vector3, Ogre::Quaternion>> mPositionOrientationVector;	
	Ogre::PlaneBoundedVolumeListSceneQuery *mVolQuery;

};
#endif 