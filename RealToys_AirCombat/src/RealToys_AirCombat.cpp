#include "StdAfx.h"
#include "RealToys_AirCombat.h"
#include "GameLoadSaveManager.h"
#include "NetworkManager.h"
#include "../gausslist.h"
#include "CustomObjects.h"
#include "NewtonMaterialManager.h"
#include "DynamicObjectsManager.h"
#include "ScoresManager.h"
#include "HUDManager.h"
#include "ParticlesManager.h"
/*
	Unidades:
	Peso: decagramas (100 unidades = 1kg)
	Distancia: centimetros (100 unidades = 1m)
*/

RealToysAirCombat::~RealToysAirCombat(void)
{
	delete NewtonMaterialManager::getSingletonPtr();
	delete DynamicObjectsManager::getSingletonPtr();
	delete MainStructManager::getSingletonPtr();
	delete ModelsManager::getSingletonPtr();
	delete WallManager::getSingletonPtr();
	delete AirplaneMarkersManager::getSingletonPtr();
	delete AirplaneManager::getSingletonPtr();
	delete GameLoadSaveManager::getSingletonPtr();
	delete HUDManager::getSingletonPtr();

	if(mWorld)
	{
		mWorld->getDebugger().deInit();	
		delete mWorld;	
	}
	//if(mSoundMgr)
	//{
	//	delete mSoundMgr;
	//}
}
void RealToysAirCombat::go()
{
	createRoot();
	defineResources();
	if(!setupRenderSystem())
		return;
	createRenderWindow();
	initializeResourceGroups();
	createCameraViweport();
	initialise();
	createFrameListener();	
	setupScene();	
	startRenderLoop();

	//make sure the connection shuts down!
	delete NetworkManager::getSingletonPtr();
}
void RealToysAirCombat::initialise()
{

	loadConfigFromFile();

	mWorld = new OgreNewt::World();
	//mWorld->setSolverModel(OgreNewt::World::SM_ADAPTIVE);
	mSoundMgr = OgreOggSound::OgreOggSoundManager::getSingletonPtr();
	Ogre::LogManager::getSingletonPtr()->logMessage(RealToys::logMessagePrefix + "OgreOggSound::OgreOggSoundManager::getSingletonPtr();");
	mSoundMgr->init();
	Ogre::LogManager::getSingletonPtr()->logMessage(RealToys::logMessagePrefix + "OgreOggSound::init()");
	mSoundMgr->setDistanceModel(AL_LINEAR_DISTANCE);
	Ogre::LogManager::getSingletonPtr()->logMessage(RealToys::logMessagePrefix + "OgreOggSound::setDistanceModel(AL_LINEAR_DISTANCE)");
	mSoundMgr->setMasterVolume(mSoundVolume);
	Ogre::LogManager::getSingletonPtr()->logMessage(RealToys::logMessagePrefix + "OgreOggSound::setMasterVolume(mSoundVolume);");
			

	new HUDManager(mWindow);
	new NewtonMaterialManager(mWorld);
	new DynamicObjectsManager();
	new MainStructManager(mSceneMgr, mWorld);
	new ModelsManager(mSceneMgr, mWorld);
	new WallManager(mSceneMgr, mWorld);
	new AirplaneMarkersManager(mSceneMgr);
	new ScoresManager();	
	new NetworkManager(mSceneMgr, mWorld);

	new ParticlesManager(mSceneMgr);
	ParticlesManager::getSingletonPtr()->setEnabled(mParticlesEnabled);
	
	

	if(mServerAddress == "0")
		new AirplaneManager(true, mSceneMgr, mWorld);
	else
		new AirplaneManager(false, mSceneMgr, mWorld);

		
	GameLoadSaveManager *glsm = new GameLoadSaveManager(mSceneMgr);
	glsm->SetMapsPath(mMapsLocation);
}
void RealToysAirCombat::createFrameListener()
{
	mFrameListener = new AppFrameListener(mWindow, mSceneMgr, mCamera, mWorld, mIsSpectator);
	mRoot->addFrameListener(mFrameListener);
	
	mWindowListener = new WindowListener(mFrameListener);
	Ogre::WindowEventUtilities::addWindowEventListener(mWindow, mWindowListener);

}
void RealToysAirCombat::loadConfigFromFile()
{
	Ogre::String valueStr;
	Ogre::LogManager::getSingletonPtr()
		->logMessage(RealToys::logMessagePrefix + "Reading main configurations from " + RealToys::configFileName);
	// Load config file
	Ogre::ConfigFile configFile;
	configFile.load(RealToys::configFileName);


	/* Grafics defenitions */
	//Texture Filtering
	valueStr = configFile.getSetting("TexFilter", "Graphic Settings", "1");
	if( valueStr.compare("0") == 0)
		Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_NONE);
	else
	{
		if( valueStr.compare("2") == 0)
			Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_TRILINEAR);
		else
		{
			if( valueStr.compare("3") == 0)
				Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_ANISOTROPIC);
			else
				Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_BILINEAR);
		}
	}

	//Anisotropric Filtering
	valueStr = configFile.getSetting("Anisotropy", "Graphic Settings", "1");
	int anisotropy = Ogre::StringConverter::parseInt(valueStr);
	Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(anisotropy);

	//particles?
	valueStr = configFile.getSetting("Particles", "Graphic Settings", "1");
	mParticlesEnabled = Ogre::StringConverter::parseBool(valueStr);

	//read map settings
	mMapsLocation = configFile.getSetting("MapsPath", "Misc", "../../saved/");
	mMapFile = configFile.getSetting("MapFileName", "Misc", "empty.zip");

	//spectator mode?
	valueStr = configFile.getSetting("Spectator", "Misc", "False");
	mIsSpectator = Ogre::StringConverter::parseBool(valueStr);

	//sound settings
	valueStr = configFile.getSetting("SoundVolume", "Misc", "1");
	mSoundVolume = Ogre::StringConverter::parseReal(valueStr);


	//read network settings
	mServerAddress = configFile.getSetting("ServerAddress", "Network", "0");
	valueStr = configFile.getSetting("ServerPort", "Network", "61231");
	mServerPort = Ogre::StringConverter::parseUnsignedInt(valueStr);

	

	Ogre::LogManager::getSingletonPtr()
		->logMessage(RealToys::logMessagePrefix + "Configurations loaded from " + RealToys::configFileName);
}
void RealToysAirCombat::setupScene()
{
	mSceneMgr->setAmbientLight(Ogre::ColourValue(0.4, 0.4, 0.4));
	addLights();


	//Ogre::CompositorInstance *instance = Ogre::CompositorManager::getSingletonPtr()
	//	->addCompositor(mCamera->getViewport(), "Gaussian Blur");
	//if(instance == 0)
	//{
	//	std::cout << "FAIL COMPOSITOR" << std::endl;
	//	exit(-1);
	//}
	//GaussianListener *gs = new GaussianListener();
	//instance->addListener(gs);
	//gs->notifyViewportSize(mCamera->getViewport()->getActualWidth(),mCamera->getViewport()->getActualHeight());
	//
	//Ogre::CompositorManager::getSingleton().setCompositorEnabled(mCamera->getViewport(), "Gaussian Blur", true);


	Ogre::OverlayManager::getSingletonPtr()->getByName("RealToys/LoadingscreenOverlay")->show();
	if(!mRoot->renderOneFrame())
	{
		mRoot->shutdown();
		exit(1);
	}

	NetworkManager *nm = NetworkManager::getSingletonPtr();
	if(mServerAddress == "0")
	{
		//start as server
		nm->start(true, mServerPort, "");
	}
	else if (mServerAddress == "1")
	{
		//start as client, try to find the lan server
		nm->start(false, mServerPort, "255.255.255.255");
	}
	else 
	{
		//start as client
		nm->start(false, mServerPort, mServerAddress);
	}
	// the network should be up and running at this point
	if(!nm->isUpAndRunning())
	{
		mRoot->shutdown();
		exit(1);
	}	
	else
	{
		if(nm->isServer())
			nm->setCurrentMap(mMapFile);
		else
		{
			GameLoadSaveManager *gmls = GameLoadSaveManager::getSingletonPtr();
			//update till it receives map
			while(mRoot->renderOneFrame() && !gmls->IsMapLoaded());
		}
	}

	Ogre::OverlayManager::getSingletonPtr()->getByName("RealToys/LoadingscreenOverlay")->hide();

	if(mIsSpectator)
	{
		Ogre::OverlayManager::getSingletonPtr()->getByName("RealToys/PopupMessageOverlay")->hide();
	}
	else
	{
		Ogre::OverlayManager::getSingletonPtr()->getByName("RealToys/PopupMessageOverlay")->show();
		Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("RealToys/PopupMessageText")
			->setCaption("Press SPACE to start!");
	}


}

