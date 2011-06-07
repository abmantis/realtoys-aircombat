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

	static const Ogre::Real			OgreNewtonFactor			=	100.0f;
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

	static Ogre::Vector3 ToNewton(const Ogre::Vector3 vec)
	{
		return vec / OgreNewtonFactor;
	}
	static Ogre::Quaternion ToNewton(const Ogre::Quaternion quat)
	{
		/*return Ogre::Quaternion(quat.w / OgreNewtonFactor,
			quat.x / OgreNewtonFactor,
			quat.y / OgreNewtonFactor,
			quat.z / OgreNewtonFactor);*/
		return quat;
	}
	static void ToNewton( Ogre::Node *node)
	{
		// let's scale the object to newton's units so the collision is created correctly
		node->scale(RealToys::ToNewton(Ogre::Vector3::UNIT_SCALE));
	}
	static Ogre::Real ToNewton( const Ogre::Real val)
	{
		// let's scale the object to newton's units so the collision is created correctly
		return val / OgreNewtonFactor;
	}

	static Ogre::Vector3 FromNewton(const Ogre::Vector3 vec)
	{
		return vec * OgreNewtonFactor;
	}
	static Ogre::Quaternion FromNewton(const Ogre::Quaternion quat)
	{
		return quat;// * OgreNewtonFactor;
	}
	static void FromNewton( Ogre::Node *node)
	{
		// let's scale the object to newton's units so the collision is created correctly
		node->scale(RealToys::FromNewton(Ogre::Vector3::UNIT_SCALE));
	}
	static Ogre::Real FromNewton( const Ogre::Real val)
	{
		// let's scale the object to newton's units so the collision is created correctly
		return val * OgreNewtonFactor;
	}
};

#endif