#include "StdAfx.h"

#include "Airplane.h"
#include "RealToysShared.h"
#include "NewtonMaterialManager.h"
#include "ScoresManager.h"

int vertFromSubMesh(Ogre::SubMesh *subMesh, Ogre::Vector3 scale, Ogre::Vector3 *&verts)
{
	Ogre::SubMesh* sub_mesh = subMesh;
	Ogre::Mesh * mesh = sub_mesh->parent;

	size_t total_verts = 0;

	Ogre::VertexData* v_data;
	bool addedShared = false;

	if (sub_mesh->useSharedVertices)
	{

		v_data = mesh->sharedVertexData;
		total_verts = v_data->vertexCount;
		addedShared = true;
	}
	else
	{
		v_data = sub_mesh->vertexData;
		total_verts = v_data->vertexCount;
	}

	//make array to hold vertex positions!
	Ogre::Vector3* vertices = new Ogre::Vector3[total_verts];
	unsigned int offset = 0;


	Ogre::VertexDeclaration* v_decl;
	const Ogre::VertexElement* p_elem;
	float* v_Posptr;

	v_decl = v_data->vertexDeclaration;
	p_elem = v_decl->findElementBySemantic( Ogre::VES_POSITION );



	if (v_data)
	{
		size_t start = v_data->vertexStart;
		//pointer
		Ogre::HardwareVertexBufferSharedPtr v_sptr = v_data->vertexBufferBinding->getBuffer( p_elem->getSource() );
		unsigned char* v_ptr = static_cast<unsigned char*>(v_sptr->lock( Ogre::HardwareBuffer::HBL_READ_ONLY ));
		unsigned char* v_offset;

		//loop through vertex data...
		for (size_t j=start; j<(start+total_verts); j++)
		{
			//get offset to Position data!
			v_offset = v_ptr + (j * v_sptr->getVertexSize());
			p_elem->baseVertexPointerToElement( v_offset, &v_Posptr );

			//now get vertex positions...
			vertices[offset].x = *v_Posptr; v_Posptr++;
			vertices[offset].y = *v_Posptr; v_Posptr++;
			vertices[offset].z = *v_Posptr; v_Posptr++;

			vertices[offset] *= scale;

			offset++;
		}

		//unlock buffer
		v_sptr->unlock();
	}

	verts = vertices;
	return total_verts;	

}
void createPlaneColisionFromMesh(Ogre::Entity* ent, OgreNewt::World* world)
{
	Ogre::Vector3 *vertex;
	int vertcount;

	std::vector<OgreNewt::CollisionPtr> collVector;
	OgreNewt::ConvexCollisionPtr conColl;
	OgreNewt::CollisionPtr collision;

	//tailturn = 11
	vertcount = vertFromSubMesh(ent->getSubEntity(11)->getSubMesh(),Ogre::Vector3::UNIT_SCALE,vertex);
	conColl = OgreNewt::ConvexCollisionPtr(new OgreNewt::CollisionPrimitives::ConvexHull(world, 
		vertex, vertcount, 0, Ogre::Quaternion::IDENTITY, Ogre::Vector3::ZERO, 2 ));
	collVector.push_back(conColl);
	//airsucker+cokpit = 5
	vertcount = vertFromSubMesh(ent->getSubEntity(5)->getSubMesh(),Ogre::Vector3::UNIT_SCALE,vertex);
	conColl = OgreNewt::ConvexCollisionPtr(new OgreNewt::CollisionPrimitives::ConvexHull(world, 
		vertex, vertcount, 0, Ogre::Quaternion::IDENTITY, Ogre::Vector3::ZERO, 5));
	collVector.push_back(conColl);
	//mainbody = 4
	vertcount = vertFromSubMesh(ent->getSubEntity(4)->getSubMesh(),Ogre::Vector3::UNIT_SCALE,vertex);
	conColl = OgreNewt::ConvexCollisionPtr(new OgreNewt::CollisionPrimitives::ConvexHull(world, 
		vertex, vertcount, 0, Ogre::Quaternion::IDENTITY, Ogre::Vector3::ZERO, 1));
	collVector.push_back(conColl);
	//tailwings = 8
	vertcount = vertFromSubMesh(ent->getSubEntity(8)->getSubMesh(),Ogre::Vector3::UNIT_SCALE,vertex);
	conColl = OgreNewt::ConvexCollisionPtr(new OgreNewt::CollisionPrimitives::ConvexHull(world, 
		vertex, vertcount, 0, Ogre::Quaternion::IDENTITY, Ogre::Vector3::ZERO, 0.01));
	collVector.push_back(conColl);
	//leftwing = 2
	vertcount = vertFromSubMesh(ent->getSubEntity(2)->getSubMesh(),Ogre::Vector3::UNIT_SCALE,vertex);
	conColl = OgreNewt::ConvexCollisionPtr(new OgreNewt::CollisionPrimitives::ConvexHull(world, 
		vertex, vertcount, 0, Ogre::Quaternion::IDENTITY, Ogre::Vector3::ZERO));
	collVector.push_back(conColl);
	//rightwing = 3
	vertcount = vertFromSubMesh(ent->getSubEntity(3)->getSubMesh(),Ogre::Vector3::UNIT_SCALE,vertex);
	conColl = OgreNewt::ConvexCollisionPtr(new OgreNewt::CollisionPrimitives::ConvexHull(world, 
		vertex, vertcount, 0, Ogre::Quaternion::IDENTITY, Ogre::Vector3::ZERO));
	collVector.push_back(conColl);
	//wingdetails = 7
	vertcount = vertFromSubMesh(ent->getSubEntity(7)->getSubMesh(),Ogre::Vector3::UNIT_SCALE,vertex);
	conColl = OgreNewt::ConvexCollisionPtr(new OgreNewt::CollisionPrimitives::ConvexHull(world, 
		vertex, vertcount, 0, Ogre::Quaternion::IDENTITY, Ogre::Vector3::ZERO, 10));
	collVector.push_back(conColl);

	//motor
	collision = OgreNewt::CollisionPtr(new OgreNewt::CollisionPrimitives::ChamferCylinder(world,
		2.5, 0.5, 0, Ogre::Quaternion(Ogre::Degree(90), Ogre::Vector3(0,1,0)) , Ogre::Vector3(0,0,6.2)) );
	collVector.push_back(collision);

	//left landing gear
	collision = OgreNewt::CollisionPtr(new OgreNewt::CollisionPrimitives::Capsule(world, 
		0.6, 2.5, 0, Ogre::Quaternion(Ogre::Degree(90), Ogre::Vector3(0,0,1)),
		Ogre::Vector3(2.9,-1.25,1.75)));
	collVector.push_back(collision);

	//right landing gear
	collision = OgreNewt::CollisionPtr(new OgreNewt::CollisionPrimitives::Capsule(world, 
		0.6, 2.5, 0, Ogre::Quaternion(Ogre::Degree(90), Ogre::Vector3(0,0,1)),
		Ogre::Vector3(-2.6,-1.25,1.75)));
	collVector.push_back(collision);

	OgreNewt::CollisionPtr col( new OgreNewt::CollisionPrimitives::CompoundCollision(world, collVector, 0));

	OgreNewt::CollisionSerializer colSer;
	colSer.exportCollision(col, ".\\..\\..\\media\\models\\fighter.collision");

};

