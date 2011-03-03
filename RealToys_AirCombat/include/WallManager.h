#ifndef WALLMANAGER_H
#define WALLMANAGER_H

#include "RealToysShared.h"


class WallManager: public Ogre::Singleton<WallManager>
{
public:	
	WallManager(Ogre::SceneManager * sceneManager, OgreNewt::World* newtWorld);
	~WallManager(void);

	/*
		Creates a new Entity holding a wall, based on it's textures and scenenode caracteristics.
		@remarks - this is usefull when loading from a pre-saved scene
	*/
	Ogre::SceneNode * addWall(Ogre::String texture1, Ogre::String texture2, Ogre::String texture3, 
		Ogre::String texture4, Ogre::Vector3 position, Ogre::Vector3 scale, Ogre::Quaternion orientation);

	/*
		Removes a wall and it's parent node.
		@param wall - Ogre::Entity pointer to the wall to be removed
	*/
	void removeWall(Ogre::Entity * wall);

	/*
		Paints a wall.
		@param wall - Ogre::Entity pointer to the wall to be removed.
		@textureName - The name of the texture to apply.
		@subEntityNumber - Index of the subentity (face of the wall) to paint.
	*/
	void paintWall(Ogre::Entity * wall, Ogre::String textureName, int subEntityNumber);

	void clearAll();


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
	static WallManager& getSingleton(void);
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
	static WallManager* getSingletonPtr(void);

private:
	Ogre::SceneManager * mSceneManager;
	OgreNewt::World* mWorld;
	Ogre::SceneNode * mMasterNode;		//node that will parent all wall nodes
	long mNextIndex;				//next index to be used to create an wall Entity
	std::list<long> mRemovedList;	//list to hold the removed wall's indices

	static const Ogre::String mEntityPrefix;
	static const Ogre::String mNodePrefix;
	static const Ogre::String mMaterialPrefix;

};
#endif