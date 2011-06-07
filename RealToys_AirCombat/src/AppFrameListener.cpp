#include "StdAfx.h"

#include "AppFrameListener.h"
#include "RealToysShared.h"
#include "NetworkManager.h"

AppFrameListener::AppFrameListener(Ogre::RenderWindow* window, Ogre::SceneManager* sceneManager, Ogre::Camera* camera, Ogre::StereoManager* stereoMgr, OgreNewt::World *world, bool spectator):
mWindow(window), mSceneManager(sceneManager), mCamera(camera), mStereoMgr(stereoMgr), mWorld(world)
{
	mOgreNewtListener = new NewtonFrameListener( mSceneManager, mWorld, 120 );
	mShowNewtonDebug = false;	

	mSoundMgr = OgreOggSound::OgreOggSoundManager::getSingletonPtr();
	mDynObjManager = DynamicObjectsManager::getSingletonPtr();
	mNetworkManager = NetworkManager::getSingletonPtr();	
	mAirplaneMgr = AirplaneManager::getSingletonPtr();
	mHUDManager = HUDManager::getSingletonPtr();
	mParticlesManager = ParticlesManager::getSingletonPtr();

	if(spectator)
		mOriginalInputState = INPUTSTATE_SPECTATOR;
	else
		mOriginalInputState = INPUTSTATE_INGAME;

	mInputState = INPUTSTATE_SPECTATOR;
	

	mInputMappingManager = InputMappingManager::getSingletonPtr();
	mInputMappingManager->initialise(mWindow);
	mInputMappingManager->setKeyListener(this);
	mInputMappingManager->setMouseListener(this);
	mInputMappingManager->setJoystickListener(this);

	mInterSenseMgr = InterSenseManager::getSingletonPtr();		

	mHUDManager->setInGameOverlaysVisible(false);
	
	
	mSceneDetailIndex = 0;
	mCamLookBack = false;

	// set the rotation and move speed per second
	mRotate = 0.13;	
	mMove = 50;
	mDirection = 0;
	mLastPlaneOmegaX = 0;
	mLastPlaneOmegaZ = 0;
	
	// continue rendering
	mContinue = true;

	mAirplane = NULL;

	mCamera->detachFromParent();	
	mCamNode = mSceneManager->getRootSceneNode()->createChildSceneNode("MainCameraNode");
	mCamNode->attachObject(mCamera);	
	mCamera->setPosition(0,0,0);
	mCamera->setDirection(Ogre::Vector3::UNIT_Z);
	//mCamNode->attachObject(mSoundMgr->getListener());

	mSoundListener = mSoundMgr->getListener();
	Ogre::LogManager::getSingletonPtr()->logMessage(RealToys::logMessagePrefix 
		+ "FrameListener created");
}

AppFrameListener::~AppFrameListener()
{
	end();	//just to make sure
}


