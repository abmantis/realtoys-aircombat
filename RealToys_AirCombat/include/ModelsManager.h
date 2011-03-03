#ifndef MODELS_MANAGER_H
#define MODELS_MANAGER_H

#include "RealToysShared.h"


class ModelsManager: public Ogre::Singleton<ModelsManager>
{
public:
	ModelsManager(Ogre::SceneManager * sceneManager, OgreNewt::World* newtWorld);
	~ModelsManager(void);

	/*
		Creates a new Entity holding a model (mesh) and adds it to the scene, using the mesh name and scene node data.
		This is usefull when loading a pre-saved scene.
		@remarks - the adjustNode is the one that is parent to the entity, the editNode is parent to the adjust node
		@returns Ogre::SceneNode pointer that holds another SceneNode (to have center correction, and scale) that holds the Entity.
	*/
	Ogre::SceneNode * addModel(Ogre::String mesh, Ogre::Vector3 adjustNodePosition, Ogre::Vector3 adjustNodeScale,
		Ogre::Quaternion adjustNodeOrientation, Ogre::Vector3 editNodePosition, Ogre::Vector3 editNodeScale,
		Ogre::Quaternion editNodeOrientation, Ogre::String mapName );	


	/*
		Removes a model and it's parent nodes.
		@param modelEntity - Ogre::Entity pointer to the model to be removed
	*/
	void removeModel(Ogre::Entity * modelEntity);

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
	static ModelsManager& getSingleton(void);
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
	static ModelsManager* getSingletonPtr(void);

private:
	Ogre::SceneManager * mSceneManager;
	Ogre::SceneNode	* mMasterNode;				// scenenode parent of all model nodes
	OgreNewt::World* mWorld;

	long mNextIndex;							//next index to be used to create an Entity/Node
	std::list<long> mRemovedList;				//list to hold the removed wall's indices
	
	Ogre::String mCollisionsLocation;

	static const Ogre::String mEntityPrefix;
	static const Ogre::String mNodePrefix;	

};
#endif //MODELS_MANAGER_H
