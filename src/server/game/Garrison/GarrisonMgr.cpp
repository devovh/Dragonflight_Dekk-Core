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

#include "GarrisonMgr.h"
#include "Containers.h"
#include "DatabaseEnv.h"
#include "DB2Stores.h"
#include "Garrison.h"
#include "Log.h"
#include "ObjectMgr.h"
#include "Random.h"
#include "Timer.h"
#include "World.h"

GarrisonMgr& GarrisonMgr::Instance()
{
    static GarrisonMgr instance;
    return instance;
}

void GarrisonMgr::Initialize()
{
    for (GarrSiteLevelPlotInstEntry const* siteLevelPlotInst : sGarrSiteLevelPlotInstStore)
        _garrisonPlotInstBySiteLevel[siteLevelPlotInst->GarrSiteLevelID].push_back(siteLevelPlotInst);

    for (GameObjectsEntry const* gameObject : sGameObjectsStore)
        if (gameObject->TypeID == GAMEOBJECT_TYPE_GARRISON_PLOT)
            _garrisonPlots[gameObject->OwnerID][gameObject->PropValue[0]] = gameObject;

    for (GarrPlotBuildingEntry const* plotBuilding : sGarrPlotBuildingStore)
        _garrisonBuildingsByPlot[plotBuilding->GarrPlotID].insert(plotBuilding->GarrBuildingID);

    for (GarrBuildingPlotInstEntry const* buildingPlotInst : sGarrBuildingPlotInstStore)
        _garrisonBuildingPlotInstances[std::make_pair(buildingPlotInst->GarrBuildingID, buildingPlotInst->GarrSiteLevelPlotInstID)] = buildingPlotInst->ID;

    for (GarrBuildingEntry const* building : sGarrBuildingStore)
        _garrisonBuildingsByType[building->BuildingType].push_back(building->ID);

    for (GarrFollowerXAbilityEntry const* followerAbility : sGarrFollowerXAbilityStore)
    {
        if (GarrAbilityEntry const* ability = sGarrAbilityStore.LookupEntry(followerAbility->GarrAbilityID))
        {
            if (ability->GarrFollowerTypeID != FOLLOWER_TYPE_GARRISON && ability->GarrFollowerTypeID != FOLLOWER_TYPE_CLASS_ORDER)
                continue;

            if (!(ability->Flags & GARRISON_ABILITY_CANNOT_ROLL) && ability->Flags & GARRISON_ABILITY_FLAG_TRAIT)
                _garrisonFollowerRandomTraits.insert(ability);

            if (followerAbility->FactionIndex < 2)
            {
                if (ability->Flags & GARRISON_ABILITY_FLAG_TRAIT)
                    _garrisonFollowerAbilities[followerAbility->FactionIndex][followerAbility->GarrFollowerID].Traits.insert(ability);
                else
                    _garrisonFollowerAbilities[followerAbility->FactionIndex][followerAbility->GarrFollowerID].Counters.insert(ability);
            }
        }
    }

    InitializeDbIdSequences();
    LoadPlotFinalizeGOInfo();
    LoadFollowerClassSpecAbilities();
    LoadTalentNPCs();
    /*LoadMissionsRewards();
    LoadMissionsOwermaxRewards();
    LoadMissionsQuestLink();
    LoadBuildingSpawnNPC();
    LoadBuildingSpawnGo();
    LoadMissionLine();
    LoadShipment(); TODO
    LoadTradeSkill();*/
}

GarrSiteLevelEntry const* GarrisonMgr::GetGarrSiteLevelEntry(uint32 garrSiteId, uint32 level) const
{
    for (GarrSiteLevelEntry const* siteLevel : sGarrSiteLevelStore)
        if (siteLevel->GarrSiteID == garrSiteId && siteLevel->GarrLevel == level)
            return siteLevel;

    return nullptr;
}

std::vector<GarrSiteLevelPlotInstEntry const*> const* GarrisonMgr::GetGarrPlotInstForSiteLevel(uint32 garrSiteLevelId) const
{
    auto itr = _garrisonPlotInstBySiteLevel.find(garrSiteLevelId);
    if (itr != _garrisonPlotInstBySiteLevel.end())
        return &itr->second;

    return nullptr;
}