void AppFrameListener::end()
{
	mContinue = false;

}
bool AppFrameListener::frameStarted(const Ogre::FrameEvent& evt)
{

	mTimeSinceLastFrame = evt.timeSinceLastFrame;
	mRotX = 0;
	mRotY = 0;

	if(mInputMappingManager) 
	{ 
		mInputMappingManager->capture();
	}
	
	if(mInterSenseMgr->isActive())
	{
		processInterSenseInput(mTimeSinceLastFrame);
	}
	 
	if(!mContinue || !mNetworkManager->update())
		return false;

	
	mOgreNewtListener->update(evt.timeSinceLastFrame);	
	if(mShowNewtonDebug)
	{
		mWorld->getDebugger().showDebugInformation();		
	}

	mDynObjManager->deleteAll();

	mAirplaneMgr->updateAirplanes(evt.timeSinceLastFrame);	
	
	

	if(mAirplane != 0)
	{
		if(mAirplane->isDead() != mLocalDead)
		{
			mHUDManager->changeInGameOverlaysVisibility();
			mLocalDead = mAirplane->isDead();
		}
		moveInGameCamera();
		//std::cout << mScoreInfo->mCurrentHealth << std::endl;
		mHUDManager->setHealth(mScoreInfo->mCurrentHealth);


		if(mCamPosition == 0)
		{
			Ogre::Quaternion listenerOri = mCamNode->_getDerivedOrientation() * mCamera->getOrientation();
			//Ogre::Vector3 translate = mCamTranslation;
			//translate.z *= 1.1;
			mSoundListener->setPosition(mCamNode->_getDerivedPosition() + listenerOri*mCamTranslation  );
			mSoundListener->setOrientation(listenerOri);
		}
		else
		{
			mSoundListener->setPosition(mCamNode->_getDerivedPosition());
			mSoundListener->setOrientation(mCamNode->_getDerivedOrientation() * mCamera->getOrientation());
		}
	}
	else
	{
		if( mInputState == INPUTSTATE_SPECTATOR )
			moveSpectatorCamera();
		else
		{
			mAirplane = mAirplaneMgr->getLocalAirplane();
			if(mAirplane != 0)
			{
				mScoreInfo = mAirplane->getScoreInfo();
				mLocalDead = mAirplane->isDead();
				mHUDManager->setInGameOverlaysVisible(true);
				
			}
		}

		mSoundListener->setPosition(mCamNode->_getDerivedPosition());
		mSoundListener->setOrientation(mCamNode->_getDerivedOrientation() * mCamera->getOrientation());
	}
	
	mHUDManager->updateDebugStats();
	mSoundMgr->update(mTimeSinceLastFrame);
	mParticlesManager->update(mTimeSinceLastFrame);

	return true;
}


void AppFrameListener::moveInGameCamera()
{
	//mCamNode->setPosition(Ogre::Vector3(100,200,100));
	////mCamNode->setOrientation(planeOri);
	//mCamNode->lookAt(Ogre::Vector3::ZERO, Ogre::Node::TS_WORLD, Ogre::Vector3::UNIT_Z);

	//return;


	Ogre::Vector3 planePos, planeDir, planeOmega;
	Ogre::Quaternion planeOri;

	mAirplane->getPositionOrientation(planePos,	planeOri);	
	mAirplane->getOmega(planeOmega);

	planeOmega = planeOri.Inverse() * planeOmega;	

	mCamNode->setPosition(planePos);
	mCamNode->setOrientation(planeOri);
	
	
	if( Ogre::Math::Abs(planeOmega.x) - Ogre::Math::Abs(mLastPlaneOmegaX) < 60*mTimeSinceLastFrame)
		mLastPlaneOmegaX = planeOmega.x;
	if( Ogre::Math::Abs(planeOmega.z) - Ogre::Math::Abs(mLastPlaneOmegaZ) < 60*mTimeSinceLastFrame)
		mLastPlaneOmegaZ = planeOmega.z;
	//if( planeVel.z > 0 && planeVel.z - mLastPlaneSpeed < 1)
	//	mLastPlaneSpeed = planeVel.z;

	
	
	if(mCamPosition != 0 || mCamLookBack)
	{
		mCamNode->pitch( Ogre::Radian(mLastPlaneOmegaX)*-0.1 );
		mCamNode->roll( Ogre::Radian(mLastPlaneOmegaZ)*-0.1);
	}
	if(mCamLookBack)
	{
		mCamNode->yaw(Ogre::Degree(180));
		//mCamNode->translate(Ogre::Vector3(0,5,-20), Ogre::Node::TS_LOCAL);
	}
	
	mCamNode->translate(mCamTranslation, Ogre::Node::TS_LOCAL);			


	//Launch a ray from airplane in it's direction, to get the first hit in order 
	//to find the correct position for the crosshair
	Ogre::Vector3 closestPos;
	Ogre::Matrix4 camMatrix;			
	bool enemyHit = false;
	enemyHit = mAirplane->getTargetHitPoint(closestPos);
	camMatrix = mCamera->getProjectionMatrix() * mCamera->getViewMatrix(true);			
	mHUDManager->setCrosshairVScroll((camMatrix*closestPos).y, enemyHit);	

	

}

