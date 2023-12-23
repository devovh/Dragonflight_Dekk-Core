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

#include "Containers.h"
#include "Creature.h"
#include "DatabaseEnv.h"
#include "DB2Stores.h"
#include "Garrison.h"
#include "GarrisonAI.h"
#include "GarrisonPackets.h"
#include "GameObject.h"
#include "GameTime.h"
#include "GarrisonMgr.h"
#include "Log.h"
#include "Map.h"
#include "MapManager.h"
#include "ObjectMgr.h"
#include "PhasingHandler.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "VehicleDefines.h"
#include "WodGarrison.h"

Garrison::Garrison(Player* owner) : _siteLevel(nullptr), _owner(owner), _garrisonType(), _numMissionsStartedToday(8), _followerActivationsRemainingToday(1)
{
    _timers[GUPDATE_MISSIONS_DISTRIBUTION].SetInterval(MINUTE * IN_MILLISECONDS);
}

bool Garrison::Create(uint32 garrSiteId)
{
    GarrSiteLevelEntry const* siteLevel = sGarrisonMgr.GetGarrSiteLevelEntry(garrSiteId, 1);
    if (!siteLevel)
        return false;

    SetSiteLevel(siteLevel);

    WorldPackets::Garrison::GarrisonCreateResult garrisonCreateResult;
    garrisonCreateResult.GarrSiteLevelID = _siteLevel->ID;
    _owner->SendDirectMessage(garrisonCreateResult.Write());
    PhasingHandler::OnConditionChange(_owner);

    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
    SaveToDB(trans);
    CharacterDatabase.CommitTransaction(trans);

    return true;
}

void Garrison::Update(uint32 const diff)
{
    // Update the different timers
    for (uint8 i = 0; i < GUPDATE_COUNT; ++i)
    {
        if (_timers[i].GetCurrent() >= 0)
            _timers[i].Update(diff);
        else
            _timers[i].SetCurrent(0);
    }
    if (_timers[GUPDATE_MISSIONS_DISTRIBUTION].Passed())
    {
        _timers[GUPDATE_MISSIONS_DISTRIBUTION].Reset();
        GenerateMissions();
    }
}

void Garrison::Delete()
{
    WorldPackets::Garrison::GarrisonDeleteResult garrisonDelete;
    garrisonDelete.Result = GARRISON_SUCCESS;
    garrisonDelete.GarrSiteID = _siteLevel->GarrSiteID;
    _owner->SendDirectMessage(garrisonDelete.Write());
}

bool Garrison::LoadFromDB()
{
    ObjectGuid::LowType lowGuid = _owner->GetGUID().GetCounter();

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHARACTER_GARRISON);
    stmt->setUInt64(0, lowGuid);
    stmt->setUInt8(1, _garrisonType);
    PreparedQueryResult garrisonStmt = CharacterDatabase.Query(stmt);

    stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHARACTER_GARRISON_FOLLOWERS);
    stmt->setUInt64(0, lowGuid);
    stmt->setUInt8(1, _garrisonType);
    PreparedQueryResult followersStmt = CharacterDatabase.Query(stmt);

    stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHARACTER_GARRISON_FOLLOWER_ABILITIES);
    stmt->setUInt64(0, lowGuid);
    stmt->setUInt8(1, _garrisonType);
    PreparedQueryResult abilitiesStmt = CharacterDatabase.Query(stmt);

    stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHARACTER_GARRISON_MISSIONS);
    stmt->setUInt64(0, lowGuid);
    stmt->setUInt8(1, _garrisonType);
    PreparedQueryResult missionsStmt = CharacterDatabase.Query(stmt);

    stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHARACTER_GARRISON_MISSION_REWARDS);
    stmt->setUInt64(0, lowGuid);
    stmt->setUInt8(1, _garrisonType);
    PreparedQueryResult rewardsStmt = CharacterDatabase.Query(stmt);

    if (!garrisonStmt)
        return false;

    Field* fields = garrisonStmt->Fetch();
    GarrSiteLevelEntry const* siteLevel = sGarrSiteLevelStore.LookupEntry(fields[0].GetUInt32());
    _followerActivationsRemainingToday = fields[1].GetUInt32();
    if (!siteLevel)
        return false;

    SetSiteLevel(siteLevel);

    //           0           1        2      3                4               5   6                7               8       9
    // SELECT dbId, followerId, quality, level, itemLevelWeapon, itemLevelArmor, xp, currentBuilding, currentMission, status FROM character_garrison_followers WHERE guid = ? AND garrison_type = ?
    if (followersStmt)
    {
        do
        {
            fields = followersStmt->Fetch();

            uint64 dbId = fields[0].GetUInt64();
            uint32 followerId = fields[1].GetUInt32();

            if (!sGarrFollowerStore.LookupEntry(followerId))
                continue;

            _followerIds.insert(followerId);
            Follower& follower = _followers[dbId];
            follower.PacketInfo.DbID = dbId;
            follower.PacketInfo.GarrFollowerID = followerId;
            follower.PacketInfo.Quality = fields[2].GetUInt32();
            follower.PacketInfo.FollowerLevel = fields[3].GetUInt32();
            follower.PacketInfo.ItemLevelWeapon = fields[4].GetUInt32();
            follower.PacketInfo.ItemLevelArmor = fields[5].GetUInt32();
            follower.PacketInfo.Xp = fields[6].GetUInt32();
            follower.PacketInfo.CurrentMissionID = fields[8].GetUInt32();
            follower.PacketInfo.FollowerStatus = fields[9].GetUInt32();
            if (!sGarrBuildingStore.LookupEntry(follower.PacketInfo.CurrentBuildingID))
                follower.PacketInfo.CurrentBuildingID = 0;

            if (!sGarrMissionStore.LookupEntry(follower.PacketInfo.CurrentMissionID))
                follower.PacketInfo.CurrentMissionID = 0;

        } while (followersStmt->NextRow());

        if (abilitiesStmt)
        {
            do
            {
                fields = abilitiesStmt->Fetch();
                uint64 dbId = fields[0].GetUInt64();
                GarrAbilityEntry const* ability = sGarrAbilityStore.LookupEntry(fields[1].GetUInt32());
                if (!ability)
                    continue;
                auto itr = _followers.find(dbId);
                if (itr == _followers.end())
                    continue;

                itr->second.PacketInfo.AbilityID.push_back(ability);
            } while (abilitiesStmt->NextRow());
        }
    }

    // SELECT dbId, missionId, offerTime, startTime, status
    if (missionsStmt)
    {
        do
        {
            fields = missionsStmt->Fetch();

            uint64 dbId = fields[0].GetUInt64();
            uint32 missionId = fields[1].GetUInt32();

            GarrMissionEntry const* missionEntry = sGarrMissionStore.LookupEntry(missionId);
            if (!missionEntry)
                continue;

            _missionIds.insert(missionId);
            Mission& mission = _missions[dbId];
            mission.PacketInfo.DbID = dbId;
            mission.PacketInfo.MissionRecID = missionId;
            mission.PacketInfo.OfferTime = time_t(fields[2].GetUInt32());
            mission.PacketInfo.OfferDuration = Seconds(missionEntry->OfferDuration);
            mission.PacketInfo.StartTime = time_t(fields[3].GetUInt32());
            mission.PacketInfo.TravelDuration = Seconds(missionEntry->TravelDuration);
            mission.PacketInfo.MissionDuration = Seconds(missionEntry->MissionDuration);
            mission.PacketInfo.MissionState = fields[4].GetUInt8();

            if (mission.PacketInfo.StartTime == 0)
                mission.PacketInfo.StartTime = time_t(2254525440);

            mission.PacketInfo.SuccessChance = sGarrisonMgr.GetMissionSuccessChance(this, missionId);

        } while (missionsStmt->NextRow());

        if (rewardsStmt)
        {
            do
            {
                fields = rewardsStmt->Fetch();
                uint64 dbId = fields[0].GetUInt64();
                uint8 type = fields[1].GetUInt8();

                auto itr = _missions.find(dbId);
                if (itr == _missions.end())
                    continue;

                WorldPackets::Garrison::GarrisonMissionReward reward;
                reward.ItemID = fields[2].GetInt32();
                reward.ItemQuantity = fields[3].GetUInt32();
                reward.CurrencyID = fields[4].GetInt32();
                reward.CurrencyQuantity = fields[5].GetUInt32();
                reward.FollowerXP = fields[6].GetUInt32();
                reward.GarrMssnBonusAbilityID = fields[7].GetUInt32();

                if (type == GarrisonMission::RewardType::Normal)
                    itr->second.Rewards.push_back(reward);
                else
                    itr->second.OvermaxRewards.push_back(reward);

            } while (rewardsStmt->NextRow());
        }
    }

    return true;
}

