/*
 * Copyright 2021 DEKKCORE
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

#include "AreaTrigger.h"
#include "AreaTriggerAI.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "ScriptMgr.h"
#include "SpellScript.h"
#include "SpellAuras.h"
#include "SpellAuraEffects.h"
#include "the_motherlode.h"
#include "ScriptedCreature.h"
#include "SpellHistory.h"
#include "TemporarySummon.h"

enum Spells
{
    SPELL_ALPHA_CANNON_MISSILE       = 260318,
    SPELL_HOMING_MISSILE_VISUAL      = 260815,
    SPELL_HOMING_MISSILE_CAST        = 260811,
    SPELL_HOMING_MISSILE_SPEED       = 260836,
    SPELL_HOMING_MISSILE_DAMAGE      = 260838,
    SPELL_GATLING_GUN_DAMAGE         = 260279,
    SPELL_GATLING_GUN_CAST           = 260280,
    SPELL_CONFIGURATION_DRILL        = 260189, //x3
    SPELL_DRILL_SMASH_CAST_DAMAGE    = 260202, // on player reach Z
    SPELL_BIG_RED_ROCKET_MISSILE     = 270276,
    SPELL_BIG_RED_ROCKET_VISUAL      = 270279,
    SPELL_DRILL_SMASH_VISUAL         = 260194,
    SPELL_DRILL_SMASH_CAST           = 271456, // cast while air Z
    SPELL_MICRO_MISSILE_CAST         = 276229,
    SPELL_MICRO_MISSILE_MISSILE      = 276231,
    SPELL_CONCUSSION_CHARGE_CAST     = 281621,
    SPELL_CONCUSSION_CHARGE_MISSILE  = 262284,
    SPELL_AZERITE_HEARTSEEKER_CAST   = 262515,
    SPELL_AZERITE_HEARTSEEKER_DAMAGE = 262516,
    SPELL_BUSTER_SHOT                = 260372,
    SPELL_JUMP_YET                   = 262436,
    SPELL_CONFIGURATION_COMBAT       = 260190,
};

enum Events
{
    EVENT_ALPHA_CANNON = 1,
    EVENT_HOMING_MISSILE,
    EVENT_GATLING_GUN,
    EVENT_DRILL_SMASH,
    EVENT_SUMMON_BIG_RED_ROCKET,
    EVENT_MICRO_MISSILE,
    EVENT_MISSILE_EXPLODE,
    EVENT_FIXATE,
    EVENT_FOLLOW,
    EVENT_CONCUSSION_CHARGE,
    EVENT_AZERITE_HEARTSEEKER,
    EVENT_BUSTER_SHOT,
    EVENT_JUMP_YET,
    EVENT_BOOMBA,
    EVENT_PHASE_2,
};

enum Timers
{
    TIMER_MICRO_MISSILE = 15 * IN_MILLISECONDS,
    TIMER_CONCUSSION_CHARGE = 10 * IN_MILLISECONDS,
    TIMER_AZERITE_HEARTSEEKER = 17 * IN_MILLISECONDS,
    TIMER_BUSTER_SHOT = 2000,
    TIMER_JUMP_YET = 30000,
    TIMER_BIG_RED_ROCKET = 13000,

    TIMER_ALPHA_CANNON = 3 * IN_MILLISECONDS,
    TIMER_GATLING_GUN = 20100,
    TIMER_HOMING_MISSILE = 22000,

    TIMER_BOOMBA = 99 * IN_MILLISECONDS,
};

enum Creatures
{
    BOSS_MOGUL_RAZDUNK = 129232,
    NPC_HOMING_MISSILE = 132338,
    NPC_BIG_RED_ROCKET = 131973,
    NPC_VENTURE_SKYSCORCHER = 133436,
    NPC_MISSILE_TARGET = 132271,
    NPC_BOOMBA = 141303,
    NPC_CENTER_STALKER = 132258,
    NPC_MICRO_MISSILE_TARGET_STALKER = 132271,
    NPC_DRILL_SMASH_TARGET_STALKER = 137513,
};

enum Texts
{
    SAY_AT_SEE = 0,             // 0 Do you bums realize how much property damage you\'ve done!?
    SAY_AGGRO,                  // 1 Right where you belong--under my heel!
    SAY_GATLING_GUN,            // 2 Taste some high-caliber carnage!
    SAY_GATLING_GUN_2,          // 3 Get a load of 300 rounds per minute!
    SAY_PHASE_2,                // 4 What am I payin\' you fools for?! Get out here and fix this!
    SAY_DRILL_SMASH,            // 5 Pulverize!
    SAY_HOMING_MISSILE,         // 6 |TINTERFACE\\ICONS\\ABILITY_SIEGE_ENGINEER_SOCKWAVE_MISSILE.BLP:20|t You have been targeted by |cFFFF0000|Hspell:260838|h[Drill Smash]|h|r!
    SAY_PHASE_2_2,              // 7 Doh! My insurance premiums!
    SAY_DRILL_SMASH_2,          // 8 Crush!
    SAY_DRILL_SMASH_3,          // 9 Smash!
    SAY_DEAD                    // 10 You\'ll pay for that!
};

enum Actions
{
    ACTION_ENGAGE_MISSILE = 1,
};

const Position centerPos = { 1101.65f, -3918.86f, 79.52f }; //40

struct bfa_boss_mogul_razdunk : public BossAI
{
    bfa_boss_mogul_razdunk(Creature* creature) : BossAI(creature, DATA_MOGUL_RAZDUNK), summons(me)
    {
        instance = me->GetInstanceScript();
    }

    SummonList summons;
    EventMap events;
    InstanceScript* instance;
    bool talk = true;
    bool phase_2 = false;

    void JustEngagedWith(Unit* who) override
    {
        Talk(SAY_AGGRO);
        instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
        Phase_1(0);
    }

    void Phase_1(uint8 why)
    {
        if (me->GetMap()->IsMythic() || me->GetMap()->IsHeroic())
            events.ScheduleEvent(EVENT_BOOMBA, 60s);

        if (why == 0)
        {
            events.ScheduleEvent(EVENT_HOMING_MISSILE, 5s);
            events.ScheduleEvent(EVENT_GATLING_GUN, 40s);
        }

        if (why == 1)
        {
            me->SetCanFly(false);
            me->GetMotionMaster()->Clear();
            me->AddAura(SPELL_CONFIGURATION_COMBAT, me);
            events.Reset();
            events.ScheduleEvent(EVENT_HOMING_MISSILE, 7s);
            events.ScheduleEvent(EVENT_GATLING_GUN, 17s);
        }

        events.ScheduleEvent(EVENT_ALPHA_CANNON, 30s);
    }
    void Phase_2()
    {
        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        if (me->HasAura(SPELL_CONFIGURATION_COMBAT))
            me->RemoveAura(SPELL_CONFIGURATION_COMBAT);

        for (uint8 i = 0; i < 3; ++i)
            me->CastSpell(me, SPELL_CONFIGURATION_DRILL);

        me->SetCanFly(true);
        events.Reset();
        events.ScheduleEvent(EVENT_SUMMON_BIG_RED_ROCKET, 13s);
        me->GetMotionMaster()->MovePoint(0, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() + 3.0f, false);
    }

    void EnterEvadeMode(EvadeReason /*why*/) override
    {
        BossAI::EnterEvadeMode();
        me->GetMotionMaster()->MoveTargetedHome();
        Reset();
    }

    void Reset() override
    {
        BossAI::Reset();
        instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
        events.Reset();
        summons.DespawnAll();
        phase_2 = false;
    }

    void JustDied(Unit*) override
    {
        instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
        summons.DespawnAll();
        Talk(SAY_DEAD);
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (!talk)
            return;

        if (who->IsPlayer() && who->IsAlive())
        {
            Talk(SAY_AT_SEE);
            talk = false;
        }
    }

    void JustSummoned(Creature* summon) override
    {
        summons.Summon(summon);

        switch (summon->GetEntry())
        {
        case NPC_BIG_RED_ROCKET:
            summon->SetUnitFlag(UnitFlags(UNIT_FLAG_NON_ATTACKABLE));
            summon->CastSpell(summon, SPELL_BIG_RED_ROCKET_VISUAL, true);
            break;
        case NPC_VENTURE_SKYSCORCHER:
            summon->SetInCombatWithZone();
            break;
        }
    }

    void Says(uint8 eventId)
    {
        uint8 randm = urand(0, 1);

        switch (eventId)
        {
        case EVENT_GATLING_GUN:
        {
            randm == 0 ? Talk(SAY_GATLING_GUN) : Talk(SAY_GATLING_GUN_2);
            break;
        }

        case EVENT_DRILL_SMASH:
        {
            randm = urand(0, 2);

            if (randm == 0)
                Talk(SAY_DRILL_SMASH);

            if (randm == 1)
                Talk(SAY_DRILL_SMASH_2);

            if (randm == 2)
                Talk(SAY_DRILL_SMASH_3);
            break;
        }

        case EVENT_PHASE_2:
        {
            (me->GetHealthPct() >= 50) ? Talk(SAY_PHASE_2) : Talk(SAY_PHASE_2_2);
            break;
        }
        }
    }

    void DamageTaken(Unit* attacker, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/) override
    {
        if (me->HealthBelowPct(50) && !phase_2)
        {
            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (me->HasAura(SPELL_GATLING_GUN_CAST))
                return;

            SummonSkyscorcher();
            Phase_2();
            phase_2 = true;
        }
    }

    void UpdateAI(uint32 diff) override
    {
        events.Update(diff);

        if (!UpdateVictim())
            return;

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        if (CheckHomeDistToEvade(diff, 45.0f))
        {
            Reset();
            return;
        }

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_ALPHA_CANNON:
            {
                if (me->IsWithinMeleeRange(me->GetVictim()))
                    me->CastSpell(me->GetVictim(), SPELL_ALPHA_CANNON_MISSILE);

                events.ScheduleEvent(EVENT_ALPHA_CANNON, 3s);
                break;
            }
            case EVENT_HOMING_MISSILE:
            {
                me->CastSpell(me, SPELL_HOMING_MISSILE_CAST);

                events.ScheduleEvent(EVENT_HOMING_MISSILE, 22s);
                break;
            }

            case EVENT_GATLING_GUN:
            {
                Says(EVENT_GATLING_GUN);
                me->CastSpell(me, SPELL_GATLING_GUN_CAST);

                events.ScheduleEvent(EVENT_GATLING_GUN, 20s);
                break;
            }
            case EVENT_SUMMON_BIG_RED_ROCKET:
            {
                std::list<Creature*> victim = me->FindNearestCreatures(NPC_MISSILE_TARGET, 100.0f);

                if (!victim.empty())
                {
                    Creature* target = Trinity::Containers::SelectRandomContainerElement(victim);
                    me->CastSpell(target, SPELL_BIG_RED_ROCKET_MISSILE);
                }

                events.ScheduleEvent(EVENT_DRILL_SMASH,  15s);
                break;
            }
            case EVENT_DRILL_SMASH:
            {
                me->GetMotionMaster()->Clear();
                Player* victim = me->SelectNearestPlayer(90.0f);
                me->CastSpell(victim, SPELL_DRILL_SMASH_CAST);

                events.ScheduleEvent(EVENT_SUMMON_BIG_RED_ROCKET, 13s);
                break;
            }
            case EVENT_BOOMBA:
            {
                // Hero/Mythic
                break;
            }

            default:
                break;
            }
        }
        // No melee attack.
    }

    void OnSpellFinished(SpellInfo const* spellInfo) //override
    {
        switch (spellInfo->Id)
        {
        case SPELL_HOMING_MISSILE_CAST:
        {
            if (Creature* missile = me->SummonCreature(NPC_HOMING_MISSILE, me->GetPosition(), TEMPSUMMON_MANUAL_DESPAWN))
                missile->AI()->DoAction(ACTION_ENGAGE_MISSILE);

            break;
        }
        case SPELL_DRILL_SMASH_CAST:
        {
            if (Unit* target = me->GetVictim())
                me->GetMotionMaster()->MoveCharge(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 30.0f);

            me->GetScheduler().Schedule(5s, [this](TaskContext /*context*/) {
                me->CastSpell(me, SPELL_DRILL_SMASH_CAST_DAMAGE);

                me->GetAI()->AddTimedDelayedOperation(1500, [this]() -> void {
                    if (Creature* dummy = me->FindNearestCreature(NPC_BIG_RED_ROCKET, 8.0f, true))
                    {
                        if (!dummy)
                            return;

                        if (Aura* conf = me->GetAura(SPELL_CONFIGURATION_DRILL))
                        {
                            if (conf->GetStackAmount() <= 1)
                            {
                                me->RemoveAura(SPELL_CONFIGURATION_DRILL);
                                Phase_1(1);
                            }
                            else if (conf->GetStackAmount() > 1)
                                conf->SetStackAmount(conf->GetStackAmount() - 1);
                        }

                        dummy->DespawnOrUnsummon(7s);
                    }
                    });
                });

            break;
        }
        }
    }

    void SummonSkyscorcher()
    {
        me->SummonCreature(NPC_VENTURE_SKYSCORCHER, 1136.92f, -3900.25f, 79.71f, TEMPSUMMON_CORPSE_DESPAWN);
        me->SummonCreature(NPC_VENTURE_SKYSCORCHER, 1104.74f, -3956.37f, 79.72f, TEMPSUMMON_CORPSE_DESPAWN);
    }
};