Airplane::Airplane(Ogre::SceneManager *sceneManager, OgreNewt::World *world, bool onServer):
mSceneMgr(sceneManager), mWorld(world), mServer(onServer)
{		
	mBody				= 0;
	mRollForce			= 0;
	mPitchForce			= 0;
	mActualThrustForce	= 0;
	mTimeToGainControll	= 0;
	mTimeToBornAgain	= 0;
	mPropRot			= 0.1;
	mPropBlurRot		= -0.05;
	mPlaneCreated		= false;
	mDestructed			= false;
	mDead				= false;
	mDeadSerialized		= false;
	mPosition			=
	mVelocity			= 
	mPrevRayPosition	=
	mPrevVelocity		= Ogre::Vector3::ZERO;
	mOrientation		=
	mPrevRayOrientation	=
	mPrevOrientation	= Ogre::Quaternion::ZERO;
	mShot1On			= 
	mPrevShot1On		= false;
	mScoreInfo			= new ScoresManager::PlayerScoreInfo();
	mTimeLastPosOriPack	= 0;
	mMotorSound			= 0;
		
	mShotRaycast = new AirplaneShotRaycast();
	mScoresManager = ScoresManager::getSingletonPtr();

#pragma region TESTBALLS

	/*Ogre::Real mass = 1;		
	OgreNewt::CollisionPtr collision;
	Ogre::Vector3 centmass, inertia;
	Ogre::Entity *ent;
	OgreNewt::Body *bod;
	Ogre::SceneNode *node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	ent = mSceneMgr->createEntity("ball1","ellipsoid.mesh");
	node->attachObject(ent);

	Ogre::Vector3 size(10,10,10);
	collision = OgreNewt::CollisionPtr(new OgreNewt::CollisionPrimitives::Ellipsoid(mWorld, size));
	NewtonConvexCollisionCalculateInertialMatrix(collision->getNewtonCollision(), &inertia.x, &centmass.x);
	inertia*=mass;

	bod = new OgreNewt::Body(mWorld,collision,1);
	bod->attachNode(node);
	bod->setMassMatrix(mass, inertia);
	bod->setStandardForceCallback();
	bod->setPositionOrientation(Ogre::Vector3(5,30,10), Ogre::Quaternion::IDENTITY);

	node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	ent = mSceneMgr->createEntity("ball21","ellipsoid.mesh");
	node->attachObject(ent);

	 node->setScale(size);
	
	collision = OgreNewt::CollisionPtr(new OgreNewt::CollisionPrimitives::Ellipsoid(mWorld, size));
	NewtonConvexCollisionCalculateInertialMatrix(collision->getNewtonCollision(), &inertia.x, &centmass.x);
	inertia*=mass;

	bod = new OgreNewt::Body(mWorld,collision,1);
	bod->attachNode(node);
	bod->setMassMatrix(mass, inertia);
	bod->setStandardForceCallback();
	bod->setPositionOrientation(Ogre::Vector3(15,30,10), Ogre::Quaternion::IDENTITY);

	node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	ent = mSceneMgr->createEntity("ball31","ellipsoid.mesh");
	node->attachObject(ent);

	node->setScale(size);
	
	collision = OgreNewt::CollisionPtr(new OgreNewt::CollisionPrimitives::Ellipsoid(mWorld, size));
	NewtonConvexCollisionCalculateInertialMatrix(collision->getNewtonCollision(), &inertia.x, &centmass.x);
	inertia*=mass;

	bod = new OgreNewt::Body(mWorld,collision,1);
	bod->attachNode(node);
	bod->setMassMatrix(mass, inertia);
	bod->setStandardForceCallback();
	bod->setPositionOrientation(Ogre::Vector3(5,50,10), Ogre::Quaternion::IDENTITY);

	node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	ent = mSceneMgr->createEntity("ball41","ellipsoid.mesh");
	node->attachObject(ent);

node->setScale(size);
	
	collision = OgreNewt::CollisionPtr(new OgreNewt::CollisionPrimitives::Ellipsoid(mWorld, size));
	NewtonConvexCollisionCalculateInertialMatrix(collision->getNewtonCollision(), &inertia.x, &centmass.x);
	inertia*=mass;

	bod = new OgreNewt::Body(mWorld,collision,1);
	bod->attachNode(node);
	bod->setMassMatrix(mass, inertia);
	bod->setStandardForceCallback();
	bod->setPositionOrientation(Ogre::Vector3(5,30,16), Ogre::Quaternion::IDENTITY);

	node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	ent = mSceneMgr->createEntity("ball51","ellipsoid.mesh");
	node->attachObject(ent);

	node->setScale(size);
	
	collision = OgreNewt::CollisionPtr(new OgreNewt::CollisionPrimitives::Ellipsoid(mWorld, size));
	NewtonConvexCollisionCalculateInertialMatrix(collision->getNewtonCollision(), &inertia.x, &centmass.x);
	inertia*=mass;

	bod = new OgreNewt::Body(mWorld,collision,1);
	bod->attachNode(node);
	bod->setMassMatrix(mass, inertia);
	bod->setStandardForceCallback();
	bod->setPositionOrientation(Ogre::Vector3(55,30,10), Ogre::Quaternion::IDENTITY);

	node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	ent = mSceneMgr->createEntity("ball61","ellipsoid.mesh");
	node->attachObject(ent);

	node->setScale(size);
	
	collision = OgreNewt::CollisionPtr(new OgreNewt::CollisionPrimitives::Ellipsoid(mWorld, size));
	NewtonConvexCollisionCalculateInertialMatrix(collision->getNewtonCollision(), &inertia.x, &centmass.x);
	inertia*=mass;

	bod = new OgreNewt::Body(mWorld,collision,1);
	bod->attachNode(node);
	bod->setMassMatrix(mass, inertia);
	bod->setStandardForceCallback();
	bod->setPositionOrientation(Ogre::Vector3(5,32,10), Ogre::Quaternion::IDENTITY);

		node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	ent = mSceneMgr->createEntity("ball711","ellipsoid.mesh");
	node->attachObject(ent);

	node->setScale(size);
	
	collision = OgreNewt::CollisionPtr(new OgreNewt::CollisionPrimitives::Ellipsoid(mWorld, size));
	NewtonConvexCollisionCalculateInertialMatrix(collision->getNewtonCollision(), &inertia.x, &centmass.x);
	inertia*=mass;

	bod = new OgreNewt::Body(mWorld,collision,1);
	bod->attachNode(node);
	bod->setMassMatrix(mass, inertia);
	bod->setStandardForceCallback();
	bod->setPositionOrientation(Ogre::Vector3(5,36,10), Ogre::Quaternion::IDENTITY);

		node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	ent = mSceneMgr->createEntity("ball712","ellipsoid.mesh");
	node->attachObject(ent);

	node->setScale(size);
	
	collision = OgreNewt::CollisionPtr(new OgreNewt::CollisionPrimitives::Ellipsoid(mWorld, size));
	NewtonConvexCollisionCalculateInertialMatrix(collision->getNewtonCollision(), &inertia.x, &centmass.x);
	inertia*=mass;

	bod = new OgreNewt::Body(mWorld,collision,1);
	bod->attachNode(node);
	bod->setMassMatrix(mass, inertia);
	bod->setStandardForceCallback();
	bod->setPositionOrientation(Ogre::Vector3(5,36,10), Ogre::Quaternion::IDENTITY);

		node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	ent = mSceneMgr->createEntity("ball731","ellipsoid.mesh");
	node->attachObject(ent);

	node->setScale(size);
	
	collision = OgreNewt::CollisionPtr(new OgreNewt::CollisionPrimitives::Ellipsoid(mWorld, size));
	NewtonConvexCollisionCalculateInertialMatrix(collision->getNewtonCollision(), &inertia.x, &centmass.x);
	inertia*=mass;

	bod = new OgreNewt::Body(mWorld,collision,1);
	bod->attachNode(node);
	bod->setMassMatrix(mass, inertia);
	bod->setStandardForceCallback();
	bod->setPositionOrientation(Ogre::Vector3(5,36,10), Ogre::Quaternion::IDENTITY);

		node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	ent = mSceneMgr->createEntity("ball741","ellipsoid.mesh");
	node->attachObject(ent);

	node->setScale(size);
	
	collision = OgreNewt::CollisionPtr(new OgreNewt::CollisionPrimitives::Ellipsoid(mWorld, size));
	NewtonConvexCollisionCalculateInertialMatrix(collision->getNewtonCollision(), &inertia.x, &centmass.x);
	inertia*=mass;

	bod = new OgreNewt::Body(mWorld,collision,1);
	bod->attachNode(node);
	bod->setMassMatrix(mass, inertia);
	bod->setStandardForceCallback();
	bod->setPositionOrientation(Ogre::Vector3(5,36,10), Ogre::Quaternion::IDENTITY);

		node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	ent = mSceneMgr->createEntity("ball715","ellipsoid.mesh");
	node->attachObject(ent);

	node->setScale(size);
	
	collision = OgreNewt::CollisionPtr(new OgreNewt::CollisionPrimitives::Ellipsoid(mWorld, size));
	NewtonConvexCollisionCalculateInertialMatrix(collision->getNewtonCollision(), &inertia.x, &centmass.x);
	inertia*=mass;

	bod = new OgreNewt::Body(mWorld,collision,1);
	bod->attachNode(node);
	bod->setMassMatrix(mass, inertia);
	bod->setStandardForceCallback();
	bod->setPositionOrientation(Ogre::Vector3(5,36,10), Ogre::Quaternion::IDENTITY);

		node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	ent = mSceneMgr->createEntity("ball716","ellipsoid.mesh");
	node->attachObject(ent);

	node->setScale(size);
	
	collision = OgreNewt::CollisionPtr(new OgreNewt::CollisionPrimitives::Ellipsoid(mWorld, size));
	NewtonConvexCollisionCalculateInertialMatrix(collision->getNewtonCollision(), &inertia.x, &centmass.x);
	inertia*=mass;

	bod = new OgreNewt::Body(mWorld,collision,1);
	bod->attachNode(node);
	bod->setMassMatrix(mass, inertia);
	bod->setStandardForceCallback();
	bod->setPositionOrientation(Ogre::Vector3(5,36,10), Ogre::Quaternion::IDENTITY);

		node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	ent = mSceneMgr->createEntity("ball717","ellipsoid.mesh");
	node->attachObject(ent);

	node->setScale(size);
	
	collision = OgreNewt::CollisionPtr(new OgreNewt::CollisionPrimitives::Ellipsoid(mWorld, size));
	NewtonConvexCollisionCalculateInertialMatrix(collision->getNewtonCollision(), &inertia.x, &centmass.x);
	inertia*=mass;

	bod = new OgreNewt::Body(mWorld,collision,1);
	bod->attachNode(node);
	bod->setMassMatrix(mass, inertia);
	bod->setStandardForceCallback();
	
	bod->setPositionOrientation(Ogre::Vector3(5,36,10), Ogre::Quaternion::IDENTITY);*/

#pragma endregion
	
}