void Garrison::SaveToDB(CharacterDatabaseTransaction& trans)
{
    DeleteFromDB(trans);

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_CHARACTER_GARRISON);
    stmt->setUInt64(0, _owner->GetGUID().GetCounter());
    stmt->setUInt8(1, _garrisonType);
    stmt->setUInt32(2, _siteLevel->ID);
    stmt->setUInt32(3, _followerActivationsRemainingToday);
    trans->Append(stmt);

    for (auto const& p : _followers)
    {
        Follower const& follower = p.second;
        uint8 index = 0;
        stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_CHARACTER_GARRISON_FOLLOWERS);
        stmt->setUInt64(index++, follower.PacketInfo.DbID);
        stmt->setUInt64(index++, _owner->GetGUID().GetCounter());
        stmt->setUInt8(index++, _garrisonType);
        stmt->setUInt32(index++, follower.PacketInfo.GarrFollowerID);
        stmt->setUInt32(index++, follower.PacketInfo.Quality);
        stmt->setUInt32(index++, follower.PacketInfo.FollowerLevel);
        stmt->setUInt32(index++, follower.PacketInfo.ItemLevelWeapon);
        stmt->setUInt32(index++, follower.PacketInfo.ItemLevelArmor);
        stmt->setUInt32(index++, follower.PacketInfo.Xp);
        stmt->setUInt32(index++, follower.PacketInfo.CurrentBuildingID);
        stmt->setUInt32(index++, follower.PacketInfo.CurrentMissionID);
        stmt->setUInt32(index++, follower.PacketInfo.FollowerStatus);
        trans->Append(stmt);

        uint8 slot = 0;
        for (GarrAbilityEntry const* ability : follower.PacketInfo.AbilityID)
        {
            stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_CHARACTER_GARRISON_FOLLOWER_ABILITIES);
            stmt->setUInt64(0, follower.PacketInfo.DbID);
            stmt->setUInt32(1, ability->ID);
            stmt->setUInt8(2, slot++);
            trans->Append(stmt);
        }
    }

for (auto const& p : _missions)
    {
    Mission const& mission = p.second;
    uint8 index = 0;
    stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_CHARACTER_GARRISON_MISSIONS);
    stmt->setUInt64(index++, mission.PacketInfo.DbID);
    stmt->setUInt64(index++, _owner->GetGUID().GetCounter());
    stmt->setUInt8(index++, _garrisonType);
    stmt->setUInt32(index++, mission.PacketInfo.MissionRecID);
    stmt->setUInt32(index++, mission.PacketInfo.OfferTime);
    stmt->setUInt32(index++, mission.PacketInfo.StartTime != time_t(2254525440) ? int(mission.PacketInfo.StartTime) : 0);
    stmt->setUInt32(index++, mission.PacketInfo.MissionState);
    trans->Append(stmt);

    uint8 rewardType = GarrisonMission::RewardType::Normal;
    for (auto rewards : { mission.Rewards, mission.OvermaxRewards })
    {
        for (WorldPackets::Garrison::GarrisonMissionReward reward : rewards)
        {
            stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_CHARACTER_GARRISON_MISSION_REWARDS);
            stmt->setUInt64(0, mission.PacketInfo.DbID);
            stmt->setUInt8(1, rewardType++);
            stmt->setInt32(2, reward.ItemID);
            stmt->setUInt32(3, reward.ItemQuantity);
            stmt->setInt32(4, reward.CurrencyID);
            stmt->setUInt32(5, reward.CurrencyQuantity);
            stmt->setUInt32(6, reward.FollowerXP);
            stmt->setUInt32(7, reward.GarrMssnBonusAbilityID);
            trans->Append(stmt);
        }
      }
    }
}

void Garrison::DeleteFromDB(CharacterDatabaseTransaction& trans)
{
    Garrison::DeleteFromDB(trans, _owner->GetGUID().GetCounter(), GetType());
}

void Garrison::DeleteFromDB(CharacterDatabaseTransaction& trans, ObjectGuid::LowType guid, GarrisonType garrType)
{
    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHARACTER_GARRISON);
    stmt->setUInt64(0, guid);
    stmt->setUInt8(1, garrType);
    trans->Append(stmt);

    stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHARACTER_GARRISON_MISSIONS);
    stmt->setUInt64(0, guid);
    stmt->setUInt8(1, garrType);
    trans->Append(stmt);

    stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHARACTER_GARRISON_FOLLOWERS);
    stmt->setUInt64(0, guid);
    stmt->setUInt8(1, garrType);
    trans->Append(stmt);

    stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHARACTER_GARRISON_BLUEPRINTS);
    stmt->setUInt64(0, guid);
    stmt->setUInt8(1, garrType);
    trans->Append(stmt);

    stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHARACTER_GARRISON_BUILDINGS);
    stmt->setUInt64(0, guid);
    stmt->setUInt8(1, garrType);
    trans->Append(stmt);
}

