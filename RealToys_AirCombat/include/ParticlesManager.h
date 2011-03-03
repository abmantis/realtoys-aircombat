#ifndef _PARTICLESMANAGER_H_
#define _PARTICLESMANAGER_H_

#include "StdAfx.h"

class ParticlesManager : public Ogre::Singleton<ParticlesManager>
{
public:
	ParticlesManager(Ogre::SceneManager *sceneManager)
		:mSceneMgr(sceneManager)
	{
		mEnabled = true;
		mShotBlueFlameCount = 0;
		mShotHitCount = 0;
	}
	~ParticlesManager(void){}

	void setEnabled(bool enabled)
	{
		mEnabled = enabled;
	}
	bool isEnabled()
	{
		return mEnabled;
	}

	void requestShotHitParticles(Ogre::Vector3 position, Ogre::Vector3 normal)
	{
		if(mEnabled)
		{
			if(mShotHitStack.empty())
			{
				std::cout << "Refill shothit particles" << std::endl;
				for(int i = 0; i < 30; i++)
				{
					mShotHitCount++;
					mShotHitStack.push(mSceneMgr->createParticleSystem("shotHitParticles" +
						Ogre::StringConverter::toString(mShotHitCount), "RealToys/HitExplosion"));
				}
			}

			Ogre::ParticleSystem *tempParticleSystem;
			tempParticleSystem = mShotHitStack.top();
			mShotHitStack.pop();

			//restart particle system
			int numEmitters = tempParticleSystem->getNumEmitters();
			for(int x = 0; x < numEmitters; x++)
			{
				Ogre::ParticleEmitter* emitter = tempParticleSystem->getEmitter(x);
				emitter->setEnabled(true);			
			}
			Ogre::SceneNode *partNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
			partNode->setPosition(position);
			partNode->setOrientation(Ogre::Vector3::UNIT_Y.getRotationTo(normal));
			partNode->attachObject(tempParticleSystem);
			
			
			ParticleSys_Time_Pair parttimepair;
			parttimepair.particleSystem = tempParticleSystem;
			parttimepair.TTL = 1;

			mShotHitInUseList.push_back(parttimepair);
		}
	}

	Ogre::ParticleSystem* requestShotBlueFlameParticles( Ogre::SceneNode *parentNode)
	{
		Ogre::ParticleSystem *part = NULL;
		if(mEnabled)
		{
			if(mShotBlueFlameStack.empty())
			{
				std::cout << "Refill shotBlueFlameStack particles" << std::endl;
				for(int i = 0; i < 30; i++)
				{
					mShotBlueFlameCount++;
					mShotBlueFlameStack.push(mSceneMgr->createParticleSystem("shotBlueFlameParticles" +
						Ogre::StringConverter::toString(mShotBlueFlameCount), "RealToys/ShotBlueFlame"));
				}
			}

			part = mShotBlueFlameStack.top();
			mShotBlueFlameStack.pop();		
			
			parentNode->attachObject(part);
			
			part->fastForward(1,0.1f);
		}
		return part;
	}

	void deliverShotBlueFlameParticles(Ogre::ParticleSystem* particleSystem)
	{
		if(mEnabled)
		{			
			mShotBlueFlameStack.push(particleSystem);			
		}
	}

	void update(float timeSinceLastUpdate)
	{
		bool eraseLast = false;
		Ogre::SceneNode *parentNode;

		mListIt = mShotHitInUseList.begin();
		
		for( ; mListIt != mShotHitInUseList.end(); mListIt++)
		{
			if(eraseLast)
			{
				mListDeleteIt = mListIt;
				mListDeleteIt--;
				mShotHitInUseList.erase(mListDeleteIt);
			}

			mListIt->TTL -= timeSinceLastUpdate;
			if(mListIt->TTL <= 0)
			{
				eraseLast = true;

				Ogre::ParticleSystem *tmp;
				tmp = mListIt->particleSystem;
				
				parentNode = tmp->getParentSceneNode();
				parentNode->removeAllChildren();
				mSceneMgr->destroySceneNode(parentNode);

				////stop particles
				//int numEmitters = tmp->getNumEmitters();				
				//for(int x = 0; x < numEmitters; x++)
				//{
				//	Ogre::ParticleEmitter* emitter = tmp->getEmitter(x);
				//	emitter->setEnabled(false);
				//}

				mShotHitStack.push(tmp);
				
			}
			else
			{
				eraseLast = false;
			}
		}
		if(eraseLast)
		{
			mListDeleteIt = mListIt;
			mListDeleteIt--;
			mShotHitInUseList.erase(mListDeleteIt);
		}
	}


private:
	struct ParticleSys_Time_Pair{
		Ogre::ParticleSystem *particleSystem;
		float TTL;
	} ;

	Ogre::SceneManager *mSceneMgr;
	bool mEnabled;
		
	Ogre::uint mShotHitCount;
	std::stack<Ogre::ParticleSystem*> mShotHitStack;
	std::list<ParticleSys_Time_Pair> mShotHitInUseList;
	std::list<ParticleSys_Time_Pair>::iterator mListIt;
	std::list<ParticleSys_Time_Pair>::iterator mListDeleteIt;

	Ogre::uint mShotBlueFlameCount;
	std::stack<Ogre::ParticleSystem*> mShotBlueFlameStack;

public:
	static ParticlesManager& getSingleton(void)
	{
		assert( ms_Singleton );  return ( *ms_Singleton );  
	}
	static ParticlesManager* getSingletonPtr(void)
	{
		return ms_Singleton;
	}
};
template<> ParticlesManager* Ogre::Singleton<ParticlesManager>::ms_Singleton = 0;

#endif