class bfa_spell_gatling_gun : public SpellScript
{
    PrepareSpellScript(bfa_spell_gatling_gun);

    void HandleAfterCast()
    {
        Unit* caster = GetCaster();

        if (!caster)
            return;

        caster->GetMotionMaster()->MoveRotate(8000, 100, ROTATE_DIRECTION_LEFT);
    }

    void Register()
    {
        AfterCast += SpellCastFn(bfa_spell_gatling_gun::HandleAfterCast);
    }
};


// Spell 260280 - Effect 0 [AT: 17012]
struct at_mogulrazdunk_gatling_gun : AreaTriggerAI
{
    at_mogulrazdunk_gatling_gun(AreaTrigger* areatrigger) : AreaTriggerAI(areatrigger) { }

    void OnUnitEnter(Unit* unit) override
    {
        if (Unit* caster = at->GetCaster())
            if (unit->IsPlayer() || unit->IsPet())
                caster->AddAura(SPELL_GATLING_GUN_DAMAGE, unit);

    }

    void OnUnitExit(Unit* unit) override
    {
        if (Unit* caster = at->GetCaster())
            unit->RemoveAura(SPELL_GATLING_GUN_DAMAGE);
    }
};

// 132338 Homing Missile
struct bfa_npc_homing_missile : public ScriptedAI
{
    bfa_npc_homing_missile(Creature* creature) : ScriptedAI(creature)
    {
        me->SetSpeed(MOVE_WALK, 2.4f);
        me->SetSpeed(MOVE_RUN, 0.85f);
        me->SetUnitFlag(UnitFlags(UNIT_FLAG_NON_ATTACKABLE));
        me->SetWalk(true);
    }