void AppFrameListener::moveSpectatorCamera()
{
	mCamNode->yaw(mRotX, Ogre::Node::TS_WORLD);
	mCamNode->pitch(mRotY);
	mCamNode->translate(mDirection * mTimeSinceLastFrame, Ogre::Node::TS_LOCAL);

	/*mCamera->yaw(mRotX);
	mCamera->pitch(mRotY);
	mCamera->moveRelative(mDirection * mTimeSinceLastFrame);	*/
}

void AppFrameListener::nextCamera()
{	
	mCamPosition++;
	if(mCamPosition <= 0)
	{
		mCamTranslation.x = 0;
		mCamTranslation.y = 0;
		//mCamTranslation.z = 1.67;
		mCamTranslation.z = 5.66;
	}
	else
	{
		if(mCamPosition == 1)
		{			
			mCamTranslation.x = 0;
			mCamTranslation.y = 05;
			mCamTranslation.z = -15;			
		}
		else
		{
			if(mCamPosition == 2)
			{
				mCamTranslation.x = 0;
				mCamTranslation.y = 07;
				mCamTranslation.z = -40;
			}
			else
			{
				mCamTranslation.x = 0;
				mCamTranslation.y = 10;
				mCamTranslation.z = -60;
				
				mCamPosition = -1;
			}
		}
	}	
	
}
// MouseListener
bool AppFrameListener::mouseMoved(const OIS::MouseEvent &e)
{
	
	mRotX = Ogre::Degree(-mRotate * e.state.X.rel);
	mRotY = Ogre::Degree(-mRotate * e.state.Y.rel);


	return true;
}

bool AppFrameListener::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{	

	switch ( id )
	{	
	case OIS::MB_Left:
		{
			break;
		}
		
	case OIS::MB_Middle:
		{
			break;
		}

	case OIS::MB_Right:
		{
			break;
		}	
	default:
		break;
	}

	return true;
}

bool AppFrameListener::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id) 
{
	switch ( id )
	{	
	case OIS::MB_Left:
		{
			break;
		}
		
	case OIS::MB_Middle:
		{
			break;
		}

	case OIS::MB_Right:
		{			
			break;
		}
	default:
		break;
	}
	return true; 
}