void Garrison::Enter()
{
    _owner->SetCurrentGarrison(_garrisonType);
    AI()->OnPlayerEnter(_owner);
}

void Garrison::Leave()
{
    _owner->SetCurrentGarrison(GARRISON_TYPE_NONE);
    AI()->OnPlayerLeave(_owner);
}

uint32 Garrison::GetScriptId() const
{
    return sObjectMgr->GetScriptIdForGarrison(_siteLevel->ID);
}

GarrisonFactionIndex Garrison::GetFaction() const
{
    return _owner->GetTeam() == HORDE ? GARRISON_FACTION_INDEX_HORDE : GARRISON_FACTION_INDEX_ALLIANCE;
}

void Garrison::SetSiteLevel(GarrSiteLevelEntry const* siteLevel)
{
    _siteLevel = siteLevel;
    AI_Initialize();
}

void Garrison::AI_Initialize()
{
    AI_Destroy();
    GarrisonAI* ai = sScriptMgr->GetGarrisonAI(this);
    if (!ai)
        ai = new NullGarrisonAI(this);

    _ai.reset(ai);
}

void Garrison::AI_Destroy()
{
    _ai.reset();
}

void Garrison::AddFollower(uint32 garrFollowerId)
{
    WorldPackets::Garrison::GarrisonAddFollowerResult addFollowerResult;
    addFollowerResult.GarrTypeID = (int32)_garrisonType;
    GarrFollowerEntry const* followerEntry = sGarrFollowerStore.LookupEntry(garrFollowerId);
    if (_followerIds.count(garrFollowerId) || !followerEntry)
    {
        addFollowerResult.Result = GARRISON_ERROR_FOLLOWER_EXISTS;
        _owner->SendDirectMessage(addFollowerResult.Write());
        return;
    }

    _followerIds.insert(garrFollowerId);
    uint64 dbId = sGarrisonMgr.GenerateFollowerDbId();
    Follower& follower = _followers[dbId];
    follower.PacketInfo.DbID = dbId;
    follower.PacketInfo.GarrFollowerID = garrFollowerId;
    follower.PacketInfo.Quality = followerEntry->Quality;   // TODO: handle magic upgrades
    follower.PacketInfo.FollowerLevel = followerEntry->FollowerLevel;
    follower.PacketInfo.ItemLevelWeapon = followerEntry->ItemLevelWeapon;
    follower.PacketInfo.ItemLevelArmor = followerEntry->ItemLevelArmor;
    follower.PacketInfo.Xp = 0;
    follower.PacketInfo.CurrentBuildingID = 0;
    follower.PacketInfo.CurrentMissionID = 0;
    follower.PacketInfo.AbilityID = sGarrisonMgr.RollFollowerAbilities(garrFollowerId, followerEntry, follower.PacketInfo.Quality, GetFaction(), true);
    follower.PacketInfo.FollowerStatus = 0;

    addFollowerResult.Follower = follower.PacketInfo;
    _owner->SendDirectMessage(addFollowerResult.Write());

    _owner->UpdateCriteria(CriteriaType::RecruitGarrisonFollower, follower.PacketInfo.DbID);
    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
    SaveToDB(trans);
    CharacterDatabase.CommitTransaction(trans);
}

Garrison::Follower* Garrison::GetFollower(uint64 dbId)
{
    auto itr = _followers.find(dbId);
    if (itr != _followers.end())
        return &itr->second;

    return nullptr;
}

uint32 Garrison::GetActiveFollowersCount() const
{
    return std::count_if(_followers.begin(), _followers.end(), [](auto followerItr)
        {
            return followerItr.second.PacketInfo.FollowerStatus != FOLLOWER_STATUS_INACTIVE;
        });
}

uint32 Garrison::GetAverageFollowerILevel() const
{
    uint32 followerIlevels = 0;
    uint32 activeFollowerCount = 0;

    for (auto itr : _followers)
    {
        if (itr.second.PacketInfo.FollowerStatus != FOLLOWER_STATUS_INACTIVE)
        {
            followerIlevels += itr.second.GetItemLevel();
            ++activeFollowerCount;
        }
    }

    return ceil(float(followerIlevels) / float(activeFollowerCount));
}

uint32 Garrison::GetMaxFollowerLevel() const
{
    uint32 maxFollowerLevel = 60;

    for (auto itr : _followers)
        if (itr.second.PacketInfo.FollowerStatus != FOLLOWER_STATUS_INACTIVE)
            maxFollowerLevel = std::max(maxFollowerLevel, itr.second.PacketInfo.FollowerLevel);

    return maxFollowerLevel;
}

void Garrison::AddMission(uint32 garrMissionId)
{
    GarrMissionEntry const* missionEntry = sGarrMissionStore.LookupEntry(garrMissionId);
    if (_missionIds.count(garrMissionId) || !missionEntry)
        return;

    _missionIds.insert(garrMissionId);
    uint64 dbId = sGarrisonMgr.GenerateMissionDbId();
    Mission& mission = _missions[dbId];
    mission.PacketInfo.DbID = dbId;
    mission.PacketInfo.MissionRecID = garrMissionId;
    mission.PacketInfo.OfferTime = time(nullptr);
    mission.PacketInfo.OfferDuration = Seconds(missionEntry->OfferDuration);
    mission.PacketInfo.StartTime = time_t(2254525440);
    mission.PacketInfo.TravelDuration = Seconds(missionEntry->TravelDuration);
    mission.PacketInfo.MissionDuration = Seconds(missionEntry->MissionDuration);
    mission.PacketInfo.MissionState = 0;
    mission.PacketInfo.SuccessChance = 0;
    mission.PacketInfo.Flags = 1; //maybe flags

    // TODO : Generate rewards for mission
    WorldPackets::Garrison::GarrisonMissionReward reward;
    reward.ItemID = 140587;
    reward.ItemQuantity = 1;
    reward.CurrencyID = 0;
    reward.CurrencyQuantity = 0;
    reward.FollowerXP = 100;
    reward.GarrMssnBonusAbilityID = 0;
    reward.ItemFileDataID = 1118739; //?? maybe
    mission.Rewards.push_back(reward);

    WorldPackets::Garrison::GarrisonAddMissionResult garrisonAddMissionResult;
    garrisonAddMissionResult.GarrType = GetType();
    garrisonAddMissionResult.Result = GarrisonMission::Result::Success;
    garrisonAddMissionResult.State = GarrisonMission::State::Available;
    garrisonAddMissionResult.Mission = mission.PacketInfo;
    garrisonAddMissionResult.Rewards = mission.Rewards;
    garrisonAddMissionResult.BonusRewards = mission.OvermaxRewards;
    garrisonAddMissionResult.Success = true;
    _owner->SendDirectMessage(garrisonAddMissionResult.Write());
}

Garrison::Mission* Garrison::GetMission(uint64 dbId)
{
    auto itr = _missions.find(dbId);
    if (itr != _missions.end())
        return &itr->second;

    return nullptr;
}

