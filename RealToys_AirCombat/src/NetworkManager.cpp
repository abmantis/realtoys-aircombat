#include "StdAfx.h"
#include "NetworkManager.h"


template<> NetworkManager* Ogre::Singleton<NetworkManager>::ms_Singleton = 0;

NetworkManager::NetworkManager(Ogre::SceneManager *sceneManager, OgreNewt::World *newtWorld)
:mSceneMgr(sceneManager), mWorld(newtWorld)
{
	mIsStarted = false;
}

bool NetworkManager::start(bool isServer, Ogre::ushort serverPort, Ogre::String serverIP)	
{
	mServer = isServer; 	
	RakNetTime waitTime = 5000;
	RakNetTime prevTime = 0;
	if(isServer)
	{
		Ogre::LogManager::getSingletonPtr()
			->logMessage(RealToys::logMessagePrefix + "Starting network as " 
			+ "server on port " + Ogre::StringConverter::toString(serverPort) );
	}
	else
	{
		Ogre::LogManager::getSingletonPtr()
			->logMessage(RealToys::logMessagePrefix + "Starting network as " 
			+ "client to server " + serverIP + " on port " + 
			Ogre::StringConverter::toString(serverPort) );
	}

	mAirplaneManager = AirplaneManager::getSingletonPtr();

	upAndRunning = false;
	mConnectAtemp = false;
	mWaitForPong = false;

	SocketDescriptor sd;
	mRakPeer = RakNetworkFactory::GetRakPeerInterface();
	
	if(isServer)
	{
		sd.port = serverPort;
		mReplicaManager = RT_ReplicaManager(mSceneMgr, mWorld);
	}
	else
	{
		sd.port = 0;
		mReplicaManager = RT_ReplicaManager(mSceneMgr);
	}
	mReplicaManager.SetDefaultPacketReliability(UNRELIABLE_SEQUENCED);
	mReplicaManager.SetDefaultPacketPriority(HIGH_PRIORITY);
	

	// The network ID authority is the system that creates the common numerical identifier used to lookup pointers (the server here).
	mNetworkIdManager.SetIsNetworkIDAuthority(isServer);
	// ObjectMemberRPC, AutoRPC for objects, and ReplicaManager3 require that you call SetNetworkIDManager()
	mRakPeer->SetNetworkIDManager(&mNetworkIdManager);

	// Setup RPC3 system and classes 
	mRPCIdManager.SetIsNetworkIDAuthority(true);
	mRPC3Inst.SetNetworkIDManager(&mRPCIdManager);
	
	this->SetNetworkIDManager(&mRPCIdManager);

	NetworkID id0;
	id0.localSystemAddress = 0;
	this->SetNetworkID(id0);
	RPC3_REGISTER_FUNCTION(&mRPC3Inst, &NetworkManager::createAirplane);
	RPC3_REGISTER_FUNCTION(&mRPC3Inst, &NetworkManager::processAirplaneInput);
	
		
	// Start RakNet, up to 32 connections if the server
	if(!mRakPeer->Startup(isServer ? (RealToys::maxClients-1) : 1, 0, &sd, 1))
	{
		if(isServer)
		{
			Ogre::LogManager::getSingletonPtr()
				->logMessage(RealToys::logMessagePrefix 
				+ "Peer startup failed (another server running?)");
		}
		else
		{
			Ogre::LogManager::getSingletonPtr()
				->logMessage(RealToys::logMessagePrefix + "Peer startup failed");
		}		
		return false;
	}
	
	
	mRakPeer->AttachPlugin(&mReplicaManager);
	mRakPeer->AttachPlugin(&mRPC3Inst);

	// The server should allow systems to connect. Clients do not need to unless we want to transmit messages directly between systems or to use RakVoice
	if (isServer)
	{
//		mRakPeer->SetMaximumIncomingConnections(RealToys::maxClients-1);
		mRakPeer->SetMaximumIncomingConnections(0);		//will not accept connections until setCurrentMap is called
		mNetworkID = RealToys::serverPlayerID;		
	}
	else
	{
		if(serverIP == "255.255.255.255")
		{
			if(mRakPeer->Ping( serverIP.c_str(), serverPort, true, 0 ) )
			{
				Ogre::LogManager::getSingletonPtr()
					->logMessage(RealToys::logMessagePrefix + "Client will try to search for servers on LAN");
				mWaitForPong = true;
			}
			else
			{
				Ogre::LogManager::getSingletonPtr()
					->logMessage(RealToys::logMessagePrefix + "Client PING failed");
				return false;
			}
		}
		else
		{
			if(!mRakPeer->Connect(serverIP.c_str(), serverPort, 0, 0, 0))
			{
				Ogre::LogManager::getSingletonPtr()
					->logMessage(RealToys::logMessagePrefix + "Client peer connect failed");
				return false;
			}
			mConnectAtemp = true;
		}
		
	}

	mIsStarted = true;
	if(!isServer)
	{
		Ogre::LogManager::getSingletonPtr()
			->logMessage(RealToys::logMessagePrefix + "Trying to connect to server" );
		//"update" until receives a package saying if it was a successful connection or not
		prevTime = RakNet::GetTime();		
		while(!upAndRunning && (mConnectAtemp || mWaitForPong) && waitTime > 0)
		{
			update();
			waitTime-=(RakNet::GetTime()-prevTime);
			prevTime = RakNet::GetTime();
		}
	}
	else
	{
		upAndRunning = true;
	}

	
	if(upAndRunning)
	{
		mAirplaneManager->setPlayerID(mNetworkID);
		ScoresManager::getSingletonPtr()->setLocalSysAddress(mNetworkID);

		Ogre::String addrs( mNetworkID.ToString() );
		if(mServer)
			addrs += " (server)";
		Ogre::LogManager::getSingletonPtr()
			->logMessage(RealToys::logMessagePrefix + "Network started " + addrs);
	}
	else
	{
		Ogre::LogManager::getSingletonPtr()
			->logMessage(RealToys::logMessagePrefix + "Network failed to start" );
	}
	return upAndRunning;
}

