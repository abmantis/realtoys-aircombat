#include "StdAfx.h"
#include "GameLoadSaveManager.h"


template<> GameLoadSaveManager* Ogre::Singleton<GameLoadSaveManager>::ms_Singleton = 0;
GameLoadSaveManager& GameLoadSaveManager::getSingleton(void)
{
	assert( ms_Singleton );  return ( *ms_Singleton );  
}
GameLoadSaveManager* GameLoadSaveManager::getSingletonPtr(void)
{
	return ms_Singleton;
}
GameLoadSaveManager::GameLoadSaveManager(Ogre::SceneManager *sceneManager):
	mSceneManager(sceneManager)
{
	mMapVersion = "";
	mMapName = "";
	mMapLoaded = false;
	mMainStructMan = MainStructManager::getSingletonPtr();
	mModelsMan = ModelsManager::getSingletonPtr();
	mWallMan = WallManager::getSingletonPtr();
}

GameLoadSaveManager::~GameLoadSaveManager(void)
{
}

bool GameLoadSaveManager::LoadMap(Ogre::String fileName, Ogre::String validateVersion )
{	
	Ogre::LogManager::getSingletonPtr()->logMessage(RealToys::logMessagePrefix 
		+ "Loading map " + fileName);

	mMapName= fileName;

	mModelsMan->clearAll();
	mWallMan->clearAll();

	mMapLoaded = false;
	mMapVersion = "";

	

	if(!LoadResourcesFromMapZip(mMapsPath, fileName))
	{
		Ogre::LogManager::getSingletonPtr()->logMessage(RealToys::logMessagePrefix 
			+ "Map loading failed!");
		return false;
	}	

	if(!ReadSceneFile(fileName, validateVersion))
	{
		Ogre::LogManager::getSingletonPtr()->logMessage(RealToys::logMessagePrefix 
			+ "Map loading failed!");
		return false;
	}	

	Ogre::LogManager::getSingletonPtr()->logMessage(RealToys::logMessagePrefix 
		+ "DONE loading map");
	mMapLoaded = true;
	return true;
}
bool GameLoadSaveManager::LoadResourcesFromMapZip(Ogre::String path, Ogre::String file)
{
	Ogre::String filepath = path + "/" + file;
	Ogre::ResourceGroupManager *rgm = Ogre::ResourceGroupManager::getSingletonPtr();

	try
	{
		rgm->destroyResourceGroup(RealToys::mapResourceGroup);
	}
	catch (Ogre::Exception e)
	{
	}

	try
	{
		rgm->addResourceLocation(filepath, "Zip", RealToys::mapResourceGroup);

		rgm->initialiseResourceGroup(RealToys::mapResourceGroup);
	}
	catch (Ogre::Exception e)
	{
		return false;
	}
	return true;
}
bool GameLoadSaveManager::ReadSceneFile(Ogre::String mapName, Ogre::String validateVersion)
{
	xmlDocPtr doc = NULL; //- result of the xml document
	xmlNodePtr root = NULL; //- pointer to the root tag of the xml doc
	xmlNodePtr xml_node = NULL;
	Ogre::String str;

	try
	{
		//get the xml file stream(converted to str) containing the name of the scene file, from the resource group manage
		Ogre::String xmlStr = Ogre::ResourceGroupManager::getSingletonPtr()
			->openResource("mapInfo.xml",RealToys::mapResourceGroup, false)->getAsString();
		//opens the xml file (from memory)
		doc = xmlReadMemory(xmlStr.c_str(),(int) xmlStr.length(), "mapInfo.xml", NULL, 0);
		if(doc == NULL){ return false; }
		root = xmlDocGetRootElement(doc);


		//gets the node that specifies the map version
		xml_node = findChildByName(root, "Version");
		if(xml_node == NULL )
			return false;
		mMapVersion = (char*)xmlNodeGetContent(xml_node);

		if( validateVersion != "" && mMapVersion != validateVersion )
		{
			Ogre::LogManager::getSingletonPtr()->logMessage(RealToys::logMessagePrefix 
				+ "Map version is wrong! (invalid)");
			return false;
		}

		//gets the node that specifies the scene file name
		xml_node = findChildByName(root, "SceneFile");
		if(xml_node == NULL )
			return false;
		str = (char*)xmlNodeGetContent(xml_node);


		xmlFreeDoc(doc);	//this document is not used anymore, we can free it.

		//get the scene xml file stream(converted to str) , from the resource group manage
		xmlStr = Ogre::ResourceGroupManager::getSingletonPtr()
			->openResource(str, RealToys::mapResourceGroup, false)->getAsString();	

		//opens the xml file (from memory)
		doc = xmlReadMemory(xmlStr.c_str(), (int)xmlStr.length(), str.c_str(), NULL, 0);
		if(doc == NULL){ return false; }
		root = xmlDocGetRootElement(doc);

		if(!ReadMainStructAttributes(root))
		{
			Ogre::LogManager::getSingletonPtr()->logMessage(RealToys::logMessagePrefix 
				+ "Failed reading main struct attributes");
			return false;
		}
		if(!ReadAirplaneMarkers(root))
		{
			Ogre::LogManager::getSingletonPtr()->logMessage(RealToys::logMessagePrefix 
				+ "Failed reading airplane markers");
			return false;
		}
		if(AirplaneMarkersManager::getSingletonPtr()->getMarkerCount() < 2)	//make sure the map has at least 2 markers
		{
			Ogre::LogManager::getSingletonPtr()->logMessage(RealToys::logMessagePrefix 
				+ "Invalid map: less than two airplane markers");
			return false;
		}
		if(!ReadWalls(root))
		{
			Ogre::LogManager::getSingletonPtr()->logMessage(RealToys::logMessagePrefix 
				+ "Failed reading walls");
			return false;
		}
		if(!ReadDecoModels(root, mapName))
		{
			Ogre::LogManager::getSingletonPtr()->logMessage(RealToys::logMessagePrefix 
				+ "Failed reading deco models");
			return false;
		}
		
		xmlFreeDoc(doc);		//free document resources
	}
	catch (Ogre::Exception e)
	{
		return false;
	}
	

	return true;
}
bool GameLoadSaveManager::ReadMainStructAttributes(xmlNodePtr mainNode)
{
	Ogre::LogManager::getSingletonPtr()->logMessage(RealToys::logMessagePrefix 
		+ "Reading main struct attributes from map");
	Ogre::String str, floorTex, ceilingTex, mainWall1Tex, mainWall2Tex, mainWall3Tex, mainWall4Tex;
	Ogre::Vector2 sceneSize;
	xmlChar* attrRet;
	
	attrRet = xmlGetProp(mainNode, (xmlChar*)"SceneSize");
	if(attrRet == NULL)	return false;
	str = (char*)attrRet;
	sceneSize = Ogre::StringConverter::parseVector2(str);

	attrRet = xmlGetProp(mainNode, (xmlChar*)"FloorTexture");
	if(attrRet == NULL)	return false;
	floorTex = (char*)attrRet;

	attrRet = xmlGetProp(mainNode, (xmlChar*)"CeilingTexture");
	if(attrRet == NULL)	return false;
	ceilingTex = (char*)attrRet;
	
	attrRet = xmlGetProp(mainNode, (xmlChar*)"MainWall1Texture");
	if(attrRet == NULL)	return false;
	mainWall1Tex = (char*)attrRet;
	
	attrRet = xmlGetProp(mainNode, (xmlChar*)"MainWall2Texture");
	if(attrRet == NULL)	return false;
	mainWall2Tex = (char*)attrRet;
	
	attrRet = xmlGetProp(mainNode, (xmlChar*)"MainWall3Texture");
	if(attrRet == NULL)	return false;
	mainWall3Tex = (char*)attrRet;
	
	attrRet = xmlGetProp(mainNode, (xmlChar*)"MainWall4Texture");
	if(attrRet == NULL)	return false;
	mainWall4Tex = (char*)attrRet;

	//set textures
	mMainStructMan->SetTextures(floorTex, ceilingTex, mainWall1Tex, mainWall2Tex, mainWall3Tex, mainWall4Tex);

	//set main struct size
	mMainStructMan->SetSize(sceneSize);


	return true;
}


