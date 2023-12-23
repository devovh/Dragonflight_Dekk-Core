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

#include "ScriptedCreature.h"
#include "AreaBoundary.h"
#include "DB2Stores.h"
#include "Cell.h"
#include "CellImpl.h"
#include "CreatureAIImpl.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "InstanceScript.h"
#include "Log.h"
#include "Loot.h"
#include "MotionMaster.h"
#include "ObjectAccessor.h"
#include "PhasingHandler.h"
#include "Spell.h"
#include "SpellMgr.h"
#include "TemporarySummon.h"
//DekkCore
#include "BotAITool.h"
#include "BotGroupAI.h"
#include "Pet.h"
//dekkCore
void SummonList::Summon(Creature const* summon)
{
    _storage.push_back(summon->GetGUID());
}

void SummonList::Despawn(Creature const* summon)
{
    _storage.remove(summon->GetGUID());
}

void SummonList::DoZoneInCombat(uint32 entry)
{
    for (StorageType::iterator i = _storage.begin(); i != _storage.end();)
    {
        Creature* summon = ObjectAccessor::GetCreature(*_me, *i);
        ++i;
        if (summon && summon->IsAIEnabled()
                && (!entry || summon->GetEntry() == entry))
        {
            summon->AI()->DoZoneInCombat();
        }
    }
}

void SummonList::DespawnEntry(uint32 entry)
{
    for (StorageType::iterator i = _storage.begin(); i != _storage.end();)
    {
        Creature* summon = ObjectAccessor::GetCreature(*_me, *i);
        if (!summon)
            i = _storage.erase(i);
        else if (summon->GetEntry() == entry)
        {
            i = _storage.erase(i);
            summon->DespawnOrUnsummon();
        }
        else
            ++i;
    }
}

void SummonList::DespawnAll()
{
    while (!_storage.empty())
    {
        Creature* summon = ObjectAccessor::GetCreature(*_me, _storage.front());
        _storage.pop_front();
        if (summon)
            summon->DespawnOrUnsummon();
    }
}

void SummonList::RemoveNotExisting()
{
    for (StorageType::iterator i = _storage.begin(); i != _storage.end();)
    {
        if (ObjectAccessor::GetCreature(*_me, *i))
            ++i;
        else
            i = _storage.erase(i);
    }
}

bool SummonList::HasEntry(uint32 entry) const
{
    for (ObjectGuid const& guid : _storage)
    {
        Creature* summon = ObjectAccessor::GetCreature(*_me, guid);
        if (summon && summon->GetEntry() == entry)
            return true;
    }

    return false;
}

void SummonList::DoActionImpl(int32 action, StorageType const& summons)
{
    for (ObjectGuid const& guid : summons)
    {
        Creature* summon = ObjectAccessor::GetCreature(*_me, guid);
        if (summon && summon->IsAIEnabled())
            summon->AI()->DoAction(action);
    }
}

ScriptedAI::ScriptedAI(Creature* creature) : ScriptedAI(creature, creature->GetScriptId()) { }

ScriptedAI::ScriptedAI(Creature* creature, uint32 scriptId) : CreatureAI(creature, scriptId),
    IsFleeing(false),
    summons(creature),
    instance(creature->GetInstanceScript()),
    _isCombatMovementAllowed(true),
    _checkHomeTimer(5000) //Dekkcor
{
    _isHeroic = me->GetMap()->IsHeroic();
    _difficulty = me->GetMap()->GetDifficultyID();
}

void ScriptedAI::AttackStartNoMove(Unit* who)
{
    if (!who)
        return;

    if (me->Attack(who, true))
        DoStartNoMovement(who);
}

void ScriptedAI::AttackStart(Unit* who)
{
    if (IsCombatMovementAllowed())
        CreatureAI::AttackStart(who);
    else
        AttackStartNoMove(who);
}

void ScriptedAI::UpdateAI(uint32 /*diff*/)
{
    // Check if we have a current target
    if (!UpdateVictim())
        return;

    DoMeleeAttackIfReady();
}

void ScriptedAI::DoStartMovement(Unit* victim, float distance, float angle)
{
    if (victim)
        me->GetMotionMaster()->MoveChase(victim, distance, angle);
}

void ScriptedAI::DoStartNoMovement(Unit* victim)
{
    if (!victim)
        return;

    me->GetMotionMaster()->MoveIdle();
}

void ScriptedAI::DoStopAttack()
{
    if (me->GetVictim())
        me->AttackStop();
}

void ScriptedAI::DoCastSpell(Unit* target, SpellInfo const* spellInfo, bool triggered)
{
    if (!target || me->IsNonMeleeSpellCast(false))
        return;

    me->StopMoving();
    me->CastSpell(target, spellInfo->Id, triggered ? TRIGGERED_FULL_MASK : TRIGGERED_NONE);
}

void ScriptedAI::DoPlaySoundToSet(WorldObject* source, uint32 soundId)
{
    if (!source)
        return;

    if (!sSoundKitStore.LookupEntry(soundId))
    {
        TC_LOG_ERROR("scripts.ai", "ScriptedAI::DoPlaySoundToSet: Invalid soundId {} used in DoPlaySoundToSet (Source: {})", soundId, source->GetGUID().ToString());
        return;
    }

    source->PlayDirectSound(soundId);
}

void ScriptedAI::AddThreat(Unit* victim, float amount, Unit* who)
{
    if (!victim)
        return;
    if (!who)
        who = me;
    who->GetThreatManager().AddThreat(victim, amount, nullptr, true, true);
}

void ScriptedAI::ModifyThreatByPercent(Unit* victim, int32 pct, Unit* who)
{
    if (!victim)
        return;
    if (!who)
        who = me;
    who->GetThreatManager().ModifyThreatByPercent(victim, pct);
}

void ScriptedAI::ResetThreat(Unit* victim, Unit* who)
{
    if (!victim)
        return;
    if (!who)
        who = me;
    who->GetThreatManager().ResetThreat(victim);
}

void ScriptedAI::ResetThreatList(Unit* who)
{
    if (!who)
        who = me;
    who->GetThreatManager().ResetAllThreat();
}

float ScriptedAI::GetThreat(Unit const* victim, Unit const* who)
{
    if (!victim)
        return 0.0f;
    if (!who)
        who = me;
    return who->GetThreatManager().GetThreat(victim);
}

