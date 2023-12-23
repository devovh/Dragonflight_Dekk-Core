/*
 * Copyright (C) LatinCore Team
 *
 */

#include "Challenge.h"
#include "ChallengeModeMgr.h"
#include "MiscPackets.h"
#include "Chat.h"
#include "LFGMgr.h"
#include "LFGGroupData.h"
#include "FunctionProcessor.h"
#include "GroupMgr.h"
#include "Group.h"
#include "DatabaseEnv.h"
#include "InstancePackets.h"
#include "Item.h"
#include "Map.h"
#include "MiscPackets.h"
#include "ObjectAccessor.h"
#include "WorldStatePackets.h"
#include "Containers.h"
#include "GameObject.h"
#include <list>
#include <vector>

Challenge::Challenge(Map* map, Player* player, uint32 instanceID, Scenario* scenario) :
    InstanceScript(map->ToInstanceMap()), _scenario(scenario)
{
    if (!player)
    {
        _canRun = false;
        return;
    }

    SetChallenge(this);

    _checkStart = true;
    _canRun = true;
    _creator = player->GetGUID();
    _instanceID = instanceID;
    _challengeTimer = 0;
    _affixQuakingTimer = 0;
    _deathCount = 0;
    _complete = false;
    _run = false;
    _item = nullptr;

    ASSERT(map);
    _map = map;
    _mapID = _map->GetId();

    if (Group* group = player->GetGroup())
    {
        group->m_challengeInstanceID = _instanceID;
        m_ownerGuid = group->m_challengeOwner;
        m_itemGuid = group->m_challengeItem;
        _challengeEntry = group->m_challengeEntry;
        m_gguid = group->GetGUID();

        if (!m_itemGuid)
        {
            _canRun = false;
            ChatHandler(player->GetSession()).PSendSysMessage("Error: Key not found.");
            return;
        }

        for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
            if (Player* member = itr->GetSource())
                _challengers.insert(member->GetGUID());

        _affixes = group->m_affixes;
        _challengeLevel = group->m_challengeLevel;
    }
    else
    {
        m_ownerGuid = player->GetGUID();
        if (Item* item = player->GetItemByEntry(158923))
            m_itemGuid = item->GetGUID();
        else
        {
            _canRun = false;
            ChatHandler(player->GetSession()).PSendSysMessage("Error: Key not found.");
            return;
        }
        _challengeLevel = player->m_challengeKeyInfo.Level;
        _challengeEntry = player->m_challengeKeyInfo.challengeEntry;

        _affixes.fill(0);
        if (_challengeLevel > 3)
            _affixes[0] = player->m_challengeKeyInfo.Affix;
        if (_challengeLevel > 6)
            _affixes[1] = player->m_challengeKeyInfo.Affix1;
        if (_challengeLevel > 9)
            _affixes[2] = player->m_challengeKeyInfo.Affix2;
        if (_challengeLevel > 12)
            _affixes[2] = player->m_challengeKeyInfo.Affix3;
    }

    if (!_challengeEntry)
    {
        ChatHandler(player->GetSession()).PSendSysMessage("Error: Is not a challenge map.");
        _canRun = false;
        return;
    }

    _challengers.insert(_creator);
    _rewardLevel = CHALLENGE_NOT_IN_TIMER;

    for (auto const& affix : _affixes)
        _affixesTest.set(affix);

    for (uint8 i = 0; i < CHALLENGE_TIMER_LEVEL_3; i++)
        _chestTimers[i] = _challengeEntry->CriteriaCount[i];
}

Challenge::~Challenge()
{
    SetChallenge(nullptr);
}

void Challenge::SetInstanceScript(InstanceScript* instanceScript)
{
    _instanceScript = instanceScript;
}

void Challenge::OnPlayerEnterForScript(Player* player)
{
    if (!player)
        return;

    player->CastSpell(player, ChallengersBurden, true);
}

void Challenge::OnPlayerLeaveForScript(Player* player)
{
    if (!player)
        return;

    player->RemoveAura(ChallengersBurden);
}

void Challenge::OnPlayerDiesForScript(Player* /*player*/)
{
    ++_deathCount;
    ModChallengeTimer(Seconds(5).count());

    SendStartElapsedTimer();

    WorldPackets::Misc::UpdateDeathCount packet;
    packet.DeathCount = _deathCount;
    BroadcastPacket(packet.Write());
}

void Challenge::OnCreatureCreateForScript(Creature* creature)
{
    if (!creature || creature->IsTrigger() || creature->IsControlledByPlayer() || creature->GetCreatureType() == CREATURE_TYPE_CRITTER)
        return;

    Unit* owner = creature->GetOwner();
    if (owner && owner->IsPlayer())
        return;

    creature->AddAura(ChallengersMight, creature);
}

