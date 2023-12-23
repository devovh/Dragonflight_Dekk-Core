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
    SPELL_STIGMA_PRIDE_PERIODIC = 323437,
    SPELL_UNLEASHED_SUFFERING = 323236,
    SPELL_TELEKINETIC_TOSS_BEAM = 323143,
    // 70 + 40
    SPELL_CHAMBERLAIN_CHORUS = 341245,
    SPELL_TELEKINETIC_ONSLAUGHT = 329113,
    SPELL_TELEKINETIC_REPULSION = 323129,
    SPELL_RITUAL_OF_WOE = 328791,
    SPELL_RITUAL_OF_WOE_MISSILE = 323401,
    SPELL_TELEKINETIC_REPULSION_KNOCBACK = 323134,
};

enum Events
{
    EVENT_STIGMA_PRIDE_PERIODIC = 1,
    EVENT_UNLEASHED_SUFFERING,
    EVENT_TELEKINETIC_TOSS_BEAM,
};

//164218
struct boss_lord_chamberlain : public BossAI
{
    boss_lord_chamberlain(Creature* c) : BossAI(c, DATA_LORD_CHAMBERLAIN) { }

private:
    bool hp70;
    bool hp40;

    void Reset() override
    {
        BossAI::Reset();
        me->SetPowerType(POWER_MANA);
        me->SetMaxPower(POWER_MANA, 100);
        me->SetPower(POWER_MANA, 100);
        hp70 = false;
        hp40 = false;
        _events.ScheduleEvent(EVENT_STIGMA_PRIDE_PERIODIC, 5s);
        _events.ScheduleEvent(EVENT_UNLEASHED_SUFFERING, 10s);
        _events.ScheduleEvent(EVENT_TELEKINETIC_TOSS_BEAM, 8s);
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
            case EVENT_STIGMA_PRIDE_PERIODIC:
                DoCastVictim(SPELL_STIGMA_PRIDE_PERIODIC, false);
                _events.Repeat(18s, 20s);
                break;

            case EVENT_UNLEASHED_SUFFERING:
                if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 100.0f, true))
                {
                    me->SetFacingToObject(target, true);
                    me->CastSpell(target, SPELL_UNLEASHED_SUFFERING, false);
                }
                _events.Repeat(25s, 30s);
                break;

            case EVENT_TELEKINETIC_TOSS_BEAM:
                if (Creature* sinstone = me->FindNearestCreature(NPC_SINSTONE_STATUE, 100.0f, true))
                    me->CastSpell(sinstone, SPELL_TELEKINETIC_TOSS_BEAM, false);
                _events.Repeat(35s, 40s);
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
        _events.ScheduleEvent(EVENT_STIGMA_PRIDE_PERIODIC, 5s);
        _events.ScheduleEvent(EVENT_UNLEASHED_SUFFERING, 10s);
        _events.ScheduleEvent(EVENT_TELEKINETIC_TOSS_BEAM, 15s);
    }

    void RemoveStatues()
    {
        std::list<Creature*> sinstoneStatueList;
        me->GetCreatureListWithEntryInGrid(sinstoneStatueList, NPC_SINSTONE_STATUE, 100.0f);
        for (Creature* sinstones : sinstoneStatueList)
        {
            sinstones->SetVisible(false);
            sinstones->DespawnOrUnsummon();
        }
    }

    void PlaceStatues() { }

    void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/) override
    {
        if (me->HealthBelowPct(70) && !hp70)
        {
            hp70 = true;
            me->CastSpell(nullptr, SPELL_CHAMBERLAIN_CHORUS, false);
            RemoveStatues();
            PlaceStatues();
        }
        if (me->HealthBelowPct(40) && !hp40)
        {
            hp40 = true;
            me->CastSpell(nullptr, SPELL_CHAMBERLAIN_CHORUS, false);
            RemoveStatues();
            PlaceStatues();
        }
    }

    void OnSpellCast(SpellInfo const* spellInfo) override
    {
        switch (spellInfo->Id)
        {
        case SPELL_TELEKINETIC_TOSS_BEAM:
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

void AddSC_boss_lord_chamberlain()
{
    RegisterCreatureAI(boss_lord_chamberlain);
}