void ScriptedAI::ForceCombatStop(Creature* who, bool reset /*= true*/)
{
    if (!who || !who->IsInCombat())
        return;

    who->CombatStop(true);
    who->DoNotReacquireSpellFocusTarget();
    who->GetMotionMaster()->Clear(MOTION_PRIORITY_NORMAL);

    if (reset)
    {
        who->LoadCreaturesAddon();
        if (!me->IsTapListNotClearedOnEvade())
            who->SetTappedBy(nullptr);

        who->ResetPlayerDamageReq();
        who->SetLastDamagedTime(0);
        who->SetCannotReachTarget(false);
    }
}

void ScriptedAI::ForceCombatStopForCreatureEntry(uint32 entry, float maxSearchRange /*= 250.0f*/, bool samePhase /*= true*/, bool reset /*= true*/)
{
    TC_LOG_DEBUG("scripts.ai", "ScriptedAI::ForceCombatStopForCreatureEntry: called on '{}'. Debug info: {}", me->GetGUID().ToString(), me->GetDebugInfo());

    std::list<Creature*> creatures;
    Trinity::AllCreaturesOfEntryInRange check(me, entry, maxSearchRange);
    Trinity::CreatureListSearcher<Trinity::AllCreaturesOfEntryInRange> searcher(me, creatures, check);

    if (!samePhase)
        PhasingHandler::SetAlwaysVisible(me, true, false);

    Cell::VisitGridObjects(me, searcher, maxSearchRange);

    if (!samePhase)
        PhasingHandler::SetAlwaysVisible(me, false, false);

    for (Creature* creature : creatures)
        ForceCombatStop(creature, reset);
}

void ScriptedAI::ForceCombatStopForCreatureEntry(std::vector<uint32> creatureEntries, float maxSearchRange /*= 250.0f*/, bool samePhase /*= true*/, bool reset /*= true*/)
{
    for (uint32 const entry : creatureEntries)
        ForceCombatStopForCreatureEntry(entry, maxSearchRange, samePhase, reset);
}

Creature* ScriptedAI::DoSpawnCreature(uint32 entry, float offsetX, float offsetY, float offsetZ, float angle, uint32 type, Milliseconds despawntime)
{
    return me->SummonCreature(entry, me->GetPositionX() + offsetX, me->GetPositionY() + offsetY, me->GetPositionZ() + offsetZ, angle, TempSummonType(type), despawntime);
}

bool ScriptedAI::HealthBelowPct(uint32 pct) const
{
    return me->HealthBelowPct(pct);
}

bool ScriptedAI::HealthAbovePct(uint32 pct) const
{
    return me->HealthAbovePct(pct);
}

SpellInfo const* ScriptedAI::SelectSpell(Unit* target, uint32 school, uint32 mechanic, SelectTargetType targets, float rangeMin, float rangeMax, SelectEffect effect)
{
    // No target so we can't cast
    if (!target)
        return nullptr;

    // Silenced so we can't cast
    if (me->IsSilenced(school ? SpellSchoolMask(school) : SPELL_SCHOOL_MASK_MAGIC))
        return nullptr;

    // Using the extended script system we first create a list of viable spells
    SpellInfo const* apSpell[MAX_CREATURE_SPELLS];
    memset(apSpell, 0, MAX_CREATURE_SPELLS * sizeof(SpellInfo*));

    uint32 spellCount = 0;

    SpellInfo const* tempSpell = nullptr;
    AISpellInfoType const* aiSpell = nullptr;

    // Check if each spell is viable(set it to null if not)
    for (uint32 spell : me->m_spells)
    {
        tempSpell = sSpellMgr->GetSpellInfo(spell, me->GetMap()->GetDifficultyID());
        aiSpell = GetAISpellInfo(spell, me->GetMap()->GetDifficultyID());

        // This spell doesn't exist
        if (!tempSpell || !aiSpell)
            continue;

        // Targets and Effects checked first as most used restrictions
        // Check the spell targets if specified
        if (targets && !(aiSpell->Targets & (1 << (targets-1))))
            continue;

        // Check the type of spell if we are looking for a specific spell type
        if (effect && !(aiSpell->Effects & (1 << (effect-1))))
            continue;

        // Check for school if specified
        if (school && (tempSpell->SchoolMask & school) == 0)
            continue;

        // Check for spell mechanic if specified
        if (mechanic && tempSpell->Mechanic != mechanic)
            continue;

        // Continue if we don't have the mana to actually cast this spell
        bool hasPower = true;
        for (SpellPowerCost const& cost : tempSpell->CalcPowerCost(me, tempSpell->GetSchoolMask()))
        {
            if (cost.Amount > me->GetPower(cost.Power))
            {
                hasPower = false;
                break;
            }
        }

        if (!hasPower)
            continue;

        // Check if the spell meets our range requirements
        if (rangeMin && me->GetSpellMinRangeForTarget(target, tempSpell) < rangeMin)
            continue;
        if (rangeMax && me->GetSpellMaxRangeForTarget(target, tempSpell) > rangeMax)
            continue;

        // Check if our target is in range
        if (me->IsWithinDistInMap(target, float(me->GetSpellMinRangeForTarget(target, tempSpell))) || !me->IsWithinDistInMap(target, float(me->GetSpellMaxRangeForTarget(target, tempSpell))))
            continue;

        // All good so lets add it to the spell list
        apSpell[spellCount] = tempSpell;
        ++spellCount;
    }

    // We got our usable spells so now lets randomly pick one
    if (!spellCount)
        return nullptr;

    return apSpell[urand(0, spellCount - 1)];
}

void ScriptedAI::DoTeleportTo(float x, float y, float z, uint32 time)
{
    me->Relocate(x, y, z);
    float speed = me->GetDistance(x, y, z) / ((float)time * 0.001f);
    me->MonsterMoveWithSpeed(x, y, z, speed);
}

void ScriptedAI::DoTeleportTo(const float position[4])
{
    me->NearTeleportTo(position[0], position[1], position[2], position[3]);
}

void ScriptedAI::DoTeleportPlayer(Unit* unit, float x, float y, float z, float o)
{
    if (!unit)
        return;

    if (Player* player = unit->ToPlayer())
        player->TeleportTo(unit->GetMapId(), x, y, z, o, TELE_TO_NOT_LEAVE_COMBAT);
    else
        TC_LOG_ERROR("scripts.ai", "ScriptedAI::DoTeleportPlayer: Creature {} Tried to teleport non-player unit ({}) to x: {} y:{} z: {} o: {}. Aborted.",
            me->GetGUID().ToString(), unit->GetGUID().ToString(), x, y, z, o);
}

void ScriptedAI::DoTeleportAll(float x, float y, float z, float o)
{
    Map* map = me->GetMap();
    if (!map->IsDungeon())
        return;

    for (MapReference const& mapref : map->GetPlayers())
        if (Player* player = mapref.GetSource())
            if (player->IsAlive())
                player->TeleportTo(me->GetMapId(), x, y, z, o, TELE_TO_NOT_LEAVE_COMBAT);
}