Garrison::Mission* Garrison::GetMissionByID(uint32 ID)
{
    return Trinity::Containers::MapGetValuePtr(_missions, ID);
}

void Garrison::DeleteMission(uint64 dbId)
{
    _missions.erase(dbId);
}

std::vector<Garrison::Follower*> Garrison::GetMissionFollowers(uint32 garrMissionId)
{
    std::vector<Garrison::Follower*> missionFollowers;
    for (auto followerItr : _followers)
        if (followerItr.second.PacketInfo.CurrentMissionID == garrMissionId)
            missionFollowers.push_back(&followerItr.second);

    return missionFollowers;
}

bool Garrison::HasMission(uint32 garrMissionId) const
{
    for (auto const& itr : GetMissions())
        if (uint32(itr.second.PacketInfo.MissionRecID) == garrMissionId)
            return true;

    return false;
}

std::pair<std::vector<GarrMissionEntry const*>, std::vector<double>> Garrison::GetAvailableMissions() const
{
    std::vector<GarrMissionEntry const*> availableMissions;
    std::vector<double> weights;

    uint32 const maxFollowerlevel = GetMaxFollowerLevel();
    uint32 const activeFolowerCount = GetActiveFollowersCount();
    uint32 const averageFollowerILevel = GetAverageFollowerILevel();

    for (auto const& missionEntry : sGarrMissionStore)
    {
        if (HasMission(missionEntry->ID))
            continue;

        // Most missions with Duration <= 10 are tests
        if (missionEntry->MissionDuration <= 10)
            continue;

        if (missionEntry->GarrTypeID != GetType())
            continue;

        if (uint32(missionEntry->TargetLevel) > maxFollowerlevel)
            continue;

         if (missionEntry->MaxFollowers > activeFolowerCount)
            continue;

         if (missionEntry->TargetItemLevel > averageFollowerILevel)
             continue;

         uint32 requiredClass = sGarrisonMgr.GetClassByMissionType(missionEntry->GarrMissionTypeID);
         if (requiredClass != CLASS_NONE && requiredClass != _owner->GetClass())
             continue;

         uint32 requiredTeam = sGarrisonMgr.GetFactionByMissionType(missionEntry->GarrMissionTypeID);
         if (requiredTeam != TEAM_OTHER && requiredTeam != _owner->GetTeamId())
             continue;

         double weight = 100.0;
         if (missionEntry->Flags & GarrisonMission::Flags::Rare)
             weight = 25.0;

         availableMissions.push_back(missionEntry);
         weights.push_back(weight);
    }

    return std::make_pair(availableMissions, weights);
}

void Garrison::GenerateMissions()
{
    uint32 maxMissionCount = ceil(GetActiveFollowersCount() * GARRISON_MISSION_DISTRIB_FOLLOWER_COEFF);

    if (GetMissions().size() >= maxMissionCount)
        return;

    uint32 missionToAddCount = urand(0, maxMissionCount - GetMissions().size());

    if (!missionToAddCount)
        return;

    std::pair<std::vector<GarrMissionEntry const*>, std::vector<double>> availableMissionWithWeights = GetAvailableMissions();

    for (uint32 i = 0; i < missionToAddCount; ++i)
    {
        auto missionItr = Trinity::Containers::SelectRandomWeightedContainerElement(availableMissionWithWeights.first, std::span((availableMissionWithWeights.second)));
        if (missionItr == availableMissionWithWeights.first.end())
            return;

        AddMission((*missionItr)->ID);

        // We will remove mission from available missions, also remove corresponding weight from vector
        auto distance = std::distance(availableMissionWithWeights.first.cbegin(), missionItr);
        auto weightItr = availableMissionWithWeights.second.begin();
        std::advance(weightItr, distance);

        availableMissionWithWeights.first.erase(missionItr);
        availableMissionWithWeights.second.erase(weightItr);
    }

    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
    SaveToDB(trans);
    CharacterDatabase.CommitTransaction(trans);
}

void Garrison::StartMission(uint32 garrMissionId, std::vector<uint64 /*DbID*/> Followers)
{
    GarrMissionEntry const* missionEntry = sGarrMissionStore.LookupEntry(garrMissionId);
    if (!missionEntry)
        return SendStartMissionResult(false);

    Garrison::Mission* mission = GetMissionByID(missionEntry->ID);
    if (!mission)
        return SendStartMissionResult(false);

    mission->PacketInfo.StartTime = time(nullptr);
    IncrementStartedTodayMissions();
    mission->PacketInfo.MissionState = GarrisonMission::State::InProgress;
    mission->PacketInfo.SuccessChance = sGarrisonMgr.GetMissionSuccessChance(this, mission->PacketInfo.MissionRecID);

    for (uint64 followerDbID : Followers)
    {
        Garrison::Follower* follower = GetFollower(followerDbID);

        if (!follower)
            return SendStartMissionResult(false);

        if (follower->PacketInfo.CurrentMissionID != 0 || follower->PacketInfo.CurrentBuildingID != 0)
            return SendStartMissionResult(false);

        follower->PacketInfo.CurrentMissionID = missionEntry->ID;
    }

    SendStartMissionResult(true, mission, &Followers);
}

void Garrison::SendStartMissionResult(bool success, Garrison::Mission* mission /*= nullptr*/, std::vector<uint64 /*DbID*/>* Followers /*= nullptr*/)
{
    WorldPackets::Garrison::GarrisonStartMissionResult garrisonStartMissionResult;

    if (success)
    {
        garrisonStartMissionResult.Result = GarrisonMission::Result::Success;
        garrisonStartMissionResult.Mission = mission->PacketInfo;
        garrisonStartMissionResult.Followers = *Followers;
    }

    else
    {
        garrisonStartMissionResult.Result = GarrisonMission::Result::Fail;
    }

    _owner->SendDirectMessage(garrisonStartMissionResult.Write());
}

void Garrison::CompleteMission(uint32 garrMissionId)
{
    GarrMissionEntry const* missionEntry = sGarrMissionStore.LookupEntry(garrMissionId);
    if (!missionEntry)
        return;

    Garrison::Mission* mission = GetMissionByID(missionEntry->ID);
    if (!mission)
        return;

    bool canComplete = Seconds(mission->PacketInfo.StartTime) + Seconds(mission->PacketInfo.MissionDuration) < Seconds(time(nullptr));
    bool success = false;

    if (canComplete)
    {
        std::vector<Garrison::Follower*> followers = GetMissionFollowers(missionEntry->ID);
        if (followers.empty())
            return;

        success = roll_chance_i(mission->PacketInfo.SuccessChance);
        mission->PacketInfo.MissionState = success ? GarrisonMission::State::CompleteSuccess : GarrisonMission::State::CompleteFailed;
    }

    WorldPackets::Garrison::GarrisonCompleteMissionResult garrisonCompleteMissionResult;
    garrisonCompleteMissionResult.Result = canComplete ? GarrisonMission::Result::Success : GarrisonMission::Result::Fail;
    garrisonCompleteMissionResult.MissionData = mission->PacketInfo;
    garrisonCompleteMissionResult.Succeeded = success;
    _owner->SendDirectMessage(garrisonCompleteMissionResult.Write());
}

