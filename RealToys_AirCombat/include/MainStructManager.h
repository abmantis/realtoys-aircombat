#ifndef MAINSTRUCTMANAGER_H
#define MAINSTRUCTMANAGER_H
#include "RealToysShared.h"

class MainStructManager: public Ogre::Singleton<MainStructManager>
{
public:
	MainStructManager(Ogre::SceneManager *sceneManager, OgreNewt::World* newtWorld);
	~MainStructManager(void);


	/*
		Sets size IN METERS!
		WARNING: this should be called only after texture units have been created in each main struct material
	*/
	void SetSize(Ogre::Vector2 size);
	
	/*
		Current size IN METERS!
	*/
	const Ogre::Vector3 GetSize() { return mSize; }

	void SetTextures(Ogre::String floorTex, Ogre::String ceilingTex, Ogre::String wall1Tex, 
		Ogre::String wall2Tex, Ogre::String wall3Tex, Ogre::String wall4Tex );

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
	static MainStructManager& getSingleton(void);
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
	static MainStructManager* getSingletonPtr(void);

private:
	Ogre::SceneManager *mSceneMgr;
	OgreNewt::World* mWorld;

	bool withTextures;
	Ogre::Vector3 mSize;

	OgreNewt::Body *mFloorBody;
	OgreNewt::Body *mCeilingBody;
	OgreNewt::Body *mWall1Body;
	OgreNewt::Body *mWall2Body;
	OgreNewt::Body *mWall3Body;
	OgreNewt::Body *mWall4Body;
};

#endif