void Challenge::OnCreatureUpdateDifficulty(Creature* creature)
{
    if (!creature || creature->IsTrigger() || creature->IsControlledByPlayer() || creature->GetCreatureType() == CREATURE_TYPE_CRITTER)
        return;

    Unit* owner = creature->GetOwner();
    if (owner && owner->IsPlayer())
        return;

    creature->AddAura(ChallengersMight, creature);
}

void Challenge::OnCreatureRemoveForScript(Creature* /*creature*/) { }

void Challenge::EnterCombatForScript(Creature* creature, Unit* /*enemy*/)
{
    if (!creature || creature->IsTrigger() || creature->IsControlledByPlayer())
        return;

    Unit* owner = creature->GetOwner();
    if (owner && owner->IsPlayer())
        return;

    if (!creature->HasAura(ChallengersMight))
        creature->AddAura(ChallengersMight, creature);
}

void Challenge::CreatureDiesForScript(Creature* creature, Unit* /*killer*/)
{
    if (!creature || creature->IsTrigger() || creature->IsControlledByPlayer() || !creature->IsHostileToPlayers() || creature->GetCreatureType() == CREATURE_TYPE_CRITTER)
        return;

    if (creature->GetOwner() || creature->IsOnVehicle(creature))
        return;

    if (HasAffix(Affixes::Bolstering) && !creature->IsAffixDisabled())
        creature->CastSpell(creature, ChallengerBolstering, true);

    if (HasAffix(Affixes::Sanguine) && !creature->IsAffixDisabled())
        creature->CastSpell(creature, ChallengerSanguine, true);

    if (HasAffix(Affixes::Bursting) && !creature->IsAffixDisabled())
        creature->CastSpell(creature, ChallengerBursting, true);
}

void Challenge::OnUnitCharmed(Unit* unit, Unit* /*charmer*/)
{
    if (!unit || !unit->ToCreature())
        return;

    unit->RemoveAura(ChallengerBolstering);
    unit->RemoveAura(ChallengerRaging);
    unit->RemoveAura(ChallengersMight);
    unit->RemoveAura(207850); // Bond of Strength
}

void Challenge::OnUnitRemoveCharmed(Unit* unit, Unit* /*charmer*/)
{
    if (!unit || !unit->ToCreature())
        return;

    unit->AddAura(ChallengersMight, unit);
    unit->RemoveAura(ChallengerBolstering);
}

void Challenge::Update(uint32 diff)
{
    m_Functions.Update(diff);

    if (_complete) // Stop update if complete
        return;

    if (!_run && _checkStart)
    {
        if (CanStart())
        {
            Start();
            SummonWall(nullptr);
        }
        return;
    }

    _challengeTimer += diff;

    if (_affixQuakingTimer)
    {
        if (_affixQuakingTimer <= diff)
        {
            _map->ApplyOnEveryPlayer([&](Player* plr)
            {
                if (!plr->HasAura(ChallengerQuake) && !plr->HasUnitFlag2(UNIT_FLAG2_NO_ACTIONS) && !plr->IsOnVehicle(plr))
                    plr->AddAura(ChallengerQuake, plr);
            });
            _affixQuakingTimer = 20000;
        }
        else
            _affixQuakingTimer -= diff;
    }

    if (!_isKeyDepleted && GetChallengeTimer() > uint32(_challengeEntry->CriteriaCount[0]))
    {
        _isKeyDepleted = true;

        if (Group* group = sGroupMgr->GetGroupByGUID(m_gguid))
        {
            group->m_challengeEntry = nullptr;
            group->m_challengeLevel = 0;
            group->m_affixes.fill(0);
            group->SetDungeonDifficultyID(DIFFICULTY_MYTHIC);
            group->m_challengeInstanceID = 0;
        }

        _item = nullptr;
        Player* keyOwner = ObjectAccessor::FindPlayer(_map, m_ownerGuid);
        if (keyOwner)
            _item = keyOwner->GetItemByGuid(m_itemGuid);

        if (_item)
            keyOwner->ChallengeKeyCharded(_item, _challengeLevel);
        else
        {
            if (auto player = ObjectAccessor::FindPlayer(m_ownerGuid))
            {
                if (auto item = player->GetItemByGuid(m_itemGuid))
                {
                    keyOwner->ChallengeKeyCharded(item, _challengeLevel);
                    _item = item;
                }
           //     else
               //     CharacterDatabase.PQuery("UPDATE challenge_key SET KeyIsCharded = 0, InstanceID = 0 WHERE guid = %u", m_ownerGuid.GetGUID());
            }
          //  else
             //   CharacterDatabase.PQuery("UPDATE challenge_key SET KeyIsCharded = 0, InstanceID = 0 WHERE guid = %u", m_ownerGuid.GetGUIDLow());
        }
    }
}