Unit* ScriptedAI::DoSelectLowestHpFriendly(float range, uint32 minHPDiff)
{
    Unit* unit = nullptr;
    Trinity::MostHPMissingInRange u_check(me, range, minHPDiff);
    Trinity::UnitLastSearcher<Trinity::MostHPMissingInRange> searcher(me, unit, u_check);
    Cell::VisitAllObjects(me, searcher, range);

    return unit;
}

Unit* ScriptedAI::DoSelectBelowHpPctFriendlyWithEntry(uint32 entry, float range, uint8 minHPDiff, bool excludeSelf)
{
    Unit* unit = nullptr;
    Trinity::FriendlyBelowHpPctEntryInRange u_check(me, entry, range, minHPDiff, excludeSelf);
    Trinity::UnitLastSearcher<Trinity::FriendlyBelowHpPctEntryInRange> searcher(me, unit, u_check);
    Cell::VisitAllObjects(me, searcher, range);

    return unit;
}

std::list<Creature*> ScriptedAI::DoFindFriendlyCC(float range)
{
    std::list<Creature*> list;
    Trinity::FriendlyCCedInRange u_check(me, range);
    Trinity::CreatureListSearcher<Trinity::FriendlyCCedInRange> searcher(me, list, u_check);
    Cell::VisitAllObjects(me, searcher, range);

    return list;
}

std::list<Creature*> ScriptedAI::DoFindFriendlyMissingBuff(float range, uint32 uiSpellid)
{
    std::list<Creature*> list;
    Trinity::FriendlyMissingBuffInRange u_check(me, range, uiSpellid);
    Trinity::CreatureListSearcher<Trinity::FriendlyMissingBuffInRange> searcher(me, list, u_check);
    Cell::VisitAllObjects(me, searcher, range);

    return list;
}

Player* ScriptedAI::GetPlayerAtMinimumRange(float minimumRange)
{
    Player* player = nullptr;

    Trinity::PlayerAtMinimumRangeAway check(me, minimumRange);
    Trinity::PlayerSearcher<Trinity::PlayerAtMinimumRangeAway> searcher(me, player, check);
    Cell::VisitWorldObjects(me, searcher, minimumRange);

    return player;
}

void ScriptedAI::SetEquipmentSlots(bool loadDefault, int32 mainHand /*= EQUIP_NO_CHANGE*/, int32 offHand /*= EQUIP_NO_CHANGE*/, int32 ranged /*= EQUIP_NO_CHANGE*/)
{
    if (loadDefault)
    {
        me->LoadEquipment(me->GetOriginalEquipmentId(), true);
        return;
    }

    if (mainHand >= 0)
        me->SetVirtualItem(0, uint32(mainHand));

    if (offHand >= 0)
        me->SetVirtualItem(1, uint32(offHand));

    if (ranged >= 0)
        me->SetVirtualItem(2, uint32(ranged));
}

void ScriptedAI::SetCombatMovement(bool allowMovement)
{
    _isCombatMovementAllowed = allowMovement;
}

// BossAI - for instanced bosses
BossAI::BossAI(Creature* creature, uint32 bossId) : ScriptedAI(creature), instance(creature->GetInstanceScript()), summons(creature), _bossId(bossId)
{
    if (instance)
        SetBoundary(instance->GetBossBoundary(bossId));
    scheduler.SetValidator([this]
    {
        return !me->HasUnitState(UNIT_STATE_CASTING);
    });
}

void BossAI::_Reset()
{
    if (!me->IsAlive())
        return;

    me->SetCombatPulseDelay(0);
    me->ResetLootMode();
    events.Reset();
    summons.DespawnAll();
    me->RemoveAllAreaTriggers();
    scheduler.CancelAll();
    if (instance && instance->GetBossState(_bossId) != DONE)
        instance->SetBossState(_bossId, NOT_STARTED);
}

void BossAI::_JustDied()
{
    Talk(TALK_DEFAULT_BOSS_DEATH);
    events.Reset();
    summons.DespawnAll();
    scheduler.CancelAll();
    if (instance)
        instance->SetBossState(_bossId, DONE);

}

void BossAI::_JustReachedHome()
{
    Talk(TALK_DEFAULT_BOSS_WIPE);
    me->setActive(false);
}

void BossAI::_JustEngagedWith(Unit* who)
{
    if (instance)
    {
        // bosses do not respawn, check only on enter combat
        if (!instance->CheckRequiredBosses(_bossId, who->ToPlayer()))
        {
            EnterEvadeMode(EVADE_REASON_SEQUENCE_BREAK);
            return;
        }
        instance->SetBossState(_bossId, IN_PROGRESS);
    }

    Talk(TALK_DEFAULT_BOSS_AGGRO);
    me->SetCombatPulseDelay(5);
    me->setActive(true);
    DoZoneInCombat();
    ScheduleTasks();
}

void BossAI::TeleportCheaters()
{
    float x, y, z;
    me->GetPosition(x, y, z);

    for (auto const& pair : me->GetCombatManager().GetPvECombatRefs())
    {
        Unit* target = pair.second->GetOther(me);
        if (target->IsControlledByPlayer() && !IsInBoundary(target))
            target->NearTeleportTo(x, y, z, 0);
    }
}

void BossAI::JustSummoned(Creature* summon)
{
    summons.Summon(summon);
    if (me->IsEngaged())
        DoZoneInCombat(summon);
}

void BossAI::SummonedCreatureDespawn(Creature* summon)
{
    summons.Despawn(summon);
}

void BossAI::UpdateAI(uint32 diff)
{
    if (!UpdateVictim())
        return;

    events.Update(diff);

    if (me->HasUnitState(UNIT_STATE_CASTING))
        return;

    while (uint32 eventId = events.ExecuteEvent())
    {
        ExecuteEvent(eventId);
        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;
    }

    if (GetBaseAttackSpell() != 0)
        DoSpellAttackIfReady(GetBaseAttackSpell());
    else
        DoMeleeAttackIfReady();
}

bool BossAI::CanAIAttack(Unit const* target) const
{
    return IsInBoundary(target);
}

