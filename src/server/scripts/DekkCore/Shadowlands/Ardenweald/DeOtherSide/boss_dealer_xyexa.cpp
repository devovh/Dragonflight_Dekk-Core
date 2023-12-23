/*
* Copyright 2021 Thordekk
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
#include "PassiveAI.h"
#include "ScriptedCreature.h"
#include "AreaTrigger.h"
#include "AreaTriggerAI.h"
#include "SpellAuraEffects.h"
#include "de_other_side.h"

enum Spells
{
    SPELL_DISPLACEMENT_TRAP = 319619,
    SPELL_DISPLACEMENT_TRAP_CREATE_AT = 319636, //23341, 19347
    SPELL_DISPLACEMENT_TRAP_DAMAGE = 320834,
    SPELL_DISPLACEMENT_TRAP_MISSILE = 319627,
    SPELL_DISPLACEMENT_TRAP_MAIN = 334199,
    SPELL_DISPLACEMENT_BLASTWAVE = 324090,
    SPELL_LOCALIZED_EXPOSIVE_CONTRIVANCE_MARK = 321948,
    SPELL_LOCALIZED_EXPLOSIVE_CONTRIVANCE_PERIODIC = 342961,
    SPELL_EXPLOSIVE_CONTRIVANCE_CAST = 320230,
    SPELL_EXPLOSIVE_CONTRIVANCE_PERIODIC = 320232,
    SPELL_LOCALIZED_EXPLOSIVE_CONTRIVANCE_CREATE_VISUAL_AT = 342962,
};

enum miscs
{
    SAY_ENTER = 0,
    SAY_AGGRO = 1,
    SAY_DEATH = 2,                
};

enum events
{
    EVENT_DISPLACEMENT_TRAP = 1,
    EVENT_LOCALIZED_EXPOSIVE_CONTRIVANCE_MARK,
    EVENT_EXPLOSIVE_CONTRIVANCE_PERIODIC,
    EVENT_DISPLACEMENT_BLASTWAVE,
};

//164450
class boss_dealer_xyexa : public CreatureScript
{
public:
    boss_dealer_xyexa() : CreatureScript("boss_dealer_xyexa") { }

    struct boss_dealer_xyexaAI : public BossAI
    {
        boss_dealer_xyexaAI(Creature* creature) : BossAI(creature, DATA_DEALER_XYEXA) { }


        void Reset() override
        {
            _events.Reset();
            Talk(SAY_AGGRO);
            _events.ScheduleEvent(EVENT_DISPLACEMENT_TRAP, 15s);
            _events.ScheduleEvent(EVENT_LOCALIZED_EXPOSIVE_CONTRIVANCE_MARK, 5s);
            _events.ScheduleEvent(EVENT_EXPLOSIVE_CONTRIVANCE_PERIODIC, 10s);
            _events.ScheduleEvent(EVENT_DISPLACEMENT_BLASTWAVE, 18s);
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
                case EVENT_DISPLACEMENT_TRAP:
                    DoCastVictim(SPELL_DISPLACEMENT_TRAP);
                    _events.Repeat(20s, 25s);
                    break;
                case EVENT_DISPLACEMENT_BLASTWAVE:
                    DoCastVictim(SPELL_DISPLACEMENT_BLASTWAVE);
                    _events.Repeat(18s, 25s);
                    break;
                case EVENT_LOCALIZED_EXPOSIVE_CONTRIVANCE_MARK:
                    if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 100.0F, true))
                    {
                        me->CastSpell(target, SPELL_LOCALIZED_EXPOSIVE_CONTRIVANCE_MARK, true);
                        me->AddAura(SPELL_LOCALIZED_EXPLOSIVE_CONTRIVANCE_PERIODIC, target);
                    }
                    _events.Repeat(30s);
                    break;
                case EVENT_EXPLOSIVE_CONTRIVANCE_PERIODIC:
                    DoCastVictim(SPELL_EXPLOSIVE_CONTRIVANCE_CAST);
                    _events.Repeat(29s);
                    break;
                }

                DoMeleeAttackIfReady();
            }
        }

        void OnSpellCast(SpellInfo const* spell) override
        {

            switch (spell->Id)
            {
            case SPELL_DISPLACEMENT_TRAP:
            {
                std::list<Player*> targetList;
                me->GetPlayerListInGrid(targetList, 100.0f);
                for (Player* targets : targetList)
                {
                    me->CastSpell(targets->GetPosition(), SPELL_DISPLACEMENT_TRAP_MISSILE, true);
                }
                break;
            }

            case SPELL_EXPLOSIVE_CONTRIVANCE_CAST:
            {
                std::list<AreaTrigger*> atList;
                me->GetAreaTriggerListWithSpellIDInRange(atList, SPELL_LOCALIZED_EXPOSIVE_CONTRIVANCE_MARK, 100.0f);
                for (AreaTrigger* at : atList)
                {
                    if (at->GetPositionZ() < -45.0f)
                    {
                        at->GetCaster()->CastSpell(at->GetPosition(), SPELL_LOCALIZED_EXPLOSIVE_CONTRIVANCE_CREATE_VISUAL_AT, true);
                        at->Remove();
                    }
                }
                std::list<Player*> targetList;
                me->GetPlayerListInGrid(targetList, 100.0f);
                for (Player* targets : targetList)
                {
                    if (targets->GetPositionZ() < -45.0f && !targets->HasAura(SPELL_EXPLOSIVE_CONTRIVANCE_PERIODIC))
                        me->CastSpell(targets, SPELL_EXPLOSIVE_CONTRIVANCE_PERIODIC, true);
                }
                break;
            }
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
            Talk(SAY_DEATH);
            _JustDied();
            me->RemoveAllAreaTriggers();
        }
                private:
                    EventMap _events;
    };
        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_dealer_xyexaAI(creature);
        }
};

//319636, 23341
struct at_displacement_trap : public AreaTriggerAI
{
    at_displacement_trap(AreaTrigger* at) : AreaTriggerAI(at) { }

    void OnUnitEnter(Unit* target) override
    {
        if (!at->GetCaster() || !target->IsPlayer())
            return;

        at->GetCaster()->CastSpell(target, SPELL_DISPLACEMENT_TRAP_DAMAGE, true);
        at->GetCaster()->CastSpell(target, SPELL_DISPLACEMENT_TRAP_MAIN, true);
        target->UpdatePosition(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ() + 30, target->GetOrientation());
	}
};

//321948, 19864
struct at_localized_explosive_contrivance : public AreaTriggerAI
{
    at_localized_explosive_contrivance(AreaTrigger* at) : AreaTriggerAI(at) { }

    void OnRemove() override
    {
        std::list<AreaTrigger*> atList;
        at->GetCaster()->GetAreaTriggerListWithSpellIDInRange(atList, SPELL_LOCALIZED_EXPOSIVE_CONTRIVANCE_MARK, 100.0f);
        for (AreaTrigger* at : atList)
        {
            if (at->GetPositionZ() < -45.0f)
            {
                at->GetCaster()->CastSpell(at->GetPosition(), SPELL_LOCALIZED_EXPLOSIVE_CONTRIVANCE_CREATE_VISUAL_AT, true);
                at->Remove();
            }
        }
    }
};

//320230
class spell_explosive_contrivance : public SpellScript
{
    PrepareSpellScript(spell_explosive_contrivance);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        if (GetHitUnit()->GetPositionZ() < -45.0f)
            targets.remove(GetHitUnit());
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_explosive_contrivance::FilterTargets, EFFECT_0, TARGET_DEST_CASTER);
    }
};

void AddSC_boss_dealer_xyexa()
{
    new boss_dealer_xyexa();
    RegisterAreaTriggerAI(at_displacement_trap);
    RegisterAreaTriggerAI(at_localized_explosive_contrivance);
    RegisterSpellScript(spell_explosive_contrivance);
}
