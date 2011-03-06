#include "StdAfx.h"
#include "InputMappingManager.h"
#include "RealToysShared.h"

InputMappingManager *InputMappingManager::mInputMappingManager;

InputMappingManager::InputMappingManager( void ) :
	mMouse( 0 ), mKeyboard( 0 ), mInputSystem( 0 ) 
{
}

InputMappingManager::~InputMappingManager( void ) {
	if( mInputSystem ) {
		if( mMouse ) {
			mInputSystem->destroyInputObject( mMouse );
			mMouse = 0;
		}

		if( mKeyboard ) {
			mInputSystem->destroyInputObject( mKeyboard );
			mKeyboard = 0;
		}

		if( mJoysticks.size() > 0 ) {
			itJoystick	= mJoysticks.begin();
			itJoystickEnd = mJoysticks.end();
			for(; itJoystick != itJoystickEnd; ++itJoystick ) {
				mInputSystem->destroyInputObject( *itJoystick );
			}

			mJoysticks.clear();
		}

		// If you use OIS1.0RC1 or above, uncomment this line
		// and comment the line below it
		mInputSystem->destroyInputSystem( mInputSystem );
		//mInputSystem->destroyInputSystem();
		mInputSystem = 0;
	}
}

void InputMappingManager::initialise( Ogre::RenderWindow *renderWindow ) {
	if( !mInputSystem ) {
		// Setup basic variables
		OIS::ParamList paramList;	
		size_t windowHnd = 0;
		std::ostringstream windowHndStr;

		// Get window handle
		renderWindow->getCustomAttribute( "WINDOW", &windowHnd );

		// Fill parameter list
		windowHndStr << (unsigned int) windowHnd;
		paramList.insert( std::make_pair( std::string( "WINDOW" ), windowHndStr.str() ) );

		// Create inputsystem
		mInputSystem = OIS::InputManager::createInputSystem( paramList );

		// If possible create a buffered keyboard
		if( mInputSystem->getNumberOfDevices(OIS::OISKeyboard) > 0 ) {
			mKeyboard = static_cast<OIS::Keyboard*>( mInputSystem->createInputObject( OIS::OISKeyboard, true ) );
		}

		// If possible create a buffered mouse
		if( mInputSystem->getNumberOfDevices(OIS::OISMouse) > 0 ) {
			mMouse = static_cast<OIS::Mouse*>( mInputSystem->createInputObject( OIS::OISMouse, true ) );

			// Get window size
			unsigned int width, height, depth;
			int left, top;
			renderWindow->getMetrics( width, height, depth, left, top );

			// Set mouse region
			this->setWindowExtents( width, height );
		}

		// If possible create all joysticks in buffered mode
		if( mInputSystem->getNumberOfDevices(OIS::OISJoyStick) > 0 ) {
			mJoysticks.resize( mInputSystem->getNumberOfDevices(OIS::OISJoyStick) );

			itJoystick	= mJoysticks.begin();
			itJoystickEnd = mJoysticks.end();
			for(; itJoystick != itJoystickEnd; ++itJoystick ) {
				(*itJoystick) = static_cast<OIS::JoyStick*>( mInputSystem->createInputObject( OIS::OISJoyStick, true ) );
				//(*itJoystick)->setEventCallback( this );
			}
		}
	}
	loadMappings();
}

void InputMappingManager::capture( void ) {
	// Need to capture / update each device every frame
	if( mMouse ) {
		mMouse->capture();
	}

	if( mKeyboard ) {
		mKeyboard->capture();
	}

	if( mJoysticks.size() > 0 ) {
		itJoystick	= mJoysticks.begin();
		itJoystickEnd = mJoysticks.end();
		for(; itJoystick != itJoystickEnd; ++itJoystick ) {
			(*itJoystick)->capture();
		}
	}
}

void InputMappingManager::setWindowExtents( int width, int height ) {
	// Set mouse region (if window resizes, we should alter this to reflect as well)
	const OIS::MouseState &mouseState = mMouse->getMouseState();
	mouseState.width  = width;
	mouseState.height = height;
}

