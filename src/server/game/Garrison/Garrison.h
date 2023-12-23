/*
 * This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef Garrison_h__
#define Garrison_h__

#include "Define.h"
#include "DatabaseEnvFwd.h"
#include "GarrisonPackets.h"
#include "GarrisonAI.h"
#include "Optional.h"
#include "QuaternionData.h"
#include "GarrisonMission.h"
#include "GarrisonMgr.h"
#include <unordered_map>

class ClassHall;
class GameObject;
class GarrisonAI;
class Map;
class Player;
class WodGarrison;
class WarCampaign;
class CovenantCampaign;
class CovenantSanctum;
struct GarrSiteLevelEntry;

typedef std::list<WorldPackets::Garrison::GarrisonTalent> TalentSet;

enum MissionState
{
    MISSION_STATE_AVAILABLE = 0,
    MISSION_STATE_IN_PROGRESS = 1,
    MISSION_STATE_WAITING_BONUS = 2,
    MISSION_STATE_WAITING_OWERMAX_BONUS = 3,
    MISSION_STATE_COMPLETED = 5,
    MISSION_STATE_COMPLETED_OWERMAX = 6
};

enum GarrisonShipmentFlag

{
        GARRISON_SHIPMENT_FLAG_REQUIRE_QUEST_NOT_COMPLETE = 0x1,
        GARRISON_SHIPMENT_FLAG_REQUIRE_QUEST_COMPLETE = 0x4,
};
class TC_GAME_API Garrison
{
public:
    struct Follower
    {
        uint32 GetItemLevel() const;
        void EarnXP(Player* owner, uint32 xp);
        uint32 _EarnXP(uint32 xp);
        uint32 GetRequiredLevelUpXP() const;
        bool HasAbility(uint32 garrAbilityId) const;
        void ModAssistant(SpellInfo const* spellInfo, Player* caster);

        WorldPackets::Garrison::GarrisonFollower PacketInfo;
    };

    struct Mission
    {
        WorldPackets::Garrison::GarrisonMission PacketInfo;
        std::vector<WorldPackets::Garrison::GarrisonMissionReward> Rewards;
        std::vector<WorldPackets::Garrison::GarrisonMissionReward> OvermaxRewards;
        bool CanStartMission = true;
    };

    explicit Garrison(Player* owner);
    virtual ~Garrison() {};
    Player* GetOwner() const { return _owner; }

    virtual bool LoadFromDB();
    virtual void SaveToDB(CharacterDatabaseTransaction& trans);
    void DeleteFromDB(CharacterDatabaseTransaction& trans);
    static void DeleteFromDB(CharacterDatabaseTransaction& trans, ObjectGuid::LowType guid, GarrisonType garrType);

    virtual bool Create(uint32 garrSiteId);
    void Update(uint32 const diff);
    virtual void Delete();

    virtual void Enter();
    virtual void Leave();

    uint32 GetScriptId() const;
    void AI_Initialize();
    void AI_Destroy();
    GarrisonAI* AI() { return _ai.get(); }

    virtual bool IsAllowedArea(AreaTableEntry const* /*area*/) const { return false; }

    GarrisonFactionIndex GetFaction() const;
    GarrisonType GetType() const { return _garrisonType; }
    void SetSiteLevel(GarrSiteLevelEntry const* siteLevel);
    GarrSiteLevelEntry const* GetSiteLevel() const { return _siteLevel; }

    // Followers
    void AddFollower(uint32 garrFollowerId);
    Follower* GetFollower(uint64 dbId);
    std::unordered_map<uint64 /*dbId*/, Garrison::Follower> const& GetFollowers() const { return _followers; }
    uint32 GetActiveFollowersCount() const;
    uint32 GetAverageFollowerILevel() const;
    uint32 GetMaxFollowerLevel() const;
    uint32 GetFollowerActivationLimit() const { return _followerActivationsRemainingToday; }
    uint32 GetCountOFollowers() const;

    template<typename Predicate>
    uint32 CountFollowers(Predicate&& predicate) const
    {
        uint32 count = 0;
        for (auto itr = _followers.begin(); itr != _followers.end(); ++itr)
            if (predicate(itr->second))
                ++count;

        return count;
    }

    void ChangeFollowerVitality(uint32 followerID);
    void ResetFollowerActivationLimit() { _followerActivationsRemainingToday = 1; }
    //DekkCore
     // Missions
    void AddMission(uint32 garrMissionId);
    Mission* GetMission(uint64 dbId);
    Mission* GetMissionByID(uint32 ID);
    void DeleteMission(uint64 dbId);
    std::unordered_map<uint64 /*dbId*/, Garrison::Mission> const& GetMissions() const { return _missions; }
    std::vector<Follower*> GetMissionFollowers(uint32 garrMissionId);
    bool HasMission(uint32 garrMissionId) const;
    void StartMission(uint32 garrMissionId, std::vector<uint64 /*DbID*/> Followers);
    void SendStartMissionResult(bool success, Garrison::Mission* mission = nullptr, std::vector<uint64 /*DbID*/>* Followers = nullptr);
    void CompleteMission(uint32 garrMissionId);
    void CalculateMissonBonusRoll(uint32 garrMissionId);
    void RewardMission(Mission* mission, bool withOvermaxReward);
    void IncrementStartedTodayMissions();
    uint32 GetNumMissionsStartedToday();
    bool hasTallent(uint32 talentID) const;
    void StartClassHallUpgrade(uint32 tallentID);
    bool canStartUpgrade();
    void AddTalentToStore(uint32 talentID, uint32 _time, uint32 flags, uint32 db_state);

    //Shipments
    void CreateShipment(ObjectGuid const& guid, uint32 count);
    void SendShipmentInfo(ObjectGuid const& guid);
    void SendShipYadShipmentInfo(ObjectGuid const& guid);

    void OnGossipTradeSkill(WorldObject* source);

    void OnGossipSelect(WorldObject* source); //test

    std::pair<std::vector<GarrMissionEntry const*>, std::vector<double>> GetAvailableMissions() const;
    void GenerateMissions();

    bool IsWodGarrison() const { return GetType() == GARRISON_TYPE_GARRISON; }
    WodGarrison* ToWodGarrison() { if (IsWodGarrison()) return reinterpret_cast<WodGarrison*>(this); else return nullptr; }
    WodGarrison const* ToWodGarrison() const { if (IsWodGarrison()) return reinterpret_cast<WodGarrison const*>(this); else return nullptr; }

    bool IsClassHall() const { return GetType() == GARRISON_TYPE_CLASS_HALL; }
    ClassHall* ToClassHall() { if (IsClassHall()) return reinterpret_cast<ClassHall*>(this); else return nullptr; }
    ClassHall const* ToClassHall() const { if (IsClassHall()) return reinterpret_cast<ClassHall const*>(this); else return nullptr; }

    bool IsWarCampaign() const { return GetType() == GARRISON_TYPE_WAR_CAMPAIGN; }
    WarCampaign* ToWarCampaign() { if (IsWarCampaign()) return reinterpret_cast<WarCampaign*>(this); else return nullptr; }
    WarCampaign const* ToWarCampaign() const { if (IsWarCampaign()) return reinterpret_cast<WarCampaign const*>(this); else return nullptr; }

    bool IsCovenantCampaign() const { return GetType() == GARRISON_TYPE_COVENANT; }
    CovenantCampaign* ToCovenantCampaign() { if (IsCovenantCampaign()) return reinterpret_cast<CovenantCampaign*>(this); else return nullptr; }
    CovenantCampaign const* ToCovenantCampaign() const { if (IsCovenantCampaign()) return reinterpret_cast<CovenantCampaign const*>(this); else return nullptr; }

    void SendMissionListUpdate(bool openMissionNpc) const;
    GarrSiteLevelEntry const* _siteLevel;
    Player* _owner;
    Map* FindMap() const;

    //DekkCore
    protected:
        GarrisonType _garrisonType;
        uint32 _numMissionsStartedToday;
        TalentSet _classHallTalentStore;

private:


    std::unique_ptr<GarrisonAI> _ai;
    IntervalTimer _timers[GUPDATE_COUNT];

    std::unordered_map<uint64 /*dbId*/, Garrison::Follower> _followers;
    std::unordered_map<uint64 /*dbId*/, Follower> _followerss[GARRISON_TYPE_MAX];
    std::unordered_set<uint32> _followerIds;
    uint32 _followerActivationsRemainingToday;

    std::set<uint32/*abilityID*/> _abilities;
    std::unordered_map<uint64 /*dbId*/, Garrison::Mission> _missions;
    std::unordered_map<uint64 /*dbId*/, Mission> _missionss[GARRISON_TYPE_MAX];
    std::unordered_set<uint32> _missionIds;
};

#endif // Garrison_h__
