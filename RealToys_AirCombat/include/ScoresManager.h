#ifndef _SCORESMANAGER_H_
#define _SCORESMANAGER_H_

#include "RealToysShared.h"

class ScoresManager : public Ogre::Singleton<ScoresManager>
{
public:
	class PlayerScoreInfo
	{
	public:
		PlayerScoreInfo();
		PlayerScoreInfo( const PlayerScoreInfo &info);
		PlayerScoreInfo(PlayerID id, int health, int kills, int deaths);
		~PlayerScoreInfo();

		PlayerID mID;
		int mCurrentHealth;
		int mKills;
		int mDeaths;

		bool operator != ( const PlayerScoreInfo &right)
		{
			return (mCurrentHealth != right.mCurrentHealth ||
				mDeaths != right.mDeaths || 
				mKills != right.mKills);
		}
	};

	typedef std::list<PlayerScoreInfo*> ScoreInfoList;

	ScoresManager();
	~ScoresManager(void);

	void setLocalSysAddress(PlayerID id);
	const PlayerScoreInfo* getLocalInfo() { return mLocalInfo; }

	const PlayerScoreInfo* addPlayer(PlayerID id);
	void removePlayer(PlayerID id);

	/*
		only on server
	*/
	void addDamage(PlayerID attackerID, PlayerID victimID, int damage);
	/*
		When there's no attacker (collision...)
		only on server
	*/
	void addDamage(PlayerID playerID, int damage);
	/*
		only on server
	*/
	void resetHealth(PlayerID playerID);
	/*
		Get the list of players that died after the last call to this function (this will clear the deadlist)
	*/
	ScoreInfoList getDeadList();
	/*
		Updates a player. This is used on clients that receive scores from server.
	*/
	void updatePlayer(ScoresManager::PlayerScoreInfo info);
	
	const PlayerScoreInfo* getPlayer(PlayerID id);
	

	static ScoresManager& getSingleton(void){ assert( ms_Singleton );  return ( *ms_Singleton ); }
	static ScoresManager* getSingletonPtr(void){ return ms_Singleton; }

private:
	PlayerScoreInfo* findPlayer(PlayerID id);
	

	ScoreInfoList mScoreInfoList;
	ScoreInfoList mDeadList;
		
	bool mServer;
	int mFullHealth;

	PlayerScoreInfo *mLocalInfo;
	PlayerID mLocalID;
};
template<> ScoresManager* Ogre::Singleton<ScoresManager>::ms_Singleton = 0;
#endif 