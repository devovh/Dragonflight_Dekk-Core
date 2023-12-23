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
#include "InstanceScript.h"
#include "theater_of_pain.h"

enum Spells
{   
    SPELL_HATEFUL_STRIKE = 323515,
    SPELL_TENDERIZING_SMASH = 318406,
    SPELL_TENDERIZING_SMASH_MEAT_HOOK = 323327,

    //Leftover
    SPELL_COAGULATING_OOZE_CREATE_AT = 321447, //23544
    SPELL_COAGULATING_OOZE_AT_DAMAGE = 323130,

    EVENT_ACTIVATE_HOOKS = 1,
};

//162317
struct boss_gorechop : public BossAI
{
    boss_gorechop(Creature* c) : BossAI(c, DATA_GORECHOP) { }

    void Reset() override
    {
        BossAI::Reset();
    }

    void JustEngagedWith(Unit* who) override
    {
        _JustEngagedWith(who);
        events.ScheduleEvent(SPELL_HATEFUL_STRIKE, 5s);
        events.ScheduleEvent(SPELL_TENDERIZING_SMASH, 10s);
        events.ScheduleEvent(EVENT_ACTIVATE_HOOKS, 15s);
    }

    void ExecuteEvent(uint32 eventId) override
    {
        switch (eventId)
        {
        case SPELL_HATEFUL_STRIKE:
            DoCastVictim(SPELL_HATEFUL_STRIKE, false);
            events.Repeat(15s, 18s);
            break;

        case SPELL_TENDERIZING_SMASH:
            me->CastSpell(nullptr, SPELL_TENDERIZING_SMASH, false);
            events.Repeat(20s, 25s);
            break;

        case EVENT_ACTIVATE_HOOKS:
            for (uint8 i = 0; i < 3; i++)
            {
                me->SummonCreature(NPC_OOZING_LEFTOVERS, me->GetRandomNearPosition(20), TEMPSUMMON_TIMED_DESPAWN);
            }
            events.Repeat(30s, 35s);
            break;
        }
    }

    void SummonedCreatureDies(Creature* summon, Unit* /*killer*/) override
    {
        if (summon->GetEntry() == NPC_OOZING_LEFTOVERS)
            me->CastSpell(summon->GetPosition(), SPELL_COAGULATING_OOZE_CREATE_AT, true);
    }

    void EnterEvadeMode(EvadeReason why) override
    {
        _JustReachedHome();
        me->RemoveAllAreaTriggers();
        _DespawnAtEvade();
    }

    void JustDied(Unit* /*victim*/) override
    {
        _JustDied();
        me->RemoveAllAreaTriggers();
    }
};

//23544
struct at_coagulating_ooze : public AreaTriggerAI
{
    at_coagulating_ooze(AreaTrigger* at) : AreaTriggerAI(at) { }

    void OnUnitEnter(Unit* target) override
    {
        if (!target->IsPlayer())
            return;

        if (!target->HasAura(SPELL_COAGULATING_OOZE_AT_DAMAGE))
            at->GetCaster()->CastSpell(target, SPELL_COAGULATING_OOZE_AT_DAMAGE, true);
    }

    void OnUnitExit(Unit* target) override
    {
        if (!target->IsPlayer())
            return;

        if (target->HasAura(SPELL_COAGULATING_OOZE_AT_DAMAGE))
            target->RemoveAura(SPELL_COAGULATING_OOZE_AT_DAMAGE);
    }
};

void AddSC_boss_gorechop()
{
    RegisterCreatureAI(boss_gorechop);
    RegisterAreaTriggerAI(at_coagulating_ooze);
}