bool InputMappingManager::setKeyListener(OIS::KeyListener *keyListener)
{
	if(!mKeyboard)
		return false;
	mKeyboard->setEventCallback(keyListener);
	return true;
}
bool InputMappingManager::setMouseListener( OIS::MouseListener *mouseListener)
{
	if(!mMouse)
		return false;
	mMouse->setEventCallback(mouseListener);
	return true;
}
bool InputMappingManager::setJoystickListener(OIS::JoyStickListener *joystickListener)
{
	bool joyFound = false;
	itJoystick	= mJoysticks.begin();
	itJoystickEnd = mJoysticks.end();
	for(; itJoystick != itJoystickEnd; ++itJoystick ) {
		(*itJoystick)->setEventCallback( joystickListener );
		joyFound = true;
	}
	return joyFound;
}
OIS::Mouse* InputMappingManager::getMouse( void ) {
	return mMouse;
}

OIS::Keyboard* InputMappingManager::getKeyboard( void ) {
	return mKeyboard;
}

OIS::JoyStick* InputMappingManager::getJoystick( unsigned int index ) {
	// Make sure it's a valid index
	if( index < mJoysticks.size() ) {
		return mJoysticks[ index ];
	}

	return 0;
}

int InputMappingManager::getNumOfJoysticks( void ) {
	// Cast to keep compiler happy ^^
	return (int) mJoysticks.size();
}


InputMappingManager* InputMappingManager::getSingletonPtr( void ) 
{
	if( !mInputMappingManager ) {
		mInputMappingManager = new InputMappingManager();
	}

	return mInputMappingManager;
}

