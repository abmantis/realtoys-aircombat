#include "StdAfx.h"
#include "AirplaneMarkersManager.h"


template<> AirplaneMarkersManager* Ogre::Singleton<AirplaneMarkersManager>::ms_Singleton = 0;

AirplaneMarkersManager::AirplaneMarkersManager(Ogre::SceneManager *sceneManager)
:mSceneMgr(sceneManager) 
{
	/* initialize random seed: */
	srand ( time(NULL) );

	mVolQuery = mSceneMgr->createPlaneBoundedVolumeQuery(Ogre::PlaneBoundedVolumeList());
	mVolQuery->setQueryMask(RealToys::MASK_AIRPLANE);

}

AirplaneMarkersManager::~AirplaneMarkersManager(void)
{
	mPositionOrientationVector.clear();
	mSceneMgr->destroyQuery(mVolQuery);
}
AirplaneMarkersManager& AirplaneMarkersManager::getSingleton(void)
{
	assert( ms_Singleton );  return ( *ms_Singleton );  
}
AirplaneMarkersManager* AirplaneMarkersManager::getSingletonPtr(void)
{
	return ms_Singleton;
}

bool AirplaneMarkersManager::addMarker(Ogre::Vector3 position, Ogre::Quaternion orientation)
{
	if(mPositionOrientationVector.size() >= 10)
		return false;
	std::pair<Ogre::Vector3, Ogre::Quaternion> posori;
	posori.first = position;
	posori.second = orientation;
	mPositionOrientationVector.push_back(posori);

	return true;
}

void AirplaneMarkersManager::clearAll()
{
	mPositionOrientationVector.clear();
}

int AirplaneMarkersManager::getMarkerCount()
{
	return (int) mPositionOrientationVector.size();
}

bool AirplaneMarkersManager::getMarkerPositionOrientation(int index, 
	Ogre::Vector3 &position, Ogre::Quaternion &orientation)
{
	if(index > -1 && index < getMarkerCount())
	{
		std::pair<Ogre::Vector3, Ogre::Quaternion> posori;

		posori = mPositionOrientationVector[index];

		position = posori.first;
		orientation = posori.second;

		return true;
	}
	return false;
}

bool AirplaneMarkersManager::getMarkerPositionOrientation(Ogre::Vector3 &position, Ogre::Quaternion &orientation)
{
	//fill list with valid position indexes
	std::list<int> indexList;
	std::list<int>::iterator it;
	int currPos;

	Ogre::Vector3 pos;
	Ogre::Quaternion ori;

	for(int i = 0; i < getMarkerCount(); i++)
	{
		indexList.push_back(i);
	}
	
	while(!indexList.empty())
	{
		//go trough all indexes until it founds a valid one (with no airplanes close to it)
		currPos = rand()%indexList.size();
		it = indexList.begin();
		
		for( ; currPos>0 && it != indexList.end(); currPos-- )
		{
			it++;
		}	
				
		if(getMarkerPositionOrientation((*it), pos, ori))
		{			
			if(isMarkerPositionValid(pos))
			{				
				position = pos;
				orientation = ori;
				break;
			}
		}

		indexList.erase(it);
	}


	return false;

}

