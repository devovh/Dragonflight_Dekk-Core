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

#include "InstanceScript.h"
#include "AreaBoundary.h"
#include "Creature.h"
#include "CreatureAI.h"
#include "CreatureAIImpl.h"
#include "DatabaseEnv.h"
#include "DB2Stores.h"
#include "GameEventSender.h"
#include "GameObject.h"
#include "Group.h"
#include "InstancePackets.h"
#include "InstanceScenario.h"
#include "InstanceScriptData.h"
#include "LFGMgr.h"
#include "Log.h"
#include "Map.h"
#include "ObjectMgr.h"
#include "PhasingHandler.h"
#include "Player.h"
#include "RBAC.h"
#include "ScriptedCreature.h"
#include "ScriptReloadMgr.h"
#include "SmartEnum.h"
#include "SpellMgr.h"
#include "World.h"
#include "WorldSession.h"
#include "WorldStateMgr.h"
#include <cstdarg>

 // DekkCore >
#include "SpellMgr.h"
#include "Challenge.h"
#include "ScenarioMgr.h"
#include "ChallengeModeMgr.h"
#include "MiscPackets.h"
// < DekkCore

#ifdef TRINITY_API_USE_DYNAMIC_LINKING
#include "ScriptMgr.h"
#endif

BossBoundaryData::~BossBoundaryData()
{
    for (const_iterator it = begin(); it != end(); ++it)
        delete it->Boundary;
}

DungeonEncounterEntry const* BossInfo::GetDungeonEncounterForDifficulty(Difficulty difficulty) const
{
    auto itr = std::find_if(DungeonEncounters.begin(), DungeonEncounters.end(), [difficulty](DungeonEncounterEntry const* dungeonEncounter)
        {
            return dungeonEncounter && (dungeonEncounter->DifficultyID == 0 || Difficulty(dungeonEncounter->DifficultyID) == difficulty);
        });

    return itr != DungeonEncounters.end() ? *itr : nullptr;
}

InstanceScript::InstanceScript(InstanceMap* map) : instance(map), completedEncounters(0), _instanceSpawnGroups(sObjectMgr->GetInstanceSpawnGroupsForMap(map->GetId())),
_entranceId(0), _temporaryEntranceId(0), _combatResurrectionTimer(0), _combatResurrectionCharges(0), _combatResurrectionTimerStarted(false)
{
#ifdef TRINITY_API_USE_DYNAMIC_LINKING
    uint32 scriptId = sObjectMgr->GetInstanceTemplate(map->GetId())->ScriptId;
    auto const scriptname = sObjectMgr->GetScriptName(scriptId);
    ASSERT(!scriptname.empty());
    // Acquire a strong reference from the script module
    // to keep it loaded until this object is destroyed.
    module_reference = sScriptMgr->AcquireModuleReferenceOfScriptName(scriptname);
#endif // #ifndef TRINITY_API_USE_DYNAMIC_LINKING
}

InstanceScript::~InstanceScript()
{
}

bool InstanceScript::IsEncounterInProgress() const
{
    for (std::vector<BossInfo>::const_iterator itr = bosses.begin(); itr != bosses.end(); ++itr)
        if (itr->state == IN_PROGRESS)
            return true;

    return false;
}

void InstanceScript::OnCreatureCreate(Creature* creature)
{
    AddObject(creature, true);
    AddMinion(creature, true);
}

void InstanceScript::OnCreatureRemove(Creature* creature)
{
    AddObject(creature, false);
    AddMinion(creature, false);
}

void InstanceScript::OnGameObjectCreate(GameObject* go)
{
    AddObject(go, true);
    AddDoor(go, true);

    //DekkCore
    if (sChallengeModeMgr->IsChest(go->GetEntry()))
        _challengeChest = go->GetGUID();

    if (sChallengeModeMgr->IsDoor(go->GetEntry()))
        AddChallengeModeDoor(go->GetGUID());

    if (go->GetEntry() == ChallengeModeOrb)
        AddChallengeModeOrb(go->GetGUID());
    //DekkCore
}

void InstanceScript::OnGameObjectRemove(GameObject* go)
{
    AddObject(go, false);
    AddDoor(go, false);
}

ObjectGuid InstanceScript::GetObjectGuid(uint32 type) const
{
    if (!_objectGuids.empty())
    {
        ObjectGuidMap::const_iterator i = _objectGuids.find(type);
        if (i != _objectGuids.end())
            return i->second;
    }
    return ObjectGuid::Empty;
}

ObjectGuid InstanceScript::GetGuidData(uint32 type) const
{
    return GetObjectGuid(type);
}

void InstanceScript::TriggerGameEvent(uint32 gameEventId, WorldObject* source /*= nullptr*/, WorldObject* target /*= nullptr*/)
{
    if (source)
    {
        ZoneScript::TriggerGameEvent(gameEventId, source, target);
        return;
    }

    ProcessEvent(target, gameEventId, source);
    instance->DoOnPlayers([gameEventId](Player* player)
        {
            GameEvents::TriggerForPlayer(gameEventId, player);
        });

    GameEvents::TriggerForMap(gameEventId, instance);
}

Creature* InstanceScript::GetCreature(uint32 type)
{
    ObjectGuid guid = GetObjectGuid(type);
    if (!guid || guid == ObjectGuid::Empty)
    {
        return nullptr;
    }
    return instance ? instance->GetCreature(guid) : nullptr;
}

GameObject* InstanceScript::GetGameObject(uint32 type)
{
    ObjectGuid guid = GetObjectGuid(type);
    if (!guid || guid == ObjectGuid::Empty)
    {
        return nullptr;
    }
    return instance ? instance->GetGameObject(guid) : nullptr;
}

void InstanceScript::SetHeaders(std::string const& dataHeaders)
{
    headers = dataHeaders;
}

void InstanceScript::LoadBossBoundaries(BossBoundaryData const& data)
{
    for (BossBoundaryEntry const& entry : data)
        if (entry.BossId < bosses.size())
            bosses[entry.BossId].boundary.push_back(entry.Boundary);
}

void InstanceScript::LoadMinionData(MinionData const* data)
{
    while (data->entry)
    {
        if (data->bossId < bosses.size())
            minions.insert(std::make_pair(data->entry, MinionInfo(&bosses[data->bossId])));

        ++data;
    }
    TC_LOG_DEBUG("scripts", "InstanceScript::LoadMinionData: {} minions loaded.", uint64(minions.size()));
}

void InstanceScript::LoadDoorData(DoorData const* data)
{
    while (data->entry)
    {
        if (data->bossId < bosses.size())
            doors.insert(std::make_pair(data->entry, DoorInfo(&bosses[data->bossId], data->type)));

        ++data;
    }
    TC_LOG_DEBUG("scripts", "InstanceScript::LoadDoorData: {} doors loaded.", uint64(doors.size()));
}

void InstanceScript::LoadObjectData(ObjectData const* creatureData, ObjectData const* gameObjectData)
{
    if (creatureData)
        LoadObjectData(creatureData, _creatureInfo);

    if (gameObjectData)
        LoadObjectData(gameObjectData, _gameObjectInfo);

    TC_LOG_DEBUG("scripts", "InstanceScript::LoadObjectData: {} objects loaded.", _creatureInfo.size() + _gameObjectInfo.size());
}

void InstanceScript::LoadObjectData(ObjectData const* data, ObjectInfoMap& objectInfo)
{
    while (data->entry)
    {
        ASSERT(objectInfo.find(data->entry) == objectInfo.end());
        objectInfo[data->entry] = data->type;
        ++data;
    }
}

void InstanceScript::LoadDungeonEncounterData(uint32 bossId, std::array<uint32, MAX_DUNGEON_ENCOUNTERS_PER_BOSS> const& dungeonEncounterIds)
{
    if (bossId < bosses.size())
        for (std::size_t i = 0; i < MAX_DUNGEON_ENCOUNTERS_PER_BOSS; ++i)
            bosses[bossId].DungeonEncounters[i] = sDungeonEncounterStore.LookupEntry(dungeonEncounterIds[i]);
}