bool Challenge::CanStart()
{
    if (_run)
        return true;

    return _map->GetPlayerCount() == _challengers.size();
}

void Challenge::Start()
{
    if (!_canRun)
        return;

    Player* keyOwner = ObjectAccessor::FindPlayer(m_ownerGuid);
    if (!keyOwner)
        return;

    _item = keyOwner->GetItemByGuid(m_itemGuid);
    if (!_item)
        return;

    keyOwner->m_challengeKeyInfo.InstanceID = keyOwner->GetInstanceId();
    keyOwner->m_challengeKeyInfo.needUpdate = true;

    _isKeyDepleted = false;

    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float o = 0.0f;

    if(!sChallengeModeMgr->GetStartPosition(_map->GetId(), x, y, z, o, m_ownerGuid))
        return;

    _map->ApplyOnEveryPlayer([&](Player* player)
    {
        if (player)
        {
            player->SafeTeleport(_map->GetId(), x, y, z, o);
            _scenario->SendStepUpdate(player, true);
        }
    });

    BroadcastPacket(WorldPackets::Misc::Reset(_mapID).Write());

    SendStartTimer();
    SendChallengeModeStart();

    m_Functions.AddFunction([this]() -> void
    {
        _challengeTimer = 0;
        SendStartElapsedTimer();

        for (ObjectGuid guid : _challengeDoorGuids)
            if (GameObject* challengeDoor = ObjectAccessor::FindGameObject(guid))
                challengeDoor->Delete();

        _run = true;

    }, m_Functions.CalculateTime(10 * IN_MILLISECONDS));

    _checkStart = false;

    if (HasAffix(Affixes::Quaking))
        _affixQuakingTimer = 20000;
}

