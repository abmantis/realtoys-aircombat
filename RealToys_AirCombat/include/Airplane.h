#ifndef _AIRPLANE_H_
#define _AIRPLANE_H_


//#include "AirplanePredictor.h"
#include "ShotsManager.h"
#include "NewtonCustomRaycasts.h"
#include "ScoresManager.h"



class Airplane : public RakNet::Replica3
{
public:
	enum SerializationModes
	{
		SERMODE_POSORI				= 1<<0,
		SERMODE_SHOT				= 1<<1,
		SERMODE_SCORE				= 1<<2,
	};

	Airplane(Ogre::SceneManager *sceneManager, OgreNewt::World *world, bool onServer);
	~Airplane(void);

	void createPlane(Ogre::Vector3 position, Ogre::Quaternion orientation);

	/*
		Sets this plane's thrust.
		@param thrust - use Airplane_Params::PITCHFORCE_X
	*/
	void setThrustForce(Ogre::Real thrust);
	void increaseThrust();
	void decreaseThrust();
	void setPitchForce(Ogre::Real force);
	void setRollForce(Ogre::Real force);
	void addPitchForce(Ogre::Real force);
	void addRollForce(Ogre::Real force);
	
	void setNormalMotionForceCallback();
	void setCollisionActions();

	void getPositionOrientation(Ogre::Vector3 &position, Ogre::Quaternion &orientation);

	void getVelocity(Ogre::Vector3 &vel);
	void getOmega(Ogre::Vector3 &omega);

	/*
		Get the first hit of a ray fired from the airplane position to it's front
		Returns true if an enemy is on target, false otherwise. 
	*/
	bool getTargetHitPoint(Ogre::Vector3 &hit);

	/*
		Updates this airplane. Should only be used by clients! In server newton updates airplanes
	*/
	void update(Ogre::Real timeSinceLastFrame);

	void setOwner(PlayerID ID) { mOwnerID = ID; }
	PlayerID getOwner() { return mOwnerID; }

	bool isDestructed() { return mDestructed; }

	void setShot1On( bool on );
	
	const ScoresManager::PlayerScoreInfo* getScoreInfo() { return mScoreInfo; }

	void die();
	void born(Ogre::Vector3 &position, Ogre::Quaternion &orientation);

	/*
		Decrease the time to born again by an amount of time given by decrement;
		Returns the result time to born again;
	*/
	float decreaseTimeToBornAgain(float decrement) { mTimeToBornAgain -= decrement; return mTimeToBornAgain; }
	bool isDead() { return mDead; }
	
	
private:
	Ogre::SceneManager *mSceneMgr;
	OgreNewt::World *mWorld;
	Ogre::Entity *mPlaneEntity;
	Ogre::Entity *mPropelerEnt;
	Ogre::Entity *mPropelerBlurEnt;
	Ogre::SceneNode *mPropNode;		//propeller node
	Ogre::SceneNode *mPropBlurNode;	//propeller blur effect node
	Ogre::SceneNode *mPlaneNode; 
	OgreNewt::Body *mBody;
	OgreNewt::CollisionPtr mPlaneCollision;
	OgreOggSound::OgreOggSoundManager *mSoundMgr;		//Sound manager

	OgreOggSound::OgreOggISound *mMotorSound;
	OgreOggSound::OgreOggISound *mExplodeSound;

	bool mServer;					//whether this is running on the server or not	
	bool mPlaneCreated;
	bool mDestructed;
	bool mDead;
	bool mDeadSerialized;

	Ogre::Vector3 mPlaneCenterOfMass;
	Ogre::Vector3 mPlaneInertia;

	Ogre::Vector3 mMotorPosition;	//front extreme of the plane
	Ogre::Vector3 mWingPosition;	//side extreme of the plane
	Ogre::Vector3 mShotRayPosition;	//start position of the fire/target raycast
	Ogre::Vector3 mShot1Position;	//shot1 position
	
	Ogre::Real mPlaneMass;			//airplane mass


	Ogre::Real mThrustForce;				//force applied to go in front direction (thrust)
	Ogre::Real mActualThrustForce;			//actual force applied to go in front direction (thrust)
	Ogre::Real mPitchForce;				//force applied to pitch
	Ogre::Real mRollForce;					//force applied to roll

