#ifndef __RealToys_AirCombat_h_
#define __RealToys_AirCombat_h_
#include "OgreFrame.h"


class RealToysAirCombat: public OgreFrameClass
{
public:
	~RealToysAirCombat(void);
	void go();

private:
	OgreNewt::World* mWorld;
	OgreOggSound::OgreOggSoundManager *mSoundMgr;

	Ogre::String mMapFile;
	Ogre::String mMapsLocation;
	Ogre::String mServerAddress;
	Ogre::ushort mServerPort;
	bool mIsSpectator;
	Ogre::Real mSoundVolume;
	bool mParticlesEnabled;
	
	void setupScene();			//virtual (OgreFrameClass)	
	void initialise();
	void createFrameListener();
	void loadConfigFromFile();
	void addLights();
};

#endif // #ifndef __RealToys_AirCombat_h_