void Garrison::CalculateMissonBonusRoll(uint32 garrMissionId)
{
    GarrMissionEntry const* missionEntry = sGarrMissionStore.LookupEntry(garrMissionId);
    if (!missionEntry)
        return;

    Garrison::Mission* mission = GetMissionByID(missionEntry->ID);
    if (!mission)
        return;

    bool withOvermaxReward = false;
    if (mission->PacketInfo.SuccessChance > 100)
        withOvermaxReward = roll_chance_i(mission->PacketInfo.SuccessChance - 100);

    RewardMission(mission, withOvermaxReward);

    WorldPackets::Garrison::GarrisonMissionBonusRollResult garrisonMissionBonusRollResult;
    garrisonMissionBonusRollResult.MissionData = mission->PacketInfo;
    garrisonMissionBonusRollResult.Result = 0;
    _owner->SendDirectMessage(garrisonMissionBonusRollResult.Write());

    DeleteMission(mission->PacketInfo.DbID);
}

void Garrison::RewardMission(Mission* mission, bool withOvermaxReward)
{
    auto rewardLists = { mission->Rewards };
    if (withOvermaxReward)
        rewardLists = { mission->Rewards, mission->OvermaxRewards };

    for (auto rewards : rewardLists)
    {
        for (WorldPackets::Garrison::GarrisonMissionReward reward : rewards)
        {
            if (reward.ItemID)
                GetOwner()->AddItem(reward.ItemID, reward.ItemQuantity);

            if (reward.CurrencyID)
                GetOwner()->ModifyCurrency(reward.CurrencyID, reward.CurrencyQuantity);

            if (reward.FollowerXP)
            {
                std::vector<Garrison::Follower*> followers = GetMissionFollowers(mission->PacketInfo.MissionRecID);
                for (Garrison::Follower* follower : followers)
                    follower->EarnXP(GetOwner(), reward.FollowerXP);
            }

            //if (reward.BonusAbilityID)
                       // TODO

      //if (reward.Unknown)
                // TODO
        }
    }
}

Map* Garrison::FindMap() const
{
    return sMapMgr->FindMap(_siteLevel->MapID, _owner->GetGUID().GetCounter());
}

uint32 Garrison::Follower::GetItemLevel() const
{
    return (PacketInfo.ItemLevelWeapon + PacketInfo.ItemLevelArmor) / 2;
}

void Garrison::Follower::EarnXP(Player* owner, uint32 xp)
{
    GarrFollowerEntry const* followerEntry = sGarrFollowerStore.LookupEntry(PacketInfo.GarrFollowerID);
    if (!followerEntry)
        return;

    WorldPackets::Garrison::GarrisonFollowerChangedXP garrisonFollowerChangeXP;
    garrisonFollowerChangeXP.Follower = PacketInfo;
    garrisonFollowerChangeXP.TotalXp = _EarnXP(xp);
    PacketInfo.AbilityID = sGarrisonMgr.RollFollowerAbilities(PacketInfo.GarrFollowerID, followerEntry, PacketInfo.Quality, owner->GetTeam() == HORDE ? GARRISON_FACTION_INDEX_HORDE : GARRISON_FACTION_INDEX_ALLIANCE, false);
    garrisonFollowerChangeXP.Follower2 = PacketInfo;
    owner->SendDirectMessage(garrisonFollowerChangeXP.Write());
}

const uint32 FollowerMaxLevel[GARRISON_TYPE_MAX] =
{
    0,
    0,
    40,    // GARRISON_TYPE_GARRISON
    50,    // GARRISON_TYPE_CLASS_HALL
    60,    // GARRISON_TYPE_WAR_CAMPAIGN
    70,    // GARRISON_TYPE_COVENANT
};

uint32 Garrison::Follower::_EarnXP(uint32 xp)
{
    GarrFollowerEntry const* followerEntry = sGarrFollowerStore.LookupEntry(PacketInfo.GarrFollowerID);
    if (!followerEntry)
        return 0;

    uint32 requiredLevelUpXP = GetRequiredLevelUpXP();
    if (!requiredLevelUpXP)
        return 0;

    if (PacketInfo.Xp + xp < requiredLevelUpXP)
    {
        PacketInfo.Xp += xp;
        return xp;
    }

    uint32 XPToMax = requiredLevelUpXP - PacketInfo.Xp;
    PacketInfo.Xp = 0;

    bool canLevelUp = PacketInfo.FollowerLevel < FollowerMaxLevel[followerEntry->GarrTypeID];
    if (canLevelUp)
        ++PacketInfo.FollowerLevel;
    else
        ++PacketInfo.Quality;

    return xp + _EarnXP(xp - XPToMax);
}

uint32 Garrison::Follower::GetRequiredLevelUpXP() const
{
    GarrFollowerEntry const* followerEntry = sGarrFollowerStore.LookupEntry(PacketInfo.GarrFollowerID);
    if (!followerEntry)
        return 0;

    if (PacketInfo.FollowerLevel < FollowerMaxLevel[followerEntry->GarrTypeID])
    {
        for (uint32 i = 0; i < sGarrFollowerLevelXPStore.GetNumRows(); ++i)
            if (GarrFollowerLevelXPEntry const* currentLevelData = sGarrFollowerLevelXPStore.LookupEntry(i))
                if (currentLevelData->FollowerLevel == PacketInfo.FollowerLevel)
                    return currentLevelData->XpToNextLevel;
    }
    else
    {
        for (uint32 i = 0; i < sGarrFollowerQualityStore.GetNumRows(); ++i)
            if (GarrFollowerQualityEntry const* garrFollowerQualityEntry = sGarrFollowerQualityStore.LookupEntry(i))
                if (garrFollowerQualityEntry->Quality == PacketInfo.Quality)
                    if (garrFollowerQualityEntry->GarrFollowerTypeId == followerEntry->GarrFollowerTypeID)
                        return garrFollowerQualityEntry->XpToNextQuality;
    }

    return 0;
}

bool Garrison::Follower::HasAbility(uint32 garrAbilityId) const
{
    return std::find_if(PacketInfo.AbilityID.begin(), PacketInfo.AbilityID.end(), [garrAbilityId](GarrAbilityEntry const* garrAbility)
        {
            return garrAbility->ID == garrAbilityId;
        }) != PacketInfo.AbilityID.end();
}

