/*
 * Copyright 2021
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

#include "AreaTriggerAI.h"
#include "CreatureTextMgr.h"
#include "DB2Stores.h"
#include "GameObject.h"
#include "GameObjectAI.h"
#include "GridNotifiers.h"
#include "Map.h"
#include "PhasingHandler.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedEscortAI.h"
#include "SpellScript.h"
#include "Player.h"
#include "Log.h"
#include "Vehicle.h"
#include "ObjectMgr.h"

#include "TemporarySummon.h"

enum TheWanderingIsleData
{
    QUEST_THE_WAY_OF_THE_TUSHUI = 29414,
    QUEST_SHU_THE_SPIRIT_OF_WATER = 29678,
    QUEST_A_NEW_FRIEND            = 29679,
    QUEST_THE_SOURCE_OF_OUR_LIVELIHOOD = 29680,
    QUEST_NEW_FATE = 31450,
    QUEST_PASSING_WISDOM = 29790,
    QUEST_SUF_SHUN_ZI = 29791,

    SPELL_MEDITATION_TIMER_BAR = 116421,
};

class at_cave_of_meditation : public AreaTriggerScript
{
public:
    at_cave_of_meditation() : AreaTriggerScript("at_cave_of_meditation") { }

    bool OnTrigger(Player* player, AreaTriggerEntry const* /*areaTrigger*/) override
    {
        if (player->IsAlive() && player->GetQuestStatus(QUEST_THE_WAY_OF_THE_TUSHUI) == QUEST_STATUS_INCOMPLETE)
        {
            if (!player->HasAura(SPELL_MEDITATION_TIMER_BAR))
                player->CastSpell(player, SPELL_MEDITATION_TIMER_BAR, true);
            else
                player->RemoveAura(SPELL_MEDITATION_TIMER_BAR);

            return true;
        }
        return false;
    }
};

class spell_summon_troublemaker : public SpellScript
{
    PrepareSpellScript(spell_summon_troublemaker);

    void HandleSummon(SpellEffIndex effIndex)
    {
        PreventHitDefaultEffect(effIndex);
        uint32 entry = GetSpellInfo()->GetEffect(effIndex).MiscValue;
        SummonPropertiesEntry const* properties = sSummonPropertiesStore.LookupEntry(GetSpellInfo()->GetEffect(effIndex).MiscValueB);
        int32 duration = GetSpellInfo()->GetDuration();

        if (!entry || !properties || !duration)
            return;

        Position pos = { 1181.75f, 3444.5f, 102.9385f, 3.285759f };
        int32 radius = urand(0, 4) == 0 ? 0 : urand(1, 7);
        float angle = M_PI + M_PI * (urand(0, 7) / 7.0f);
        pos.RelocateOffset({ radius * sin(angle), radius * cos(angle), 0.0f, 0.0f });

        if (TempSummon* summon = GetCaster()->GetMap()->SummonCreature(entry, pos, properties, duration, GetCaster()))
        {
            summon->SetTempSummonType(TEMPSUMMON_CORPSE_TIMED_DESPAWN);
            summon->SetDefaultMovementType(MovementGeneratorType::RANDOM_MOTION_TYPE);
        }
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_summon_troublemaker::HandleSummon, EFFECT_0, SPELL_EFFECT_SUMMON);
    }
};

enum MeditationTimerSpells
{
    SPELL_CAVE_OF_SCROLLS_CREDIT = 102447,
    SPELL_CAVE_OF_SCROLLS_COMP_TIMER_AURA = 128598
};

class spell_meditation_timer_bar : public AuraScript
{
    PrepareAuraScript(spell_meditation_timer_bar);

    void HandleEffectPeriodic(AuraEffect const* /*aurEff*/)
    {
        if (Unit* target = GetTarget())
        {
            target->ModifyPower(POWER_ALTERNATE_POWER, 1);

            if (target->GetPowerPct(POWER_ALTERNATE_POWER) == 100)
            {
                target->CastSpell(GetTarget(), SPELL_CAVE_OF_SCROLLS_CREDIT, true);
                target->CastSpell(GetTarget(), SPELL_CAVE_OF_SCROLLS_COMP_TIMER_AURA, true);
                target->RemoveAura(GetId());
                target->ClearInCombat();
            }
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_meditation_timer_bar::HandleEffectPeriodic, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
    }
};

enum CaveOfScrollsCompTimerAura
{
    NPC_AYSA_CLOUDSINGER_IN_CAVE = 54567,
    TEXT_SPEAK_TO_MASTER = 1
};

class spell_cave_of_scrolls_comp_timer_aura : public AuraScript
{
    PrepareAuraScript(spell_cave_of_scrolls_comp_timer_aura);

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* target = GetTarget())
            if (Creature* creature = target->FindNearestCreature(NPC_AYSA_CLOUDSINGER_IN_CAVE, target->GetVisibilityRange(), true))
                creature->AI()->Talk(TEXT_SPEAK_TO_MASTER, target);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_cave_of_scrolls_comp_timer_aura::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

class q_the_way_of_the_tushui : public QuestScript
{
public:
    q_the_way_of_the_tushui() : QuestScript("q_the_way_of_the_tushui") { }

    void OnQuestStatusChange(Player* player, Quest const* /*quest*/, QuestStatus /*oldStatus*/, QuestStatus newStatus) override
    {
        if (newStatus == QUEST_STATUS_NONE)
        {
            player->RemoveAura(SPELL_MEDITATION_TIMER_BAR);
            PhasingHandler::OnConditionChange(player);
        }
    }
};

class spell_summon_living_air : public SpellScript
{
    PrepareSpellScript(spell_summon_living_air);

    void HandleSummon(SpellEffIndex effIndex)
    {
        PreventHitDefaultEffect(effIndex);
        uint32 entry = GetSpellInfo()->GetEffect(effIndex).MiscValue;
        SummonPropertiesEntry const* properties = sSummonPropertiesStore.LookupEntry(GetSpellInfo()->GetEffect(effIndex).MiscValueB);
        int32 duration = GetSpellInfo()->GetDuration();

        if (!entry || !properties || !duration)
            return;

        Position const spawnPosition[6] =
        {
            { 1237.073f, 3697.739f, 93.62743f },
            { 1230.608f, 3701.063f, 93.94895f },
            { 1229.429f, 3732.776f, 92.22045f },
            { 1223.438f, 3700.607f, 93.93437f },
            { 1239.606f, 3732.907f, 94.10403f },
            { 1224.92f, 3727.201f, 92.4472f }
        };

        if (TempSummon* summon = GetCaster()->GetMap()->SummonCreature(entry, spawnPosition[urand(0, 5)], properties, duration, GetCaster()))
            summon->SetTempSummonType(TEMPSUMMON_DEAD_DESPAWN);
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_summon_living_air::HandleSummon, EFFECT_0, SPELL_EFFECT_SUMMON);
    }
};

enum QuestOnlyTheWorthyShallPassSpells
{
    SPELL_CANCEL_FIRE_CRASH_PHASE = 108153
};

class q_only_the_worthy_shall_pass : public QuestScript
{
public:
    q_only_the_worthy_shall_pass() : QuestScript("q_only_the_worthy_shall_pass") { }

    void OnQuestStatusChange(Player* player, Quest const* /*quest*/, QuestStatus /*oldStatus*/, QuestStatus newStatus) override
    {
        if (newStatus == QUEST_STATUS_COMPLETE)
        {
            player->CastSpell(player, SPELL_CANCEL_FIRE_CRASH_PHASE, true);
            PhasingHandler::OnConditionChange(player);
        }
    }
};

enum FanTheFlamesSpells
{
    SPELL_THROW_WOOD = 109090,
    SPELL_BLOW_AIR = 109095,
    SPELL_BLOW_AIR_BIG = 109105,
    SPELL_BLOW_AIR_BIGGER = 109109
};

enum FanTheFlamesNPCs
{
    NPC_HUO = 57779
};

class spell_fan_the_flames : public SpellScriptLoader
{
public:
    spell_fan_the_flames() : SpellScriptLoader("spell_fan_the_flames") { }

    class spell_fan_the_flames_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_fan_the_flames_AuraScript);

        uint32 counter = 0;

        void HandlePeriodic(AuraEffect const* /*aurEff*/)
        {
            Unit* caster = GetCaster();

            counter++;

            if (Unit* target = GetTarget()->FindNearestCreature(NPC_HUO, GetSpellInfo()->GetMaxRange(), true))
            {
                if (counter == 1 || counter == 4 || counter == 6)
                    caster->CastSpell(target, SPELL_THROW_WOOD, true);
                if (counter == 8 || counter == 10 || counter == 13 || counter == 15)
                    caster->CastSpell(target, SPELL_BLOW_AIR, true);
                if (counter == 12 || counter == 14)
                    caster->CastSpell(target, SPELL_BLOW_AIR_BIG, true);
                if (counter == 16)
                    caster->CastSpell(target, SPELL_BLOW_AIR_BIGGER, true);
            }
        }

        void Register() override
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_fan_the_flames_AuraScript::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_fan_the_flames_AuraScript();
    }
};

enum QuestPassionOfShenZinSuSpells
{
    SPELL_DESPAWN_FIRE_SPIRIT = 109178,
    SPELL_SUMMON_FIRE_SPIRIT = 128700,
    SPELL_SUMMON_FIRE_SPIRIT_AFTER_RELOG = 102632
};

class q_passion_of_shenzin_su : public QuestScript
{
public:
    q_passion_of_shenzin_su() : QuestScript("q_passion_of_shenzin_su") { }

    void OnQuestStatusChange(Player* player, Quest const* /*quest*/, QuestStatus /*oldStatus*/, QuestStatus newStatus) override
    {
        if (newStatus == QUEST_STATUS_NONE)
        {
            player->CastSpell(player, SPELL_DESPAWN_FIRE_SPIRIT, true);
            player->RemoveAura(SPELL_SUMMON_FIRE_SPIRIT);
            player->RemoveAura(SPELL_SUMMON_FIRE_SPIRIT_AFTER_RELOG);
            PhasingHandler::OnConditionChange(player);
        }
    }
};

enum QuestDisciplesChallengeSpells
{
    SPELL_GENERIC_QUEST_INVISIBILITY_DETECTION = 60922
};

class q_disciples_challenge : public QuestScript
{
public:
    q_disciples_challenge() : QuestScript("q_disciples_challenge") { }

    void OnQuestStatusChange(Player* player, Quest const* /*quest*/, QuestStatus /*oldStatus*/, QuestStatus newStatus) override
    {
        if (newStatus == QUEST_STATUS_COMPLETE)
        {
            player->AddAura(SPELL_GENERIC_QUEST_INVISIBILITY_DETECTION, player);
        }

        if (newStatus == QUEST_STATUS_FAILED)
        {
            player->RemoveAura(SPELL_GENERIC_QUEST_INVISIBILITY_DETECTION);
        }
    }
};

enum SingingPoolsATSpells
{
    SPELL_CURSE_OF_THE_FROG = 102938,
    SPELL_CURSE_OF_THE_SKUNK = 102939,
    SPELL_CURSE_OF_THE_TURTLE = 102940,
    SPELL_CURSE_OF_THE_CRANE = 102941,
    SPELL_CURSE_OF_THE_CROCODILE = 102942,
    SPELL_RIDE_VEHICLE_POLE = 102717,
    SPELL_RIDE_VEHICLE_BELL_POLE = 107049,
    SPELL_TRAINING_BELL_EXCLUSION_AURA = 133381
};

class at_singing_pools_transform : public AreaTriggerScript
{
public:
    at_singing_pools_transform() : AreaTriggerScript("at_singing_pools_transform") { }

    bool OnTrigger(Player* player, AreaTriggerEntry const* areaTrigger) override
    {
        if (player->IsAlive() && !player->HasAura(SPELL_RIDE_VEHICLE_POLE))
            {
                switch (areaTrigger->ID)
                {
                case 6986:
                case 6987:
                    if (!player->HasAura(SPELL_CURSE_OF_THE_FROG))
                        player->CastSpell(player, SPELL_CURSE_OF_THE_FROG, true);
                    if (player->HasAura(SPELL_TRAINING_BELL_EXCLUSION_AURA))
                        player->RemoveAura(SPELL_TRAINING_BELL_EXCLUSION_AURA);
                    break;
                case 6988:
                case 6989:
                    if (!player->HasAura(SPELL_CURSE_OF_THE_SKUNK))
                        player->CastSpell(player, SPELL_CURSE_OF_THE_SKUNK, true);
                    break;
                case 6990:
                    if (!player->HasAura(SPELL_CURSE_OF_THE_CROCODILE))
                        player->CastSpell(player, SPELL_CURSE_OF_THE_CROCODILE, true);
                    break;
                case 6991:
                case 6992:
                    if (!player->HasAura(SPELL_CURSE_OF_THE_CRANE))
                        player->CastSpell(player, SPELL_CURSE_OF_THE_CRANE, true);
                    break;
                case 7011:
                case 7012:
                    if (!player->HasAura(SPELL_CURSE_OF_THE_TURTLE))
                        player->CastSpell(player, SPELL_CURSE_OF_THE_TURTLE, true);
                    break;
                }
            }
            else
            {
                switch (areaTrigger->ID)
                {
                case 6986:
                case 6987:
                    if (!player->IsInAreaTriggerRadius(sAreaTriggerStore.LookupEntry(6986)) && !player->IsInAreaTriggerRadius(sAreaTriggerStore.LookupEntry(6987)))
                        if (player->HasAura(SPELL_CURSE_OF_THE_FROG))
                            player->RemoveAura(SPELL_CURSE_OF_THE_FROG);
                    break;
                case 6988:
                case 6989:
                    if (!player->IsInAreaTriggerRadius(sAreaTriggerStore.LookupEntry(6988)) && !player->IsInAreaTriggerRadius(sAreaTriggerStore.LookupEntry(6989)))
                        if (player->HasAura(SPELL_CURSE_OF_THE_SKUNK))
                            player->RemoveAura(SPELL_CURSE_OF_THE_SKUNK);
                    break;
                case 6990:
                    if (player->HasAura(SPELL_CURSE_OF_THE_CROCODILE))
                        player->RemoveAura(SPELL_CURSE_OF_THE_CROCODILE);
                    break;
                case 6991:
                case 6992:
                    if (!player->IsInAreaTriggerRadius(sAreaTriggerStore.LookupEntry(6991)) && !player->IsInAreaTriggerRadius(sAreaTriggerStore.LookupEntry(6992)))
                        if (player->HasAura(SPELL_CURSE_OF_THE_CRANE))
                            player->RemoveAura(SPELL_CURSE_OF_THE_CRANE);
                    break;
                case 7011:
                case 7012:
                    if (!player->IsInAreaTriggerRadius(sAreaTriggerStore.LookupEntry(7011)) && !player->IsInAreaTriggerRadius(sAreaTriggerStore.LookupEntry(7012)))
                        if (player->HasAura(SPELL_CURSE_OF_THE_TURTLE))
                            player->RemoveAura(SPELL_CURSE_OF_THE_TURTLE);
                    break;
                }
            }
            return true;
    }
};

enum BalancePoleEvents
{
    EVENT_CAST_TRANSFORM = 1
};

enum BalancePoleNPCs
{
    NPC_BALANCE_POLE_1 = 54993,
    NPC_BALANCE_POLE_2 = 57431,
    NPC_TRAINING_BELL_BALANCE_POLE = 55083,
    NPC_CURSED_POOL_CONTROLLER = 55123
};

enum BalancePoleSpells
{
    SPELL_MONK_RIDE_POLE = 103030,
    SPELL_TRAINING_BELL_FORCECAST_RIDE_VEHICLE = 107050,
    SPELL_TRAINING_BELL_RIDE_VEHICLE = 107049
};

class npc_balance_pole : public CreatureScript
{
public:
    npc_balance_pole() : CreatureScript("npc_balance_pole") { }

    struct npc_balance_poleAI : public ScriptedAI
    {
        npc_balance_poleAI(Creature* creature) : ScriptedAI(creature) {
            _passengerGuid.Clear();
        }

        void PassengerBoarded(Unit* passenger, int8 /*seat*/, bool apply) override
        {
            if (passenger->GetTypeId() == TYPEID_PLAYER)
            {
                _passengerGuid = passenger->GetGUID();

                if (!apply)
                    _events.ScheduleEvent(EVENT_CAST_TRANSFORM, 1s);
                else
                {
                    if (me->GetEntry() == NPC_TRAINING_BELL_BALANCE_POLE)
                        DoCast(passenger, SPELL_TRAINING_BELL_FORCECAST_RIDE_VEHICLE, true);
                }
            }
        }

        void UpdateAI(uint32 diff) override
        {
            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_CAST_TRANSFORM:
                    // Transform is casted only when in frog pool
                    Unit* passenger = ObjectAccessor::GetUnit(*me, _passengerGuid);
                    if (passenger->GetPositionZ() > 116.0f && !passenger->HasAura(SPELL_TRAINING_BELL_RIDE_VEHICLE) && !passenger->HasAura(SPELL_RIDE_VEHICLE_POLE))
                    {
                        passenger->CastSpell(passenger, SPELL_CURSE_OF_THE_FROG, true);

                        if (passenger->HasAura(SPELL_TRAINING_BELL_EXCLUSION_AURA))
                            passenger->RemoveAura(SPELL_TRAINING_BELL_EXCLUSION_AURA);
                    }
                    break;
                }
            }
        }

    private:
        EventMap _events;
        ObjectGuid _passengerGuid;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_balance_poleAI(creature);
    }
};

enum TushuiMonkOnPoleEvents
{
    EVENT_THROW_ROCK = 1,
    EVENT_SWITCH_POLE = 2,
    EVENT_DESPAWN = 3
};

enum TushuiMonkOnPoleNPCs
{
    NPC_MONK_ON_POLE_1 = 55019,
    NPC_MONK_ON_POLE_2 = 65468,
};

enum TushuiMonkOnPoleSpells
{
    SPELL_FORCECAST_RIDE_POLE = 103031,
    SPELL_THROW_ROCK = 109308
};

enum TushuiMonkOnPoleMisc
{
    QUEST_LESSON_OF_BALANCED_ROCK = 29663
};

class npc_tushui_monk_on_pole : public CreatureScript
{
public:
    npc_tushui_monk_on_pole() : CreatureScript("npc_tushui_monk_on_pole") { }

    struct npc_tushui_monk_on_poleAI : public ScriptedAI
    {
        npc_tushui_monk_on_poleAI(Creature* creature) : ScriptedAI(creature) { }

