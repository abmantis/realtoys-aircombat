#include "StdAfx.h"
#include "OgreFrame.h"


OgreFrameClass::OgreFrameClass()
{
	mFrameListener = 0;
	mWindowListener = 0;
	mRoot = 0;
	

	// Provide a nice cross platform solution for locating the configuration files
	// On windows files are searched for in the current working directory, on OS X however
	// you must provide the full path, the helper function macBundlePath does this for us.
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
	mResourcePath = macBundlePath() + "/Contents/Resources/";
#else
	mResourcePath = "";
#endif
}	
OgreFrameClass::~OgreFrameClass()
{
	if(mWindowListener)
	{
		Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, mWindowListener);
		delete mWindowListener;
	}
	if(mFrameListener)
	{		
		mRoot->removeFrameListener(mFrameListener);
		delete mFrameListener;
	}
	if(mRoot)
		OGRE_DELETE mRoot;
}

void OgreFrameClass::go()
{
	createRoot();
	defineResources();
	if(!setupRenderSystem())
		return;
	createRenderWindow();
	initializeResourceGroups();
	createCameraViweport();
	createFrameListener();	
	setupScene();		
	startRenderLoop();
}


void OgreFrameClass::createRoot()
{
	Ogre::String pluginsPath;
	// only use plugins.cfg if not static
#ifndef OGRE_STATIC_LIB
	pluginsPath = mResourcePath + "plugins.cfg";
#endif
	
	mRoot = OGRE_NEW Ogre::Root(pluginsPath, 
		mResourcePath + "ogre.cfg", mResourcePath + "Ogre.log");
}   



void OgreFrameClass::defineResources()
{
	// Load resource paths from config file
	Ogre::ConfigFile cf;
	cf.load(mResourcePath + "resources.cfg");

	// Go through all sections & settings in the file
	Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

	Ogre::String secName, typeName, archName;
	while (seci.hasMoreElements())
	{
		secName = seci.peekNextKey();
		Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
		Ogre::ConfigFile::SettingsMultiMap::iterator i;
		for (i = settings->begin(); i != settings->end(); ++i)
		{
			typeName = i->first;
			archName = i->second;			
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
			// OS X does not set the working directory relative to the app,
			// In order to make things portable on OS X we need to provide
			// the loading with it's own bundle path location
			ResourceGroupManager::getSingleton().addResourceLocation(
				String(macBundlePath() + "/" + archName), typeName, secName);
#else
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
				archName, typeName, secName);			
#endif
		}
	}	
}   

bool OgreFrameClass::setupRenderSystem()
{
	if (!mRoot->restoreConfig() && !mRoot->showConfigDialog())
		return false;
	return true;
}	

void OgreFrameClass::createRenderWindow()
{
	mWindow = mRoot->initialise(true, "RealToys: Air Combat (Alpha 1)");		
}

void OgreFrameClass::initializeResourceGroups()
{
	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

void OgreFrameClass::createCameraViweport()
{
	mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC, "Default SceneManager");

	mCamera = mSceneMgr->createCamera("Camera");
	mCamera->setNearClipDistance(1);
	mCamera->setFarClipDistance(0);

	Ogre::Viewport *vp = mWindow->addViewport(mCamera);
	mCamera->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));	
	vp->setBackgroundColour(Ogre::ColourValue(0.7f,0.7f,0.7f));
}



void OgreFrameClass::startRenderLoop()
{
	//float time = RakNet::GetTime();
	//Ogre::FrameEvent fe;
	//fe.timeSinceLastFrame = 0;
	//while(mFrameListener->frameStarted(fe))
	//{
	//	mRoot->renderOneFrame(); 
	//	fe.timeSinceLastFrame = (RakNet::GetTime() - time)/1000.0f;
	//	time = RakNet::GetTime();
	//	
	//}
 	 mRoot->startRendering();
}


