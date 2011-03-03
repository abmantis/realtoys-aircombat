#ifndef _AIRPLANEMANAGER_H_
#define _AIRPLANEMANAGER_H_

#include "NewtonContactCallback.h"
#include "Airplane.h"
#include "AirplaneMarkersManager.h"

enum InputActions
{
	INPUTACTION_PITCHUP,
	INPUTACTION_PITCHDOWN,
	INPUTACTION_ROLLLEFT,
	INPUTACTION_ROLLRIGHT,
	INPUTACTION_INCSPEED,
	INPUTACTION_DECSPEED,
	INPUTACTION_SPEED0,
	INPUTACTION_SPEED1,
	INPUTACTION_SPEED2,
	INPUTACTION_SPEED3,
	INPUTACTION_SHOT1
};

class AirplaneManager : Ogre::Singleton<AirplaneManager>
{
public:	
	AirplaneManager(bool server, Ogre::SceneManager *sceneMgr, OgreNewt::World *world);
	~AirplaneManager(void);

	Airplane* createAirplane(PlayerID ownerID);

	void destroyAirplane(PlayerID ownerID);

	/*
		Updates all airplanes; Should only be called by clients! In server newton updates airplanes
	*/
	void updateAirplanes(Ogre::Real timeSinceLastFrame);

	/*
		Defines this local player ID, used to identify the players/airplanes
	*/
	void setPlayerID(PlayerID ID);
	
	/*
		Get the airplane owned by "this" local player. 
		Returns NULL if this player has not airplane yet.
	*/
	Airplane* getLocalAirplane();

	void applyInput(PlayerID ownerID, InputActions action, bool pressed);

	Airplane* findAirplane(PlayerID ownerID);

	bool canHaveMoreAirplanes();
	
private:	
	Ogre::SceneManager *mSceneMgr;
	OgreNewt::World *mWorld;
	bool mIsServer;

	AirplaneMarkersManager *mAirplaneMarkersManager;

	const OgreNewt::MaterialID* mMatAirplane;
	

	std::list<Airplane*> mAirplanesList;
	std::list<Airplane*>::iterator mAirplanesListIt;
	std::list<Airplane*>::iterator mAirplanesListEnd;

	PlayerID mLocalID;
	Airplane * mLocalAirplane;

	void createMaterials();
	
public:
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
	static AirplaneManager& getSingleton(void);
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
	static AirplaneManager* getSingletonPtr(void);
};

#endif