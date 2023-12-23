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

/*
 * Scripts for spells with SPELLFAMILY_GENERIC spells used by dragonrider and advanced fly spells.
 * Scriptnames of files in this file should be prefixed with "spell_dr_".
 */

#include "Containers.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "Spell.h"
#include "SpellAuraEffects.h"
#include "SpellHistory.h"
#include "SpellMgr.h"
#include "SpellScript.h"
#include <MovementPackets.h>
#include <G3D/g3dmath.h>

const float THRILL_OF_THE_SKIES_MIN_VELOCITY = 60.f;

enum DragonRidingSpells
{
    SPELL_DRAGONRIDER_ENERGIZE  = 372606,
    SPELL_THRILL_OF_THE_SKIES   = 377234
};

// 406095 - Dynamic Flight
class spell_dragonriding : public AuraScript
{
    PrepareAuraScript(spell_dragonriding);

    void OnPeriodic(AuraEffect const* /*aurEff*/)
    {
        float advFlyingVelocity = GetTarget()->GetAdvFlyingVelocity();
        bool advFylingEnabled = GetTarget()->HasAuraType(SPELL_AURA_ADVANCED_FLYING);

        float ground = GetTarget()->GetFloorZ();
        bool isInAir = (G3D::fuzzyGt(GetTarget()->GetPositionZ(), ground + GROUND_HEIGHT_TOLERANCE) || G3D::fuzzyLt(GetTarget()->GetPositionZ(), ground - GROUND_HEIGHT_TOLERANCE));

        if (isInAir && advFlyingVelocity && advFylingEnabled)
        {
            if (advFlyingVelocity > THRILL_OF_THE_SKIES_MIN_VELOCITY)
            {
                if (!GetTarget()->HasAura(SPELL_THRILL_OF_THE_SKIES))
                    GetTarget()->CastSpell(GetTarget(), SPELL_THRILL_OF_THE_SKIES, TRIGGERED_FULL_MASK);
            }
            else
                GetTarget()->RemoveAurasDueToSpell(SPELL_THRILL_OF_THE_SKIES);
        }
        else
            GetTarget()->RemoveAurasDueToSpell(SPELL_THRILL_OF_THE_SKIES);

    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_dragonriding::OnPeriodic, EFFECT_2, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 372771 - Dragonrider Energy
class spell_dragonrider_energy : public AuraScript
{
    PrepareAuraScript(spell_dragonrider_energy);

    void OnPeriodic(AuraEffect* aurEff)
    {
        if (Unit* caster = GetCaster()) {
            if (ShouldRegenEnergy(caster)) {
                int32 baseRegen = 25;

                int32 newAmount = aurEff->GetAmount() + baseRegen;

                if (newAmount >= 100) {
                    newAmount -= 100;

                    caster->CastSpell(caster, SPELL_DRAGONRIDER_ENERGIZE, TRIGGERED_FULL_MASK);
                }

                aurEff->SetAmount(newAmount);
                aurEff->GetBase()->SetNeedClientUpdateForTargets();
            }
        }
    }

    bool ShouldRegenEnergy(Unit const* caster) const
    {
        if (caster->GetPower(POWER_ALTERNATE_MOUNT) == caster->GetMaxPower(POWER_ALTERNATE_MOUNT)) {
            return false;
        }

        if (caster->GetAdvFlyingVelocity() > THRILL_OF_THE_SKIES_MIN_VELOCITY)
            return true;

        float ground = caster->GetFloorZ();
        bool isInAir = (G3D::fuzzyGt(caster->GetPositionZ(), ground + GROUND_HEIGHT_TOLERANCE) || G3D::fuzzyLt(caster->GetPositionZ(), ground - GROUND_HEIGHT_TOLERANCE));
        return !isInAir;
    }

    void Register() override
    {
        OnEffectUpdatePeriodic += AuraEffectUpdatePeriodicFn(spell_dragonrider_energy::OnPeriodic, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 374763 - Lift off
// 372610 - Skyward Ascent (Dragonriding)
class spell_dr_skyward_ascent : public SpellScript
{
    PrepareSpellScript(spell_dr_skyward_ascent);

    void HandleHitTarget(SpellEffIndex effIndex)
    {
        if (Player* caster = GetCaster()->ToPlayer())
        {
            uint32 ascentSpeed = uint32(GetSpellValue()->EffectBasePoints[effIndex]);
            caster->AddMoveImpulse(Position(0, 0, float(ascentSpeed) / 10));
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_dr_skyward_ascent::HandleHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 372608 - Surge Forward (Dragonriding)
class spell_dr_surge_forward : public SpellScript
{
    PrepareSpellScript(spell_dr_surge_forward);

    void HandleHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (Player* caster = GetCaster()->ToPlayer())
        {
            float SURGE_SPEED = 14.0f;

            float destX = caster->GetPositionX() + SURGE_SPEED * std::cos(caster->GetOrientation());
            float destY = caster->GetPositionY() + SURGE_SPEED * std::sin(caster->GetOrientation());
            float destZ = caster->GetPositionZ() + SURGE_SPEED * std::tan(caster->GetPitch());

            caster->AddMoveImpulse(Position(destX - caster->GetPositionX(), destY - caster->GetPositionY(), destZ - caster->GetPositionZ()));
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_dr_surge_forward::HandleHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 361584 - Whirling Surge (Dragonriding)
class spell_dr_whirling_surge : public SpellScript
{
    PrepareSpellScript(spell_dr_whirling_surge);

    void HandleHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (Player* caster = GetCaster()->ToPlayer())
        {
            float SURGE_SPEED = 50.0f;

            float destX = caster->GetPositionX() + SURGE_SPEED * std::cos(caster->GetOrientation());
            float destY = caster->GetPositionY() + SURGE_SPEED * std::sin(caster->GetOrientation());
            float destZ = caster->GetPositionZ() + SURGE_SPEED * std::tan(caster->GetPitch());

            caster->AddMoveImpulse(Position(destX - caster->GetPositionX(), destY - caster->GetPositionY(), destZ - caster->GetPositionZ()));

            if (!GetCaster()->HasAura(SPELL_THRILL_OF_THE_SKIES))
                GetCaster()->CastSpell(GetCaster(), SPELL_THRILL_OF_THE_SKIES, TRIGGERED_FULL_MASK);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_dr_whirling_surge::HandleHitTarget, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
    }
};

void AddSC_dragonriding_spell_scripts()
{
    RegisterSpellScript(spell_dragonriding);
    RegisterSpellScript(spell_dragonrider_energy);
    RegisterSpellScript(spell_dr_skyward_ascent);
    RegisterSpellScript(spell_dr_surge_forward);
    RegisterSpellScript(spell_dr_whirling_surge);
}
