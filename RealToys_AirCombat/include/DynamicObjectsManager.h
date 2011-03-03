#ifndef _DYNAMICOBJECTSMANAGER_H_
#define _DYNAMICOBJECTSMANAGER_H_

#include "StdAfx.h"

class DynamicObject
{
public:
	DynamicObject(void) { mInDeleteList = false; }
	virtual ~DynamicObject(void){}

	void setToDeleteListMode() { mInDeleteList = true; }
	bool isOnDeleteListMode() { return mInDeleteList; }

private:
	bool mInDeleteList;
};


class DynamicObjectsManager: public Ogre::Singleton<DynamicObjectsManager>
{
public:
	DynamicObjectsManager()
	{
		mListSize = 0;
		mListIndex = 0;
		
	}
	~DynamicObjectsManager(){}

	void addToDeleteList(DynamicObject *dyn_obj)
	{
		////check if this is not on the list yet
		//for(mListIndex = 0; mListIndex < mListSize; mListIndex++)
		//{
		//	if(dyn_obj == mDeleteList[mListIndex])
		//		return;
		//}				
		 
		if(dyn_obj->isOnDeleteListMode())
			return;

		mDeleteList.push_back(dyn_obj);
		mListSize++;
		dyn_obj->setToDeleteListMode();
	}

	void deleteAll()
	{		
		while(mListSize>0)
		{
			mDynObj = mDeleteList.back();
			mDeleteList.pop_back();
			mListSize--;
			delete mDynObj;
		}
	}


	static DynamicObjectsManager& getSingleton(void) { assert( ms_Singleton );  return ( *ms_Singleton ); }
	static DynamicObjectsManager* getSingletonPtr(void) { return ms_Singleton; }
private:
	std::vector<DynamicObject*> mDeleteList;
	unsigned int mListSize;
	unsigned int mListIndex;

	DynamicObject *mDynObj;		//DELETE THIS!
};
template<> DynamicObjectsManager* Ogre::Singleton<DynamicObjectsManager>::ms_Singleton = 0;

#endif