bool GameLoadSaveManager::ReadDecoModels(xmlNodePtr parent, Ogre::String mapName)
{
	Ogre::LogManager::getSingletonPtr()->logMessage(RealToys::logMessagePrefix 
		+ "Reading deco models from map");
	xmlNodePtr child, top_scene_node, no = findChildByName(parent, "DecoModel");
	if(!no)
		return true;
	no = no->children;
	Ogre::String mesh, str;
	Ogre::Vector3 position1, position2, scale1, scale2;
	Ogre::Quaternion orientation1, orientation2;
	xmlChar* attrRet;
	

	mModelsMan->clearAll();

	for(; no; no = no->next)	//for each DecoModel node
	{
		ModelsManager::PhysicsBodyDataStructure physicsDataStrct;

		while(no && no->type != XML_ELEMENT_NODE){no=no->next;}		//ignores "text" nodes
		if(!no) break;

		attrRet = xmlGetProp(no, (xmlChar*)"Mesh");
		if(attrRet == NULL) return false;
		mesh = (char*)attrRet;

		//get the first children (will be parent on the scene)
		child = no->children;
		while(child && child->type != XML_ELEMENT_NODE)
			{child= child->next;}	//ignores "text" nodes
		
		if(child)
		{
			top_scene_node = child;

			if(!ReadSceneNodeInfoAttributes(child, position1, scale1, orientation1))
				return false;

			//get the second children (will be grandpa on the scene)
			child = child->children;
			while(child && child->type != XML_ELEMENT_NODE)
				{child= child->next;}	//ignores "text" nodes
			
			if(child)
			{
				if(!ReadSceneNodeInfoAttributes(child, position2, scale2, orientation2))
					return false;
			}	
		}
		
		//get the physics node
		child = top_scene_node->next;
		while(child && child->type != XML_ELEMENT_NODE)
		{child= child->next;}	//ignores "text" nodes

		if(child)
		{
			if(!ReadPhysicsInfoAttributes(child, physicsDataStrct))
				physicsDataStrct = ModelsManager::PhysicsBodyDataStructure();
		}

		mModelsMan->addModel(mesh, position1, scale1, orientation1, 
			position2, scale2, orientation2, mapName, physicsDataStrct);
	}	
	return true;
}
bool GameLoadSaveManager::ReadWalls(xmlNodePtr parent)
{
	Ogre::LogManager::getSingletonPtr()->logMessage(RealToys::logMessagePrefix 
		+ "Reading walls from map");
	xmlNodePtr child, no = findChildByName(parent, "Wall");
	if(!no)
		return true;
	no = no->children;
	Ogre::String tex1, tex2, tex3, tex4, str;
	Ogre::Vector3 position, scale;
	Ogre::Quaternion orientation;
	xmlChar* attRet;

	mWallMan->clearAll();

	for(; no; no = no->next)	//for each DecoModel node
	{
		while(no && no->type != XML_ELEMENT_NODE){no=no->next;}		//ignores "text" nodes
		if(!no) break;

		//get texture names
		attRet = xmlGetProp(no, (xmlChar*)"Texture1");
		if(attRet == NULL) return false;		
		tex1 = (char*)attRet;

		attRet = xmlGetProp(no, (xmlChar*)"Texture2");
		if(attRet == NULL) return false;
		tex2 = (char*)attRet;

		attRet = xmlGetProp(no, (xmlChar*)"Texture3");
		if(attRet == NULL) return false;
		tex3 = (char*)attRet;

		attRet = xmlGetProp(no, (xmlChar*)"Texture4");
		if(attRet == NULL) return false;
		tex4 = (char*)attRet;

		//get the children (will be parent on the scene)
		child = no->children;
		while(child && child->type != XML_ELEMENT_NODE)
			{child= child->next;}	//ignores "text" nodes
		
		if(child)
		{
			if(!ReadSceneNodeInfoAttributes(child, position, scale, orientation))
				return false;

			mWallMan->addWall(tex1, tex2, tex3, tex4, position, scale, orientation);
				
		}		
	}	
	return true;
}
bool GameLoadSaveManager::ReadAirplaneMarkers(xmlNodePtr parent)
{
	Ogre::LogManager::getSingletonPtr()->logMessage(RealToys::logMessagePrefix 
		+ "Reading airplane markers from map");
	xmlNodePtr no = findChildByName(parent, "AirpMarker");
	if(!no)
		return true;
	no = no->children;
	Ogre::Vector3 position, scale;
	Ogre::Quaternion orientation;


	AirplaneMarkersManager::getSingletonPtr()->clearAll();

	for(; no; no = no->next)	//for each marker node
	{
		while(no && no->type != XML_ELEMENT_NODE){no=no->next;}		//ignores "text" nodes
		if(!no) break;

		if(!ReadSceneNodeInfoAttributes(no, position, scale, orientation))
			return false;

		AirplaneMarkersManager::getSingletonPtr()->addMarker(position,orientation);		
	}	
	return true;
}

