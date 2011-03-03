#ifndef _HUDMANAGER_H_
#define _HUDMANAGER_H_

#include "StdAfx.h"

class HUDManager : public Ogre::Singleton<HUDManager>
{
public:
	HUDManager(Ogre::RenderWindow* window) : mWindow(window)
	{
		mOverlayManager = Ogre::OverlayManager::getSingletonPtr();
		mMaterialManager = Ogre::MaterialManager::getSingletonPtr();

		mCrosshairOverlay = mOverlayManager->getByName("RealToys/Crosshair_overlay");
		mCrosshairMaterial = mMaterialManager->getByName("RealToys/crosshair_material");
		mCrosshairHighlighted = false;
		mDebugStatsOverlay = mOverlayManager->getByName("RealToys/DebugOverlay");
		mHealthOverlay = mOverlayManager->getByName("RealToys/Healthpanel_overlay");
		mHealthRedDamageContainer = mHealthOverlay->getChild("RealToys/Healthpanel_overlay/health_container");
		mHealthRedDamageFullSize = mHealthRedDamageContainer->getHeight();
		mHealthRedDamageFullTopPos = mHealthRedDamageContainer->getTop();
		mInGameOverlaysVisible = false;

		Ogre::LogManager::getSingletonPtr()->logMessage(RealToys::logMessagePrefix + "HUDManager created");
		
	}
	~HUDManager(void){}

	void setInGameOverlaysVisible(bool visible)
	{	
		mInGameOverlaysVisible = visible;
		if(visible)
		{
			mCrosshairOverlay->show();
			mHealthOverlay->show();
		}
		else
		{
			mCrosshairOverlay->hide();
			mHealthOverlay->hide();
		}
	}
	bool changeInGameOverlaysVisibility() 
	{ 
		setInGameOverlaysVisible(!mInGameOverlaysVisible);
		return mInGameOverlaysVisible;
	}
	bool changeDebugStatsOverlayVisibility()
	{
		if(mDebugStatsOverlay->isVisible())
		{
			mDebugStatsOverlay->hide();
			return false;
		}
		else
		{
			mDebugStatsOverlay->show();
			return true;
		}
	}


	void updateDebugStats()
	{
		if(mDebugStatsOverlay->isVisible())
		{
			static const Ogre::String currFps	= "Current FPS: ";
			static const Ogre::String avgFps	= "Average FPS: ";
			static const Ogre::String bestFps	= "Best FPS: ";
			static const Ogre::String worstFps	= "Worst FPS: ";
			static const Ogre::String tris		= "Triangle Count: ";
			static const Ogre::String batch		= "Batch Count: ";


			Ogre::OverlayElement* guiCurr  = Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("RealToys/CurrFps");
			Ogre::OverlayElement* guiAvg   = Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("RealToys/AverageFps");
			Ogre::OverlayElement* guiBest  = Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("RealToys/BestFps");
			Ogre::OverlayElement* guiWorst = Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("RealToys/WorstFps");
			Ogre::OverlayElement* guiTris = Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("RealToys/NumTris");
			Ogre::OverlayElement* guiBatch = Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("RealToys/NumBatches");

			const Ogre::RenderTarget::FrameStats& stats = mWindow->getStatistics();

			guiCurr->setCaption(currFps + Ogre::StringConverter::toString(stats.lastFPS));
			guiAvg->setCaption(avgFps + Ogre::StringConverter::toString(stats.avgFPS));
			guiBest->setCaption(bestFps + Ogre::StringConverter::toString(stats.bestFPS)
				+" "+ Ogre::StringConverter::toString(stats.bestFrameTime)+" ms");
			guiWorst->setCaption(worstFps + '\t' + Ogre::StringConverter::toString(stats.worstFPS)
				+" "+ Ogre::StringConverter::toString(stats.worstFrameTime)+" ms");		
			guiTris->setCaption(tris + Ogre::StringConverter::toString(stats.triangleCount));  
			guiBatch->setCaption(batch + Ogre::StringConverter::toString(stats.batchCount));
		}
	}
	void setCrosshairVScroll(Ogre::Real vScroll, bool highlight)
	{
		mCrosshairOverlay->setScroll(0, vScroll);
		if(highlight)
		{
			if(!mCrosshairHighlighted)
			{
				//turn highlight on
				Ogre::TextureUnitState *texUnit = mCrosshairMaterial->getTechnique(0)->getPass(0)->getTextureUnitState(0);
				texUnit->setColourOperationEx(Ogre::LBX_MODULATE_X2, Ogre::LBS_MANUAL,
					Ogre::LBS_TEXTURE, Ogre::ColourValue(1,0,0));
				mCrosshairHighlighted = true;
			}
		}
		else
		{
			if(mCrosshairHighlighted)
			{
				//turn highlight off
				Ogre::TextureUnitState *texUnit = mCrosshairMaterial->getTechnique(0)->getPass(0)->getTextureUnitState(0);
				texUnit->setColourOperationEx(Ogre::LBX_SOURCE1, Ogre::LBS_TEXTURE);
				mCrosshairHighlighted = false;
			}
		}
		
		
	}
	void setCrosshairVisible(bool visible)
	{
		if(visible)
			mCrosshairOverlay->show();
		else
			mCrosshairOverlay->hide();
	}
	void setHealth(int health)
	{
		//FALTA VERIFICAR SE MUDOU 
		Ogre::Real invHealthPercent = 1 - (health * 0.01f);
		Ogre::Real change = mHealthRedDamageFullSize * invHealthPercent;
		mHealthRedDamageContainer->setHeight(mHealthRedDamageFullSize - change);
		mHealthRedDamageContainer->setTop(mHealthRedDamageFullTopPos + change);
		mHealthRedDamageContainer->setParameter("uv_coords","0 " +
			Ogre::StringConverter::toString(invHealthPercent) +" 1 1" 	);
	}
private:
	Ogre::RenderWindow* mWindow;
	Ogre::OverlayManager *mOverlayManager;
	Ogre::MaterialManager *mMaterialManager;

	Ogre::Overlay *mDebugStatsOverlay;
	Ogre::Overlay *mCrosshairOverlay;
	Ogre::Overlay *mHealthOverlay;

	Ogre::OverlayContainer *mHealthRedDamageContainer;

	Ogre::MaterialPtr mCrosshairMaterial;

	Ogre::Real mHealthRedDamageFullSize;
	Ogre::Real mHealthRedDamageFullTopPos;

	bool mInGameOverlaysVisible;
	bool mCrosshairHighlighted;

public:
	static HUDManager& HUDManager::getSingleton(void) { assert( ms_Singleton );  return ( *ms_Singleton ); }
	static HUDManager* HUDManager::getSingletonPtr(void) { return ms_Singleton; }
};
template<> HUDManager* Ogre::Singleton<HUDManager>::ms_Singleton = 0;

#endif