GameObjectsEntry const* GarrisonMgr::GetPlotGameObject(uint32 mapId, uint32 garrPlotInstanceId) const
{
    auto mapItr = _garrisonPlots.find(mapId);
    if (mapItr != _garrisonPlots.end())
    {
        auto plotItr = mapItr->second.find(garrPlotInstanceId);
        if (plotItr != mapItr->second.end())
            return plotItr->second;
    }

    return nullptr;
}

bool GarrisonMgr::IsPlotMatchingBuilding(uint32 garrPlotId, uint32 garrBuildingId) const
{
    auto plotItr = _garrisonBuildingsByPlot.find(garrPlotId);
    if (plotItr != _garrisonBuildingsByPlot.end())
        return plotItr->second.count(garrBuildingId) > 0;

    return false;
}

uint32 GarrisonMgr::GetGarrBuildingPlotInst(uint32 garrBuildingId, uint32 garrSiteLevelPlotInstId) const
{
    auto itr = _garrisonBuildingPlotInstances.find(std::make_pair(garrBuildingId, garrSiteLevelPlotInstId));
    if (itr != _garrisonBuildingPlotInstances.end())
        return itr->second;

    return 0;
}

uint32 GarrisonMgr::GetPreviousLevelBuildingId(uint32 buildingType, uint32 currentLevel) const
{
    auto itr = _garrisonBuildingsByType.find(buildingType);
    if (itr != _garrisonBuildingsByType.end())
        for (uint32 buildingId : itr->second)
            if (sGarrBuildingStore.AssertEntry(buildingId)->UpgradeLevel == currentLevel - 1)
                return buildingId;

    return 0;
}

FinalizeGarrisonPlotGOInfo const* GarrisonMgr::GetPlotFinalizeGOInfo(uint32 garrPlotInstanceID) const
{
    return Trinity::Containers::MapGetValuePtr(_finalizePlotGOInfo, garrPlotInstanceID);
}

uint64 GarrisonMgr::GenerateFollowerDbId()
{
    if (_followerDbIdGenerator >= std::numeric_limits<uint64>::max())
    {
        TC_LOG_DEBUG("misc", "Garrison follower db id overflow! Can't continue, shutting down server. ");
        World::StopNow(ERROR_EXIT_CODE);
    }

    return _followerDbIdGenerator++;
}

uint64 GarrisonMgr::GenerateMissionDbId()
{
    if (_missionDbIdGenerator >= std::numeric_limits<uint64>::max())
    {
        TC_LOG_DEBUG("misc", "Garrison mission db id overflow! Can't continue, shutting down server. ");
        World::StopNow(ERROR_EXIT_CODE);
    }

    return _missionDbIdGenerator++;
}

uint64 GarrisonMgr::GenerateShipmentDbId()
{
    if (_shipmentDbIdGenerator >= std::numeric_limits<uint64>::max())
    {
        TC_LOG_DEBUG("misc", "Garrison shipment db id overflow! Can't continue, shutting down server. ");
        World::StopNow(ERROR_EXIT_CODE);
    }

    return _shipmentDbIdGenerator++;
}

uint32 const AbilitiesForQuality[][2] =
{
    // Counters, Traits
    { 0, 0 },
    { 1, 0 },
    { 1, 1 },   // Uncommon
    { 1, 2 },   // Rare
    { 2, 3 },   // Epic
    { 2, 3 }    // Legendary
};