void Garrison::Follower::ModAssistant(SpellInfo const* spellInfo, Player* caster)
{
    enum Types : uint32
    {
        SetWeaponLevel = 0,
        SetArmorLevel = 1,
        IncreaseWeaponLevel = 2,
        IncreaseArmorLevel = 3
    };

  //  auto itemLevelUpgradeDataEntry = sGarrItemLevelUpgradeDataStore[spellInfo->Effects[0]->MiscValue];
 //   if (!itemLevelUpgradeDataEntry)
 //       return;

 //   if (itemLevelUpgradeDataEntry->FollowerTypeID != TypeID)
  //      return;

   // auto followerTypeData = sGarrFollowerTypeStore[TypeID];
  //  if (!followerTypeData)
   //     return;

  //  auto maxAllowedItemLevel = itemLevelUpgradeDataEntry->MaxItemLevel;
  //  if (maxAllowedItemLevel > followerTypeData->MaxItemLevel)
   //     return;

  //  if (!maxAllowedItemLevel)
  //      maxAllowedItemLevel = followerTypeData->MaxItemLevel;

//    auto updateInfo = false;
  //  uint32 value = spellInfo->Effects[0]->BasePoints;

   /* switch (itemLevelUpgradeDataEntry->Operation)
    {
    case SetWeaponLevel:
        updateInfo = true;
        PacketInfo.ItemLevelWeapon = value;
        break;
    case SetArmorLevel:
        updateInfo = true;
        PacketInfo.ItemLevelArmor = value;
        break;
    case IncreaseWeaponLevel:
        updateInfo = true;
        PacketInfo.ItemLevelWeapon += value;
        break;
    case IncreaseArmorLevel:
        updateInfo = true;
        PacketInfo.ItemLevelArmor += value;
        break;
    default:
        break;
    }

    if (!updateInfo)
        return;

    PacketInfo.ItemLevelWeapon = std::min(PacketInfo.ItemLevelWeapon, static_cast<uint32>(maxAllowedItemLevel));
    PacketInfo.ItemLevelArmor = std::min(PacketInfo.ItemLevelArmor, static_cast<uint32>(maxAllowedItemLevel));

    DbState = DB_STATE_CHANGED;*/

    WorldPackets::Garrison::GarrisonFollowerChangedItemLevel update;
    update.Follower = PacketInfo;
    caster->SendDirectMessage(update.Write());
}

void Garrison::ChangeFollowerVitality(uint32 followerID)
{
    auto follower = GetFollower(followerID);
    if (!follower)
        return;

    auto followerEntry = sGarrFollowerStore.LookupEntry(follower->PacketInfo.GarrFollowerID);
    if (!followerEntry)
        return;

   // if (followerEntry->GarrFollowerTypeID != spellInfo->Effects[0]->MiscValueB)
     //   return;

   // auto addedVitality = spellInfo->Effects[0]->BasePoints;
   // follower->PacketInfo.Health += addedVitality;
   // follower->db_state_ability = DB_STATE_NEW;

   /* WorldPackets::Garrison::GarrisonFollowerChangedDurability packet;
    packet.Follower = follower->PacketInfo;
    packet.UnkInt = addedVitality;
    _owner->SendDirectMessage(packet.Write());*/
}

void Garrison::SendMissionListUpdate(bool openMissionNpc) const
{
    uint8 type = _owner->GetCurrentGarrison();

    if (!_siteLevel)
        return;

    WorldPackets::Garrison::GarrisonMissionUpdate res;

    if (!_missionss[type].empty())
    {
        res.CanStartMission.reserve(_missionss[type].size());
        res.ArchivedMissions.reserve(_missionss[type].size());
    }

    for (auto const& i : _missionss[type])
    {
        res.CanStartMission.push_back(true);
        res.ArchivedMissions.push_back(i.second.PacketInfo.MissionRecID);
    }

    _owner->SendDirectMessage(res.Write());
}

uint32 Garrison::GetCountOFollowers() const
{
    uint8 type = _owner->GetMap()->GetEntry()->ExpansionID == 5 ? GARRISON_TYPE_GARRISON : GARRISON_TYPE_COVENANT;
    return _followerss[type].size();
}

void Garrison::IncrementStartedTodayMissions()
{
    ++_numMissionsStartedToday;
}

uint32 Garrison::GetNumMissionsStartedToday()
{
    return _numMissionsStartedToday;
}

uint32 getQuestIdReqForShipment(uint32 siteID, uint32 buildingType)
{
    switch (buildingType)
    {
    case GARR_BTYPE_ALCHEMY_LAB:
        return siteID == GARRISON_SITE_WOD_ALLIANCE ? 36641 : 37568;
    case GARR_BTYPE_TAILORING:
        return siteID == GARRISON_SITE_WOD_ALLIANCE ? 36643 : 37575;
    case GARR_BTYPE_FORGE:
        return siteID == GARRISON_SITE_WOD_ALLIANCE ? 35168 : 37569;
    case GARR_BTYPE_TANNERY:
        return siteID == GARRISON_SITE_WOD_ALLIANCE ? 36642 : 37574;
    case GARR_BTYPE_GEM:
        return siteID == GARRISON_SITE_WOD_ALLIANCE ? 36644 : 37573;
    case GARR_BTYPE_ENCHANTERS:
        return siteID == GARRISON_SITE_WOD_ALLIANCE ? 36645 : 37570;
    case GARR_BTYPE_ENGINEERING:
        return siteID == GARRISON_SITE_WOD_ALLIANCE ? 36646 : 37571;
    case GARR_BTYPE_SCRIBE:
        return siteID == GARRISON_SITE_WOD_ALLIANCE ? 36647 : 37572;
    case GARR_BTYPE_LUMBER_MILL:
        return siteID == GARRISON_SITE_WOD_ALLIANCE ? 36189 : 36137;
    case GARR_BTYPE_TRADING_POST:
        return siteID == GARRISON_SITE_WOD_ALLIANCE ? 37088 : 37062;
    default:
        return 0;
    }
    return 0;
}

void Garrison::OnGossipSelect(WorldObject* source)
{
   // if (!source->HasFlag(UNIT_FIELD_NPC_FLAGS2, UNIT_NPC_FLAG2_AI_OBSTACLE | UNIT_NPC_FLAG2_RECRUITER))
    //    return;

    auto data = sGarrisonMgr.GetGarrShipment(source->GetEntry(), SHIPMENT_GET_BY_NPC, _owner->GetClass());
    if (!data)
        return;

    //! GarrShipment should have garrType.
    auto site = _siteLevel[data->cEntry->GarrTypeID];

    if (uint32 questID = getQuestIdReqForShipment(_siteLevel->GarrSiteID, data->cEntry->GarrBuildingType))
        if (_owner->GetQuestStatus(questID) == QUEST_STATUS_NONE)
            return;

    if (!data->selectShipment(_owner))
        return;

}

