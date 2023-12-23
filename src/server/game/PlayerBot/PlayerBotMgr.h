
#ifndef __PLAYERBOTMGR_H__
#define __PLAYERBOTMGR_H__

#include <chrono>

#include "Log.h"
#include "Common.h"
#include "SharedDefines.h"
#include "DatabaseEnv.h"
#include "Timer.h"
#include "PlayerBotSession.h"
#include "BotAITool.h"
#include "LFGMgr.h"
#include "ArenaTeamMgr.h"
#include "Containers.h"
#include <random>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifdef __linux__
#define sprintf_s(buffer, buffer_size, stringbuffer, ...) (sprintf(buffer, stringbuffer, __VA_ARGS__))
#endif
#include "DetourNavMesh.h"
#include "DetourNavMeshQuery.h"
#include <MMapDefines.h>
#include <PathGenerator.h>

#define CONVERT_ARENAAI_TOBG

class ObjectGuid;
struct LFGBotRequirement;
struct BotGlobleSchedule;
class PlayerBotSession;

enum PlayerBotAIType
{
	PBAIT_FIELD,
	PBAIT_BG,
	PBAIT_GROUP,
	PBAIT_DUNGEON,
	PBAIT_DUEL,
	PBAIT_ARENA,
	PBAIT_OVER
};

struct PlayerBotCharBaseInfo
{
	uint64 guid;
	uint32 account;
	std::string name;
	uint16 race;
	uint16 profession;
	uint16 gender;
	uint16 level;

	PlayerBotCharBaseInfo()
	{
		guid = 0;
		account = 0;
		race = profession = gender = level = 0;
	}
	PlayerBotCharBaseInfo(uint64 id, uint32 acc, const std::string &na, uint16 ra, uint16 pro, uint16 gen, uint16 lv) :
		guid(id), account(acc), name(na), race(ra), profession(pro), gender(gen), level(lv)
	{
	}

	std::string GetNameANDClassesText();

	TeamId GetCamp()
	{
		if (race == 1 || race == 3 || race == 4 || race == 7 || race == 11)
		{
			return TeamId::TEAM_ALLIANCE;
		}
		if (race == 2 || race == 5 || race == 6 || race == 8 || race == 10)
		{
			return TeamId::TEAM_HORDE;
		}
		return TeamId::TEAM_NEUTRAL;
	}
};

struct PlayerBotBaseInfo
{
	static PlayerBotCharBaseInfo empty;
	bool isAccountInfo;
	uint32 id;
	std::string username;
	std::string pass;
	using CharInfoMap = std::map<uint64, PlayerBotCharBaseInfo>;
	CharInfoMap characters;
	std::queue<WorldPacket> needCreateBots;

	PlayerBotBaseInfo(uint32 uid, const char *name, std::string &pa, bool isAcc) :
		isAccountInfo(isAcc), id(uid), pass(pa)
	{
		characters.clear();
		username = name;
	}

	bool MatchRaceByFuction(bool fuction, uint16 race)
	{
		if (fuction)
		{
			if (race == 1 || race == 3 || race == 4 || race == 7 || race == 11)
			{
				return true;
			}
		}
		else
		{
			if (race == 2 || race == 5 || race == 6 || race == 8 || race == 10)
			{
				return true;
			}
		}
		return false;
	}
	bool ExistClass(bool fuction, uint16 prof)
	{
#ifdef INCOMPLETE_BOT
		if (prof != 1 && prof != 5 && prof != 9)
			return false;
#endif
		for (CharInfoMap::iterator it = characters.begin();
			it != characters.end();
			it++)
		{
			if (it->second.profession == prof)
			{
				uint16 race = it->second.race;
				if (MatchRaceByFuction(fuction, race))
					return true;
			}
		}
		return false;
	}

