/*
 * This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
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
#include "InstanceScript.h"
#include "Map.h"
#include "SpellAuras.h"
#include "SpellScript.h"
#include "ScriptMgr.h"
#include "Unit.h"
#include "ruby_life_pools.h"

enum DefierDraganSpells
{
    SPELL_JOB_DONE = 373601,
    SPELL_EXECUTION = 371072,
    SPELL_STEEL_BARRAGE = 372047,
    SPELL_STEEL_BARRAGE_DAMAGE = 372794,
    SPELL_MOLTEN_STEEL_AREA = 385281,
    SPELL_MOLTEN_STEEL_MISSILE = 385273
};

enum DefierDraganTexts
{
    SAY_INTRO_1 = 0,
    SAY_INTRO_2 = 1,
    SAY_AGGRO = 2,
    SAY_DEATH = 3
};

enum MovePoint
{
    POINT_INTRO_1 = 1,
    POINT_INTRO_2 = 2
};

enum Events
{
    EVENT_INTRO_1 = 1,
    EVENT_INTRO_2,
    EVENT_MOVE,
    EVENT_MOVE_2,
    EVENT_STEEL_BARRAGE
};

enum Actions
{
    ACTION_DEFIER_DRAGHAR_INTRO = 0
};

enum Phases
{
    PHASE_INTRO = 1,
    PHASE_COMBAT = 2
};

Position const DefierDragharIntro[2] =
{
    {1600.7067f,  -184.49826f,  126.73425f, 0.0f},
    {1604.0876f,  -180.31857f,  126.97682f, 7.143491744995117187f}
};

// 187897 - Defier Draghar
struct npc_defier_draghar : public ScriptedAI
{
    npc_defier_draghar(Creature* creature) : ScriptedAI(creature), _instance(creature->GetInstanceScript()) { }

    void Reset() override
    {
        _events.SetPhase(PHASE_INTRO);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        Talk(SAY_AGGRO);
        _events.SetPhase(PHASE_COMBAT);
        _events.ScheduleEvent(EVENT_STEEL_BARRAGE, 6500ms, 0, PHASE_COMBAT);
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        if (type != POINT_MOTION_TYPE)
            return;

        switch (id)
        {
        case POINT_INTRO_1:
            _events.ScheduleEvent(EVENT_MOVE_2, 1ms, 0, PHASE_INTRO);
            break;
        }
    }

    void DoAction(int32 actionId) override
    {
        switch (actionId)
        {
        case ACTION_DEFIER_DRAGHAR_INTRO:
        {
            Talk(SAY_INTRO_1);
            _events.SetPhase(PHASE_INTRO);
            _events.ScheduleEvent(EVENT_INTRO_1, 5s, 0, PHASE_INTRO);
            break;
        }
        default:
            break;
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (!_events.IsInPhase(PHASE_INTRO))
            if (!UpdateVictim())
                return;

        _events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_INTRO_1:
                DoCastSelf(SPELL_JOB_DONE);
                DoCastSelf(SPELL_EXECUTION);
                _events.ScheduleEvent(EVENT_INTRO_2, 2s, 0, PHASE_INTRO);
                break;
            case EVENT_INTRO_2:
                Talk(SAY_INTRO_2);
                _events.ScheduleEvent(EVENT_MOVE, 1500ms, 0, PHASE_INTRO);
                break;
            case EVENT_MOVE:
                me->GetMotionMaster()->MovePoint(POINT_INTRO_1, DefierDragharIntro[0]);
                break;
            case EVENT_MOVE_2:
                me->GetMotionMaster()->MovePoint(POINT_INTRO_2, DefierDragharIntro[1]);
                me->SetHomePosition(DefierDragharIntro[1]);
                _events.SetPhase(PHASE_COMBAT);
                break;
            case EVENT_STEEL_BARRAGE:
                if (Unit* target = SelectTarget(SelectTargetMethod::MaxThreat))
                    DoCast(target, SPELL_STEEL_BARRAGE);
            default:
                break;
            }
        }

        DoMeleeAttackIfReady();
    }

private:
    EventMap _events;
    InstanceScript* _instance;
};

// 372047 - Steel Barrage
class spell_ruby_life_pools_steel_barrage : public AuraScript
{
    PrepareAuraScript(spell_ruby_life_pools_steel_barrage);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_STEEL_BARRAGE_DAMAGE });
    }

    void HandleEffectPeriodic(AuraEffect const* /*aurEff*/)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetTarget())
            {
                caster->CastSpell(target->GetPosition(), 385273, TRIGGERED_FULL_MASK);
                caster->CastSpell(target->GetPosition(), 398086, TRIGGERED_FULL_MASK);
                caster->CastSpell(target, SPELL_STEEL_BARRAGE_DAMAGE, TRIGGERED_FULL_MASK);
            }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_ruby_life_pools_steel_barrage::HandleEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 385281 - Molten Steel