void Challenge::Complete()
{
    if (_complete)
        return;

    _complete = true;

    Player* keyOwner = ObjectAccessor::FindPlayer(_map, m_ownerGuid);
    _item = nullptr;
    if (keyOwner)
        _item = keyOwner->GetItemByGuid(m_itemGuid);

    HitTimer();

    WorldPackets::Misc::StopElapsedTimer stopElapsedTimer;
    stopElapsedTimer.TimerID = WORLD_TIMER_TYPE_CHALLENGE_MODE;
    BroadcastPacket(stopElapsedTimer.Write());

    if (_challengeEntry)
    {
        WorldPackets::Misc::Complete complete;
        complete.MapId = _mapID;
        complete.CompletionMilliseconds = _challengeTimer;
        complete.ChallengeLevel = _challengeLevel;
        complete.ChallengeId = _challengeEntry->ID;
        BroadcastPacket(complete.Write());
    }

    if (Group* group = sGroupMgr->GetGroupByGUID(m_gguid))
    {
        group->m_challengeEntry = nullptr;
        group->m_challengeLevel = 0;
        group->m_affixes.fill(0);
        group->SetDungeonDifficultyID(DIFFICULTY_MYTHIC);
        group->m_challengeInstanceID = 0;
    }

    // Reward part
    if (_item)
    {
        if (!_isKeyDepleted)
        {
          //  _item->SetModifier(ITEM_MODIFIER_CHALLENGE_MAP_CHALLENGE_MODE_ID, *Trinity::Containers::SelectRandomWeightedContainerElement(sDB2Manager.GetChallngeMaps(), sDB2Manager.GetChallngesWeight()));
            _item->SetModifier(ITEM_MODIFIER_CHALLENGE_KEYSTONE_LEVEL, std::min(_challengeLevel + _rewardLevel, sWorld->getIntConfig(CONFIG_CHALLENGE_LEVEL_LIMIT)));
        }
        else
            keyOwner->ChallengeKeyCharded(_item, _challengeLevel);

        keyOwner->UpdateChallengeKey(_item);
        _item->SetState(ITEM_CHANGED, keyOwner);
    }
    else
    {
        if (auto player = ObjectAccessor::FindPlayer(m_ownerGuid))
        {
            if (auto item = player->GetItemByGuid(m_itemGuid))
            {
                keyOwner->ChallengeKeyCharded(_item, _challengeLevel);
                _item = item;
            }
          // else
           //     CharacterDatabase.PQuery("UPDATE challenge_key SET KeyIsCharded = 0, InstanceID = 0 WHERE guid = %u", m_ownerGuid.GetGUIDLow());
        }
       // else
          //  CharacterDatabase.PQuery("UPDATE challenge_key SET KeyIsCharded = 0, InstanceID = 0 WHERE guid = %u", m_ownerGuid.GetGUIDLow());
    }

    auto challengeData = new ChallengeData;
   // challengeData->ID = sObjectMgr->GetGenerator<HighGuid::Scenario>().Generate(); 
    challengeData->MapID = _mapID;
    challengeData->RecordTime = _challengeTimer;
    challengeData->Date = time(nullptr);
    challengeData->ChallengeLevel = _challengeLevel;
    challengeData->TimerLevel = _rewardLevel;
    challengeData->ChallengeID = _challengeEntry ? _challengeEntry->ID : 0;
    challengeData->Affixes = _affixes;
    challengeData->GuildID = 0;
    challengeData->ChestID = _challengeChest.GetEntry();

    std::map<ObjectGuid::LowType /*guild*/, uint32> guildCounter;
    std::map<uint32, std::string> anticheatData;
    _map->ApplyOnEveryPlayer([&](Player* player)
    {
        ChallengeMember member;
        member.guid = player->GetGUID();
        member.specId = player->GetSpecializationId();
        member.Date = time(nullptr);
        member.ChallengeLevel = _challengeLevel;
        member.ChestID = _challengeChest.GetEntry();

       // anticheatData[player->GetGUID()] = player->GetName();

        if (player->GetGuildId())
            guildCounter[player->GetGuildId()]++;

        challengeData->member.insert(member);
        if (sChallengeModeMgr->CheckBestMemberMapId(member.guid, challengeData))
            SendChallengeModeNewPlayerRecord(player);

        player->UpdateCriteria(CriteriaType::CompleteChallengeMode, _mapID, _rewardLevel);

        player->RemoveAura(ChallengersBurden);
        player->CastSpell(player, SPELL_CHALLENGE_ANTIKICK, true);
        player->KilledMonsterCredit(542180); // for daily event quest
    });

    if (GetChallengeTimer() < 9 * MINUTE)
    {
        std::string cheaters = "";
        for (auto const& record : anticheatData)
            cheaters += record.second + " (" + std::to_string(record.first) + ") ";
    }

    for (auto const& v : guildCounter)
        if (v.second >= 3)
            challengeData->GuildID = v.first;

    sChallengeModeMgr->SetChallengeMapData(challengeData->ID, challengeData);
    sChallengeModeMgr->CheckBestMapId(challengeData);
    sChallengeModeMgr->CheckBestGuildMapId(challengeData);
    sChallengeModeMgr->SaveChallengeToDB(challengeData);
}

void Challenge::HitTimer()
{
    if (GetChallengeTimer() < _chestTimers[2])
        _rewardLevel = CHALLENGE_TIMER_LEVEL_3; // 3 chests + 3 levels
    else if (GetChallengeTimer() < _chestTimers[1])
        _rewardLevel = CHALLENGE_TIMER_LEVEL_2; // 2 chests + 2 levels
    else if (GetChallengeTimer() < _chestTimers[0])
        _rewardLevel = CHALLENGE_TIMER_LEVEL_1; // 1 chest + 1 level
    else
        _rewardLevel = CHALLENGE_NOT_IN_TIMER;

    if (!_map)
        return;

    for (auto const& guid : _challengers)
        _countItems[guid] = 0;

    switch (_rewardLevel)
    {
        case CHALLENGE_TIMER_LEVEL_3: // 3 chests + 3 levels
        case CHALLENGE_TIMER_LEVEL_2: // 2 chests + 2 levels
        case CHALLENGE_TIMER_LEVEL_1: // 1 chest + 1 level
        case CHALLENGE_NOT_IN_TIMER:  // 0 chest
        {
            if (GameObject* chest = _map->GetGameObject(_challengeChest))
                chest->SetRespawnTime(7 * DAY);

            float _chance = sChallengeModeMgr->GetChanceItem(_rewardLevel, _challengeLevel);
            auto countItems = int32(_chance / 100.0f);
            _chance -= countItems * 100.0f;

            if (roll_chance_f(_chance))
                countItems++;

            while (countItems > 0)
            {
                auto _tempList = _challengers;
                //  Trinity::Containers::RandomResize(_tempList, countItems);

                for (auto const& guid : _tempList)
                {
                    countItems--;
                    _countItems[guid] += 1;
                }
            }
        }
        break;
        default:
            break;
    }
}