	PlayerBotCharBaseInfo& GetRandomCharacterByFuction(bool faction)
	{
		if (characters.size() <= 0)
		{
			return empty;
		}
#ifdef INCOMPLETE_BOT
		for (int i = 0; i < 20; i++)
#else
		for (int i = 0; i < 5; i++)
#endif
		{
			int16 select = irand(0, characters.size() / 2 - 1);
			for (CharInfoMap::iterator it = characters.begin();
				it != characters.end();
				it++)
			{
#ifdef INCOMPLETE_BOT
				if (it->second.profession != 1 && it->second.profession != 5 && it->second.profession != 9)
					continue;
#endif
				if (MatchRaceByFuction(faction, it->second.race))
				{
					if (select <= 0)
						return it->second;
					else
						--select;
				}
			}
		}
		return characters.begin()->second;
	}

	std::vector<uint32> GetNoArenaTeamCharacterIDsByFuction(bool faction, ArenaTeamTypes type)
	{
		std::vector<uint32> outIDs;
		if (characters.size() <= 0)
		{
			return outIDs;
		}
		for (CharInfoMap::iterator it = characters.begin();
			it != characters.end();
			it++)
		{
#ifdef INCOMPLETE_BOT
			if (it->second.profession != 1 && it->second.profession != 5 && it->second.profession != 9)
				continue;
#endif
			if (MatchRaceByFuction(faction, it->second.race))
			{
				//if (sArenaTeamMgr->ExistArenaTeamByType(ObjectGuid(uint64(it->second.guid)), type))
				//	continue;
				outIDs.push_back(it->second.guid);
			}
		}
		Trinity::Containers::RandomShuffle(outIDs);
		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		std::shuffle(outIDs.begin(), outIDs.end(), std::default_random_engine(time(NULL)));
		return outIDs;
	}

	PlayerBotCharBaseInfo& GetCharacter(bool faction, uint32 prof)
	{
		if (characters.size() <= 0)
			return empty;
		for (CharInfoMap::iterator it = characters.begin();
			it != characters.end();
			it++)
		{
			if (!MatchRaceByFuction(faction, it->second.race))
				continue;
			if (it->second.profession == prof)
				return it->second;
		}
		return empty;
	}

	uint32 GetCharIDByNoArenaType(bool faction, uint32 prof, uint32 arenaType, std::vector<ObjectGuid>& fliters);

	bool ExistCharacterByGUID(ObjectGuid& guid)
	{
		for (CharInfoMap::iterator it = characters.begin();
			it != characters.end();
			it++)
		{
			if (it->second.guid == guid.GetCounter())
				return true;
		}
		return false;
	}

	TeamId GetTeamIDByChar(ObjectGuid& guid)
	{
		uint32 id = guid.GetCounter();
		for (CharInfoMap::iterator it = characters.begin();
			it != characters.end();
			it++)
		{
			if (it->second.guid != id)
				continue;
			return it->second.GetCamp();
		}
		return TeamId::TEAM_NEUTRAL;
	}

	std::string GetCharNameANDClassesText(ObjectGuid& guid)
	{
		uint32 id = guid.GetCounter();
		for (CharInfoMap::iterator it = characters.begin();
			it != characters.end();
			it++)
		{
			if (it->second.guid != id)
				continue;
			return it->second.GetNameANDClassesText();
		}
		return "";
	}

	bool RemoveCharacterByGUID(ObjectGuid& guid)
	{
		uint32 id = guid.GetCounter();
		for (CharInfoMap::iterator it = characters.begin();
			it != characters.end();
			it++)
		{
			if (it->second.guid != id)
				continue;
			characters.erase(it);
			return true;
		}
		return false;
	}
};

class TC_GAME_API PlayerBotMgr
{
	typedef std::vector<BattlegroundTypeId> BattleGroundTypes;

private:
	PlayerBotMgr();
	~PlayerBotMgr();

public:
	PlayerBotMgr(PlayerBotMgr const&) = delete;
	PlayerBotMgr(PlayerBotMgr&&) = delete;

