#ifndef __OgreFrameClass_H__
#define __OgreFrameClass_H__

#include "AppFrameListener.h"
#include "WindowListener.h"


#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
#include <CoreFoundation/CoreFoundation.h>

// This function will locate the path to our application on OS X,
// unlike windows you can not rely on the curent working directory
// for locating your configuration files and resources.
std::string macBundlePath()
{
	char path[1024];
	CFBundleRef mainBundle = CFBundleGetMainBundle();
	assert(mainBundle);

	CFURLRef mainBundleURL = CFBundleCopyBundleURL(mainBundle);
	assert(mainBundleURL);

	CFStringRef cfStringRef = CFURLCopyFileSystemPath( mainBundleURL, kCFURLPOSIXPathStyle);
	assert(cfStringRef);

	CFStringGetCString(cfStringRef, path, 1024, kCFStringEncodingASCII);

	CFRelease(mainBundleURL);
	CFRelease(cfStringRef);

	return std::string(path);
}
#endif

class OgreFrameClass
{
public:
	OgreFrameClass();
	~OgreFrameClass();
	virtual void go();
protected:
	Ogre::Root *mRoot;
	Ogre::SceneManager *mSceneMgr;
	Ogre::RenderWindow* mWindow;
	Ogre::String mResourcePath;	
	Ogre::Camera *mCamera;
	AppFrameListener *mFrameListener;	
	WindowListener *mWindowListener;
	
	void createRoot();
	void defineResources();
	bool setupRenderSystem();
	void createRenderWindow();
	void initializeResourceGroups();
	void createCameraViweport();
	virtual void setupScene() = 0;		// pure virtual - this has to be overridden
	virtual void createFrameListener() = 0;
	void startRenderLoop();
};

#endif