uint32 Challenge::GetChallengeLevel() const
{
    return std::min(_challengeLevel, sWorld->getIntConfig(CONFIG_CHALLENGE_LEVEL_LIMIT));
}

std::array<uint32, 4> Challenge::GetAffixes() const
{
    return _affixes;
}

bool Challenge::HasAffix(Affixes affix)
{
    return _affixesTest.test(size_t(affix));
}

uint32 Challenge::GetChallengeTimerToNow() const
{
    return (getMSTime() - _challengeTimer) / IN_MILLISECONDS;
}

void Challenge::BroadcastPacket(WorldPacket const* data) const
{
    _map->ApplyOnEveryPlayer([&](Player* player)
    {
        player->SendDirectMessage(data);
    });
}

uint32 Challenge::GetChallengeTimer()
{
    if (!_challengeTimer)
        return 0;

    return _challengeTimer / IN_MILLISECONDS;
}

void Challenge::ModChallengeTimer(uint32 timer)
{
    if (!timer)
        return;

    _challengeTimer += timer * IN_MILLISECONDS;
}

void Challenge::ResetGo()
{
    for (ObjectGuid guid : _challengeDoorGuids)
        if (GameObject* challengeDoor = ObjectAccessor::FindGameObject(guid))
            challengeDoor->SetGoState(GO_STATE_READY);

    if (GameObject* challengeOrb = _map->GetGameObject(_challengeOrbGuid))
    {
        challengeOrb->SetGoState(GO_STATE_READY);
        challengeOrb->RemoveFlag(GO_FLAG_NODESPAWN);
    }
}

void Challenge::SendStartTimer(Player* player)
{
    WorldPackets::Misc::StartTimer startTimer;
    startTimer.Type = WorldPackets::Misc::StartTimer::ChallengeMode;
    startTimer.TimeLeft = (Seconds(ChallengeDelayTimer));
    startTimer.TotalTime = (Seconds(ChallengeDelayTimer));
    if (player)
        player->SendDirectMessage(startTimer.Write());
    else
        BroadcastPacket(startTimer.Write());
}

void Challenge::SendStartElapsedTimer(Player* player)
{
    WorldPackets::Misc::StartElapsedTimer timer;
    timer.Timer.TimerID = 10;
    timer.Timer.CurrentDuration = GetChallengeTimer();

    if (player)
        player->SendDirectMessage(timer.Write());
    else
        BroadcastPacket(timer.Write());
}

void Challenge::SendChallengeModeStart(Player* player)
{
    if (!_challengeEntry)
        return;

    WorldPackets::Misc::Start start;
    start.MapID = _mapID;
    start.ChallengeID = _challengeEntry->ID;
    start.ChallengeLevel = _challengeLevel;
    start.Affixes1 = 0;
    start.Affixes2 = 0;
    start.Affixes3 = 0;
    start.Affixes4 = 0;

    if (player)
        player->SendDirectMessage(start.Write());
    else
        BroadcastPacket(start.Write());
}

void Challenge::SendChallengeModeNewPlayerRecord(Player* player)
{
    WorldPackets::Misc::NewPlayerRecord newRecord;
    newRecord.MapID = _mapID;
    newRecord.CompletionMilliseconds = _challengeTimer;
    newRecord.ChallengeLevel = _rewardLevel;

    if (player)
        player->SendDirectMessage(newRecord.Write());
}

void Challenge::SummonWall(Player* /*player*/)
{
    for (ObjectGuid guid : _challengeDoorGuids)
        if (GameObject* challengeDoor = ObjectAccessor::FindGameObject(guid))
            challengeDoor->SetRespawnTime(7 * DAY);
}

uint8 Challenge::GetItemCount(ObjectGuid guid) const
{
    auto itr = _countItems.find(guid);
    if (itr == _countItems.end())
        return 0;

    return itr->second;
}

uint8 Challenge::GetLevelBonus() const
{
    switch (_rewardLevel)
    {
        case CHALLENGE_TIMER_LEVEL_3: // 3 chests + 3 levels
            return 2;
        case CHALLENGE_TIMER_LEVEL_2: // 2 chests + 2 levels
            return 1;
        case CHALLENGE_TIMER_LEVEL_1: // 1 chest + 1 level
        case CHALLENGE_NOT_IN_TIMER:  // 0 chest
        default:
            return 0;
    }
}

void Challenge::SendDeathCount(Player* player/*= nullptr*/)
{
    WorldPackets::Misc::UpdateDeathCount packet;
    packet.DeathCount = _deathCount;

    if (player)
        player->SendDirectMessage(packet.Write());
    else
        BroadcastPacket(packet.Write());
}
