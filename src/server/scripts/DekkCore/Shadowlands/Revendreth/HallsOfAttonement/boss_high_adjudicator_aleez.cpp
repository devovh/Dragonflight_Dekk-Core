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

enum Spells
{
    SPELL_BOLT_OF_POWER = 323538,
    SPELL_VOLLEY_OF_POWER_CAST = 323552,
    SPELL_ANIMA_FOUNTAIN_CAST = 329340,
    SPELL_ANIMA_FOUNTAIN_CREATE_AT = 328471,
    SPELL_ANIMA_FOUNTAIN_MISSILE = 338012,
};

enum events
{
    EVENT_SUMMON_GHASTLY_PARISHIONER = 1,
    EVENT_BOLT_OF_POWER,
    EVENT_VOLLEY_OF_POWER_CAST,
    EVENT_ANIMA_FOUNTAIN_CAST,
};
//165410
struct boss_high_adjudicator_aleez : public BossAI
{
    boss_high_adjudicator_aleez(Creature* c) : BossAI(c, DATA_HIGH_ADJUDICATOR_ALEEZ) { }

    void Reset() override
    {
        _events.Reset();
        me->SetPowerType(POWER_ENERGY);
        me->SetMaxPower(POWER_ENERGY, 100);
        me->SetPower(POWER_ENERGY, 100);
        _events.ScheduleEvent(EVENT_BOLT_OF_POWER, 5s);
        _events.ScheduleEvent(EVENT_VOLLEY_OF_POWER_CAST, 10s);
        _events.ScheduleEvent(EVENT_ANIMA_FOUNTAIN_CAST, 8s);
        _events.ScheduleEvent(EVENT_SUMMON_GHASTLY_PARISHIONER, 20s);
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
            case EVENT_BOLT_OF_POWER:
                DoCastVictim(SPELL_BOLT_OF_POWER);
                events.Repeat(15s, 18s);
                break;

            case EVENT_VOLLEY_OF_POWER_CAST:
                DoCastVictim(SPELL_VOLLEY_OF_POWER_CAST);
                events.Repeat(20s, 25s);
                break;

            case EVENT_ANIMA_FOUNTAIN_CAST:
                DoCastVictim(SPELL_ANIMA_FOUNTAIN_CAST);
                events.Repeat(30s, 35s);
                break;

            case EVENT_SUMMON_GHASTLY_PARISHIONER:
                me->SummonCreature(NPC_GHASTLY_PARISHIONER, me->GetRandomNearPosition(10), TEMPSUMMON_MANUAL_DESPAWN);
                events.Repeat(40s, 45s);
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
        Talk(0);
        _events.ScheduleEvent(EVENT_BOLT_OF_POWER, 5s);
        _events.ScheduleEvent(EVENT_VOLLEY_OF_POWER_CAST, 10s);
        _events.ScheduleEvent(EVENT_ANIMA_FOUNTAIN_CAST, 15s);
        _events.ScheduleEvent(EVENT_SUMMON_GHASTLY_PARISHIONER, 20s);
    }

    void OnSpellCast(SpellInfo const* spellInfo) override
    {
        switch (spellInfo->Id)
        {
        case SPELL_ANIMA_FOUNTAIN_CAST:
        {
            std::list<Player*> targetList;
            me->GetPlayerListInGrid(targetList, 100.0F);
            for (Player* targets : targetList)
            {
                me->CastSpell(targets->GetPosition(), SPELL_ANIMA_FOUNTAIN_MISSILE, true);
                me->CastSpell(targets->GetPosition(), SPELL_ANIMA_FOUNTAIN_CREATE_AT, true);
            }
        }
        break;
        }
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

    void JustDied(Unit* /*who*/) override
    {
        _JustDied();
        Talk(1);
        me->RemoveAllAreaTriggers();
    }

private:
    EventMap _events;
};

//328471, at_anima_fountain

void AddSC_boss_high_adjuticator_aleez()
{
    RegisterCreatureAI(boss_high_adjudicator_aleez);
}