std::list<GarrAbilityEntry const*> GarrisonMgr::RollFollowerAbilities(uint32 garrFollowerId, GarrFollowerEntry const* follower, uint32 quality, uint32 faction, bool initial) const
{
    ASSERT(faction < 2);

    bool hasForcedExclusiveTrait = false;
    std::list<GarrAbilityEntry const*> result;
    uint32 slots[2] = { AbilitiesForQuality[quality][0], AbilitiesForQuality[quality][1] };

    GarrAbilities const* abilities = nullptr;
    auto itr = _garrisonFollowerAbilities[faction].find(garrFollowerId);
    if (itr != _garrisonFollowerAbilities[faction].end())
        abilities = &itr->second;

    std::list<GarrAbilityEntry const*> abilityList, forcedAbilities, traitList, forcedTraits;
    if (abilities)
    {
        for (GarrAbilityEntry const* ability : abilities->Counters)
        {
            if (ability->Flags & GARRISON_ABILITY_HORDE_ONLY && faction != GARRISON_FACTION_INDEX_HORDE)
                continue;
            else if (ability->Flags & GARRISON_ABILITY_ALLIANCE_ONLY && faction != GARRISON_FACTION_INDEX_ALLIANCE)
                continue;

            if (ability->Flags & GARRISON_ABILITY_FLAG_CANNOT_REMOVE)
                forcedAbilities.push_back(ability);
            else
                abilityList.push_back(ability);
        }

        for (GarrAbilityEntry const* ability : abilities->Traits)
        {
            if (ability->Flags & GARRISON_ABILITY_HORDE_ONLY && faction != GARRISON_FACTION_INDEX_HORDE)
                continue;
            else if (ability->Flags & GARRISON_ABILITY_ALLIANCE_ONLY && faction != GARRISON_FACTION_INDEX_ALLIANCE)
                continue;

            if (ability->Flags & GARRISON_ABILITY_FLAG_CANNOT_REMOVE)
                forcedTraits.push_back(ability);
            else
                traitList.push_back(ability);
        }
    }

    Trinity::Containers::RandomResize(abilityList, std::max<int32>(0, slots[0] - forcedAbilities.size()));
    Trinity::Containers::RandomResize(traitList, std::max<int32>(0, slots[1] - forcedTraits.size()));

    // Add abilities specified in GarrFollowerXAbility.db2 before generic classspec ones on follower creation
    if (initial)
    {
        forcedAbilities.splice(forcedAbilities.end(), abilityList);
        forcedTraits.splice(forcedTraits.end(), traitList);
    }

    forcedAbilities.sort();
    abilityList.sort();
    forcedTraits.sort();
    traitList.sort();

    // check if we have a trait from exclusive category
    for (GarrAbilityEntry const* ability : forcedTraits)
    {
        if (ability->Flags & GARRISON_ABILITY_FLAG_EXCLUSIVE)
        {
            hasForcedExclusiveTrait = true;
            break;
        }
    }

    if (slots[0] > forcedAbilities.size() + abilityList.size())
    {
        std::list<GarrAbilityEntry const*> classSpecAbilities = GetClassSpecAbilities(follower, faction);
        std::list<GarrAbilityEntry const*> classSpecAbilitiesTemp, classSpecAbilitiesTemp2;
        classSpecAbilitiesTemp2.swap(abilityList);
        std::set_difference(classSpecAbilities.begin(), classSpecAbilities.end(), forcedAbilities.begin(), forcedAbilities.end(), std::back_inserter(classSpecAbilitiesTemp));
        std::set_union(classSpecAbilitiesTemp.begin(), classSpecAbilitiesTemp.end(), classSpecAbilitiesTemp2.begin(), classSpecAbilitiesTemp2.end(), std::back_inserter(abilityList));

        Trinity::Containers::RandomResize(abilityList, std::max<int32>(0, slots[0] - forcedAbilities.size()));
    }

    if (slots[1] > forcedTraits.size() + traitList.size())
    {
        std::list<GarrAbilityEntry const*> genericTraits, genericTraitsTemp;
        for (GarrAbilityEntry const* ability : _garrisonFollowerRandomTraits)
        {
            if (ability->Flags & GARRISON_ABILITY_HORDE_ONLY && faction != GARRISON_FACTION_INDEX_HORDE)
                continue;
            else if (ability->Flags & GARRISON_ABILITY_ALLIANCE_ONLY && faction != GARRISON_FACTION_INDEX_ALLIANCE)
                continue;

            // forced exclusive trait exists, skip other ones entirely
            if (hasForcedExclusiveTrait && ability->Flags & GARRISON_ABILITY_FLAG_EXCLUSIVE)
                continue;

            genericTraitsTemp.push_back(ability);
        }

        std::set_difference(genericTraitsTemp.begin(), genericTraitsTemp.end(), forcedTraits.begin(), forcedTraits.end(), std::back_inserter(genericTraits));
        genericTraits.splice(genericTraits.begin(), traitList);
        // "split" the list into two parts [nonexclusive, exclusive] to make selection later easier
        genericTraits.sort([](GarrAbilityEntry const* a1, GarrAbilityEntry const* a2)
            {
                uint32 e1 = a1->Flags & GARRISON_ABILITY_FLAG_EXCLUSIVE;
                uint32 e2 = a2->Flags & GARRISON_ABILITY_FLAG_EXCLUSIVE;
                if (e1 != e2)
                    return e1 < e2;

                return a1->ID < a2->ID;
            });
        genericTraits.unique();

        std::size_t firstExclusive = 0, total = genericTraits.size();
        for (auto genericTraitItr = genericTraits.begin(); genericTraitItr != genericTraits.end(); ++genericTraitItr, ++firstExclusive)
            if ((*genericTraitItr)->Flags & GARRISON_ABILITY_FLAG_EXCLUSIVE)
                break;

        while (traitList.size() < size_t(std::max<int32>(0, slots[1] - forcedTraits.size())) && total)
        {
            auto genericTraitItr = genericTraits.begin();
            std::advance(genericTraitItr, urand(0, total-- - 1));
            if ((*genericTraitItr)->Flags & GARRISON_ABILITY_FLAG_EXCLUSIVE)
                total = firstExclusive; // selected exclusive trait - no other can be selected now
            else
                --firstExclusive;

            traitList.push_back(*genericTraitItr);
            genericTraits.erase(genericTraitItr);
        }
    }

    result.splice(result.end(), forcedAbilities);
    result.splice(result.end(), abilityList);
    result.splice(result.end(), forcedTraits);
    result.splice(result.end(), traitList);

    return result;
}

