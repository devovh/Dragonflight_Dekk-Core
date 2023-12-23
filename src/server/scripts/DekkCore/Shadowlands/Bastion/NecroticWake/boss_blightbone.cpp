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

#include "ScriptMgr.h"
#include "Player.h"
#include "Creature.h"
#include "CreatureAI.h"
#include "GameObject.h"
#include "GameObjectAI.h"
#include "InstanceScript.h"
#include "ScriptedCreature.h"
#include "AreaTrigger.h"
#include "AreaTriggerAI.h"
#include "Spell.h"
#include "SpellAuras.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "Map.h"
#include "necrotic_wake.h"

enum Spells
{
    SPELL_CRUNCH = 320655,
    SPELL_HEAVING_RETCH = 320596,
    SPELL_HEAVING_RETCH_MISSILE = 320699,
    SPELL_FETID_GAS_CREATE_AT = 320637,
    SPELL_FETID_GAS_AURA = 320646,
    //Carrion Worm
    SPELL_BLOOD_GORGE = 320614,
    SPELL_BLOOD_GORGE_BUFF = 320630,
    SPELL_CARRION_ERUPTION = 320631,

};

enum blightboneTalks : uint8
{
    TALK_AGGRO = 0,
    TALK_DEATH = 1,
};

enum Events
{
    EVENT_SPELL_CAST_START = 1,
    EVENT_SPELL_PERIODIC_DAMAGE,
    EVENT_SPELL_PERIODIC_MISSED,
    EVENT_UNIT_SPELLCAST_START
};

//162691

struct boss_blightbone : public BossAI
    {
        boss_blightbone(Creature* creature) : BossAI(creature, DATA_BLIGHTBONE)
        {

        }


    void InitializeAI() override
    {
        scheduler.ClearValidator();
        BossAI::InitializeAI();
    }

    void Reset() override
    {
        events.Reset();
        scheduler.CancelAll();
        summons.DespawnAll();
        instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
        BossAI::Reset();
    }

    void JustEngagedWith(Unit* who) override
    {
        BossAI::JustEngagedWith(who);
        Talk(TALK_AGGRO);
        DoCastVictim(SPELL_HEAVING_RETCH);
        instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
        if (auto* encounterDoor = me->FindNearestGameObject(GO_BLIGHTBONE_EXIT, 100.0f))
            encounterDoor->SetGoState(GO_STATE_READY);
        events.ScheduleEvent(EVENT_SPELL_CAST_START, 3s);
        events.ScheduleEvent(EVENT_SPELL_PERIODIC_DAMAGE, 5s);
        events.ScheduleEvent(EVENT_SPELL_PERIODIC_MISSED, 10s);
        events.ScheduleEvent(EVENT_UNIT_SPELLCAST_START, 15s);      
    }

    void SpellHitTarget(WorldObject* target, SpellInfo const* spellInfo) override 
    { 
        switch (spellInfo->Id)
        {
        case SPELL_HEAVING_RETCH:
            if (target->IsUnit())
                me->AddAura(SPELL_HEAVING_RETCH, target->ToUnit());
            break;
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
        case EVENT_SPELL_CAST_START:
            DoCastVictim(SPELL_CRUNCH, true);
            SpellCastVisual();
            DoCastRandom(SPELL_HEAVING_RETCH, true);
            DoCastAOE(SPELL_HEAVING_RETCH);
            events.Repeat(10s);
            break;
        case EVENT_SPELL_PERIODIC_DAMAGE:
            if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 100.0f, true))
            {
                me->CastSpell(target, SPELL_HEAVING_RETCH);
              
                AddTimedDelayedOperation(2600, [this, target]() -> void
                {
                    for (uint8 i = 0; i < 3; i++)
                    {
                        me->CastSpell(target, SPELL_HEAVING_RETCH_MISSILE, true);
                    }
                });
            }
            events.Repeat(20s);
            break;
        case EVENT_SPELL_PERIODIC_MISSED:
            me->CastSpell(nullptr, SPELL_FETID_GAS_CREATE_AT, false);
            events.Repeat(18s);
            break;
        }
    }
        if (me->GetVictim() && me->GetVictim()->IsWithinMeleeRange(me))
            DoMeleeAttackIfReady();

    }

    void JustReachedHome() override
    {
        _JustReachedHome();
        me->RemoveAllAreaTriggers();
    }

    void JustDied(Unit* /*who*/) override
    {
        _JustDied();
        Talk(TALK_DEATH);
        me->RemoveAllAreaTriggers();
        instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
        if (auto* encounterDoor = me->FindNearestGameObject(GO_BLIGHTBONE_EXIT, 100.0f))
            encounterDoor->SetGoState(GO_STATE_ACTIVE);
    }
};


//23443
struct at_fetid_gas : public AreaTriggerAI
{
    at_fetid_gas(AreaTrigger* at) : AreaTriggerAI(at) { }

    void OnUnitEnter(Unit* target) override
    {
        if (!target->IsPlayer())
            return;

        if (!target->HasAura(SPELL_FETID_GAS_AURA))
            at->GetCaster()->AddAura(SPELL_FETID_GAS_AURA, target);
    }

    void OnUnitExit(Unit* target) override
    {
        if (target->HasAura(SPELL_FETID_GAS_AURA))
            target->RemoveAura(SPELL_FETID_GAS_AURA);
    }
};

//164702
struct npc_carrion_worm_nw : public ScriptedAI
{
    npc_carrion_worm_nw(Creature* c) : ScriptedAI(c) { }
    
    void Reset() override
    {
        ScriptedAI::Reset();
    }

    void JustEngagedWith(Unit* who) override
    {
        _events.ScheduleEvent(SPELL_BLOOD_GORGE, 3s);
    }

    void ExecuteEvent(uint32 eventId) override
    {
        if (Aura const* gorge = me->GetAura(SPELL_BLOOD_GORGE_BUFF))
            if (gorge->GetStackAmount() == 3)
            {
                me->RemoveAurasDueToSpell(SPELL_BLOOD_GORGE_BUFF);
                me->CastSpell(nullptr, SPELL_CARRION_ERUPTION, false);
            }

        switch (eventId)
        {
        case SPELL_BLOOD_GORGE:
            DoCastVictim(SPELL_BLOOD_GORGE, false);
            me->AddAura(SPELL_BLOOD_GORGE_BUFF, me);
            _events.Repeat(10s);
            break;
        }
    }

    void JustDied(Unit* /*who*/) override
    {
        if (IsMythic())
            me->CastSpell(nullptr, SPELL_FETID_GAS_CREATE_AT, true);
    }

private:
    EventMap _events;
};

void AddSC_boss_blightbone()
{
    RegisterCreatureAI(boss_blightbone);
    RegisterAreaTriggerAI(at_fetid_gas);
    RegisterCreatureAI(npc_carrion_worm_nw);
}