void InstanceScript::UpdateDoorState(GameObject* door)
{
    DoorInfoMapBounds range = doors.equal_range(door->GetEntry());
    if (range.first == range.second)
        return;

    bool open = true;
    for (; range.first != range.second && open; ++range.first)
    {
        DoorInfo const& info = range.first->second;
        switch (info.type)
        {
        case DOOR_TYPE_ROOM:
            open = (info.bossInfo->state != IN_PROGRESS);
            break;
        case DOOR_TYPE_PASSAGE:
            open = (info.bossInfo->state == DONE);
            break;
        case DOOR_TYPE_SPAWN_HOLE:
            open = (info.bossInfo->state == IN_PROGRESS);
            break;
        default:
            break;
        }
    }

    door->SetGoState(open ? GO_STATE_ACTIVE : GO_STATE_READY);
}

void InstanceScript::UpdateMinionState(Creature* minion, EncounterState state)
{
    switch (state)
    {
    case NOT_STARTED:
        if (!minion->IsAlive())
            minion->Respawn();
        else if (minion->IsInCombat())
            minion->AI()->EnterEvadeMode();
        break;
    case IN_PROGRESS:
        if (!minion->IsAlive())
            minion->Respawn();
        else if (!minion->GetVictim())
            minion->AI()->DoZoneInCombat();
        break;
    default:
        break;
    }
}

void InstanceScript::UpdateSpawnGroups()
{
    if (!_instanceSpawnGroups)
        return;
    enum states { BLOCK, SPAWN, FORCEBLOCK };
    std::unordered_map<uint32, states> newStates;
    for (auto it = _instanceSpawnGroups->begin(), end = _instanceSpawnGroups->end(); it != end; ++it)
    {
        InstanceSpawnGroupInfo const& info = *it;
        states& curValue = newStates[info.SpawnGroupId]; // makes sure there's a BLOCK value in the map
        if (curValue == FORCEBLOCK) // nothing will change this
            continue;
        if (!((1 << GetBossState(info.BossStateId)) & info.BossStates))
            continue;
        if (((instance->GetTeamIdInInstance() == TEAM_ALLIANCE) && (info.Flags & InstanceSpawnGroupInfo::FLAG_HORDE_ONLY))
            || ((instance->GetTeamIdInInstance() == TEAM_HORDE) && (info.Flags & InstanceSpawnGroupInfo::FLAG_ALLIANCE_ONLY)))
            continue;
        if (info.Flags & InstanceSpawnGroupInfo::FLAG_BLOCK_SPAWN)
            curValue = FORCEBLOCK;
        else if (info.Flags & InstanceSpawnGroupInfo::FLAG_ACTIVATE_SPAWN)
            curValue = SPAWN;
    }
    for (auto const& pair : newStates)
    {
        uint32 const groupId = pair.first;
        bool const doSpawn = (pair.second == SPAWN);
        if (instance->IsSpawnGroupActive(groupId) == doSpawn)
            continue; // nothing to do here
        // if we should spawn group, then spawn it...
        if (doSpawn)
            instance->SpawnGroupSpawn(groupId);
        else // otherwise, set it as inactive so it no longer respawns (but don't despawn it)
            instance->SetSpawnGroupInactive(groupId);
    }
}

BossInfo* InstanceScript::GetBossInfo(uint32 id)
{
    ASSERT(id < bosses.size());
    return &bosses[id];
}

void InstanceScript::AddObject(Creature* obj, bool add)
{
    ObjectInfoMap::const_iterator j = _creatureInfo.find(obj->GetEntry());
    if (j != _creatureInfo.end())
        AddObject(obj, j->second, add);
}

void InstanceScript::AddObject(GameObject* obj, bool add)
{
    ObjectInfoMap::const_iterator j = _gameObjectInfo.find(obj->GetEntry());
    if (j != _gameObjectInfo.end())
        AddObject(obj, j->second, add);
}

void InstanceScript::AddObject(WorldObject* obj, uint32 type, bool add)
{
    if (add)
        _objectGuids[type] = obj->GetGUID();
    else
    {
        ObjectGuidMap::iterator i = _objectGuids.find(type);
        if (i != _objectGuids.end() && i->second == obj->GetGUID())
            _objectGuids.erase(i);
    }
}

void InstanceScript::AddDoor(GameObject* door, bool add)
{
    DoorInfoMapBounds range = doors.equal_range(door->GetEntry());
    if (range.first == range.second)
        return;

    for (; range.first != range.second; ++range.first)
    {
        DoorInfo const& data = range.first->second;

        if (add)
        {
            data.bossInfo->door[data.type].insert(door->GetGUID());
        }
        else
            data.bossInfo->door[data.type].erase(door->GetGUID());
    }

    if (add)
        UpdateDoorState(door);
}

void InstanceScript::AddMinion(Creature* minion, bool add)
{
    MinionInfoMap::iterator itr = minions.find(minion->GetEntry());
    if (itr == minions.end())
        return;

    if (add)
        itr->second.bossInfo->minion.insert(minion->GetGUID());
    else
        itr->second.bossInfo->minion.erase(minion->GetGUID());
}

bool InstanceScript::SetBossState(uint32 id, EncounterState state)
{
    if (id < bosses.size())
    {
        BossInfo* bossInfo = &bosses[id];
        if (bossInfo->state == TO_BE_DECIDED) // loading
        {
            bossInfo->state = state;
            TC_LOG_DEBUG("scripts", "InstanceScript: Initialize boss {} state as {} (map {}, {}).", id, GetBossStateName(state), instance->GetId(), instance->GetInstanceId());
            return false;
        }
        else
        {
            if (bossInfo->state == state)
                return false;

            if (bossInfo->state == DONE)
            {
                TC_LOG_ERROR("map", "InstanceScript: Tried to set instance boss {} state from {} back to {} for map {}, instance id {}. Blocked!", id, GetBossStateName(bossInfo->state), GetBossStateName(state), instance->GetId(), instance->GetInstanceId());
                return false;
            }

            if (state == DONE)
                for (GuidSet::iterator i = bossInfo->minion.begin(); i != bossInfo->minion.end(); ++i)
                    if (Creature* minion = instance->GetCreature(*i))
                        if (minion->isWorldBoss() && minion->IsAlive())
                            return false;

            DungeonEncounterEntry const* dungeonEncounter = nullptr;
            switch (state)
            {
            case IN_PROGRESS:
            {
                uint32 resInterval = GetCombatResurrectionChargeInterval();
                InitializeCombatResurrections(1, resInterval);
                SendEncounterStart(1, 9, resInterval, resInterval);

                instance->DoOnPlayers([](Player* player)
                    {
                        player->AtStartOfEncounter();
                    });
                break;
            }
            case FAIL:
            {
                ResetCombatResurrections();
                SendEncounterEnd();

                instance->DoOnPlayers([](Player* player)
                    {
                        player->AtEndOfEncounter();
                    });
                break;
            }
            case DONE:
            {
                ResetCombatResurrections();
                SendEncounterEnd();
                dungeonEncounter = bossInfo->GetDungeonEncounterForDifficulty(instance->GetDifficultyID());
                if (dungeonEncounter)
                {
                    DoUpdateCriteria(CriteriaType::DefeatDungeonEncounter, dungeonEncounter->ID);
                    SendBossKillCredit(dungeonEncounter->ID);
                }
                instance->DoOnPlayers([](Player* player)
                    {
                        player->AtEndOfEncounter();
                    });
                break;
            }
            default:
                break;
            }

            bossInfo->state = state;
            if (dungeonEncounter)
                instance->UpdateInstanceLock({ dungeonEncounter, id, state });
        }

        for (uint32 type = 0; type < MAX_DOOR_TYPES; ++type)
            for (GuidSet::iterator i = bossInfo->door[type].begin(); i != bossInfo->door[type].end(); ++i)
                if (GameObject* door = instance->GetGameObject(*i))
                    UpdateDoorState(door);

        GuidSet minions = bossInfo->minion; // Copy to prevent iterator invalidation (minion might be unsummoned in UpdateMinionState)
        for (GuidSet::iterator i = minions.begin(); i != minions.end(); ++i)
            if (Creature* minion = instance->GetCreature(*i))
                UpdateMinionState(minion, state);

        UpdateSpawnGroups();
        return true;
    }
    return false;
}

bool InstanceScript::_SkipCheckRequiredBosses(Player const* player /*= nullptr*/) const
{
    return player && player->GetSession()->HasPermission(rbac::RBAC_PERM_SKIP_CHECK_INSTANCE_REQUIRED_BOSSES);
}

void InstanceScript::Create()
{
    for (size_t i = 0; i < bosses.size(); ++i)
        SetBossState(i, NOT_STARTED);
    UpdateSpawnGroups();
}