	PlayerBotMgr& operator= (PlayerBotMgr const&) = delete;
	PlayerBotMgr& operator= (PlayerBotMgr&&) = delete;

	static PlayerBotMgr* instance();
	static void SwitchPlayerBotAI(Player* player, PlayerBotAIType aiType, bool force);

	std::string GetPlayerLinkText(Player const* player) const;

	std::string RandomArenaName();
	PlayerBotSession* GetBotSessionByCharGUID(ObjectGuid& guid);
	TeamId GetTeamIDByPlayerBotGUID(ObjectGuid& guid);
	bool IsPlayerBot(WorldSession *pSession);
	bool IsBotAccuntName(std::string name);
	bool IsIDLEPlayerBot(Player* player);
	void DestroyBotMail(uint32 guid);
	void LoadPlayerBotBaseInfo();
	void AddNewAccountBotBaseInfo(std::string name);
	std::set<uint32> GetArenaTeamPlayerBotIDCountByTeam(TeamId team, int32 count, ArenaTeamTypes type);
	PlayerBotBaseInfo* GetPlayerBotAccountInfo(uint32 guid);
	PlayerBotBaseInfo* GetAccountBotAccountInfo(uint32 guid);

	void UpdateLastAccountIndex(std::string& username);
	void UpAllPlayerBotSession();

	void OnPlayerBotCreate(ObjectGuid const& guid, uint32 accountId, std::string const& name, uint8 gender, uint8 race, uint8 playerClass, uint8 level);
	void OnAccountBotCreate(ObjectGuid const& guid, uint32 accountId, std::string const& name, uint8 gender, uint8 race, uint8 playerClass, uint8 level);
	void OnAccountBotDelete(ObjectGuid& guid, uint32 accountId);
	void OnPlayerBotLogin(WorldSession* pSession, Player* pPlayer);
	void OnPlayerBotLogout(WorldSession* pSession);
	void OnPlayerBotLeaveOriginalGroup(Player* pPlayer);
	void LoginGroupBotByPlayer(Player* pPlayer);
	void LoginFriendBotByPlayer(Player* pPlayer);
	void LogoutAllGroupPlayerBot(Group* pGroup, bool force);

	void AllPlayerBotRandomLogin(const char* name = "");
	void AllPlayerBotLogout();
	bool PlayerBotLogout(uint32 account);
	bool AllPlayerLeaveBG(uint32 account);
	void SupplementPlayerBot();

	void OnRealPlayerJoinBattlegroundQueue(uint32 bgTypeId, uint32 level);
	//void OnRealPlayerLeaveBattlegroundQueue(uint32 bgTypeId, uint32 level);
//	void OnRealPlayerLeaveArenaQueue(uint32 bgTypeId, uint32 level, uint32 aaType);
	void OnRealPlayerEnterBattleground(uint32 bgTypeId, uint32 level);
//	void OnRealPlayerLeaveBattleground(const Player* player);

	void Update();
	uint32 GetOnlineBotCount(TeamId team, bool hasReal);
	uint32 GetOnlineBotCount2(TeamId team, bool hasReal);

	bool LoginBotByAccountIndex(uint32 account, uint32 index);
	void DelayLoginPlayerBotByGUID(ObjectGuid& guid) { m_DelayOnlineBots.push(guid); }
	bool AddNewPlayerBotByGUID(ObjectGuid& guid);
	bool AddNewPlayerBotByGUID2(ObjectGuid& guid);
	void AddNewPlayerBot(bool faction, Classes prof, uint32 count);
	void AddNewAccountBot(bool faction, Classes prof);
	void AddNewPlayerBotByClass(uint32 count, Classes prof);
	bool ChangePlayerBotSetting(uint32 account, uint32 minLV, uint32 maxLV, uint32 talent);
	lfg::LfgRoles GetPlayerBotCurrentLFGRoles(Player* player);
	ObjectGuid GetNoArenaMatchCharacter(TeamId team, uint32 arenaType, Classes cls, std::vector<ObjectGuid>& fliters);
	std::string GetNameANDClassesText(ObjectGuid& guid);
	bool CanReadyArenaByArenaTeamID(uint32 arenaTeamId);
    void SetMax(int max) { m_MaxOnlineBot = max; }
    int32 m_MaxOnlineBot;
    int32 m_BotOnlineCount;

