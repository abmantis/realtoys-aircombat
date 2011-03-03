#ifndef _NEWTONMATERIALMANAGER_H_
#define _NEWTONMATERIALMANAGER_H_

enum NewtonMaterialsEnum
{
	NME_AIRPLANE,
	NME_SHOT1,

	NME_DEFAULT //THIS SHOULD BE THE LAST ONE HERE, TO COUNT THEM!!	
};


class NewtonMaterialManager: public Ogre::Singleton<NewtonMaterialManager>
{
public:

	NewtonMaterialManager(OgreNewt::World *world)
	{
		mMaterialsVect.resize(NME_DEFAULT+1);
		for(int i = 0; i < NME_DEFAULT; i++)
		{
			mMaterialsVect[i] = new OgreNewt::MaterialID(world);
		}
		mMaterialsVect[NME_DEFAULT] = world->getDefaultMaterialID();

		Ogre::LogManager::getSingletonPtr()->logMessage(RealToys::logMessagePrefix + "NewtonMaterialManager created");
	}

	~NewtonMaterialManager(void)
	{
	}

	const OgreNewt::MaterialID* getMaterialID(NewtonMaterialsEnum nme)
	{
		return mMaterialsVect[nme];
	}

	
	static NewtonMaterialManager& getSingleton(void) { assert( ms_Singleton );  return ( *ms_Singleton ); }
	static NewtonMaterialManager* getSingletonPtr(void) { return ms_Singleton; }

private:
	std::vector<const OgreNewt::MaterialID*> mMaterialsVect;
};
template<> NewtonMaterialManager* Ogre::Singleton<NewtonMaterialManager>::ms_Singleton = 0;

#endif