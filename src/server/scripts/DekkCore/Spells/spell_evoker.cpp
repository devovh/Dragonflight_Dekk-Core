/*
 * Copyright 2023 DekkCore
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

#include "Containers.h"
#include "SpellAuraEffects.h"
#include "SpellAuras.h"
#include "SpellHistory.h"
#include "Spell.h"
#include "SpellMgr.h"
#include "SpellScript.h"
#include "Player.h"
#include "Unit.h"

enum EvokerSpells
{
    SPELL_EVOKER_ENERGIZING_FLAME = 400006,
    SPELL_EVOKER_GLIDE_KNOCKBACK = 358736,
    SPELL_EVOKER_HOVER = 358267,
    SPELL_EVOKER_LIVING_FLAME = 361469,
    SPELL_EVOKER_LIVING_FLAME_DAMAGE = 361500,
    SPELL_EVOKER_LIVING_FLAME_HEAL = 361509,
    SPELL_EVOKER_SOAR_RACIAL = 369536,
    SPELL_SKYWARD_ASCENT = 367033,
    SPELL_SURGE_FORWARD = 369541,
    SPELL_EVOKER_DREAM_BREATH = 355936,
    SPELL_EVOKER_DREAM_BREATH_2 = 382614,
    SPELL_EVOKER_PYRE_DAMAGE = 357212,
    SPELL_EVOKER_PERMEATING_CHILL_TALENT = 370897,
    SPELL_EVOKER_DREAM_BREATH_CHARGED = 355941,
};

enum EvokerSpellLabels
{
    SPELL_LABEL_EVOKER_BLUE = 1465,
};

// 358733 - Glide (Racial)
class spell_evo_glide : public SpellScript
{
    PrepareSpellScript(spell_evo_glide);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_EVOKER_GLIDE_KNOCKBACK, SPELL_EVOKER_HOVER, SPELL_EVOKER_SOAR_RACIAL });
    }

    SpellCastResult CheckCast()
    {
        Unit* caster = GetCaster();

        if (!caster->IsFalling())
            return SPELL_FAILED_NOT_ON_GROUND;

        return SPELL_CAST_OK;
    }

    void HandleCast()
    {
        Player* caster = GetCaster()->ToPlayer();
        if (!caster)
            return;

        caster->CastSpell(caster, SPELL_EVOKER_GLIDE_KNOCKBACK, true);

        caster->GetSpellHistory()->StartCooldown(sSpellMgr->AssertSpellInfo(SPELL_EVOKER_HOVER, GetCastDifficulty()), 0, nullptr, false, 250ms);
        caster->GetSpellHistory()->StartCooldown(sSpellMgr->AssertSpellInfo(SPELL_EVOKER_SOAR_RACIAL, GetCastDifficulty()), 0, nullptr, false, 250ms);
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_evo_glide::CheckCast);
        OnCast += SpellCastFn(spell_evo_glide::HandleCast);
    }
};

//10.0.2
// Fire Breath-357208 - 382266
class spell_evoker_fire_breath_382266 : public AuraScript
{
    PrepareAuraScript(spell_evoker_fire_breath_382266);

    enum eSpells
    {
        FireBreath = 357209,
    };

    void HandleAfterRemove(AuraEffect const* p_AuraEff, AuraEffectHandleModes p_Mode)
    {
        Unit* l_Caster = GetCaster();
        if (!l_Caster || GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_CANCEL)
            return;

        l_Caster->CastSpell(l_Caster, eSpells::FireBreath, true);
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_evoker_fire_breath_382266::HandleAfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 362969 - Azure Strike (blue)
class spell_evo_azure_strike : public SpellScript
{
    PrepareSpellScript(spell_evo_azure_strike);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        Trinity::Containers::RandomResize(targets, GetEffectInfo(EFFECT_0).CalcValue());
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_evo_azure_strike::FilterTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENEMY);
    }
};

//357208
class FireBreath : public SpellScript
{
    PrepareSpellScript(FireBreath);

    void HandleOnHit()
    {
        if (Unit* target = GetHitUnit())
        {
            int32 damage = GetHitDamage();
            int32 maxHealth = target->GetMaxHealth();

            if (target->GetHealth() + damage > maxHealth)
                damage = maxHealth - target->GetHealth();

            SetHitDamage(damage);
            target->SetHealth(target->GetHealth() - damage);
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(FireBreath::HandleOnHit);
    }
};

//362969
class spell_azure_strike : public SpellScriptLoader
{
public:
    spell_azure_strike() : SpellScriptLoader("spell_azure_strike") { }

    class spell_azure_strike_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_azure_strike_SpellScript);

        bool Validate(SpellInfo const* /*spellInfo*/ ) override
        {
            if (!sSpellMgr->GetSpellInfo(362969, DIFFICULTY_NONE))
                return false;
            return true;
        }

        void HandleOnHit()
        {
            Unit* caster = GetCaster();
            Unit* target = GetHitUnit();
            if (caster && target)
            {
                int32 damage = GetHitDamage();
                int32 bp0 = damage + (damage * 0.5f); // Damage + 50% of damage
                CastSpellExtraArgs args;
                args.AddSpellBP0(bp0);

                caster->CastSpell(target, 362969, args);
            }
        }

        void Register() override
        {
            OnHit += SpellHitFn(spell_azure_strike_SpellScript::HandleOnHit);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_azure_strike_SpellScript();
    }
};