    EventMap events;

    void DoAction(int32 action)
    {
        switch (action)
        {
        case ACTION_ENGAGE_MISSILE:
            me->CastSpell(me, SPELL_HOMING_MISSILE_SPEED);
            me->CastSpell(me, SPELL_HOMING_MISSILE_VISUAL);
            events.ScheduleEvent(EVENT_MISSILE_EXPLODE, 10s);
            events.ScheduleEvent(EVENT_FIXATE, 5s);
            break;
        }
    }

    void UpdateAI(uint32 diff) override
    {
        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_FIXATE:
            {
                std::list<Player*> targets;
                me->GetPlayerListInGrid(targets, 40.0f);

                if (targets.size() > 1)
                {
                    for (auto toDelete : targets)
                        if (toDelete->GetRoleBySpecializationId(ROLE_TANK))
                            targets.remove(toDelete);

                  //  for (auto target : targets)
                       // if (!target->IsWithinMeleeRange(me->ToTempSummon()->GetSummoner()))
                        //    for (auto toDelete : targets)
                            //    if (target->IsWithinMeleeRange(me->ToTempSummon()->GetSummoner()))
                                 //   targets.remove(toDelete);
                }

                if (targets.size() > 1)
                    targets.resize(1);

                for (auto target : targets)
                    me->GetAI()->AttackStart(target);


                events.ScheduleEvent(EVENT_FOLLOW, 1s);
                break;
            }
            case EVENT_FOLLOW:
            {
                if (Unit* taget = me->GetVictim())
                    me->GetMotionMaster()->MoveFollow(taget, 0.0f, 0.0f);
                break;
            }
            case EVENT_MISSILE_EXPLODE:
                me->CastSpell(me, SPELL_HOMING_MISSILE_DAMAGE);
                me->DespawnOrUnsummon();
                break;
            }
        }

        if (Unit* followed = me->GetVictim())
            if (me->GetDistance(followed) < 2.0f)
            {
                me->CastSpell(followed, SPELL_HOMING_MISSILE_DAMAGE);
                me->DespawnOrUnsummon();
            }
    }
};