void BossAI::_DespawnAtEvade(Seconds delayToRespawn /*= 30s*/, Creature* who /*= nullptr*/)
{
    if (delayToRespawn < 2s)
    {
        TC_LOG_ERROR("scripts.ai", "BossAI::_DespawnAtEvade: called with delay of {} seconds, defaulting to 2 (me: {})", delayToRespawn.count(), me->GetGUID().ToString());
        delayToRespawn = 2s;
    }

    if (!who)
        who = me;

    if (TempSummon* whoSummon = who->ToTempSummon())
    {
        TC_LOG_WARN("scripts.ai", "BossAI::_DespawnAtEvade: called on a temporary summon (who: {})", who->GetGUID().ToString());
        whoSummon->UnSummon();
        return;
    }

    who->DespawnOrUnsummon(0s, delayToRespawn);

    if (instance && who == me)
        instance->SetBossState(_bossId, FAIL);
}

void BossAI::_KilledUnit(Unit* victim)
{
    if (victim->IsPlayer())
        Talk(TALK_DEFAULT_BOSS_KILLED);
}

// WorldBossAI - for non-instanced bosses
WorldBossAI::WorldBossAI(Creature* creature) : ScriptedAI(creature), summons(creature) { }

void WorldBossAI::_Reset()
{
    if (!me->IsAlive())
        return;

    events.Reset();
    summons.DespawnAll();
}

void WorldBossAI::_JustDied()
{
    events.Reset();
    summons.DespawnAll();
}

void WorldBossAI::_JustEngagedWith()
{
    Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 0.0f, true);
    if (target)
        AttackStart(target);
}

void WorldBossAI::JustSummoned(Creature* summon)
{
    summons.Summon(summon);
    Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 0.0f, true);
    if (target)
        summon->AI()->AttackStart(target);
}

void WorldBossAI::SummonedCreatureDespawn(Creature* summon)
{
    summons.Despawn(summon);
}

void WorldBossAI::UpdateAI(uint32 diff)
{
    if (!UpdateVictim())
        return;

    events.Update(diff);

    if (me->HasUnitState(UNIT_STATE_CASTING))
        return;

    while (uint32 eventId = events.ExecuteEvent())
    {
        ExecuteEvent(eventId);
        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;
    }

    DoMeleeAttackIfReady();
}



// DekkCore >
void GetPositionWithDistInOrientation(Position* pUnit, float dist, float orientation, float& x, float& y)
{
    x = pUnit->GetPositionX() + (dist * cos(orientation));
    y = pUnit->GetPositionY() + (dist * sin(orientation));
}

void GetPositionWithDistInOrientation(Position* fromPos, float dist, float orientation, Position& movePosition)
{
    float x = 0.0f;
    float y = 0.0f;

    GetPositionWithDistInOrientation(fromPos, dist, orientation, x, y);

    movePosition.m_positionX = x;
    movePosition.m_positionY = y;
    movePosition.m_positionZ = fromPos->GetPositionZ();
}

void GetPositionWithDistInFront(Position* centerPos, float dist, Position& movePosition)
{
    GetPositionWithDistInOrientation(centerPos, dist, centerPos->GetOrientation(), movePosition);
}

void ScriptedAI::ApplyDefaultBossImmuneMask()
{
    static uint32 const placeholderSpellId = std::numeric_limits<uint32>::max();

    for (uint32 i = MECHANIC_NONE + 1; i < MAX_MECHANIC; ++i)
    {
        if (IMMUNE_TO_MOVEMENT_IMPAIRMENT_AND_LOSS_CONTROL_MASK & (1 << (i - 1)))
            me->ApplySpellImmune(placeholderSpellId, IMMUNITY_MECHANIC, i, true);
    }
}

bool ScriptedAI::CheckHomeDistToEvade(uint32 diff, float dist, float x, float y, float z, bool onlyZ)
{
    if (!me->IsInCombat())
        return false;

    bool evade = false;

    if (_checkHomeTimer <= diff)
    {
        _checkHomeTimer = 1500;

        if (onlyZ)
        {
            if ((me->GetPositionZ() > z + dist) || (me->GetPositionZ() < z - dist))
                evade = true;
        }
        else if (x != 0.0f || y != 0.0f || z != 0.0f)
        {
            if (me->GetDistance(x, y, z) >= dist)
                evade = true;
        }
        else if (me->GetDistance(me->GetHomePosition()) >= dist)
            evade = true;

        if (evade)
        {
            EnterEvadeMode();
            return true;
        }
    }
    else
    {
        _checkHomeTimer -= diff;
    }

    return false;
}

void ScriptedAI::GetInViewBotPlayers(std::list<Player*>& outPlayers, float range)
{
    Map::PlayerList const& players = me->GetMap()->GetPlayers();
    for (Map::PlayerList::const_iterator i = players.begin(); i != players.end(); ++i)
    {
        Player* player = i->GetSource();
        if (!player || !player->IsAlive() || !player->IsPlayerBot())
            continue;
        if (!me->InSamePhase(player->GetPhaseShift()))
            continue;
        if (me->GetDistance(player->GetPosition()) > range)
            continue;
        if (!me->IsWithinLOSInMap(player))
            continue;
        outPlayers.push_back(player);
    }
}

void ScriptedAI::SearchTargetPlayerAllGroup(std::list<Player*>& players, float range)
{
    if (range < 3.0f)
        return;
    ObjectGuid targetGUID = me->GetTarget();
    Player* targetPlayer = NULL;
    if (targetGUID == ObjectGuid::Empty)
    {
        std::list<Player*> playersNearby;
        me->GetPlayerListInGrid(playersNearby, range);
        if (playersNearby.empty())
            return;
        for (Player* p : playersNearby)
        {
            if (p->IsAlive() && p->GetMap() == me->GetMap())
            {
                targetPlayer = p;
                targetGUID = p->GetGUID();
                break;
            }
        }
    }
    if (!targetPlayer)
        targetPlayer = ObjectAccessor::FindPlayer(targetGUID);
    if (!targetPlayer || targetPlayer->GetMap() != me->GetMap())
        return;
    players.clear();
    players.push_back(targetPlayer);

    Group* pGroup = targetPlayer->GetGroup();
    if (!pGroup || pGroup->isBFGroup())
        return;
    Group::MemberSlotList const& memList = pGroup->GetMemberSlots();
    for (Group::MemberSlot const& slot : memList)
    {
        Player* player = ObjectAccessor::FindPlayer(slot.guid);
        if (!player || !player->IsAlive() || targetPlayer->GetMap() != player->GetMap() ||
            !player->IsInWorld() || player == targetPlayer || !player->IsPlayerBot())
            continue;
        if (me->GetDistance(player->GetPosition()) > range)
            continue;
        players.push_back(player);
    }
}