        void Reset() override
        {
            _events.Reset();
            _events.ScheduleEvent(EVENT_SWITCH_POLE, 0s);
            me->RestoreFaction();
            me->SetReactState(REACT_DEFENSIVE);
        }

        void SpellHit(WorldObject* caster, SpellInfo const* spell) override
        {
            if (spell->Id == SPELL_FORCECAST_RIDE_POLE)
                DoCast(caster->ToUnit(), SPELL_MONK_RIDE_POLE, true);
        }

        void JustEngagedWith(Unit* who) override
        {
            _events.ScheduleEvent(EVENT_THROW_ROCK, 0s);
        }

    void DamageTaken(Unit* attacker, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/)override
        {
            if (damage >= me->GetHealth())
            {
                me->SetHealth(10);
                _events.Reset();
                me->RemoveAllAuras();
                me->SetFaction(35);
                me->SetUnitFlag(UnitFlags(UNIT_FLAG_CAN_SWIM | UNIT_FLAG_IMMUNE_TO_PC));
                me->AttackStop();
                attacker->AttackStop();
                me->_ExitVehicle();
                attacker->ToPlayer()->KilledMonsterCredit(NPC_MONK_ON_POLE_1);
                //_events.ScheduleEvent(EVENT_DESPAWN, 1s); // will cause crash if npc is dead. -Varjgard
                me->ForcedDespawn(); // tempfix.
            }
        }

        void UpdateAI(uint32 diff) override
        {
            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_THROW_ROCK:
                    if (!me->IsWithinMeleeRange(me->GetVictim()))
                        DoCastVictim(SPELL_THROW_ROCK);
                    _events.ScheduleEvent(EVENT_THROW_ROCK, 2500ms);
                    break;
                case EVENT_SWITCH_POLE:
                    if (!me->IsInCombat())
                    {
                        SwitchPole();
                        _events.ScheduleEvent(EVENT_SWITCH_POLE, Seconds(urand(15, 30)));
                    }
                    break;
                    //case EVENT_DESPAWN:
                        // Transform is casted only when in frog pool
                        //if (me->FindNearestCreature(NPC_CURSED_POOL_CONTROLLER, 71.0f, true))
                        //    DoCastSelf(SPELL_CURSE_OF_THE_FROG, true);
                        //ClearThreadList();
                        //me->SetWalk(true);
                        //MoveForward(10.0f);
                        //me->DespawnOrUnsummon(3s);
                        //break;
                }
            }

            DoMeleeAttackIfReady();
        }

    private:
        EventMap _events;

        void SwitchPole()
        {
            std::list<Creature*> polesList;
            std::list<Creature*> polesList2;
            // This stores objects that are too far away due to big combat reach
            me->GetCreatureListWithEntryInGrid(polesList, NPC_BALANCE_POLE_1, 1.0f);
            me->GetCreatureListWithEntryInGrid(polesList2, NPC_BALANCE_POLE_2, 1.0f);
            // Join both lists with possible different NPC entries
            polesList.splice(polesList.end(), polesList2);
            // Convert list to vector, so we can access iterator to be able to shuffle the list
            std::vector<Creature*> balancePolesList{ std::make_move_iterator(std::begin(polesList)), std::make_move_iterator(std::end(polesList)) };
            // Shuffle the list so NPCs won't jump always on the same poles
            Trinity::Containers::RandomShuffle(balancePolesList);

            for (std::vector<Creature*>::const_iterator itr = balancePolesList.begin(); itr != balancePolesList.end(); ++itr)
            {
                Position offset;
                offset.m_positionX = fabsf((*itr)->GetPositionX() - me->GetPositionX());
                offset.m_positionY = fabsf((*itr)->GetPositionY() - me->GetPositionY());

                // Object is too far
                if (offset.m_positionX > 5.0f || offset.m_positionY > 5.0f)
                    continue;

                if (!(*itr)->HasAura(SPELL_MONK_RIDE_POLE) && !(*itr)->HasAura(SPELL_RIDE_VEHICLE_POLE))
                {
                    (*itr)->CastSpell(me, SPELL_FORCECAST_RIDE_POLE, true);
                    break;
                }
            }
        }

        void ClearThreadList()
        {
            //  std::list<HostileReference*> threatList = me->GetThreatManager().getThreatList();;
            //  for (std::list<HostileReference*>::const_iterator itr = threatList.begin(); itr != threatList.end(); ++itr)
             //     if (Unit* target = (*itr)->getTarget()->ToUnit())
              //        target->ClearInCombat();
        }

        void MoveForward(float distance)
        {
            Position movePos;
            float ori = me->GetOrientation();
            float x = me->GetPositionX() + distance * cos(ori);
            float y = me->GetPositionY() + distance * sin(ori);
            float z = me->GetPositionZ();
            me->UpdateGroundPositionZ(x, y, z);
            movePos = { x, y, z };
            me->GetMotionMaster()->MovePoint(1, movePos);
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_tushui_monk_on_poleAI(creature);
    }
};

class at_singing_pools_training_bell : public AreaTriggerScript
{
public:
    at_singing_pools_training_bell() : AreaTriggerScript("at_singing_pools_training_bell") { }

    bool OnTrigger(Player* player, AreaTriggerEntry const* areaTrigger) override
    {
        if (player->IsAlive())
                player->AddAura(SPELL_TRAINING_BELL_EXCLUSION_AURA, player);
            else
                player->RemoveAura(SPELL_TRAINING_BELL_EXCLUSION_AURA);

            return true;
    }
};

class spell_rock_jump_a : public SpellScriptLoader
{
public:
    spell_rock_jump_a() : SpellScriptLoader("spell_rock_jump_a") { }

    class spell_rock_jump_a_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_rock_jump_a_SpellScript);

        void HandleJumpDest(SpellEffIndex effIndex)
        {
            PreventHitDefaultEffect(effIndex);

            if (Unit* caster = GetCaster())
            {
                if (caster->GetPositionZ() > 92.0f)
                {
                    Position const jumpPos = { 1077.019f, 2844.103f, 95.27103f };
                    caster->GetMotionMaster()->MoveJump(jumpPos, GetSpellInfo()->GetEffect(effIndex).MiscValue, 10);
                }
                else
                {
                    if (GameObject* go = caster->FindNearestGameObject(209576, 8.0f))
                        caster->GetMotionMaster()->MoveJump(go->GetPosition(), GetSpellInfo()->GetEffect(effIndex).MiscValue, 5);
                }
            }
        }

        void Register() override
        {
            OnEffectHit += SpellEffectFn(spell_rock_jump_a_SpellScript::HandleJumpDest, EFFECT_0, SPELL_EFFECT_JUMP_DEST);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_rock_jump_a_SpellScript();
    }
};

class spell_jump_to_front_right : public SpellScriptLoader
{
public:
    spell_jump_to_front_right() : SpellScriptLoader("spell_jump_to_front_right") { }

    class spell_jump_to_front_right_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_jump_to_front_right_SpellScript);

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            if (Unit* caster = GetCaster())
            {
                Position const jumpPos = { 1111.13f, 2850.21f, 94.6873f };
                caster->GetMotionMaster()->MoveJump(jumpPos, 12, 15);
            }
        }

        void Register() override
        {
            OnEffectHit += SpellEffectFn(spell_jump_to_front_right_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_jump_to_front_right_SpellScript();
    }
};

class spell_jump_to_front_left : public SpellScriptLoader
{
public:
    spell_jump_to_front_left() : SpellScriptLoader("spell_jump_to_front_left") { }

    class spell_jump_to_front_left_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_jump_to_front_left_SpellScript);

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            if (Unit* caster = GetCaster())
            {
                Position const jumpPos = { 1100.83f, 2881.36f, 94.0386f };
                caster->GetMotionMaster()->MoveJump(jumpPos, 12, 15);
            }
        }

        void Register() override
        {
            OnEffectHit += SpellEffectFn(spell_jump_to_front_left_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_jump_to_front_left_SpellScript();
    }
};

class spell_jump_to_back_right : public SpellScriptLoader
{
public:
    spell_jump_to_back_right() : SpellScriptLoader("spell_jump_to_back_right") { }

    class spell_jump_to_back_right_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_jump_to_back_right_SpellScript);

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            if (Unit* caster = GetCaster())
            {
                Position const jumpPos = { 1127.26f, 2859.8f, 97.2817f };
                caster->GetMotionMaster()->MoveJump(jumpPos, 12, 15);
            }
        }

        void Register() override
        {
            OnEffectHit += SpellEffectFn(spell_jump_to_back_right_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_jump_to_back_right_SpellScript();
    }
};

class spell_jump_to_back_left : public SpellScriptLoader
{
public:
    spell_jump_to_back_left() : SpellScriptLoader("spell_jump_to_back_left") { }

    class spell_jump_to_back_left_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_jump_to_back_left_SpellScript);

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            if (Unit* caster = GetCaster())
            {
                Position const jumpPos = { 1120.16f, 2882.66f, 96.345f };
                caster->GetMotionMaster()->MoveJump(jumpPos, 12, 15);
            }
        }

        void Register() override
        {
            OnEffectHit += SpellEffectFn(spell_jump_to_back_left_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_jump_to_back_left_SpellScript();
    }
};

enum ShuSpells
{
    SPELL_JUMP_FRONT_RIGHT = 117033,
    SPELL_JUMP_FRONT_LEFT = 117034,
    SPELL_JUMP_BACK_RIGHT = 117035,
    SPELL_JUMP_BACK_LEFT = 117036,
    SPELL_SUMMON_WATER_SPOUT = 116810,
    SPELL_WATER_SPOUT = 117063
};

enum ShuJumpPositions
{
    JUMP_POSITION_1 = 0,
    JUMP_POSITION_2 = 1,
    JUMP_POSITION_3 = 2,
    JUMP_POSITION_4 = 3
};

enum ShuEvents
{
    EVENT_JUMP_SPELL = 1,
    EVENT_SET_ORIENTATION = 2,
    EVENT_SUMMON = 3
};

enum ShuData
{
    DATA_JUMP_POSITION = 1
};

class npc_shu_playing : public CreatureScript
{
public:
    npc_shu_playing() : CreatureScript("npc_shu_playing") { }

    struct npc_shu_playingAI : public ScriptedAI
    {
        npc_shu_playingAI(Creature* creature) : ScriptedAI(creature) { }

        void Initialize()
        {
            jumpPosition = 1;
            positionBefore = 1;
            startAI = true;
        }

        void Reset() override
        {
            events.Reset();
            Initialize();
        }

        void MovementInform(uint32 type, uint32 id) override
        {
            if (type == EFFECT_MOTION_TYPE && id == EVENT_JUMP)
                events.ScheduleEvent(EVENT_SET_ORIENTATION, 500ms);
        }

        uint32 GetData(uint32 id) const override
        {
            if (id == DATA_JUMP_POSITION)
                return jumpPosition;

            return false;
        }

        void UpdateAI(uint32 diff) override
        {
            if (startAI)
            {
                events.ScheduleEvent(EVENT_JUMP_SPELL, 1s);
                startAI = false;
            }

            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_JUMP_SPELL:
                    if (urand(0, 2) != 0)
                        jumpPosition = urand(JUMP_POSITION_1, JUMP_POSITION_4);
                    else
                        jumpPosition = positionBefore;

                    if (jumpPosition == positionBefore)
                    {
                    events.CancelEvent(EVENT_SET_ORIENTATION);
                    events.ScheduleEvent(EVENT_SUMMON, 1500ms);
                    }
                    else
                    {
                        DoCast(jumpSpells[jumpPosition]);
                        positionBefore = jumpPosition;
                    }
                    break;
                case EVENT_SET_ORIENTATION:
                    switch (jumpPosition)
                    {
                    case JUMP_POSITION_1:
                        me->SetFacingTo(1.32645f);
                        break;
                    case JUMP_POSITION_2:
                        me->SetFacingTo(5.654867f);
                        break;
                    case JUMP_POSITION_3:
                        me->SetFacingTo(2.338741f);
                        break;
                    case JUMP_POSITION_4:
                        me->SetFacingTo(4.34587f);
                        break;
                    }
                    events.ScheduleEvent(EVENT_SUMMON, 1500ms);
                    break;
                case EVENT_SUMMON:
                    DoCast(SPELL_SUMMON_WATER_SPOUT);
                    DoCast(SPELL_WATER_SPOUT);
                    events.ScheduleEvent(EVENT_JUMP_SPELL, 6s);
                    break;
                }
            }
        }

    private:
        EventMap events;
        uint32 jumpSpells[4] = { SPELL_JUMP_FRONT_RIGHT, SPELL_JUMP_FRONT_LEFT, SPELL_JUMP_BACK_RIGHT, SPELL_JUMP_BACK_LEFT };
        uint8 jumpPosition;
        uint8 positionBefore;
        bool startAI;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_shu_playingAI(creature);
    }
};

class spell_summon_water_spout : public SpellScriptLoader
{
public:
    spell_summon_water_spout() : SpellScriptLoader("spell_summon_water_spout") { }

    class spell_summon_water_spout_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_summon_water_spout_SpellScript);

        void HandleSummon(SpellEffIndex effIndex)
        {
            PreventHitDefaultEffect(effIndex);
            uint32 entry = GetSpellInfo()->GetEffect(effIndex).MiscValue;
            SummonPropertiesEntry const* properties = sSummonPropertiesStore.LookupEntry(GetSpellInfo()->GetEffect(effIndex).MiscValueB);
            if (!entry || !properties)
                return;

            int32 duration = GetSpellInfo()->GetDuration();
            uint32 randomPos = urand(0, 6);

            Position const SpawnPosition[4][7] =
            {
                {
                    { 1117.516f, 2848.437f, 92.14017f },
                    { 1105.92f, 2853.432f, 92.14017f },
                    { 1105.231f, 2847.766f, 92.14017f },
                    { 1114.819f, 2844.094f, 92.14017f },
                    { 1110.618f, 2856.7f, 92.14017f },
                    { 1109.559f, 2843.255f, 92.14017f },
                    { 1116.04f, 2854.104f, 92.14017f }
                },
                {
                    { 1106.743f, 2879.544f, 92.14017f },
                    { 1105.793f, 2885.37f, 92.14017f },
                    { 1098.16f, 2874.628f, 92.14017f },
                    { 1104.28f, 2875.759f, 92.14017f },
                    { 1095.38f, 2885.097f, 92.14017f },
                    { 1100.078f, 2888.365f, 92.14017f },
                    { 1094.693f, 2879.431f, 92.14017f }
                },
                {
                    { 1132.911f, 2864.381f, 92.14017f },
                    { 1125.672f, 2851.84f, 92.14017f },
                    { 1121.057f, 2856.08f, 92.14017f },
                    { 1134.373f, 2858.654f, 92.14017f },
                    { 1126.556f, 2867.097f, 92.14017f },
                    { 1120.064f, 2863.003f, 92.14017f },
                    { 1131.856f, 2852.781f, 92.14017f }
                },
                {
                    { 1118.22f, 2875.427f, 92.14017f },
                    { 1113.274f, 2879.232f, 92.14017f },
                    { 1125.439f, 2887.632f, 92.14017f },
                    { 1118.766f, 2890.419f, 92.14017f },
                    { 1113.783f, 2886.404f, 92.14017f },
                    { 1123.7f, 2876.575f, 92.14017f },
                    { 1126.358f, 2881.005f, 92.14017f }
                }
            };

            uint32 stone = GetCaster()->ToCreature()->AI()->GetData(DATA_JUMP_POSITION);

            if (TempSummon* summon = GetCaster()->GetMap()->SummonCreature(entry, SpawnPosition[stone][randomPos], properties, duration, GetCaster()))
                summon->SetTempSummonType(TEMPSUMMON_MANUAL_DESPAWN);
        }

        void Register() override
        {
            OnEffectHit += SpellEffectFn(spell_summon_water_spout_SpellScript::HandleSummon, EFFECT_0, SPELL_EFFECT_SUMMON);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_summon_water_spout_SpellScript();
    }
};

enum WaterSpoutQuestCreditSpells
{
    SPELL_AYSA_CONGRATS_TIMER = 128589,
    SPELL_SUMMON_SPIRIT_OF_WATER = 103538
};

class spell_water_spout_quest_credit : public SpellScriptLoader
{
public:
    spell_water_spout_quest_credit() : SpellScriptLoader("spell_water_spout_quest_credit") { }

    class spell_water_spout_quest_credit_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_water_spout_quest_credit_SpellScript);

        void HandleScript()
        {
            if (Player* target = GetHitPlayer())
            {
                if (target->GetQuestStatus(QUEST_A_NEW_FRIEND) == QUEST_STATUS_COMPLETE)
                {
                    target->AddAura(SPELL_AYSA_CONGRATS_TIMER, target);
                    target->CastSpell(target, SPELL_SUMMON_SPIRIT_OF_WATER, true);
                }
            }
        }

        void Register() override
        {
            AfterHit += SpellHitFn(spell_water_spout_quest_credit_SpellScript::HandleScript);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_water_spout_quest_credit_SpellScript();
    }
};

class spell_aysa_congrats_timer : public SpellScriptLoader
{
public:
    spell_aysa_congrats_timer() : SpellScriptLoader("spell_aysa_congrats_timer") { }

    class spell_aysa_congrats_timer_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_aysa_congrats_timer_AuraScript);

        void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            if (Unit* target = GetTarget())
                if (Creature* creature = target->FindNearestCreature(54975, 70.0f, true))
                    creature->AI()->Talk(0, target);
        }

        void Register() override
        {
            OnEffectRemove += AuraEffectRemoveFn(spell_aysa_congrats_timer_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_aysa_congrats_timer_AuraScript();
    }
};

class spell_aysa_congrats_trigger_aura : public SpellScriptLoader
{
public:
    spell_aysa_congrats_trigger_aura() : SpellScriptLoader("spell_aysa_congrats_trigger_aura") { }

    class spell_aysa_congrats_trigger_aura_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_aysa_congrats_trigger_aura_AuraScript);

        void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            if (Unit* target = GetTarget())
                if (Creature* creature = target->FindNearestCreature(54975, 70.0f, true))
                    creature->AI()->Talk(1, target);
        }

        void Register() override
        {
            OnEffectRemove += AuraEffectRemoveFn(spell_aysa_congrats_trigger_aura_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_aysa_congrats_trigger_aura_AuraScript();
    }
};