std::list<GarrAbilityEntry const*> GarrisonMgr::GetClassSpecAbilities(GarrFollowerEntry const* follower, uint32 faction) const
{
    std::list<GarrAbilityEntry const*> abilities;
    uint32 classSpecId;
    switch (faction)
    {
    case GARRISON_FACTION_INDEX_HORDE:
        classSpecId = follower->HordeGarrClassSpecID;
        break;
    case GARRISON_FACTION_INDEX_ALLIANCE:
        classSpecId = follower->AllianceGarrClassSpecID;
        break;
    default:
        return abilities;
    }

    if (!sGarrClassSpecStore.LookupEntry(classSpecId))
        return abilities;

    auto itr = _garrisonFollowerClassSpecAbilities.find(classSpecId);
    if (itr != _garrisonFollowerClassSpecAbilities.end())
        abilities = itr->second;

    return abilities;
}

uint32 GarrisonMgr::GetMissionSuccessChance(Garrison* /*garrison*/, uint32 /*missionId*/)
{
    uint32 winChance = 100;

    // TODO

    return winChance;
}

uint32 GarrisonMgr::GetClassByMissionType(uint32 missionType)
{
    switch (missionType)
    {
    case GarrisonMission::Type::ArtifactMonk:           return CLASS_MONK;
    case GarrisonMission::Type::ArtifactShaman:         return CLASS_SHAMAN;
    case GarrisonMission::Type::ArtifactDruid:          return CLASS_DRUID;
    case GarrisonMission::Type::ArtifactMage:           return CLASS_MAGE;
    case GarrisonMission::Type::ArtifactHunter:         return CLASS_HUNTER;
    case GarrisonMission::Type::ArtifactPaladin:        return CLASS_PALADIN;
    case GarrisonMission::Type::ArtifactWarlock:        return CLASS_WARLOCK;
    case GarrisonMission::Type::ArtifactDemonHunter:    return CLASS_DEMON_HUNTER;
    case GarrisonMission::Type::ArtifactRogue:          return CLASS_ROGUE;
    case GarrisonMission::Type::ArtifactPriest:         return CLASS_PRIEST;
    case GarrisonMission::Type::ArtifactDeathKnight:    return CLASS_DEATH_KNIGHT;
    case GarrisonMission::Type::ArtifactWarrior:        return CLASS_WARRIOR;
    }

    return CLASS_NONE;
}

uint32 GarrisonMgr::GetFactionByMissionType(uint32 missionType)
{
    switch (missionType)
    {
    case GarrisonMission::Type::ZoneSupportAlliance:    return TEAM_ALLIANCE;
    case GarrisonMission::Type::ZoneSupportHorde:       return TEAM_HORDE;
    }

    return TEAM_OTHER;
}

void GarrisonMgr::InitializeDbIdSequences()
{
    if (QueryResult result = CharacterDatabase.Query("SELECT MAX(dbId) FROM character_garrison_followers"))
        _followerDbIdGenerator = (*result)[0].GetUInt64() + 1;

    if (QueryResult result = CharacterDatabase.Query("SELECT MAX(dbId) FROM character_garrison_missions"))
        _missionDbIdGenerator = (*result)[0].GetUInt64() + 1;
}