// KeyListener
bool AppFrameListener::keyPressed(const OIS::KeyEvent &e)
{
	switch(e.key)		
	{
	case OIS::KC_ADD:
		mStereoMgr->setEyesSpacing(mStereoMgr->getEyesSpacing() + 0.1f);		
		Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("RealToys/DebugText")->show();
		Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("RealToys/DebugText")->setCaption(
			"EYES SPACING = " + Ogre::StringConverter::toString(mStereoMgr->getEyesSpacing()));
		
		break;
	case OIS::KC_SUBTRACT:
		Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("RealToys/DebugText")->show();
		mStereoMgr->setEyesSpacing(mStereoMgr->getEyesSpacing() - 0.1f);
		Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("RealToys/DebugText")->setCaption(
			"EYES SPACING = " + Ogre::StringConverter::toString(mStereoMgr->getEyesSpacing()));
		break;
	case OIS::KC_MULTIPLY:
		mStereoMgr->setEyesSpacing(0.06f);
		Ogre::OverlayManager::getSingletonPtr()->getOverlayElement("RealToys/DebugText")->setCaption(
			"EYES SPACING = " + Ogre::StringConverter::toString(mStereoMgr->getEyesSpacing()));
		break;
	}

	if(mInputState == INPUTSTATE_INGAME)
	{
		
		switch(mInputMappingManager->getKeyAction(e.key))
		{
			case KA__PITCH_DOWN:
			{
				//mAirplane->addPitchForce(-PITCHFORCE_2);	
				mNetworkManager->applyAirplaneInput(INPUTACTION_PITCHDOWN, true);
				break;
			}
			case KA__PITCH_UP:
			{
				//mAirplane->addPitchForce(PITCHFORCE_2);			
				mNetworkManager->applyAirplaneInput(INPUTACTION_PITCHUP, true);
				break;
			}

			case KA__ROLL_LEFT:
			{
				//mAirplane->addRollForce(-ROLLFORCE_2);
				mNetworkManager->applyAirplaneInput(INPUTACTION_ROLLLEFT, true);
				break;
			}

			case KA__ROLL_RIGHT:
			{
				//mAirplane->addRollForce(ROLLFORCE_2);
				mNetworkManager->applyAirplaneInput(INPUTACTION_ROLLRIGHT, true);
				break;
			}

			case KA__DEC_THRUST:
			{
				//mAirplane->decreaseThrust();
				mNetworkManager->applyAirplaneInput(INPUTACTION_DECSPEED, true);
				break;
			}

			case KA__INC_THRUST:
			{
				//mAirplane->increaseThrust();
				mNetworkManager->applyAirplaneInput(INPUTACTION_INCSPEED, true);
				break;
			}	
			case KA__THRUST_0:
			{
				//mAirplane->setThrustForce(THRUSTFORCE_0);
				mNetworkManager->applyAirplaneInput(INPUTACTION_SPEED0, true);
				break;
			}
			case KA__THRUST_1:
			{
				//mAirplane->setThrustForce(THRUSTFORCE_1);
				mNetworkManager->applyAirplaneInput(INPUTACTION_SPEED1, true);
				break;
			}
			case KA__THRUST_2:
			{
				//mAirplane->setThrustForce(THRUSTFORCE_2);
				mNetworkManager->applyAirplaneInput(INPUTACTION_SPEED2, true);
				break;
			}
			case KA__THRUST_3:
			{
				//mAirplane->setThrustForce(THRUSTFORCE_3);
				mNetworkManager->applyAirplaneInput(INPUTACTION_SPEED3, true);
				break;
			}
			
			case KA__NEXT_CAM:
			{
				nextCamera();
				break;
			}
			case KA__LOOK_BACK:
			{
				mCamLookBack = true;
				mHUDManager->setCrosshairVisible(false);
				break;
			}

			case KA__FIRE1:
			{
				mNetworkManager->applyAirplaneInput(INPUTACTION_SHOT1, true);
				break;
			}

			case KA_TOGGLE_ISENSE:
			{
				if(mNetworkManager->isServer())
				{
					bool isenseWasActive = mInterSenseMgr->isActive();
			
					if(isenseWasActive)
					{
						mInterSenseMgr->setActive(false);
						mAirplane->setPitchForce(0.0);
						mAirplane->setRollForce(0.0);
					}
					else
					{
						// is intersense started?
						if(mInterSenseMgr->isInited() == false)
						{
							// try to start it up
							if(mInterSenseMgr->init())
							{
								mInterSenseMgr->resetAngles();
							}
						}

						mInterSenseMgr->setActive(true);
					}
				}				
				break;
			}

			case KA_RESET_ISENSE:
			{
				if(mInterSenseMgr->isActive())
				{
					mInterSenseMgr->resetAngles();
				}
				
				break;
			}

			case KA__DEBUGPANEL:
			{
				mHUDManager->changeDebugStatsOverlayVisibility();
				break;
			}		
			case KA__DEBUGDRAWMODE:
			{
				mSceneDetailIndex = (mSceneDetailIndex+1)%3 ;
				switch(mSceneDetailIndex) 
				{
					case 0 : mCamera->setPolygonMode(Ogre::PM_SOLID); break;
					case 1 : mCamera->setPolygonMode(Ogre::PM_WIREFRAME); break;
					case 2 : mCamera->setPolygonMode(Ogre::PM_POINTS); break;
				}
				break;
			}
			case KA__DEBUGNEWTON:
			{
				mShowNewtonDebug = true;
				break;
			}
			case KA__DEC_SOUND:
			{
				mSoundMgr->setMasterVolume(mSoundMgr->getMasterVolume()-0.1f);
				break;
			}
			case KA__INC_SOUND:
			{
				mSoundMgr->setMasterVolume(mSoundMgr->getMasterVolume()+0.1f);				
				break;
			}
			case KA__SCREEN_SHOT:
			{
				mWindow->writeContentsToTimestampedFile("screenshot",".jpg");
				break;
			}
			case KA__EXIT:
			{
				this->end();
				break;
			}
			default:
				break;
		}	//switch
		
		return true;
	}
	if(mInputState == INPUTSTATE_SPECTATOR)
	{
		switch (e.key)
		{		
		case OIS::KC_UP:
		case OIS::KC_W:
			mDirection.z += mMove;			
			break;

		case OIS::KC_DOWN:
		case OIS::KC_S:
			mDirection.z += -mMove;
			break;

		case OIS::KC_LEFT:
		case OIS::KC_A:
			mDirection.x += mMove;
			break;

		case OIS::KC_RIGHT:
		case OIS::KC_D:
			mDirection.x += -mMove;
			break;

		case OIS::KC_PGDOWN:
		case OIS::KC_Q:
			mDirection.y += -mMove;
			break;

		case OIS::KC_PGUP:
		case OIS::KC_E:
			mDirection.y += mMove;
			break;	

		case OIS::KC_F9:
			mHUDManager->changeDebugStatsOverlayVisibility();
			break;		
		case OIS::KC_F10:
			{
				mSceneDetailIndex = (mSceneDetailIndex+1)%3 ;
				switch(mSceneDetailIndex) {
					case 0 : mCamera->setPolygonMode(Ogre::PM_SOLID); break;
					case 1 : mCamera->setPolygonMode(Ogre::PM_WIREFRAME); break;
					case 2 : mCamera->setPolygonMode(Ogre::PM_POINTS); break;
				}
				break;
			}
		case OIS::KC_F11:
			mShowNewtonDebug = true;
			break;
		case OIS::KC_F12:
		{
			mWindow->writeContentsToTimestampedFile("screenshot",".jpg");
			break;
		}

		case OIS::KC_ESCAPE:
			this->end();
			break;
		default:
			break;
		}
		return true;
	}
	
	return true;
}