// 361469 - Living Flame (Red)
class spell_evo_living_flame : public SpellScript
{
    PrepareSpellScript(spell_evo_living_flame);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_EVOKER_LIVING_FLAME_DAMAGE, SPELL_EVOKER_LIVING_FLAME_HEAL });
    }

    void HandleTarget(SpellEffIndex /*effIndex*/)
    {
        if (GetCaster()->IsFriendlyTo(GetHitUnit()))
            GetCaster()->CastSpell(GetHitUnit(), SPELL_EVOKER_LIVING_FLAME_HEAL, true);
        else
            GetCaster()->CastSpell(GetHitUnit(), SPELL_EVOKER_LIVING_FLAME_DAMAGE, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_evo_living_flame::HandleTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 361500 - Living Flame (damage)
class spell_evo_living_flame_damage : public SpellScript
{
    PrepareSpellScript(spell_evo_living_flame_damage);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_EVOKER_ENERGIZING_FLAME, SPELL_EVOKER_LIVING_FLAME });
    }

    void HandleManaRestored(SpellEffIndex /*effIndex*/)
    {
        if (AuraEffect* auraEffect = GetCaster()->GetAuraEffect(SPELL_EVOKER_ENERGIZING_FLAME, EFFECT_0))
        {
            SpellInfo const* spellInfo = sSpellMgr->AssertSpellInfo(SPELL_EVOKER_LIVING_FLAME, GetCastDifficulty());

            Optional<SpellPowerCost> cost = spellInfo->CalcPowerCost(POWER_MANA, false, GetCaster(), GetSpellInfo()->GetSchoolMask(), nullptr);

            if (!cost)
                return;

            int32 manaRestored = CalculatePct(cost->Amount, auraEffect->GetAmount());
            GetCaster()->ModifyPower(POWER_MANA, manaRestored);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_evo_living_flame_damage::HandleManaRestored, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

class spell_soar : public SpellScript
{
    PrepareSpellScript(spell_soar);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_EVOKER_SOAR_RACIAL, SPELL_SKYWARD_ASCENT, SPELL_SURGE_FORWARD });
    }

    void HandleOnHit(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        // Increase flight speed by 830540%
        caster->SetSpeedRate(MOVE_FLIGHT, 83054.0f);

        Player * player = GetHitPlayer();
        // Add "Skyward Ascent" and "Surge Forward" to the caster's spellbook
        player->LearnSpell(SPELL_SKYWARD_ASCENT, false);
        player->LearnSpell(SPELL_SURGE_FORWARD, false);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_soar::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 351239 - Visage (Racial)
class spell_cosmic_evoker_visage : public SpellScript
{
    PrepareSpellScript(spell_cosmic_evoker_visage);

    void HandleOnCast()
    {
        Unit* caster = GetCaster();

        if (caster->HasAura(372014))
        {
            // Dracthyr Form
            caster->RemoveAurasDueToSpell(372014);
            caster->CastSpell(caster, 97709, true);
            caster->SendPlaySpellVisual(caster, 118328, 0, 0, 60, false);
            caster->SetDisplayId(108590);
        }
        else
        {
            // Visage Form
            if (caster->HasAura(97709))
                caster->RemoveAurasDueToSpell(97709);
            caster->CastSpell(caster, 372014, true);
            caster->SendPlaySpellVisual(caster, 118328, 0, 0, 60, false);
            caster->SetDisplayId(104597);
        }
    }

    void Register()
    {
        OnCast += SpellCastFn(spell_cosmic_evoker_visage::HandleOnCast);
    }
};

//363898 10.0.5
class spell_evo_fire_breath : public SpellScript
{
    PrepareSpellScript(spell_evo_fire_breath);

    void HandleOnEmpowerEnd()
    {
        if (Unit* caster = GetCaster())
        {
            if (Unit* target = GetHitUnit())
            {
                // Get the spell power and calculate the damage based on the empowering level
                float spellPower = caster->SpellBaseDamageBonusDone(GetSpellInfo()->GetSchoolMask());
                float instantDamage = 0.0f;
                float dotDamage = 0.0f;
                int empoweringLevel = 0;

                // Check if the player has the empowering aura
                if (AuraEffect const* empoweringAura = caster->GetAuraEffect(363898, EFFECT_0))
                    empoweringLevel = empoweringAura->GetAmount();

                switch (empoweringLevel)
                {
                case 1:
                    instantDamage = spellPower * 0.1134f;
                    dotDamage = spellPower * 0.0274f * 10.0f;
                    break;
                case 2:
                    instantDamage = spellPower * 0.1134f + spellPower * 0.0274f * 3.0f;
                    dotDamage = spellPower * 0.0274f * 7.0f;
                    break;
                case 3:
                    instantDamage = spellPower * 0.1134f + spellPower * 0.0274f * 6.0f;
                    dotDamage = spellPower * 0.0274f * 4.0f;
                    break;
                default:
                    break;
                }

                // Apply the instant damage and dot damage
                if (instantDamage > 0.0f)
                    caster->CastSpell(target, 363897, CastSpellExtraArgs().AddSpellBP0(instantDamage));
                if (dotDamage > 0.0f)
                    caster->CastSpell(target, 363896, CastSpellExtraArgs().AddSpellBP0(dotDamage));
            }
        }
    }

    void Register()
    {
        OnEmpowerEnd += SpellCastFn(spell_evo_fire_breath::HandleOnEmpowerEnd);
    }
};

class spell_evo_dream_breath : public SpellScript
{
    PrepareSpellScript(spell_evo_dream_breath);

    void HandleOnEmpowerEnd()
    {
        //GetSpell()->GetEmpowerReleasedStage(); THIS IS HOW YOU GET THE STAGE FOR SPELLSCRIPTS WHICH MODIFY THE MECHANICS AND THE POWERS
        GetCaster()->CastSpell(GetHitUnit(), EvokerSpells::SPELL_EVOKER_DREAM_BREATH_CHARGED, true);
    }

    void Register()
    {
        OnEmpowerEnd += SpellCastFn(spell_evo_dream_breath::HandleOnEmpowerEnd);
    }
};

// 393568 - Pyre
class spell_evo_pyre : public SpellScript
{
    PrepareSpellScript(spell_evo_pyre);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_EVOKER_PYRE_DAMAGE });
    }

    void HandleDamage(SpellEffIndex /*effIndex*/)
    {
        GetCaster()->CastSpell(GetHitUnit()->GetPosition(), SPELL_EVOKER_PYRE_DAMAGE, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_evo_pyre::HandleDamage, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 370455 - Charged Blast
class spell_evo_charged_blast : public AuraScript
{
    PrepareAuraScript(spell_evo_charged_blast);

    bool CheckProc(ProcEventInfo& procInfo)
    {
        return procInfo.GetSpellInfo() && procInfo.GetSpellInfo()->HasLabel(SPELL_LABEL_EVOKER_BLUE);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_evo_charged_blast::CheckProc);
    }
};

// 381773 - Permeating Chill
class spell_evo_permeating_chill : public AuraScript
{
    PrepareAuraScript(spell_evo_permeating_chill);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_EVOKER_PERMEATING_CHILL_TALENT });
    }

    bool CheckProc(ProcEventInfo& procInfo)
    {
        SpellInfo const* spellInfo = procInfo.GetSpellInfo();
        if (!spellInfo)
            return false;

        if (!spellInfo->HasLabel(SPELL_LABEL_EVOKER_BLUE))
            return false;

        if (!procInfo.GetActor()->HasAura(SPELL_EVOKER_PERMEATING_CHILL_TALENT))
            if (!spellInfo->IsAffected(SPELLFAMILY_EVOKER, { 0x40, 0, 0, 0 })) // disintegrate
                return false;

        return true;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_evo_permeating_chill::CheckProc);
    }
};

void AddSC_DekkCore_evoker_spell_scripts()
{
    RegisterSpellScript(spell_evoker_fire_breath_382266);
    RegisterSpellScript(spell_evo_glide);
    RegisterSpellScript(spell_evo_azure_strike);
    RegisterSpellScript(FireBreath);
    RegisterSpellScript(spell_azure_strike);
    RegisterSpellScript(spell_evo_living_flame);
    RegisterSpellScript(spell_evo_living_flame_damage);
    RegisterSpellScript(spell_soar);
    RegisterSpellScript(spell_cosmic_evoker_visage);
    RegisterSpellScript(spell_evo_fire_breath);
    RegisterSpellScript(spell_evo_dream_breath);
    RegisterSpellScript(spell_evo_pyre);
    RegisterSpellScript(spell_evo_charged_blast);
    RegisterSpellScript(spell_evo_permeating_chill);
}
