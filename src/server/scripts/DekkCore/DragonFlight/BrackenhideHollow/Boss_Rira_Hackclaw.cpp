/*
* DekkCore Team Devs
*
*
*
*
*
*/

#include "Zone_BrackenhideHollow.h"
#include "AreaTriggerAI.h"
#include "AreaTrigger.h"
#include "InstanceScript.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "SpellAuras.h"
#include "SpellHistory.h"
#include "Spell.h"
#include "Player.h"
#include "ScriptedCreature.h"
#include "ScriptMgr.h"

enum Spells
{
    SPELL_SAVAGE_CHARGE = 381419,
    SPELL_BLADESTORM    = 377827, //381834, 381835 , 377830 damage
    SPELL_BLADESTORM_DAMAGE = 377830,
    SPELL_BLADESTORM_VISUAL = 381834,
    SPELL_BLADESTORM_AURA = 377844,

    SPELL_CLEAVE        = 377807
};

enum Says
{
    SAY_COUNTDOWN = 0
};

enum Events
{
    EVENT_SAVAGE_CHARGE = 1,
    EVENT_BLADESTORM    = 2,
    EVENT_CLEAVE        = 3
};


class npc_rira_hackclaw : public CreatureScript
{
public:
    npc_rira_hackclaw() : CreatureScript("npc_rira_hackclaw") { }

    struct npc_rira_hackclawAI : public ScriptedAI
    {
        npc_rira_hackclawAI(Creature* creature) : ScriptedAI(creature, DATA_RIRA_HACKLAW) { }

        void Reset() override
        {
        }

        void JustEngagedWith(Unit* /*who*/) override
        {
            events.ScheduleEvent(EVENT_SAVAGE_CHARGE, 8s);
            events.ScheduleEvent(EVENT_BLADESTORM, 15s);
            events.ScheduleEvent(EVENT_CLEAVE, 18s);
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_SAVAGE_CHARGE:
                    DoCastVictim(SPELL_SAVAGE_CHARGE);
                    events.ScheduleEvent(EVENT_SAVAGE_CHARGE, 10s);
                    break;
                case EVENT_BLADESTORM:
                    DoCastVictim(SPELL_BLADESTORM);
                    DoCastAOE(SPELL_BLADESTORM_VISUAL);
                    DoCastRandom(SPELL_BLADESTORM_DAMAGE, 100.0f, SPELL_BLADESTORM_AURA);
                    events.ScheduleEvent(EVENT_BLADESTORM, 8s);
                    break;
                case EVENT_CLEAVE:
                    DoCastVictim(SPELL_CLEAVE);
                    events.ScheduleEvent(EVENT_CLEAVE, 13s);
                    break;
                default:
                    break;
                }
            }

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_rira_hackclawAI(creature);
    }
};

//377827
class spell_rira_bladestorm : public SpellScript
{
    PrepareSpellScript(spell_rira_bladestorm);

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        if (Unit* target = GetHitUnit())
        {
            target->CastSpell(target, SPELL_BLADESTORM_DAMAGE, true);
            target->SetSpeed(MOVE_RUN, 0.7f);
            target->AddUnitState(UNIT_STATE_ROOT);
            target->AddAura(SPELL_BLADESTORM, target);
            target->SetAuraStack(SPELL_BLADESTORM, target, 4);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_rira_bladestorm::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

//377844
class spell_rira_bladestorm_aura : public AuraScript
{
    PrepareAuraScript(spell_rira_bladestorm_aura);

    void HandlePeriodic(AuraEffect const* aurEff)
    {
        if (Unit* caster = GetCaster())
        {
            if (Unit* target = GetTarget())
            {
                int32 _damage = 0;
                std::list<Unit*> targetList;
                Trinity::AnyUnfriendlyUnitInObjectRangeCheck u_check(target, target, 8.0f);
                Trinity::UnitListSearcher<Trinity::AnyUnfriendlyUnitInObjectRangeCheck> searcher(target, targetList, u_check);
                Cell::VisitAllObjects(target, searcher, 8.0f);
                for (auto itr : targetList)
                {
                    if (itr->IsAlive())
                    {
                        _damage += itr->SpellDamageBonusTaken(caster, GetSpellInfo(), 0, SPELL_DIRECT_DAMAGE);
                        caster->CastSpell(target, SPELL_BLADESTORM_DAMAGE, SPELLVALUE_BASE_POINT0);
                    }
                }
            }
        }
    }

    void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* target = GetTarget())
        {
            target->SetSpeed(MOVE_RUN, 1.0f);
            target->ClearUnitState(UNIT_STATE_ROOT);
        }

    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_rira_bladestorm_aura::HandlePeriodic, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
    }
};

void AddSC_npc_rira_hackclaw()
{
    new npc_rira_hackclaw();
    RegisterSpellScript(spell_rira_bladestorm);
    RegisterSpellScript(spell_rira_bladestorm_aura);
}