Airplane::~Airplane(void)
{	
	BroadcastDestruction();
	ScoresManager::getSingletonPtr()->removePlayer(mOwnerID);
	

	if(mBody)
		delete mBody;

	mSceneMgr->destroyEntity(mPlaneEntity);
	mSceneMgr->destroyEntity(mPropelerEnt);
	mSceneMgr->destroyEntity(mPropelerBlurEnt);

	mPlaneNode->removeAndDestroyChild(mPropNode->getName());
	mPlaneNode->removeAndDestroyChild(mPropBlurNode->getName());
	mSceneMgr->getRootSceneNode()->removeAndDestroyChild(mPlaneNode->getName());	

	mMotorSound->stop();
	mSoundMgr->destroySound(mMotorSound);
	
	delete mShotsMgr;
}



void Airplane::createPlane(Ogre::Vector3 position, Ogre::Quaternion orientation)
{
	if(mPlaneCreated)
		return;

	mPlaneMass = 50;
	Ogre::String idStr(mOwnerID.ToString());
	
	mPlaneEntity = mSceneMgr->createEntity("plane" + Ogre::String(mOwnerID.ToString()),
		"fighter_noprop.mesh");
	mPlaneEntity->setQueryFlags(RealToys::MASK_AIRPLANE);
	mPlaneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	mPlaneNode->attachObject(mPlaneEntity);



	mMotorPosition.z = mPlaneEntity->getBoundingBox().getMaximum().z;// * scale.z;
	mMotorPosition.x = mMotorPosition.y = 0;

	mWingPosition.x = mPlaneEntity->getBoundingBox().getMaximum().x;// * scale.x;
	mWingPosition.z = mWingPosition.y = 0;

	mShotRayPosition.x = mShotRayPosition.y = 0;
	mShotRayPosition.z = mMotorPosition.z ;

	mShot1Position.x = mWingPosition.x - mWingPosition.x*0.3;
	mShot1Position.y = -1;
	mShot1Position.z = 0;

	std::vector<Ogre::Vector3> shot1Positions;
	shot1Positions.push_back(mShot1Position);
	mShot1Position.x *= -1;
	shot1Positions.push_back(mShot1Position);
	mShotsMgr = new ShotsManager(50, 0.05f, shot1Positions,	idStr + "_shot1_", mSceneMgr, mWorld, mOwnerID, mPlaneNode);


	/*	Add the propeller	*/
	mPropNode = mPlaneNode->createChildSceneNode();
	mPropelerEnt = mSceneMgr->createEntity("propEnt" + idStr, 
		"fighterprop.mesh");
	mPropelerEnt->setQueryFlags(RealToys::MASK_AIRPLANE);
	mPropNode->attachObject(mPropelerEnt);
	mPropNode->translate(mMotorPosition.x, mMotorPosition.y, mMotorPosition.z-0.5);
	//mPropNode->showBoundingBox(true);
	Ogre::AxisAlignedBox bbox = mPropelerEnt->getBoundingBox();

	//propeller blur effect
	Ogre::MovablePlane *plane = new Ogre::MovablePlane("PropellerBlurPlane");
	plane->d = 0;	
	plane->normal = Ogre::Vector3::UNIT_Z;	
	Ogre::MeshManager::getSingleton().createPlane("PropellerBlurMesh", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
		*plane, bbox.getSize().x+0.5, bbox.getSize().y+1, 1, 1, true, 1, 1, 1, Ogre::Vector3::UNIT_Y);

	mPropBlurNode = mPlaneNode->createChildSceneNode();
	mPropelerBlurEnt = mSceneMgr->createEntity("propBlurEnt" + idStr, 
		"PropellerBlurMesh");
	mPropelerBlurEnt->setQueryFlags(RealToys::MASK_AIRPLANE);
	mPropelerBlurEnt->setMaterialName("RealToys/propellerblur_material");
	mPropBlurNode->attachObject(mPropelerBlurEnt);
	mPropBlurNode->translate(mMotorPosition.x, mMotorPosition.y, mMotorPosition.z-0.5);

	mPlaneCreated = true;

	createPlaneColisionFromMesh(mSceneMgr->createEntity("fightercollision.mesh"), mWorld);
	Ogre::DataStreamPtr dsptr = Ogre::ResourceGroupManager::getSingletonPtr()->openResource("fighter.collision");
	OgreNewt::CollisionSerializer colSer;
	//mPlaneCollision = OgreNewt::CollisionPtr(new OgreNewt::CollisionPrimitives::Cylinder(mWorld, 3, 10, 0));
	mPlaneCollision = colSer.importCollision((*dsptr.getPointer()), mWorld);

	//conCollision = boost::dynamic_pointer_cast<OgreNewt::ConvexCollision>( collision );


	//collision->calculateInertialMatrix(inertia, centmass);
	NewtonConvexCollisionCalculateInertialMatrix(mPlaneCollision->getNewtonCollision(), &mPlaneInertia.x, &mPlaneCenterOfMass.x);
	mPlaneInertia*=mPlaneMass;	

	mPlaneCenterOfMass = mMotorPosition*0.5;

	mScoreInfo = mScoresManager->addPlayer(mOwnerID);	
	
	mSoundMgr = OgreOggSound::OgreOggSoundManager::getSingletonPtr();
	if ( mMotorSound = mSoundMgr->createSound(idStr + "_motorsound", "seaplane.wav", false, true ) )
	{
		mMotorSound->setMaxDistance(500);
		mMotorSound->setReferenceDistance(300);
		mMotorSound->setRolloffFactor(1);
		mPlaneNode->attachObject(mMotorSound);
	}
	if ( mExplodeSound = mSoundMgr->createSound(idStr + "_explodesound", "crash.wav", false, false, false ) )
	{
		mExplodeSound->setMaxDistance(500);
		mExplodeSound->setReferenceDistance(300);
		mExplodeSound->setRolloffFactor(1);
		mPlaneNode->attachObject(mExplodeSound);
	}

	born(position, orientation);
}

