#include "StdAfx.h"
#include "ModelsManager.h"
#if OGRE_PLATFORM == PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#include <direct.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#endif


const Ogre::String ModelsManager::mEntityPrefix			= "modelEntNr";
const Ogre::String ModelsManager::mNodePrefix			= "modelNodeNr";

template<> ModelsManager* Ogre::Singleton<ModelsManager>::ms_Singleton = 0;


ModelsManager::ModelsManager(Ogre::SceneManager * sceneManager, OgreNewt::World* newtWorld)
	: mSceneManager(sceneManager), mWorld(newtWorld)
{
	mNextIndex = 0;

	mMasterNode = mSceneManager->getRootSceneNode()
		->createChildSceneNode(RealToys::modelsMasterSceneNode);

	Ogre::ConfigFile configFile;
	configFile.load(RealToys::configFileName);
	mCollisionsLocation = configFile.getSetting("MapsPath", "Misc", "../../saved");
	mCollisionsLocation += "/colls/";

#if OGRE_PLATFORM == PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	mkdir( mCollisionsLocation.c_str());	
#else
	mkdir( mCollisionsLocation.c_str(),0666);	
#endif
}

ModelsManager::~ModelsManager(void)
{
}
ModelsManager& ModelsManager::getSingleton(void)
{
	assert( ms_Singleton );  return ( *ms_Singleton );  
}
ModelsManager* ModelsManager::getSingletonPtr(void)
{
	return ms_Singleton;
}

Ogre::SceneNode* ModelsManager::addModel(Ogre::String mesh, Ogre::Vector3 adjustNodePosition, Ogre::Vector3 adjustNodeScale,
		Ogre::Quaternion adjustNodeOrientation, Ogre::Vector3 editNodePosition, Ogre::Vector3 editNodeScale,
		Ogre::Quaternion editNodeOrientation, Ogre::String mapName )
{
	long index;	
	if(mRemovedList.empty())
	{
		index = mNextIndex;
		mNextIndex++;		//increment index
	}
	else
	{
		std::list<long>::iterator it = mRemovedList.begin();
		index = (*it);
		mRemovedList.pop_front();
	}

	Ogre::String number = Ogre::StringConverter::toString(index);


	/* 	Create the Entity	*/
	Ogre::String entityName = mEntityPrefix + number;
	Ogre::Entity * ent = mSceneManager->createEntity(entityName, mesh);
	ent->setQueryFlags(RealToys::MASK_DECOMODEL);	
	ent->setUserAny(Ogre::Any(index));
	ent->setCastShadows(false);

	
	Ogre::Vector3 position = editNodePosition + editNodeOrientation * adjustNodePosition * editNodeScale ;
	Ogre::Quaternion orientation = editNodeOrientation * adjustNodeOrientation;
	//create scene node
	Ogre::SceneNode * node = mMasterNode->createChildSceneNode(mNodePrefix + number );
	node->attachObject(ent);
	node->setScale(editNodeScale * adjustNodeScale);	


	/*	create Newton stuff	*/
	OgreNewt::CollisionPtr col;
	Ogre::String collisionFileName = mesh + '_' + mapName + "_.collision";
	OgreNewt::CollisionSerializer colSer;
	//try to load the collision from file
	try
	{
		Ogre::DataStreamPtr dsptr = Ogre::ResourceGroupManager::getSingletonPtr()->openResource(collisionFileName);		
		col = colSer.importCollision(dsptr, mWorld);
	}
	catch (Ogre::Exception e)
	{
		//if it fails to open, creat it and save it
		Ogre::LogManager::getSingletonPtr()->logMessage(RealToys::logMessagePrefix + mesh + " collision not found, creating");
		col = OgreNewt::CollisionPtr( new OgreNewt::CollisionPrimitives::TreeCollision(mWorld, ent, false, 0) );
		colSer.exportCollision(col, mCollisionsLocation+collisionFileName);
	}	

	OgreNewt::Body* modelBody = new OgreNewt::Body( mWorld, col );
	modelBody->attachNode(node);
	modelBody->setPositionOrientation(position, orientation);
	modelBody->setType(RealToys::BODYTYPE_DECOMODEL);
	node->setUserAny(Ogre::Any(modelBody));

	//col = OgreNewt::CollisionPtr( new OgreNewt::CollisionPrimitives::TreeCollision(mWorld, ent, false, OgreNewt::CollisionPrimitives::FW_REVERSE) );
	//modelBody = new OgreNewt::Body( mWorld, col );
	//modelBody->attachNode(node);
	//modelBody->setPositionOrientation(position, orientation);
	//ent->setUserAny(Ogre::Any(modelBody));
	
	return node;
}

void ModelsManager::removeModel(Ogre::Entity *modelEntity)
{
	//make sure it's a model
	if(modelEntity == NULL || modelEntity->getQueryFlags() != RealToys::MASK_DECOMODEL)
		return;

	long remValue = Ogre::any_cast<long>(modelEntity->getUserAny());
	Ogre::String valueStr = Ogre::StringConverter::toString(remValue);

	mRemovedList.push_back(remValue);	
	mRemovedList.sort();
	
	//delete Ogre::any_cast<OgreNewt::Body*>(modelEntity->getUserAny());
	delete Ogre::any_cast<OgreNewt::Body*>(modelEntity->getParentSceneNode()->getUserAny());
	
	mMasterNode->removeAndDestroyChild(mNodePrefix + valueStr);	
	mSceneManager->destroyEntity(modelEntity);		
}

void ModelsManager::clearAll()
{
	Ogre::Entity *ent;
	Ogre::SceneNode *node;
	Ogre::SceneNode::ChildNodeIterator childIt= mMasterNode->getChildIterator();

	while(childIt.hasMoreElements())
	{
		node = static_cast<Ogre::SceneNode*>(childIt.getNext());	//obtem o nó de "ajuste"
		ent = static_cast<Ogre::Entity*>(node->getAttachedObject(0));
		removeModel(ent);		
	}
	mRemovedList.clear();
	mNextIndex = 0;
}