void InstanceScript::Load(char const* data)
{
    if (!data)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(data);

    InstanceScriptDataReader reader(*this);
    if (reader.Load(data) == InstanceScriptDataReader::Result::Ok)
    {
        // in loot-based lockouts instance can be loaded with later boss marked as killed without preceding bosses
        // but we still need to have them alive
        for (uint32 i = 0; i < bosses.size(); ++i)
            if (bosses[i].state == DONE && !CheckRequiredBosses(i))
                bosses[i].state = NOT_STARTED;

        UpdateSpawnGroups();
        AfterDataLoad();
    }
    else
        OUT_LOAD_INST_DATA_FAIL;

    OUT_LOAD_INST_DATA_COMPLETE;
}

std::string InstanceScript::GetSaveData()
{
    OUT_SAVE_INST_DATA;

    InstanceScriptDataWriter writer(*this);

    writer.FillData();

    OUT_SAVE_INST_DATA_COMPLETE;

    return writer.GetString();
}

std::string InstanceScript::UpdateBossStateSaveData(std::string const& oldData, UpdateBossStateSaveDataEvent const& event)
{
    if (!instance->GetMapDifficulty()->IsUsingEncounterLocks())
        return GetSaveData();

    InstanceScriptDataWriter writer(*this);
    writer.FillDataFrom(oldData);
    writer.SetBossState(event);
    return writer.GetString();
}

std::string InstanceScript::UpdateAdditionalSaveData(std::string const& oldData, UpdateAdditionalSaveDataEvent const& event)
{
    if (!instance->GetMapDifficulty()->IsUsingEncounterLocks())
        return GetSaveData();

    InstanceScriptDataWriter writer(*this);
    writer.FillDataFrom(oldData);
    writer.SetAdditionalData(event);
    return writer.GetString();
}

Optional<uint32> InstanceScript::GetEntranceLocationForCompletedEncounters(uint32 completedEncountersMask) const
{
    if (!instance->GetMapDifficulty()->IsUsingEncounterLocks())
        return _entranceId;

    return ComputeEntranceLocationForCompletedEncounters(completedEncountersMask);
}

Optional<uint32> InstanceScript::ComputeEntranceLocationForCompletedEncounters(uint32 /*completedEncountersMask*/) const
{
    return { };
}

void InstanceScript::HandleGameObject(ObjectGuid guid, bool open, GameObject* go /*= nullptr*/)
{
    if (!go)
        go = instance->GetGameObject(guid);
    if (go)
        go->SetGoState(open ? GO_STATE_ACTIVE : GO_STATE_READY);
    else
        TC_LOG_DEBUG("scripts", "InstanceScript: HandleGameObject failed");
}

void InstanceScript::DoUseDoorOrButton(ObjectGuid guid, uint32 withRestoreTime /*= 0*/, bool useAlternativeState /*= false*/)
{
    if (!guid)
        return;

    if (GameObject* go = instance->GetGameObject(guid))
    {
        if (go->GetGoType() == GAMEOBJECT_TYPE_DOOR || go->GetGoType() == GAMEOBJECT_TYPE_BUTTON)
        {
            if (go->getLootState() == GO_READY)
                go->UseDoorOrButton(withRestoreTime, useAlternativeState);
            else if (go->getLootState() == GO_ACTIVATED)
                go->ResetDoorOrButton();
        }
        else
            TC_LOG_ERROR("scripts", "InstanceScript: DoUseDoorOrButton can't use gameobject entry {}, because type is {}.", go->GetEntry(), go->GetGoType());
    }
    else
        TC_LOG_DEBUG("scripts", "InstanceScript: DoUseDoorOrButton failed");
}

void InstanceScript::DoCloseDoorOrButton(ObjectGuid guid)
{
    if (!guid)
        return;

    if (GameObject* go = instance->GetGameObject(guid))
    {
        if (go->GetGoType() == GAMEOBJECT_TYPE_DOOR || go->GetGoType() == GAMEOBJECT_TYPE_BUTTON)
        {
            if (go->getLootState() == GO_ACTIVATED)
                go->ResetDoorOrButton();
        }
        else
            TC_LOG_ERROR("scripts", "InstanceScript: DoCloseDoorOrButton can't use gameobject entry {}, because type is {}.", go->GetEntry(), go->GetGoType());
    }
    else
        TC_LOG_DEBUG("scripts", "InstanceScript: DoCloseDoorOrButton failed");
}

void InstanceScript::DoRespawnGameObject(ObjectGuid guid, Seconds timeToDespawn /*= 1min */)
{
    if (GameObject* go = instance->GetGameObject(guid))
    {
        switch (go->GetGoType())
        {
        case GAMEOBJECT_TYPE_DOOR:
        case GAMEOBJECT_TYPE_BUTTON:
        case GAMEOBJECT_TYPE_TRAP:
        case GAMEOBJECT_TYPE_FISHINGNODE:
            // not expect any of these should ever be handled
            TC_LOG_ERROR("scripts", "InstanceScript: DoRespawnGameObject can't respawn gameobject entry {}, because type is {}.", go->GetEntry(), go->GetGoType());
            return;
        default:
            break;
        }

        if (go->isSpawned())
            return;

        go->SetRespawnTime(timeToDespawn.count());
    }
    else
        TC_LOG_DEBUG("scripts", "InstanceScript: DoRespawnGameObject failed");
}

void InstanceScript::DoUpdateWorldState(int32 worldStateId, int32 value)
{
    sWorldStateMgr->SetValue(worldStateId, value, false, instance);
}

// Send Notify to all players in instance
void InstanceScript::DoSendNotifyToInstance(char const* format, ...)
{
    va_list ap;
    va_start(ap, format);
    char buff[1024];
    vsnprintf(buff, 1024, format, ap);
    va_end(ap);

    instance->DoOnPlayers([&buff](Player const* player)
        {
            player->GetSession()->SendNotification("%s", buff);
        });
}

// Update Achievement Criteria for all players in instance
void InstanceScript::DoUpdateCriteria(CriteriaType type, uint32 miscValue1 /*= 0*/, uint32 miscValue2 /*= 0*/, Unit* unit /*= nullptr*/)
{
    instance->DoOnPlayers([type, miscValue1, miscValue2, unit](Player* player)
        {
            player->UpdateCriteria(type, miscValue1, miscValue2, 0, unit);
        });
}

void InstanceScript::DoRemoveAurasDueToSpellOnPlayers(uint32 spell, bool includePets /*= false*/, bool includeControlled /*= false*/)
{
    instance->DoOnPlayers([this, spell, includePets, includeControlled](Player* player)
        {
            DoRemoveAurasDueToSpellOnPlayer(player, spell, includePets, includeControlled);
        });
}

void InstanceScript::DoRemoveAurasDueToSpellOnPlayer(Player* player, uint32 spell, bool includePets /*= false*/, bool includeControlled /*= false*/)
{
    if (!player)
        return;

    player->RemoveAurasDueToSpell(spell);

    if (!includePets)
        return;

    for (uint8 itr2 = 0; itr2 < MAX_SUMMON_SLOT; ++itr2)
    {
        ObjectGuid summonGUID = player->m_SummonSlot[itr2];
        if (!summonGUID.IsEmpty())
            if (Creature* summon = instance->GetCreature(summonGUID))
                summon->RemoveAurasDueToSpell(spell);
    }

    if (!includeControlled)
        return;

    for (auto itr2 = player->m_Controlled.begin(); itr2 != player->m_Controlled.end(); ++itr2)
    {
        if (Unit* controlled = *itr2)
            if (controlled->IsInWorld() && controlled->GetTypeId() == TYPEID_UNIT)
                controlled->RemoveAurasDueToSpell(spell);
    }
}

void InstanceScript::DoCastSpellOnPlayers(uint32 spell, bool includePets /*= false*/, bool includeControlled /*= false*/)
{
    instance->DoOnPlayers([this, spell, includePets, includeControlled](Player* player)
        {
            DoCastSpellOnPlayer(player, spell, includePets, includeControlled);
        });
}

void InstanceScript::DoCastSpellOnPlayer(Player* player, uint32 spell, bool includePets /*= false*/, bool includeControlled /*= false*/)
{
    if (!player)
        return;

    player->CastSpell(player, spell, true);

    if (!includePets)
        return;

    for (uint8 itr2 = 0; itr2 < MAX_SUMMON_SLOT; ++itr2)
    {
        ObjectGuid summonGUID = player->m_SummonSlot[itr2];
        if (!summonGUID.IsEmpty())
            if (Creature* summon = instance->GetCreature(summonGUID))
                summon->CastSpell(player, spell, true);
    }

    if (!includeControlled)
        return;

    for (auto itr2 = player->m_Controlled.begin(); itr2 != player->m_Controlled.end(); ++itr2)
    {
        if (Unit* controlled = *itr2)
            if (controlled->IsInWorld() && controlled->GetTypeId() == TYPEID_UNIT)
                controlled->CastSpell(player, spell, true);
    }
}