    /* reverse:  reverse string s in place */
    static void reverse(char s[])
    {
        int i, j;
        char c;

        for (i = 0, j = strlen(s) - 1; i < j; i++, j--) {
            c = s[i];
            s[i] = s[j];
            s[j] = c;
        }
    }
    /* itoa:  convert n to characters in s */
    static void itoa(int n, char s[])
    {
        int i, sign;

        if ((sign = n) < 0)  /* record sign */
            n = -n;          /* make n positive */
        i = 0;
        do {       /* generate digits in reverse order */
            s[i++] = n % 10 + '0';   /* get next digit */
        } while ((n /= 10) > 0);     /* delete it */
        if (sign < 0)
            s[i++] = '-';
        s[i] = '\0';
        reverse(s);
    }

private:
	bool ExistClassByRace(uint8 race, uint8 prof);
	void InitializeCreatePlayerBotName();
	std::string RandomName();
	uint8 RandomRace(bool group, uint8 prof);
	uint8 RandomSkinColor(uint8 race, uint8 gender, uint8 prof);
	uint8 RandomFace(uint8 race, uint8 gender, uint8 skinColor, uint8 prof);
	uint8 RandomHair(uint8 race, uint8 gender, uint8 prof);
	uint8 RandomHairColor(uint8 race, uint8 gender, uint8 hairID, uint8 prof);
	uint8 RandomFacialHair(uint8 race, uint8 gender, uint8 hairColor, uint8 prof);
	PlayerBotSession* UpPlayerBotSessionByBaseInfo(PlayerBotBaseInfo *pAcc, bool accountInfo);
	WorldPacket BuildCreatePlayerData(bool group, uint8 prof);
	void CreateOncePlayerBot();

	void ClearBaseInfo();
	void SupplementAccount();
	void LoadCharBaseInfo();
	void LoadSessionPermissionsCallback(PreparedQueryResult result);

	void ClearEmptyNeedPlayer();
	void ClearNeedPlayer(uint32 bgTypeID, uint32 bracketID);
	void AddNewPlayerBotToBG(TeamId team, uint32 minLV, uint32 maxLV, BattlegroundTypeId bgTypeID);
	//void AddNewPlayerBotToLFG(lfg::LFGBotRequirement* botRequirement);
	void AddNewPlayerBotToAA(TeamId team, BattlegroundTypeId bgTypeID, uint32 bracketID, uint32 aaType);
	void AddTeamBotToRatedArena(uint32 arenaTeamId);
	//bool FillOnlineBotScheduleByLFGRequirement(lfg::LFGBotRequirement* botRequirement, BotGlobleSchedule* botSchedule);
	uint32 GetScheduleTalentByLFGRequirement(lfg::LfgRoles roles, uint32 botCls);
	void QueryBattlegroundRequirement();
	void QueryRatedArenaRequirement();
	void QueryNonRatedArenaRequirement();
	void OnlinePlayerBotByGUIDQueue();
	bool ExistUnBGPlayerBot();
//	PVPDifficultyEntry const* FindBGBracketEntry(Battleground* bg_template, uint32 level);
//
private:
	uint32 m_BotAccountAmount;
	uint32 m_LastBotAccountIndex;
	uint32 m_LFGSearchTick;
	uint32 m_ArenaSearchTick;
	std::map<uint32, PlayerBotBaseInfo*> m_idPlayerBotBase;
	std::map<uint32, PlayerBotBaseInfo*> m_idAccountBotBase;

