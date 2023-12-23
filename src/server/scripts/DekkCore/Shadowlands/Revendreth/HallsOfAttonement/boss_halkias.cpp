/*
 * Copyright 2022 BaldazzarCore-Shadowlands
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


#include "ScriptMgr.h"
#include "Player.h"
#include "Creature.h"
#include "CreatureAI.h"
#include "GameObject.h"
#include "GameObjectAI.h"
#include "AreaTrigger.h"
#include "AreaTriggerAI.h"
#include "SpellAuraEffects.h"
#include "halls_of_attonement.h"

#define AGGRO "ROOOOOOAAARRRRRRRRRRRRRRRRRR"

enum Spells
{
    SPELL_CRUMBLING_SLAM = 322936, //23808
    SPELL_GLASS_SHARD_AT_DAMAGE = 323001,
    SPELL_SHATTERED_PSYCHE = 344663,
	SPELL_LIGHT_OF_ATTONEMENT = 339235,
    SPELL_SINLIGHT_VISION = 322977,
	SPELL_SINLIGHT_VISION_FEAR = 339237,
    SPELL_REFRACTED_SINLIGHT = 322913,
    SPELL_REFRACTED_SINLIGHT_MAIN_APPLY_AT = 322711, //19661,19662
    SPELL_REFRACTED_SINLIGHT_DAMAGE = 324044,
    SPELL_HEAVE_DEBRIS = 322943,
    SPELL_ANIMA_CONTAIMENT = 324871,
    SPELL_LOYAL_BEASTS = 326450,
    SPELL_RAPID_FIRE = 325793,
    SPELL_SHOOT = 325535,
    SPELL_COLLECT_SINS = 325700,
    SPELL_LETHARGY = 326868,

};

enum events
{
    EVENT_RESET = 1,
    EVENT_CRUMBLING_SLAM,
    EVENT_HEAVE_DEBRIS,
    EVENT_SINLIGHT_VISION,
    EVENT_GLASS_SHARD_AT_DAMAGE,
    EVENT_REFRACTED_SINLIGHT_MAIN_APPLY_AT,
    EVENT_SINS,
    EVENT_LETHARGY,
    EVENT_SHATTERED_PSYCHE,
};

//165408
struct boss_halkias : public BossAI
{
    boss_halkias(Creature* c) : BossAI(c, DATA_HALKIAS) { }



    void Reset() override
    {
        _events.Reset();
        me->RemoveAurasDueToSpell(SPELL_ANIMA_CONTAIMENT);
        _events.ScheduleEvent(EVENT_CRUMBLING_SLAM, 5s);
        _events.ScheduleEvent(EVENT_HEAVE_DEBRIS, 20s);
        _events.ScheduleEvent(EVENT_SINLIGHT_VISION, 10s);
        _events.ScheduleEvent(EVENT_GLASS_SHARD_AT_DAMAGE, 15s);  
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
        case EVENT_CRUMBLING_SLAM:
           DoCastVictim(SPELL_CRUMBLING_SLAM);
            glassShards++;
            if (glassShards == 4)
            {
                glassShards = 0;
                _events.Reset();
                DoCastVictim(SPELL_REFRACTED_SINLIGHT);
                DoCastVictim(SPELL_REFRACTED_SINLIGHT_MAIN_APPLY_AT);
                _events.ScheduleEvent(EVENT_RESET, 30s);
            }
            _events.Repeat(15s, 18s);
            break;

        case EVENT_HEAVE_DEBRIS:
            DoCastVictim(SPELL_HEAVE_DEBRIS);
            _events.Repeat(21s, 24s);
            break;
        case EVENT_SINLIGHT_VISION:
            if (Unit* target = SelectTarget(SelectTargetMethod::MaxThreat, 0, 100, false))
                DoCast(target, SPELL_SINLIGHT_VISION);
            _events.Repeat(Seconds(18));
        case EVENT_GLASS_SHARD_AT_DAMAGE:
            DoCastVictim(SPELL_GLASS_SHARD_AT_DAMAGE);
            _events.Repeat(10s, 20s);
            break;
        case EVENT_RESET:
            glassShards = 0;            
            me->RemoveUnitFlag(UNIT_FLAG_UNINTERACTIBLE); 
            me->SetReactState(REACT_AGGRESSIVE);
            _events.ScheduleEvent(EVENT_CRUMBLING_SLAM, 5s);
            _events.ScheduleEvent(EVENT_HEAVE_DEBRIS, 8s);
            break;
        default:
            break;
        }

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;
    }

    DoMeleeAttackIfReady();
    }

    void JustEngagedWith(Unit* who) override
    {
        _JustEngagedWith(who);
        me->Yell(AGGRO, LANG_UNIVERSAL, NULL);
        _events.ScheduleEvent(EVENT_CRUMBLING_SLAM, 5s);
        _events.ScheduleEvent(EVENT_REFRACTED_SINLIGHT_MAIN_APPLY_AT, 20s);
    }

    void JustDied(Unit* player) override
    {
        Talk(1);
        me->RemoveAllAreaTriggers();
        player->RemoveAllAuras();
        _JustDied();
    }

    void JustReachedHome() override
    {
        _JustReachedHome();
        me->RemoveAllAreaTriggers();
    }

    void EnterEvadeMode(EvadeReason /*why*/) override
    {
        me->RemoveAllAreaTriggers();
        _DespawnAtEvade();
    }