// 133436 Venture Co. Skyscorcher
struct bfa_npc_venture_skyscorcher : public ScriptedAI
{
    bfa_npc_venture_skyscorcher(Creature* creature) : ScriptedAI(creature)
    { }

    EventMap events;
    Unit* concussion_charge_target;
    Unit* azerite_heartseeker_target;

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* who) override
    {
        events.ScheduleEvent(EVENT_BUSTER_SHOT, 2s);
        events.ScheduleEvent(EVENT_CONCUSSION_CHARGE, 10s);
        events.ScheduleEvent(EVENT_AZERITE_HEARTSEEKER, 17s);
        events.ScheduleEvent(EVENT_JUMP_YET, 30s);
    }

    void UpdateAI(uint32 diff) override
    {
        events.Update(diff);

        if (!UpdateVictim())
            return;

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_BUSTER_SHOT:
            {
                me->CastSpell(me->GetVictim(), SPELL_BUSTER_SHOT);
                events.ScheduleEvent(EVENT_BUSTER_SHOT, 2s);
                break;
            }
            case EVENT_CONCUSSION_CHARGE:
            {
                if (auto target = SelectTarget(SelectTargetMethod::Random, 0, 90.0f, true))
                {
                    me->CastSpell(target, SPELL_CONCUSSION_CHARGE_CAST);
                    concussion_charge_target = target;
                }

                events.ScheduleEvent(EVENT_CONCUSSION_CHARGE, 10s);
                break;
            }
            case EVENT_AZERITE_HEARTSEEKER:
            {
                if (auto target = SelectTarget(SelectTargetMethod::Random, 0, 90.0f, true))
                {
                    me->CastSpell(target, SPELL_AZERITE_HEARTSEEKER_CAST);
                    azerite_heartseeker_target = target;
                }

                events.ScheduleEvent(EVENT_AZERITE_HEARTSEEKER, 17s);
                break;
            }
            case EVENT_JUMP_YET:
            {
                if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 100.0f, true))
                {
                    Position random = target->GetNearPosition(15.0f, float(2 * M_PI));
                    me->CastSpell(random, SPELL_JUMP_YET);
                }
            }
            }
        }

        DoMeleeAttackIfReady();
    }

    void OnSpellFinished(SpellInfo const* spellInfo) override
    {
        switch (spellInfo->Id)
        {
        case SPELL_CONCUSSION_CHARGE_CAST:
        {
            if (!concussion_charge_target)
                return;

            me->CastSpell(concussion_charge_target, SPELL_CONCUSSION_CHARGE_MISSILE);
            break;
        }
        case SPELL_AZERITE_HEARTSEEKER_CAST:
            if (!azerite_heartseeker_target)
                return;

            me->CastSpell(azerite_heartseeker_target, SPELL_AZERITE_HEARTSEEKER_DAMAGE, true);
            break;
        }
    }
};

void AddSC_boss_mogulrazdunk()
{
    RegisterCreatureAI(bfa_boss_mogul_razdunk);
   // RegisterCreatureAI(bfa_npc_boomba);
    RegisterCreatureAI(bfa_npc_homing_missile);
    RegisterCreatureAI(bfa_npc_venture_skyscorcher);
    RegisterSpellScript(bfa_spell_gatling_gun);
    RegisterAreaTriggerAI(at_mogulrazdunk_gatling_gun);
}