void GarrisonMgr::LoadPlotFinalizeGOInfo()
{
    //                                                                0                  1       2       3       4       5               6
    QueryResult result = WorldDatabase.Query("SELECT garrPlotInstanceId, hordeGameObjectId, hordeX, hordeY, hordeZ, hordeO, hordeAnimKitId, "
        //                      7          8          9         10         11                 12
        "allianceGameObjectId, allianceX, allianceY, allianceZ, allianceO, allianceAnimKitId FROM garrison_plot_finalize_info");
    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 garrison follower class spec abilities. DB table `garrison_plot_finalize_info` is empty.");
        return;
    }

    uint32 msTime = getMSTime();
    do
    {
        Field* fields = result->Fetch();
        uint32 garrPlotInstanceId = fields[0].GetUInt32();
        uint32 hordeGameObjectId = fields[1].GetUInt32();
        uint32 allianceGameObjectId = fields[7].GetUInt32();
        uint16 hordeAnimKitId = fields[6].GetUInt16();
        uint16 allianceAnimKitId = fields[12].GetUInt16();

        if (!sGarrPlotInstanceStore.LookupEntry(garrPlotInstanceId))
        {
            TC_LOG_DEBUG("sql.sql", "Non-existing GarrPlotInstance.db2 entry {} was referenced in `garrison_plot_finalize_info`.", garrPlotInstanceId);
            continue;
        }

        GameObjectTemplate const* goTemplate = sObjectMgr->GetGameObjectTemplate(hordeGameObjectId);
        if (!goTemplate)
        {
            TC_LOG_DEBUG("sql.sql", "Non-existing gameobject_template entry {} was referenced in `garrison_plot_finalize_info`.`hordeGameObjectId` for garrPlotInstanceId {}.",
                hordeGameObjectId, garrPlotInstanceId);
            continue;
        }

        if (goTemplate->type != GAMEOBJECT_TYPE_GOOBER)
        {
            TC_LOG_DEBUG("sql.sql", "Invalid gameobject type {} (entry {}) was referenced in `garrison_plot_finalize_info`.`hordeGameObjectId` for garrPlotInstanceId {}.",
                goTemplate->type, hordeGameObjectId, garrPlotInstanceId);
            continue;
        }

        goTemplate = sObjectMgr->GetGameObjectTemplate(allianceGameObjectId);
        if (!goTemplate)
        {
            TC_LOG_DEBUG("sql.sql", "Non-existing gameobject_template entry {}  was referenced in `garrison_plot_finalize_info`.`allianceGameObjectId` for garrPlotInstanceId {} .",
                allianceGameObjectId, garrPlotInstanceId);
            continue;
        }

        if (goTemplate->type != GAMEOBJECT_TYPE_GOOBER)
        {
            TC_LOG_DEBUG("sql.sql", "Invalid gameobject type {}  (entry {} ) was referenced in `garrison_plot_finalize_info`.`allianceGameObjectId` for garrPlotInstanceId {} .",
                goTemplate->type, allianceGameObjectId, garrPlotInstanceId);
            continue;
        }

        if (hordeAnimKitId && !sAnimKitStore.LookupEntry(hordeAnimKitId))
        {
            TC_LOG_DEBUG("sql.sql", "Non-existing AnimKit.dbc entry {}  was referenced in `garrison_plot_finalize_info`.`hordeAnimKitId` for garrPlotInstanceId {} .",
                hordeAnimKitId, garrPlotInstanceId);
            continue;
        }

        if (allianceAnimKitId && !sAnimKitStore.LookupEntry(allianceAnimKitId))
        {
            TC_LOG_DEBUG("sql.sql", "Non-existing AnimKit.dbc entry {}  was referenced in `garrison_plot_finalize_info`.`allianceAnimKitId` for garrPlotInstanceId {} .",
                allianceAnimKitId, garrPlotInstanceId);
            continue;
        }

        FinalizeGarrisonPlotGOInfo& info = _finalizePlotGOInfo[garrPlotInstanceId];
        info.FactionInfo[GARRISON_FACTION_INDEX_HORDE].GameObjectId = hordeGameObjectId;
        info.FactionInfo[GARRISON_FACTION_INDEX_HORDE].Pos.Relocate(fields[2].GetFloat(), fields[3].GetFloat(), fields[4].GetFloat(), fields[5].GetFloat());
        info.FactionInfo[GARRISON_FACTION_INDEX_HORDE].AnimKitId = hordeAnimKitId;

        info.FactionInfo[GARRISON_FACTION_INDEX_ALLIANCE].GameObjectId = allianceGameObjectId;
        info.FactionInfo[GARRISON_FACTION_INDEX_ALLIANCE].Pos.Relocate(fields[8].GetFloat(), fields[9].GetFloat(), fields[10].GetFloat(), fields[11].GetFloat());
        info.FactionInfo[GARRISON_FACTION_INDEX_ALLIANCE].AnimKitId = allianceAnimKitId;

    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded {}  garrison plot finalize entries in {} .", uint32(_finalizePlotGOInfo.size()), GetMSTimeDiffToNow(msTime));
}

