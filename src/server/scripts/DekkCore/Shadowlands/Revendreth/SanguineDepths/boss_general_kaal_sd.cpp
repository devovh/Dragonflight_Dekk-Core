/*
 * Copyright DekkCore team 2023
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or {at your
 * option}, any later version.
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

enum KaalSpells
{
    SPELL_WIKKED_RUSH_CHARGUE  = 323846,
    SPELL_WIKKED_RUSH          = 323845,
    SPELL_PIERCING_BLUR        = 323821,
    SPELL_PIERCING_BLUR_VISUAL = 335915,
    SPELL_GLOOM_SQUALL         = 322903,
    SPELL_GLOOM_SQUALL_TELE    = 322898,
    SPELL_WIKKED_GASH          = 322796, 
};

enum KaalEvents
{
    EVENT_WIKKED_RUSH = 1,
    EVENT_PIERCING_BLUR,
    EVENT_GLOOM_SQUALL,
    EVENT_GASH,
};

#define AGGRO "Surrender the prisoner!"
#define WIKKED "Bleed for your crimes." //on spell wikked rush
#define PIERCING "You lack the strength of stone." // on spell piercing blur
#define GLOOM "The depths will take you all!"//On spell gloom
#define DEAD "Enough! Your suffering will be a testament to the Master's wrath."

//165318
struct boss_general_kaal : public BossAI
{
    boss_general_kaal(Creature* creature) : BossAI(creature, DATA_KAAL) {}

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* who) override
    {
        BossAI::JustEngagedWith(who);
        instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
        me->Yell(AGGRO, LANG_UNIVERSAL, NULL);
        events.ScheduleEvent(EVENT_WIKKED_RUSH, 6s);
        events.ScheduleEvent(EVENT_PIERCING_BLUR, 17s);
        events.ScheduleEvent(EVENT_GLOOM_SQUALL, 35s);
        events.ScheduleEvent(EVENT_GASH, 11s);
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
            case EVENT_WIKKED_RUSH:
                DoCastRandom(SPELL_WIKKED_RUSH_CHARGUE, 50.5f);
                DoCastVictim(SPELL_WIKKED_RUSH);
                me->Yell(WIKKED, LANG_UNIVERSAL, NULL);
                events.ScheduleEvent(EVENT_WIKKED_RUSH, 8s);
                break;
            case EVENT_PIERCING_BLUR:
                DoCastVictim(SPELL_PIERCING_BLUR);
                DoCastVictim(SPELL_PIERCING_BLUR_VISUAL);
                me->Yell(PIERCING, LANG_UNIVERSAL, NULL);
                events.ScheduleEvent(EVENT_PIERCING_BLUR, 14s);
                break;
            case EVENT_GLOOM_SQUALL:
                DoCastVictim(SPELL_GLOOM_SQUALL);
                DoCastVictim(SPELL_GLOOM_SQUALL_TELE);
                me->Yell(GLOOM, LANG_UNIVERSAL, NULL);
                events.ScheduleEvent(EVENT_GLOOM_SQUALL, 22s);
                break;
            case EVENT_GASH:
                DoCastVictim(SPELL_WIKKED_GASH);
                events.ScheduleEvent(EVENT_GASH, 13s);
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
        me->Yell(DEAD, LANG_UNIVERSAL, NULL);
    }
};

void AddSC_boss_general_kaal()
{
    RegisterSanguineDepthsCreatureAI(boss_general_kaal);
}