void Airplane::setThrustForce(int thrust)
{	

	mThrustForce = thrust;
}

void Airplane::increaseThrust()
{
	if(mThrustForce < THRUSTFORCE_3)
	{
		if(mThrustForce >= THRUSTFORCE_2)
			setThrustForce(THRUSTFORCE_3);
		else
		{
			if(mThrustForce >= THRUSTFORCE_1)
				setThrustForce(THRUSTFORCE_2);
			else
				setThrustForce(THRUSTFORCE_1);			
		}
	}	
}
void Airplane::decreaseThrust()
{
	if(mThrustForce > THRUSTFORCE_0)
	{
		if(mThrustForce <= THRUSTFORCE_1)
			setThrustForce(THRUSTFORCE_0);
		else
		{
			if(mThrustForce <= THRUSTFORCE_2)
				setThrustForce(THRUSTFORCE_1);
			else
			{
				if(mThrustForce <= THRUSTFORCE_3)
					setThrustForce(THRUSTFORCE_2);
				else
					setThrustForce(THRUSTFORCE_3);
			}
		}		
	}
}
void Airplane::setPitchForce(Ogre::Real force)
{
	mPitchForce = force;
}
void Airplane::setRollForce(Ogre::Real force)
{
	mRollForce = force;
}
void Airplane::addPitchForce(Ogre::Real force)
{
	mPitchForce += force;

	//prevent from "overloading" the force (like pressing two keys with the same action)
	if(mPitchForce > PITCHFORCE_2)
		mPitchForce = PITCHFORCE_2;
	else if(mPitchForce < -PITCHFORCE_2)
		mPitchForce = -PITCHFORCE_2;
}
void Airplane::addRollForce(Ogre::Real force)
{
	mRollForce += force;
	//prevent from "overloading" the force (like pressing two keys with the same action)
	if(mRollForce > ROLLFORCE_2)
		mRollForce = ROLLFORCE_2;
	else if(mRollForce < -ROLLFORCE_2)
		mRollForce = -ROLLFORCE_2;
	
}
void Airplane::AfterCollisionCallback(OgreNewt::Body* body, float timeStep, int threadIndex )
{		
	if(mTimeToGainControll > 0)
	{
		mTimeToGainControll-=timeStep;

		Ogre::Vector3 force;
		
		//apply pitch
		force = Ogre::Vector3(0, mPitchForce, 0);
		body->addLocalForce(force, mMotorPosition);
		//body->addLocalForce(force*-1, -mMotorPosition);	
		
		//apply roll
		force = Ogre::Vector3(0, mRollForce, 0);
		body->addLocalForce(force, mWingPosition);
		body->addLocalForce(force*-1, -mWingPosition);
		
	}
	else
	{
		mTimeToGainControll = 0;
		setNormalMotionForceCallback();
	}
	
	//mPropNode->roll(mPropRot);
	//mPropBlurNode->roll(mPropBlurRot);
}
void Airplane::NormalMotionCallback(OgreNewt::Body* body, float timeStep, int threadIndex )
{	
	if(mActualThrustForce < mThrustForce - 50)
	{
		mActualThrustForce += 50;
	}
	else if(mActualThrustForce > mThrustForce + 50)
	{
		mActualThrustForce -= 50;
	}
	Ogre::Vector3 force;

	
	//apply thrust
	force = Ogre::Vector3(0,0, mActualThrustForce);
	body->addLocalForce(force, mMotorPosition);		
		

	//apply pitch
	force = Ogre::Vector3(0, mPitchForce, 0);
	body->addLocalForce(force, mMotorPosition);
	//body->addLocalForce(force*-1, -mMotorPosition);	
	
	//apply roll
	force = Ogre::Vector3(0, mRollForce, 0);
	body->addLocalForce(force, mWingPosition);
	body->addLocalForce(force*-1, -mWingPosition);

	/*Ogre::Radian addedRot( (mActualThrustForce*0.03f)/THRUSTFORCE_3*1.0f );
	
	mPropNode->roll(mPropRot+addedRot);
	mPropBlurNode->roll(mPropBlurRot+addedRot);*/

}