bool AppFrameListener::keyReleased(const OIS::KeyEvent &e)
{	
	if(mInputState == INPUTSTATE_INGAME)
	{
		switch( mInputMappingManager->getKeyAction(e.key))
		{
			case KA__PITCH_DOWN:
			{
				//mAirplane->addPitchForce(PITCHFORCE_2);
				mNetworkManager->applyAirplaneInput(INPUTACTION_PITCHDOWN, false);
				break;
			}
			case KA__PITCH_UP:
			{			
				//mAirplane->addPitchForce(-PITCHFORCE_2);
				mNetworkManager->applyAirplaneInput(INPUTACTION_PITCHUP, false);
				break;
			}
			case KA__ROLL_LEFT:
			{
				//mAirplane->addRollForce(ROLLFORCE_2);
				mNetworkManager->applyAirplaneInput(INPUTACTION_ROLLLEFT, false);
				break;
			}
			case KA__ROLL_RIGHT:
			{
				//mAirplane->addRollForce(-ROLLFORCE_2);
				mNetworkManager->applyAirplaneInput(INPUTACTION_ROLLRIGHT, false);
				break;
			}
			case KA__LOOK_BACK:
			{
				mCamLookBack = false;
				mHUDManager->setCrosshairVisible(true);
				break;
			}

			case KA__FIRE1:
			{
				mNetworkManager->applyAirplaneInput(INPUTACTION_SHOT1, false);
				break;
			}
			case KA__DEBUGNEWTON:
			{
				mShowNewtonDebug = false;
				mWorld->getDebugger().hideDebugInformation();
				break;
			}		
			default:
				break;
			
		} // switch

		return true;
	}
	if(mInputState == INPUTSTATE_SPECTATOR)
	{
		switch (e.key)
		{
		case OIS::KC_UP:
		case OIS::KC_W:
			mDirection.z -= mMove;
			break;

		case OIS::KC_DOWN:
		case OIS::KC_S:
			mDirection.z -= -mMove;
			break;

		case OIS::KC_LEFT:
		case OIS::KC_A:
			mDirection.x -= mMove;
			break;

		case OIS::KC_RIGHT:
		case OIS::KC_D:
			mDirection.x -= -mMove;
			break;

		case OIS::KC_PGDOWN:
		case OIS::KC_Q:
			mDirection.y -= -mMove;
			break;

		case OIS::KC_PGUP:
		case OIS::KC_E:
			mDirection.y -= mMove;
			break;


		case OIS::KC_F3:
			mShowNewtonDebug = false;
			mWorld->getDebugger().hideDebugInformation();
			break;
			
		case OIS::KC_SPACE:
			Ogre::OverlayManager::getSingletonPtr()->getByName("RealToys/PopupMessageOverlay")->hide();
			if(mOriginalInputState == INPUTSTATE_INGAME)
			{
				if(mInterSenseMgr->isActive())
				{
					mInterSenseMgr->resetAngles();
				}

				mCamPosition = 2;
				nextCamera();

				mNetworkManager->requestAirplane();

				mInputState = INPUTSTATE_INGAME;

			}

		default:
			break;
			
		} // switch
		return true;
	}
	return true;
}