void GarrisonMgr::LoadFollowerClassSpecAbilities()
{
    QueryResult result = WorldDatabase.Query("SELECT classSpecId, abilityId FROM garrison_follower_class_spec_abilities");
    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 garrison follower class spec abilities. DB table `garrison_follower_class_spec_abilities` is empty.");
        return;
    }

    uint32 msTime = getMSTime();
    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();
        uint32 classSpecId = fields[0].GetUInt32();
        uint32 abilityId = fields[1].GetUInt32();

        if (!sGarrClassSpecStore.LookupEntry(classSpecId))
        {
            TC_LOG_DEBUG("sql.sql", "Non-existing GarrClassSpec.db2 entry {}  was referenced in `garrison_follower_class_spec_abilities` by row ({} , {} ).", classSpecId, classSpecId, abilityId);
            continue;
        }

        GarrAbilityEntry const* ability = sGarrAbilityStore.LookupEntry(abilityId);
        if (!ability)
        {
            TC_LOG_DEBUG("sql.sql", "Non-existing GarrAbility.db2 entry {}  was referenced in `garrison_follower_class_spec_abilities` by row ({} , {} ).", abilityId, classSpecId, abilityId);
            continue;
        }

        _garrisonFollowerClassSpecAbilities[classSpecId].push_back(ability);
        ++count;

    } while (result->NextRow());

    for (auto& pair : _garrisonFollowerClassSpecAbilities)
        pair.second.sort();

    TC_LOG_INFO("server.loading", ">> Loaded {}  garrison follower class spec abilities in {} .", count, GetMSTimeDiffToNow(msTime));
}

GarrisonTalentNPC const* GarrisonMgr::GetTalentNPCEntry(int32 entry) const
{
    return Trinity::Containers::MapGetValuePtr(_garrisonTalentNPCs, entry);
}

void GarrisonMgr::LoadTalentNPCs()
{
    //                                                      0                 1                    2
    QueryResult result = WorldDatabase.Query("SELECT NpcEntry, GarrTalentTreeID, FriendshipFactionID FROM garrison_talent_npc");
    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 garrison talent NPCs. DB table `garrison_talent_npc` is empty.");
        return;
    }

    uint32 msTime = getMSTime();
    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();
        uint32 npcEntry = fields[0].GetUInt32();
        uint32 garrTalentTreeID = fields[1].GetUInt32();
        uint32 friendshipFactionID = fields[2].GetUInt32();

        CreatureTemplate const* creatureEntry = sObjectMgr->GetCreatureTemplate(npcEntry);
        if (!creatureEntry)
        {
            TC_LOG_DEBUG("sql.sql", "Non-existing creature_template entry {}  was referenced in `garrison_talent_npc`.`npcEntry` for npcEntry {} .",
                npcEntry, garrTalentTreeID);
            continue;
        }

        GarrTalentTreeEntry const* talentTree = sGarrTalentTreeStore.LookupEntry(garrTalentTreeID);
        if (!talentTree)
        {
            TC_LOG_DEBUG("sql.sql", "Non-existing GarrTalentTree.db2 entry {}  was referenced in `garrison_talent_npc` by row ({} , {} ).", garrTalentTreeID, npcEntry, garrTalentTreeID);
            continue;
        }

        if (FactionEntry const* faction = sFactionStore.LookupEntry(friendshipFactionID))
        {
            if (!sFriendshipReputationStore.LookupEntry(faction->FriendshipRepID))
            {
                TC_LOG_DEBUG("sql.sql", "NPC entry {}  has invalid FriendshipFactionID ({} ) in `garrison_talent_npc`, set to 0.", npcEntry, friendshipFactionID);
                friendshipFactionID = 0;
            }
        }

        GarrisonTalentNPC& data = _garrisonTalentNPCs[npcEntry];
        data.GarrTalentTreeID = garrTalentTreeID;
        data.FriendshipFactionID = friendshipFactionID;
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded {}  garrison talent NPCs in {} .", count, GetMSTimeDiffToNow(msTime));
}

