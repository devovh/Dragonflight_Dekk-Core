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

enum executorSpells
{
    SPELL_CASTIGATE = 322554,
    SPELL_SINTOUCHED_ANIMA = 328494,
    SPELL_RESIDUE = 323573,
};

enum executorEvents
{
    EVENT_CASTIGATE = 1,
    EVENT_SINTOUCHED_ANIMA,
    EVENT_RESIDUE,
};

#define AGGRO_TEXT "Let us begin."
#define DEAD_TEXT "The sinners... still... await..."
#define CASTIGATE_TEXT " You will confess, one way or another."


//162103 
 struct boss_executor_tarvold : public BossAI
 {
        boss_executor_tarvold(Creature* creature) : BossAI(creature, DATA_EXECUTOR) {}

        void Reset() override
        {
            events.Reset();
        }

        void JustEngagedWith(Unit* who) override
        {
            BossAI::JustEngagedWith(who);
            instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
            me->Yell(AGGRO_TEXT, LANG_UNIVERSAL, NULL);
            events.ScheduleEvent(EVENT_CASTIGATE, 6s);
            events.ScheduleEvent(EVENT_SINTOUCHED_ANIMA, 10s);
            events.ScheduleEvent(EVENT_RESIDUE, 18s);
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
                case EVENT_CASTIGATE:
                    DoCastAOE(SPELL_CASTIGATE);
                    me->Yell(CASTIGATE_TEXT, LANG_UNIVERSAL, NULL);
                    events.ScheduleEvent(EVENT_CASTIGATE, 8s);
                    break;
                case EVENT_SINTOUCHED_ANIMA:
                    DoCastVictim(SPELL_SINTOUCHED_ANIMA);
                    events.ScheduleEvent(EVENT_SINTOUCHED_ANIMA, 14s);
                    break;
                case EVENT_RESIDUE:
                    DoCastVictim(SPELL_RESIDUE);
                    events.ScheduleEvent(EVENT_RESIDUE, 19s);
                    break;
                default:
                    break;
                }

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;
            }

            DoMeleeAttackIfReady();
        }

        void JustDied(Unit* killer) override
        {
            BossAI::JustDied(killer);
            me->Yell(DEAD_TEXT, LANG_UNIVERSAL, NULL);
            instance->DoUpdateWorldState(WORLD_STATE_EXECUTOR_ENCOUNTER_COMPLETE, 1);
        }

        void SpellHit(WorldObject* caster, SpellInfo const* spell) override
        {
            // Handle spell hits here.
            if (spell->Id == SPELL_CASTIGATE)
            {
                me->Yell(CASTIGATE_TEXT, LANG_UNIVERSAL, NULL);
            }            
        }
 };
     
void AddSC_boss_executor_tarvold()
{
    RegisterSanguineDepthsCreatureAI(boss_executor_tarvold);
}