void InputMappingManager::loadMappings()
{
	Ogre::ConfigFile configFile;
	Ogre::String confString;
	configFile.load(RealToys::configFileName);
	

	//load keyboard maps
	mKeyMapVec.resize(0xEE);
	for(int i = 0; i < 0xEE; i++)
	{
		mKeyMapVec[i] = KA__NO_ACTION;
	}
	
	confString = configFile.getSetting("PITCH_UP", "Keyboard", "KC_UNASSIGNED");
	addKeyMapFromString(confString, KA__PITCH_UP);

	confString = configFile.getSetting("PITCH_DOWN", "Keyboard", "KC_UNASSIGNED");
	addKeyMapFromString(confString, KA__PITCH_DOWN);

	confString = configFile.getSetting("ROLL_LEFT", "Keyboard", "KC_UNASSIGNED");
	addKeyMapFromString(confString, KA__ROLL_LEFT);

	confString = configFile.getSetting("ROLL_RIGHT", "Keyboard", "KC_UNASSIGNED");
	addKeyMapFromString(confString, KA__ROLL_RIGHT);

	confString = configFile.getSetting("THRUST_0", "Keyboard", "KC_UNASSIGNED");
	addKeyMapFromString(confString, KA__THRUST_0);

	confString = configFile.getSetting("THRUST_1", "Keyboard", "KC_UNASSIGNED");
	addKeyMapFromString(confString, KA__THRUST_1);

	confString = configFile.getSetting("THRUST_2", "Keyboard", "KC_UNASSIGNED");
	addKeyMapFromString(confString, KA__THRUST_2);

	confString = configFile.getSetting("THRUST_3", "Keyboard", "KC_UNASSIGNED");
	addKeyMapFromString(confString, KA__THRUST_3);

	confString = configFile.getSetting("INC_THRUST", "Keyboard", "KC_UNASSIGNED");
	addKeyMapFromString(confString, KA__INC_THRUST);
	
	confString = configFile.getSetting("DEC_THRUST", "Keyboard", "KC_UNASSIGNED");
	addKeyMapFromString(confString, KA__DEC_THRUST);

	confString = configFile.getSetting("NEXT_CAM", "Keyboard", "KC_UNASSIGNED");
	addKeyMapFromString(confString, KA__NEXT_CAM);

	confString = configFile.getSetting("LOOK_BACK", "Keyboard", "KC_UNASSIGNED");
	addKeyMapFromString(confString, KA__LOOK_BACK);

	confString = configFile.getSetting("FIRE1", "Keyboard", "KC_UNASSIGNED");
	addKeyMapFromString(confString, KA__FIRE1);

	mKeyMapVec[OIS::KC_F7] = KA__DEC_SOUND;
	mKeyMapVec[OIS::KC_F8] = KA__INC_SOUND;	
	mKeyMapVec[OIS::KC_F9] = KA__DEBUGPANEL;
	mKeyMapVec[OIS::KC_F10] = KA__DEBUGDRAWMODE;
	mKeyMapVec[OIS::KC_F11] = KA__DEBUGNEWTON;
	mKeyMapVec[OIS::KC_F12] = KA__SCREEN_SHOT;

	mKeyMapVec[OIS::KC_ESCAPE] =  KA__EXIT;


	//load joystick map
	mJoyAxisMapVec.resize(10);
	for(int i = 0; i < 10; i++)
	{
		mJoyAxisMapVec[i] = JA__NO_ACTION;
	}
	mJoyButtonMapVec.resize(100);
	for(int i = 0; i < 100; i++)
	{
		mJoyButtonMapVec[i] = JA__NO_ACTION;
	}
	confString = configFile.getSetting("THRUST_AXIS", "Joystick", "-1");
	addJoyAxisMapFromString(confString, JA__THRUST_AXIS);

	confString = configFile.getSetting("PITCH_AXIS", "Joystick", "-1");
	addJoyAxisMapFromString(confString, JA__PITCH_AXIS);

	confString = configFile.getSetting("ROLL_AXIS", "Joystick", "-1");
	addJoyAxisMapFromString(confString, JA__ROLL_AXIS);

	confString = configFile.getSetting("NEXT_CAM", "Joystick", "-1");
	addJoyButtonMapFromString(confString, JA__NEXT_CAM);

	confString = configFile.getSetting("INC_THRUST", "Joystick", "-1");
	addJoyButtonMapFromString(confString, JA__INC_THRUST);

	confString = configFile.getSetting("DEC_THRUST", "Joystick", "-1");
	addJoyButtonMapFromString(confString, JA__DEC_THRUST);
	

}
void InputMappingManager::addKeyMapFromString(Ogre::String keyMaps, KeyboardActions action)
{
	Ogre::String keycode;
	Ogre::String aux;
	Ogre::uint pos;
	
	pos = keyMaps.find(';');
	while(pos != -1)
	{
		aux = keyMaps.substr(0, pos);
		mKeyMapVec[convertFromString(aux)] = action;
		aux = keyMaps.substr(pos+1);
		keyMaps = aux;
		pos = keyMaps.find(';');
	}
	mKeyMapVec[convertFromString(keyMaps)] = action;
	
}
void InputMappingManager::addJoyAxisMapFromString(Ogre::String joyMap, JoystickActions action)
{
	if(joyMap == "")
		return;
	int intJoyMap;
	intJoyMap = Ogre::StringConverter::parseInt(joyMap);

	if(intJoyMap > -1)
		mJoyAxisMapVec[intJoyMap] = action;	
}
void InputMappingManager::addJoyButtonMapFromString(Ogre::String joyMap, JoystickActions action)
{
	int intJoyMap;
	intJoyMap = Ogre::StringConverter::parseInt(joyMap);

	if(intJoyMap > -1)
		mJoyButtonMapVec[intJoyMap] = action;	
}
OIS::KeyCode InputMappingManager::convertFromString(Ogre::String keyCode)
{
	if(keyCode == "KC_UNASSIGNED")
		return OIS::KC_UNASSIGNED;
	else if(keyCode == "KC_ESCAPE")
		return OIS::KC_ESCAPE;
	else if(keyCode == "KC_1")
		return OIS::KC_1;
	else if(keyCode == "KC_2")
		return OIS::KC_2;
	else if(keyCode == "KC_3")
		return OIS::KC_3;
	else if(keyCode == "KC_4")
		return OIS::KC_4;
	else if(keyCode == "KC_5")
		return OIS::KC_5;
	else if(keyCode == "KC_6")
		return OIS::KC_6;
	else if(keyCode == "KC_7")
		return OIS::KC_7;
	else if(keyCode == "KC_8")
		return OIS::KC_8;
	else if(keyCode == "KC_9")
		return OIS::KC_9;
	else if(keyCode == "KC_0")
		return OIS::KC_0;
	else if(keyCode == "KC_MINUS" )
		return OIS::KC_MINUS;
	else if(keyCode == "KC_EQUALS")
		return OIS::KC_EQUALS;
	else if(keyCode == "KC_BACK")
		return OIS::KC_BACK;
	else if(keyCode == "KC_TAB")
		return OIS::KC_TAB;
	else if(keyCode == "KC_Q")
		return OIS::KC_Q;
	else if(keyCode == "KC_W")
		return OIS::KC_W;
	else if(keyCode == "KC_E")
		return OIS::KC_E;
	else if(keyCode == "KC_R")
		return OIS::KC_R;
	else if(keyCode == "KC_T")
		return OIS::KC_T;
	else if(keyCode == "KC_Y")
		return OIS::KC_Y;
	else if(keyCode == "KC_U")
		return OIS::KC_U;
	else if(keyCode == "KC_I")
		return OIS::KC_I;
	else if(keyCode == "KC_O")
		return OIS::KC_O;
	else if(keyCode == "KC_P")
		return OIS::KC_P;
	else if(keyCode == "KC_LBRACKET")
		return OIS::KC_LBRACKET;
	else if(keyCode == "KC_RBRACKET")
		return OIS::KC_RBRACKET;
	else if(keyCode == "KC_RETURN")
		return OIS::KC_RETURN;
	else if(keyCode == "KC_LCONTROL")
		return OIS::KC_LCONTROL;
	else if(keyCode == "KC_A")
		return OIS::KC_A;
	else if(keyCode == "KC_S")
		return OIS::KC_S;
	else if(keyCode == "KC_D")
		return OIS::KC_D;
	else if(keyCode == "KC_F")
		return OIS::KC_F;
	else if(keyCode == "KC_G")
		return OIS::KC_G;
	else if(keyCode == "KC_H")
		return OIS::KC_H;
	else if(keyCode == "KC_J")
		return OIS::KC_J;
	else if(keyCode == "KC_K")
		return OIS::KC_K;
	else if(keyCode == "KC_L")
		return OIS::KC_L;
	else if(keyCode == "KC_SEMICOLON")
		return OIS::KC_SEMICOLON;
	else if(keyCode == "KC_APOSTROPHE")
		return OIS::KC_APOSTROPHE;
	else if(keyCode == "KC_GRAVE")
		return OIS::KC_GRAVE;
	else if(keyCode == "KC_LSHIFT")
		return OIS::KC_LSHIFT;
	else if(keyCode == "KC_BACKSLASH")
		return OIS::KC_BACKSLASH;
	else if(keyCode == "KC_Z")
		return OIS::KC_Z;
	else if(keyCode == "KC_X")
		return OIS::KC_X;
	else if(keyCode == "KC_C")
		return OIS::KC_C;
	else if(keyCode == "KC_V")
		return OIS::KC_V;
	else if(keyCode == "KC_B")
		return OIS::KC_B;
	else if(keyCode == "KC_N")
		return OIS::KC_N;
	else if(keyCode == "KC_M")
		return OIS::KC_M;
	else if(keyCode == "KC_COMMA")
		return OIS::KC_COMMA;
	else if(keyCode == "KC_PERIOD")
		return OIS::KC_PERIOD;
	else if(keyCode == "KC_SLASH")
		return OIS::KC_SLASH;
	else if(keyCode == "KC_RSHIFT")
		return OIS::KC_RSHIFT;
	else if(keyCode == "KC_MULTIPLY")
		return OIS::KC_MULTIPLY;
	else if(keyCode == "KC_LMENU")
		return OIS::KC_LMENU;
	else if(keyCode == "KC_SPACE")
		return OIS::KC_SPACE;
	else if(keyCode == "KC_CAPITAL")
		return OIS::KC_CAPITAL;
	//else if(keyCode == "KC_F1")
	//	return OIS::KC_F1;
	//else if(keyCode == "KC_F2")
	//	return OIS::KC_F2;
	//else if(keyCode == "KC_F3")
	//	return OIS::KC_F3;
	//else if(keyCode == "KC_F4")
	//	return OIS::KC_F4;
	//else if(keyCode == "KC_F5")
	//	return OIS::KC_F5;
	//else if(keyCode == "KC_F6")
	//	return OIS::KC_F6;
	//else if(keyCode == "KC_F7")
	//	return OIS::KC_F7;
	//else if(keyCode == "KC_F8")
	//	return OIS::KC_F8;
	//else if(keyCode == "KC_F9")
	//	return OIS::KC_F9;
	//else if(keyCode == "KC_F10")
	//	return OIS::KC_F10;
	else if(keyCode == "KC_NUMLOCK")
		return OIS::KC_NUMLOCK;
	else if(keyCode == "KC_SCROLL")
		return OIS::KC_SCROLL;
	else if(keyCode == "KC_NUMPAD7")
		return OIS::KC_NUMPAD7;
	else if(keyCode == "KC_NUMPAD8")
		return OIS::KC_NUMPAD8;
	else if(keyCode == "KC_NUMPAD9")
		return OIS::KC_NUMPAD9;
	else if(keyCode == "KC_SUBTRACT")
		return OIS::KC_SUBTRACT;
	else if(keyCode == "KC_NUMPAD4")
		return OIS::KC_NUMPAD4;
	else if(keyCode == "KC_NUMPAD5")
		return OIS::KC_NUMPAD5;
	else if(keyCode == "KC_NUMPAD6")
		return OIS::KC_NUMPAD6;
	else if(keyCode == "KC_ADD")
		return OIS::KC_ADD;
	else if(keyCode == "KC_NUMPAD1")
		return OIS::KC_NUMPAD1;
	else if(keyCode == "KC_NUMPAD2")
		return OIS::KC_NUMPAD2;
	else if(keyCode == "KC_NUMPAD3")
		return OIS::KC_NUMPAD3;
	else if(keyCode == "KC_NUMPAD0")
		return OIS::KC_NUMPAD0;
	else if(keyCode == "KC_DECIMAL")
		return OIS::KC_DECIMAL;
	else if(keyCode == "KC_OEM_102")
		return OIS::KC_OEM_102;
	else if(keyCode == "KC_F11")
		return OIS::KC_F11;
	else if(keyCode == "KC_F12")
		return OIS::KC_F12;
	else if(keyCode == "KC_F13")
		return OIS::KC_F13;
	else if(keyCode == "KC_F14")
		return OIS::KC_F14;
	else if(keyCode == "KC_F15")
		return OIS::KC_F15;	
	else if(keyCode == "KC_ABNT_C1")
		return OIS::KC_ABNT_C1;
	else if(keyCode == "KC_ABNT_C2")
		return OIS::KC_ABNT_C2;
	else if(keyCode == "KC_NUMPADENTER")
		return OIS::KC_NUMPADENTER;
	else if(keyCode == "KC_RCONTROL")
		return OIS::KC_RCONTROL;
	else if(keyCode == "KC_DIVIDE")
		return OIS::KC_DIVIDE;
	else if(keyCode == "KC_SYSRQ")
		return OIS::KC_SYSRQ;
	else if(keyCode == "KC_RMENU")
		return OIS::KC_RMENU;
	else if(keyCode == "KC_HOME")
		return OIS::KC_HOME;
	else if(keyCode == "KC_UP")
		return OIS::KC_UP;
	else if(keyCode == "KC_PGUP")
		return OIS::KC_PGUP;
	else if(keyCode == "KC_LEFT")
		return OIS::KC_LEFT;
	else if(keyCode == "KC_RIGHT")
		return OIS::KC_RIGHT;
	else if(keyCode == "KC_END")
		return OIS::KC_END;
	else if(keyCode == "KC_DOWN")
		return OIS::KC_DOWN;
	else if(keyCode == "KC_PGDOWN")
		return OIS::KC_PGDOWN;
	else if(keyCode == "KC_INSERT")
		return OIS::KC_INSERT;
	else if(keyCode == "KC_DELETE")
		return OIS::KC_DELETE;
	else if(keyCode == "KC_LWIN")
		return OIS::KC_LWIN;
	else if(keyCode == "KC_RWIN")
		return OIS::KC_RWIN;

	return OIS::KC_UNASSIGNED;
}