NetworkManager::~NetworkManager(void)
{
	Ogre::LogManager::getSingletonPtr()
		->logMessage(RealToys::logMessagePrefix + "Connection shutting down (destroying)");
	mRakPeer->Shutdown(100,0, HIGH_PRIORITY);
	RakNetworkFactory::DestroyRakPeerInterface(mRakPeer);
}

NetworkManager& NetworkManager::getSingleton(void)
{
	assert( ms_Singleton );  return ( *ms_Singleton );  
}
NetworkManager* NetworkManager::getSingletonPtr(void)
{
	return ms_Singleton;
}

bool NetworkManager::update()
{
	if(!mIsStarted)
		return true;

	Packet *packet;	
	for (packet = mRakPeer->Receive(); packet; 
		mRakPeer->DeallocatePacket(packet), packet = mRakPeer->Receive())
	{
		switch (packet->data[0])
		{
		case ID_CONNECTION_ATTEMPT_FAILED:
			Ogre::LogManager::getSingletonPtr()->logMessage(RealToys::logMessagePrefix +
				"ID_CONNECTION_ATTEMPT_FAILED", Ogre::LML_CRITICAL );
			upAndRunning = false;
			mConnectAtemp = false;
			return false;
			break;
		case ID_NO_FREE_INCOMING_CONNECTIONS:
			Ogre::LogManager::getSingletonPtr()->logMessage(RealToys::logMessagePrefix +
				"ID_NO_FREE_INCOMING_CONNECTIONS", Ogre::LML_CRITICAL);
			upAndRunning = false;
			mConnectAtemp = false;
			return false;
			break;
		case ID_CONNECTION_REQUEST_ACCEPTED:
			Ogre::LogManager::getSingletonPtr()->logMessage(RealToys::logMessagePrefix +
				"ID_CONNECTION_REQUEST_ACCEPTED" );
			if(!mServer)
			{
				//this is a client, get the client external address relative to server
				mNetworkID = mRakPeer->GetExternalID(packet->systemAddress);
				mConnectAtemp = false;
			}
			upAndRunning = true;
			return false;
			break;
		case ID_NEW_INCOMING_CONNECTION:
			{
				Ogre::LogManager::getSingletonPtr()->logMessage(RealToys::logMessagePrefix +
					"ID_NEW_INCOMING_CONNECTION from " + packet->systemAddress.ToString() ); 
				//send current map name
				RakNet::BitStream bStream;
				RakNet::RakString mapName(GameLoadSaveManager::getSingletonPtr()->GetMapName().c_str());
				RakNet::RakString mapVersion(GameLoadSaveManager::getSingletonPtr()->GetMapVersion().c_str());
								
				
				bStream.Write((MessageID)CUSTOMID_MAP_NAME);
				bStream.Write(mapName);
				bStream.Write(mapVersion);
				mRakPeer->Send(&bStream, LOW_PRIORITY, RELIABLE, 0, packet->systemAddress, false);
				break;
			}
		case ID_DISCONNECTION_NOTIFICATION:
			if(mServer)
			{
				Ogre::String id = packet->systemAddress.ToString();
				Ogre::LogManager::getSingletonPtr()->logMessage(RealToys::logMessagePrefix +
					"ID_DISCONNECTION_NOTIFICATION from client " + id );
				mAirplaneManager->destroyAirplane(packet->systemAddress);
					
			}
			else
			{
				mConnectAtemp = false;
				Ogre::LogManager::getSingletonPtr()->logMessage(RealToys::logMessagePrefix +
					"ID_DISCONNECTION_NOTIFICATION from server " + packet->systemAddress.ToString() 
					, Ogre::LML_CRITICAL);
				upAndRunning = false;
				return false;
			}
			break;
		case ID_CONNECTION_LOST:
			if(mServer)
			{
				Ogre::String id = packet->systemAddress.ToString();
				Ogre::LogManager::getSingletonPtr()->logMessage(RealToys::logMessagePrefix +
					"ID_CONNECTION_LOST from client " + id,
					Ogre::LML_CRITICAL);
				mAirplaneManager->destroyAirplane(packet->systemAddress);
			}
			else
			{
				mConnectAtemp = false;
				Ogre::LogManager::getSingletonPtr()->logMessage(RealToys::logMessagePrefix +
					"ID_CONNECTION_LOST from server " + packet->systemAddress.ToString(), 
					Ogre::LML_CRITICAL);
				upAndRunning = false;
				return false;
			}
			break;
		case ID_PONG:
			// Found the server
			if(!upAndRunning && mWaitForPong)			
			{
				mWaitForPong = false;
				if(!mRakPeer->Connect(packet->systemAddress.ToString(false),packet->systemAddress.port,0,0,0))
				{
					Ogre::LogManager::getSingletonPtr()
						->logMessage(RealToys::logMessagePrefix + "Client peer connect failed");
					mConnectAtemp = false;				
					upAndRunning = false;
					return false;
				}
				mConnectAtemp = true;				
			}
			break;
		default:
			recieveCustomPacket(packet);
			break;
		}
	}
	
	return upAndRunning;
}