private:
    uint8 glassShards;
    EventMap _events;
};

//23808
struct at_glass_shard : public AreaTriggerAI
{
    at_glass_shard(AreaTrigger* at) : AreaTriggerAI(at) { }

    void OnUnitEnter(Unit* target) override
    {
        if (!target->IsPlayer())
            return;

        if (!target->HasAura(SPELL_GLASS_SHARD_AT_DAMAGE))
            at->GetCaster()->CastSpell(target, SPELL_GLASS_SHARD_AT_DAMAGE, true);
    }

    void OnUnitExit(Unit* target) override
    {
        if (!target->IsPlayer())
            return;

        if (target->HasAura(SPELL_GLASS_SHARD_AT_DAMAGE))
            target->RemoveAura(SPELL_GLASS_SHARD_AT_DAMAGE);
    }
};

struct depraved_houndmaster : public ScriptedAI
{
   depraved_houndmaster(Creature* creature) : ScriptedAI(creature) { }

   void Reset() override
   {
       _events.Reset();
       _events.ScheduleEvent(SPELL_LOYAL_BEASTS, 10s);
       _events.ScheduleEvent(SPELL_RAPID_FIRE, 8s);
       _events.ScheduleEvent(SPELL_SHATTERED_PSYCHE, 15s);
       _events.ScheduleEvent(SPELL_SHOOT, 5s);
   }

   void UpdateAI(uint32 diff) override
   {
       if (!UpdateVictim())
           return;

       _events.Update(diff);

       if (me->HasUnitState(UNIT_STATE_CASTING))
           return;

       while (uint32 eventId = _events.ExecuteEvent())
       {
           switch (eventId)
       {
       case SPELL_LOYAL_BEASTS:
           if (Unit* victim = me->GetVictim())
               DoCast(victim, SPELL_LOYAL_BEASTS);
           events.Repeat(12s, 18s);
           break;
       case SPELL_RAPID_FIRE:
           DoCastVictim(SPELL_RAPID_FIRE);
           events.Repeat(10s, 20s);
           break;
       case SPELL_SHATTERED_PSYCHE:
           DoCastVictim(SPELL_SHATTERED_PSYCHE);
           events.Repeat(20s);
           break;
       case SPELL_SHOOT:
           DoCastVictim(SPELL_SHOOT);
           events.Repeat(10s);
           break;
       default:
           break;
           }
       }
       DoMeleeAttackIfReady();
   }

private:
    EventMap _events;
};

//npc_depraved_collector 165529
struct npc_depraved_collector : public ScriptedAI
{
    npc_depraved_collector(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        _events.Reset();
        _events.ScheduleEvent(EVENT_SINS, 13s);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        _events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_SINS:
                DoCastVictim(SPELL_COLLECT_SINS);
                _events.ScheduleEvent(EVENT_SINS, 30s);
                break;
            case EVENT_LETHARGY:
                DoCast(me, SPELL_LETHARGY);
                _events.ScheduleEvent(EVENT_LETHARGY, 5s);
                break;
            case EVENT_SHATTERED_PSYCHE:
                DoCast(me, SPELL_SHATTERED_PSYCHE);
                    _events.ScheduleEvent(EVENT_SHATTERED_PSYCHE, 8s);
                break;
            }
        }

        DoMeleeAttackIfReady();
    }

private:
    EventMap _events;
};

void AddSC_boss_halkias()
{
    RegisterCreatureAI(boss_halkias);
    RegisterAreaTriggerAI(at_glass_shard);
    RegisterCreatureAI(depraved_houndmaster);
    RegisterCreatureAI(npc_depraved_collector);
}