class at_temple_of_five_dawns_summon_zhaoren : public AreaTriggerScript
{
public:
    at_temple_of_five_dawns_summon_zhaoren() : AreaTriggerScript("at_temple_of_five_dawns_summon_zhaoren") { }

    bool OnTrigger(Player* player, AreaTriggerEntry const* areaTrigger) override
    {
        if (player->IsAlive())
        {
                Position const pos = { 750.5781f, 4262.676f, 323.0713f, 5.042483f };

                player->SummonCreature(64554, pos, TEMPSUMMON_MANUAL_DESPAWN);
        }

        return true;
    }
};

enum MonkeyWisdomTexts
{
    TEXT_MONKEY_WISDOM = 54073,
    TEXT_MONKEY_WISDOM_2 = 54074,
    TEXT_MONKEY_WISDOM_3 = 54075,
    TEXT_MONKEY_WISDOM_4 = 54076,
    TEXT_MONKEY_WISDOM_5 = 54077,
    TEXT_MONKEY_WISDOM_6 = 54078,
    TEXT_MONKEY_WISDOM_7 = 54079,
    TEXT_MONKEY_WISDOM_8 = 54080
};

class spell_monkey_wisdom_text : public SpellScriptLoader
{
public:
    spell_monkey_wisdom_text() : SpellScriptLoader("spell_monkey_wisdom_text") { }

    class spell_monkey_wisdom_text_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_monkey_wisdom_text_SpellScript);

        bool Validate(SpellInfo const* /*spellInfo*/) override
        {
            if (!sBroadcastTextStore.LookupEntry(TEXT_MONKEY_WISDOM) || !sBroadcastTextStore.LookupEntry(TEXT_MONKEY_WISDOM_2) ||
                !sBroadcastTextStore.LookupEntry(TEXT_MONKEY_WISDOM_3) || !sBroadcastTextStore.LookupEntry(TEXT_MONKEY_WISDOM_4) ||
                !sBroadcastTextStore.LookupEntry(TEXT_MONKEY_WISDOM_5) || !sBroadcastTextStore.LookupEntry(TEXT_MONKEY_WISDOM_6) ||
                !sBroadcastTextStore.LookupEntry(TEXT_MONKEY_WISDOM_7) || !sBroadcastTextStore.LookupEntry(TEXT_MONKEY_WISDOM_8))
                return false;
            return true;
        }

        bool Load() override
        {
            return GetCaster()->GetTypeId() == TYPEID_PLAYER;
        }

        void HandleScript(SpellEffIndex /*effIndex*/)
        {
            uint32 randomText = urand(0, 7);

            GetCaster()->Talk(TEXT_MONKEY_WISDOM + randomText, CHAT_MSG_RAID_BOSS_WHISPER, 0.0f, GetHitPlayer());
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_monkey_wisdom_text_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_monkey_wisdom_text_SpellScript();
    }
};

enum RukRukOoksplosions
{
    SPELL_OOKSPLOSIONS_TRIGGERED = 125885
};

class spell_ruk_ruk_ooksplosions : public SpellScriptLoader
{
public:
    spell_ruk_ruk_ooksplosions() : SpellScriptLoader("spell_ruk_ruk_ooksplosions") { }

    class spell_ruk_ruk_ooksplosions_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_ruk_ruk_ooksplosions_AuraScript);

        void HandleEffectPeriodic(AuraEffect const* /*aurEff*/)
        {
            Unit* caster = GetCaster();
            caster->CastSpell(caster->GetRandomNearPosition(10.0f), SPELL_OOKSPLOSIONS_TRIGGERED, true);
        }

        void Register() override
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_ruk_ruk_ooksplosions_AuraScript::HandleEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_ruk_ruk_ooksplosions_AuraScript();
    }
};

enum RukRukEvents
{
    EVENT_AIM = 1,
    EVENT_OOKSPLOSIONS = 2
};

enum RukRukSpells
{
    SPELL_AIM = 125609,
    SPELL_OOKSPLOSIONS = 125699,
    SPELL_AIM_VISUAL = 26079
};

class npc_ruk_ruk : public CreatureScript
{
public:
    npc_ruk_ruk() : CreatureScript("npc_ruk_ruk") { }

    struct npc_ruk_rukAI : public ScriptedAI
    {
        npc_ruk_rukAI(Creature* creature) : ScriptedAI(creature) { }

        void Reset() override
        {
            _events.Reset();
        }

        void JustEngagedWith(Unit* who) override
        {
            _events.ScheduleEvent(EVENT_AIM, 10s);
            _events.ScheduleEvent(EVENT_OOKSPLOSIONS, 30s);
        }

        Position GetRocketTargetPos() const
        {
            return _pos;
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_AIM:
                    if (me->HasUnitState(UNIT_STATE_CASTING))
                    {
                        _events.RescheduleEvent(EVENT_AIM, 1s);
                        break;
                    }
                    if (Unit* target = SelectTarget(SelectTargetMethod::Random))
                    {
                        me->SetFacingToObject(target);
                        CalculateSpellVisual(target);
                        DoCast(target, SPELL_AIM);
                        _events.ScheduleEvent(EVENT_AIM, Seconds(urand(15, 25)));
                    }
                    break;
                case EVENT_OOKSPLOSIONS:
                    if (me->HasUnitState(UNIT_STATE_CASTING))
                    {
                        _events.RescheduleEvent(EVENT_OOKSPLOSIONS, 1s);
                        break;
                    }
                    DoCast(SPELL_OOKSPLOSIONS);
                    _events.ScheduleEvent(EVENT_OOKSPLOSIONS, Seconds(urand(25, 35)));
                    break;
                }
            }

            DoMeleeAttackIfReady();
        }

    private:
        EventMap _events;
        Position _pos;

        void CalculateSpellVisual(Unit* target)
        {
            float ori = me->GetOrientation();
            float z = me->GetPositionZ();
            float targetDist = target->GetExactDist(me->GetPosition());

            for (int radius = 1; ; radius++)
            {
                if (radius <= ceilf(targetDist))
                {
                    float x = me->GetPositionX() + radius * cos(ori);
                    float y = me->GetPositionY() + radius * sin(ori);
                    me->UpdateGroundPositionZ(x, y, z);
                    _pos = { x, y, z };
                    me->SendPlaySpellVisual(_pos, SPELL_AIM_VISUAL, true, 0.0f,  0, 0, 2.0f);
                }
                else
                    break;
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_ruk_rukAI(creature);
    }
};

typedef npc_ruk_ruk::npc_ruk_rukAI RukRukAI;

enum RukRukRocketEvents
{
    EVENT_FIRE = 1
};

enum RukRukRocketSpells
{
    SPELL_EXPLOSSION_VISUAL = 125612,
    SPELL_EXPLOSSION_DMG = 125619
};

class npc_ruk_ruk_rocket : public CreatureScript
{
public:
    npc_ruk_ruk_rocket() : CreatureScript("npc_ruk_ruk_rocket") { }

    struct npc_ruk_ruk_rocketAI : public ScriptedAI
    {
        npc_ruk_ruk_rocketAI(Creature* creature) : ScriptedAI(creature) { }

        void Reset() override
        {
            me->SetDisplayFromModel(0);
            _events.ScheduleEvent(EVENT_FIRE, 500ms);
        }

        void IsSummonedBy(WorldObject* owner) override
        {
          //  if (RukRukAI* rukRukAI = CAST_AI(RukRukAI, owner->GetAI()))
            {
             //   _rocketTargetPos = rukRukAI->GetRocketTargetPos();

                if (me->GetExactDist2d(_rocketTargetPos) > 30)
                    RecalculateTargetPos();
            }
        }

        void MovementInform(uint32 /*type*/, uint32 id) override
        {
            if (id == 1)
            {
                DoCast(SPELL_EXPLOSSION_VISUAL);
                DoCast(SPELL_EXPLOSSION_DMG);
                me->DespawnOrUnsummon(1s);
            }
        }

        void UpdateAI(uint32 diff) override
        {
            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_FIRE:
                    me->GetMotionMaster()->MovePoint(1, _rocketTargetPos);
                    break;
                }
            }
        }

    private:
        EventMap _events;
        Position _rocketTargetPos;

        void RecalculateTargetPos()
        {
            float ori = me->GetOrientation();
            float x = me->GetPositionX() + 30 * cos(ori);
            float y = me->GetPositionY() + 30 * sin(ori);
            float z = me->GetPositionZ();
            me->UpdateGroundPositionZ(x, y, z);
            _rocketTargetPos = { x, y, z };
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_ruk_ruk_rocketAI(creature);
    }
};

enum ZhaorenEvents
{
    EVENT_LIGHTNING = 1,
    EVENT_MOVE_CENTER = 2,
    EVENT_STUN = 3,
    EVENT_SWEEP = 4,
    EVENT_RESUME_WP = 5
};

enum ZhaorenSpells
{
    SPELL_LIGHTNING_POOL = 126006,
    SPELL_STUNNED_BY_FIREWORKS = 125992,
    SPELL_SERPENT_SWEEP = 125990,
    SPELL_FORCECAST_SUMMON_SHANG = 128808,
    SPELL_OVERPACKED_FIREWORK = 104855,
    SPELL_FIREWORK_INACTIVE = 125964
};

enum ZhaorenPhases
{
    PHASE_FLYING = 1,
    PHASE_GROUNDED = 2,
    PHASE_STAY_IN_CENTER = 3
};

enum ZhaorenMisc
{
    ZHAOREN_PATH = 210014590,
    NPC_JI_FIREPAW = 64505,
    NPC_AYSA_CLOUDSINGER = 64506,
    NPC_DAFENG = 64532,
    NPC_FIREWORK = 64507,
    DATA_1 = 1,
    DATA_COMBAT = 2,
    DATA_AYSA_TALK_3 = 3,
    DATA_PHASE_OOC = 4,
    DATA_ZHAOREN_DEATH = 5,
    DATA_EVADE = 6
};

Position ZhaoPos[] =
{
    {699.134f, 4170.06f, 216.06f}, // Center
};

//55786
class npc_zhaoren : public CreatureScript
{
public:
    npc_zhaoren() : CreatureScript("npc_zhaoren") { }

    struct npc_zhaorenAI : public ScriptedAI
    {
        npc_zhaorenAI(Creature* creature) : ScriptedAI(creature) { }

        Position const pos = { 723.163f, 4163.8f, 204.999f };

    public:
        uint8 phase;
        bool _sweepScheduled;

        void Reset() override
        {
            events.Reset();
            me->SetReactState(REACT_PASSIVE);
            me->setActive(true);
            phase = 0;
            _sweepScheduled = false;

            if (Creature* creature = me->FindNearestCreature(NPC_JI_FIREPAW, me->GetVisibilityRange(), true))
                creature->AI()->SetData(DATA_EVADE, DATA_EVADE);

            std::list<Creature*> fireworks;
            me->GetCreatureListWithEntryInGrid(fireworks, NPC_FIREWORK, me->GetVisibilityRange());
            for (std::list<Creature*>::iterator itr = fireworks.begin(); itr != fireworks.end(); ++itr)
            {
                (*itr)->RemoveAura(SPELL_FIREWORK_INACTIVE);
                (*itr)->AI()->SetData(DATA_1, DATA_1);
            }
            me->GetMotionMaster()->Clear();
            me->GetMotionMaster()->MovePoint(0, ZhaoPos[0].GetPositionX(), ZhaoPos[0].GetPositionY(), ZhaoPos[0].GetPositionZ());
        }

        void JustEngagedWith(Unit* who) override
        {
            me->GetMotionMaster()->MovePath(ZHAOREN_PATH, true);
            events.SetPhase(PHASE_FLYING);
            events.ScheduleEvent(EVENT_LIGHTNING, 5s);
        }

        void SpellHit(WorldObject* caster, SpellInfo const* spell) override
        {
            if (spell->Id == SPELL_OVERPACKED_FIREWORK)
            {
                if (!me->IsInCombat())
                    me->Attack(caster->ToUnit(), true);
            }
        }

        void MovementInform(uint32 type, uint32 id) override
        {
            if (type == POINT_MOTION_TYPE && id == EVENT_MOVE_CENTER)
                events.ScheduleEvent(EVENT_STUN, 0s);
        }

        void KilledUnit(Unit* who) override
        {
            if (who->IsPlayer())
                //  if (me->GetThreatManager().GetThreatListSize().empty())
                me->ForcedDespawn(0, 10s);
        }

        void JustDied(Unit* /*killer*/) override
        {
            DoCastAOE(SPELL_FORCECAST_SUMMON_SHANG, true);

            if (Creature* creature = me->FindNearestCreature(NPC_JI_FIREPAW, me->GetVisibilityRange(), true))
                creature->AI()->SetData(DATA_ZHAOREN_DEATH, DATA_ZHAOREN_DEATH);
            if (Creature* creature = me->FindNearestCreature(NPC_AYSA_CLOUDSINGER, me->GetVisibilityRange(), true))
                creature->AI()->SetData(DATA_ZHAOREN_DEATH, DATA_ZHAOREN_DEATH);
            if (Creature* creature = me->FindNearestCreature(NPC_DAFENG, me->GetVisibilityRange(), true))
                creature->AI()->SetData(DATA_ZHAOREN_DEATH, DATA_ZHAOREN_DEATH);
            me->ForcedDespawn(10000, 10s);
        }

        void UpdateAI(uint32 diff) override
        {
            events.Update(diff);
            if (phase == 0 && HealthBelowPct(85))
            {
                phase++;
                if (Creature* creature = me->FindNearestCreature(NPC_AYSA_CLOUDSINGER, me->GetVisibilityRange(), true))
                    creature->AI()->SetData(DATA_1, DATA_1);
            }
            if (phase == 1 && HealthBelowPct(75))
            {
                phase++;
                events.SetPhase(PHASE_GROUNDED);
                events.CancelEvent(EVENT_LIGHTNING);
                events.ScheduleEvent(EVENT_MOVE_CENTER, 0s);
            }
            if (phase == 2 && HealthBelowPct(25))
            {
                phase++;
                events.SetPhase(PHASE_STAY_IN_CENTER);
                events.CancelEvent(EVENT_LIGHTNING);
                events.ScheduleEvent(EVENT_MOVE_CENTER, 0s);
            }
            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    /*    case EVENT_LIGHTNING:
                        {
                            //std::list<HostileReference*> threatList = me->GetThreatManager().getThreatList();
                          //  if (!threatList.empty())
                            {
                                for (HostileReference* ref : threatList)
                                    if (ref->getTarget()->IsPlayer())
                                        DoCast(ref->getTarget(), SPELL_LIGHTNING_POOL);

                                events.ScheduleEvent(EVENT_LIGHTNING, events.IsInPhase(PHASE_FLYING) ? 5000 : 3500);
                                if (!_sweepScheduled && events.IsInPhase(PHASE_STAY_IN_CENTER))
                                {
                                    events.ScheduleEvent(EVENT_SWEEP, 15000, 0, PHASE_STAY_IN_CENTER);
                                    _sweepScheduled = true;
                                }
                            }
                            else
                            me->ForcedDespawn(0, 10s);
                            break;
                        }*/
                case EVENT_MOVE_CENTER:
                    me->GetMotionMaster()->MovePoint(EVENT_MOVE_CENTER, pos);
                    break;

                case EVENT_STUN:
                    DoCast(SPELL_STUNNED_BY_FIREWORKS);
                    events.ScheduleEvent(EVENT_SWEEP, 12s);
                    if (Creature* creature = me->FindNearestCreature(NPC_AYSA_CLOUDSINGER, me->GetVisibilityRange(), true))
                    {
                        if (phase == 2)
                            creature->AI()->SetData(DATA_COMBAT, DATA_COMBAT);
                        else if (phase == 3)
                            creature->AI()->SetData(DATA_AYSA_TALK_3, DATA_AYSA_TALK_3);
                    }
                    if (Creature* creature = me->FindNearestCreature(NPC_JI_FIREPAW, me->GetVisibilityRange(), true))
                        creature->AI()->SetData(DATA_COMBAT, DATA_COMBAT);
                    break;

                case EVENT_SWEEP:
                    events.CancelEvent(EVENT_LIGHTNING);
                    DoCast(SPELL_SERPENT_SWEEP);
                    _sweepScheduled = false;
                    events.ScheduleEvent(EVENT_LIGHTNING, 3500ms, 0, PHASE_STAY_IN_CENTER);
                    events.ScheduleEvent(EVENT_RESUME_WP, 5s, 0, PHASE_GROUNDED);
                    if (events.IsInPhase(PHASE_GROUNDED))
                        if (Creature* creature = me->FindNearestCreature(NPC_JI_FIREPAW, me->GetVisibilityRange(), true))
                            creature->AI()->SetData(DATA_PHASE_OOC, DATA_PHASE_OOC);
                    break;
                case EVENT_RESUME_WP:
                    me->GetMotionMaster()->MovePath(ZHAOREN_PATH, true);
                    events.SetPhase(PHASE_FLYING);
                    events.ScheduleEvent(EVENT_LIGHTNING, 5s);
                    break;
                }
            }
        }
    private:
        EventMap events;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_zhaorenAI(creature);
    }
};

enum SpellMasterShangFinalEscortNPCs
{
    NPC_MASTER_SHANG = 55672
};

class spell_master_shang_final_escort_say : public SpellScriptLoader
{
public:
    spell_master_shang_final_escort_say() : SpellScriptLoader("spell_master_shang_final_escort_say") { }

    class spell_master_shang_final_escort_say_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_master_shang_final_escort_say_AuraScript);

        void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            if (Unit* target = GetTarget())
                if (Creature* creature = target->FindNearestCreature(NPC_MASTER_SHANG, target->GetVisibilityRange(), true))
                    creature->AI()->Talk(0, target);
        }

        void Register() override
        {
            OnEffectRemove += AuraEffectRemoveFn(spell_master_shang_final_escort_say_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_master_shang_final_escort_say_AuraScript();
    }
};

enum ShenZinShuBunnySpells
{
    SPELL_TRIGGER_WITH_ANIM_0 = 114898,
    SPELL_TRIGGER = 106759,
    SPELL_TRIGGER_WITH_ANIM_1 = 118571,
    SPELL_TRIGGER_WITH_TURN = 118572
};

enum ShenZinShuBunnyTexts
{
    TEXT_1 = 55550,
    TEXT_2 = 55568,
    TEXT_3 = 55569,
    TEXT_4 = 55570,
    TEXT_5 = 55572,
    TEXT_6 = 63407
};

