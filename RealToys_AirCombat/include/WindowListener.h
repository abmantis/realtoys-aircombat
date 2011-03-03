#ifndef WINDOWLISTENER_H
#define WINDOWLISTENER_H

#include "AppFrameListener.h"

class WindowListener: public Ogre::WindowEventListener
{
public:
	WindowListener(AppFrameListener* frameListener);
	~WindowListener(void);
};
#endif