void Airplane::setNormalMotionForceCallback()
{
	if(mServer)
		mBody->setCustomForceAndTorqueCallback<Airplane>(&Airplane::NormalMotionCallback, this);
}

void Airplane::setCollisionActions()
{	
	if(mTimeToGainControll!= 0 || mServer == false)
		return;

	mTimeToGainControll = 0.5f;
	
	mBody->setCustomForceAndTorqueCallback<Airplane>(&Airplane::AfterCollisionCallback, this);
}

void Airplane::getPositionOrientation(Ogre::Vector3 &position, Ogre::Quaternion &orientation)
{
	//mBody->getPositionOrientation(position, orientation);
	position = mPosition;
	orientation = mOrientation;	
}

void Airplane::getVelocity(Ogre::Vector3 &vel)
{
	if(mServer)
		vel = mBody->getVelocity();
	else
		vel = mVelocity;
}
void Airplane::getOmega(Ogre::Vector3 &omega)
{
	if(mServer && !mDead)
		omega = mBody->getOmega();
	else
	{
		omega = 0;
		return;

		//omega.x = mVelocity/mMotorPosition.z;
		//omega.y = mVelocity/mMotorPosition.z;
		//omega.z = mVelocity/mWingPosition.x;
		Ogre::Quaternion oriDelta = (mOrientation-mPrevOrientation);

		omega.x = (oriDelta.getPitch()/mTimeSinceLastUpdate).valueRadians();
		omega.y = (oriDelta.getYaw()/mTimeSinceLastUpdate).valueRadians();
		omega.z = (oriDelta.getRoll()/mTimeSinceLastUpdate).valueRadians();
	}
}