class npc_shen_zin_shu_bunny : public CreatureScript
{
public:
    npc_shen_zin_shu_bunny() : CreatureScript("npc_shen_zin_shu_bunny") { }

    struct npc_shen_zin_shu_bunnyAI : public ScriptedAI
    {
        npc_shen_zin_shu_bunnyAI(Creature* creature) : ScriptedAI(creature) { }

        void Initialize()
        {
            _hitCount = 0;
        }

        void Reset() override
        {
            Initialize();
            me->setActive(true);
        }

        void SpellHit(WorldObject* caster, SpellInfo const* spell) override
        {
            switch (spell->Id)
            {
            case SPELL_TRIGGER_WITH_ANIM_0:
                me->Talk(TEXT_1, CHAT_MSG_MONSTER_SAY, 300.0f, caster);
                me->PlayDirectSound(27822, caster->ToPlayer());
                break;
            case SPELL_TRIGGER:
                me->Talk(TEXT_2, CHAT_MSG_MONSTER_SAY, 300.0f, caster);
                me->PlayDirectSound(27823, caster->ToPlayer());
                break;
            case SPELL_TRIGGER_WITH_ANIM_1:
                if (_hitCount == 0)
                {
                    me->Talk(TEXT_3, CHAT_MSG_MONSTER_SAY, 300.0f, caster);
                    me->PlayDirectSound(27824, caster->ToPlayer());
                    _hitCount++;
                }
                else if (_hitCount == 1)
                {
                    me->Talk(TEXT_4, CHAT_MSG_MONSTER_SAY, 300.0f, caster);
                    me->PlayDirectSound(27825, caster->ToPlayer());
                    _hitCount++;
                }
                else if (_hitCount == 2)
                {
                    me->Talk(TEXT_6, CHAT_MSG_MONSTER_SAY, 350.0f, caster);
                    me->PlayDirectSound(27827, caster->ToPlayer());
                    _hitCount = 0;
                }
                break;
            case SPELL_TRIGGER_WITH_TURN:
                me->Talk(TEXT_5, CHAT_MSG_MONSTER_SAY, 300.0f, caster);
                me->PlayDirectSound(27826, caster->ToPlayer());
                break;
            }
        }

    private:
        uint8 _hitCount;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_shen_zin_shu_bunnyAI(creature);
    }
};

class spell_injured_sailor_feign_death : public SpellScriptLoader
{
public:
    spell_injured_sailor_feign_death() : SpellScriptLoader("spell_injured_sailor_feign_death") { }

    class spell_injured_sailor_feign_death_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_injured_sailor_feign_death_AuraScript);

        void HandleEffectApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            Unit* target = GetTarget();
            target->SetUnitFlag(UNIT_FLAG_PREVENT_EMOTES_FROM_CHAT_TEXT);
            target->SetUnitFlag2(UNIT_FLAG2_FEIGN_DEATH);
            target->SetHealth(target->CountPctFromMaxHealth(25));
        }

        void Register() override
        {
            OnEffectApply += AuraEffectApplyFn(spell_injured_sailor_feign_death_AuraScript::HandleEffectApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_injured_sailor_feign_death_AuraScript();
    }
};

enum RescueInjuredSailor
{
    SPELL_RESCUE_SAILOR_MALE_CAST = 105520,
    SPELL_RESCUE_SAILOR_FEMALE_CAST = 129340,
    NPC_RESCUED_SAILOR = 56236
};

class spell_rescue_injured_sailor : public SpellScriptLoader
{
public:
    spell_rescue_injured_sailor() : SpellScriptLoader("spell_rescue_injured_sailor") { }

    class spell_rescue_injured_sailor_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_rescue_injured_sailor_SpellScript);

        bool Load() override
        {
            return GetCaster()->GetTypeId() == TYPEID_PLAYER;
        }

        void HandleDummy(SpellEffIndex effIndex)
        {
            PreventHitDefaultEffect(effIndex);
            Player* player = GetCaster()->ToPlayer();

            if (player->GetGender() == GENDER_MALE)
                player->CastSpell(player, SPELL_RESCUE_SAILOR_MALE_CAST, true);
            else
                player->CastSpell(player, SPELL_RESCUE_SAILOR_FEMALE_CAST, true);
        }

        void HandleSummon(SpellEffIndex effIndex)
        {
            PreventHitDefaultEffect(effIndex);
            GetCaster()->GetMap()->SummonCreature(NPC_RESCUED_SAILOR, GetCaster()->GetPosition(), 0, 240000, GetCaster());
        }

        void Register() override
        {
            OnEffectHit += SpellEffectFn(spell_rescue_injured_sailor_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            OnEffectHit += SpellEffectFn(spell_rescue_injured_sailor_SpellScript::HandleSummon, EFFECT_1, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_rescue_injured_sailor_SpellScript();
    }
};

enum WreckOfTheSkyseeker
{
    SPELL_CANCEL_RESCUE_AURA = 117987,
    NPC_INJURED_SAILOR = 56236,
    QUEST_NONE_LEFT_BEHIND = 29794
};

class at_wreck_of_the_skyseeker_injured_sailor : public AreaTriggerScript
{
public:
    at_wreck_of_the_skyseeker_injured_sailor() : AreaTriggerScript("at_wreck_of_the_skyseeker_injured_sailor") { }

    bool OnTrigger(Player* player, AreaTriggerEntry const* /*areaTrigger*/) override
    {
        if (player->IsAlive() && player->IsVehicle() && player->GetQuestStatus(QUEST_NONE_LEFT_BEHIND) == QUEST_STATUS_INCOMPLETE)
        {
                if (Creature* creature = player->FindNearestCreature(NPC_INJURED_SAILOR, 1.0f, true))
                    creature->CastSpell(player, SPELL_CANCEL_RESCUE_AURA, true);
        }

        return true;
    }
};

enum AysaVordrakaFightEvents
{
    EVENT_TEMPERED_FURY = 1,
    EVENT_COMBAT_ROLL = 2,
    EVENT_UPDATE_PHASES = 3
};

enum AysaVordrakaFightSpells
{
    SPELL_TEMPERED_FURY = 117275,
    SPELL_COMBAT_ROLL = 117312,
    SPELL_FORCECAST_SUMMON_AYSA = 117499,
    SPELL_DEEP_SEA_SMASH = 117287
};

enum AysaVordrakaFightData
{
    DATA_AYSA_TALK_INTRO = 1,
    DATA_AYSA_TALK_SMASH = 2,
    DATA_SUMMON_AGGRESSORS = 3,
    DATA_VORDRAKA_DEATH = 4,
};

enum AysaVordrakaFightTexts
{
    TEXT_INTRO = 0,
    TEXT_SMASH = 1,
    TEXT_REINFORCEMENTS = 2,
    TEXT_LOW_HP = 3,
    TEXT_DEATH = 4
};

enum AysaVordrakaFightMisc
{
    QUEST_AN_ANCIENT_EVIL = 29798,
    QUEST_RISKING_IT_ALL = 30767,
    NPC_VORDRAKA = 56009,
    DB_PHASE_FIGHT = 543,
    DB_PHASE_AFTER_FIGHT = 993
};

class npc_aysa_vordraka_fight : public CreatureScript
{
public:
    npc_aysa_vordraka_fight() : CreatureScript("npc_aysa_vordraka_fight") { }

    struct npc_aysa_vordraka_fightAI : public ScriptedAI
    {
        npc_aysa_vordraka_fightAI(Creature* creature) : ScriptedAI(creature) {
            me->SetReactState(REACT_DEFENSIVE);
        }

        void MoveInLineOfSight(Unit* who) override
        {
            if (who->IsPlayer())
                if (Creature* creature = me->FindNearestCreature(NPC_VORDRAKA, me->GetVisibilityRange(), true))
                    AttackStart(creature);
        }

        void JustEngagedWith(Unit* who) override
        {
            who->GetAI()->AttackStart(me);
            _events.ScheduleEvent(EVENT_TEMPERED_FURY, Seconds(urand(2, 4)));
        }

    void DamageTaken(Unit* attacker, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/) override
        {
            if (HealthAbovePct(85))
                damage = urand(1, 2);
            else
                me->SetHealth(me->GetMaxHealth() * 0.85f);
        }

        void SpellHit(WorldObject* /*caster*/, SpellInfo const* spell) override
        {
            if (spell->Id == SPELL_DEEP_SEA_SMASH)
            {
                _events.CancelEvent(EVENT_TEMPERED_FURY);
                _events.ScheduleEvent(EVENT_COMBAT_ROLL, 1s);
            }
        }

        void SetData(uint32 id, uint32 /*value*/) override
        {
            switch (id)
            {
            case DATA_AYSA_TALK_INTRO:
                Talk(TEXT_INTRO);
                break;
            case DATA_AYSA_TALK_SMASH:
                Talk(TEXT_SMASH);
                break;
            case DATA_SUMMON_AGGRESSORS:
                Talk(TEXT_REINFORCEMENTS);
                break;
            case DATA_VORDRAKA_DEATH:
                Talk(TEXT_DEATH);
                EnterEvadeMode(EVADE_REASON_OTHER);
                _events.CancelEvent(EVENT_TEMPERED_FURY);
                break;
            }
        }

        void JustReachedHome() override
        {
            _events.ScheduleEvent(EVENT_UPDATE_PHASES, 5s);
        }

        void OnQuestAccept(Player* player, Quest const* quest) override
        {
            if (quest->GetQuestId() == QUEST_RISKING_IT_ALL)
                DoCast(player, SPELL_FORCECAST_SUMMON_AYSA, true);
        }

        void UpdateAI(uint32 diff) override
        {
            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_TEMPERED_FURY:
                    DoCastVictim(SPELL_TEMPERED_FURY);
                    _events.ScheduleEvent(EVENT_TEMPERED_FURY, Seconds(urand(2, 4)));
                    break;
                case EVENT_COMBAT_ROLL:
                    // todo: cast combat roll only if it won't kick Vordraka outside of ship boundaries
                    DoCastVictim(SPELL_COMBAT_ROLL);
                    _events.ScheduleEvent(EVENT_TEMPERED_FURY, Seconds(urand(5, 7)));
                    break;
                case EVENT_UPDATE_PHASES:
                    std::list<Player*> players;
                    me->GetPlayerListInGrid(players, me->GetVisibilityRange());
                    for (std::list<Player*>::const_iterator itr = players.begin(); itr != players.end(); ++itr)
                    {
                        if ((*itr)->ToPlayer()->GetQuestStatus(QUEST_AN_ANCIENT_EVIL) == QUEST_STATUS_COMPLETE)
                        {
                            PhasingHandler::AddPhase(*itr, DB_PHASE_AFTER_FIGHT, true);
                            PhasingHandler::RemovePhase(*itr, DB_PHASE_FIGHT, true);
                        }
                    }
                    break;
                }
            }

            DoMeleeAttackIfReady();
        }

    private:
        EventMap _events;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_aysa_vordraka_fightAI(creature);
    }
};

class spell_tempered_fury : public SpellScriptLoader
{
public:
    spell_tempered_fury() : SpellScriptLoader("spell_tempered_fury") { }

    class spell_tempered_fury_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_tempered_fury_SpellScript);

        void HandleJumpDest(SpellEffIndex effIndex)
        {
            PreventHitDefaultEffect(effIndex);
            Position const jumpPos = GetHitDest()->GetPosition();
            GetCaster()->GetMotionMaster()->MoveJump(jumpPos, 20, 0);
        }

        void Register() override
        {
            OnEffectLaunch += SpellEffectFn(spell_tempered_fury_SpellScript::HandleJumpDest, EFFECT_0, SPELL_EFFECT_JUMP_DEST);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_tempered_fury_SpellScript();
    }
};

enum VordrakaEvents
{
    EVENT_SMASH = 1,
    EVENT_RUPTURE = 2,
    EVENT_SUMMON_AGGRESSOR = 3
};

enum VordrakaSpells
{
    SPELL_DEEP_SEA_RUPTURE = 117456,
    SPELL_FORCECAST_AGGRESSOR = 117403,
    SPELL_DEATH_INVIS = 117555,
    SPELL_SEE_DEATH_INVIS = 117491
};

enum VordrakaMisc
{
    NPC_AYSA_CLOUDSINGER_VORDRAKA = 56416,
    NPC_DEEPSCALE_AGGRESSOR = 60685
};

class npc_vordraka : public CreatureScript
{
public:
    npc_vordraka() : CreatureScript("npc_vordraka") { }

    struct npc_vordrakaAI : public ScriptedAI
    {
        npc_vordrakaAI(Creature* creature) : ScriptedAI(creature) {
            Initialize();
        }

        void Initialize()
        {
            _playerParticipating = false;
            _secondSmash = false;
            _smashAnnounced = false;
            _aggressorSummoned = 0;
        }

        void Reset() override
        {
            _events.Reset();
            Initialize();
            me->setActive(true);
            me->SetReactState(REACT_PASSIVE);
            PhasingHandler::AddPhase(me, 543, true);
        }

        void JustEngagedWith(Unit* who) override
        {
            _events.ScheduleEvent(EVENT_SMASH, 8s);
            _events.ScheduleEvent(EVENT_RUPTURE, Seconds(urand(12, 16)));
        }

    void DamageTaken(Unit* attacker, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/)override
        {
            if (HealthAbovePct(85) && attacker->IsCreature())
                if (attacker->GetEntry() == NPC_AYSA_CLOUDSINGER_VORDRAKA)
                    damage = urand(1, 2);

            if (HealthBelowPct(85) && attacker->IsCreature())
                if (attacker->GetEntry() == NPC_AYSA_CLOUDSINGER_VORDRAKA)
                    me->SetHealth(me->GetHealth() + damage);

            if (!_playerParticipating && attacker->ToPlayer())
            {
                if (Creature* creature = me->FindNearestCreature(NPC_AYSA_CLOUDSINGER_VORDRAKA, me->GetVisibilityRange(), true))
                {
                    creature->AI()->SetData(DATA_AYSA_TALK_INTRO, DATA_AYSA_TALK_INTRO);
                    _playerParticipating = true;
                }
            }

            if (damage >= me->GetHealth())
            {
                //  std::list<HostileReference*> threatList;
               //   threatList = me->GetThreatManager().getThreatList();
               //   for (std::list<HostileReference*>::const_iterator itr = threatList.begin(); itr != threatList.end(); ++itr)
                    //  if (Player* target = (*itr)->getTarget()->ToPlayer())
                      //    if (target->GetQuestStatus(QUEST_AN_ANCIENT_EVIL) == QUEST_STATUS_INCOMPLETE)
                      //        target->KilledMonsterCredit(me->GetEntry());
            }
        }

        void JustDied(Unit* /*killer*/) override
        {
            std::list<Creature*> summonedAggressors;
            me->GetCreatureListWithEntryInGrid(summonedAggressors, NPC_DEEPSCALE_AGGRESSOR, me->GetVisibilityRange());
            for (std::list<Creature*>::const_iterator itr = summonedAggressors.begin(); itr != summonedAggressors.end(); ++itr)
                (*itr)->ToCreature()->DespawnOrUnsummon(100ms);

            if (Creature* creature = me->FindNearestCreature(NPC_AYSA_CLOUDSINGER_VORDRAKA, me->GetVisibilityRange(), true))
                creature->AI()->SetData(DATA_VORDRAKA_DEATH, DATA_VORDRAKA_DEATH);

            std::list<Player*> playersVisibility;
            me->GetPlayerListInGrid(playersVisibility, me->GetVisibilityRange());
            for (std::list<Player*>::const_iterator itr = playersVisibility.begin(); itr != playersVisibility.end(); ++itr)
                (*itr)->CastSpell((*itr), SPELL_SEE_DEATH_INVIS, true);

            DoCastSelf(SPELL_DEATH_INVIS, true);
            me->DespawnOrUnsummon(20s, Seconds(1));
        }

        void UpdateAI(uint32 diff) override
        {
            _events.Update(diff);

            if (!UpdateVictim())
                return;

            if (_aggressorSummoned == 0 && HealthBelowPct(60))
            {
                _events.ScheduleEvent(EVENT_SUMMON_AGGRESSOR, 0s);
                _events.ScheduleEvent(EVENT_SUMMON_AGGRESSOR, 0s);
            }

            if (_aggressorSummoned == 2 && HealthBelowPct(30))
            {
                _events.ScheduleEvent(EVENT_SUMMON_AGGRESSOR, 0s);
                _events.ScheduleEvent(EVENT_SUMMON_AGGRESSOR, Seconds(urand(1, 5)));
            }

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_SMASH:
                    DoCastVictim(SPELL_DEEP_SEA_SMASH);
                    _events.ScheduleEvent(EVENT_SMASH, 12s);
                    if (_playerParticipating && !_smashAnnounced)
                    {
                        if (_secondSmash)
                        {
                            if (Creature* creature = me->FindNearestCreature(NPC_AYSA_CLOUDSINGER_VORDRAKA, me->GetVisibilityRange(), true))
                            {
                                creature->AI()->SetData(DATA_AYSA_TALK_SMASH, DATA_AYSA_TALK_SMASH);
                                _smashAnnounced = true;
                            }
                        }
                        _secondSmash = true;
                    }
                    break;
                case EVENT_RUPTURE:
                    DoCast(SelectTarget(SelectTargetMethod::Random, 1), SPELL_DEEP_SEA_RUPTURE);
                    _events.ScheduleEvent(EVENT_RUPTURE, Seconds(urand(12, 16)));
                    break;
                case EVENT_SUMMON_AGGRESSOR:
                    DoCast(me, SPELL_FORCECAST_AGGRESSOR, true);
                    if (_aggressorSummoned == 0)
                        if (Creature* creature = me->FindNearestCreature(NPC_AYSA_CLOUDSINGER_VORDRAKA, me->GetVisibilityRange(), true))
                            creature->AI()->SetData(DATA_SUMMON_AGGRESSORS, DATA_SUMMON_AGGRESSORS);
                    _aggressorSummoned++;
                    break;
                }
            }

            DoMeleeAttackIfReady();
        }

    private:
        EventMap _events;
        bool _playerParticipating;
        bool _secondSmash;
        bool _smashAnnounced;
        uint8 _aggressorSummoned;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_vordrakaAI(creature);
    }
};

class spell_summon_deep_sea_aggressor : public SpellScriptLoader
{
public:
    spell_summon_deep_sea_aggressor() : SpellScriptLoader("spell_summon_deep_sea_aggressor") { }