bool InstanceScript::ServerAllowsTwoSideGroups()
{
    return sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_GROUP);
}

DungeonEncounterEntry const* InstanceScript::GetBossDungeonEncounter(uint32 id) const
{
    return id < bosses.size() ? bosses[id].GetDungeonEncounterForDifficulty(instance->GetDifficultyID()) : nullptr;
}

DungeonEncounterEntry const* InstanceScript::GetBossDungeonEncounter(Creature const* creature) const
{
    if (BossAI const* bossAi = dynamic_cast<BossAI const*>(creature->GetAI()))
        return GetBossDungeonEncounter(bossAi->GetBossId());

    return nullptr;
}

bool InstanceScript::CheckAchievementCriteriaMeet(uint32 criteria_id, Player const* /*source*/, Unit const* /*target*/ /*= nullptr*/, uint32 /*miscvalue1*/ /*= 0*/)
{
    TC_LOG_ERROR("misc", "Achievement system call InstanceScript::CheckAchievementCriteriaMeet but instance script for map {} not have implementation for achievement criteria {}",
        instance->GetId(), criteria_id);
    return false;
}

bool InstanceScript::IsEncounterCompleted(uint32 dungeonEncounterId) const
{
    for (std::size_t i = 0; i < bosses.size(); ++i)
        for (std::size_t j = 0; j < bosses[i].DungeonEncounters.size(); ++j)
            if (bosses[i].DungeonEncounters[j] && bosses[i].DungeonEncounters[j]->ID == dungeonEncounterId)
                return bosses[i].state == DONE;

    return false;
}

bool InstanceScript::IsEncounterCompletedInMaskByBossId(uint32 completedEncountersMask, uint32 bossId) const
{
    if (DungeonEncounterEntry const* dungeonEncounter = GetBossDungeonEncounter(bossId))
        if (completedEncountersMask & (1 << dungeonEncounter->Bit))
            return bosses[bossId].state == DONE;

    return false;
}

void InstanceScript::SetEntranceLocation(uint32 worldSafeLocationId)
{
    _entranceId = worldSafeLocationId;
    _temporaryEntranceId = 0;
}

void InstanceScript::SendEncounterUnit(uint32 type, Unit* unit /*= nullptr*/, uint8 priority)
{
    switch (type)
    {
    case ENCOUNTER_FRAME_ENGAGE:                    // SMSG_INSTANCE_ENCOUNTER_ENGAGE_UNIT
    {
        if (!unit)
            return;

        WorldPackets::Instance::InstanceEncounterEngageUnit encounterEngageMessage;
        encounterEngageMessage.Unit = unit->GetGUID();
        encounterEngageMessage.TargetFramePriority = priority;
        instance->SendToPlayers(encounterEngageMessage.Write());
        break;
    }
    case ENCOUNTER_FRAME_DISENGAGE:                 // SMSG_INSTANCE_ENCOUNTER_DISENGAGE_UNIT
    {
        if (!unit)
            return;

        WorldPackets::Instance::InstanceEncounterDisengageUnit encounterDisengageMessage;
        encounterDisengageMessage.Unit = unit->GetGUID();
        instance->SendToPlayers(encounterDisengageMessage.Write());
        break;
    }
    case ENCOUNTER_FRAME_UPDATE_PRIORITY:           // SMSG_INSTANCE_ENCOUNTER_CHANGE_PRIORITY
    {
        if (!unit)
            return;

        WorldPackets::Instance::InstanceEncounterChangePriority encounterChangePriorityMessage;
        encounterChangePriorityMessage.Unit = unit->GetGUID();
        encounterChangePriorityMessage.TargetFramePriority = priority;
        instance->SendToPlayers(encounterChangePriorityMessage.Write());
        break;
    }
    default:
        break;
    }
}

void InstanceScript::SendEncounterStart(uint32 inCombatResCount /*= 0*/, uint32 maxInCombatResCount /*= 0*/, uint32 inCombatResChargeRecovery /*= 0*/, uint32 nextCombatResChargeTime /*= 0*/)
{
    WorldPackets::Instance::InstanceEncounterStart encounterStartMessage;
    encounterStartMessage.InCombatResCount = inCombatResCount;
    encounterStartMessage.MaxInCombatResCount = maxInCombatResCount;
    encounterStartMessage.CombatResChargeRecovery = inCombatResChargeRecovery;
    encounterStartMessage.NextCombatResChargeTime = nextCombatResChargeTime;

    instance->SendToPlayers(encounterStartMessage.Write());
}

void InstanceScript::SendEncounterEnd()
{
    WorldPackets::Instance::InstanceEncounterEnd encounterEndMessage;
    instance->SendToPlayers(encounterEndMessage.Write());
}

void InstanceScript::SendBossKillCredit(uint32 encounterId)
{
    WorldPackets::Instance::BossKill bossKillCreditMessage;
    bossKillCreditMessage.DungeonEncounterID = encounterId;

    instance->SendToPlayers(bossKillCreditMessage.Write());
}

void InstanceScript::UpdateEncounterState(EncounterCreditType type, uint32 creditEntry, Unit* /*source*/)
{
    DungeonEncounterList const* encounters = sObjectMgr->GetDungeonEncounterList(instance->GetId(), instance->GetDifficultyID());
    if (!encounters)
        return;

    uint32 dungeonId = 0;

    for (auto const& encounter : *encounters)
    {
        if (encounter.creditType == type && encounter.creditEntry == creditEntry)
        {
            completedEncounters |= 1 << encounter.dbcEntry->Bit;
            if (encounter.dbcEntry->CompleteWorldStateID)
                DoUpdateWorldState(encounter.dbcEntry->CompleteWorldStateID, 1);

            if (encounter.lastEncounterDungeon)
            {
                dungeonId = encounter.lastEncounterDungeon;
                TC_LOG_DEBUG("lfg", "UpdateEncounterState: Instance {} (instanceId {}) completed encounter {}. Credit Dungeon: {}",
                    instance->GetMapName(), instance->GetInstanceId(), encounter.dbcEntry->Name[sWorld->GetDefaultDbcLocale()], dungeonId);
                break;
            }
        }
    }

    if (dungeonId)
    {
        Map::PlayerList const& players = instance->GetPlayers();
        for (auto const& ref : players)
        {
            if (Player* player = ref.GetSource())
            {
                if (Group* grp = player->GetGroup())
                {
                    if (grp->isLFGGroup())
                    {
                        sLFGMgr->FinishDungeon(grp->GetGUID(), dungeonId, instance);
                        return;
                    }
                }
            }
        }
    }
}

void InstanceScript::UpdateEncounterStateForKilledCreature(uint32 creatureId, Unit* source)
{
    UpdateEncounterState(ENCOUNTER_CREDIT_KILL_CREATURE, creatureId, source);
}

void InstanceScript::UpdateEncounterStateForSpellCast(uint32 spellId, Unit* source)
{
    UpdateEncounterState(ENCOUNTER_CREDIT_CAST_SPELL, spellId, source);
}

void InstanceScript::SetCompletedEncountersMask(uint32 newMask)
{
    completedEncounters = newMask;

    if (DungeonEncounterList const* encounters = sObjectMgr->GetDungeonEncounterList(instance->GetId(), instance->GetDifficultyID()))
        for (DungeonEncounter const& encounter : *encounters)
            if (completedEncounters & (1 << encounter.dbcEntry->Bit) && encounter.dbcEntry->CompleteWorldStateID)
                DoUpdateWorldState(encounter.dbcEntry->CompleteWorldStateID, 1);
}

void InstanceScript::UpdatePhasing()
{
    instance->DoOnPlayers([](Player const* player)
        {
            PhasingHandler::SendToPlayer(player);
        });
}

char const* InstanceScript::GetBossStateName(uint8 state)
{
    return EnumUtils::ToConstant(EncounterState(state));
}

void InstanceScript::UpdateCombatResurrection(uint32 diff)
{
    if (!_combatResurrectionTimerStarted)
        return;

    if (_combatResurrectionTimer <= diff)
        AddCombatResurrectionCharge();
    else
        _combatResurrectionTimer -= diff;
}

