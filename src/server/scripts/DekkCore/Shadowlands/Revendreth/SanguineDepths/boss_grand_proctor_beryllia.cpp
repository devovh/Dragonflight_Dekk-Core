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

enum berylliaSpells
{
    SPELL_AGONIZE = 328593, //Random player
    SPELL_ENDLESS_TORMENT = 326039, //AOE
    SPELL_IRON_SPIKES = 325254, // MOST CASTED ON TANKS
    SPELL_RITE_OF_SUPREMACY = 325360, //AOE 
};

enum berylliaEvents
{
    EVENT_AGONIZE = 1,
    EVENT_ENDLESS_TORMENT,
    EVENT_IRON_SPIKES,
    EVENT_RITE_OF_SUPREMACY,
};

#define AGGRO_TEXTS "How dare you interrupt my research!"
#define DEAD_TEXTS "So much... left... to discover..."
#define RITUAL_TEXT "My patience is wearing thin!"

//162102 
struct boss_grand_proctor_beryllia : public BossAI
    {
        boss_grand_proctor_beryllia(Creature* creature) : BossAI (creature, DATA_BERYLLIA) {}

        void Reset() override
        {
            events.Reset();
        }

        void JustEngagedWith(Unit* who) override
        {
            me->Yell(AGGRO_TEXTS, LANG_UNIVERSAL, NULL);
            events.ScheduleEvent(EVENT_AGONIZE, 6s);
            events.ScheduleEvent(EVENT_ENDLESS_TORMENT, 24s);
            events.ScheduleEvent(EVENT_IRON_SPIKES, 5s);
            events.ScheduleEvent(EVENT_RITE_OF_SUPREMACY, 11s);     
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
                case EVENT_AGONIZE:
                    DoCastRandom(SPELL_AGONIZE, 100.5f); 
                    events.ScheduleEvent(EVENT_AGONIZE, 7s);
                    break;
                case EVENT_ENDLESS_TORMENT:
                    DoCastAOE(SPELL_ENDLESS_TORMENT);
                    events.ScheduleEvent(EVENT_ENDLESS_TORMENT, 24s);
                    break;
                case EVENT_IRON_SPIKES:
                    DoCastVictim(SPELL_IRON_SPIKES);
                    events.ScheduleEvent(EVENT_IRON_SPIKES, 6s);
                    break;
                case EVENT_RITE_OF_SUPREMACY:
                    DoCastAOE(SPELL_IRON_SPIKES);
                    me->Yell(RITUAL_TEXT, LANG_UNIVERSAL, NULL);
                    events.ScheduleEvent(EVENT_RITE_OF_SUPREMACY, 11s);
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
            instance->DoUpdateWorldState(WORLD_STATE_BERYLLIA_ENCOUNTER_COMPLETE, 1);
            me->Yell(DEAD_TEXTS, LANG_UNIVERSAL, NULL);
        }

        void SpellHit(WorldObject* caster, SpellInfo const* spell) override
        {
            if (spell->Id == SPELL_RITE_OF_SUPREMACY)
            {
                me->Yell(RITUAL_TEXT, LANG_UNIVERSAL, NULL);
            }
        }
    };

void AddSC_boss_grand_proctor_beryllia()
{
    RegisterSanguineDepthsCreatureAI(boss_grand_proctor_beryllia);
}