bool Garrison::hasTallent(uint32 talentID) const
{
    auto talentEntry = sGarrTalentStore.LookupEntry(talentID);
    if (!talentEntry)
        return false;

    for (auto data : _classHallTalentStore)
    {
        if (data.GarrTalentID == talentID)
        {
            uint32 const researchTime = data.Flags & ClassHallTalentFlag::CLASS_HALL_TALENT_CHANGE; //? talentEntry->RespecDurationSecs : talentEntry->ResearchDurationSecs;
            return time(nullptr) > (researchTime + data.ResearchStartTime);
        }
    }

    return false;
};

void Garrison::OnGossipTradeSkill(WorldObject* source)
{
   // if (!source->HasFlag(UNIT_FIELD_NPC_FLAGS2, UNIT_NPC_FLAG2_TRADESKILL_NPC))
   //     return;

    TradeskillList const* trade = sGarrisonMgr.GetTradeSkill(source->GetEntry());
    if (!trade)
        return;

    //! ToDo: link npc with plot or something else about it.
  //  const Plot* plot = GetPlotWithNpc(source->GetEntry());
  //  if (!plot || !plot->BuildingInfo.PacketInfo)
     //   return;

   // GarrBuildingEntry const* existingBuilding = sGarrBuildingStore.AssertEntry(plot->BuildingInfo.PacketInfo->GarrBuildingID);

    //! SMSG_GARRISON_OPEN_TRADESKILL_NPC
    WorldPackets::Garrison::GarrisonTradeSkillResponse tradeSkillPacket;
    tradeSkillPacket.GUID = source->GetGUID();
    for (auto const& tr : *trade)
    {
        bool find = false;
        for (uint32& d : tradeSkillPacket.TradeSkill.SkillLineIDs)
        {
            if (d == tr.skillID)
            {
                find = true;
                break;
            }
        }
        if (!find)
            tradeSkillPacket.TradeSkill.SkillLineIDs.push_back(tr.skillID);
        tradeSkillPacket.TradeSkill.KnownAbilitySpellIDs.push_back(tr.spellID);
      //  tradeSkillPacket.PlayerConditionID.push_back(existingBuilding->UpgradeLevel < tr.reqBuildingLvl ? tr.conditionID : 0);
    }

    _owner->SendDirectMessage(tradeSkillPacket.Write());
}

void Garrison::SendShipmentInfo(ObjectGuid const& guid)
{

    if (guid.GetEntry() == 94429 || guid.GetEntry() == 95002)
    {
        SendShipYadShipmentInfo(guid);
        return;
    }

    GarrShipment const* shipment = sGarrisonMgr.GetGarrShipment(guid.GetEntry(), SHIPMENT_GET_BY_NPC, _owner->GetClass());
    //const Plot* plot = GetPlotWithBuildingType(shipment->cEntry->GarrBuildingType);

    auto site = _siteLevel[shipment->cEntry->GarrTypeID];

//   uint32 questID = getQuestIdReqForShipment(site->GarrSiteID, shipment->cEntry->GarrBuildingType);
 //  uint32 shipmentID = sGarrisonMgr.GetShipmentID(shipment);

   //if (!shipmentID)
    //    shipmentID = shipment->selectShipment(_owner);

    //SMSG_GET_SHIPMENT_INFO_RESPONSE
    WorldPackets::Garrison::GetShipmentInfoResponse shipmentResponse;
  //  shipmentResponse.Success = shipment && (plot || shipment->cEntry->GarrTypeID == GARRISON_TYPE_CLASS_HALL) && (!questID || questID && _owner->GetQuestStatus(questID) != QUEST_STATUS_NONE);

    //! placeholder for check is allowed shipment.
    uint32 sh = shipment->ShipmentID;
   // sh = sGarrisonMgr.GetShipmentID(shipment);
    if (!sh)
        sh = shipment->selectShipment(_owner);

    auto shipmentEntry = sCharShipmentStore.LookupEntry(sh);
   // if (shipmentEntry)
      //  shipmentResponse.Success = _owner->CheckShipment(shipmentEntry);

    if (shipmentResponse.Success)
    {
    //    // check if has finish quest for activate. if rewardet - use usual state. if in progress - send
       // shipmentResponse.ShipmentID = (!questID || _owner->GetQuestStatus(questID) == QUEST_STATUS_REWARDED) ? shipmentID : getProgressShipment(questID);
     //   shipmentResponse.Shipments.assign(_shipments[idxShipment(shipment->cEntry)].begin(), _shipments[idxShipment(shipment->cEntry)].end());

        if (shipment->cEntry->GarrTypeID == GARRISON_TYPE_CLASS_HALL && shipmentEntry)
            shipmentResponse.MaxShipments = shipmentEntry->MaxShipments;
      //  else
      //  {
        //    shipmentResponse.MaxShipments = sGarrBuildingStore.AssertEntry(plot->BuildingInfo.PacketInfo->GarrBuildingID)->ShipmentCapacity + GetShipmentMaxMod();
       //     shipmentResponse.PlotInstanceID = plot->BuildingInfo.PacketInfo->GarrPlotInstanceID;
       // }
    }

    _owner->SendDirectMessage(shipmentResponse.Write());

}

void Garrison::SendShipYadShipmentInfo(ObjectGuid const& guid)
{
}

void Garrison::StartClassHallUpgrade(uint32 tallentID)
{
    WorldPackets::Garrison::GarrisonUpgradeResult result;

    if (!canStartUpgrade())
      {
        result.Result = GARRISON_ERROR_ALREADY_RESEARCHING_TALENT;
        _owner->SendDirectMessage(result.Write());
        return;
      }

    GarrTalentEntry const* entry = sGarrTalentStore.LookupEntry(tallentID);
    if (!entry)
    {
        result.Result = GARRISON_ERROR_INVALID_TALENT;
        _owner->SendDirectMessage(result.Write());
        return;
    }

    // cost taken from old site, not from new
        if (!_owner->HasCurrency(entry->ResearchCostSource, entry->ResearchCostSource))
        {
        result.Result = GARRISON_ERROR_NOT_ENOUGH_CURRENCY;
        _owner->SendDirectMessage(result.Write());
        return;
        }

     _owner->RemoveCurrency(entry->ResearchCostSource, -entry->ResearchCostSource, CurrencyDestroyReason::Garrison);

    WorldPackets::Garrison::GarrisonResearchTalent data;
    data.TalentID = tallentID;
    data.ResearchTime = time(NULL);
    _owner->SendDirectMessage(data.Write());

  //  AddTalentToStore(tallentID, data.ResearchTime, 0, DB_STATE_NEW);

    result.Result = GARRISON_SUCCESS;
    _owner->SendDirectMessage(result.Write());
}