// JoystickListener
bool AppFrameListener::buttonPressed( const OIS::JoyStickEvent &arg, int button )
{
	if(mInputState == INPUTSTATE_INGAME)
	{
		switch(mInputMappingManager->getJoyButtonAction(button))
		{
			case JA__NEXT_CAM:
			{
				nextCamera();
				break;
			}
			case JA__DEC_THRUST:
			{
				mAirplane->decreaseThrust();
				break;
			}

			case JA__INC_THRUST:
			{
				mAirplane->increaseThrust();
				break;
			}	
			
			default:
				break;
		}

		return true;
	}
	return true;
}
bool AppFrameListener::buttonReleased( const OIS::JoyStickEvent &e, int button )
{
	return true;
}
bool AppFrameListener::axisMoved( const OIS::JoyStickEvent &e, int axis )
{
	if(mInputState == INPUTSTATE_INGAME)
	{
		int absPos = e.state.mAxes[axis].abs;
		
		switch(mInputMappingManager->getJoyAxisAction(axis))
		{
		case JA__THRUST_AXIS:
		{
			//adjust this axis so it is always positive with minimum = 0;
			absPos = absPos*-1 + OIS::JoyStick::MAX_AXIS;
			
			mAirplane->setThrustForce( ((THRUSTFORCE_3 - THRUSTFORCE_0) * absPos) /
				(OIS::JoyStick::MAX_AXIS + OIS::JoyStick::MIN_AXIS*-1) + THRUSTFORCE_0);	

			break;
		}
		case JA__PITCH_AXIS: 
		{
			if(absPos < 0)
			{
				mAirplane->setPitchForce( (PITCHFORCE_2 * absPos) / OIS::JoyStick::MIN_AXIS*-1);
			}
			else
			{
				mAirplane->setPitchForce( (PITCHFORCE_2 * absPos) / OIS::JoyStick::MAX_AXIS);
			}
			break;
		}
		case JA__ROLL_AXIS:
		{
			if(absPos < 0)
			{
				mAirplane->setRollForce( (ROLLFORCE_2 * absPos) / OIS::JoyStick::MIN_AXIS*-1);
			}
			else
			{
				mAirplane->setRollForce( (ROLLFORCE_2 * absPos) / OIS::JoyStick::MAX_AXIS);
			}
			break;
		}
		}
		return true;
	}
	return true;
}
bool AppFrameListener::sliderMoved( const OIS::JoyStickEvent &e, int sliderID )
{
	return true;
}
bool AppFrameListener::povMoved( const OIS::JoyStickEvent &e, int pov )
{
	return true;
}

// InterSense 
void AppFrameListener::processInterSenseInput( Ogre::Real timeSinceLastFrame )
{	
	InterSenseCube icube = mInterSenseMgr->capture(timeSinceLastFrame);
	
	if(mInputState == INPUTSTATE_INGAME && mAirplane)
	{		
		mAirplane->setPitchForce( (PITCHFORCE_2 * -icube.GetPitch()) / 45.0f); // people would not raise their head much more than 45degrees
		mAirplane->setRollForce ( (ROLLFORCE_2 * -icube.GetRoll()) / 45.0f);
	}
}