    class spell_summon_deep_sea_aggressor_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_summon_deep_sea_aggressor_SpellScript);

        void HandleSummon(SpellEffIndex effIndex)
        {
            PreventHitDefaultEffect(effIndex);
            uint32 entry = GetSpellInfo()->GetEffect(effIndex).MiscValue;
            SummonPropertiesEntry const* properties = sSummonPropertiesStore.LookupEntry(GetSpellInfo()->GetEffect(effIndex).MiscValueB);
            int32 duration = GetSpellInfo()->GetDuration();

            if (!entry || !properties || !duration)
                return;

            Position const spawnPosition[15] =
            {
                { 313.9983f, 3973.418f, 86.55342f },
                { 249.4063f, 3972.389f, 75.72471f },
                { 316.1406f, 3979.06f, 85.13287f },
                { 252.6632f, 4008.125f, 78.23856f },
                { 266.5712f, 4014.581f, 79.36336f },
                { 269.8854f, 4017.54f, 79.76926f },
                { 271.9392f, 4018.929f, 79.99733f },
                { 309.474f, 3964.438f, 87.50405f },
                { 247.1337f, 3968.642f, 75.44573f },
                { 292.3837f, 3925.203f, 87.69834f },
                { 254.1892f, 3982.678f, 71.8816f },
                { 276.5608f, 4034.241f, 75.90926f },
                { 287.4236f, 3935.447f, 85.55875f },
                { 256.0226f, 3963.012f, 74.87388f },
                { 301.6267f, 3923.195f, 87.80573f }
            };

            if (TempSummon* summon = GetCaster()->GetMap()->SummonCreature(entry, spawnPosition[urand(0, 14)], properties, duration, GetCaster()))
                summon->SetTempSummonType(TEMPSUMMON_CORPSE_TIMED_DESPAWN);
        }

        void Register() override
        {
            OnEffectHit += SpellEffectFn(spell_summon_deep_sea_aggressor_SpellScript::HandleSummon, EFFECT_0, SPELL_EFFECT_SUMMON);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_summon_deep_sea_aggressor_SpellScript();
    }
};

class areatrigger_healing_sphere : public AreaTriggerEntityScript
{
public:
    areatrigger_healing_sphere() : AreaTriggerEntityScript("areatrigger_healing_sphere") { }

    struct areatrigger_healing_sphereAI : AreaTriggerAI
    {
        areatrigger_healing_sphereAI(AreaTrigger* areatrigger) : AreaTriggerAI(areatrigger) { }

        void OnUnitEnter(Unit* /*unit*/) override
        {
            at->SetDuration(0);
        }
    };

    AreaTriggerAI* GetAI(AreaTrigger* areatrigger) const override
    {
        return new areatrigger_healing_sphereAI(areatrigger);
    }
};

enum NpcHealersActive
{
    DATA_HEALER_ACTIVE = 1,
    DATA_HEALER_DIED = 2,
    WORLD_STATE_HEALERS_ACTIVE = 6488
};

class npc_healers_active_bunny : public CreatureScript
{
public:
    npc_healers_active_bunny() : CreatureScript("npc_healers_active_bunny") { }

    struct npc_healers_active_bunnyAI : public ScriptedAI
    {
        npc_healers_active_bunnyAI(Creature* creature) : ScriptedAI(creature) { }

        void Reset()
        {
            _healersActive = 0;
        }

        void SetData(uint32 id, uint32 value)
        {
            if (id == DATA_HEALER_ACTIVE)
            {
                if (_healersActive < 12)
                    _healersActive += value;
            }
            else if (id == DATA_HEALER_DIED)
            {
                if (_healersActive > 0)
                    _healersActive -= value;
            }

            Map::PlayerList const& playerList = me->GetMap()->GetPlayers();
            if (playerList.isEmpty())
                return;

            for (Map::PlayerList::const_iterator itr = playerList.begin(); itr != playerList.end(); ++itr)
                if (Player* player = itr->GetSource())
                    if (player->GetAreaId() == AREA_WANDERING_ISLE_WRECK_OF_THE_SKYSEEKER)
                        player->SendUpdateWorldState(WORLD_STATE_HEALERS_ACTIVE, _healersActive);
        }

        uint32 GetData(uint32 id) const
        {
            if (id == DATA_HEALER_ACTIVE)
                return _healersActive;

            return 0;
        }

    private:
        uint8 _healersActive;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_healers_active_bunnyAI(creature);
    }
};

enum HealingShenzinSuSpells
{
    SPELL_HEALING_SHENZIN_SU_CREDIT = 108898
};

class spell_healing_shenzin_su : public SpellScriptLoader
{
public:
    spell_healing_shenzin_su() : SpellScriptLoader("spell_healing_shenzin_su") { }

    class spell_healing_shenzin_su_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_healing_shenzin_su_AuraScript);

        void HandleEffectPeriodic(AuraEffect const* /*aurEff*/)
        {
            if (Unit* target = GetTarget())
            {
                // somehow get worldstate value, so the alternate power bar can fill based on active healers
                target->ModifyPower(POWER_ALTERNATE_POWER, 700);

                if (target->GetPowerPct(POWER_ALTERNATE_POWER) == 100)
                {
                    target->CastSpell(GetTarget(), SPELL_HEALING_SHENZIN_SU_CREDIT, true);
                    target->RemoveAura(GetId());
                }
            }
        }

        void Register() override
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_healing_shenzin_su_AuraScript::HandleEffectPeriodic, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_healing_shenzin_su_AuraScript();
    }
};

enum TurtleHealedPhaseTimerSpells
{
    SPELL_TURTLE_HEALED_PHASE_UPDATE = 118232
};

class spell_turtle_healed_phase_timer : public SpellScriptLoader
{
public:
    spell_turtle_healed_phase_timer() : SpellScriptLoader("spell_turtle_healed_phase_timer") { }

    class spell_turtle_healed_phase_timer_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_turtle_healed_phase_timer_AuraScript);

        void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            if (Unit* target = GetTarget())
                if (target->IsPlayer())
                    target->CastSpell(GetTarget(), SPELL_TURTLE_HEALED_PHASE_UPDATE, true);
        }

        void Register() override
        {
            AfterEffectRemove += AuraEffectRemoveFn(spell_turtle_healed_phase_timer_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_turtle_healed_phase_timer_AuraScript();
    }
};

enum AllyHordeArgumentNPCs
{
    NPC_KORGA_STRONGMANE = 60888,
    NPC_DELORA_LIONHEART = 60889
};

class spell_ally_horde_argument : public SpellScriptLoader
{
public:
    spell_ally_horde_argument() : SpellScriptLoader("spell_ally_horde_argument") { }

    class spell_ally_horde_argument_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_ally_horde_argument_AuraScript);

        void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            if (Unit* target = GetTarget())
                if (Creature* creature = target->FindNearestCreature(NPC_KORGA_STRONGMANE, 40.0f, true))
                    creature->AI()->Talk(0, target);
        }

        void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            if (Unit* target = GetTarget())
                if (Creature* creature = target->FindNearestCreature(NPC_DELORA_LIONHEART, 40.0f, true))
                    creature->AI()->Talk(0, target);
        }

        void Register() override
        {
            OnEffectApply += AuraEffectApplyFn(spell_ally_horde_argument_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            OnEffectRemove += AuraEffectRemoveFn(spell_ally_horde_argument_AuraScript::OnRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_ally_horde_argument_AuraScript();
    }
};

class spell_pandaren_faction_choice : public SpellScriptLoader
{
public:
    spell_pandaren_faction_choice() : SpellScriptLoader("spell_pandaren_faction_choice") { }

    class spell_pandaren_faction_choice_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_pandaren_faction_choice_SpellScript);

        void HandleScriptEffect(SpellEffIndex /*effIndex*/)
        {
            if (Unit* caster = GetCaster())
                if (caster->IsPlayer())
                    caster->ToPlayer()->ShowNeutralPlayerFactionSelectUI();
        }

        void Register() override
        {
            OnEffectHit += SpellEffectFn(spell_pandaren_faction_choice_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_pandaren_faction_choice_SpellScript();
    }
};

class spell_faction_choice_trigger : public SpellScriptLoader
{
public:
    spell_faction_choice_trigger() : SpellScriptLoader("spell_faction_choice_trigger") { }

    class spell_faction_choice_trigger_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_faction_choice_trigger_AuraScript);

        void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            if (Unit* target = GetTarget())
                if (target->IsPlayer())
                    target->CastSpell(target, GetSpellInfo()->GetEffect(EFFECT_0).BasePoints, true);
        }

        void Register() override
        {
            OnEffectRemove += AuraEffectRemoveFn(spell_faction_choice_trigger_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_SCREEN_EFFECT, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_faction_choice_trigger_AuraScript();
    }
};

enum BalloonExitTimerSpells
{
    SPELL_BALOON_EXIT_MOVIE = 132212
};

class spell_balloon_exit_timer : public SpellScriptLoader
{
public:
    spell_balloon_exit_timer() : SpellScriptLoader("spell_balloon_exit_timer") { }

    class spell_balloon_exit_timer_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_balloon_exit_timer_AuraScript);

        void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            if (Unit* target = GetTarget())
                if (target->IsPlayer())
                    target->CastSpell(target, SPELL_BALOON_EXIT_MOVIE, true);
        }

        void Register() override
        {
            OnEffectRemove += AuraEffectRemoveFn(spell_balloon_exit_timer_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_balloon_exit_timer_AuraScript();
    }
};

class npc_ji_firepaw_escort : public CreatureScript
{
public:
    npc_ji_firepaw_escort() : CreatureScript("npc_ji_firepaw_escort") { }

    struct npc_ji_firepaw_escortAI : public ScriptedAI
    {
        npc_ji_firepaw_escortAI(Creature* creature) : ScriptedAI(creature)
        {
            playerGuid = 0;
        }

        uint64 playerGuid;

        void IsSummonedBy(WorldObject* summoner) override
        {
            //playerGuid = summoner->GetGUID();
          //  me->GetMotionMaster()->MoveFollow(summoner->ToUnit(), 1.0f, 1.0f, MOTION_SLOT_ACTIVE);
        }

        void UpdateAI(const uint32 diff)
        {
            Player* summoner;
            //Player* summoner = sObjectAccessor->FindPlayer(playerGuid);
//            if (!summoner)
            return;

            if (Unit* target = summoner->GetVictim())
            {
                me->Attack(target, true);
                DoMeleeAttackIfReady();
                return;
            }

            if ((summoner->GetQuestStatus(29780) == QUEST_STATUS_COMPLETE || summoner->GetQuestStatus(29780) == QUEST_STATUS_REWARDED) && (summoner->GetQuestStatus(29779) == QUEST_STATUS_COMPLETE
                || summoner->GetQuestStatus(29779) == QUEST_STATUS_REWARDED) && (summoner->GetQuestStatus(29781) == QUEST_STATUS_COMPLETE || summoner->GetQuestStatus(29781) == QUEST_STATUS_REWARDED))
                me->DespawnOrUnsummon();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ji_firepaw_escortAI(creature);
    }
};

class npc_ji_firepaw : public CreatureScript
{
public:
    npc_ji_firepaw() : CreatureScript("npc_ji_firepaw")
    {
        isSummoned = false;
    }

    bool isSummoned;

    struct npc_ji_firepawAI : public ScriptedAI
    {
        npc_ji_firepawAI(Creature* creature) : ScriptedAI(creature) { }
    };
    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_ji_firepawAI(creature);
    }

    void SummonHiFirepawHelper(Player* summoner, uint32 entry)
    {
        //uint32 phase = summoner->GetPhaseMask();
        uint32 team = summoner->GetTeam();
        Position pos;

        // summoner->GetPosition(&pos);

        Guardian* summon = new Guardian(NULL, summoner, false);

        //  if (!summon->Create(sObjectMgr->GenerateLowGuid(HIGHGUID_UNIT), summoner->GetMap(), phase, entry, 0, team, pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), pos.GetOrientation()))
        {
            delete summon;
            return;
        }



        summon->SetHomePosition(pos);
        summon->InitStats(summoner, 0);
        summoner->GetMap()->AddToMap(summon->ToCreature());
        summon->InitSummon(summoner);
        summon->InitStatsForLevel(10);
        summon->SetFollowAngle(summoner->GetAbsoluteAngle(summon));
        summon->SetReactState(REACT_AGGRESSIVE);

    }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest) override
    {
        std::list<Creature*> summonList;
        GetCreatureListWithEntryInGrid(summonList, player, 59960, 6.0f);

        for (auto summoned : summonList)
            isSummoned = true;

        if (isSummoned == false)
        {
            if (quest->GetQuestId() == 29779)
            {
                SummonHiFirepawHelper(player, 59960);
                isSummoned = true;
            }

            if (quest->GetQuestId() == 29780)
            {
                SummonHiFirepawHelper(player, 59960);
                isSummoned = true;
            }

            if (quest->GetQuestId() == 29781)
            {
                SummonHiFirepawHelper(player, 59960);
                isSummoned = true;
            }
        }

        return true;
    }
};

class mob_tushui_trainee : public CreatureScript
{
public:
    mob_tushui_trainee() : CreatureScript("mob_tushui_trainee") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_tushui_trainee_AI(creature);
    }

    struct mob_tushui_trainee_AI : public ScriptedAI
    {
        mob_tushui_trainee_AI(Creature* creature) : ScriptedAI(creature) {}

        enum data
        {
            EVENT_1 = 1,
            EVENT_2 = 2,
            EVENT_3 = 3,
            SPELL = 109080,
        };

        void Reset() override
        {
            events.Reset();
            me->SetReactState(REACT_DEFENSIVE);
            if (!me->isMoving())
                me->RemoveUnitFlag(UnitFlags(UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_UNINTERACTIBLE));
            events.RescheduleEvent(EVENT_3, 0s);
        }

        void JustEngagedWith(Unit* unit) override
        {
            events.CancelEvent(EVENT_2);
            events.CancelEvent(EVENT_3);
            events.RescheduleEvent(EVENT_1, 4s);
        }

    void DamageTaken(Unit* attacker, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/)override
        {
            if (me->HealthBelowPctDamaged((int32)25, damage))
            {
                if (attacker->IsPlayer())
                    attacker->ToPlayer()->KilledMonsterCredit(54586);
                me->SetUnitFlag(UnitFlags(UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_UNINTERACTIBLE));
                me->CombatStop();
                me->HandleEmoteCommand(EMOTE_ONESHOT_SALUTE);
                me->AI()->Talk(0);
                me->SetFullHealth();
                me->DespawnOrUnsummon(3s);
                damage = 0;
            }
        }

        void UpdateAI(uint32 diff) override
        {
            UpdateVictim();

            events.Update(diff);

            if (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_1:
                    if (me->GetVictim())
                        me->CastSpell(me->GetVictim(), SPELL, true);
                    events.RescheduleEvent(EVENT_1, 4s);
                    break;
                case EVENT_2:
                    me->HandleEmoteCommand(EMOTE_ONESHOT_MONKOFFENSE_ATTACKUNARMEDOFF);
                    events.RescheduleEvent(EVENT_3, 1s);
                    break;
                case EVENT_3:
                    me->HandleEmoteCommand(EMOTE_STATE_MONKOFFENSE_READYUNARMED);
                    events.RescheduleEvent(EVENT_2, 3s + Seconds((urand(0, 3))));
                    break;
                }
            }
            DoMeleeAttackIfReady();
        }
        EventMap events;
    };
};

// Should be done by summon npc 59591
class mob_master_shang_xi : public CreatureScript
{
public:
    mob_master_shang_xi() : CreatureScript("mob_master_shang_xi") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest) override
    {
        if (quest->GetQuestId() == 29408) // La lecon du parchemin brulant
        {
            creature->AddAura(114610, creature);
            creature->RemoveUnitFlag(UnitFlags(UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER));
            creature->SetUnitFlag(UnitFlags(UNIT_NPC_FLAG_SPELLCLICK));
            player->KilledMonsterCredit(59591);
        }

        return true;
    }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new mob_master_shang_xi_AI(creature);
    }

    struct mob_master_shang_xi_AI : public ScriptedAI
    {
        mob_master_shang_xi_AI(Creature* creature) : ScriptedAI(creature)
        {
            checkPlayersTime = 2000;
        }

        uint32 checkPlayersTime;

        void OnQuestReward(Player* player, Quest const* quest, LootItemType /*type*/, uint32 /*opt*/) override
        {
            if (quest->GetQuestId() == 30027 || (quest->GetQuestId() >= 30033 && quest->GetQuestId() <= 30038))
            {
                Quest const* quest_ = sObjectMgr->GetQuestTemplate(29406);
                player->AddQuest(quest_, me);
            }
        }

        void SpellHit(WorldObject* caster, const SpellInfo* pSpell) override
        {
            if (pSpell->Id == 114746) // Attraper la flamme
            {
                if (caster->GetTypeId() == TYPEID_PLAYER)
                {
                    if (caster->ToPlayer()->GetQuestStatus(29408) == QUEST_STATUS_INCOMPLETE)
                    {
                        me->CastSpell(caster, 114611, true);
                        me->RemoveAurasDueToSpell(114610);
                        me->SetUnitFlag(UnitFlags(UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER));
                        me->RemoveUnitFlag(UnitFlags(UNIT_NPC_FLAG_SPELLCLICK));
                    }
                }
            }
        }

        void UpdateAI(uint32 diff) override
        {
            if (checkPlayersTime <= diff)
            {
                std::list<Player*> playerList;
                GetPlayerListInGrid(playerList, me, 5.0f);

                bool playerWithQuestNear = false;

                for (std::list<Player*>::const_iterator itr = playerList.begin(); itr != playerList.end(); ++itr)
                {
                    if ((*itr)->GetQuestStatus(29408) == QUEST_STATUS_INCOMPLETE && !(*itr)->HasItemCount(80212))
                        playerWithQuestNear = true;
                }

                if (playerWithQuestNear && !me->HasAura(114610))
                {
                    me->AddAura(114610, me);
                    me->RemoveUnitFlag(UnitFlags(UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER));
                    me->SetUnitFlag(UnitFlags(UNIT_NPC_FLAG_SPELLCLICK));
                }
                else if (!playerWithQuestNear && me->HasAura(114610))
                {
                    me->RemoveAurasDueToSpell(114610);
                    me->SetUnitFlag(UnitFlags(UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER));
                    me->RemoveUnitFlag(UnitFlags(UNIT_NPC_FLAG_SPELLCLICK));
                }

                checkPlayersTime = 2000;
            }
            else
                checkPlayersTime -= diff;
        }
    };
};