void ScriptedAI::PickBotPullMeToPosition(Position pullPos, ObjectGuid fliterTarget)
{
    std::list<BotGroupAI*> tankAIs;// , healAIs;
    std::list<Player*> targets;
    SearchTargetPlayerAllGroup(targets, 120);
    BotGroupAI* pNearTankAI = NULL;
    float nearTankAIDist = 999999;
    BotGroupAI* pNearHealAI[3] = { NULL };
    float nearHealAIDist[3] = { 999999 };
    for (Player* player : targets)
    {
        if (BotGroupAI* pGroupAI = dynamic_cast<BotGroupAI*>(player->GetAI()))
        {
            if (pGroupAI->IsHealerBotAI())
            {
                float thisDist = me->GetDistance(player->GetPosition());
                for (int i = 0; i < 3; i++)
                {
                    if (thisDist < nearHealAIDist[i])
                    {
                        pNearHealAI[i] = pGroupAI;
                        nearHealAIDist[i] = thisDist;
                        break;
                    }
                }
            }
            else if (pGroupAI->IsTankBotAI())
            {
                if (fliterTarget != ObjectGuid::Empty)
                {
                    Unit* tankTarget = player->GetSelectedUnit();
                    if (tankTarget != NULL && tankTarget->GetGUID() == fliterTarget)
                    {
                        if (pNearTankAI == NULL)
                        {
                            if (urand(0, 99) > 50)
                                continue;
                        }
                        else
                            continue;
                    }
                }
                tankAIs.push_back(pGroupAI);
                float thisDist = me->GetDistance(player->GetPosition());
                if (thisDist < nearTankAIDist)
                {
                    pNearTankAI = pGroupAI;
                    nearTankAIDist = thisDist;
                }
            }
        }
    }
    if (pNearTankAI == NULL)
        return;
    for (BotGroupAI* pGroupAI : tankAIs)
    {
        if (pGroupAI == pNearTankAI)
        {
            pGroupAI->ClearTankTarget();
            pGroupAI->AddTankTarget(me);
            pGroupAI->SetTankPosition(pullPos);
            pGroupAI->GetAIPayer()->SetSelection(me->GetGUID());
            for (int i = 0; i < 3; i++)
            {
                if (pNearHealAI[i])
                    pNearHealAI[i]->GetAIPayer()->SetSelection(me->GetGUID());
            }
        }
        else
        {
            pGroupAI->ClearTankTarget();
            pGroupAI->GetAIPayer()->SetSelection(ObjectGuid::Empty);
        }
    }
}

bool ScriptedAI::ExistPlayerBotByRange(float range)
{
    std::list<Player*> targets;
    SearchTargetPlayerAllGroup(targets, range);
    return targets.size() > 0;
}

void ScriptedAI::BotBlockCastingMe()
{
    std::list<Player*> targets;
    SearchTargetPlayerAllGroup(targets, 120);
    for (Player* player : targets)
    {
        if (player->HasUnitState(UNIT_STATE_CASTING))
            continue;
        if (BotGroupAI* pGroupAI = dynamic_cast<BotGroupAI*>(player->GetAI()))
        {
            if (pGroupAI->TryBlockCastingByTarget(me))
                return;
        }
    }
}

void ScriptedAI::ClearBotMeTarget(bool all)
{
    std::list<Player*> targets;
    SearchTargetPlayerAllGroup(targets, 120);
    for (Player* player : targets)
    {
        if (Pet* pPet = player->GetPet())
        {
            if (pPet->GetVictim() == me)
            {
                if (WorldSession* pSession = player->GetSession())
                {
                    pSession->HandlePetActionHelper(pPet, pPet->GetGUID(), 1, 7, ObjectGuid::Empty, 0);
                }
            }
        }
        if (BotGroupAI* pGroupAI = dynamic_cast<BotGroupAI*>(player->GetAI()))
        {
            if (all || !pGroupAI->IsTankBotAI())
            {
                player->SetSelection(ObjectGuid::Empty);
                pGroupAI->ToggleFliterCreature(me, true);
            }
        }
    }
}

void ScriptedAI::BotAllMovetoFarByDistance(Unit* pUnit, float range, float dist, float offset)
{
    float onceAngle = float(M_PI) * 2.0f / 8.0f;
    std::list<Position> allPosition;
    for (float angle = 0.0f; angle < (float(M_PI) * 2.0f); angle += onceAngle)
    {
        Position pos;
        pUnit->GetFirstCollisionPosition(dist, angle);
        float dist = pUnit->GetDistance(pos);
        if (!pUnit->IsWithinLOS(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ()))
            continue;
        allPosition.push_back(pos);
    }
    if (allPosition.empty())
        return;
    Position targetPos;
    float maxDist = 0.0f;
    for (Position pos : allPosition)
    {
        float distance = pUnit->GetDistance(pos);
        if (distance > maxDist)
        {
            maxDist = distance;
            targetPos = pos;
        }
    }
    if (maxDist < dist * 0.1f)
        return;
    targetPos.m_positionZ = pUnit->GetMap()->GetHeight(pUnit->GetPhaseShift(), targetPos.GetPositionX(), targetPos.GetPositionY(), targetPos.GetPositionZ());
    std::list<Player*> targets;
    SearchTargetPlayerAllGroup(targets, range);
    for (Player* player : targets)
    {
        if (BotGroupAI* pGroupAI = dynamic_cast<BotGroupAI*>(player->GetAI()))
        {
            Position offsetPos = Position(targetPos.GetPositionX() + frand(-offset, offset),
                targetPos.GetPositionY() + frand(-offset, offset), targetPos.GetPositionZ());
            pGroupAI->ClearCruxMovement();
            pGroupAI->SetCruxMovement(targetPos);
        }
    }
}

void ScriptedAI::BotCruxFlee(uint32 durTime, ObjectGuid fliter)
{
    std::list<Player*> targets;
    SearchTargetPlayerAllGroup(targets, 120);
    for (Player* player : targets)
    {
        if (fliter != ObjectGuid::Empty)
        {
            if (player->GetGUID() == fliter)
                continue;
        }
        if (BotGroupAI* pGroupAI = dynamic_cast<BotGroupAI*>(player->GetAI()))
        {
            pGroupAI->AddCruxFlee(durTime, me);
        }
    }
}

void ScriptedAI::BotRndCruxMovement(float dist)
{
    if (dist < 1.0f)
        return;
    std::list<Player*> playersNearby;
    me->GetPlayerListInGrid(playersNearby, 120);
    if (playersNearby.empty())
        return;
    for (Player* player : playersNearby)
    {
        if (BotGroupAI* pGroupAI = dynamic_cast<BotGroupAI*>(player->GetAI()))
        {
            pGroupAI->RndCruxMovement(dist);
        }
    }
}

