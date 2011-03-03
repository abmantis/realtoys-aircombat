#include "StdAfx.h"
#include "NewtonFrameListener.h"
#include "RealToysShared.h"
#include "Shots.h"
#include "DynamicObjectsManager.h"

NewtonFrameListener::NewtonFrameListener( Ogre::SceneManager* mgr, OgreNewt::World* W, int update_framerate) :
		FrameListener()
{
	m_World = W;
	desired_framerate = update_framerate;
	
	m_update = (Ogre::Real)(1.0f / (Ogre::Real)desired_framerate);
	m_elapsed = 0.0f;

	// add the standard debug viewer.
	m_World->getDebugger().init(mgr);
	
	m_World->setLeaveWorldCallback<NewtonFrameListener>(&NewtonFrameListener::LeaveWorldCallback, this);
}

NewtonFrameListener::~NewtonFrameListener(void)
{
}

void NewtonFrameListener::update(Ogre::Real timeSinceLastFrame)
{
	m_elapsed += timeSinceLastFrame;


	// loop through and update as many times as necessary (up to 10 times maximum).
	if ((m_elapsed > m_update) && (m_elapsed < (m_update * 10)) )
	{
		while (m_elapsed > m_update)
		{
			m_World->update( m_update );
			m_elapsed -= m_update;
		}
	}
	else
	{
		if (m_elapsed < (m_update))
		{
			// not enough time has passed this loop, so ignore for now.
		}
		else
		{
			// too much time has passed (would require more than 10 updates!), so just update once and reset.
			// this often happens on the first frame of a game, where assets and other things were loading, then
			// the elapsed time since the last drawn frame is very long.
			m_World->update( m_elapsed );
			m_elapsed = 0.0f; // reset the elapsed time so we don't become "eternally behind".
		}
	}
}

void NewtonFrameListener::LeaveWorldCallback(OgreNewt::Body* body, int threadIndex)
{
	std::cout << "Body of type ";
	switch(body->getType())
	{
	case RealToys::BODYTYPE_SHOT1:
		{
			std::cout << "BODYTYPE_SHOT1";
			DynamicObjectsManager::getSingletonPtr()->addToDeleteList(Ogre::any_cast<Shot1*>(body->getUserData()) );
			break;
		}
	default:
		{
			std::cout << body->getType();
			break;
		}
	}
	std::cout << " has fired LeavWorldCallback." << std::endl;
}