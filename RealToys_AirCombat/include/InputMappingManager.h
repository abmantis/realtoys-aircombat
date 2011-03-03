#ifndef _INPUTMAPPINGMANAGER_H_
#define _INPUTMAPPINGMANAGER_H_

enum InputStates
{
	INPUTSTATE_INGAME,
	INPUTSTATE_SPECTATOR
};

enum KeyboardActions
{
	KA__PITCH_UP,
	KA__PITCH_DOWN,
	KA__ROLL_LEFT,
	KA__ROLL_RIGHT,
	KA__THRUST_0,
	KA__THRUST_1,
	KA__THRUST_2,
	KA__THRUST_3,
	KA__INC_THRUST,
	KA__DEC_THRUST,
	KA__NEXT_CAM,
	KA__LOOK_BACK,
	KA__FIRE1,
	KA__EXIT,
	KA__DEBUGNEWTON,
	KA__DEBUGDRAWMODE,
	KA__DEBUGPANEL,
	KA__DEC_SOUND,
	KA__INC_SOUND,
	KA__SCREEN_SHOT,
	KA__NO_ACTION
};

enum JoystickActions
{
	JA__PITCH_AXIS,
	JA__ROLL_AXIS,
	JA__THRUST_AXIS,
	JA__NEXT_CAM,	
	JA__INC_THRUST,
	JA__DEC_THRUST,
	JA__NO_ACTION
};

class InputMappingManager
{
public:
	~InputMappingManager( void );

	void initialise( Ogre::RenderWindow *renderWindow );
	void capture( void );

	void setWindowExtents( int width, int height );

    bool setKeyListener( OIS::KeyListener *keyListener);
    bool setMouseListener( OIS::MouseListener *mouseListener);
    bool setJoystickListener( OIS::JoyStickListener *joystickListener);

	OIS::Mouse*	getMouse( void );
	OIS::Keyboard* getKeyboard( void );
	OIS::JoyStick* getJoystick( unsigned int index );

	int getNumOfJoysticks( void );

	KeyboardActions getKeyAction(OIS::KeyCode keycode){ return mKeyMapVec[keycode]; };
	JoystickActions getJoyAxisAction(int axis){ return mJoyAxisMapVec[axis]; };
	JoystickActions getJoyButtonAction(int button){ return mJoyButtonMapVec[button]; };

	static InputMappingManager* getSingletonPtr( void );

private:
	static InputMappingManager *mInputMappingManager;

	OIS::Mouse			*mMouse;
	OIS::Keyboard		*mKeyboard;
	OIS::InputManager	*mInputSystem;

	std::vector<OIS::JoyStick*>				mJoysticks;
	std::vector<OIS::JoyStick*>::iterator	itJoystick;
	std::vector<OIS::JoyStick*>::iterator	itJoystickEnd;

	
	std::vector<KeyboardActions>	mKeyMapVec;
	std::vector<JoystickActions>	mJoyAxisMapVec;
	std::vector<JoystickActions>	mJoyButtonMapVec;



	InputMappingManager( void );
	InputMappingManager( const InputMappingManager& ) { }
	InputMappingManager & operator = ( const InputMappingManager& );

	void loadMappings();
	void addKeyMapFromString(Ogre::String keyMaps, KeyboardActions action);
	void addJoyAxisMapFromString(Ogre::String joyMap, JoystickActions action);
	void addJoyButtonMapFromString(Ogre::String joyMap, JoystickActions action);
	OIS::KeyCode convertFromString(Ogre::String keyCode);



};


#endif
