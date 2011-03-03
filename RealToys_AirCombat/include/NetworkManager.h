#ifndef _NETWORKMANAGER_H_
#define _NETWORKMANAGER_H_

#include "RealToysShared.h"
#include "AirplaneManager.h"
#include "GameLoadSaveManager.h"


enum CustomMessageIDTypes
{
	CUSTOMID_MAP_NAME = ID_USER_PACKET_ENUM,
};

class RT_RM3Connection : public RakNet::Connection_RM3 {
public:
	RT_RM3Connection(SystemAddress _systemAddress, RakNetGUID _guid,
		Ogre::SceneManager *sceneMgr, OgreNewt::World *newtWorld) 
		: RakNet::Connection_RM3(_systemAddress, _guid), 
		mSceneMgr(sceneMgr), mWorld(newtWorld) {}

	virtual ~RT_RM3Connection() {}

	virtual RakNet::Replica3 *AllocReplica(RakNet::BitStream *allocationId)
	{
		RakNet::RakString typeName;
		allocationId->Read(typeName);
		
		if (typeName==Airplane::GetName()) 
		{
			//this is only called on client-side, so systemadress will be assigned latter on Deserialize Construction
			return AirplaneManager::getSingletonPtr()->createAirplane(PlayerID());
		}
		
		return 0;
	}
private:
	Ogre::SceneManager *mSceneMgr;
	OgreNewt::World *mWorld;
};

class RT_ReplicaManager : public RakNet::ReplicaManager3
{
public:
	RT_ReplicaManager(Ogre::SceneManager *sceneMgr, OgreNewt::World *newtWorld = 0)
		:RakNet::ReplicaManager3(), mSceneMgr(sceneMgr), mWorld(newtWorld) {}

	RT_ReplicaManager(): RakNet::ReplicaManager3(), mSceneMgr(0), mWorld(0) {}

	virtual RakNet::Connection_RM3* AllocConnection(SystemAddress systemAddress,
		RakNetGUID rakNetGUID) const 
	{
		RakAssert(mSceneMgr != 0);
		return new RT_RM3Connection(systemAddress, rakNetGUID, mSceneMgr, mWorld);
	}

	virtual void DeallocConnection(RakNet::Connection_RM3 *connection) const 
	{
		delete connection;
	}

private:
	Ogre::SceneManager *mSceneMgr;
	OgreNewt::World *mWorld;
};
class NetworkManager : public Ogre::Singleton<NetworkManager>, public NetworkIDObject
{
public:
	NetworkManager(Ogre::SceneManager *sceneManager, OgreNewt::World *newtWorld);
	~NetworkManager(void);

	bool start(bool isServer, Ogre::ushort serverPort, Ogre::String serverIP);

	bool isUpAndRunning() { return upAndRunning; }
	bool isServer() { return mServer; }

	bool update();
	void addReplica(RakNet::Replica3 *replica) { mReplicaManager.Reference(replica); }

	/*
		Request a new airplane.
		This should be called only once per game.
	*/
	void requestAirplane();

	/*
		When input is received (key pressed/released)
	*/
	void applyAirplaneInput(InputActions action, bool pressed);

	void setCurrentMap(Ogre::String mapName) 
	{ 		
		if(!GameLoadSaveManager::getSingletonPtr()->LoadMap(mapName))
		{
			upAndRunning = false;				
			return;
		}		
		if(mServer)
			mRakPeer->SetMaximumIncomingConnections(RealToys::maxClients-1);	
	}

	Ogre::String getCurrentMap();

private:
	RakPeerInterface *mRakPeer;				
	NetworkIDManager mNetworkIdManager;		// RM3 requires this to lookup pointers from numbers.	
	NetworkIDManager mRPCIdManager;			// RPC3 requires us to set ids on our classes used by RPC3	
	RT_ReplicaManager mReplicaManager;		// The RM3 System
	RakNet::RPC3 mRPC3Inst;					// The RPC3 system

	Ogre::SceneManager *mSceneMgr;
	OgreNewt::World *mWorld;

	bool mIsStarted;						//after start() is called
	bool upAndRunning;						//client is connected to the server (or server is running)
	bool mConnectAtemp;						//client called RakNetPeer::Connect and is waiting to connect
	bool mWaitForPong;						//when client sends a PING and is waiting for PONG

	bool mServer;

	PlayerID mNetworkID;				// ID representing the player (0.0.0.0:0 on server)

	AirplaneManager* mAirplaneManager;


	/*
		Create a new airplane and add it to the replica system.
		This will only work on the server
	*/
	void createAirplane(RakNet::RPC3 *rpc);	

	/*
		Create a new airplane and add it to the replica system.
		This will only work on the server
	*/
	void processAirplaneInput(RakNet::RPC3 *rpc, InputActions action, bool pressed);


	void recieveCustomPacket(Packet *packet);
	


public:
	static NetworkManager& getSingleton(void);
	static NetworkManager* getSingletonPtr(void);
};

#endif