GarrShipment const* GarrisonMgr::GetGarrShipment(uint32 entry, ShipmentGetType type, uint8 classID) const
{
    auto i = shipment.find(type);
    if (i == shipment.end())
        return nullptr;

    auto itr = i->second.find(entry);
    if (itr == i->second.end())
        return nullptr;

    if (itr->second.classReq != 0 && !(itr->second.classReq & ((1 << (classID - 1)))))
        return nullptr;

    return &itr->second;
}

uint32 GarrShipment::selectShipment(Player* p) const
{
    if (ShipmentID)
        return ShipmentID;

    auto g = p->GetGarrison(GARRISON_TYPE_CLASS_HALL);
    if (!g)
        return ShipmentID;

    DB2Manager::ShipmentConteinerMapBounds bound = sDB2Manager.GetShipmentConteinerBounds(ContainerID);
    for (auto itr2 = bound.first; itr2 != bound.second; ++itr2)
    {
        if (itr2->second->Flags & GARRISON_SHIPMENT_FLAG_REQUIRE_QUEST_NOT_COMPLETE)
            if (p->GetQuestStatus(questReq) == QUEST_STATUS_INCOMPLETE)
                return itr2->second->ID;

        if ((itr2->second->Flags & GARRISON_SHIPMENT_FLAG_REQUIRE_QUEST_NOT_COMPLETE) == 0)
        {
            //special. requirement.
            switch (itr2->second->ContainerID)
            {
                // class halls. mage.
            case 131:
                //http://ru.wowhead.com/order-advancement=384/elemental-power
                if (itr2->second->GarrFollowerID == 769 && !g->hasTallent(384))
                    continue;
                if (itr2->second->GarrFollowerID == 660 && g->hasTallent(384))
                    continue;
                break;
            case 132:
                //http://www.wowhead.com/order-advancement=385/higher-learning
                if (itr2->second->GarrFollowerID == 769 && !g->hasTallent(385))
                    continue;
                if (itr2->second->GarrFollowerID == 659 && g->hasTallent(385))
                    continue;
                break;
                // class hall. Dk
            case 134:
            {
                //if tallent http://www.wowhead.com/order-advancement=428/construct-quarter
                if (itr2->second->GarrFollowerID == 664 && !g->hasTallent(428))
                    continue;
                if (itr2->second->GarrFollowerID == 662 && g->hasTallent(428))
                    continue;
                break;
            }
            case 135:
                //http://www.wowhead.com/order-advancement=429/live-by-the-sword
                if (itr2->second->GarrFollowerID == 894 && !g->hasTallent(429))
                    continue;
                if (itr2->second->GarrFollowerID == 663 && g->hasTallent(429))
                    continue;
                break;
                // class halls. warrior.
            case 158:
                //http://www.wowhead.com/order-advancement=406/trial-by-fire
                if (itr2->second->GarrFollowerID == 688 && !g->hasTallent(406))
                    continue;
                if (itr2->second->GarrFollowerID == 686 && g->hasTallent(406))
                    continue;
                break;
            case 159:
                //http://www.wowhead.com/order-advancement=407/ascension
                if (itr2->second->GarrFollowerID == 852 && !g->hasTallent(407))
                    continue;
                if (itr2->second->GarrFollowerID == 687 && g->hasTallent(407))
                    continue;
                break;
                // class halls. druid.
            case 140:
                //http://www.wowhead.com/order-advancement=351/laughing-sisters
                if (itr2->second->GarrFollowerID == 668 && !g->hasTallent(351))
                    continue;
                if (itr2->second->GarrFollowerID == 763 && g->hasTallent(351))
                    continue;
                break;
            case 141:
                //http://www.wowhead.com/order-advancement=352/force-of-the-forest
                if (itr2->second->GarrFollowerID == 670 && !g->hasTallent(352))
                    continue;
                if (itr2->second->GarrFollowerID == 669 && g->hasTallent(352))
                    continue;
                break;
                // class halls. prist.
            case 149:
                //http://www.wowhead.com/order-advancement=450/inquisition
                if (itr2->second->GarrFollowerID == 678 && !g->hasTallent(450))
                    continue;
                if (itr2->second->GarrFollowerID == 677 && g->hasTallent(450))
                    continue;
                break;
            case 150:
                //http ://www.wowhead.com/order-advancement=451/shadow-heresy
                if (itr2->second->GarrFollowerID == 920 && !g->hasTallent(451))
                    continue;
                if (itr2->second->GarrFollowerID == 679 && g->hasTallent(451))
                    continue;
                break;
                // class halls. monk.
            case 124:
                //http://www.wowhead.com/order-advancement=250/path-of-the-ox
                if (itr2->second->GarrFollowerID == 627 && !g->hasTallent(250))
                    continue;
                if (itr2->second->GarrFollowerID == 622 && g->hasTallent(250))
                    continue;
                break;
            case 125:
                //http://www.wowhead.com/order-advancement=251/path-of-the-tiger
                if (itr2->second->GarrFollowerID == 630 && !g->hasTallent(251))
                    continue;
                if (itr2->second->GarrFollowerID == 629 && g->hasTallent(251))
                    continue;
                break;
                // class halls. hunter.
            case 143:
                //http://www.wowhead.com/order-advancement=373/keen-eye
                if (itr2->second->GarrFollowerID == 799 && !g->hasTallent(373))
                    continue;
                if (itr2->second->GarrFollowerID == 671 && g->hasTallent(373))
                    continue;
                break;
            case 144:
                //http://www.wowhead.com/order-advancement=374/wild-calling
                if (itr2->second->GarrFollowerID == 800 && !g->hasTallent(374))
                    continue;
                if (itr2->second->GarrFollowerID == 672 && g->hasTallent(374))
                    continue;
                break;
                // class halls. paladin.
            case 146:
                //http://www.wowhead.com/order-advancement=395/as-one
                if (itr2->second->GarrFollowerID == 770 && !g->hasTallent(395))
                    continue;
                if (itr2->second->GarrFollowerID == 674 && g->hasTallent(395))
                    continue;
                break;
            case 147:
                //http://www.wowhead.com/order-advancement=396/templar-of-the-silver-hand
                if (itr2->second->GarrFollowerID == 771 && !g->hasTallent(396))
                    continue;
                if (itr2->second->GarrFollowerID == 675 && g->hasTallent(396))
                    continue;
                break;
                // class halls. rogue
            case 152:
                //http://www.wowhead.com/order-advancement=439/defiant-legacy
                if (itr2->second->GarrFollowerID == 681 && !g->hasTallent(439))
                    continue;
                if (itr2->second->GarrFollowerID == 680 && g->hasTallent(439))
                    continue;
                break;
            case 153:
                //http://www.wowhead.com/order-advancement=440/crimson-sails
                if (itr2->second->GarrFollowerID == 907 && !g->hasTallent(440))
                    continue;
                if (itr2->second->GarrFollowerID == 682 && g->hasTallent(440))
                    continue;
                break;
                // class halls. warlock
            case 129:
                //http ://www.wowhead.com/order-advancement=362/dark-mastery
                if (itr2->second->GarrFollowerID == 681 && !g->hasTallent(362))
                    continue;
                if (itr2->second->GarrFollowerID == 649 && g->hasTallent(362))
                    continue;
                break;
            case 128:
                //http://www.wowhead.com/order-advancement=365/grimoire-of-servitude
                if (itr2->second->GarrFollowerID == 767 && !g->hasTallent(365))
                    continue;
                if (itr2->second->GarrFollowerID == 741 && g->hasTallent(365))
                    continue;
                break;
                // class halls. demon hunter.
            case 137:
                //http://www.wowhead.com/order-advancement=417/naga-myrmidons
                if (itr2->second->GarrFollowerID == 876 && !g->hasTallent(417))
                    continue;
                if (itr2->second->GarrFollowerID == 665 && g->hasTallent(417))
                    continue;
                break;
            case 138:
                //http://www.wowhead.com/order-advancement=418/demonic-power
                if (itr2->second->GarrFollowerID == 877 && !g->hasTallent(418))
                    continue;
                if (itr2->second->GarrFollowerID == 666 && g->hasTallent(418))
                    continue;
                break;
            default:
                break;
            }

            if (questReq)
            {
                QuestStatus status = p->GetQuestStatus(questReq);

                if (status != QUEST_STATUS_INCOMPLETE && status != QUEST_STATUS_REWARDED)
                    return 0;

                if (status != QUEST_STATUS_REWARDED)
                    continue;

            }
            return itr2->second->ID;
        }
        return itr2->second->ID;
    }
    return ShipmentID;
}

TradeskillList const* GarrisonMgr::GetTradeSkill(uint32 npcID)
{
    return Trinity::Containers::MapGetValuePtr(_garrNpcTradeSkill, npcID);
}