void InstanceScript::InitializeCombatResurrections(uint8 charges /*= 1*/, uint32 interval /*= 0*/)
{
    _combatResurrectionCharges = charges;
    if (!interval)
        return;

    _combatResurrectionTimer = interval;
    _combatResurrectionTimerStarted = true;
}

void InstanceScript::AddCombatResurrectionCharge()
{
    ++_combatResurrectionCharges;
    _combatResurrectionTimer = GetCombatResurrectionChargeInterval();

    WorldPackets::Instance::InstanceEncounterGainCombatResurrectionCharge gainCombatResurrectionCharge;
    gainCombatResurrectionCharge.InCombatResCount = _combatResurrectionCharges;
    gainCombatResurrectionCharge.CombatResChargeRecovery = _combatResurrectionTimer;
    instance->SendToPlayers(gainCombatResurrectionCharge.Write());
}

void InstanceScript::UseCombatResurrection()
{
    --_combatResurrectionCharges;

    instance->SendToPlayers(WorldPackets::Instance::InstanceEncounterInCombatResurrection().Write());
}

void InstanceScript::ResetCombatResurrections()
{
    _combatResurrectionCharges = 0;
    _combatResurrectionTimer = 0;
    _combatResurrectionTimerStarted = false;
}

uint32 InstanceScript::GetCombatResurrectionChargeInterval() const
{
    uint32 interval = 0;
    if (uint32 playerCount = instance->GetPlayers().getSize())
        interval = 90 * MINUTE * IN_MILLISECONDS / playerCount;

    return interval;
}

PersistentInstanceScriptValueBase::PersistentInstanceScriptValueBase(InstanceScript& instance, char const* name, std::variant<int64, double> value)
    : _instance(instance), _name(name), _value(std::move(value))
{
    _instance.RegisterPersistentScriptValue(this);
}

PersistentInstanceScriptValueBase::~PersistentInstanceScriptValueBase() = default;

void PersistentInstanceScriptValueBase::NotifyValueChanged()
{
    _instance.instance->UpdateInstanceLock(CreateEvent());
}

bool InstanceHasScript(WorldObject const* obj, char const* scriptName)
{
    if (InstanceMap* instance = obj->GetMap()->ToInstanceMap())
        return instance->GetScriptName() == scriptName;

    return false;
}

void InstanceScript::DoNearTeleportPlayers(const Position pos, bool casting /*=false*/)
{
    DoOnPlayers([pos, casting](Player* player)
        {
            player->NearTeleportTo(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), pos.GetOrientation(), casting);
        });
}

void InstanceScript::DoPlayConversation(uint32 conversationId)
{
    DoOnPlayers([conversationId](Player* player)
    {
        player->PlayConversation(conversationId);
    });
}

// DekkCore >
void InstanceScript::DoOnPlayers(std::function<void(Player*)>&& function)
{
    Map::PlayerList const& plrList = instance->GetPlayers();

    if (!plrList.isEmpty())
        for (Map::PlayerList::const_iterator i = plrList.begin(); i != plrList.end(); ++i)
            if (Player* player = i->GetSource())
                function(player);
}

// Add aura on all players in instance
void InstanceScript::DoAddAuraOnPlayers(uint32 spell)
{
    DoOnPlayers([spell](Player* player)
        {
            player->AddAura(spell, player);
        });
}
//dekkcore
// Cast spell on all players in instance
void InstanceScript::DoPlayScenePackageIdOnPlayers(uint32 scenePackageId)
{
    DoOnPlayers([scenePackageId](Player* player)
        {
            player->GetSceneMgr().PlaySceneByPackageId(scenePackageId, SceneFlag::None);
        });
}

void InstanceScript::DoStartMovie(uint32 movieId)
{
    MovieEntry const* movieEntry = sMovieStore.LookupEntry(movieId);
    if (!movieEntry)
    {
        TC_LOG_ERROR("scripts", "DoStartMovie called for not existing movieId {}", movieId);
        return;
    }

    DoOnPlayers([movieId](Player* player)
        {
            player->SendMovieStart(movieId);
        });
}
//dekkcore
void InstanceScript::DoCompleteAchievement(uint32 achievement)
{
    AchievementEntry const* achievementEntry = sAchievementStore.LookupEntry(achievement);
    if (!achievementEntry)
    {
        TC_LOG_ERROR("scripts", "DoCompleteAchievement called for not existing achievement {}", achievement);
        return;
    }

    DoOnPlayers([achievementEntry](Player* player)
        {
            player->CompletedAchievement(achievementEntry);
        });
}

void InstanceScript::SetChallenge(Challenge* challenge)
{
    _challenge = challenge;

    _inCombatResCount = 1;
    _maxInCombatResCount = 5;
    _combatResChargeTime = 10 * MINUTE * IN_MILLISECONDS;
    _nextCombatResChargeTime = 10 * MINUTE * IN_MILLISECONDS;
}

void InstanceScript::DoSendScenarioEvent(uint32 eventId)
{
    Map::PlayerList const& playerList = instance->GetPlayers();
    if (!playerList.isEmpty())
        for (Map::PlayerList::const_iterator i = playerList.begin(); i != playerList.end(); ++i)
            if (Player* player = i->GetSource())
            {
                player->GetScenario()->SendScenarioEvent(player, eventId);
                return;
            }
}

void InstanceScript::GetScenarioByID(Player* p_Player, uint32 p_ScenarioId)
{
    InstanceMap* map = instance->ToInstanceMap();

    if (InstanceScenario* instanceScenario = sScenarioMgr->CreateInstanceScenarioByID(map, p_ScenarioId))
    {
        TC_LOG_ERROR("scripts", "GetScenarioByID CreateInstanceScenario {}", p_ScenarioId, "");
        map->SetInstanceScenario(instanceScenario);
    }
    else
        TC_LOG_DEBUG("scripts", "InstanceScript: GetScenarioByID failed");
}

void InstanceScript::DoTeleportPlayers(uint32 mapId, const Position pos)
{
    DoOnPlayers([mapId, pos](Player* player)
        {
            player->TeleportTo(mapId, pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), pos.GetOrientation());
        });
}

void InstanceScript::CastIslandAzeriteAura()
{
    DoCastSpellOnPlayers(SPELL_AZERITE_RESIDUE);
    //TO DO cast on pve NPC
}

void InstanceScript::AddChallengeModeChests(ObjectGuid chestGuid, uint8 chestLevel)
{
    _challengeChestGuids[chestLevel] = chestGuid;
}

ObjectGuid InstanceScript::GetChellngeModeChests(uint8 chestLevel)
{
    return _challengeChestGuids[chestLevel];
}

void InstanceScript::AddChallengeModeDoor(ObjectGuid doorGuid)
{
    _challengeDoorGuids.push_back(doorGuid);
}

void InstanceScript::AddChallengeModeOrb(ObjectGuid orbGuid)
{
    _challengeOrbGuid = orbGuid;
}

class ChallengeModeWorker
{
public:
    ChallengeModeWorker(InstanceScript* instance) : _instance(instance) { }

    void Visit(std::unordered_map<ObjectGuid, Creature*>& creatureMap)
    {
        for (auto const& p : creatureMap)
        {
            if (p.second->IsInWorld() && !p.second->IsPet())
            {
                if (!p.second->IsAlive())
                    p.second->Respawn();

                _instance->CastChallengeCreatureSpell(p.second);
            }
        }
    }

    template<class T>
    void Visit(std::unordered_map<ObjectGuid, T*>&) { }

private:
    InstanceScript* _instance;
};

