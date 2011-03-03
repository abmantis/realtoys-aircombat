#include "StdAfx.h"
#ifndef _CUSTOM_OBJECTS_H_
#define _CUSTOM_OBJECTS_H_

class ShotLine : public Ogre::ManualObject
{
public:
	ShotLine(const Ogre::String &name)
		: ManualObject(name)
	{
		this->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_LIST);
			this->position(0,0,0);
			this->position(0,0,5);
		this->end();

	}		
};

#endif