void RealToysAirCombat::addLights()
{	
	Ogre::String nodePrefix = "lightNode";
	Ogre::String num;
	Ogre::Light *l;
	Ogre::SceneNode *node, *masterNode;
	int lightCount=0;

	masterNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(nodePrefix);

	num = Ogre::StringConverter::toString(lightCount);
	// Create a light
	l = mSceneMgr->createLight("TopLight" + num);			
	l->setPosition(0, RealToys::wallHeight*20, 0);
	l->setDiffuseColour(0.4,0.4,0.4);
	lightCount++;

	node = masterNode->createChildSceneNode(nodePrefix + num,
		Ogre::Vector3(0, RealToys::wallHeight*20, 0));
	node->setScale(0.1,0.1,0.1);


	num = Ogre::StringConverter::toString(lightCount);
	// Create a light
	l = mSceneMgr->createLight("TopLight" + num);			
	l->setPosition(-10000, RealToys::wallHeight*2, -10000);
	l->setDiffuseColour(0.4,0.4,0.4);
	lightCount++;

	node = masterNode->createChildSceneNode(nodePrefix + num,
		Ogre::Vector3(-10000, RealToys::wallHeight*2, -10000));
	node->setScale(0.1,0.1,0.1);


	num = Ogre::StringConverter::toString(lightCount);
	// Create a light
	l = mSceneMgr->createLight("TopLight" + num);			
	l->setPosition(-10000, RealToys::wallHeight*2, 10000);
	l->setDiffuseColour(0.4,0.4,0.4);
	lightCount++;

	node = masterNode->createChildSceneNode(nodePrefix + num,
		Ogre::Vector3(-10000, RealToys::wallHeight*2, 10000));
	node->setScale(0.1,0.1,0.1);


	num = Ogre::StringConverter::toString(lightCount);
	// Create a light
	l = mSceneMgr->createLight("TopLight" + num);			
	l->setPosition(10000, RealToys::wallHeight*2, 10000);
	l->setDiffuseColour(0.4,0.4,0.4);
	lightCount++;

	node = masterNode->createChildSceneNode(nodePrefix + num,
		Ogre::Vector3(10000, RealToys::wallHeight*2, 10000));
	node->setScale(0.1,0.1,0.1);


	num = Ogre::StringConverter::toString(lightCount);
	// Create a light
	l = mSceneMgr->createLight("TopLight" + num);			
	l->setPosition(10000, RealToys::wallHeight*2, -10000);
	l->setDiffuseColour(0.4,0.4,0.4);
	lightCount++;

	node = masterNode->createChildSceneNode(nodePrefix + num,
		Ogre::Vector3(10000, RealToys::wallHeight*2, -10000));
	node->setScale(0.1,0.1,0.1);
}


#ifdef __cplusplus
extern "C" {
#endif

	void terminate( int param )
	{
		OgreOggSound::OgreOggSoundManager::getSingleton().stopAllSounds();
		OgreOggSound::OgreOggSoundManager::getSingleton().destroyAllSounds();
		exit(-1);
	}

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
   INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
   int main(int argc, char *argv[])
#endif
   {
	   signal (SIGSEGV,terminate);

	  // Create application object
	  RealToysAirCombat app;
	try
	{
		try {
			 app.go();
		} catch( Ogre::Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
			 MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
			std::cerr << "An exception has occured: " <<
				e.getFullDescription().c_str() << std::endl;
#endif
		}
	}
	catch( ... )
	{
		return -1;
	}

	  return 0;
   }

#ifdef __cplusplus
}
#endif

