#ifndef __RealToys_AirCombat_h_
#define __RealToys_AirCombat_h_
#include "OgreFrame.h"


class RealToysAirCombat: public OgreFrameClass
{
public:
	RealToysAirCombat(void);
	~RealToysAirCombat(void);
	void go();

private:
	OgreNewt::World* mWorld;
	OgreOggSound::OgreOggSoundManager *mSoundMgr;
	Ogre::StereoManager *mStereoManager;

	Ogre::String mMapFile;
	Ogre::String mMapsLocation;
	Ogre::String mServerAddress;
	Ogre::ushort mServerPort;
	bool mIsSpectator;
	Ogre::Real mSoundVolume;
	bool mParticlesEnabled;
	Ogre::StereoManager::StereoMode mStereoMode;
	
	void setupScene();					//virtual (OgreFrameClass)	
	void initialise();
	void createFrameListener();
	void createStereoManager();
	void loadConfigFromFile();
	void addLights();
	void initialiseStereoManager();
};

#endif // #ifndef __RealToys_AirCombat_h_