void InstanceScript::StartChallengeMode(uint8 modeid, uint8 level, uint8 affix1, uint8 affix2, uint8 affix3, uint8 affix4)
{
    _challengeModeId = modeid;
    MapChallengeModeEntry const* mapChallengeModeEntry = sChallengeModeMgr->GetMapChallengeModeEntryByModeId(GetChallengeModeId());
    if (!mapChallengeModeEntry)
        return;

    if (IsChallengeModeStarted())
        return;

    if (GetCompletedEncounterMask() != 0)
        return;

    if (instance->GetDifficultyID() != DIFFICULTY_MYTHIC)
        return;

    _affixes[0] = affix1;
    _affixes[1] = affix2;
    _affixes[2] = affix3;
    _affixes[3] = affix4;

    for (auto const& affix : _affixes)
        _affixesTest.set(affix);
    _challengeModeStarted = true;
    _challengeModeLevel = level;


    instance->SendToPlayers(WorldPackets::Misc::ChangePlayerDifficultyResult(5).Write());

    // Add the health/dmg modifier aura to all creatures
    ChallengeModeWorker worker(this);
    TypeContainerVisitor<ChallengeModeWorker, MapStoredObjectTypesContainer> visitor(worker);
    visitor.Visit(instance->GetObjectsStore());

    // Add Karazhan Start Position
    if (modeid == 234)
        SetChallengeStartPos({ -11063.781f, -2018.33f, 115.1926f, 0.43196f });
    else if (modeid == 227)
        SetChallengeStartPos({ -11056.8759f, -1977.4697f, 101.1527f, 0.805f });

    // Tp back all players to begin
    Position entranceLocation;
    if (_challengeModeStartPosition.has_value())
        entranceLocation.Relocate(_challengeModeStartPosition->GetPosition());
    else if (WorldSafeLocsEntry const* entranceSafeLocEntry = sObjectMgr->GetWorldSafeLoc(GetEntranceLocation()))
        entranceLocation.Relocate(entranceSafeLocEntry->Loc);

    // else if (AreaTriggerTeleportStruct const* areaTrigger = sObjectMgr->GetMapEntranceTrigger(instance->GetId()))
     //    entranceLocation.Relocate(areaTrigger->target_X, areaTrigger->target_Y, areaTrigger->target_Z, areaTrigger->target_Orientation);
    // DoNearTeleportPlayers(entranceLocation);

    if (_challengeModeDoorPosition.has_value())
        instance->SummonGameObject(GOB_CHALLENGER_DOOR, *_challengeModeDoorPosition, QuaternionData(), WEEK);

    ShowChallengeDoor();
    AfterChallengeModeStarted();

    WorldPackets::Misc::ChangePlayerDifficultyResult changePlayerDifficultyResult(11);
    changePlayerDifficultyResult.InstanceDifficultyID = instance->GetId();
    changePlayerDifficultyResult.DifficultyRecID = DIFFICULTY_MYTHIC_KEYSTONE;
    instance->SendToPlayers(changePlayerDifficultyResult.Write());

   // instance->SendToPlayers(WorldPackets::Misc::ResetChallengeMode(instance->GetId()).Write()); CRASH HERE 

    WorldPackets::Misc::StartTimer startTimer;
    startTimer.Type = WorldPackets::Misc::StartTimer::ChallengeMode;
    startTimer.TotalTime = (Seconds(10));
    startTimer.TimeLeft = (Seconds(10));
    instance->SendToPlayers(startTimer.Write());

    SendChallengeModeStart();

    DoOnPlayers([this, level](Player* player)
        {
            CastChallengePlayerSpell(player);
            // HOOK to PLAYERSCRIPT
            sScriptMgr->OnPlayerStartChallengeMode(player, level, _affixes[0], _affixes[1], _affixes[2], _affixes[3]);
        });

    AddTimedDelayedOperation(10000, [this]()
        {
            _challengeModeStartTime = getMSTime();

            SendChallengeModeElapsedTimer();

            if (GameObject* door = GetGameObject(GOB_CHALLENGER_DOOR))
                DoUseDoorOrButton(door->GetGUID(), WEEK);

            HideChallengeDoor();
        });
}

void InstanceScript::AfterChallengeModeStarted()
{
    if (_challengeModeScenario.has_value())
    {
        uint32 scenarioId = *_challengeModeScenario;
        DoOnPlayers([this, scenarioId](Player* player)
            {
                GetScenarioByID(player, scenarioId);
            });
    }
}

void InstanceScript::SpawnFontOfPower()
{
    if (_challengeModeFontOfPowerPosition.has_value() && instance->IsMythic())
        instance->SummonGameObject(GO_FONT_OF_POWER, *_challengeModeFontOfPowerPosition, QuaternionData(), WEEK);
    if (_challengeModeFontOfPowerPosition2.has_value() && instance->IsMythic())
        instance->SummonGameObject(GO_FONT_OF_POWER, *_challengeModeFontOfPowerPosition2, QuaternionData(), WEEK);
}

void InstanceScript::CastChallengeCreatureSpellOnDeath(Creature* creature)
{
    if (!creature || creature->IsAffixDisabled() || creature->IsTrigger() || creature->IsControlledByPlayer() || !creature->IsHostileToPlayers() || creature->GetCreatureType() == CREATURE_TYPE_CRITTER)
        return;

    if (creature->IsOnVehicle(creature))
        return;

    Unit* owner = creature->GetOwner();
    if (owner && owner->IsPlayer())
        return;

    // 7 Bolstering
    if (!creature->IsDungeonBoss() && HasAffix(Affixes::Bolstering))
        creature->CastSpell(creature, ChallengerBolstering, true);
    // 8 Sanguine
    if (!creature->IsDungeonBoss() && HasAffix(Affixes::Sanguine))
        creature->CastSpell(creature, ChallengerSanguine, true);
    // 11 Bursting 243237
    if (!creature->IsDungeonBoss() && HasAffix(Affixes::Bursting))
        creature->CastSpell(creature, ChallengerBursting, true);
}

void InstanceScript::CastChallengePlayerSpell(Player* player)
{
    CastSpellExtraArgs values;

    // Affixes
    values.AddSpellMod(SPELLVALUE_BASE_POINT1, HasAffix(Affixes::Overflowing) ? 1 : 0);// 1 Overflowing
    values.AddSpellMod(SPELLVALUE_BASE_POINT2, (HasAffix(Affixes::Skittish) && player->IsTankPlayer()) ? 1 : 0); // 2 Skittish
    values.AddSpellMod(SPELLVALUE_BASE_POINT3, HasAffix(Affixes::Grievous) ? 1 : 0);  // 12 Grievous

   player->CastSpell(player, SPELL_CHALLENGER_BURDEN, values);
}

void InstanceScript::CastChallengeCreatureSpell(Creature* creature)
{
    if (!creature || creature->IsTrigger() || creature->IsControlledByPlayer() || creature->GetCreatureType() == CREATURE_TYPE_CRITTER)
        return;

    Unit* owner = creature->GetOwner();
    if (owner && owner->IsPlayer())
        return;

    float modHealth = sChallengeModeMgr->GetHealthMultiplier(_challengeModeLevel);
    float modDamage = sChallengeModeMgr->GetDamageMultiplier(_challengeModeLevel);

    bool isDungeonBoss = false;
    if (creature->IsDungeonBoss())
        isDungeonBoss = true;

    if (isDungeonBoss)
    {
        // 9 Tyrannical 
        if (HasAffix(Affixes::Tyrannical))
        {
            modHealth *= 1.4f;
            modDamage *= 1.15f;
        }
    }//10 Fortified 
    else if (HasAffix(Affixes::Fortified))
    {
        modHealth *= 1.2f;
        modDamage *= 1.3f;
    }

    CastSpellExtraArgs values;
    values.AddSpellMod(SPELLVALUE_BASE_POINT0, modHealth);
    values.AddSpellMod(SPELLVALUE_BASE_POINT1, modDamage);

    // Affixes
    values.AddSpellMod(SPELLVALUE_BASE_POINT2, (HasAffix(Affixes::Raging) && !isDungeonBoss) ? 1 : 0); // 6 Raging 
    values.AddSpellMod(SPELLVALUE_BASE_POINT3, HasAffix(Affixes::Bolstering) ? 1 : 0); // 7 Bolstering 
    values.AddSpellMod(SPELLVALUE_BASE_POINT4, (HasAffix(Affixes::Tyrannical) && isDungeonBoss) ? 1 : 0); // 9 Tyrannical 
    values.AddSpellMod(SPELLVALUE_BASE_POINT5, 1); //
    values.AddSpellMod(SPELLVALUE_BASE_POINT6, 1); //
    values.AddSpellMod(SPELLVALUE_BASE_POINT7, HasAffix(Affixes::Volcanic) ? 1 : 0); // 3 Volcanic 
    values.AddSpellMod(SPELLVALUE_BASE_POINT8, HasAffix(Affixes::Necrotic) ? 1 : 0); // 4 Necrotic 
    values.AddSpellMod(SPELLVALUE_BASE_POINT9, (HasAffix(Affixes::Fortified) && !isDungeonBoss) ? 1 : 0); // 10 Fortified 
    values.AddSpellMod(SPELLVALUE_BASE_POINT10, HasAffix(Affixes::Sanguine) ? 1 : 0); // 8 Sanguine 
    values.AddSpellMod(SPELLVALUE_BASE_POINT11, HasAffix(Affixes::Quaking) ? 1 : 0); // 14 Quaking 
    values.AddSpellMod(SPELLVALUE_BASE_POINT12, HasAffix(Affixes::Explosive) ? 1 : 0); // 13 Explosive 
    values.AddSpellMod(SPELLVALUE_BASE_POINT13, HasAffix(Affixes::Bursting) ? 1 : 0); // 11 Bursting 
    //5 15
    //values.AddSpellMod(SPELLVALUE_BASE_POINT14, 0); //
    //values.AddSpellMod(SPELLVALUE_BASE_POINT15, 0); //

    creature->CastSpell(creature, SPELL_CHALLENGER_MIGHT, values);

    //5 Teeming 
    if (HasAffix(Affixes::Teeming) && !creature->IsDungeonBoss() && !creature->IsSummon() && !creature->IsAffixDisabled() && roll_chance_f(30.0f) && creature->GetSpawnId()) // Only for real creature summon copy
    {
        Position pos;
        pos = creature->GetNearPosition(6.0f, creature->GetOrientation());
        creature->SummonCreature(creature->GetEntry(), pos, TEMPSUMMON_DEAD_DESPAWN, 60s);
    }
    if (!creature->IsDungeonBoss() && HasAffix(Affixes::Relentless))//Relentless 
    {
        creature->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        creature->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK_DEST, true);
        creature->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
        creature->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
        creature->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
        creature->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
        creature->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FREEZE, true);
        creature->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
        creature->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
        creature->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
        creature->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
        creature->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
        creature->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_INTERRUPT, true);
        creature->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CONFUSE, true);
    }
}