bool GameLoadSaveManager::ReadSceneNodeInfoAttributes(xmlNodePtr scenenode, Ogre::Vector3 &position,
		Ogre::Vector3 &scale, Ogre::Quaternion &orientation)
{
	Ogre::String str;
	xmlChar* attRet;
	
	attRet = xmlGetProp(scenenode, (xmlChar*)"Position");
	if(attRet == NULL) return false;
	str = (char*)attRet;
	position = Ogre::StringConverter::parseVector3(str);

	attRet = xmlGetProp(scenenode, (xmlChar*)"Scale");
	if(attRet == NULL) return false;
	str = (char*)attRet;
	scale = Ogre::StringConverter::parseVector3(str);

	attRet = xmlGetProp(scenenode, (xmlChar*)"Orientation");
	if(attRet == NULL) return false;
	str = (char*)attRet;
	orientation = Ogre::StringConverter::parseQuaternion(str);

	return true;
}

bool GameLoadSaveManager::ReadPhysicsInfoAttributes(xmlNodePtr physicsnode, 
	ModelsManager::PhysicsBodyDataStructure &physicsDataStructure)
{
	Ogre::String str;
	xmlChar* attRet;

	attRet = xmlGetProp(physicsnode, (xmlChar*)"Type");
	if(attRet == NULL) return false;
	str = (char*)attRet;
	if(str == "CONVEX")
	{
		physicsDataStructure.collisionType = ModelsManager.CONVEX;
	}
	else
	{
		return false;
	}

	attRet = xmlGetProp(physicsnode, (xmlChar*)"Mass");
	if(attRet == NULL) return false;
	str = (char*)attRet;
	physicsDataStructure.mass = Ogre::StringConverter::parseReal(str);

	attRet = xmlGetProp(physicsnode, (xmlChar*)"Gravity");
	if(attRet == NULL) return false;
	str = (char*)attRet;
	physicsDataStructure.gravity = Ogre::StringConverter::parseReal(str);

	return true;
}