// cast 88811 for check something
class boss_jaomin_ro : public CreatureScript
{
public:
    boss_jaomin_ro() : CreatureScript("boss_jaomin_ro") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_jaomin_roAI(creature);
    }

    struct boss_jaomin_roAI : public ScriptedAI
    {
        boss_jaomin_roAI(Creature* creature) : ScriptedAI(creature) {}

        enum eEvents
        {
            EVENT_JAOMIN_JUMP = 1,
            EVENT_HIT_CIRCLE = 2,
            EVENT_FALCON = 3,
            EVENT_RESET = 4,
        };

        EventMap events;
        bool isInFalcon;
        bool fightEnd;

        void JustEngagedWith(Unit* unit)  override
        {
            events.RescheduleEvent(EVENT_JAOMIN_JUMP, 1s);
            events.RescheduleEvent(EVENT_HIT_CIRCLE, 2s);
        }

        void Reset() override
        {
            events.Reset();
            me->SetReactState(REACT_DEFENSIVE);
            me->SetDisplayId(39755);
            me->RemoveUnitFlag(UnitFlags(UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_UNINTERACTIBLE));
            isInFalcon = false;
            fightEnd = false;
        }

    void DamageTaken(Unit* attacker, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/)override
        {
            if (me->HealthBelowPctDamaged(30, damage) && !isInFalcon)
            {
                isInFalcon = true;
                me->SetDisplayId(39796); //faucon
                events.RescheduleEvent(EVENT_FALCON, 1s);
                events.CancelEvent(EVENT_JAOMIN_JUMP);
                events.CancelEvent(EVENT_HIT_CIRCLE);
            }

            if (me->HealthBelowPctDamaged(5, damage) && !fightEnd)
            {
                std::list<Player*> playerList;
                GetPlayerListInGrid(playerList, me, 10.0f);
                for (std::list<Player*>::const_iterator itr = playerList.begin(); itr != playerList.end(); ++itr)
                    (*itr)->KilledMonsterCredit(me->GetEntry(), ObjectGuid::Empty);

                fightEnd = true;
                me->AttackStop();
                me->SetFullHealth();
                me->SetUnitFlag(UnitFlags(UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_UNINTERACTIBLE));
                attacker->AttackStop();
                me->SetDisplayId(39755);
                me->HandleEmoteCommand(EMOTE_ONESHOT_SALUTE);
                events.Reset();
                events.RescheduleEvent(EVENT_RESET, 9s);
                DoCast(me, 108959, true);
                me->DespawnOrUnsummon(3s);
                damage = 0;
            }

            if (damage >= me->GetHealth())
                damage = 0;
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_JAOMIN_JUMP:
                    DoCastVictim(108938);
                    events.RescheduleEvent(EVENT_JAOMIN_JUMP, 30s);
                    break;
                case EVENT_HIT_CIRCLE:
                    DoCastVictim(119301);
                    events.RescheduleEvent(EVENT_HIT_CIRCLE, 8s);
                    break;
                case EVENT_FALCON:
                    DoCast(108935);
                    events.RescheduleEvent(EVENT_FALCON, 6s);
                    break;
                case EVENT_RESET:
                    me->NearTeleportTo(me->GetHomePosition());
                    EnterEvadeMode();
                    break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };
};

class go_break_gong : public GameObjectScript
{
public:
    go_break_gong() : GameObjectScript("go_break_gong") { }

    struct go_break_gongAI : public GameObjectAI
    {
        go_break_gongAI(GameObject* go) : GameObjectAI(go) {}

        bool OnReportUse(Player* player) override
        {
            if (player->GetQuestStatus(29772) == QUEST_STATUS_INCOMPLETE)
            {
                player->KilledMonsterCredit(55546, ObjectGuid::Empty);

                if (Creature* wugou = player->FindNearestCreature(55539, 20.0f, true))
                {
                    wugou->AI()->Talk(0);
                    player->GetScheduler().Schedule(Milliseconds(5000), [player](TaskContext context)
                        {
                            if (Creature* firepaw = player->FindNearestCreature(55477, 20.0f, true))
                                firepaw->AI()->Talk(11);
                        });
                }
            }
            return true;
        }
    };

    GameObjectAI* GetAI(GameObject* go) const override
    {
        return new go_break_gongAI(go);
    }
};


/*######
## mob_min_dimwind_final - 54785
######*/

enum eMinDimwindFinalEvents
{
    EVENT_SUMMON_SCAMP_1 = 1,
    EVENT_KEG_CARRY_1 = 2,
    EVENT_RUNAWAY_1 = 3,
    EVENT_SUMMON_SCAMP_2 = 4,
    EVENT_KEG_CARRY_2 = 5,
    EVENT_RUNAWAY_2 = 6,
    EVENT_SUMMON_SCAMP_3 = 7,
    EVENT_KEG_CARRY_3 = 8,
    EVENT_RUNAWAY_3 = 9
};

enum eMiscMinDimwindFinal
{
    NPC_AMBERLEAF_SCAMP_ = 54130,
    SPELL_KEG_CARRY = 109137,
    QUEST_THE_MISSING_DRIVER_ = 29419
};

// Thift Amberleaf Scamp positions this are near.
Position const ThiftScampPoints[] =
{
    { 1293.51f, 3528.53f, 98.0433f, 4.67748f }, // Thift Amberleaf Scamp 1
    { 1294.22f, 3519.09f, 100.127f, 2.02458f }, // Thift Amberleaf Scamp 2
    { 1288.38f, 3528.15f, 96.9992f, 5.27089f }, // Thift Amberleaf Scamp 3
};

// Positions were the Amberleaf Scamp Thift will run away.
Position const ThiftEscapePoints[] =
{
    { 1342.825928f, 3514.124268f, 101.61726f }, // Amberleaf Scamp Thift 1
    { 1325.679932f, 3471.875244f, 112.65136f }, // Amberleaf Scamp Thift 2
    { 1251.993408f, 3560.332275f, 101.59293f }  // Amberleaf Scamp Thift 3
};

class mob_min_dimwind_final : public CreatureScript
{
public:
    mob_min_dimwind_final() : CreatureScript("mob_min_dimwind_final") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_min_dimwind_finalAI(creature);
    }

    struct mob_min_dimwind_finalAI : public ScriptedAI
    {
        mob_min_dimwind_finalAI(Creature* creature) : ScriptedAI(creature) {}

        Creature* thiftScamp1;
        Creature* thiftScamp2;
        Creature* thiftScamp3;

        uint64 m_checkTimer;

        void Reset() override
        {
            m_checkTimer = 2000;
            events.Reset();
        }

        void JustEngagedWith(Unit* /*who*/) override
        {
            events.ScheduleEvent(EVENT_SUMMON_SCAMP_1, 4s);
        }

        void UpdateAI(const uint32 diff) override
        {
            events.Update(diff);

            if (m_checkTimer <= diff)
            {
                std::list<Player*> PlayerList;
                GetPlayerListInGrid(PlayerList, me, 200.0f);
                for (auto cPlayer : PlayerList)
                {
                    if (cPlayer->GetQuestStatus(QUEST_THE_MISSING_DRIVER_) != QUEST_STATUS_REWARDED)
                    {
                        me->DestroyForPlayer(cPlayer);
                    }

                    if (cPlayer->GetQuestStatus(QUEST_THE_MISSING_DRIVER_) == QUEST_STATUS_REWARDED)
                    {
                        me->ToUnit()->UpdateObjectVisibility(true);
                    }
                }

                m_checkTimer = 2500;
            }
            else
            {
                m_checkTimer -= diff;
            }

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_SUMMON_SCAMP_1:
                    if (auto _thiftScamp1 = me->SummonCreature(NPC_AMBERLEAF_SCAMP_, ThiftScampPoints[0], TEMPSUMMON_TIMED_DESPAWN, 15s))
                    {
                        thiftScamp1 = _thiftScamp1;
                        PhasingHandler::AddPhase(thiftScamp1, 65504, true);
                    }

                    events.ScheduleEvent(EVENT_KEG_CARRY_1, Seconds(irand(2, 4)));
                    events.CancelEvent(EVENT_SUMMON_SCAMP_1);
                    break;
                case EVENT_KEG_CARRY_1:
                    if (thiftScamp1)
                        thiftScamp1->CastSpell(thiftScamp1, SPELL_KEG_CARRY, true);

                    events.ScheduleEvent(EVENT_RUNAWAY_1, Seconds(irand(4, 6)));
                    events.CancelEvent(EVENT_KEG_CARRY_1);
                    break;
                case EVENT_RUNAWAY_1:
                    if (thiftScamp1)
                        thiftScamp1->GetMotionMaster()->MovePoint(0, ThiftEscapePoints[0]);
                    events.ScheduleEvent(EVENT_SUMMON_SCAMP_2, Seconds(irand(2, 6)));
                    events.CancelEvent(EVENT_RUNAWAY_1);
                    break;
                case EVENT_SUMMON_SCAMP_2:
                    if (auto _thiftScamp2 = me->SummonCreature(NPC_AMBERLEAF_SCAMP_, ThiftScampPoints[1], TEMPSUMMON_TIMED_DESPAWN, 15s))
                    {
                        thiftScamp2 = _thiftScamp2;
                        PhasingHandler::AddPhase(thiftScamp2, 65504, true);
                    }

                    events.ScheduleEvent(EVENT_KEG_CARRY_2, Seconds(irand(2, 4)));
                    events.CancelEvent(EVENT_SUMMON_SCAMP_2);
                    break;
                case EVENT_KEG_CARRY_2:
                    if (thiftScamp2)
                        thiftScamp2->CastSpell(thiftScamp2, SPELL_KEG_CARRY, true);

                    events.ScheduleEvent(EVENT_RUNAWAY_2, Seconds(irand(4, 6)));
                    events.CancelEvent(EVENT_KEG_CARRY_2);
                    break;
                case EVENT_RUNAWAY_2:
                    if (thiftScamp2)
                        thiftScamp2->GetMotionMaster()->MovePoint(0, ThiftEscapePoints[1]);

                    events.ScheduleEvent(EVENT_SUMMON_SCAMP_3, Seconds(irand(2, 6)));
                    events.CancelEvent(EVENT_RUNAWAY_2);
                    break;
                case EVENT_SUMMON_SCAMP_3:
                    if (auto _thiftScamp3 = me->SummonCreature(NPC_AMBERLEAF_SCAMP_, ThiftScampPoints[2], TEMPSUMMON_TIMED_DESPAWN, 15s))
                    {
                        thiftScamp3 = _thiftScamp3;
                        PhasingHandler::AddPhase(thiftScamp3, 65504, true);
                    }

                    events.ScheduleEvent(EVENT_KEG_CARRY_3, Seconds(irand(2, 4)));
                    events.CancelEvent(EVENT_SUMMON_SCAMP_3);
                    break;
                case EVENT_KEG_CARRY_3:
                    if (thiftScamp3)
                        thiftScamp3->CastSpell(thiftScamp3, SPELL_KEG_CARRY, true);

                    events.ScheduleEvent(EVENT_RUNAWAY_3, Seconds(irand(4, 6)));
                    events.CancelEvent(EVENT_KEG_CARRY_3);
                    break;
                case EVENT_RUNAWAY_3:
                    if (thiftScamp3)
                        thiftScamp3->GetMotionMaster()->MovePoint(0, ThiftEscapePoints[2]);

                    events.ScheduleEvent(EVENT_SUMMON_SCAMP_1, Seconds(irand(2, 6)));
                    events.CancelEvent(EVENT_RUNAWAY_3);
                    break;
                default:
                    break;
                }
            }
        }
    private:
        EventMap events;
    };
};

/*######
## mob_min_dimwind_summon - 56503
######*/

enum eMiscMinDimwindSummon
{
    NPC_MIN_DIMWIND = 54855,
};

enum eSayMinDimwindSummon
{
    SAY_AMBERLEAF_1 = 0, // Look out! Its friendses are coming!
    SAY_AMBERLEAF_2 = 1, // Run away!

    SAY_MIN_DIMWIND_1 = 0, // Master Shang has trained you well. Thank you, Friend!
    SAY_MIN_DIMWIND_2 = 1, // I couldn't have fougth off alone. Now, if you'll excuse me, I shoul go find my cart.
    SAY_MIN_DIMWIND_3 = 2, // Cart!
    SAY_MIN_DIMWIND_4 = 3, // Hello, cart. Still upside-downed, I see.
};

enum eEventsMinDimwind
{
    EVENT_AMBERLEAF_RUN_AWAY_1 = 1,
    EVENT_AMBERLEAF_RUN_AWAY_2 = 2,
    EVENT_DIMWIND_SAY_1 = 3,
    EVENT_DIMWIND_SAY_2 = 4,
    EVENT_DIMWIND_RUN_1 = 5,
    EVENT_DIMWIND_SAY_3 = 6,
    EVENT_DIMWIND_RUN_2 = 7,
    EVENT_DIMWIND_SAY_4 = 8,
    EVENT_DIMWIND_RUN_3 = 9,
    EVENT_DIMWIND_RUN_4 = 10,
    EVENT_DIMWIND_RUN_5 = 11,
    EVENT_DIMWIND_SAY_5 = 12,
    EVENT_DIMWIND_RUN_6 = 13,
    EVENT_DIMWIND_RUN_7 = 14,
    EVENT_DIMWIND_END = 15,
};

// Positions were the Amberleaf Scamp will run away.
Position const EscapePoints[] =
{
    { 1462.280762f, 3571.749268f, 87.852112f }, // Amberleaf Scamp 1
    { 1412.113770f, 3581.308594f, 89.331223f }, // Amberleaf Scamp 2
    { 1390.180664f, 3581.708008f, 91.498085f }, // Amberleaf Scamp 3
    { 1371.005249f, 3575.204102f, 91.947105f }, // Amberleaf Scamp 4
    { 1374.989258f, 3539.136230f, 93.007309f }, // Amberleaf Scamp 5
};

// Min Dimwind summon WayPoints
Position const MinDimiwindRunPoints[] =
{
    { 1397.118652f, 3548.915283f, 90.728721f },
    { 1373.306885f, 3581.243896f, 91.602699f },
    { 1354.650513f, 3583.795898f, 89.320351f },
    { 1323.678467f, 3538.115967f, 98.475006f },
    { 1299.884277f, 3527.468018f, 98.938644f },
    { 1295.715942f, 3520.725098f, 99.892564f },
    { 1286.92f, 3521.41f, 97.880f }
};