class spell_ruby_life_pools_molten_steel : public SpellScript
{
    PrepareSpellScript(spell_ruby_life_pools_molten_steel);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_STEEL_BARRAGE, SPELL_MOLTEN_STEEL_MISSILE });
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if(Trinity::UnitAuraCheck(true, SPELL_STEEL_BARRAGE));
    }

    void HandleMissile()
    {
        //if (Player* hitTarget = GetHitPlayer())
        if (Unit* caster = GetCaster())
            caster->CastSpell(*GetHitDest(), SPELL_MOLTEN_STEEL_MISSILE, TRIGGERED_FULL_MASK);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_ruby_life_pools_molten_steel::FilterTargets, EFFECT_0, TARGET_DEST_DEST_RADIUS);
        OnHit += SpellHitFn(spell_ruby_life_pools_molten_steel::HandleMissile);
    }
};

enum RLPSpells
{
    // Flashfrost Chillweaver
    SPELL_ICE_SHIELD = 372749,
    // Primal Juggernaut
    SPELL_EXCAVATE = 373497
};

// 371652 - Executed
class spell_ruby_life_pools_executed : public AuraScript
{
    PrepareAuraScript(spell_ruby_life_pools_executed);

    void HandleEffectApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* target = GetTarget();
        target->SetUnitFlag3(UNIT_FLAG3_FAKE_DEAD);
        target->SetUnitFlag2(UNIT_FLAG2_FEIGN_DEATH);
        target->SetUnitFlag(UNIT_FLAG_PREVENT_EMOTES_FROM_CHAT_TEXT);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_ruby_life_pools_executed::HandleEffectApply, EFFECT_0, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
    }
};

// 384933 - Ice Shield
class spell_ruby_life_pools_ice_shield : public AuraScript
{
    PrepareAuraScript(spell_ruby_life_pools_ice_shield);

    void HandleEffectPeriodic(AuraEffect const* /*aurEff*/)
    {
        Unit* target = GetTarget();

        if (Aura* iceShield = target->GetAura(SPELL_ICE_SHIELD))
            iceShield->RefreshDuration();
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_ruby_life_pools_ice_shield::HandleEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 372793 - Excavate
class spell_ruby_life_pools_excavate : public AuraScript
{
    PrepareAuraScript(spell_ruby_life_pools_excavate);

    void HandleEffectPeriodic(AuraEffect const* /*aurEff*/)
    {
        if (Unit* caster = GetCaster())
            caster->CastSpell(GetTarget(), SPELL_EXCAVATE, true);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_ruby_life_pools_excavate::HandleEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 395029 - Storm Infusion
class spell_ruby_life_pools_storm_infusion : public SpellScript
{
    PrepareSpellScript(spell_ruby_life_pools_storm_infusion);

    void SetDest(SpellDestination& dest)
    {
        dest.RelocateOffset({ 9.0f, 0.0f, 4.0f, 0.0f });
    }

    void Register() override
    {
        OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_ruby_life_pools_storm_infusion::SetDest, EFFECT_1, TARGET_DEST_DEST);
    }
};

// Serverside Areatrigger - 23
struct areatrigger_start_defier_draghar_intro : AreaTriggerAI
{
    areatrigger_start_defier_draghar_intro(AreaTrigger* areatrigger) : AreaTriggerAI(areatrigger), _instance(areatrigger->GetInstanceScript()) { }

    void OnUnitEnter(Unit* /*unit*/) override
    {
        if (Creature* draghar = _instance->GetCreature(DATA_DEFIER_DRAGHAR))
            draghar->AI()->DoAction(ACTION_DEFIER_DRAGHAR_INTRO);

        at->Remove();
    }
private:
    InstanceScript* _instance;
};

void AddSC_ruby_life_pools()
{
    RegisterSpellScript(spell_ruby_life_pools_executed);
    RegisterSpellScript(spell_ruby_life_pools_ice_shield);
    RegisterSpellScript(spell_ruby_life_pools_excavate);
    RegisterSpellScript(spell_ruby_life_pools_storm_infusion);
    RegisterRubyLifePoolsCreatureAI(npc_defier_draghar);
    RegisterSpellScript(spell_ruby_life_pools_steel_barrage);
    RegisterSpellScript(spell_ruby_life_pools_molten_steel);
    RegisterAreaTriggerAI(areatrigger_start_defier_draghar_intro);
}
