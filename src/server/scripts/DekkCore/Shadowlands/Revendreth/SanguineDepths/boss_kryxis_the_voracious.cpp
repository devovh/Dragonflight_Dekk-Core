/*
 * Copyright DekkCore team 2023
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
#include "CreatureAI.h"
#include "CreatureAIImpl.h"
#include "InstanceScript.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "SpellScript.h"
#include "SpellAuras.h"
#include "SharedDefines.h"
#include "TemporarySummon.h"
#include "sanguine_depths.h"

enum KryxisSpells
{
    SPELL_CALL_FOR_HELP = 338471,  
    SPELL_ESSENCE_SURGE = 319657, // Casted on him
    SPELL_HUNGERING_DRAIN = 319654, //Casted on nearby players
    SPELL_JUGERNAUTH_RUSH = 319713, //Random player
    SPELL_JUGERNAUTH_RUSH2 = 319715, // Less damage on rotate
    SPELL_SEVERING_SMASH = 319685, //ENERGY 100
    SPELL_VICIOUS_HEADBUTT = 319650 //knockback
};

enum KryxisEvents
{
    EVENT_VICIOUS_HEADBUTT = 1,
    EVENT_HUNGERING_DRAIN,
    EVENT_JUGERNAUTH_RUSH,
    EVENT_SEVERING_SMASH,
    EVENT_ESSENCE_SURGE,
};

//162100 Kryxis the Voracious
struct boss_Kryxis_the_Voracius : public BossAI
{
    boss_Kryxis_the_Voracius(Creature* creature) : BossAI(creature, DATA_KRYXIS) { }

    void JustAppeared() override
    {
        DoCastSelf(SPELL_ESSENCE_SURGE);
    }

    void JustDied(Unit* killer) override
    {
        BossAI::JustDied(killer);
        instance->DoUpdateWorldState(WORLD_STATE_KRYXIS_ENCOUNTER_COMPLETE, 1);
        instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
    }

    void OnChannelFinished(SpellInfo const* spell) override
    {
        if (spell->Id == SPELL_HUNGERING_DRAIN)
            DoCastAOE(SPELL_HUNGERING_DRAIN, true);
    };

    void JustEngagedWith(Unit* who) override
    {
        BossAI::JustEngagedWith(who);
        instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
        instance->SetData(DATA_KRYXIS, IN_PROGRESS);
        events.ScheduleEvent(EVENT_VICIOUS_HEADBUTT, 6s);
        events.ScheduleEvent(EVENT_HUNGERING_DRAIN, 10s);  
        events.ScheduleEvent(EVENT_JUGERNAUTH_RUSH, 23s);
        events.ScheduleEvent(EVENT_SEVERING_SMASH, 33s);
        events.ScheduleEvent(EVENT_ESSENCE_SURGE, 35s);
        instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me, 1);
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);

        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_VICIOUS_HEADBUTT:
                DoCastVictim(SPELL_VICIOUS_HEADBUTT);
                events.ScheduleEvent(EVENT_VICIOUS_HEADBUTT, 8s);
                break;
            case EVENT_HUNGERING_DRAIN:
                DoCastVictim(SPELL_HUNGERING_DRAIN);
                events.ScheduleEvent(EVENT_HUNGERING_DRAIN, 11s);
                break;
            case EVENT_JUGERNAUTH_RUSH:
                DoCastVictim(SPELL_JUGERNAUTH_RUSH);
                events.ScheduleEvent(EVENT_JUGERNAUTH_RUSH, 23s);
                break;
            case EVENT_SEVERING_SMASH:
                DoCastSelf(SPELL_SEVERING_SMASH);
                events.ScheduleEvent(EVENT_SEVERING_SMASH, 33s);
                break;
            case EVENT_ESSENCE_SURGE:
                DoCastSelf(SPELL_ESSENCE_SURGE);
                events.ScheduleEvent(EVENT_ESSENCE_SURGE, 39s);
                break;
            default:
                break;
            }

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;
        }

        DoMeleeAttackIfReady();
    }
};

void AddSC_boss_kryxis_the_voracious()
{
    RegisterSanguineDepthsCreatureAI(boss_Kryxis_the_Voracius);
}
