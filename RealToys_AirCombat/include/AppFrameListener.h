#ifndef __AppFrameListener_H__
#define __AppFrameListener_H__

#include "Airplane.h"
#include "AirplaneManager.h"
#include "NewtonFrameListener.h"
#include "CollisionTools.h"
#include "InputMappingManager.h"
#include "NetworkManager.h"
#include "DynamicObjectsManager.h"
#include "HUDManager.h"
#include "ParticlesManager.h"
#include "StereoManager.h"
#include "InterSenseManager.h"


class AppFrameListener: public Ogre::FrameListener, public OIS::MouseListener, public OIS::KeyListener, public OIS::JoyStickListener
{
public:
	AppFrameListener(Ogre::RenderWindow* window, Ogre::SceneManager* sceneManager, Ogre::Camera* camera, Ogre::StereoManager* stereoMgr, OgreNewt::World *world, bool spectator);
	~AppFrameListener(void);

	/*
		Terminates the application
	*/
	void end();
	bool frameStarted(const Ogre::FrameEvent& evt);
	void nextCamera();



	/* MouseListener */
	bool mouseMoved(const OIS::MouseEvent &e);
	/* MouseListener */
	bool mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id);
	/* MouseListener */
	bool mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id);

	/* KeyListener */
	bool keyPressed(const OIS::KeyEvent &e);
	/* KeyListener */
	bool keyReleased(const OIS::KeyEvent &e);

	/* JoyStickListener */
	bool buttonPressed( const OIS::JoyStickEvent &e, int button );
	/* JoyStickListener */
	bool buttonReleased( const OIS::JoyStickEvent &e, int button );
	/* JoyStickListener */
	bool axisMoved( const OIS::JoyStickEvent &e, int axis );
	/* JoyStickListener */
	bool sliderMoved( const OIS::JoyStickEvent &e, int sliderID );
	/* JoyStickListener */
	bool povMoved( const OIS::JoyStickEvent &e, int pov );

private:
	Ogre::RenderWindow* mWindow;
	Ogre::SceneManager* mSceneManager;
	Ogre::Camera* mCamera;
	Ogre::SceneNode* mCamNode;
	Ogre::StereoManager* mStereoMgr;
	OgreNewt::World *mWorld;	
	OgreOggSound::OgreOggSoundManager *mSoundMgr;
	NewtonFrameListener* mOgreNewtListener;
	InputMappingManager *mInputMappingManager;	
	NetworkManager *mNetworkManager;
	
	HUDManager *mHUDManager;
	DynamicObjectsManager *mDynObjManager;	//deletes objects that cannot be deleted immediately 

	Ogre::Vector3 mCamTranslation;		//translation to apply to the camera (distance from the airplane)
	int mCamPosition;					//camera position index (used to cycle tru different positions relative to the airplain)
	bool mCamLookBack;					//should we look back?

	Ogre::Real mTimeSinceLastFrame;		// Time passed since the last frame was printed
	int mSceneDetailIndex;				// solid, wireframe, points
	bool mContinue;						// Whether to continue rendering or not
	bool mShowNewtonDebug;
	

	Ogre::Real mRotate;					// The rotate constant
	Ogre::Real mMove;					// The movement constant
	Ogre::Degree mRotX;					// "Actual" X Rotation
	Ogre::Degree mRotY;					// "Actual" Y Rotation
	Ogre::Vector3 mDirection;			// Value to move in the correct direction	
	Ogre::Real mLastPlaneOmegaX;		// Used to check weather the new pitch is valid (if the difference between the two is big, use this one)
	Ogre::Real mLastPlaneOmegaZ;		// Like mLastPlaneOmegaX
	//Ogre::Real mLastPlaneSpeed;		// Like mLastPlaneOmegaX

	const ScoresManager::PlayerScoreInfo *mScoreInfo;	//Local player info
	Airplane *mAirplane;				//Local player airplane
	bool mLocalDead;					//Is the local player dead?
	AirplaneManager *mAirplaneMgr;		

	InputStates mInputState;
	InputStates mOriginalInputState;

	OgreOggSound::OgreOggListener *mSoundListener;

	ParticlesManager *mParticlesManager;

	InterSenseManager *mInterSenseMgr;


	
	void moveInGameCamera();
	void moveSpectatorCamera();
	void processInterSenseInput(Ogre::Real timeSinceLastFrame);

};
#endif