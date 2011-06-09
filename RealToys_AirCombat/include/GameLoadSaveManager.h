#ifndef _GameLoadSaveManager_H_
#define _GameLoadSaveManager_H_

#if OGRE_PLATFORM == PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#include <direct.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#endif
#include <libxml/xmlreader.h>
#include "RealToysShared.h"
#include "ModelsManager.h"
#include "WallManager.h"
#include "MainStructManager.h"
#include "AirplaneMarkersManager.h"
//#include "XMLFile.h"

#define MY_ENCODING "ISO-8859-1"

class GameLoadSaveManager: public Ogre::Singleton<GameLoadSaveManager>
{
public:
	GameLoadSaveManager(Ogre::SceneManager *sceneManager);
	~GameLoadSaveManager(void);

	void SetMapsPath(Ogre::String path) { mMapsPath = path; }

	bool LoadMap(Ogre::String fileName, Ogre::String validateVersion = "");

	bool IsMapLoaded() { return mMapLoaded; }

	Ogre::String GetMapName() { return mMapName; }

	Ogre::String GetMapVersion() { return mMapVersion; }
	

	/** Override standard Singleton retrieval.
	@remarks
	Why do we do this? Well, it's because the Singleton
	implementation is in a .h file, which means it gets compiled
	into anybody who includes it. This is needed for the
	Singleton template to work, but we actually only want it
	compiled into the implementation of the class based on the
	Singleton, not all of them. If we don't change this, we get
	link errors when trying to use the Singleton-based class from
	an outside dll.
	*/
	static GameLoadSaveManager& getSingleton(void);
	/** Override standard Singleton retrieval.
	@remarks
	Why do we do this? Well, it's because the Singleton
	implementation is in a .h file, which means it gets compiled
	into anybody who includes it. This is needed for the
	Singleton template to work, but we actually only want it
	compiled into the implementation of the class based on the
	Singleton, not all of them. If we don't change this, we get
	link errors when trying to use the Singleton-based class from
	an outside dll.
	*/
	static GameLoadSaveManager* getSingletonPtr(void);

private:
	Ogre::SceneManager *mSceneManager;
	MainStructManager *mMainStructMan;
	ModelsManager *mModelsMan;
	WallManager *mWallMan;

	Ogre::String mMapsPath;
	Ogre::String mMapName;
	Ogre::String mMapVersion;
	bool mMapLoaded;
	/*
		Loads resources from the map zip file
	*/
	bool LoadResourcesFromMapZip(Ogre::String path, Ogre::String file);
	/*
		Reads the XML files that hold the scene structure (models, walls, etc).
		@Attention! This function reads the files from Ogre ResourceGroupManager,
		so the files should be loaded into the resource manager first, to the correct group
	*/
	bool ReadSceneFile(Ogre::String mapName, Ogre::String validateVersion = "");
	/*
		Reads main struct attributes
	*/
	bool ReadMainStructAttributes(xmlNodePtr mainNode);

	/*
	*/
	bool ReadDecoModels(xmlNodePtr parent, Ogre::String mapName);

	/*
	*/
	bool ReadWalls(xmlNodePtr parent);

	/*
	*/
	bool ReadAirplaneMarkers(xmlNodePtr parent);

	bool ReadSceneNodeInfoAttributes(xmlNodePtr scenenode, Ogre::Vector3 &position,
		Ogre::Vector3 &scale, Ogre::Quaternion &orientation);

	bool ReadPhysicsInfoAttributes(xmlNodePtr physicsnode, 
		ModelsManager::PhysicsBodyDataStructure &physicsDataStructure);
	
	/**
	* ConvertInput:
	* @in: string in a given encoding
	* @encoding: the encoding used
	*
	* Converts @in into UTF-8 for processing with libxml2 APIs
	*
	* Returns the converted UTF-8 string, or NULL in case of error.
	*/
	xmlChar * ConvertInput(const char *in, const char *encoding);

	bool checkError(int rc, Ogre::String operation, Ogre::String tag_atribute);

	xmlNodePtr findChildByName(const xmlNodePtr parent, std::string name);

};
#endif