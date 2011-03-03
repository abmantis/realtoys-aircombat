#include "StdAfx.h"

#include "WallManager.h"

const Ogre::String WallManager::mEntityPrefix	= "wallEntNr";
const Ogre::String WallManager::mNodePrefix		= "wallNodeNr";
const Ogre::String WallManager::mMaterialPrefix = "wallMat";

template<> WallManager* Ogre::Singleton<WallManager>::ms_Singleton = 0;

WallManager::WallManager(Ogre::SceneManager * sceneManager, OgreNewt::World* newtWorld)
	: mSceneManager(sceneManager), mWorld(newtWorld)
{
	mNextIndex = 0;
	//create node that walls stuff	
	mMasterNode = mSceneManager->getRootSceneNode()
		->createChildSceneNode(RealToys::wallsMasterSceneNode);
}

WallManager::~WallManager(void)
{
}
WallManager& WallManager::getSingleton(void)
{
	assert( ms_Singleton );  return ( *ms_Singleton );  
}
WallManager* WallManager::getSingletonPtr(void)
{
	return ms_Singleton;
}

Ogre::SceneNode * WallManager::addWall(Ogre::String texture1, Ogre::String texture2, Ogre::String texture3, 
		Ogre::String texture4, Ogre::Vector3 position, Ogre::Vector3 scale, Ogre::Quaternion orientation)
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

	/* 	Create the Entity and parent node 	*/
	Ogre::String wallN = mEntityPrefix + number;
	Ogre::Entity * ent = mSceneManager->createEntity(wallN, RealToys::unitCubeMeshName);
	ent->setQueryFlags(RealToys::MASK_WALL);	
	ent->setUserAny(Ogre::Any(index));

	Ogre::SceneNode * node = mMasterNode->createChildSceneNode(mNodePrefix + number);
	node->attachObject(ent);
	node->setScale(scale );					


	
	
	OgreNewt::CollisionPtr col( new OgreNewt::CollisionPrimitives::TreeCollision(mWorld, ent, true, 0) );
	OgreNewt::Body* wallBody = new OgreNewt::Body( mWorld, col );
	wallBody->attachNode(node);
	wallBody->setPositionOrientation(position, orientation);
	wallBody->setType(RealToys::BODYTYPE_WALL);

	node->setUserAny(Ogre::Any(wallBody));
	
	
	/*	Create four Materials from texture and apply it to the four sides of the wall	*/
	
	Ogre::MaterialPtr mat; 	
	//ent->getSubEntity(0)->setMaterial(mat);		//top
	//ent->getSubEntity(1)->setMaterial(mat);		//bottom

	/*	left	*/
	mat = Ogre::MaterialManager::getSingleton().create(mMaterialPrefix + Ogre::String("Left") + number, 
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	mat->getTechnique(0)->getPass(0)->createTextureUnitState(texture1)
		->setTextureScale( (1.0f/scale.z*100) , 1.0);
	mat->setDiffuse(Ogre::ColourValue::White);
	ent->getSubEntity(2)->setMaterial(mat);		

	/*	right	*/
	mat = Ogre::MaterialManager::getSingleton().create(mMaterialPrefix + Ogre::String("Right") + number, 
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	mat->getTechnique(0)->getPass(0)->createTextureUnitState(texture2)
		->setTextureScale( (1.0f/scale.z*100) , 1.0);
	ent->getSubEntity(3)->setMaterial(mat);		//right

	/*	front	*/
	mat = Ogre::MaterialManager::getSingleton().create(mMaterialPrefix + Ogre::String("Front") + number, 
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	mat->getTechnique(0)->getPass(0)->createTextureUnitState(texture3)
		->setTextureScale( (1.0f/scale.x*100) , 1.0);
	mat->setDiffuse(Ogre::ColourValue::White);
	ent->getSubEntity(4)->setMaterial(mat);		//front

	/*	back	*/
	mat = Ogre::MaterialManager::getSingleton().create(mMaterialPrefix + Ogre::String("Back") + number, 
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	mat->getTechnique(0)->getPass(0)->createTextureUnitState(texture4)
		->setTextureScale( (1.0f/scale.x*100) , 1.0);
	mat->setDiffuse(Ogre::ColourValue::White);
	ent->getSubEntity(5)->setMaterial(mat);		//back	


	return node;
}

void WallManager::removeWall(Ogre::Entity *wall)
{
	//make sure it's a wall
	if(wall->getQueryFlags() != RealToys::MASK_WALL)
		return;

	long remValue = Ogre::any_cast<long>(wall->getUserAny());
	Ogre::String valueStr = Ogre::StringConverter::toString(remValue);

	mRemovedList.push_back(remValue);	
	mRemovedList.sort();

	delete Ogre::any_cast<OgreNewt::Body*>(wall->getParentSceneNode()->getUserAny());
	mMasterNode->removeAndDestroyChild(mNodePrefix + valueStr);
	mSceneManager->destroyEntity(wall);
	
}

void WallManager::paintWall(Ogre::Entity *wall, Ogre::String textureName, int subEntityNumber)
{
	//make sure it's a wall and the subEntityNumber is valid
	if(	wall->getQueryFlags() == RealToys::MASK_WALL )
	{
		if(subEntityNumber < 2 || subEntityNumber > 5)
			return;
	}
	else
	{
		if( wall->getQueryFlags() == RealToys::MASK_MAINWALL )
		{
			if(subEntityNumber != 0)
				return;
		}
		
	}


	//get the material from the subentity
	Ogre::MaterialPtr mat = wall->getSubEntity(subEntityNumber)->getMaterial();
	
	//remove previous texture units and create the new one
	Ogre::TextureUnitState * texUnit = mat->getTechnique(0)->getPass(0)->getTextureUnitState(0);
	texUnit->setTextureName(textureName);

}
void WallManager::clearAll()
{
	Ogre::Entity *ent;
	Ogre::SceneNode *node;
	Ogre::SceneNode::ChildNodeIterator childIt= mMasterNode->getChildIterator();

	while(childIt.hasMoreElements())
	{
		node = static_cast<Ogre::SceneNode*>(childIt.getNext());
		ent = static_cast<Ogre::Entity*>(node->getAttachedObject(0));
		removeWall(ent);		
	}
	mRemovedList.clear();
	mNextIndex = 0;
}