	float mTimeToGainControll;		//time remaining to get thrust, usually after a collision
	float mTimeSinceLastUpdate;		//time since the last update to the actual on (only used by clients)
	float mTimeToBornAgain;	

	RakNet::Time mTimeLastPosOriPack;
		
	Ogre::Radian mPropRot;			//propeller rotation on newton each call
	Ogre::Radian mPropBlurRot;		//propeller blur rotation on newton each call


	Ogre::Vector3 mPosition;		
	Ogre::Vector3 mPrevPosition;	//only used by client
	Ogre::Vector3 mPrevRayPosition;	//used to prevent unnecessary raycasting
	Ogre::Vector3 mVelocity;		
	Ogre::Vector3 mPrevVelocity;	//on server used only to decide to serialize or not
	Ogre::Quaternion mOrientation;	
	Ogre::Quaternion mPrevOrientation;//on server used only to decide to serialize or not
	Ogre::Quaternion mPrevRayOrientation;//used to prevent unnecessary raycasting


	PlayerID mOwnerID;			// The client (or server) that owns this airplane

	ShotsManager *mShotsMgr;
	bool mShot1On;					//is shot1 being fired?
	bool mPrevShot1On;				//used to decide if we should serialize
	AirplaneShotRaycast *mShotRaycast;

	ScoresManager *mScoresManager;
	const ScoresManager::PlayerScoreInfo *mScoreInfo;
	ScoresManager::PlayerScoreInfo mLastSerializedScoreInfo;
	

	//AirplanePredictor mAirplanePredictor;
	//Extrapolator<3, float> mPositionExtrapolator;


	/* FORCE CALLBACKS */
	void AfterCollisionCallback(OgreNewt::Body* body, float timeStep, int threadIndex );
	void NormalMotionCallback(OgreNewt::Body* body, float timeStep, int threadIndex );

public:	//Replica3 functions

	static RakNet::RakString GetName(void) {
		return RakNet::RakString("Airplane");
	}

	void WriteAllocationID(RakNet::Connection_RM3 *destinationConnection, RakNet::BitStream *allocationIdBitstream) const
	{
		allocationIdBitstream->Write(GetName());
	}

	void SerializeConstruction(RakNet::BitStream *constructionBitstream, RakNet::Connection_RM3 *destinationConnection)
	{		
		RakAssert(mServer);
		std::cout << "Serializing airplane construction - " << mOwnerID.ToString() << std::endl;

		mBody->getPositionOrientation(mPosition, mOrientation);
		mPosition = RealToys::FromNewton(mPosition);
		mOrientation = RealToys::FromNewton(mOrientation);
		mVelocity = RealToys::FromNewton(mBody->getVelocity());

		constructionBitstream->Write(mOwnerID);

		constructionBitstream->Write(mShot1On);
		mPrevShot1On = mShot1On;

		constructionBitstream->WriteAlignedBytes((const unsigned char*)&mPosition,sizeof(mPosition));
		constructionBitstream->WriteAlignedBytes((const unsigned char*)&mOrientation,sizeof(mOrientation));
		constructionBitstream->WriteAlignedBytes((const unsigned char*)&mVelocity,sizeof(mVelocity));
		constructionBitstream->WriteAlignedBytes((const unsigned char*)&mActualThrustForce,sizeof(mActualThrustForce));
		mPrevOrientation = mOrientation;
		mPrevVelocity = mVelocity;			
		mTimeLastPosOriPack = RakNet::GetTime(); 

		constructionBitstream->WriteCompressed(mScoreInfo->mCurrentHealth);
		constructionBitstream->WriteCompressed(mScoreInfo->mDeaths);
		constructionBitstream->WriteCompressed(mScoreInfo->mKills);
		mLastSerializedScoreInfo = ScoresManager::PlayerScoreInfo(*mScoreInfo);

		return;
	}