void InstanceScript::SendChallengeModeNewPlayerRecord(Player* player)
{
    WorldPackets::Misc::NewPlayerRecord newRecord;
    newRecord.MapID = _mapID;
    newRecord.CompletionMilliseconds = _challengeTimer;
    newRecord.ChallengeLevel = _rewardLevel;

    if (player)
        player->SendDirectMessage(newRecord.Write());
}

void InstanceScript::SendChallengeModeElapsedTimer(Player* player/* = nullptr*/) const
{
    WorldPackets::Misc::StartTimer startElapsedTimer;
    startElapsedTimer.TotalTime = (Seconds(1));
    startElapsedTimer.TimeLeft = (Seconds(GetChallengeModeCurrentDuration()));

    if (player)
        player->SendDirectMessage(startElapsedTimer.Write());
    else
        instance->SendToPlayers(startElapsedTimer.Write());
}

void InstanceScript::SendChallengeModeDeathCount(Player* player/* = nullptr*/) const
{
    WorldPackets::Misc::UpdateDeathCount updateDeathCount;
    updateDeathCount.DeathCount = _challengeModeDeathCount;

    if (player)
        player->SendDirectMessage(updateDeathCount.Write());
    else
        instance->SendToPlayers(updateDeathCount.Write());
}

void InstanceScript::SendChallengeModeStart(Player* player/* = nullptr*/) const
{
    MapChallengeModeEntry const* mapChallengeModeEntry = sChallengeModeMgr->GetMapChallengeModeEntryByModeId(GetChallengeModeId());
    if (!mapChallengeModeEntry)
        return;

    WorldPackets::Misc::Start start;
    start.MapID = instance->GetId();
    start.ChallengeID = mapChallengeModeEntry->ID;
    start.ChallengeLevel = _challengeModeLevel;
    instance->SendToPlayers(start.Write());

    if (player)
        player->SendDirectMessage(start.Write());
    else
        instance->SendToPlayers(start.Write());
}

uint32 InstanceScript::GetChallengeModeCurrentDuration() const
{
    return uint32(GetMSTimeDiffToNow(_challengeModeStartTime) / 1000) + (5 * _challengeModeDeathCount);
}

std::array<uint32, 4> InstanceScript::GetAffixes() const
{
    return _affixes;
}

bool InstanceScript::HasAffix(Affixes affix)
{
    return _affixesTest.test(size_t(affix));
}

void InstanceScript::CompleteChallengeMode()
{
    MapChallengeModeEntry const* mapChallengeModeEntry = sChallengeModeMgr->GetMapChallengeModeEntryByModeId(GetChallengeModeId());
    if (!mapChallengeModeEntry)
        return;
    uint32 totalDuration = GetChallengeModeCurrentDuration();
    // Todo : Send stats
    uint8 mythicIncrement = 0;
    for (uint8 i = 0; i < 3; ++i)
        if (uint32(mapChallengeModeEntry->CriteriaCount[i]) > totalDuration)
            ++mythicIncrement;
    DoOnPlayers([this, mythicIncrement](Player* player)
        {
            player->AddChallengeKey(sChallengeModeMgr->GetRandomChallengeId(), std::max(_challengeModeLevel + mythicIncrement, 2));
        });

    WorldPackets::Misc::Complete complete;
    complete.CompletionMilliseconds = totalDuration;
    complete.MapId = instance->GetId();
    complete.ChallengeId = mapChallengeModeEntry->ID;
    complete.ChallengeLevel = _challengeModeLevel + mythicIncrement;
    instance->SendToPlayers(complete.Write());
    // Achievements only if timer respected
    if (mythicIncrement)
    {
        if (_challengeModeLevel >= 2)
            DoCompleteAchievement(11183);
        if (_challengeModeLevel >= 5)
            DoCompleteAchievement(11184);
        if (_challengeModeLevel >= 10)
            DoCompleteAchievement(11185);
        if (_challengeModeLevel >= 15)
        {
            DoCompleteAchievement(11162);
            DoCompleteAchievement(11224);
        }
    }
    SpawnChallengeModeRewardChest();

    if (GameObject* chest = instance->GetGameObject(_challengeChest))
        chest->SetRespawnTime(7 * DAY);

    //ChallengeNewPlayerRecord
    uint32 totalDurations = totalDuration * 1000;
    auto challengeData = new ChallengeData;

    challengeData->ID = instance->GetInstanceId(); //sObjectMgr->GetGenerator<HighGuid::Scenario>().Generate();
    challengeData->MapID = instance->GetId();
    challengeData->RecordTime = totalDurations;
    challengeData->Date = time(nullptr);
    challengeData->ChallengeLevel = _challengeModeLevel;
    challengeData->TimerLevel = std::max(_challengeModeLevel + mythicIncrement, 2);
    challengeData->ChallengeID = mapChallengeModeEntry ? mapChallengeModeEntry->ID : 0;
    challengeData->Affixes = _affixes;
    challengeData->GuildID = 0;
    //chest id
    if (_challengeChest.IsEmpty())
        challengeData->ChestID = sChallengeModeMgr->GetChest(challengeData->ChallengeID);
    else
        challengeData->ChestID = _challengeChest.GetEntry();

    std::map<ObjectGuid::LowType /*guild*/, uint32> guildCounter;
    DoOnPlayers([&](Player* player)
        {
            sChallengeModeMgr->Reward(player, _challengeModeLevel);

            ChallengeMember member;
            member.guid = player->GetGUID();
            member.specId = player->GetSpecializationId();
            member.Date = time(nullptr);
            member.ChallengeLevel = _challengeModeLevel;
            //chest id
            if (_challengeChest.IsEmpty())
                member.ChestID = sChallengeModeMgr->GetChest(challengeData->ChallengeID);
            else
                member.ChestID = _challengeChest.GetEntry();

            if (player->GetGuildId())
                guildCounter[player->GetGuildId()]++;

            challengeData->member.insert(member);
            if (sChallengeModeMgr->CheckBestMemberMapId(member.guid, challengeData))
            {
                WorldPackets::Misc::NewPlayerRecord newplayerrecord;
                newplayerrecord.CompletionMilliseconds = totalDurations;
                newplayerrecord.MapID = instance->GetId();
                newplayerrecord.ChallengeLevel = _challengeModeLevel;
                player->GetSession()->SendPacket(newplayerrecord.Write());
            }

            SendChallengeModeMapStatsUpdate(player, challengeData->ChallengeID, challengeData->RecordTime);
            player->GetSession()->SendChallengeModeMapStatsUpdate(mapChallengeModeEntry->ID);

            player->UpdateCriteria(CriteriaType::CompleteChallengeMode, instance->GetId(), _challengeModeLevel);

            player->RemoveAura(ChallengersBurden);
        });

    for (auto const& v : guildCounter)
        if (v.second >= 3)
            challengeData->GuildID = v.first;

    sChallengeModeMgr->SetChallengeMapData(challengeData->ID, challengeData);
    sChallengeModeMgr->CheckBestMapId(challengeData);
    sChallengeModeMgr->CheckBestGuildMapId(challengeData);
    sChallengeModeMgr->SaveChallengeToDB(challengeData);

    DoOnPlayers([this, totalDurations, mapChallengeModeEntry](Player* player)
        {
            sChallengeModeMgr->Reward(player, _challengeModeLevel);
            if (player->HasChallengeCompleted(mapChallengeModeEntry->ID))
            {
                CompletedChallenge* l_Challenge = player->GetCompletedChallenge(mapChallengeModeEntry->ID);
                if (l_Challenge)
                {
                    bool l_NewBestTime = totalDurations < l_Challenge->BestCompletion;
                    bool l_NewBestMedal = _challengeModeLevel > l_Challenge->Medal;

                    CharacterDatabasePreparedStatement* l_Statement = CharacterDatabase.GetPreparedStatement(CHAR_UPD_COMPLETED_CHALLENGE);
//                    "UPDATE character_completed_challenges SET BestCompletion = ? , LastCompletion = ? , Medal = ? , MedalDate = ? WHERE guid = ? AND KeyId = ? ",
                    l_Statement->setUInt32(0, l_NewBestTime ? totalDurations : l_Challenge->BestCompletion);
                    l_Statement->setUInt32(1, totalDurations);
                    l_Statement->setUInt32(2, l_NewBestMedal ? _challengeModeLevel : l_Challenge->Medal);
                    l_Statement->setUInt32(3, l_NewBestMedal ? uint32(time(nullptr)) : l_Challenge->MedalDate);
                    l_Statement->setUInt32(4, player->GetGUID().GetCounter());
                    l_Statement->setUInt32(5, mapChallengeModeEntry->ID);
                    CharacterDatabase.Execute(l_Statement);

                    if (l_NewBestMedal)
                    {
                        l_Challenge->Medal = _challengeModeLevel;
                        l_Challenge->MedalDate = uint32(time(nullptr));
                    }

                    if (l_NewBestTime)
                        l_Challenge->BestCompletion = totalDurations;

                    l_Challenge->LastCompletion = totalDurations;

                    /// Send new record only for new best time
                    if (!l_NewBestTime)
                        return;
                }
            }
            else
            {
                CharacterDatabasePreparedStatement* l_Statement = CharacterDatabase.GetPreparedStatement(CHAR_INS_COMPLETED_CHALLENGE);
//                "INSERT INTO character_completed_challenges (guid, KeyId, MapId, BestCompletion, LastCompletion, Medal, MedalDate) VALUE (?, ?, ?, ?, ?, ?, ?)",
                l_Statement->setUInt32(0, player->GetGUID().GetCounter());
                l_Statement->setUInt32(1, mapChallengeModeEntry->ID);
                l_Statement->setUInt32(2, instance->GetId());
                l_Statement->setUInt32(3, totalDurations);
                l_Statement->setUInt32(4, totalDurations);
                l_Statement->setUInt32(5, _challengeModeLevel);
                l_Statement->setUInt32(6, uint32(time(nullptr)));
                CharacterDatabase.Execute(l_Statement);
                CompletedChallenge l_Challenge;
                l_Challenge.MapID = instance->GetId();
                l_Challenge.Medal = _challengeModeLevel;
                l_Challenge.MedalDate = uint32(time(nullptr));
                l_Challenge.BestCompletion = totalDurations;
                l_Challenge.LastCompletion = totalDurations;
                player->AddCompletedChallenge(mapChallengeModeEntry->ID, l_Challenge);
                player->GetSession()->SendChallengeModeMapStatsUpdate(mapChallengeModeEntry->ID);
            }

            //new player record
            if (CompletedChallenge* t_Challenge = player->GetCompletedChallenge(mapChallengeModeEntry->ID))
            {
                WorldPackets::Misc::NewPlayerRecord newplayerrecord;
                newplayerrecord.CompletionMilliseconds = t_Challenge->BestCompletion;
                newplayerrecord.MapID = t_Challenge->MapID;
                newplayerrecord.ChallengeLevel = t_Challenge->Medal;
                player->GetSession()->SendPacket(newplayerrecord.Write());
            }
        });
}