xmlChar * GameLoadSaveManager::ConvertInput(const char *in, const char *encoding)
{
	xmlChar *out;
	int ret;
	int size;
	int out_size;
	int temp;
	xmlCharEncodingHandlerPtr handler;

	if (in == 0)
		return 0;

	handler = xmlFindCharEncodingHandler(encoding);

	if (!handler) {
		printf("ConvertInput: no encoding handler found for '%s'\n",
			   encoding ? encoding : "");
		return 0;
	}

	size = (int) strlen(in) + 1;
	out_size = size * 2 - 1;
	out = (unsigned char *) xmlMalloc((size_t) out_size);

	if (out != 0) {
		temp = size - 1;
		ret = handler->input(out, &out_size, (const xmlChar *) in, &temp);
		if ((ret < 0) || (temp - size + 1)) {
			if (ret < 0) {
				printf("ConvertInput: conversion wasn't successful.\n");
			} else {
				printf
					("ConvertInput: conversion wasn't successful. converted: %i octets.\n",
					 temp);
			}

			xmlFree(out);
			out = 0;
		} else {
			out = (unsigned char *) xmlRealloc(out, out_size + 1);
			out[out_size] = 0;  /*null terminating out */
		}
	} else {
		printf("ConvertInput: no mem\n");
	}

	return out;
}

bool GameLoadSaveManager::checkError(int rc, Ogre::String operation, Ogre::String tag_atribute)
{
	if (rc < 0) {
		Ogre::LogManager::getSingletonPtr()
			->logMessage(RealToys::logMessagePrefix 
				+ "Error at " + operation + " - " + tag_atribute,
				Ogre::LML_CRITICAL);		
		return false;
	}
	return true;
}

xmlNodePtr GameLoadSaveManager::findChildByName(const xmlNodePtr parent, std::string name)
{	
	xmlNodePtr no = parent;

		if(no == NULL || name.empty()) return NULL;

		for(no=no->children; no!= NULL; no=no->next){
				if(no->name && (strcmp((char*)no->name,name.c_str()) == 0)){
						return no;
				}
		}
	return NULL;
}