	bool DeserializeConstruction(RakNet::BitStream *constructionBitstream, RakNet::Connection_RM3 *sourceConnection)
	{
		RakAssert(!mServer);
		std::cout << "De serializing airplane construction - "; 

		constructionBitstream->Read(mOwnerID);

		std::cout << mOwnerID.ToString() << std::endl;

		constructionBitstream->Read(mShot1On);

		constructionBitstream->ReadAlignedBytes((unsigned char*)&mPosition, sizeof(mPosition));
		constructionBitstream->ReadAlignedBytes((unsigned char*)&mOrientation, sizeof(mOrientation));
		constructionBitstream->ReadAlignedBytes((unsigned char*)&mVelocity, sizeof(mVelocity));
		constructionBitstream->ReadAlignedBytes((unsigned char*)&mActualThrustForce, sizeof(mActualThrustForce));
		
		

		//mAirplanePredictor.add(mPrevPosition, mPrevVelocity, mPrevPosition, mPrevVelocity);		//the last two params won't mather at this point (first record)

		//mPosition = mPrevPosition;
		//mVelocity = mPrevVelocity;

		createPlane(mPosition, mOrientation);

		constructionBitstream->ReadCompressed(mScoreInfo->mCurrentHealth);
		constructionBitstream->ReadCompressed(mScoreInfo->mDeaths);
		constructionBitstream->ReadCompressed(mScoreInfo->mKills);

		return true;
	}

	void SerializeDestruction(RakNet::BitStream *destructionBitstream, RakNet::Connection_RM3 *destinationConnection)
	{		
		std::cout << "Serializing destruction" << std::endl;
		return;
	}

	bool DeserializeDestruction(RakNet::BitStream *destructionBitstream, RakNet::Connection_RM3 *sourceConnection)
	{
		std::cout << "Deserializing destruction" << std::endl;
		return true;
	}

	void DeallocReplica(RakNet::Connection_RM3 *sourceConnection)
	{
		mDestructed = true;
	}

	RakNet::RM3ActionOnPopConnection QueryActionOnPopConnection(RakNet::Connection_RM3 *droppedConnection) const
	{
		if (mServer)
			return QueryActionOnPopConnection_Server(droppedConnection);
		else
			return QueryActionOnPopConnection_Client(droppedConnection);
	}

	
	//void SerializeConstructionRequestAccepted(RakNet::BitStream *serializationBitstream,
	//	RakNet::Connection_RM3 *requestingConnection);

	//void DeserializeConstructionRequestAccepted(RakNet::BitStream *serializationBitstream, 
	//	RakNet::Connection_RM3 *acceptingConnection);

	//void SerializeConstructionRequestRejected(RakNet::BitStream *serializationBitstream,
	//	RakNet::Connection_RM3 *requestingConnection);

	//void DeserializeConstructionRequestRejected(RakNet::BitStream *serializationBitstream,
	//	RakNet::Connection_RM3 *rejectingConnection);

	RakNet::RM3ConstructionState QueryConstruction(RakNet::Connection_RM3 *destinationConnection, RakNet::ReplicaManager3 *replicaManager3) 
	{
		return QueryConstruction_ServerConstruction(destinationConnection, mServer);
	}
	bool QueryRemoteConstruction(RakNet::Connection_RM3 *sourceConnection) 
	{
		return QueryRemoteConstruction_ServerConstruction(sourceConnection, mServer);
	}

	RakNet::RM3QuerySerializationResult QuerySerialization(RakNet::Connection_RM3 *destinationConnection)
	{
		return QuerySerialization_ServerSerializable(destinationConnection, mServer);
	}
	