void InstanceScript::ResetChallengeMode()
{
    if (_challenge)
        _challenge->ResetGo();

    instance->m_respawnChallenge = time(nullptr); // For respawn all mobs
    RepopPlayersAtGraveyard();
    instance->SetSpawnMode(DIFFICULTY_MYTHIC);
}

void InstanceScript::RepopPlayersAtGraveyard()
{
    if (!instance)
        return;

    instance->ApplyOnEveryPlayer([&](Player* player)
    {
        player->RepopAtGraveyard();
    });
}

bool InstanceScript::IsChallenge() const
{
    return _challenge != nullptr;
}

Challenge* InstanceScript::GetChallenge() const
{
    return _challenge;
}

void InstanceScript::GiveIslandAzeriteXpGain(Player* player, ObjectGuid guid, int32 xp)
{
    WorldPackets::Misc::IslandAzeriteXpGain XpGain;
    XpGain.SourceGuid = guid;
    XpGain.SourceID = guid.GetEntry();
    XpGain.PlayerGuid = player->GetGUID();
    XpGain.XpGain = xp;
    player->GetSession()->SendPacket(XpGain.Write());

    if (player->IsInAlliance())
        _islandCount[0] = _islandCount[0] + xp;
    else
        _islandCount[1] = _islandCount[1] + xp;

    player->CastSpell(player, SPELL_AZERITE_ENERGY, true);

  //  DoUpdateWorldState(WORLDSTATE_ALLIANCE_GAIN, _islandCount[0]); //declare to do later
   // DoUpdateWorldState(WORLDSTATE_HORDE_GAIN, _islandCount[1]);
}

void InstanceScript::IslandComplete(bool winnerIsAlliance)
{
    DoRemoveAurasDueToSpellOnPlayers(SPELL_AZERITE_RESIDUE);
    DoCastSpellOnPlayers(SPELL_ISLAND_COMPLETE);

    WorldPackets::Misc::IslandCompleted package;
    package.MapID = instance->GetId();
    package.Winner = winnerIsAlliance ? 0 : 1;
    DoOnPlayers([&](Player* player)
        {
            WorldPackets::Inspect::PlayerModelDisplayInfo playerModelDisplayInfo;
            playerModelDisplayInfo.Initialize(player);

            if ((winnerIsAlliance && player->IsInAlliance()) || (!winnerIsAlliance && player->IsInHorde()))
                package.DisplayInfos.push_back(playerModelDisplayInfo);

            if (!winnerIsAlliance)
                player->PlayConversation(7504);
        });
    instance->SendToPlayers(package.Write());
    //item 163612
    // 1553 255
}

void InstanceScript::OnPlayerExit(Player* player)
{
    player->RemoveAurasDueToSpell(SPELL_CHALLENGER_BURDEN);
}

void InstanceScript::OnPlayerDeath(Player* /*player*/)
{
    if (IsChallengeModeStarted())
    {
        ++_challengeModeDeathCount;

        SendChallengeModeElapsedTimer();
        SendChallengeModeDeathCount();
    }
}

void InstanceScript::OnUnitDeath(Unit* unit)
{
    if (IsChallengeModeStarted())
        if (!unit->IsPet() && !unit->IsPlayer())
            CastChallengeCreatureSpellOnDeath(unit->ToCreature());
}

uint32 InstanceScript::getScenarionStep() const
{
    return scenarioStep;
}

void InstanceScript::SendMythicPlusMapStatsUpdate(Player* player, uint32 challengeId, uint32 recordTime) const
{
    ChallengeByMap* bestMap = sChallengeModeMgr->BestForMember(player->GetGUID());
    if (!bestMap)
        return;

    auto itr = bestMap->find(instance->GetId());
    if (itr == bestMap->end())
        return;

    ChallengeData* best = itr->second;
    recordTime = best->RecordTime;
    if (!best)
        return;

    WorldPackets::Misc::NewPlayerRecord update;
    update.MapID = instance->GetId();
    update.CompletionMilliseconds = best->RecordTime;
    update.ChallengeLevel = challengeId;

    ChallengeMemberList members = best->member;

    if (player)
        player->SendDirectMessage(update.Write());
}
// < DekkCore
