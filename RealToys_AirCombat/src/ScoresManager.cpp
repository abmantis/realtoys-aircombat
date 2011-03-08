#include "StdAfx.h"
#include "ScoresManager.h"
#include "NetworkManager.h"

ScoresManager::ScoresManager()
{
	mLocalInfo = NULL;
	mFullHealth = 100;
}

ScoresManager::~ScoresManager(void)
{
}

ScoresManager::PlayerScoreInfo::PlayerScoreInfo()
{	
	mCurrentHealth = mKills = mDeaths = -1;
}
ScoresManager::PlayerScoreInfo::PlayerScoreInfo(const PlayerScoreInfo &info)
{
	mCurrentHealth = info.mCurrentHealth;
	mDeaths = info.mDeaths;
	mKills = info.mKills;
	mID = info.mID;
}
ScoresManager::PlayerScoreInfo::PlayerScoreInfo(PlayerID id, int health, int kills, int deaths)
: mID(id), mCurrentHealth(health), mKills(kills), mDeaths(deaths) {}
ScoresManager::PlayerScoreInfo::~PlayerScoreInfo() {}

const ScoresManager::PlayerScoreInfo* ScoresManager::addPlayer(PlayerID id)
{
	PlayerScoreInfo *info = findPlayer(id);
	//check if it exists already
	if(info)
	{
		info->mID = id;
		info->mCurrentHealth = mFullHealth;
		info->mDeaths = 0;
		info->mKills = 0;
	}
	else
	{
		info = new PlayerScoreInfo(id, mFullHealth, 0, 0);
		mScoreInfoList.push_back( info );
	}

	//check if it's the local guy
	if(id == mLocalID)
		mLocalInfo = info;

	return info;
}
void ScoresManager::removePlayer(PlayerID id)
{
	for (ScoreInfoList::iterator it = mScoreInfoList.begin(); it != mScoreInfoList.end(); it++)
	{	
		if((*it)->mID == id)
		{
			mScoreInfoList.erase(it);
			return;
		}
	}
}
ScoresManager::PlayerScoreInfo* ScoresManager::findPlayer(PlayerID id)
{
	for (ScoreInfoList::iterator it = mScoreInfoList.begin(); it != mScoreInfoList.end(); it++)
	{	
		if((*it)->mID == id)
		{			
			return (*it);
		}
	}
	return NULL;
}

const ScoresManager::PlayerScoreInfo* ScoresManager::getPlayer(PlayerID id)
{
	for (ScoreInfoList::iterator it = mScoreInfoList.begin(); it != mScoreInfoList.end(); it++)
	{	
		if((*it)->mID == id)
		{			
			return (*it);
		}
	}

	//player not found, create it!
	
	return addPlayer(id);
}

void ScoresManager::addDamage(PlayerID attackerID, PlayerID victimID, int damage)
{

	//damage is managed by server
	if(!mServer)
		return;

	// try to find both attacker and victim
	PlayerScoreInfo *attacker = NULL, *victim = NULL;
	for (ScoreInfoList::iterator it = mScoreInfoList.begin(); it != mScoreInfoList.end(); it++)
	{	
		if( attacker == NULL && (*it)->mID == attackerID )
		{
			attacker = (*it);
			if(victim != NULL)
			{
				break;
			}
		}
		else if ( victim == NULL && (*it)->mID == victimID )
		{
			victim = (*it);
			if(attacker != NULL)
			{
				break;
			}
		}
	}


	//make sure that the victim was found and that it is not a "zombie" (AKA player without health)
	if(victim != NULL && victim->mCurrentHealth > 0)
	{
		victim->mCurrentHealth -= damage;
//		std::cout << RakNet::GetTime() << "  " << victim->mID.ToString() << " -- " << victim->mCurrentHealth  << std::endl;
		if(victim->mCurrentHealth <= 0)
		{
			mDeadList.push_back( new PlayerScoreInfo(*victim) );
			victim->mDeaths++;

			if(attacker != NULL)
			{
				attacker->mKills++;
			}
		}
	}
}

void ScoresManager::addDamage(PlayerID playerID, int damage)
{
	//damage is managed by server
	if(!mServer)
		return;

	// try to find both attacker and victim
	PlayerScoreInfo *victim = NULL;
	victim = findPlayer(playerID);


	//make sure that the victim was found and that it is not a "zombie" (AKA player without health)
	if(victim != NULL && victim->mCurrentHealth > 0)
	{
		victim->mCurrentHealth -= damage;
		//		std::cout << RakNet::GetTime() << "  " << victim->mID.ToString() << " -- " << victim->mCurrentHealth  << std::endl;
		if(victim->mCurrentHealth <= 0)
		{
			mDeadList.push_back( new PlayerScoreInfo(*victim) );
			victim->mDeaths++;
		}
	}
}

void ScoresManager::resetHealth(PlayerID playerID)
{
	//damage is managed by server
	if(!mServer)
		return;

	PlayerScoreInfo *info = findPlayer(playerID);
	if(info)
	{
		info->mCurrentHealth = mFullHealth;
	}
}

ScoresManager::ScoreInfoList ScoresManager::getDeadList()
{
	ScoreInfoList returnList = mDeadList;
	mDeadList.clear();
	return returnList;
}

void ScoresManager::updatePlayer(ScoresManager::PlayerScoreInfo info)
{	
	PlayerScoreInfo *listInfo;
	listInfo = findPlayer(info.mID);

	RakAssert(listInfo && "updated player need to be added already!");

	listInfo->mCurrentHealth = info.mCurrentHealth;
	listInfo->mDeaths = info.mDeaths;
	listInfo->mKills = info.mKills;
}



void ScoresManager::setLocalSysAddress(PlayerID id)
{
	if(mLocalInfo == NULL)
	{
		mLocalID = id;

		//see if the local player has been added yet
		PlayerScoreInfo *testInfo = findPlayer(id);
		if(testInfo)
			mLocalInfo = testInfo;

		/*if(id == RealToys::serverPlayerID)
			mServer = true;
		else
			mServer = false;*/
		mServer = NetworkManager::getSingletonPtr()->isServer();
	}
}