bool AirplaneMarkersManager::isMarkerPositionValid(Ogre::Vector3 position)
{
	Ogre::Real distfromcenter = 15;
	
	Ogre::Vector3 directionV = Ogre::Vector3::NEGATIVE_UNIT_Y;

	Ogre::Vector3 topLeftV(position.x - distfromcenter, position.y + distfromcenter, position.z + distfromcenter);
	Ogre::Vector3 topRightV(position.x + distfromcenter, position.y + distfromcenter, position.z + distfromcenter);
	Ogre::Vector3 bottomLeftV(position.x - distfromcenter, position.y + distfromcenter, position.z - distfromcenter);
	Ogre::Vector3 bottomRightV(position.x + distfromcenter, position.y + distfromcenter, position.z - distfromcenter);

	Ogre::Ray topLeft;
	Ogre::Ray topRight;
	Ogre::Ray bottomLeft;
	Ogre::Ray bottomRight;


	topLeft.setOrigin(topLeftV);
	topLeft.setDirection(directionV);

	topRight.setOrigin(topRightV);
	topRight.setDirection(directionV);

	bottomLeft.setOrigin(bottomLeftV);
	bottomLeft.setDirection(directionV);

	bottomRight.setOrigin(bottomRightV);
	bottomRight.setDirection(directionV);

	Ogre::AxisAlignedBox aabb;
	
	Ogre::PlaneBoundedVolume vol;
	vol.planes.push_back(Ogre::Plane( bottomRight.getOrigin(), topRight.getOrigin(), topLeft.getOrigin() ));			// top plane
	vol.planes.push_back(Ogre::Plane( topRight.getPoint(100), topLeft.getPoint(100), topLeft.getOrigin() ));			// front plane
	vol.planes.push_back(Ogre::Plane( topLeft.getPoint(100), bottomLeft.getPoint(100), topLeft.getOrigin() ));			// left plane
	vol.planes.push_back(Ogre::Plane( bottomLeft.getPoint(100), bottomRight.getPoint(100), bottomLeft.getOrigin() ));	// back plane
	vol.planes.push_back(Ogre::Plane( bottomRight.getPoint(100), topRight.getPoint(100), topRight.getOrigin() ));		// right plane
	vol.planes.push_back(Ogre::Plane( topLeft.getPoint(distfromcenter*2), topRight.getPoint(distfromcenter*2), bottomRight.getPoint(distfromcenter*2) ));			// bottom plane


	//this->begin("", Ogre::RenderOperation::OT_TRIANGLE_STRIP);
	//this->position(bottomRight.getOrigin());
	//this->position(topRight.getOrigin());
	//this->position(topLeft.getOrigin());
	//this->end();
	//
	//this->begin("", Ogre::RenderOperation::OT_TRIANGLE_STRIP);
	//this->position(topRight.getPoint(distfromcenter*2));
	//this->position(topLeft.getPoint(distfromcenter*2));
	//this->position(topLeft.getOrigin());		
	//this->end();

	//this->begin("", Ogre::RenderOperation::OT_TRIANGLE_STRIP);
	//this->position(topLeft.getPoint(distfromcenter*2));
	//this->position(bottomLeft.getPoint(distfromcenter*2));
	//this->position(topLeft.getOrigin());
	//this->end();

	//this->begin("", Ogre::RenderOperation::OT_TRIANGLE_STRIP);
	//this->position(bottomLeft.getPoint(distfromcenter*2));
	//this->position(bottomRight.getPoint(distfromcenter*2));
	//this->position(bottomLeft.getOrigin());
	//this->end();

	//this->begin("", Ogre::RenderOperation::OT_TRIANGLE_STRIP);
	//this->position(bottomRight.getPoint(distfromcenter*2));
	//this->position(topRight.getPoint(distfromcenter*2));
	//this->position(topRight.getOrigin());
	//this->end();

	//this->begin("", Ogre::RenderOperation::OT_TRIANGLE_STRIP);	
	//this->position(topLeft.getPoint(distfromcenter*2));	
	//this->position(topRight.getPoint(distfromcenter*2));
	//this->position(bottomRight.getPoint(distfromcenter*2));
	//this->end();

	//mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(this);

	Ogre::PlaneBoundedVolumeList volList;
	volList.push_back(vol);
	
	mVolQuery->setVolumes(volList);	
	Ogre::SceneQueryResult result = mVolQuery->execute();

	
	//Ogre::SceneQueryResultMovableList::iterator itr;
	//for (itr = result.movables.begin(); itr != result.movables.end(); ++itr)
	//{		
	//	std::cout <<"    "<< (*itr)->getName() << std::endl;		
	//}

	if(result.movables.size() != 0)
	{
		std::cout << "INVALID AIRPLANE POSITION" << std::endl;		
		return false;
	}
	
	return true;
}