void NetworkManager::recieveCustomPacket(Packet *packet)
{
	RakNet::BitStream myBitStream(packet->data, packet->length, false); // The false is for efficiency so we don't make a copy of the passed data
	unsigned char typeId;

	myBitStream.Read(typeId);
	

	switch (typeId)
	{
	case CUSTOMID_MAP_NAME:
		{
			if(!mServer)
			{				
				Ogre::LogManager::getSingletonPtr()
					->logMessage(RealToys::logMessagePrefix + "Received map name");
				RakNet::RakString mapName, mapVersion, mapXmlFile;
				myBitStream.Read(mapName);	
				myBitStream.Read(mapVersion);
								
				if(!GameLoadSaveManager::getSingletonPtr()->LoadMap(mapName.C_String(),
					mapVersion.C_String()) )
				{
					upAndRunning = false;					
				}
			}
			break;
		}
	default:
		break;
	}
}

void NetworkManager::createAirplane(RakNet::RPC3 *rpc)
{
	if(!mServer)
		return;
	Airplane *plane;
	if(rpc)
	{
		//called remotely from a client
		Ogre::LogManager::getSingletonPtr()
			->logMessage(RealToys::logMessagePrefix + "Airplane creation requested (from remote source)" );
		if(mAirplaneManager->canHaveMoreAirplanes())
		{
			plane = mAirplaneManager->createAirplane(rpc->GetLastSenderAddress());
			mReplicaManager.Reference(plane);
		}
		else
		{
			//SHOULD SEND MESSAGE BACK WARNING THAT SERVER IS FULL
		}
	}
	else
	{
		//called locally on the server
		Ogre::LogManager::getSingletonPtr()
			->logMessage(RealToys::logMessagePrefix + "Airplane creation requested (from local source)" );

		if(mAirplaneManager->canHaveMoreAirplanes())
		{
			plane = mAirplaneManager->createAirplane(mNetworkID);
			mReplicaManager.Reference(plane);
		}
		else
		{
			//SHOULD SHOW WARNING THAT IS FULL
		}
	}	
}

void NetworkManager::requestAirplane()
{
	if(mServer)
	{		
		createAirplane(0);
	}
	else
	{
		mRPC3Inst.CallCPP("&NetworkManager::createAirplane", this->GetNetworkID());
	}
}

void NetworkManager::processAirplaneInput(RakNet::RPC3 *rpc, InputActions action, bool pressed)
{
	if(!mServer)
		return;

	if(rpc)
	{
		mAirplaneManager->applyInput(rpc->GetLastSenderAddress(), action, pressed);
	}
	else
		mAirplaneManager->applyInput(mNetworkID, action, pressed);		
}
void NetworkManager::applyAirplaneInput(InputActions action, bool pressed)
{
	if(mServer)
	{		
		processAirplaneInput(0, action, pressed);
	}
	else
	{		
		mRPC3Inst.CallCPP("&NetworkManager::processAirplaneInput", this->GetNetworkID(), action, pressed);
	}
}