void ScriptedAI::BotCruxFleeByRange(float range)
{
    if (range < 3.0f)
        return;
    std::list<Player*> targets;
    SearchTargetPlayerAllGroup(targets, range);
    for (Player* player : targets)
    {
        if (BotGroupAI* pGroupAI = dynamic_cast<BotGroupAI*>(player->GetAI()))
        {
            Position targetPos;
            if (!BotUtility::FindFirstCollisionPosition(player, range, me, targetPos))
                continue;
            if (pGroupAI->GetAIPayer()->HasUnitState(UNIT_STATE_CASTING))
            	pGroupAI->GetAIPayer()->CastStop();
            pGroupAI->SetCruxMovement(targetPos);
            player->SetSelection(ObjectGuid::Empty);
        }
    }
}

void ScriptedAI::BotCruxFleeByRange(float range, Unit* pCenter)
{
    if (range < 3.0f || !pCenter)
        return;
    std::list<Player*> targets;
    SearchTargetPlayerAllGroup(targets, range);
    for (Player* player : targets)
    {
        if (BotGroupAI* pGroupAI = dynamic_cast<BotGroupAI*>(player->GetAI()))
        {
            Position targetPos;
            if (!BotUtility::FindFirstCollisionPosition(player, range, pCenter, targetPos))
                continue;
            pGroupAI->SetCruxMovement(targetPos);
            //player->SetSelection(ObjectGuid::Empty);
        }
    }
}

void ScriptedAI::BotCruxFleeByArea(float range, float fleeDist, Unit* pCenter)
{
    if (range < 3.0f || fleeDist < 3.0f || !pCenter)
        return;
    Position centerPos = pCenter->GetPosition();
    std::list<Player*> players;
    SearchTargetPlayerAllGroup(players, 80);
    for (Player* player : players)
    {
        if (player->GetDistance(centerPos) > range)
            continue;
        if (BotGroupAI* pGroupAI = dynamic_cast<BotGroupAI*>(player->GetAI()))
        {
            Position targetPos;
            if (!BotUtility::FindFirstCollisionPosition(player, fleeDist, pCenter, targetPos))
                continue;
            pGroupAI->SetCruxMovement(targetPos);
            //player->SetSelection(ObjectGuid::Empty);
        }
    }
}

void ScriptedAI::BotAllTargetMe(bool all)
{
    std::list<Player*> targets;
    SearchTargetPlayerAllGroup(targets, 120);
    for (Player* player : targets)
    {
        if (BotGroupAI* pGroupAI = dynamic_cast<BotGroupAI*>(player->GetAI()))
        {
            if (all)
                player->SetSelection(me->GetGUID());
            else if (!pGroupAI->IsTankBotAI() && !pGroupAI->IsHealerBotAI())
                player->SetSelection(me->GetGUID());
        }
    }
}

void ScriptedAI::BotPhysicsDPSTargetMe(Unit* pUnit)
{
    std::list<Player*> targets;
    SearchTargetPlayerAllGroup(targets, 120);
    for (Player* player : targets)
    {
        if (!player->IsPlayerBot())
            continue;
        if (player->GetClass() == Classes::CLASS_ROGUE || player->GetClass() == Classes::CLASS_HUNTER)
            player->SetSelection(me->GetGUID());
        /*else if (player->GetClass() == Classes::CLASS_WARRIOR)
        {
            if (player->FindTalentType() != 2)
                player->SetSelection(pUnit->GetGUID());
        }*/
        else
        {
            if (player->GetSelectedUnit() == pUnit)
            {
                if (BotGroupAI* pGroupAI = dynamic_cast<BotGroupAI*>(player->GetAI()))
                {
                    if (!pGroupAI->IsHealerBotAI() && !pGroupAI->IsTankBotAI())
                        player->SetSelection(ObjectGuid::Empty);
                }
            }
        }
    }
}

void ScriptedAI::BotMagicDPSTargetMe(Unit* pUnit)
{
    std::list<Player*> targets;
    SearchTargetPlayerAllGroup(targets, 120);
    for (Player* player : targets)
    {
        if (!player->IsPlayerBot())
            continue;
        if (player->GetClass() == Classes::CLASS_ROGUE || player->GetClass() == Classes::CLASS_HUNTER ||
            player->GetClass() == Classes::CLASS_WARRIOR)
        {
            if (player->GetSelectedUnit() == pUnit)
            {
                if (BotGroupAI* pGroupAI = dynamic_cast<BotGroupAI*>(player->GetAI()))
                {
                    if (!pGroupAI->IsHealerBotAI() && !pGroupAI->IsTankBotAI())
                        player->SetSelection(ObjectGuid::Empty);
                }
            }
        }
        else
        {
            if (BotGroupAI* pGroupAI = dynamic_cast<BotGroupAI*>(player->GetAI()))
            {
                if (!pGroupAI->IsHealerBotAI() && !pGroupAI->IsTankBotAI())
                    player->SetSelection(pUnit->GetGUID());
            }
        }
    }
}

void ScriptedAI::BotAverageCreatureTarget(std::vector<Creature*>& targets, float searchRange)
{
    if (targets.empty() || searchRange < 3.0f)
        return;
    std::queue<Player*> players;
    std::list<Player*> searchUnits;
    SearchTargetPlayerAllGroup(searchUnits, searchRange);
    for (Player* player : searchUnits)
    {
        players.push(player);
    }
    while (!players.empty())
    {
        for (Creature* pCreature : targets)
        {
            Player* player = players.front();
            players.pop();
            if (pCreature)
                player->SetSelection(pCreature->GetGUID());
            if (players.empty())
                break;
        }
    }
}

void ScriptedAI::BotAllotCreatureTarget(std::vector<Creature*>& targets, float searchRange, uint32 onceCount)
{
    if (onceCount < 1 || targets.empty() || searchRange < 3.0f)
        return;
    std::queue<Player*> players;
    std::list<Player*> searchUnits;
    SearchTargetPlayerAllGroup(searchUnits, searchRange);
    for (Player* player : searchUnits)
    {
        players.push(player);
    }
    while (!players.empty())
    {
        for (Creature* pCreature : targets)
        {
            int32 allot = int32(onceCount);
            while (!players.empty() && allot > 0)
            {
                --allot;
                Player* player = players.front();
                players.pop();
                if (pCreature)
                    player->SetSelection(pCreature->GetGUID());
            }
            if (players.empty())
                break;
        }
    }
}

