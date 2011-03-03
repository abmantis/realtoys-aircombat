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
		SERMODE_POSORI		= 1<<0,
		SERMODE_FORCES		= 1<<1,
		SERMODE_SHOT		= 1<<2,
		SERMODE_SCORE		= 1<<3,
	};

	Airplane(Ogre::SceneManager *sceneManager, OgreNewt::World *world, bool onServer);
	~Airplane(void);

	void createPlane(Ogre::Vector3 position, Ogre::Quaternion orientation);

	/*
		Sets this plane's thrust.
		@param thrust - use Airplane_Params::PITCHFORCE_X
	*/
	void setThrustForce(int thrust);
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


	int mThrustForce;				//force applied to go in front direction (thrust)
	int mActualThrustForce;			//actual force applied to go in front direction (thrust)
	int mPitchForce;				//force applied to pitch
	int mRollForce;					//force applied to roll
	int mPrevThrustForce;			//on server used only to decide to serialize or not
	int mPrevPitchForce;			//on server used only to decide to serialize or not
	int mPrevRollForce;				//on server used only to decide to serialize or not

	float mTimeToGainControll;		//time remaining to get thrust, usually after a collision
	float mTimeSinceLastUpdate;		//time since the last update to the actual on (only used by clients)
	float mTimeToBornAgain;	

	RakNetTime mTimeLastPosOriPack;
		
	Ogre::Radian mPropRot;			//propeller rotation on newton each call
	Ogre::Radian mPropBlurRot;		//propeller blur rotation on newton each call


	Ogre::Vector3 mPosition;		
	Ogre::Vector3 mPrevPosition;	//on server used only to decide to serialize or not
	Ogre::Vector3 mPrevRayPosition;	//used to prevent unnecessary raycasting
	Ogre::Vector3 mVelocity;		
	Ogre::Vector3 mPrevVelocity;	//on server used only to decide to serialize or not
	Ogre::Vector3 mOmega;
	Ogre::Vector3 mPrevOmega;		//on server used only to decide to serialize or not
	Ogre::Quaternion mOrientation;	
	Ogre::Quaternion mPrevOrientation;//on server used only to decide to serialize or not
	Ogre::Quaternion mPrevRayOrientation;//used to prevent unnecessary raycasting


	PlayerID mOwnerID;				// The client (or server) that owns this airplane

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

	void WriteAllocationID(RakNet::BitStream *allocationIdBitstream) const
	{
		allocationIdBitstream->Write(GetName());
	}

	void SerializeConstruction(RakNet::BitStream *constructionBitstream, 
		RakNet::Connection_RM3 *destinationConnection)
	{		
		RakAssert(mServer);
		std::cout << "Serializing airplane construction - " << mOwnerID.ToString() << std::endl;

		mBody->getPositionOrientation(mPosition, mOrientation);
		mVelocity = mBody->getVelocity();
	
		constructionBitstream->Write(mOwnerID);
		constructionBitstream->WriteAlignedBytes((const unsigned char*)&mPosition,sizeof(mPosition));
		constructionBitstream->WriteAlignedBytes((const unsigned char*)&mOrientation,sizeof(mOrientation));
		constructionBitstream->WriteAlignedBytes((const unsigned char*)&mVelocity,sizeof(mVelocity));
		constructionBitstream->WriteAlignedBytes((const unsigned char*)&mOmega,sizeof(mOmega));
		constructionBitstream->WriteAlignedBytes((const unsigned char*)&mThrustForce,sizeof(mThrustForce));

		mPrevOrientation = mOrientation;
		mPrevVelocity = mVelocity;
		mPrevOmega = mOmega;
		mTimeLastPosOriPack = RakNet::GetTime(); 

		return;
	}

	bool DeserializeConstruction(RakNet::BitStream *constructionBitstream, 
		RakNet::Connection_RM3 *sourceConnection)
	{
		RakAssert(!mServer);
		std::cout << "De serializing airplane construction - "; 

		constructionBitstream->Read(mOwnerID);
		constructionBitstream->ReadAlignedBytes((unsigned char*)&mPosition,sizeof(mPosition));
		constructionBitstream->ReadAlignedBytes((unsigned char*)&mOrientation,sizeof(mOrientation));
		constructionBitstream->ReadAlignedBytes((unsigned char*)&mVelocity,sizeof(mVelocity));
		constructionBitstream->ReadAlignedBytes((unsigned char*)&mOmega,sizeof(mOmega));
		constructionBitstream->ReadAlignedBytes((unsigned char*)&mThrustForce, sizeof(mThrustForce));
		std::cout << mOwnerID.ToString() << std::endl;

		//mAirplanePredictor.add(mPrevPosition, mPrevVelocity, mPrevPosition, mPrevVelocity);		//the last two params won't mather at this point (first record)

		//mPosition = mPrevPosition;
		//mVelocity = mPrevVelocity;

		createPlane(mPosition, mOrientation);

		return true;
	}

	void SerializeDestruction(RakNet::BitStream *destructionBitstream, 
		RakNet::Connection_RM3 *destinationConnection)
	{		
		std::cout << "Serializing destruction" << std::endl;
		return;
	}

	bool DeserializeDestruction(RakNet::BitStream *destructionBitstream, 
		RakNet::Connection_RM3 *sourceConnection)
	{
		std::cout << "Deserializing destruction" << std::endl;
		return true;
	}

	void DeallocReplica(RakNet::Connection_RM3 *sourceConnection)
	{
		mDestructed = true;
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
			mVelocity = mBody->getVelocity();


			if( mPrevShot1On != mShot1On )
			{
				serMode += SERMODE_SHOT;			
			}
			if( (RakNet::GetTime() - mTimeLastPosOriPack) > 500 
				|| mPrevVelocity != mVelocity || mPrevOrientation != mOrientation 
				|| mPrevOmega != mOmega)
			{
				serMode += SERMODE_POSORI;			
			}
			if( mLastSerializedScoreInfo != (*mScoreInfo))
			{
				serMode += SERMODE_SCORE;		
			}
			if( mPrevThrustForce != mThrustForce
				|| mPrevPitchForce != mPitchForce
				|| mPrevRollForce != mRollForce)
			{
				serMode += SERMODE_FORCES;
			}
		}		

		
				
		RakNet::BitStream *serBitstream = &serializeParameters->outputBitstream;
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
			serBitstream->WriteAlignedBytes((const unsigned char*)&mOmega,sizeof(mOmega));			

			mPrevOrientation = mOrientation;
			mPrevVelocity = mVelocity;
			mPrevOmega = mOmega;
			mTimeLastPosOriPack = RakNet::GetTime(); 
		}		
		if(serMode & SERMODE_SCORE)
		{
			serBitstream->WriteCompressed(mScoreInfo->mCurrentHealth);
			serBitstream->WriteCompressed(mScoreInfo->mDeaths);
			serBitstream->WriteCompressed(mScoreInfo->mKills);
			mLastSerializedScoreInfo = ScoresManager::PlayerScoreInfo(*mScoreInfo);
		}
		if(serMode & SERMODE_FORCES)
		{
			serBitstream->WriteCompressed(mThrustForce);
			serBitstream->WriteCompressed(mPitchForce);
			serBitstream->WriteCompressed(mRollForce);

			mPrevThrustForce = mThrustForce;
			mPrevRollForce = mRollForce;
			mPrevPitchForce = mPitchForce;
		}
		return RakNet::RM3SR_BROADCAST_IDENTICALLY;
	}

	
	void Deserialize(RakNet::BitStream *serializationBitstream,	
		RakNetTime timeStamp, RakNet::Connection_RM3 *sourceConnection)
	{
		RakAssert(!mServer);

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
			Ogre::Vector3 mDifPos, mDifVel, mDifOmega;

			serializationBitstream->ReadAlignedBytes((unsigned char*)&mPrevPosition,sizeof(mPrevPosition));
			serializationBitstream->ReadAlignedBytes((unsigned char*)&mPrevOrientation,sizeof(mPrevOrientation));
			serializationBitstream->ReadAlignedBytes((unsigned char*)&mPrevVelocity,sizeof(mPrevVelocity));
			serializationBitstream->ReadAlignedBytes((unsigned char*)&mPrevOmega,sizeof(mPrevOmega));

			mDifPos = mPrevPosition - mPosition;
			mDifVel = mPrevVelocity - mVelocity;
			mDifOmega = mPrevOmega - mOmega;

			std::cout << mPrevVelocity << std::endl;

			if(Ogre::Math::Abs(mDifPos.x) > 0.1f || Ogre::Math::Abs(mDifPos.y) > 0.1f 
				|| Ogre::Math::Abs(mDifPos.z) > 0.1f )
			{
				mBody->setPositionOrientation(mPrevPosition, mPrevOrientation);			
				
			}
			if(Ogre::Math::Abs(mDifVel.x) > 0.5f || Ogre::Math::Abs(mDifVel.y) > 0.5f 
				|| Ogre::Math::Abs(mDifVel.z) > 0.5f )
			{
				mBody->setVelocity(mPrevVelocity);
				std::cout << std::endl;
			}
			if(Ogre::Math::Abs(mDifOmega.x) > 0.5f 	|| Ogre::Math::Abs(mDifOmega.y) > 0.5f 
				|| Ogre::Math::Abs(mDifOmega.z) > 0.5f 	)
			{
				mBody->setOmega(mPrevOmega);
			}

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
		if(serMode & SERMODE_FORCES)
		{
			serializationBitstream->ReadCompressed(mThrustForce);
			serializationBitstream->ReadCompressed(mPitchForce);
			serializationBitstream->ReadCompressed(mRollForce);

		}

		if(mDead && mScoreInfo->mCurrentHealth > 0)
		{
			born(mPosition, mOrientation);
		}
	}

	//void SerializeConstructionRequestAccepted(RakNet::BitStream *serializationBitstream,
	//	RakNet::Connection_RM3 *requestingConnection);

	//void DeserializeConstructionRequestAccepted(RakNet::BitStream *serializationBitstream, 
	//	RakNet::Connection_RM3 *acceptingConnection);

	//void SerializeConstructionRequestRejected(RakNet::BitStream *serializationBitstream,
	//	RakNet::Connection_RM3 *requestingConnection);

	//void DeserializeConstructionRequestRejected(RakNet::BitStream *serializationBitstream,
	//	RakNet::Connection_RM3 *rejectingConnection);

	RakNet::RM3ConstructionState QueryConstruction(RakNet::Connection_RM3 *destinationConnection, 
		RakNet::ReplicaManager3 *replicaManager3) 
	{
		return QueryConstruction_ServerConstruction(destinationConnection);
	}
	bool QueryRemoteConstruction(RakNet::Connection_RM3 *sourceConnection) 
	{
		return QueryRemoteConstruction_ServerConstruction(sourceConnection);
	}

	bool QuerySerialization(RakNet::Connection_RM3 *destinationConnection)
	{
		return QuerySerialization_ServerSerializable(destinationConnection);
	}
	
};

enum Airplane_Params
{
	PITCHFORCE_0 = 0,
	PITCHFORCE_1 = 200,
	PITCHFORCE_2 = 400,

	THRUSTFORCE_0 = 3000,
	THRUSTFORCE_1 = 5000,
	THRUSTFORCE_2 = 7000,
	THRUSTFORCE_3 = 10000,

	ROLLFORCE_0 = 0,
	ROLLFORCE_1 = 50,
	ROLLFORCE_2 = 100
};

#endif