void Garrison::AddTalentToStore(uint32 talentID, uint32 _time, uint32 flags, uint32 db_state/* = DB_STATE_UNCHANGED*/)
{
    auto talentEntry = sGarrTalentStore.LookupEntry(talentID);
    if (!talentEntry)
        return;

    //! second check.
    for (auto data = _classHallTalentStore.begin(); data != _classHallTalentStore.end(); ++data)
    {
        GarrTalentEntry const* __entry = sGarrTalentStore.AssertEntry(data->GarrTalentID);
        if (__entry->Tier == talentEntry->Tier)
        {
            ObjectGuid::LowType lowGuid = _owner->GetGUID().GetCounter();
            auto stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHARACTER_GARRISON_TALENT_BY_ID);
            stmt->setUInt64(0, lowGuid);
            stmt->setUInt32(1, __entry->ID);
            CharacterDatabase.Execute(stmt);

            //! ToDo: remove _abilities
            _classHallTalentStore.erase(data);
            break;
        }
    }

    uint32 researchTime = ClassHallTalentFlag::CLASS_HALL_TALENT_CHANGE & flags ? talentEntry->ActiveDurationSecs : talentEntry->ActiveDurationSecs;
    researchTime += _time;

    WorldPackets::Garrison::GarrisonTalent talentData;
    talentData.GarrTalentID = talentID;
    talentData.ResearchStartTime = _time;
    talentData.Flags = flags;
 //   talentData.DbState = DbState;

    //! handle abilities added by talents.
    if (talentEntry->GarrAbilityID)
        _abilities.insert(talentEntry->GarrAbilityID);

 //   if (time(nullptr) < researchTime)
   //     talentResearchTimer = researchTime;
    else
    {
        talentData.Flags |= ClassHallTalentFlag::CLASS_HALL_TALENT_READY;
      //  auto id = sGarrTalentRankStore.LookupEntry(talentID);

     //   if (ID->PerkSpellID)
        //    _owner->CastSpell(_owner, talentEntry->PerkSpellID, true);
    }

    _classHallTalentStore.push_back(talentData);
}

bool Garrison::canStartUpgrade()
{
    enum q
    {
        QUEST_DRUID = 42588, //50175
        QUEST_MAGE = 42696, //50181
        QUEST_WARRIOR = 42611,//50178
        QUEST_PRIEST = 43277, //50172
        QUEST_MONK = 42191, //49435
        QUEST_HUNTER = 42526,//50163
        QUEST_DH = 42683, //50184
        QUEST_PALADIN = 42850, //50166
        QUEST_ROGUE = 43015, //50187
        QUEST_WARLOCK = 42601, //50169
        QUEST_SHAMAN = 41740, //50160
        QUEST_DK = 43268, //50190

    };

    uint32 q = 0;
    switch (_owner->GetClass())
    {
    case CLASS_DEMON_HUNTER:
        q = QUEST_DH;
        break;
    case CLASS_DRUID:
        q = QUEST_DRUID;
        break;
    case CLASS_MONK:
        q = QUEST_MONK;
        break;
    case CLASS_WARLOCK:
        q = QUEST_WARLOCK;
        break;
    case CLASS_SHAMAN:
        q = QUEST_SHAMAN;
        break;
    case CLASS_DEATH_KNIGHT:
        q = QUEST_DK;
        break;
    case CLASS_PRIEST:
        q = QUEST_PRIEST;
        break;
    case CLASS_ROGUE:
        q = QUEST_ROGUE;
        break;
    case CLASS_HUNTER:
        q = QUEST_HUNTER;
        break;
    case CLASS_PALADIN:
        q = QUEST_PALADIN;
        break;
    case CLASS_WARRIOR:
        q = QUEST_WARRIOR;
        break;
    case CLASS_MAGE:
        q = QUEST_MAGE;
        break;
    default:
        break;
    }

    if (_owner->GetQuestStatus(q) == QUEST_STATUS_NONE)
        return false;

    return true;
}

void Garrison::CreateShipment(ObjectGuid const& guid, uint32 count)
{
    auto shipment = sGarrisonMgr.GetGarrShipment(guid.GetEntry(), SHIPMENT_GET_BY_NPC, _owner->GetClass());
    if (!shipment)
        return;

    auto site = _siteLevel[shipment->cEntry->GarrTypeID];

  //  GarrPlotEntry const* plot = GetPlotWithBuildingType(shipment->cEntry->GarrBuildingType);
    //if (!plot && shipment->cEntry->GarrTypeID == GARRISON_TYPE_GARRISON)
     //   return;

    uint32 sh = shipment->ShipmentID;
  //  sh = sGarrisonMgr.GetShipmentID(shipment);

   // if (uint32 questID = getQuestIdReqForShipment(site->GarrSiteID, shipment->cEntry->GarrBuildingType))
     //   if (_owner->GetQuestStatus(questID) != QUEST_STATUS_REWARDED)
       //     sh = getProgressShipment(questID);

    if (!sh)
        sh = shipment->selectShipment(_owner);

    CharShipmentEntry const* shipmentEntry = sCharShipmentStore.LookupEntry(sh);
    if (!shipmentEntry)
        return;

    //! placeholder for artifact check.
   // if (!_owner->CheckShipment(shipmentEntry)) //! return if not allowed.
   //     return;

    uint32 max = 0;
    if (shipment->cEntry->GarrTypeID == GARRISON_TYPE_CLASS_HALL)
        max = shipmentEntry->MaxShipments;
 //   else
   //     max = sGarrBuildingStore.AssertEntry(plot->BuildingInfo.PacketInfo->GarrBuildingID)->ShipmentCapacity + GetShipmentMaxMod();

//    auto t_shipments = _shipments[idxShipment(shipment->cEntry)];
  //  if (t_shipments.size() >= max)
   //     return;

    uint32 spellCast = 0;
    for (uint32 i = 0; i < count; ++i)
    {
        spellCast = shipmentEntry->SpellID;
        switch (sh)
        {
        case 60:  spellCast = 172840; break;
        case 103: spellCast = 171959; break;
        case 106: spellCast = 172841; break;
        case 107: spellCast = 172842; break;
        case 108: spellCast = 172843; break;
        case 105: spellCast = 172844; break;
        case 104: spellCast = 172845; break;
        case 72:  spellCast = 172846; break;
        case 125: spellCast = 173065; break;
        case 123: spellCast = 173066; break;
        case 121: spellCast = 173067; break;
        case 135: spellCast = 173068; break;
        case 133: spellCast = 173069; break;
        case 131: spellCast = 173070; break;
        case 129: spellCast = 173071; break;
        case 127: spellCast = 173072; break;
        }
        if (spellCast)
            _owner->CastSpell(_owner, spellCast, false);

        //! if no shipment in spell - just add it after cast.
     //   if (SpellInfo const* excludeTargetSpellInfo = sSpellMgr->GetSpellInfo(spellCast))
           // if (!excludeTargetSpellInfo->HasEffect(SPELL_EFFECT_CREATE_SHIPMENT))
             //   CreateGarrisonShipment(shipmentEntry->ID);
    }
}
