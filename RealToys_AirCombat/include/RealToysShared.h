#ifndef REALTOYSSHARED_H
#define REALTOYSSHARED_H

#define PlayerID RakNet::RakNetGUID //easy to change ID type

namespace RealToys
{

#pragma region constantes
	static const Ogre::Real		wallHeight					=	250;

	static const int			maxClients					=	10;


	static const Ogre::String	mapResourceGroup			=	"MapResGroup";
	static const Ogre::String	configFileName				=	"config.cfg";
	static const Ogre::String	unitCubeMeshName			=	"UnitCubeMesh.mesh";

	static const Ogre::String	wallsMasterSceneNode		=	"WallsMasterNode";
	static const Ogre::String	modelsMasterSceneNode		=	"ModelsMasterNode";
	static const Ogre::String	mainStructMasterSceneNode	=	"MainStructMasterNode";	

	static const Ogre::String	logMessagePrefix			=	"[**REALTOYS_AC LOG**] ";

	//static const PlayerID		serverPlayerID				=	RakNet::SystemAddress( "0.0.0.0", 0);
#pragma endregion

	enum QueryFlags
	{
		MASK_NONE			= 0,

		MASK_MAINFLOOR		= 1<<0,
		MASK_MAINCEILING	= 1<<1,
		MASK_MAINWALL		= 1<<2,

		MASK_WALL			= 1<<3,
		
		MASK_DECOMODEL		= 1<<4,

		MASK_AIRPLANE		= 1<<5
	};

	enum BodyType 
	{ 		
		BODYTYPE_MAINSTRUCT,
		BODYTYPE_DECOMODEL,
		BODYTYPE_WALL,
		BODYTYPE_AIRPLANE,
		BODYTYPE_SHOT1
	};

};

#endif