void Airplane::update(Ogre::Real timeSinceLastFrame)
{
	if(mScoreInfo->mCurrentHealth <= 0)
	{
		die();
		return;
	}

	
		
	if(mServer)
	{
		mBody->getPositionOrientation(mPosition, mOrientation);
		mVelocity = mBody->getVelocity();
	}
	else
	{
		mPosition += mVelocity * timeSinceLastFrame;	
		mBody->setPositionOrientation(mPosition, mOrientation);
	}
	mTimeSinceLastUpdate = timeSinceLastFrame;

	float soundPitch = 1.0f * Ogre::Math::Abs( (mOrientation.Inverse()*mVelocity).z ) / 150.0f  + 0.6f;
	mMotorSound->setPitch( soundPitch );

	//std::cout << soundPitch << std::endl;

	Ogre::Radian addedRot( (mActualThrustForce*0.03f)/THRUSTFORCE_3*1.0f);
	//simulate fixed timestep in propeller effects 
	mPropNode->roll( ((mPropRot+addedRot) * timeSinceLastFrame) / 0.0088333333333f  );
	mPropBlurNode->roll( ((mPropBlurRot+addedRot) * timeSinceLastFrame) / 0.0088333333333f);

	if(mShot1On)
	{
		Ogre::Vector3 hit;
		this->getTargetHitPoint(hit);
		mShotsMgr->fire(timeSinceLastFrame, mPosition, mOrientation, hit);
	}
}
bool Airplane::getTargetHitPoint(Ogre::Vector3 &hit)
{
	//ballN->setPosition(mPosition+mOrientation*mShotRayPosition);
	//return false;

	//Ogre::MovableObject *movObj;
	//float dist;

	//std::cout << mPosition << std::endl;
	//
	//return mMocColTools->raycastFromPoint(mPosition+mOrientation*mShotRayPosition,
	//	mOrientation*Ogre::Vector3::UNIT_Z,	hit, movObj, dist);


	Ogre::Vector3 start, end;
	Ogre::Real hitDist = -1, rayDist = 500 , maxDist = 10000;
	Ogre::Real startDist = 0, endDist = rayDist;
	Ogre::Ray hitRay(mPosition+mOrientation*mShotRayPosition, mOrientation*Ogre::Vector3::UNIT_Z);
	
	start = hitRay.getOrigin();
	end = hitRay.getPoint(endDist);
	while(hitDist < 0)
	{
		// Cast a ray between these points and check for first hit
		mShotRaycast->go(mWorld, start, end);
		hitDist = mShotRaycast->getClosestHit().mDistance;
		//OgreNewt::BasicRaycast::BasicRaycastInfo info = mShotRaycast->getClosestHit();

		/*OgreNewt::BasicRaycast *basicRaycast = new OgreNewt::BasicRaycast(mWorld, start, end, false);
		OgreNewt::BasicRaycast::BasicRaycastInfo info = basicRaycast->getFirstHit();*/
				
		startDist = endDist;
		endDist += rayDist;

		if(startDist >= maxDist)
		{			
			//no body found
			hit = hitRay.getPoint(maxDist);
			return false;
		}

		start = end;
		end = hitRay.getPoint(endDist);
	}

	hit = hitRay.getPoint((startDist-rayDist) + hitDist*startDist);	//determinar ponto correcto relativamente a hitDist que está entre [0,1] e corresponde ao tamanho do raio

	mPrevRayPosition = mPosition;
	mPrevRayOrientation = mOrientation;

	if(mShotRaycast->getClosestHit().mBody->getType() == RealToys::BODYTYPE_AIRPLANE)
		return true;
	else
		return false;
}