class mob_min_dimwind_summon : public CreatureScript
{
public:
    mob_min_dimwind_summon() : CreatureScript("mob_min_dimwind_summon") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_min_dimwind_summonAI(creature);
    }

    struct mob_min_dimwind_summonAI : public ScriptedAI
    {
        mob_min_dimwind_summonAI(Creature* creature) : ScriptedAI(creature) { }

        ObjectGuid playerOwnerGUID;

        ObjectGuid scamp1;
        ObjectGuid scamp2;
        ObjectGuid scamp3;
        ObjectGuid scamp4;
        ObjectGuid scamp5;
        ObjectGuid dimwind;

        uint64 m_checkTimer;

        bool startEvent;

        void Reset()
        {
            me->SetVisible(false);
            playerOwnerGUID.Clear();
            m_checkTimer = 1000;
            startEvent = false;
        }

        void UpdateAI(const uint32 diff)
        {
            events.Update(diff);

            if (m_checkTimer <= diff)
            {
                std::list<Player*> PlayerList;
                GetPlayerListInGrid(PlayerList, me, 25.0f);
                for (auto _cPlayer : PlayerList)
                {
                    if (_cPlayer->GetGUID() == playerOwnerGUID)
                    {
                        if (!startEvent)
                        {
                            events.ScheduleEvent(EVENT_AMBERLEAF_RUN_AWAY_1, 2500ms);
                            startEvent = true;
                        }
                    }
                }
                //Despawn after owner get rewarded
                if (Unit* _cPlayerOwner = ObjectAccessor::GetPlayer(me->GetMap(), playerOwnerGUID))
                {
                    if (_cPlayerOwner->ToPlayer()->GetQuestStatus(QUEST_THE_MISSING_DRIVER_) == QUEST_STATUS_REWARDED)
                        me->DespawnOrUnsummon(1s);
                }
                m_checkTimer = 1000;
            }
            else
            {
                m_checkTimer -= diff;
            }

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_AMBERLEAF_RUN_AWAY_1:
                {
                    // Make the Scamps 1 run away.
                    if (Unit* aScamp1 = ObjectAccessor::GetUnit(*me, scamp1))
                    {
                        aScamp1->GetMotionMaster()->MovePoint(0, EscapePoints[0]);
                        aScamp1->ToCreature()->DespawnOrUnsummon(15s);
                    }
                    // Make the Scamps 2 run away.
                    if (Unit* aScamp2 = ObjectAccessor::GetUnit(*me, scamp2))
                    {
                        aScamp2->ToCreature()->AI()->Talk(SAY_AMBERLEAF_1);
                        aScamp2->GetMotionMaster()->MovePoint(0, EscapePoints[1]);
                        aScamp2->ToCreature()->DespawnOrUnsummon(15s);
                    }
                    // Make the Scamps 3 run away.
                    if (Unit* aScamp3 = ObjectAccessor::GetUnit(*me, scamp3))
                    {
                        aScamp3->GetMotionMaster()->MovePoint(0, EscapePoints[2]);
                        aScamp3->ToCreature()->DespawnOrUnsummon(15s);
                    }
                    // Make the Scamps 4 run away.
                    if (Unit* aScamp4 = ObjectAccessor::GetUnit(*me, scamp4))
                    {
                        aScamp4->GetMotionMaster()->MovePoint(0, EscapePoints[3]);
                        aScamp4->ToCreature()->DespawnOrUnsummon(15s);
                    }
                    // Complete Quest by giving kill monster credit
                    if (Unit* cPlayerOwner = ObjectAccessor::GetPlayer(me->GetMap(), playerOwnerGUID))
                    {
                        cPlayerOwner->ToPlayer()->KilledMonsterCredit(NPC_MIN_DIMWIND);
                        if (Unit* aDimwind = ObjectAccessor::GetUnit(*me, dimwind))
                        {
                            aDimwind->DestroyForPlayer(cPlayerOwner->ToPlayer());
                        }
                    }
                    // End
                    me->HandleEmoteCommand(EMOTE_STATE_STAND);
                    me->SetVisible(true);
                    events.ScheduleEvent(EVENT_AMBERLEAF_RUN_AWAY_2, 2s);
                    events.CancelEvent(EVENT_AMBERLEAF_RUN_AWAY_1);
                } break;
                case EVENT_AMBERLEAF_RUN_AWAY_2:
                    if (Unit* aScamp5 = ObjectAccessor::GetUnit(*me, scamp5))
                    {
                        aScamp5->ToCreature()->AI()->Talk(SAY_AMBERLEAF_2);
                        aScamp5->GetMotionMaster()->MovePoint(0, EscapePoints[4]);
                        aScamp5->ToCreature()->DespawnOrUnsummon(15s);
                    }
                    events.ScheduleEvent(EVENT_DIMWIND_SAY_1, 2200ms);
                    events.CancelEvent(EVENT_AMBERLEAF_RUN_AWAY_2);
                    break;
                case EVENT_DIMWIND_SAY_1:
                    Talk(SAY_MIN_DIMWIND_1);
                    events.ScheduleEvent(EVENT_DIMWIND_SAY_2, 4s);
                    events.CancelEvent(EVENT_DIMWIND_SAY_1);
                    break;
                case EVENT_DIMWIND_SAY_2:
                    Talk(SAY_MIN_DIMWIND_2);
                    events.ScheduleEvent(EVENT_DIMWIND_RUN_1, 3200ms);
                    events.CancelEvent(EVENT_DIMWIND_SAY_2);
                    break;
                case EVENT_DIMWIND_RUN_1:
                    me->GetMotionMaster()->MovePoint(0, MinDimiwindRunPoints[0]);
                    events.ScheduleEvent(EVENT_DIMWIND_SAY_3, 3s);
                    events.CancelEvent(EVENT_DIMWIND_RUN_1);
                    break;
                case EVENT_DIMWIND_SAY_3:
                    Talk(SAY_MIN_DIMWIND_3);
                    events.ScheduleEvent(EVENT_DIMWIND_RUN_2, 2s);
                    events.CancelEvent(EVENT_DIMWIND_SAY_3);
                    break;
                case EVENT_DIMWIND_RUN_2:
                    me->GetMotionMaster()->MovePoint(0, MinDimiwindRunPoints[1]);
                    events.ScheduleEvent(EVENT_DIMWIND_SAY_4, 5200ms);
                    events.CancelEvent(EVENT_DIMWIND_RUN_2);
                    break;
                case EVENT_DIMWIND_SAY_4:
                    Talk(SAY_MIN_DIMWIND_3);
                    events.ScheduleEvent(EVENT_DIMWIND_RUN_3, 2s);
                    events.CancelEvent(EVENT_DIMWIND_SAY_4);
                    break;
                case EVENT_DIMWIND_RUN_3:
                    me->GetMotionMaster()->MovePoint(0, MinDimiwindRunPoints[2]);
                    events.ScheduleEvent(EVENT_DIMWIND_RUN_4, 2300ms);
                    events.CancelEvent(EVENT_DIMWIND_RUN_3);
                    break;
                case EVENT_DIMWIND_RUN_4:
                    me->GetMotionMaster()->MovePoint(0, MinDimiwindRunPoints[3]);
                    events.ScheduleEvent(EVENT_DIMWIND_RUN_5, 6600ms);
                    events.CancelEvent(EVENT_DIMWIND_RUN_4);
                    break;
                case EVENT_DIMWIND_RUN_5:
                    me->GetMotionMaster()->MovePoint(0, MinDimiwindRunPoints[4]);
                    events.ScheduleEvent(EVENT_DIMWIND_SAY_5, 4s);
                    events.CancelEvent(EVENT_DIMWIND_RUN_5);
                    break;
                case EVENT_DIMWIND_SAY_5:
                    Talk(SAY_MIN_DIMWIND_4);
                    events.ScheduleEvent(EVENT_DIMWIND_RUN_6, 1500ms);
                    events.CancelEvent(EVENT_DIMWIND_SAY_5);
                    break;
                case EVENT_DIMWIND_RUN_6:
                    me->GetMotionMaster()->MovePoint(0, MinDimiwindRunPoints[5]);
                    me->SetSpeed(MOVE_RUN, 0.5f);
                    events.ScheduleEvent(EVENT_DIMWIND_RUN_7, 3300ms);
                    events.CancelEvent(EVENT_DIMWIND_RUN_6);
                    break;
                case EVENT_DIMWIND_RUN_7:
                    me->GetMotionMaster()->MovePoint(0, MinDimiwindRunPoints[6]);
                    events.ScheduleEvent(EVENT_DIMWIND_END, 3s);
                    events.CancelEvent(EVENT_DIMWIND_RUN_7);
                    break;
                case EVENT_DIMWIND_END:
                    me->SetFacingTo(0.869715f);
                    events.CancelEvent(EVENT_DIMWIND_END);
                    break;
                default:
                    break;
                }
            }
        }
    private:
        EventMap events;
    };
};

/*######
## mob_min_dimwind - 54855
######*/

enum eMiscMinDimwind
{
    NPC_MIN_DIMWIND_SUMMON = 56503,
    NPC_AMBERLEAF_SCAMP = 54130,
    QUEST_THE_MISSING_DRIVER = 29419
};

// Amberleaf Scamp temporaly summoned positions, this positions are constants, not randomly created.
Position const AmberleafScampPoints[] =
{
    { 1418.814941f, 3538.107422f, 85.971985f, 4.17430f }, // Amberleaf Scamp 1
    { 1413.725342f, 3542.623047f, 87.532526f, 4.92588f }, // Amberleaf Scamp 2
    { 1411.864380f, 3540.173340f, 87.579727f, 5.02405f }, // Amberleaf Scamp 3
    { 1408.024048f, 3539.550049f, 87.931252f, 6.01367f }, // Amberleaf Scamp 4
    { 1408.695435f, 3534.402100f, 86.883087f, 5.99031f }, // Amberleaf Scamp 5
};

class mob_min_dimwind : public CreatureScript
{
public:
    mob_min_dimwind() : CreatureScript("mob_min_dimwind") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_min_dimwindAI(creature);
    }

    struct mob_min_dimwindAI : public ScriptedAI
    {
        mob_min_dimwindAI(Creature* creature) : ScriptedAI(creature) { }

        std::list<ObjectGuid> _playerDataList;

        Creature* Scamp1;
        Creature* Scamp2;
        Creature* Scamp3;
        Creature* Scamp4;
        Creature* Scamp5;
        Creature* Dimwind;

        uint32 m_checkTimer;

        void Reset()
        {
            m_checkTimer = 2000;
        }

        bool IsPlayerOnlist(Player* player, std::list<ObjectGuid>& list)
        {
            for (auto _cPlayer : list)
            {
                if (_cPlayer == player->GetGUID())
                {
                    return true;
                    break;
                }
            }

            return false;
        }

        void SetStageFor(Player* player)
        {
            _playerDataList.push_back(player->GetGUID());

            Scamp1 = me->SummonCreature(NPC_AMBERLEAF_SCAMP, AmberleafScampPoints[0], TEMPSUMMON_MANUAL_DESPAWN, 0s, 0, 0, player->GetGUID());
            Scamp2 = me->SummonCreature(NPC_AMBERLEAF_SCAMP, AmberleafScampPoints[1], TEMPSUMMON_MANUAL_DESPAWN, 0s, 0, 0, player->GetGUID());
            Scamp3 = me->SummonCreature(NPC_AMBERLEAF_SCAMP, AmberleafScampPoints[2], TEMPSUMMON_MANUAL_DESPAWN, 0s, 0, 0, player->GetGUID());
            Scamp4 = me->SummonCreature(NPC_AMBERLEAF_SCAMP, AmberleafScampPoints[3], TEMPSUMMON_MANUAL_DESPAWN, 0s, 0, 0, player->GetGUID());
            Scamp5 = me->SummonCreature(NPC_AMBERLEAF_SCAMP, AmberleafScampPoints[4], TEMPSUMMON_MANUAL_DESPAWN, 0s, 0, 0, player->GetGUID());
            Dimwind = me->SummonCreature(NPC_MIN_DIMWIND_SUMMON, me->GetHomePosition(), TEMPSUMMON_MANUAL_DESPAWN, 0s, 0, 0, player->GetGUID());

            PhasingHandler::AddPhase(Scamp1, 63, true);
            PhasingHandler::AddPhase(Scamp2, 63, true);
            PhasingHandler::AddPhase(Scamp3, 63, true);
            PhasingHandler::AddPhase(Scamp4, 63, true);
            PhasingHandler::AddPhase(Scamp5, 63, true);
            PhasingHandler::AddPhase(Dimwind, 63, true);

            Dimwind->HandleEmoteCommand(EMOTE_STATE_CRANE);

            std::list<Creature*> DimwindList;
            GetCreatureListWithEntryInGrid(DimwindList, me, NPC_MIN_DIMWIND_SUMMON, 5.0f);
            for (auto _cDimwind : DimwindList)
            {
                if (_cDimwind->GetGUID() == Dimwind->GetGUID())
                {
                    // Set owner GUID
                    CAST_AI(mob_min_dimwind_summon::mob_min_dimwind_summonAI, _cDimwind->AI())->playerOwnerGUID = player->GetGUID();
                    // Set Scamps GUIDs
                    CAST_AI(mob_min_dimwind_summon::mob_min_dimwind_summonAI, _cDimwind->AI())->scamp1 = Scamp1->GetGUID();
                    CAST_AI(mob_min_dimwind_summon::mob_min_dimwind_summonAI, _cDimwind->AI())->scamp2 = Scamp2->GetGUID();
                    CAST_AI(mob_min_dimwind_summon::mob_min_dimwind_summonAI, _cDimwind->AI())->scamp3 = Scamp3->GetGUID();
                    CAST_AI(mob_min_dimwind_summon::mob_min_dimwind_summonAI, _cDimwind->AI())->scamp4 = Scamp4->GetGUID();
                    CAST_AI(mob_min_dimwind_summon::mob_min_dimwind_summonAI, _cDimwind->AI())->scamp5 = Scamp5->GetGUID();
                    CAST_AI(mob_min_dimwind_summon::mob_min_dimwind_summonAI, _cDimwind->AI())->dimwind = me->GetGUID();
                }
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (m_checkTimer <= diff)
            {
                std::list<Player*> PlayerList;
                GetPlayerListInGrid(PlayerList, me, 80.0f);
                for (auto _cPlayer : PlayerList)
                {
                    if (_cPlayer->GetQuestStatus(QUEST_THE_MISSING_DRIVER) == QUEST_STATUS_INCOMPLETE)
                    {
                        if (!IsPlayerOnlist(_cPlayer, _playerDataList))
                            SetStageFor(_cPlayer);
                    }
                }
                m_checkTimer = 2000;
            }
            else
            {
                m_checkTimer -= diff;
            }
        }
    };
};

// Wu-Song Villager 57132 65472
struct npc_wu_song_village_57132_65472 : public ScriptedAI
{
    npc_wu_song_village_57132_65472(Creature* creature) : ScriptedAI(creature) {}

    uint32 waitTime;
    ObjectGuid brewthiefTarget;

    void Reset() override
    {
        brewthiefTarget = ObjectGuid::Empty;
        me->SetSheath(SHEATH_STATE_MELEE);
        waitTime = urand(0, 2000);
    }

    void DamageTaken(Unit* doneBy, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/) override
    {
        if (doneBy->ToCreature())
            if (me->GetHealth() <= damage || me->GetHealthPct() <= 80.0f)
                damage = 0;
    }

    void DamageDealt(Unit* target, uint32& damage, DamageEffectType /*damageType*/) override
    {
        if (target->ToCreature())
            if (target->GetHealth() <= damage || target->GetHealthPct() <= 70.0f)
                damage = 0;
    }

    void UpdateAI(uint32 diff) override
    {
        DoMeleeAttackIfReady();

        if (waitTime && waitTime >= diff)
        {
            waitTime -= diff;
            return;
        }

        waitTime = urand(10000, 20000);

        if (!brewthiefTarget.IsEmpty())
        {
            if (Creature* brewthief = ObjectAccessor::GetCreature(*me, brewthiefTarget))
            {
                if (brewthief->IsAlive())
                {
                    if (me->GetVictim() != brewthief)
                    {
                        me->GetThreatManager().AddThreat(brewthief, 1000.0f);
                        brewthief->GetThreatManager().AddThreat(me, 1000.0f);
                        me->Attack(brewthief, true);
                        brewthief->Attack(me, true);
                    }
                }
                else
                {
                    brewthief->DespawnOrUnsummon();
                    brewthiefTarget = ObjectGuid::Empty;
                }
            }
        }
        else
        {
            Position brewthiefPos = me->GetPosition();
            GetPositionWithDistInFront(me, 2.5f, brewthiefPos);

            float z = me->GetMap()->GetHeight(me->GetPhaseShift(), brewthiefPos.GetPositionX(), brewthiefPos.GetPositionY(), brewthiefPos.GetPositionZ());
            brewthiefPos.m_positionZ = z;

            if (Creature* brewthief = me->SummonCreature(56730, brewthiefPos))
            {
                me->GetThreatManager().AddThreat(brewthief, 1000.0f);
                brewthief->GetThreatManager().AddThreat(me, 1000.0f);
                AttackStart(brewthief);
                brewthief->SetFacingToObject(me);
                brewthiefTarget = brewthief->GetGUID();

            }
        }
    }
};

// Aysa Cloudsinger 54975
struct npc_aysa_cloudsinger_54975 : public ScriptedAI
{
    npc_aysa_cloudsinger_54975(Creature* creature) : ScriptedAI(creature) { Reset(); }
private:
    bool quest_accept;
    bool quest_complete;
    TaskScheduler _scheduler;

    void Reset() override
    {
        quest_accept = false;
        quest_complete = false;
    }

    void UpdateAI(uint32 diff) override
    {
        _scheduler.Update(diff);
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
        {
            if (player->IsInDist(me, 30.0f))
            {
                /*if (player->GetQuestStatus(QUEST_A_NEW_FRIEND) == QUEST_STATUS_COMPLETE)
                    if (const Quest* quest = sObjectMgr->GetQuestTemplate(QUEST_A_NEW_FRIEND))
                    {
                        player->RewardQuest(quest, LootItemType::Item, 0, nullptr, true);
                        player->PrepareQuestMenu(player->GetGUID());
                    }*/
              if (player->GetQuestStatus(QUEST_SHU_THE_SPIRIT_OF_WATER) == QUEST_STATUS_REWARDED
                  && !player->HasQuest(QUEST_THE_SOURCE_OF_OUR_LIVELIHOOD)
                  && !player->HasQuest(QUEST_A_NEW_FRIEND))
                if (!quest_accept)
                {
                    quest_accept = true;
                        if (const Quest* quest = sObjectMgr->GetQuestTemplate(QUEST_A_NEW_FRIEND))
                        {
                            player->AddQuest(quest, me);
                        }
                }
              if (player->GetQuestObjectiveProgress(QUEST_A_NEW_FRIEND, 0) == 5)
              {
                  if (!quest_complete)
                  {
                      quest_complete = true;
                      player->ForceCompleteQuest(QUEST_A_NEW_FRIEND);
                      player->CastSpell(player, SPELL_SUMMON_SPIRIT_OF_WATER);
                      Talk(0);
                      _scheduler.Schedule(Milliseconds(5000), [this](TaskContext context)
                          {
                              Talk(1);
                          });
                  }
              }
            }
            if (player->GetDistance2d(me) > 70.0f)
                if (player->GetQuestStatus(QUEST_A_NEW_FRIEND) == QUEST_STATUS_REWARDED)
                {
                    quest_accept = false;
                    quest_complete = false;
                }
        }
    }
};

class npc_shang_xi_choose_faction : public CreatureScript
{
public:
    npc_shang_xi_choose_faction() : CreatureScript("npc_shang_xi_choose_faction") { }

    struct npc_shang_xi_choose_factionAI : public CreatureAI
    {
        npc_shang_xi_choose_factionAI(Creature* creature) : CreatureAI(creature)
        {
        }

        bool OnGossipSelect(Player* player, uint32 menuId, uint32 gossipListId) override
        {
            if (menuId == 1)
            {
                if (player->HasQuest(QUEST_NEW_FATE))
                    player->ShowNeutralPlayerFactionSelectUI();
            }
            else if (menuId == 2)
                player->TeleportTo(0, -8866.55f, 671.93f, 97.90f, 5.31f);
            else if (menuId == 3)
                player->TeleportTo(1, 1577.30f, -4453.64f, 15.68f, 1.84f);

            player->PlayerTalkClass->SendCloseGossip();
            return true;
        }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_NEW_FATE)
            me->AI()->SetGUID(player->GetGUID(), 0);

    }


        EventMap events;
        ObjectGuid playerGuid;
        enum ev
        {
            EVENT_0 = 1,
            EVENT_1 = 2,
            EVENT_2 = 3,
            EVENT_3 = 4,
            EVENT_4 = 5,
            EVENT_5 = 6,
        };

        void SetGUID(ObjectGuid const& guid, int32 id)
        {
            playerGuid = guid;
            uint32 t = 0;
            events.RescheduleEvent(EVENT_0, 1s);     //18:08:37.000
            events.RescheduleEvent(EVENT_1, 9s);     //18:08:46.000
            events.RescheduleEvent(EVENT_2, 11s);    //18:08:57.000
            events.RescheduleEvent(EVENT_3, 7s);     //18:09:04.000
            events.RescheduleEvent(EVENT_4, 8s);     //18:09:12.000
            events.RescheduleEvent(EVENT_5, 10s);    //18:09:22.000
        }

        void Reset()
        {
            playerGuid.Clear();
            events.Reset();
        }

        void UpdateAI(uint32 diff)
        {
            events.Update(diff);

            if (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_0:
                    sCreatureTextMgr->SendChat(me, 0);
                    break;
                case EVENT_1:
                    sCreatureTextMgr->SendChat(me, 1);
                    break;
                case EVENT_2:
                    if (Creature* aysa = me->FindNearestCreature(57721, 100.0f, true))
                        sCreatureTextMgr->SendChat(aysa, 0);
                    break;
                case EVENT_3:
                    sCreatureTextMgr->SendChat(me, 1);
                    break;
                case EVENT_4:
                    if (Creature* czi = me->FindNearestCreature(57720, 100.0f, true))
                        sCreatureTextMgr->SendChat(czi, 0);
                    break;
                case EVENT_5:
                    sCreatureTextMgr->SendChat(me, 3);
                    break;
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_shang_xi_choose_factionAI(creature);
    }
};