	BattleGroundTypes m_BGTypes;
	std::vector<std::string> allName;
	std::vector<std::string> allArenaName;
	std::queue<ObjectGuid> m_DelayOnlineBots;

public:
	static std::map<uint32, std::list<UnitAI*> > m_DelayDestroyAIs;
	static std::mutex g_uniqueLock;
};

struct PathParameter
{
    bool findOK;
    uint32 sessionID;
    ObjectGuid unitGUID;
    Map* pMap;
    Map* pBaseMap;
    uint32 mapID;
    uint32 instID;
    TypeID typeID;
    bool canFly;
    bool canSwim;
    bool canWalk;
    bool isInWater;
    bool isUnderWater;
    std::set<uint32> phaseMask;
    bool hasAuraType;

    float posX;
    float posY;
    float posZ;

    float targetPosX;
    float targetPosY;
    float targetPosZ;

    float offset;

    std::vector<G3D::Vector3> finishPaths;
 //   G3D::Vector3 destPosition;

    PathParameter(uint32 seID, Unit const* owner)
    {
        findOK = false;
        sessionID = seID;
        unitGUID = owner->GetGUID();
        pMap = owner->GetMap();
        mapID = owner->GetMapId();
        pBaseMap = (Map*)owner->GetMap();
        instID = owner->GetInstanceId();
        typeID = owner->GetTypeId();

        canFly = false;
        canSwim = false;
        canWalk = true;
        if (typeID == TypeID::TYPEID_UNIT)
        {
            canFly = owner->ToCreature()->CanFly();
            canSwim = owner->ToCreature()->CanSwim();
            canWalk = owner->ToCreature()->CanWalk();
        }
        isInWater = owner->IsInWater();
        isUnderWater = owner->IsUnderWater();
        hasAuraType = owner->HasAuraType(SPELL_AURA_WATER_WALK);

        posX = owner->GetPositionX();
        posY = owner->GetPositionY();
        posZ = owner->GetPositionZ();

        offset = 0;
    }

    void TrimOldPathpoint(Position& pos)
    {
        if (finishPaths.size() <= 1)
            return;
        finishPaths.erase(finishPaths.begin());
       /* if (finishPaths.size() == 1)
        {
            G3D::Vector3 oncePosition = *(finishPaths.begin());
            finishPaths.push_back(oncePosition);
        }*/
    }
    //void TrimOldPathpoint(Position& pos)
    //{
    //	if (finishPaths.size() <= 2)
    //		return;
    //	G3D::Vector3 refer;
    //	refer.x = pos.GetPositionX();
    //	refer.y = pos.GetPositionY();
    //	refer.z = pos.GetPositionZ();
    //	float gap = 9999999;
    //	uint32 popCount = 0;
    //	for (G3D::Vector3& point : finishPaths)
    //	{
    //		float distance = (refer - point).length();
    //		if (distance < gap)
    //		{
    //			gap = distance;
    //			++popCount;
    //		}
    //		else
    //			break;
    //	}
    //	while (popCount > 0 && finishPaths.size() > 2)
    //	{
    //		--popCount;
    //		finishPaths.erase(finishPaths.begin());
    //	}
    //}
};

#define FAR_MAX_PATH_LENGTH 32768
#define FAR_MAX_POINT_PATH_LENGTH   32768

class TC_GAME_API Pathfinding
{
public:
    Pathfinding(PathParameter* pathParam, dtNavMesh const* pNavMesh, dtNavMeshQuery const* pNavMeshQuery);
    ~Pathfinding();

    bool CalculatePath(float destX, float destY, float destZ, bool forceDest = false, bool straightLine = false);