void ScriptedAI::BotAllToSelectionTarget(Unit* pUnit, float searchRange, bool all)
{
    if (!pUnit)
        return;
    ObjectGuid guid = pUnit->GetGUID();
    std::list<Player*> searchUnits;
    SearchTargetPlayerAllGroup(searchUnits, searchRange);
    for (Player* player : searchUnits)
    {
        if (player->GetTarget() == guid)
            continue;
        if (all)
            player->SetSelection(pUnit->GetGUID());
        else if (BotGroupAI* pAI = dynamic_cast<BotGroupAI*>(player->GetAI()))
        {
            if (!pAI->IsTankBotAI())
                player->SetSelection(pUnit->GetGUID());
        }
    }
}

void ScriptedAI::BotAllFullDispel(bool enables)
{
    std::list<Player*> targets;
    SearchTargetPlayerAllGroup(targets, 120);
    for (Player* player : targets)
    {
        if (BotGroupAI* pGroupAI = dynamic_cast<BotGroupAI*>(player->GetAI()))
        {
            if (enables)
                pGroupAI->StartFullDispel();
            else
                pGroupAI->ClearFullDispel();
        }
    }
}

void ScriptedAI::BotAllFullDispelByDecPoison(bool enables)
{
    std::list<Player*> targets;
    SearchTargetPlayerAllGroup(targets, 120);
    for (Player* player : targets)
    {
        if (player->GetClass() != Classes::CLASS_DRUID && player->GetClass() != Classes::CLASS_SHAMAN)
            continue;
        if (BotGroupAI* pGroupAI = dynamic_cast<BotGroupAI*>(player->GetAI()))
        {
            if (enables)
                pGroupAI->StartFullDispel();
            else
                pGroupAI->ClearFullDispel();
        }
    }
}

void ScriptedAI::BotFleeLineByAngle(Unit* center, float angle, bool force)
{
    angle = Position::NormalizeOrientation(angle);
    std::list<Player*> playersNearby;
    center->GetPlayerListInGrid(playersNearby, center->GetObjectScale() + 80.0f);
    for (Player* player : playersNearby)
    {
        if (!player->IsPlayerBot() || !player->IsInWorld() || player->GetMap() != center->GetMap() || !player->IsAlive())
            continue;
        float fleeRange = center->GetDistance(player->GetPosition());
        if (fleeRange <= 0)
            fleeRange = center->GetObjectScale() + 1.0f;
        float pangle = center->GetAbsoluteAngle(player->GetPosition()) - angle;
        if (pangle >= 0 && pangle <= float(M_PI_4))
        {
            if (BotGroupAI* pGroupAI = dynamic_cast<BotGroupAI*>(player->GetAI()))
            {
                float fleeAngle = Position::NormalizeOrientation(angle + float(M_PI_4));
                Position fleePos = Position(center->GetPositionX() + fleeRange * std::cosf(fleeAngle),
                    center->GetPositionY() + fleeRange * std::sinf(fleeAngle), player->GetPositionZ(), player->GetOrientation());
                fleePos.m_positionZ = player->GetMap()->GetHeight(player->GetPhaseShift(), fleePos.GetPositionX(), fleePos.GetPositionY(), fleePos.GetPositionZ());
                //if (pGroupAI->GetAIPayer()->HasUnitState(UNIT_STATE_CASTING))
                //	pGroupAI->GetAIPayer()->CastStop();
                if (force)
                    pGroupAI->ClearCruxMovement();
                pGroupAI->SetCruxMovement(fleePos);
            }
        }
        else if (pangle < 0 && pangle >= float(-M_PI_4))
        {
            if (BotGroupAI* pGroupAI = dynamic_cast<BotGroupAI*>(player->GetAI()))
            {
                float fleeAngle = Position::NormalizeOrientation(angle - float(M_PI_4));
                Position fleePos = Position(center->GetPositionX() + fleeRange * std::cosf(fleeAngle),
                    center->GetPositionY() + fleeRange * std::sinf(fleeAngle), player->GetPositionZ(), player->GetOrientation());
                fleePos.m_positionZ = player->GetMap()->GetHeight(player->GetPhaseShift(), fleePos.GetPositionX(), fleePos.GetPositionY(), fleePos.GetPositionZ());
                //if (pGroupAI->GetAIPayer()->HasUnitState(UNIT_STATE_CASTING))
                //	pGroupAI->GetAIPayer()->CastStop();
                if (force)
                    pGroupAI->ClearCruxMovement();
                pGroupAI->SetCruxMovement(fleePos);
            }
        }
    }
}

void ScriptedAI::BotSwitchPullTarget(Unit* pTarget)
{
    if (!pTarget || !pTarget->ToCreature())
        return;
    Player* pTargetPlayer = ObjectAccessor::FindPlayer(pTarget->GetTarget());
    if (pTargetPlayer && pTargetPlayer->IsAlive())
    {
        pTargetPlayer->SetSelection(ObjectGuid::Empty);
        if (BotGroupAI* pGroupAI = dynamic_cast<BotGroupAI*>(pTargetPlayer->GetAI()))
            pGroupAI->ClearTankTarget();
    }
    std::list<Player*> playersNearby;
    pTarget->GetPlayerListInGrid(playersNearby, pTarget->GetObjectScale() + 80.0f);
    for (Player* player : playersNearby)
    {
        if (player && player == pTargetPlayer)
            continue;
        if (!player->IsPlayerBot() || !player->IsInWorld() || player->GetMap() != pTarget->GetMap() || !player->IsAlive())
            continue;
        if (BotGroupAI* pGroupAI = dynamic_cast<BotGroupAI*>(player->GetAI()))
        {
            if (pGroupAI->IsTankBotAI())
            {
                if (pGroupAI->ProcessPullSpell(pTarget))
                    return;
            }
        }
    }
}