class mob_aisa_pre_balon_event : public CreatureScript
{
public:
    mob_aisa_pre_balon_event() : CreatureScript("mob_aisa_pre_balon_event") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_SUF_SHUN_ZI)
            sCreatureTextMgr->SendChat(creature, 1);

        return true;
    }

    struct mob_aisa_pre_balon_eventAI : public ScriptedAI
    {
        mob_aisa_pre_balon_eventAI(Creature* creature) : ScriptedAI(creature)
        {}

        bool justSpeaking;
        EventMap _events;
        GuidSet m_player_for_event;

        enum events
        {
            EVENT_1 = 1,
            EVENT_2 = 2,
            EVENT_3 = 3,

            NPC_FRIEND = 56663,
        };

        void Reset()
        {
            justSpeaking = false;
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (justSpeaking || who->GetTypeId() != TYPEID_PLAYER || who->IsOnVehicle(who))
                return;

            GuidSet::iterator itr = m_player_for_event.find(who->GetGUID());
            if (itr != m_player_for_event.end())
                return;

            if (who->ToPlayer()->GetQuestStatus(QUEST_PASSING_WISDOM) != QUEST_STATUS_COMPLETE)
                return;

            m_player_for_event.insert(who->GetGUID());
            justSpeaking = true;
            _events.RescheduleEvent(EVENT_1, 10s);
            sCreatureTextMgr->SendChat(me, 0);
        }

        void UpdateAI(uint32 diff)
        {
            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_1:
                {
                    _events.RescheduleEvent(EVENT_2, 8s);
                    if (Creature* f = me->FindNearestCreature(NPC_FRIEND, 100.0f, true))
                    {
                        sCreatureTextMgr->SendChat(f, 0);
                        f->SetFacingToObject(me);
                    }
                    break;
                }
                case EVENT_2:
                    sCreatureTextMgr->SendChat(me, 2);
                    justSpeaking = false;
                    break;
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_aisa_pre_balon_eventAI(creature);
    }
};

class mop_air_balloon : public VehicleScript
{
public:
    mop_air_balloon() : VehicleScript("mop_air_balloon") { }

    struct mop_air_balloonAI : public EscortAI
    {
        mop_air_balloonAI(Creature* creature) : EscortAI(creature)
        {}

        ObjectGuid playerGuid;
        ObjectGuid aisaGUID;
        ObjectGuid firepawGUID;
        ObjectGuid shenZiGUID;
        ObjectGuid headGUID;
        EventMap events;

        void Reset()
        {
            me->SetWalk(false);
            me->SetSpeed(MOVE_FLIGHT, 8.0f);

            playerGuid.Clear();
            aisaGUID.Clear();
            firepawGUID.Clear();
            shenZiGUID.Clear();
            headGUID.Clear();
            me->setActive(true);
            me->SetReactState(REACT_PASSIVE);
            me->m_invisibilityDetect.AddFlag(INVISIBILITY_UNK5);
            me->m_invisibilityDetect.AddValue(INVISIBILITY_UNK5, 999);
        }

        enum localdata
        {
            NPC_AISA = 56661,
            NPC_FIREPAW = 56660,
            NPC_SHEN_ZI_SU = 56676,
            NPC_TURTLE_HEAD = 57769,

            SPELL_HEAD_ANIM_RISE = 114888,
            SPELL_HEAD_ANIM_1 = 114898,
            SPELL_HEAD_ANIM_2 = 118571,
            SPELL_HEAD_ANIM_3 = 118572,
            SPELL_VOICE_ANIM = 106759,

            SPELL_AISA_ENTER_SEAT_2 = 63313, //106617

            SPELL_CREDIT_1 = 105895,
            SPELL_CREDIT_2 = 105010,
            SPELL_EJECT_PASSANGER = 60603,
            SPELL_PARASHUT = 45472,

            EVENT_1 = 1, // 17:24:47.000

            EVENT_AISA_TALK_0 = 2,  //17:24:51.000
            EVENT_AISA_TALK_1 = 3,  //17:25:07.000
            EVENT_AISA_TALK_2 = 4,  //17:25:18.000
            EVENT_AISA_TALK_3 = 5,  //17:25:31.000
            EVENT_AISA_TALK_4 = 6,  //17:25:38.000
            EVENT_AISA_TALK_5 = 7,  //17:26:40.000
            EVENT_AISA_TALK_6 = 8,  //17:27:02.000
            EVENT_AISA_TALK_7 = 9,  //17:27:29.000
            EVENT_AISA_TALK_8 = 10, //17:27:50.000
            EVENT_AISA_TALK_9 = 11, //17:28:04.000
            EVENT_AISA_TALK_10 = 12, //17:28:10.000

            EVENT_FIREPAW_TALK_0 = 13, //17:24:47.000
            EVENT_FIREPAW_TALK_1 = 14, //17:24:57.000
            EVENT_FIREPAW_TALK_2 = 15, //17:25:13.000
            EVENT_FIREPAW_TALK_3 = 16, //17:27:16.000
            EVENT_FIREPAW_TALK_4 = 17, //17:27:22.000
            EVENT_FIREPAW_TALK_5 = 18, //17:27:43.000
            EVENT_FIREPAW_TALK_6 = 19, //17:27:57.000

            EVENT_SHEN_ZI_SU_TALK_0 = 20, //17:25:44.000
            EVENT_SHEN_ZI_SU_TALK_1 = 21, //17:25:58.000
            EVENT_SHEN_ZI_SU_TALK_2 = 22, //17:26:12.000
            EVENT_SHEN_ZI_SU_TALK_3 = 23, //17:26:25.000
            EVENT_SHEN_ZI_SU_TALK_4 = 24, //17:26:47.000 
            EVENT_SHEN_ZI_SU_TALK_5 = 25, //17:27:09.000
        };

        void InitTalking(Player* player)
        {
            me->GetMap()->LoadGrid(865.222f, 4986.84f); //voice
            me->GetMap()->LoadGrid(868.356f, 4631.19f); //head
           // if (WorldObject* head = me->GetMap()->GetActiveObjectWithEntry(NPC_TURTLE_HEAD))
            //{
            //    head->m_invisibilityDetect.AddFlag(INVISIBILITY_UNK5);
            //    head->m_invisibilityDetect.AddValue(INVISIBILITY_UNK5, 999);
            ////    player->AddToExtraLook(head->GetGUID());
            //    headGUID = head->GetGUID();
            //}
            //else
           // {
              //  me->Say("SCRIPT::mop_air_balloon not find turtle heat entry 57769", LANG_UNIVERSAL, playerGuid);
            //    player->ExitVehicle();
                return;
           // }

      //      if (WorldObject* shen = me->GetMap()->GetActiveObjectWithEntry(NPC_SHEN_ZI_SU))
          //      shenZiGUID = shen->GetGUID();
         //   else
          //  {
           //     me->MonsterSay("SCRIPT::mop_air_balloon not find shen zi su entry 56676", LANG_UNIVERSAL, playerGuid);
            //    player->ExitVehicle();
            //    return;
          //  }


            events.RescheduleEvent(EVENT_FIREPAW_TALK_0,  1s);       //17:24:47.000
            events.RescheduleEvent(EVENT_AISA_TALK_0, 4s);          //17:24:51.000
            events.RescheduleEvent(EVENT_FIREPAW_TALK_1,  6s);       //17:24:57.000
            events.RescheduleEvent(EVENT_AISA_TALK_1, 10s);         //17:25:07.000
            events.RescheduleEvent(EVENT_FIREPAW_TALK_2, 6s);       //17:25:13.000
            events.RescheduleEvent(EVENT_AISA_TALK_2, 5s);          //17:25:18.000
            events.RescheduleEvent(EVENT_AISA_TALK_3, 14s);         //17:25:31.000
            events.RescheduleEvent(EVENT_AISA_TALK_4, 6s);          //17:25:38.000
            events.RescheduleEvent(EVENT_SHEN_ZI_SU_TALK_0, 6s);    //17:25:44.000
            events.RescheduleEvent(EVENT_SHEN_ZI_SU_TALK_1, 14s);   //17:25:58.000
            events.RescheduleEvent(EVENT_SHEN_ZI_SU_TALK_2, 14s);   //17:26:12.000
            events.RescheduleEvent(EVENT_SHEN_ZI_SU_TALK_3, 13s);   //17:26:25.000
            events.RescheduleEvent(EVENT_AISA_TALK_5, 15s);         //17:26:40.000
            events.RescheduleEvent(EVENT_SHEN_ZI_SU_TALK_4, 7s);     //17:26:47.000 
            events.RescheduleEvent(EVENT_AISA_TALK_6, 15s);         //17:27:02.000
            events.RescheduleEvent(EVENT_SHEN_ZI_SU_TALK_5, 7s);     //17:27:09.000
            events.RescheduleEvent(EVENT_FIREPAW_TALK_3, 7s);       //17:27:16.000
            events.RescheduleEvent(EVENT_FIREPAW_TALK_4, 6s);       //17:27:22.000
            events.RescheduleEvent(EVENT_AISA_TALK_7, 7s);          //17:27:29.000
            events.RescheduleEvent(EVENT_FIREPAW_TALK_5, 14s);      //17:27:43.000
            events.RescheduleEvent(EVENT_AISA_TALK_8, 7s);          //17:27:50.000
            events.RescheduleEvent(EVENT_FIREPAW_TALK_6, 7s);       //17:27:57.000
            events.RescheduleEvent(EVENT_AISA_TALK_9, 7s);          //17:28:04.000
            events.RescheduleEvent(EVENT_AISA_TALK_10, 7s);         //17:28:10.000
        };

        void TalkShenZiSU(uint32 text)
        {
            Creature* shen = me->GetMap()->GetCreature(shenZiGUID);

            if (!shen)
                return;

            if (Player* plr = ObjectAccessor::FindPlayer(playerGuid))
            {
                Creature* head = me->GetMap()->GetCreature(headGUID);
                if (!head)
                    return;

                switch (text)
                {
                    //cast 114888                                   //17:25:31.000
                case 0:                            //17:25:44.000
                    plr->CastSpell(shen, SPELL_HEAD_ANIM_1, false);
                    break;
                case 1:                            //17:25:58.000
                    plr->CastSpell(shen, SPELL_VOICE_ANIM, false);
                    break;
                case 2:                            //17:26:11.000
                case 3:                            //17:26:25.000
                case 5:                            //17:27:08.000
                    plr->CastSpell(shen, SPELL_HEAD_ANIM_2, false);
                    break;
                case 4:                            //17:26:47.000
                    plr->CastSpell(shen, SPELL_HEAD_ANIM_3, false);
                    break;
                }
             //   if (text == 5) // restore emote
             //   {
                   // head->SetUInt32Value(UNIT_FIELD_EMOTE_STATE, ANIM_FLY_LAND);   //hack
                   // plr->RemoveFromExtraLook(head->GetGUID());
               // }
            }
           // sCreatureTextMgr->SendChat(shen, text, CHAT_MSG_ADDON, LANG_ADDON, TEXT_RANGE_AREA);
        }

        void PassengerBoarded(Unit* passenger, int8 seatId, bool apply)
        {
            if (!apply)
            {
                if (passenger->GetTypeId() == TYPEID_PLAYER)
                {
                    me->DespawnOrUnsummon(1s);
                    me->CastSpell(passenger, SPELL_PARASHUT, true);
                }
                else
                    passenger->ToCreature()->DespawnOrUnsummon(1s);
                return;
            }

            if (seatId == 0)
            {
                if (Player* player = passenger->ToPlayer())
                {
                    playerGuid = player->GetGUID();
                    me->CastSpell(player, SPELL_CREDIT_1, true);
                    InitTalking(player);
                }
            }

            if (passenger->GetTypeId() != TYPEID_PLAYER)
            {
                passenger->m_invisibilityDetect.AddFlag(INVISIBILITY_UNK5);
                passenger->m_invisibilityDetect.AddValue(INVISIBILITY_UNK5, 999);
                switch (passenger->GetEntry())
                {
                case NPC_AISA: aisaGUID = passenger->GetGUID(); break;
                case NPC_FIREPAW: firepawGUID = passenger->GetGUID(); break;
                default:
                    break;
                }
            }
        }

        void IsSummonedBy(WorldObject* summoner)
        {
        //    me->AddPlayerInPersonnalVisibilityList(summoner->GetGUID());
            playerGuid = summoner->GetGUID();
           // summoner->EnterVehicle(me, 0);
            events.RescheduleEvent(EVENT_1, 1s);
        }

        void WaypointReached(uint32 waypointId, uint32 pathId) override
        {
            switch (waypointId)
            {
            case 8:
                me->SetSpeed(MOVE_FLIGHT, 3.0f);
                break;
            case 15:
            {
                if (Player* plr = ObjectAccessor::FindPlayer(playerGuid))
                    me->CastSpell(plr, SPELL_CREDIT_2, true);
                break;
            }
            default:
                break;
            }
        }

        void UpdateAI(uint32 diff)
        {
            events.Update(diff);
            EscortAI::UpdateAI(diff);

            if (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_1:
                {
                    if (Creature* f = me->FindNearestCreature(NPC_AISA, 100.0f, true))
                        f->CastSpell(me, SPELL_AISA_ENTER_SEAT_2, true);
                    Start(false);
                    break;
                }
                case EVENT_AISA_TALK_3:
                    if (Creature* head = me->GetMap()->GetCreature(headGUID))
                        if (Player* plr = ObjectAccessor::FindPlayer(playerGuid))
                        {
                            plr->CastSpell(plr, SPELL_HEAD_ANIM_RISE, false);    //17:25:31.000
                           // head->SetUInt32Value(UNIT_FIELD_EMOTE_STATE, 0);   //hack
                        }
                case EVENT_AISA_TALK_0:
                case EVENT_AISA_TALK_1:
                case EVENT_AISA_TALK_2:
                case EVENT_AISA_TALK_4:
                case EVENT_AISA_TALK_5:
                case EVENT_AISA_TALK_6:
                case EVENT_AISA_TALK_7:
                case EVENT_AISA_TALK_8:
                case EVENT_AISA_TALK_9:
                case EVENT_AISA_TALK_10:
                {
                    if (Creature* aisa = me->GetMap()->GetCreature(aisaGUID))
                        sCreatureTextMgr->SendChat(aisa, eventId - 2);
                    break;
                }

                case EVENT_FIREPAW_TALK_0:
                case EVENT_FIREPAW_TALK_1:
                case EVENT_FIREPAW_TALK_2:
                case EVENT_FIREPAW_TALK_3:
                case EVENT_FIREPAW_TALK_4:
                case EVENT_FIREPAW_TALK_5:
                case EVENT_FIREPAW_TALK_6:
                {
                    if (Creature* paw = me->GetMap()->GetCreature(firepawGUID))
                        sCreatureTextMgr->SendChat(paw, eventId - 13);
                    break;
                }
                case EVENT_SHEN_ZI_SU_TALK_0:   // 114898
                case EVENT_SHEN_ZI_SU_TALK_1:   //cast 106759
                case EVENT_SHEN_ZI_SU_TALK_2:   //cast 118571
                case EVENT_SHEN_ZI_SU_TALK_3:   //118571
                case EVENT_SHEN_ZI_SU_TALK_4:   //118572
                case EVENT_SHEN_ZI_SU_TALK_5:   //118571
                    TalkShenZiSU(eventId - 20);
                    break;
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new mop_air_balloonAI(creature);
    }
};


void AddSC_the_wandering_isle()
{
    RegisterSpellScript(spell_summon_troublemaker);
    RegisterSpellScript(spell_meditation_timer_bar);
    RegisterSpellScript(spell_cave_of_scrolls_comp_timer_aura);
    RegisterSpellScript(spell_summon_living_air);
    RegisterSpellScript(spell_fan_the_flames);
    RegisterSpellScript(spell_rock_jump_a);
    RegisterSpellScript(spell_jump_to_front_right);
    RegisterSpellScript(spell_jump_to_front_left);
    RegisterSpellScript(spell_jump_to_back_right);
    RegisterSpellScript(spell_jump_to_back_left);
    RegisterSpellScript(spell_summon_water_spout);
    RegisterSpellScript(spell_water_spout_quest_credit);
    RegisterSpellScript(spell_aysa_congrats_timer);
    RegisterSpellScript(spell_aysa_congrats_trigger_aura);
    RegisterSpellScript(spell_monkey_wisdom_text);
    RegisterSpellScript(spell_ruk_ruk_ooksplosions);
    RegisterSpellScript(spell_master_shang_final_escort_say);
    RegisterSpellScript(spell_injured_sailor_feign_death);
    RegisterSpellScript(spell_rescue_injured_sailor);
    RegisterSpellScript(spell_tempered_fury);
    RegisterSpellScript(spell_summon_deep_sea_aggressor);
    RegisterSpellScript(spell_healing_shenzin_su);
    RegisterSpellScript(spell_turtle_healed_phase_timer);
    RegisterSpellScript(spell_ally_horde_argument);
    RegisterSpellScript(spell_pandaren_faction_choice);
    RegisterSpellScript(spell_faction_choice_trigger);
    RegisterSpellScript(spell_balloon_exit_timer);

    new at_cave_of_meditation();
    new at_singing_pools_transform();
    new at_singing_pools_training_bell();
    new at_temple_of_five_dawns_summon_zhaoren();
    new at_wreck_of_the_skyseeker_injured_sailor();
    new areatrigger_healing_sphere();

    new q_the_way_of_the_tushui();
    new q_only_the_worthy_shall_pass();
    new q_passion_of_shenzin_su();
    new q_disciples_challenge();

    new npc_balance_pole();
    new npc_tushui_monk_on_pole();
    new npc_shu_playing();
    new npc_ji_firepaw_escort();
    new npc_ruk_ruk();
    new npc_ruk_ruk_rocket();
    new npc_zhaoren;
    new npc_ji_firepaw();
    new npc_shen_zin_shu_bunny();
    new npc_aysa_vordraka_fight();
    new npc_vordraka();
    new npc_healers_active_bunny();
    new mob_tushui_trainee();
    new mob_master_shang_xi();
    new boss_jaomin_ro();

    new go_break_gong();

    new mob_min_dimwind_final();
    new mob_min_dimwind_summon();
    new mob_min_dimwind();
    new npc_shang_xi_choose_faction();

    new mob_aisa_pre_balon_event();
    new mop_air_balloon();

    RegisterCreatureAI(npc_wu_song_village_57132_65472);
    RegisterCreatureAI(npc_aysa_cloudsinger_54975);
}
