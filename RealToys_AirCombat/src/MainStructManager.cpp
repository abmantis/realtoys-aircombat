#include "StdAfx.h"
#include "MainStructManager.h"


template<> MainStructManager* Ogre::Singleton<MainStructManager>::ms_Singleton = 0;
MainStructManager& MainStructManager::getSingleton(void)
{
	assert( ms_Singleton );  return ( *ms_Singleton );  
}
MainStructManager* MainStructManager::getSingletonPtr(void)
{
	return ms_Singleton;
}
MainStructManager::MainStructManager(Ogre::SceneManager *sceneManager, OgreNewt::World* newtWorld) : 
	mSceneMgr(sceneManager), mWorld(newtWorld)
{	
	withTextures = false;

	Ogre::LogManager::getSingletonPtr()
		->logMessage(RealToys::logMessagePrefix + "Setting up main struct base");

	int basePlaneSize = 100;		//tamanho base (sem scale) de cada plano (excepto altura das paredes), em cm
	int numPlaneSegments = 10;

	mWorld->setWorldSize(RealToys::ToNewton(Ogre::Vector3(-basePlaneSize,-10,-basePlaneSize)),
		RealToys::ToNewton(Ogre::Vector3(basePlaneSize, RealToys::wallHeight+10, basePlaneSize)));


	// Create a material for the plane (just a simple texture, here grass.jpg)
	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().create("FloorMat", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

	//Material do tecto
	mat = Ogre::MaterialManager::getSingleton().create("CeilingMat", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

		
	// Create materials for the walls couple 1
	mat = Ogre::MaterialManager::getSingleton().create("WallMat1-1", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	mat = Ogre::MaterialManager::getSingleton().create("WallMat1-2", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

	// Create materials for the walls couple 2
	mat = Ogre::MaterialManager::getSingleton().create("WallMat2-1", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	mat = Ogre::MaterialManager::getSingleton().create("WallMat2-2", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);



	//chao e tecto
	Ogre::MovablePlane *plane = new Ogre::MovablePlane("FloorPlane");
	plane->d = 0;	
	plane->normal = Ogre::Vector3::UNIT_Y;	
	Ogre::MeshManager::getSingleton().createPlane("FloorCeilingPlaneMesh", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
		*plane, basePlaneSize, basePlaneSize, numPlaneSegments, numPlaneSegments, true, 1, 1, 1, Ogre::Vector3::UNIT_Z);

	
	//paredes
	plane = new Ogre::MovablePlane("WallPlane");
	plane->d = 0;	
	plane->normal = Ogre::Vector3::UNIT_Z;
	Ogre::MeshManager::getSingleton().createPlane("WallPlaneMesh", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
		*plane, basePlaneSize, RealToys::wallHeight, numPlaneSegments, numPlaneSegments*0.5f, true, 1, 1, 1, Ogre::Vector3::UNIT_Y);
	


	
	//Criar SceneNode principal
	Ogre::SceneNode *mPlaneNode = mSceneMgr->getRootSceneNode()
		->createChildSceneNode(RealToys::mainStructMasterSceneNode);
	

	//Chao
	Ogre::Entity *ent = mSceneMgr->createEntity("FloorEnt", "FloorCeilingPlaneMesh");	
	ent->setMaterialName("FloorMat");
	ent->setQueryFlags(RealToys::MASK_MAINFLOOR);
	
	Ogre::SceneNode *node = mPlaneNode->createChildSceneNode("FloorNode");
	node->attachObject(ent);
	
	RealToys::ToNewton(node);
	OgreNewt::CollisionPtr col( new OgreNewt::CollisionPrimitives::TreeCollision(mWorld, ent, true, 0) );
	RealToys::FromNewton(node);

	mFloorBody = new OgreNewt::Body( mWorld, col );
	mFloorBody->attachNode(node);	
	mFloorBody->setType(RealToys::BODYTYPE_MAINSTRUCT);
	mFloorBody->setOgreUpdateScaleFactor(RealToys::OgreNewtonFactor);
	

	//Tecto
	ent = mSceneMgr->createEntity("CeilingEnt", "FloorCeilingPlaneMesh");
	ent->setMaterialName("CeilingMat");
	ent->setQueryFlags(RealToys::MASK_MAINCEILING);

	node = mPlaneNode->createChildSceneNode("CeilingNode");
	node->attachObject(ent);

	Ogre::Vector3 position = Ogre::Vector3(0, RealToys::wallHeight, 0);
	Ogre::Quaternion orientation = Ogre::Quaternion(Ogre::Degree(180), Ogre::Vector3(1,0,0));
		
	mCeilingBody = new OgreNewt::Body( mWorld, col );
	mCeilingBody->attachNode(node);
	mCeilingBody->setPositionOrientation(RealToys::ToNewton(position), RealToys::ToNewton(orientation));
	mCeilingBody->setType(RealToys::BODYTYPE_MAINSTRUCT);
	mCeilingBody->setOgreUpdateScaleFactor(RealToys::OgreNewtonFactor);

	
	
	ent = mSceneMgr->createEntity("MainWall1_Ent", "WallPlaneMesh");
	ent->setMaterialName("WallMat1-1");
	ent->setQueryFlags(RealToys::MASK_MAINWALL);	

	node = mPlaneNode->createChildSceneNode("MainWall1_Node");
	node->attachObject(ent);

	position = Ogre::Vector3(basePlaneSize*0.5f, RealToys::wallHeight*0.5f, 0.0f);
	orientation = Ogre::Quaternion(Ogre::Degree(-90), Ogre::Vector3(0,1,0));
	
	RealToys::ToNewton(node);
	col = OgreNewt::CollisionPtr(new OgreNewt::CollisionPrimitives::TreeCollision(mWorld, ent, true, 0));
	RealToys::FromNewton(node);

	mWall1Body = new OgreNewt::Body( mWorld, col );
	mWall1Body->attachNode(node);
	mWall1Body->setPositionOrientation(RealToys::ToNewton(position), RealToys::ToNewton(orientation));
	mWall1Body->setType(RealToys::BODYTYPE_MAINSTRUCT);
	mWall1Body->setOgreUpdateScaleFactor(RealToys::OgreNewtonFactor);

	
	ent = mSceneMgr->createEntity("MainWall2_Ent", "WallPlaneMesh");
	ent->setMaterialName("WallMat1-2");
	ent->setQueryFlags(RealToys::MASK_MAINWALL);
	

	node = mPlaneNode->createChildSceneNode("MainWall2_Node");
	node->attachObject(ent);
	node->setInheritScale(false);

	position = Ogre::Vector3(-basePlaneSize*0.5f, RealToys::wallHeight*0.5f, 0.0f);
	orientation = Ogre::Quaternion(Ogre::Degree(90), Ogre::Vector3(0,1,0));
	
	mWall2Body = new OgreNewt::Body( mWorld, col );
	mWall2Body->attachNode(node);
	mWall2Body->setPositionOrientation(RealToys::ToNewton(position), RealToys::ToNewton(orientation));
	mWall2Body->setType(RealToys::BODYTYPE_MAINSTRUCT);
	mWall2Body->setOgreUpdateScaleFactor(RealToys::OgreNewtonFactor);


	ent = mSceneMgr->createEntity("MainWall3_Ent", "WallPlaneMesh");
	ent->setMaterialName("WallMat2-1");
	ent->setQueryFlags(RealToys::MASK_MAINWALL);
	

	node = mPlaneNode->createChildSceneNode("MainWall3_Node");
	node->attachObject(ent);
	node->setInheritScale(false);

	position = Ogre::Vector3(0.0f, RealToys::wallHeight*0.5f, basePlaneSize*0.5f);
	orientation = Ogre::Quaternion(Ogre::Degree(180), Ogre::Vector3(0,1,0));
	
	
	mWall3Body = new OgreNewt::Body( mWorld, col );
	mWall3Body->attachNode(node);
	mWall3Body->setPositionOrientation(RealToys::ToNewton(position), RealToys::ToNewton(orientation));
	mWall3Body->setType(RealToys::BODYTYPE_MAINSTRUCT);
	mWall3Body->setOgreUpdateScaleFactor(RealToys::OgreNewtonFactor);
		

	ent = mSceneMgr->createEntity("MainWall4_Ent", "WallPlaneMesh");
	ent->setMaterialName("WallMat2-2");
	ent->setQueryFlags(RealToys::MASK_MAINWALL);


	node = mPlaneNode->createChildSceneNode("MainWall4_Node");
	node->attachObject(ent);
	node->setInheritScale(false);

	position = Ogre::Vector3(0.0f, RealToys::wallHeight*0.5f, -basePlaneSize*0.5f);
	orientation = Ogre::Quaternion::IDENTITY;
		
	mWall4Body = new OgreNewt::Body( mWorld, col );
	mWall4Body->attachNode(node);
	mWall4Body->setPositionOrientation(RealToys::ToNewton(position), RealToys::ToNewton(orientation));
	mWall4Body->setType(RealToys::BODYTYPE_MAINSTRUCT);
	mWall4Body->setOgreUpdateScaleFactor(RealToys::OgreNewtonFactor);




	Ogre::LogManager::getSingletonPtr()
		->logMessage(RealToys::logMessagePrefix + "Done setting up main struct base");
}

MainStructManager::~MainStructManager(void)
{
}

void MainStructManager::SetSize(Ogre::Vector2 size)
{
	Ogre::Real width, length;
	length = size.x;
	width = size.y;

	mSize.x = length;
	mSize.z = width;
	mSize.y = RealToys::wallHeight*0.01f;

	mWorld->setWorldSize(RealToys::ToNewton(Ogre::Vector3(-length*100, -100, -width*100)), 
		RealToys::ToNewton(Ogre::Vector3(length*100, RealToys::wallHeight+100, width*100)));

	Ogre::SceneNode *node;

	node = mSceneMgr->getSceneNode("FloorNode");
	node->setScale(Ogre::Vector3(length, 1, width));

	RealToys::ToNewton(node);
	OgreNewt::CollisionPtr col(new OgreNewt::CollisionPrimitives::TreeCollision(mWorld, (Ogre::Entity*)node->getAttachedObject(0), true, 0));
	RealToys::FromNewton(node);
	mFloorBody->setCollision(col);

	node = mSceneMgr->getSceneNode("CeilingNode");
	node->setScale(Ogre::Vector3(length, 1, width));		
	mCeilingBody->setCollision(col);
	
	
	Ogre::Vector3 pos;
	Ogre::Quaternion ori;

	node = mSceneMgr->getSceneNode("MainWall1_Node");
	node->setScale(Ogre::Vector3(width,1,1));

	node = mSceneMgr->getSceneNode("MainWall2_Node");
	node->setScale(Ogre::Vector3(width,1,1));

	RealToys::ToNewton(node);
	col = OgreNewt::CollisionPtr(new OgreNewt::CollisionPrimitives::TreeCollision(mWorld, (Ogre::Entity*)node->getAttachedObject(0), true, 0));
	RealToys::FromNewton(node);

	mWall1Body->setCollision(col);
	mWall1Body->getPositionOrientation(pos, ori);
	pos = Ogre::Vector3(length*100*0.5, RealToys::wallHeight*0.5, 0);
	mWall1Body->setPositionOrientation(RealToys::ToNewton(pos), ori);
	
	mWall2Body->setCollision(col);
	mWall2Body->getPositionOrientation(pos, ori);
	pos = Ogre::Vector3(-length*100*0.5, RealToys::wallHeight*0.5, 0);
	mWall2Body->setPositionOrientation(RealToys::ToNewton(pos),ori);
	
	

	node = mSceneMgr->getSceneNode("MainWall3_Node");
	node->setScale(Ogre::Vector3(length,1,1));
	
	node = mSceneMgr->getSceneNode("MainWall4_Node");
	node->setScale(Ogre::Vector3(length,1,1));

	RealToys::ToNewton(node);
	col = OgreNewt::CollisionPtr(new OgreNewt::CollisionPrimitives::TreeCollision(mWorld, (Ogre::Entity*)node->getAttachedObject(0), true, 0));
	RealToys::FromNewton(node);

	mWall3Body->setCollision(col);
	mWall3Body->getPositionOrientation(pos, ori);
	pos = Ogre::Vector3(0, RealToys::wallHeight*0.5, width*100*0.5);
	mWall3Body->setPositionOrientation(RealToys::ToNewton(pos),ori);

	mWall4Body->setCollision(col);
	mWall4Body->getPositionOrientation(pos, ori);
	pos = Ogre::Vector3(0, RealToys::wallHeight*0.5, -width*100*0.5);
	mWall4Body->setPositionOrientation(RealToys::ToNewton(pos),ori);


	

	Ogre::MaterialPtr material = Ogre::MaterialPtr(Ogre::MaterialManager::getSingletonPtr()->getByName("FloorMat"));
	material->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTextureScale( (1.0f/length) , (1.0f/width));

	material = Ogre::MaterialPtr(Ogre::MaterialManager::getSingletonPtr()->getByName("CeilingMat"));
	material->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTextureScale( (1.0f/length) , (1.0f/width));

	material = Ogre::MaterialPtr(Ogre::MaterialManager::getSingletonPtr()->getByName("WallMat1-1"));
	material->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTextureScale( (1.0f/width) , 1.0f);
	material = Ogre::MaterialPtr(Ogre::MaterialManager::getSingletonPtr()->getByName("WallMat1-2"));
	material->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTextureScale( (1.0f/width) , 1.0f);

	material = Ogre::MaterialPtr(Ogre::MaterialManager::getSingletonPtr()->getByName("WallMat2-1"));
	material->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTextureScale( (1.0f/length) , 1.0f);
	material = Ogre::MaterialPtr(Ogre::MaterialManager::getSingletonPtr()->getByName("WallMat2-2"));
	material->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTextureScale( (1.0f/length) , 1.0f);
}

void MainStructManager::SetTextures(Ogre::String floorTex, Ogre::String ceilingTex, Ogre::String wall1Tex, 
		Ogre::String wall2Tex, Ogre::String wall3Tex, Ogre::String wall4Tex )
{	
	Ogre::MaterialPtr material;
	Ogre::Pass *pass;

	material = Ogre::MaterialPtr(Ogre::MaterialManager::getSingletonPtr()->getByName("FloorMat"));
	pass = material->getTechnique(0)->getPass(0);
	pass->removeAllTextureUnitStates();
	pass->createTextureUnitState(floorTex);


	material = Ogre::MaterialPtr(Ogre::MaterialManager::getSingletonPtr()->getByName("CeilingMat"));
	pass = material->getTechnique(0)->getPass(0);
	pass->removeAllTextureUnitStates();
	pass->createTextureUnitState(ceilingTex);


	material = Ogre::MaterialPtr(Ogre::MaterialManager::getSingletonPtr()->getByName("WallMat1-1"));
	pass = material->getTechnique(0)->getPass(0);
	pass->removeAllTextureUnitStates();
	pass->createTextureUnitState(wall1Tex);
	
	material = Ogre::MaterialPtr(Ogre::MaterialManager::getSingletonPtr()->getByName("WallMat1-2"));
	pass = material->getTechnique(0)->getPass(0);
	pass->removeAllTextureUnitStates();
	pass->createTextureUnitState(wall2Tex);

	material = Ogre::MaterialPtr(Ogre::MaterialManager::getSingletonPtr()->getByName("WallMat2-1"));
	pass = material->getTechnique(0)->getPass(0);
	pass->removeAllTextureUnitStates();
	pass->createTextureUnitState(wall3Tex);

	material = Ogre::MaterialPtr(Ogre::MaterialManager::getSingletonPtr()->getByName("WallMat2-2"));
	pass = material->getTechnique(0)->getPass(0);
	pass->removeAllTextureUnitStates();
	pass->createTextureUnitState(wall4Tex);
}