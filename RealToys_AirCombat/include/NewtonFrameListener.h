#ifndef _NEWTONFRAMELISTENER_H_
#define _NEWTONFRAMELISTENER_H_


//! simple frame listener to update the physics.
/*!
	updates the Newton World at the specified rate, with time-slicing, and
	also implements a simple debug view, press F3 to render Newto world with lines in 3D view.
*/
class NewtonFrameListener : public Ogre::FrameListener
{
protected:
	OgreNewt::World* m_World;
	
	int desired_framerate;
	Ogre::Real m_update, m_elapsed;

	void LeaveWorldCallback(OgreNewt::Body* body, int threadIndex);

public:
	NewtonFrameListener(Ogre::SceneManager* mgr, OgreNewt::World* W, int update_framerate = 60);
	~NewtonFrameListener(void);

	void update(Ogre::Real timeSinceLastFrame);


};
#endif