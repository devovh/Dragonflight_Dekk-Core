/*
* DekkCore Team Devs
*
*
*
*
*
*/

#include "ruby_life_pools.h"
#include "SpellAuraEffects.h"
#include "SpellAuras.h"
#include "SpellHistory.h"
#include "Spell.h"
#include "Player.h"
#include "ScriptedCreature.h"

enum KyrakkaSpells
{
    SPELL_INFERNO_CORE                = 384494,
    SPELL_KYRAKKAS_ROARING_FIREBREATH = 381525,
    SPELL_KYRAKKAS_FLAMESPIT          = 381607,
    SPELL_KYRAKKAS_WINDS_OF_CHANGE    = 381517

};
class spell_kyrakkas_inferno_core : public SpellScript
{
    PrepareSpellScript(spell_kyrakkas_inferno_core);

    void HandleOnHit()
    {
        Unit* caster = GetCaster();
        Unit* target = GetHitUnit();
        if (!caster || !target)
            return;

        caster->CastSpell(target, SPELL_INFERNO_CORE, true);

        int32 damage = 2204;
        int32 duration = 3000;
        int32 tick = 500;
        int32 stacks = 3;
        int32 damagePerStack = damage * stacks;
        int32 damageOnRemove = 14327;

        if (Aura* aura = target->GetAura(SPELL_INFERNO_CORE))
        {
            aura->SetDuration(duration);
            aura->SetMaxDuration(duration);
            aura->ModStackAmount(stacks);
            target->AddUnitState(UNIT_STATE_ROOT);
        }
    }

    void HandleAfterHit()
    {
        Unit* caster = GetCaster();
        Unit* target = GetHitUnit();
        uint32 spellId = 0;

        if (!caster || !target)
            return;

        int32 damageOnRemove = 14327;
        int32 stacks = 0;
        if (Aura* aura = target->GetAura(SPELL_INFERNO_CORE))
            stacks = aura->GetStackAmount();

        int32 damage = damageOnRemove * stacks;

        if (Aura* aura = target->GetAura(384494))
            aura->Remove();

        if (spellId == SPELL_KYRAKKAS_ROARING_FIREBREATH)
            caster->CastSpell(caster, SPELL_KYRAKKAS_FLAMESPIT, true);
        else if (spellId == SPELL_KYRAKKAS_FLAMESPIT)
            caster->CastSpell(caster, SPELL_KYRAKKAS_ROARING_FIREBREATH, true);
    }

    void Register()
    {
        OnHit += SpellHitFn(spell_kyrakkas_inferno_core::HandleOnHit);
        AfterHit += SpellHitFn(spell_kyrakkas_inferno_core::HandleAfterHit);
    }
};

//190484 // 199790
class boss_kyrakka : public CreatureScript
{
public:
    boss_kyrakka() : CreatureScript("boss_kyrakka") { }

    struct boss_kyrakkaAI : public BossAI
    {
        boss_kyrakkaAI(Creature* creature) : BossAI(creature, BOSS_KYRAKKA)
        {
            Initialize();
        }

        void Initialize()
        {
            _infernoCoreStacks = 0;
            _roaringFirebreathTimer = 10000;
            _flamespitTimer = 15000;
            _windsofchange = 20000;
        }

        void Reset() override
        {
            _Reset();
            Initialize();
        }

        void JustEngagedWith(Unit* who) override
        {
            _JustEngagedWith(who);
          //  Talk(SAY_AGGRO);
        }

        void JustDied(Unit* /*killer*/) override
        {
            _JustDied();
            instance->SetBossState(DATA_KYRAKKA_AND_ERKHART_STORMVEIN, DONE);
           // Talk(SAY_DEATH);
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            if (_roaringFirebreathTimer <= diff)
            {
                DoCastVictim(SPELL_KYRAKKAS_ROARING_FIREBREATH);
                _roaringFirebreathTimer = 10000;
            }
            else
                _roaringFirebreathTimer -= diff;

            if (_flamespitTimer <= diff)
            {
                DoCastVictim(SPELL_KYRAKKAS_FLAMESPIT);
                _flamespitTimer = 15000;
            }
            else
                _flamespitTimer -= diff;

            if (_windsofchange <= diff)
            {
                DoCast(SPELL_KYRAKKAS_WINDS_OF_CHANGE);
                _windsofchange = 20000;
            }
            else
                _windsofchange -= diff;

            DoMeleeAttackIfReady();
        }

    private:
        uint32 _infernoCoreStacks;
        uint32 _roaringFirebreathTimer;
        uint32 _flamespitTimer;
        uint32 _windsofchange;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_kyrakkaAI(creature);
    }
};


void AddSC_Boss_Kyrakka()
{
    new boss_kyrakka();
    RegisterSpellScript(spell_kyrakkas_inferno_core);
};