    // option setters - use optional
    void SetUseStraightPath(bool useStraightPath) { _useStraightPath = useStraightPath; }
    void SetPathLengthLimit(float distance) { _pointPathLimit = std::min<uint32>(uint32(distance / SMOOTH_PATH_STEP_SIZE), FAR_MAX_POINT_PATH_LENGTH); }

    // result getters
    G3D::Vector3 const& GetStartPosition() const { return _startPosition; }
    G3D::Vector3 const& GetEndPosition() const { return _endPosition; }
    G3D::Vector3 const& GetActualEndPosition() const { return _actualEndPosition; }

    Movement::PointsArray const& GetPath() const { return _pathPoints; }

    PathType GetPathType() const { return _type; }
    void UpdateAllowedPositionZ(float x, float y, float& z) const;

private:

    dtPolyRef _pathPolyRefs[FAR_MAX_PATH_LENGTH];   // array of detour polygon references
    uint32 _polyLength;                         // number of polygons in the path

    Movement::PointsArray _pathPoints;  // our actual (x,y,z) path to the target
    PathType _type;                     // tells what kind of path this is

    bool _useStraightPath;  // type of path will be generated
    bool _forceDestination; // when set, we will always arrive at given point
    uint32 _pointPathLimit; // limit point path size; min(this, MAX_POINT_PATH_LENGTH)
    bool _straightLine;     // use raycast if true for a straight line path

    G3D::Vector3 _startPosition;        // {x, y, z} of current location
    G3D::Vector3 _endPosition;          // {x, y, z} of the destination
    G3D::Vector3 _actualEndPosition;    // {x, y, z} of the closest possible point to given destination

    PathParameter* _pathParameter;
    dtNavMesh const* _navMesh;              // the nav mesh
    dtNavMeshQuery const* _navMeshQuery;    // the nav mesh query used to find the path

    dtQueryFilter _filter;  // use single filter for all movements, update it when needed

    void SetStartPosition(G3D::Vector3 const& point) { _startPosition = point; }
    void SetEndPosition(G3D::Vector3 const& point) { _actualEndPosition = point; _endPosition = point; }
    void SetActualEndPosition(G3D::Vector3 const& point) { _actualEndPosition = point; }
    void NormalizePath();

    void Clear()
    {
        _polyLength = 0;
        _pathPoints.clear();
    }

    bool InRange(G3D::Vector3 const& p1, G3D::Vector3 const& p2, float r, float h) const;
    float Dist3DSqr(G3D::Vector3 const& p1, G3D::Vector3 const& p2) const;
    bool InRangeYZX(float const* v1, float const* v2, float r, float h) const;

    dtPolyRef GetPathPolyByPosition(dtPolyRef const* polyPath, uint32 polyPathSize, float const* Point, float* Distance = NULL) const;
    dtPolyRef GetPolyByLocation(float const* Point, float* Distance) const;
    bool HaveTile(G3D::Vector3 const& p) const;

    void BuildPolyPath(G3D::Vector3 const& startPos, G3D::Vector3 const& endPos);
    void BuildPointPath(float const* startPoint, float const* endPoint);
    void BuildShortcut();

    NavTerrainFlag GetNavTerrain(float x, float y, float z);
    void CreateFilter();
    void UpdateFilter();

    // smooth path aux functions
    uint32 FixupCorridor(dtPolyRef* path, uint32 npath, uint32 maxPath, dtPolyRef const* visited, uint32 nvisited);
    bool GetSteerTarget(float const* startPos, float const* endPos, float minTargetDist, dtPolyRef const* path, uint32 pathSize, float* steerPos,
        unsigned char& steerPosFlag, dtPolyRef& steerPosRef);
    dtStatus FindSmoothPath(float const* startPos, float const* endPos,
        dtPolyRef const* polyPath, uint32 polyPathSize,
        float* smoothPath, int* smoothPathSize, uint32 smoothPathMaxSize);
};
#define sPlayerBotMgr PlayerBotMgr::instance()

#endif // __PLAYERBOTMGR_H__