void Airplane::die()
{
	if(mDead)
		return;

	mTimeToBornAgain = 5.0f;		//five seconds to reborn

	if(mBody)
	{
		delete mBody;
		mBody = 0;
	}

	mPlaneNode->setVisible(false);
	if(mMotorSound) mMotorSound->stop();
	if(mExplodeSound) mExplodeSound->play();

	mShot1On = false;
	mDead = true;
	mDeadSerialized = false;
}
void Airplane::born(Ogre::Vector3 &position, Ogre::Quaternion &orientation)
{
	mPosition = position;
	mOrientation = orientation;

	mBody = new OgreNewt::Body(mWorld, mPlaneCollision);			
	mBody->attachNode(mPlaneNode);
	mBody->setUserData(Ogre::Any(this));
	mBody->setMassMatrix(mPlaneMass, mPlaneInertia);	
	mBody->setCenterOfMass(mPlaneCenterOfMass);
	mBody->setContinuousCollisionMode(1); 
	mBody->setAngularDamping(Ogre::Vector3(1,1,1));
	mBody->setLinearDamping(1);
	mBody->setPositionOrientation(position, orientation);
	//mBody->setPositionOrientation(position, Ogre::Quaternion(0.943156, 0.28065, 0.177861, 0.00761348));
	mBody->setType(RealToys::BODYTYPE_AIRPLANE);
	mBody->setMaterialGroupID(NewtonMaterialManager::getSingletonPtr()->getMaterialID(NME_AIRPLANE));

	setNormalMotionForceCallback();

	mActualThrustForce = 0;
	setThrustForce(THRUSTFORCE_1);

	mPlaneNode->setVisible(true);

	mScoresManager->resetHealth(mOwnerID);

	if(mMotorSound) mMotorSound->play();

	mDead = false;
	mDeadSerialized = true;
}

void Airplane::setShot1On( bool on )
{ 
	mShot1On = on; 
	if(!on) 
		mShotsMgr->stopFireSound(); 
}