	RakNet::RM3SerializationResult Serialize(RakNet::SerializeParameters *serializeParameters)
	{
		RakAssert(mServer);

		unsigned char serMode = 0;

		if(mDeadSerialized == false && mDead == true)
		{
			serMode = SERMODE_POSORI + SERMODE_SCORE + SERMODE_SHOT;
		}
		else
		{
			mBody->getPositionOrientation(mPosition, mOrientation);
			mPosition = RealToys::FromNewton(mPosition);
			mOrientation = RealToys::FromNewton(mOrientation);
			mVelocity = RealToys::FromNewton(mBody->getVelocity());


			if( mPrevShot1On != mShot1On )
			{
				serMode += SERMODE_SHOT;			
			}
			if( (RakNet::GetTime() - mTimeLastPosOriPack) > 500 
				|| mPrevVelocity != mVelocity || mPrevOrientation != mOrientation )
			{
				serMode += SERMODE_POSORI;			
			}
			if( mLastSerializedScoreInfo != (*mScoreInfo))
			{
				serMode += SERMODE_SCORE;		
			}
		}		



		RakNet::BitStream *serBitstream = &serializeParameters->outputBitstream[0];
		serBitstream->WriteCompressed(serMode);

		if(serMode == 0)
		{
			return RakNet::RM3SR_DO_NOT_SERIALIZE;
		}
		if(serMode & SERMODE_SHOT)
		{
			serBitstream->Write(mShot1On);
			mPrevShot1On = mShot1On;

		}
		if(serMode & SERMODE_POSORI)
		{
			serBitstream->WriteAlignedBytes((const unsigned char*)&mPosition,sizeof(mPosition));
			serBitstream->WriteAlignedBytes((const unsigned char*)&mOrientation,sizeof(mOrientation));
			serBitstream->WriteAlignedBytes((const unsigned char*)&mVelocity,sizeof(mVelocity));
			serBitstream->WriteAlignedBytes((const unsigned char*)&mActualThrustForce,sizeof(mActualThrustForce));

			mPrevOrientation = mOrientation;
			mPrevVelocity = mVelocity;			
			mTimeLastPosOriPack = RakNet::GetTime(); 
		}
		if(serMode & SERMODE_SCORE)
		{
			serBitstream->WriteCompressed(mScoreInfo->mCurrentHealth);
			serBitstream->WriteCompressed(mScoreInfo->mDeaths);
			serBitstream->WriteCompressed(mScoreInfo->mKills);
			mLastSerializedScoreInfo = ScoresManager::PlayerScoreInfo(*mScoreInfo);
		}		
		return RakNet::RM3SR_BROADCAST_IDENTICALLY;
	}

	void Deserialize(RakNet::DeserializeParameters *deserializeParameters)
//		RakNet::BitStream *serializationBitstream,	RakNet::Time timeStamp, RakNet::Connection_RM3 *sourceConnection)
	{
		RakAssert(!mServer);

		RakNet::BitStream *serializationBitstream = deserializeParameters->serializationBitstream;	

		unsigned char serMode;
		serializationBitstream->ReadCompressed(serMode);

		if(serMode & SERMODE_SHOT)
		{
			bool shotOn;
			serializationBitstream->Read(shotOn);
			mPrevShot1On = shotOn;
			setShot1On(shotOn);
		}
		if(serMode & SERMODE_POSORI)
		{
			serializationBitstream->ReadAlignedBytes((unsigned char*)&mPosition,sizeof(mPosition));
			serializationBitstream->ReadAlignedBytes((unsigned char*)&mOrientation,sizeof(mOrientation));
			serializationBitstream->ReadAlignedBytes((unsigned char*)&mVelocity,sizeof(mVelocity));
			serializationBitstream->ReadAlignedBytes((unsigned char*)&mActualThrustForce, sizeof(mActualThrustForce));	
		}
		if(serMode & SERMODE_SCORE)
		{
			ScoresManager::PlayerScoreInfo updatedScoreInfo;
			updatedScoreInfo.mID = mScoreInfo->mID;

			serializationBitstream->ReadCompressed(updatedScoreInfo.mCurrentHealth);
			serializationBitstream->ReadCompressed(updatedScoreInfo.mDeaths);
			serializationBitstream->ReadCompressed(updatedScoreInfo.mKills);

			mScoresManager->updatePlayer(updatedScoreInfo);
		}

		if(mDead && mScoreInfo->mCurrentHealth > 0)
		{
			born(mPosition, mOrientation);
		}

	}


};

//enum Airplane_Params
//{
//	//PITCHFORCE_0 = 0,
//	//PITCHFORCE_1 = 4,
//	PITCHFORCE_2 = 6,
//
//	THRUSTFORCE_0 = 30,
//	THRUSTFORCE_1 = 60,
//	THRUSTFORCE_2 = 80,
//	THRUSTFORCE_3 = 100,
//
//	//ROLLFORCE_0 = 0,
//	//ROLLFORCE_1 = 1,	//50
//	ROLLFORCE_2 = 2		//100
//};

static const Ogre::Real	PITCHFORCE_2	= 0.06f;

static const Ogre::Real	THRUSTFORCE_0	= 0.3f;
static const Ogre::Real	THRUSTFORCE_1	= 0.6f;
static const Ogre::Real	THRUSTFORCE_2	= 0.8f;
static const Ogre::Real	THRUSTFORCE_3	= 1.0f;

static const Ogre::Real	ROLLFORCE_2	= 0.013f;

#endif