void ScriptedAI::BotVehicleChaseTarget(Unit* pTarget, float distance)
{
    if (!pTarget || !pTarget->ToCreature())
        return;
    std::list<Player*> playersNearby;
    me->GetPlayerListInGrid(playersNearby, distance * 2);
    if (playersNearby.empty())
        return;
    for (Player* bot : playersNearby)
    {
        if (!bot->IsPlayerBot() || !bot->IsAlive() || bot->GetMap() != pTarget->GetMap())
            continue;
        if (bot->GetTarget() != pTarget->GetGUID())
            bot->SetSelection(pTarget->GetGUID());
        if (BotGroupAI* pBotAI = dynamic_cast<BotGroupAI*>(bot->GetAI()))
        {
            pBotAI->SetVehicle3DNextMoveGap(8.0f);
            pBotAI->SetVehicle3DMoveTarget(pTarget, distance);
        }
        Unit* vehBase = bot->GetCharmed();
        if (!vehBase || !vehBase->IsAlive() || vehBase->GetMap() != pTarget->GetMap())
            continue;
        if (vehBase->GetTarget() != pTarget->GetGUID())
            vehBase->SetTarget(pTarget->GetGUID());
        if (vehBase->HasSpell(57092) && !vehBase->HasAura(57092))
            vehBase->CastSpell(vehBase, 57092);
        float power = (float)vehBase->GetPower(POWER_ENERGY) / (float)vehBase->GetMaxPower(POWER_ENERGY);
        if (power >= 0.4f)
        {
            uint8 combo = bot->GetComboPoints();
            if (combo > 4)
            {
                if (vehBase->GetHealthPct() < 75 && urand(0, 99) > 60)
                {
                    if (vehBase->HasSpell(57108) && !vehBase->HasAura(57108) && urand(0, 99) > 60)
                        vehBase->CastSpell(vehBase, 57108);
                    else if (vehBase->HasSpell(57143))
                        vehBase->CastSpell(vehBase, 57143);
                }
                else if (vehBase->HasSpell(56092))
                    vehBase->CastSpell(pTarget, 56092);
            }
            else
            {
                if (vehBase->GetHealthPct() < 75 && vehBase->HasSpell(57090) && urand(0, 99) > 60)
                    vehBase->CastSpell(vehBase, 57090);
                else if (vehBase->HasSpell(56091))
                    vehBase->CastSpell(pTarget, 56091);
            }
        }
    }
}

void ScriptedAI::BotUseGOTarget(GameObject* pGO)
{
    if (!pGO)
        return;
    std::list<Player*> playersNearby;
    me->GetPlayerListInGrid(playersNearby, 100);
    if (playersNearby.empty())
        return;
    ObjectGuid goGUID = pGO->GetGUID();
    std::map<float, Player*> distPlayers;
    for (Player* bot : playersNearby)
    {
        if (!bot->IsPlayerBot() || !bot->IsAlive() || bot->GetMap() != pGO->GetMap())
            continue;
        if (bot->HasUnitState(UNIT_STATE_CASTING))
            continue;
        distPlayers[bot->GetDistance(pGO->GetPosition())] = bot;
    }
    for (std::map<float, Player*>::iterator itDist = distPlayers.begin();
        itDist != distPlayers.end();
        itDist++)
    {
        Player* bot = itDist->second;
        if (BotGroupAI* pAI = dynamic_cast<BotGroupAI*>(bot->GetAI()))
        {
            if (pAI->SetMovetoUseGOTarget(goGUID))
                return;
        }
    }
}

bool NeedBotAttackCreature::UpdateProcess(std::list<ObjectGuid>& freeBots)
{
    Creature* pCreature = atMap->GetCreature(needCreature);
    if (!pCreature || !pCreature->IsAlive())
    {
        allUsedBots.clear();
        return false;
    }
    if (pCreature && !pCreature->IsVisible())
    {
        allUsedBots.clear();
        return true;
    }

    while (int32(allUsedBots.size()) < needCount)
    {
        if (freeBots.empty())
            break;
        allUsedBots.push_back(*freeBots.begin());
        freeBots.erase(freeBots.begin());
    }
    std::list<std::list<ObjectGuid>::iterator > needClearBot;
    for (std::list<ObjectGuid>::iterator itBot = allUsedBots.begin(); itBot != allUsedBots.end(); itBot++)
    {
        ObjectGuid& guid = *itBot;
        Player* player = ObjectAccessor::FindPlayer(guid);
        if (!player || !player->IsAlive() || player->GetMap() != atMap)
            needClearBot.push_back(itBot);
        else if (player->GetDistance(pCreature->GetPosition()) < 120)
            player->SetSelection(needCreature);
    }
    for (std::list<ObjectGuid>::iterator itClear : needClearBot)
    {
        allUsedBots.erase(itClear);
    }
    return true;
}

void BotAttackCreature::UpdateNeedAttackCreatures(uint32 diff, ScriptedAI* affiliateAI, bool attackMain)
{
    currentTick -= int32(diff);
    if (currentTick >= 0)
        return;
    currentTick = updateGap;
    if (!mainCreature)
        return;

    if (!affiliateAI)
        return;
    if (allNeedCreatures.empty())
        return;
    std::list<Player*> allBots;
    affiliateAI->SearchTargetPlayerAllGroup(allBots, 120);
    std::list<ObjectGuid> allBotGUIDs;
    for (Player* player : allBots)
    {
        ObjectGuid guid = player->GetGUID();
        bool canPush = true;
        for (NeedBotAttackCreature* pNeed : allNeedCreatures)
        {
            if (pNeed->IsThisUsedBot(guid))
            {
                canPush = false;
                break;
            }
        }
        if (canPush)
            allBotGUIDs.push_back(guid);
    }
    std::list<std::list<NeedBotAttackCreature*>::iterator > needClears;
    for (std::list<NeedBotAttackCreature*>::iterator itNeed = allNeedCreatures.begin(); itNeed != allNeedCreatures.end(); itNeed++)
    {
        NeedBotAttackCreature* pNeed = *itNeed;
        bool ing = pNeed->UpdateProcess(allBotGUIDs);
        if (!ing)
            needClears.push_back(itNeed);
    }
    for (std::list<NeedBotAttackCreature*>::iterator itClear : needClears)
    {
        NeedBotAttackCreature* pNeed = *itClear;
        delete pNeed;
        allNeedCreatures.erase(itClear);
    }
    if (attackMain && mainCreature && mainCreature->IsAlive())
    {
        for (ObjectGuid guid : allBotGUIDs)
        {
            Player* player = ObjectAccessor::FindPlayer(guid);
            if (player && player->IsAlive() && player->GetMap() == mainCreature->GetMap())
                player->SetSelection(mainCreature->GetGUID());
        }
    }
    else if (attackMain)
    {
        mainCreature = NULL;
    }
}

void BotAttackCreature::AddNewCreatureNeedAttack(Creature* pCreature, int32 needBotCount)
{
    if (!pCreature || !pCreature->IsAlive() || needBotCount < 0 || pCreature == mainCreature)
        return;
    Map* atMap = pCreature->GetMap();
    if (!atMap)
        return;
    ObjectGuid guid = pCreature->GetGUID();
    for (NeedBotAttackCreature* pNeed : allNeedCreatures)
    {
        if (pNeed->IsThisCreature(guid))
            return;
    }
    NeedBotAttackCreature* pNeedCreature = new NeedBotAttackCreature(atMap, needBotCount, guid);
    allNeedCreatures.push_back(pNeedCreature);
}
// < DekkCore
