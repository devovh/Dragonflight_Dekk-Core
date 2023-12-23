/*
 * This file is part of DekkCore Team
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

#include "AreaTriggerAI.h"
#include "G3DPosition.hpp"
#include "GridNotifiers.h"
#include "Log.h"
#include "MoveSplineInitArgs.h"
#include "ObjectAccessor.h"
#include "PathGenerator.h"
#include "MoveSplineInitArgs.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "SpellAuraEffects.h"
#include "SpellHistory.h"
#include "SpellMgr.h"
#include "SpellScript.h"
#include "TaskScheduler.h"
#include <G3D/Vector3.h>

enum PriestSpells
{
    SPELL_PRIEST_2P_S12_HEAL = 33333,
    SPELL_PRIEST_2P_S12_SHADOW = 92711,
    SPELL_PRIEST_4P_S12_HEAL = 131566,
    SPELL_PRIEST_4P_S12_SHADOW = 131556,
    SPELL_PRIEST_ABSOLUTION = 33167,
    SPELL_PRIEST_ANGELIC_FEATHER_AREATRIGGER = 158624,
    SPELL_PRIEST_ANGELIC_FEATHER_AURA = 121557,
    SPELL_PRIEST_ANGELIC_FEATHER_TRIGGER = 121536,
    SPELL_PRIEST_ARCHANGEL = 81700,
    SPELL_PRIEST_ATONEMENT = 81749,
    SPELL_PRIEST_ATONEMENT_AURA = 194384,
    SPELL_PRIEST_ATONEMENT_HEAL = 81751,
    SPELL_PRIEST_BODY_AND_SOUL_AURA = 64129,
    SPELL_PRIEST_BODY_AND_SOUL_SPEED = 65081,
    SPELL_PRIEST_CENSURE = 200199,
    SPELL_PRIEST_CONTRITION = 197419,
    SPELL_PRIEST_CONTRITION_HEAL = 270501,
    SPELL_PRIEST_CURE_DISEASE = 528,
    SPELL_PRIEST_DEVOURING_PLAGUE = 2944,
    SPELL_PRIEST_DEVOURING_PLAGUE_HEAL = 127626,
    SPELL_PRIEST_DARK_ARCHANGEL_BUFF = 197874,
    SPELL_PRIEST_DISPEL_MAGIC_FRIENDLY = 97690,
    SPELL_PRIEST_SHADOWY_INSIGHTS = 124430,
    SPELL_PRIEST_DISPEL_MAGIC_HOSTILE = 97691,
    SPELL_PRIEST_DISPERSION_SPRINT = 129960,
    SPELL_PRIEST_DIVINE_AEGIS = 47753,
    SPELL_PRIEST_DIVINE_INSIGHT_DISCIPLINE = 123266,
    SPELL_PRIEST_DIVINE_INSIGHT_HOLY = 123267,
    SPELL_PRIEST_DIVINE_INSIGHT_TALENT = 109175,
    SPELL_PRIEST_DIVINE_STAR = 110744,
    SPELL_PRIEST_DIVINE_TOUCH = 63544,
    SPELL_PRIEST_ECHO_OF_LIGHT = 77485,
    SPELL_PRIEST_ECHO_OF_LIGHT_HEAL = 77489,
    SPELL_PRIEST_EMPOWERED_RENEW = 63544,
    SPELL_PRIEST_EVANGELISM_AURA = 81662,
    SPELL_PRIEST_EVANGELISM_STACK = 81661,
    SPELL_PRIEST_FOCUSED_WILL_BUFF = 45242,
    SPELL_PRIEST_FROM_DARKNESS_COMES_LIGHT_AURA = 109186,
    SPELL_PRIEST_GRACE = 271534,
    SPELL_PRIEST_GLYPH_OF_HOLY_WORD_SANCTIFY = 55675,
    SPELL_PRIEST_GLYPH_OF_DISPEL_MAGIC = 55677,
    SPELL_PRIEST_GLYPH_OF_DISPEL_MAGIC_HEAL = 56131,
    SPELL_PRIEST_GLYPH_OF_LIGHTWELL = 55673,
    SPELL_PRIEST_GLYPH_OF_PRAYER_OF_HEALING_HEAL = 56161,
    SPELL_PRIEST_GLYPH_OF_SHADOW = 107906,
    SPELL_PRIEST_GUARDIAN_SPIRIT_AURA = 47788,
    SPELL_PRIEST_GUARDIAN_SPIRIT_HEAL = 48153,
    SPELL_PRIEST_HOLY_FIRE = 14914,
    SPELL_PRIEST_HOLY_SPARK = 131567,
    SPELL_PRIEST_HOLY_WORD_CHASTISE = 88625,
    SPELL_PRIEST_HOLY_WORD_CHASTISE_STUN = 200200,
    SPELL_PRIEST_HOLY_WORD_SANCTIFY = 34861,
    SPELL_PRIEST_HOLY_WORD_SANCTUARY_AREA = 88685,
    SPELL_PRIEST_HOLY_WORD_SANCTUARY_HEAL = 88686,
    SPELL_PRIEST_HOLY_WORD_SERENITY = 2050,
    SPELL_PRIEST_HOLY_WORDS = 63733,
    SPELL_PRIEST_INNER_FIRE = 588,
    SPELL_PRIEST_INNER_FOCUS = 89485,
    SPELL_PRIEST_INNER_WILL = 73413,
    SPELL_PRIEST_ITEM_EFFICIENCY = 37595,
    SPELL_PRIEST_LEAP_OF_FAITH = 73325,
    SPELL_PRIEST_LEAP_OF_FAITH_EFFECT = 92832,
    SPELL_PRIEST_LEAP_OF_FAITH_EFFECT_TRIGGER = 92833,
    SPELL_PRIEST_LEAP_OF_FAITH_GLYPH = 119850,
    SPELL_PRIEST_LEAP_OF_FAITH_JUMP = 110726,
    SPELL_PRIEST_LEAP_OF_FAITH_TRIGGERED = 92572,
    SPELL_PRIEST_LEVITATE = 111758,
    SPELL_PRIEST_LEVITATE_AURA = 111759,
    SPELL_PRIEST_LEVITATE_TRIGGERED = 111758,
    SPELL_PRIEST_LIGHTSPRING_RENEW = 126154,
    SPELL_PRIEST_LIGHTWELL_CHARGES = 59907,
    SPELL_PRIEST_LINGERING_INSANITY = 197937,
    SPELL_PRIEST_MANA_LEECH_PROC = 34650,
    SPELL_PRIEST_MASOCHISM = 193063,
    SPELL_PRIEST_MASOCHISM_HEAL = 193065,
    SPELL_PRIEST_MIND_BLAST = 8092,
    SPELL_PRIEST_MIND_BOMB = 205369,
    SPELL_PRIEST_MIND_BOMB_STUN = 226943,
    SPELL_PRIEST_MIND_SEAR_INSANITY = 208232,
    SPELL_PRIEST_MISERY = 238558,
    SPELL_PRIEST_NPC_PSYFIEND = 59190,
    SPELL_PRIEST_NPC_SHADOWY_APPARITION = 46954,
    SPELL_PRIEST_SHADOWY_APPARITION_MISSILE = 147193,
    SPELL_PRIEST_NPC_VOID_TENDRILS = 65282,
    SPELL_PRIEST_PENANCE = 47540,
    SPELL_PRIEST_PENANCE_DAMAGE = 47666,
    SPELL_PRIEST_PENANCE_HEAL = 47750,
    SPELL_PRIEST_PENANCE_TARGET_ALLY = 47757,
    SPELL_PRIEST_PENANCE_TARGET_ENEMY = 47758,
    SPELL_PRIEST_PHANTASM_AURA = 108942,
    SPELL_PRIEST_PHANTASM_PROC = 114239,
    SPELL_PRIEST_POWER_OF_THE_DARK_SIDE_AURA = 198069,
    SPELL_PRIEST_POWER_OF_THE_DARK_SIDE_MARKER = 225795,
    SPELL_PRIEST_PIETY = 197034,
    SPELL_PRIEST_PLEA = 200829,
    SPELL_PRIEST_PLEA_MANA = 212100,
    SPELL_PRIEST_POWER_WORD_BARRIER_BUFF = 81782,
    SPELL_PRIEST_POWER_WORD_BARRIER_VISUAL = 146810,
    SPELL_PRIEST_POWER_WORD_FORTITUDE = 21562,
    SPELL_PRIEST_POWER_WORD_SHIELD = 17,
    SPELL_PRIEST_PRAYER_OF_MENDING = 33076,
    SPELL_PRIEST_PRAYER_OF_MENDING_BUFF = 41635,
    SPELL_PRIEST_PRAYER_OF_MENDING_HEAL = 33110,
    SPELL_PRIEST_PRAYER_OF_MENDING_RADIUS = 123262,
    SPELL_PRIEST_POWER_WORD_RADIANCE = 194509,
    SPELL_PRIEST_RAPID_RENEWAL_AURA = 95649,
    SPELL_PRIEST_PURGE_THE_WICKED = 204197,
    SPELL_PRIEST_PURGE_THE_WICKED_DOT = 204213,
    SPELL_PRIEST_PURGE_THE_WICKED_SEARCHER = 204215,
    SPELL_PRIEST_RAPTURE = 47536,
    SPELL_PRIEST_RAPTURE_ENERGIZE = 47755,
    SPELL_PRIEST_REFLECTIVE_SHIELD_R1 = 33201,
    SPELL_PRIEST_REFLECTIVE_SHIELD_TRIGGERED = 33619,
    SPELL_PRIEST_SHADOWFORM = 165767,
    SPELL_PRIEST_SHADOWFORM_STANCE = 232698,
    SPELL_PRIEST_SHADOWFORM_VISUAL_WITHOUT_GLYPH = 107903,
    SPELL_PRIEST_SHADOWFORM_VISUAL_WITH_GLYPH = 107904,
    SPELL_PRIEST_SHADOW_MEND_AURA = 187464,
    SPELL_PRIEST_SHADOW_MEND_DAMAGE = 186439,
    SPELL_PRIEST_SHADOW_MEND_HEAL = 186263,
    SPELL_PRIEST_SHADOW_WORD_DEATH = 32379,
    SPELL_PRIEST_SHADOW_WORD_DEATH_ENERGIZE_KILL = 190714,
    SPELL_PRIEST_SHADOW_WORD_INSANITY_ALLOWING_CAST = 130733,
    SPELL_PRIEST_SHADOW_WORD_INSANITY_DAMAGE = 129249,
    SPELL_PRIEST_SHADOW_WORD_PAIN = 589,
    SPELL_PRIEST_SIN_AND_PUNISHMENT = 87204,
    SPELL_PRIEST_SMITE_ABSORB = 208771,
    SPELL_PRIEST_SMITE_AURA = 208772,
    SPELL_PRIEST_SOUL_OF_DIAMOND = 96219,
    SPELL_PRIEST_SPECTRAL_GUISE_CHARGES = 119030,
    SPELL_PRIEST_SPIRIT_OF_REDEMPTION_FORM = 27795,
    SPELL_PRIEST_SPIRIT_OF_REDEMPTION_IMMUNITY = 62371,
    SPELL_PRIEST_SPIRIT_OF_REDEMPTION_SHAPESHIFT = 27827,
    SPELL_PRIEST_SPIRIT_OF_REDEMPTION_UNTRANS_HERO = 25100,
    SPELL_PRIEST_SPIRIT_SHELL_ABSORPTION = 114908,
    SPELL_PRIEST_SPIRIT_SHELL_AURA = 109964,
    SPELL_PRIEST_STRENGTH_OF_SOUL = 197535,
    SPELL_PRIEST_STRENGTH_OF_SOUL_AURA = 197548,
    SPELL_PRIEST_SURGE_OF_DARKNESS = 87160,
    SPELL_PRIEST_SURGE_OF_LIGHT = 114255,
    SPELL_PRIEST_T9_HEALING_2P = 67201,
    SPELL_PRIEST_TRAIN_OF_THOUGHT = 92297,
    SPELL_PRIEST_TWIN_DISCIPLINES_RANK_1 = 47586,
    SPELL_PRIEST_TWIST_OF_FATE = 109142,
    SPELL_PRIEST_VAMPIRIC_EMBRACE_HEAL = 15290,
    SPELL_PRIEST_VAMPIRIC_TOUCH = 34914,
    SPELL_PRIEST_VAMPIRIC_TOUCH_DISPEL = 201146, // Fear
    SPELL_PRIEST_VOIDFORM = 228264,
    SPELL_PRIEST_VOIDFORM_BUFFS = 194249,
    SPELL_PRIEST_VOIDFORM_TENTACLES = 210196,
    SPELL_PRIEST_VOID_BOLT = 205448,
    SPELL_PRIEST_VOID_BOLT_DURATION = 231688,
    SPELL_PRIEST_VOID_ERUPTION = 228260,
    SPELL_PRIEST_VOID_ERUPTION_DAMAGE = 228360,
    SPELL_PRIEST_VOID_SHIFT = 108968,
    SPELL_PRIEST_VOID_TENDRILS_SUMMON = 127665,
    SPELL_PRIEST_VOID_TENDRILS_TRIGGER = 127665,
    SPELL_PRIEST_VOID_TORRENT_PREVENT_REGEN = 262173,
    SPELL_PRIEST_WEAKENED_SOUL = 6788,
    SPELL_SHADOW_PRIEST_BASE_AURA = 137033,
    SPELL_PRIEST_HOLY_WORLD_SALVATION = 265202,
    SPELL_PRIEST_RENEW = 139,
    SPELL_PRIEST_MASS_DISPELL = 32375,
    SPELL_PRIESt_PURIFY_DISEASE = 213634,
    SPELL_PRIEST_VAMPIRIC_EMBRACE = 15286,
    SPELL_PRIEST_VOID_TORRENT = 263165,
    SPELL_PRIEST_DARK_VOID = 263346,
    SPELL_PRIEST_DARK_ASCENSION = 280711,
    SPELL_PRIEST_DARK_ASCENSION_DAMAGE = 280800,

    //This needs to be scripted
    SPELL_PRIEST_LIGHT_OF_THE_NAARU_HOLY = 196985,
    SPELL_PRIEST_ENDURING_RENEWAL_HOLY = 200153,
    SPELL_PRIEST_GUARDIAN_ANGEL_HOLY = 200209,
    SPELL_PRIEST_SHINING_FORCE_HOLY = 204263,
    SPELL_TRAIL_OF_LIGHT = 200128,
    SPELL_PRIEST_TRAIL_OF_LIGHT_HEAL = 234946,
    SPELL_MIRACLE_WORKER = 235587,
    SPELL_PRIEST_HALLUCINATIONS = 280752,
    SPELL_PRIEST_HALLUCINATIONS_GIVE_POWER = 199579,
    SPELL_PRIEST_SURRENDER_TO_MADNESS = 193223,
    SPELL_PRIEST_ARMOR_OF_FAITH = 28810,
    SPELL_PRIEST_ATONEMENT_TRIGGERED = 194384,
    SPELL_PRIEST_ATONEMENT_TRIGGERED_TRINITY = 214206,
    SPELL_PRIEST_BLESSED_HEALING = 70772,
    SPELL_PRIEST_BODY_AND_SOUL = 64129,
    SPELL_PRIEST_DARK_REPRIMAND = 400169,
    SPELL_PRIEST_DARK_REPRIMAND_CHANNEL_DAMAGE = 373129,
    SPELL_PRIEST_DARK_REPRIMAND_CHANNEL_HEALING = 400171,
    SPELL_PRIEST_DARK_REPRIMAND_DAMAGE = 373130,
    SPELL_PRIEST_DARK_REPRIMAND_HEALING = 400187,
    SPELL_PRIEST_DIVINE_BLESSING = 40440,
    SPELL_PRIEST_DIVINE_STAR_HOLY = 110744,
    SPELL_PRIEST_DIVINE_STAR_SHADOW = 122121,
    SPELL_PRIEST_DIVINE_STAR_HOLY_DAMAGE = 122128,
    SPELL_PRIEST_DIVINE_STAR_HOLY_HEAL = 110745,
    SPELL_PRIEST_DIVINE_STAR_SHADOW_DAMAGE = 390845,
    SPELL_PRIEST_DIVINE_STAR_SHADOW_HEAL = 390981,
    SPELL_PRIEST_DIVINE_WRATH = 40441,
    SPELL_PRIEST_EMPOWERED_RENEW_HEAL = 391359,
    SPELL_PRIEST_FLASH_HEAL = 2061,
    SPELL_PRIEST_HALO_HOLY = 120517,
    SPELL_PRIEST_HALO_SHADOW = 120644,
    SPELL_PRIEST_HALO_HOLY_DAMAGE = 120696,
    SPELL_PRIEST_HALO_HOLY_HEAL = 120692,
    SPELL_PRIEST_HALO_SHADOW_DAMAGE = 390964,
    SPELL_PRIEST_HALO_SHADOW_HEAL = 390971,
    SPELL_PRIEST_HEAL = 2060,
    SPELL_PRIEST_HOLY_MENDING_HEAL = 391156,
    SPELL_PRIEST_LEVITATE_EFFECT = 111759,
    SPELL_PRIEST_MASOCHISM_TALENT = 193063,
    SPELL_PRIEST_MASOCHISM_PERIODIC_HEAL = 193065,
    SPELL_PRIEST_MASTERY_GRACE = 271534,
    SPELL_PRIEST_ORACULAR_HEAL = 26170,
    SPELL_PRIEST_PENANCE_CHANNEL_DAMAGE = 47758,
    SPELL_PRIEST_PENANCE_CHANNEL_HEALING = 47757,
    SPELL_PRIEST_PENANCE_HEALING = 47750,
    SPELL_PRIEST_POWER_OF_THE_DARK_SIDE = 198069,
    SPELL_PRIEST_POWER_OF_THE_DARK_SIDE_TINT = 225795,
    SPELL_PRIEST_POWER_WORD_SOLACE_ENERGIZE = 129253,
    SPELL_PRIEST_PRAYER_OF_HEALING = 596,
    SPELL_PRIEST_PURGE_THE_WICKED_DUMMY = 204215,
    SPELL_PRIEST_PURGE_THE_WICKED_PERIODIC = 204213,
    SPELL_PRIEST_RENEWED_HOPE = 197469,
    SPELL_PRIEST_RENEWED_HOPE_EFFECT = 197470,
    SPELL_PRIEST_REVEL_IN_PURITY = 373003,
    SPELL_PRIEST_SAY_YOUR_PRAYERS = 391186,
    SPELL_PRIEST_SHADOW_MEND_PERIODIC_DUMMY = 187464,
    SPELL_PRIEST_SHIELD_DISCIPLINE_ENERGIZE = 47755,
    SPELL_PRIEST_SHIELD_DISCIPLINE_PASSIVE = 197045,
    SPELL_PRIEST_SINS_OF_THE_MANY = 280398,
    SPELL_PRIEST_SMITE = 585,
    SPELL_PRIEST_SPIRIT_OF_REDEMPTION = 27827,
    SPELL_PRIEST_STRENGTH_OF_SOUL_EFFECT = 197548,
    SPELL_PRIEST_THE_PENITENT_AURA = 200347,
    SPELL_PRIEST_TRINITY = 214205,
    SPELL_PRIEST_VOID_SHIELD = 199144,
    SPELL_PRIEST_VOID_SHIELD_EFFECT = 199145,
    SPELL_PRIEST_PRAYER_OF_MENDING_AURA = 41635,
    SPELL_PRIEST_PRAYER_OF_MENDING_JUMP = 155793,
    SPELL_PRIEST_ANSWERED_PRAYERS = 394289,
    SPELL_PRIEST_APOTHEOSIS = 200183,
    SPELL_GEN_REPLENISHMENT = 57669
};

enum PriestSpellIcons
{
    PRIEST_ICON_ID_BORROWED_TIME = 2899,
    PRIEST_ICON_ID_DIVINE_TOUCH_TALENT = 3021,
    PRIEST_ICON_ID_PAIN_AND_SUFFERING = 2874
};

enum MiscSpells
{
    SPELL_VISUAL_SHADOWY_APPARITION = 33584,
    SHADOWY_APPARITION_TRAVEL_SPEED = 6
};

// Clarity of Will - 152118
class spell_pri_clarity_of_will : public AuraScript
{
    PrepareAuraScript(spell_pri_clarity_of_will);

    void CalculateAmount(AuraEffect const* aurEff, int32& amount, bool& /*canBeRecalculated*/)
    {
        if (Unit* caster = aurEff->GetCaster())
        {
            if (Player* player = caster->ToPlayer())
            {
                int32 absorbamount = int32(9.0f * player->SpellBaseHealingBonusDone(GetSpellInfo()->GetSchoolMask()));
                amount += absorbamount;
            }
        }
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pri_clarity_of_will::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
    }
};

// 78203 - Shadowy Apparitions
class spell_pri_shadowy_apparitions : public AuraScript
{
    PrepareAuraScript(spell_pri_shadowy_apparitions);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_PRIEST_SHADOWY_APPARITION_MISSILE,
                SPELL_PRIEST_SHADOW_WORD_PAIN
            });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (eventInfo.GetSpellInfo()->Id == SPELL_PRIEST_SHADOW_WORD_PAIN)
            if ((eventInfo.GetHitMask() & PROC_HIT_CRITICAL))
                return true;

        return false;
    }

    void HandleProc(AuraEffect* /*aurEff*/, ProcEventInfo& eventInfo)
    {
        if (GetTarget() && eventInfo.GetActionTarget())
        {
            GetTarget()->CastSpell(eventInfo.GetActionTarget(), SPELL_PRIEST_SHADOWY_APPARITION_MISSILE, true);
            GetTarget()->SendPlaySpellVisual(eventInfo.GetActionTarget()->GetPosition(), GetCaster()->GetOrientation(), SPELL_VISUAL_SHADOWY_APPARITION, SPELL_MISS_NONE, SPELL_MISS_NONE, SHADOWY_APPARITION_TRAVEL_SPEED, false);
        }
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_pri_shadowy_apparitions::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
        DoCheckProc += AuraCheckProcFn(spell_pri_shadowy_apparitions::CheckProc);
    }
};

// 162452 - Shadowy Insight
class spell_pri_shadowy_insight : public AuraScript
{
    PrepareAuraScript(spell_pri_shadowy_insight);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_PRIEST_MIND_BLAST
            });
    }

    void HandleProc(AuraEffect* /*aurEff*/, ProcEventInfo& /*eventInfo*/)
    {
        GetTarget()->GetSpellHistory()->ResetCharges(sSpellMgr->AssertSpellInfo(SPELL_PRIEST_MIND_BLAST, DIFFICULTY_NONE)->ChargeCategoryId);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_pri_shadowy_insight::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// Mind Blast - 8092
class spell_pri_mind_blast : public SpellScript
{
    PrepareSpellScript(spell_pri_mind_blast);

    void HandleOnHit(SpellEffIndex /*effIndex*/)
    {
        if (GetCaster()->HasAura(SPELL_PRIEST_SHADOWY_INSIGHTS))
            GetCaster()->RemoveAurasDueToSpell(SPELL_PRIEST_SHADOWY_INSIGHTS);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_pri_mind_blast::HandleOnHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// Holy Word: Chastise - 88625
class spell_pri_holy_word_chastise : public SpellScript
{
    PrepareSpellScript(spell_pri_holy_word_chastise);

    void HandleOnHit(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        Unit* target = GetHitUnit();
        if (!caster || !target)
            return;

        if (caster->HasAura(SPELL_PRIEST_CENSURE))
            caster->CastSpell(target, SPELL_PRIEST_HOLY_WORD_CHASTISE_STUN, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_pri_holy_word_chastise::HandleOnHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// Flash Heal - 2061 / Heal - 2060
class spell_pri_heal_flash_heal : public SpellScript
{
    PrepareSpellScript(spell_pri_heal_flash_heal);

    void HandleAfterCast()
    {
        Player* caster = GetCaster()->ToPlayer();
        if (!caster->ToPlayer())
            return;

        if (caster->GetSpecializationId() == TALENT_SPEC_PRIEST_HOLY)
        {
            if (caster->GetSpellHistory()->HasCooldown(SPELL_PRIEST_HOLY_WORD_SERENITY))
                caster->GetSpellHistory()->ModifyCooldown(SPELL_PRIEST_HOLY_WORD_SERENITY, Seconds (-6 * IN_MILLISECONDS));
        }
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_pri_heal_flash_heal::HandleAfterCast);
    }
};

// Binding Heal - 32546
class spell_pri_binding_heal : public SpellScript
{
    PrepareSpellScript(spell_pri_binding_heal);

    void HandleAfterCast()
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        if (caster->GetSpellHistory()->HasCooldown(SPELL_PRIEST_HOLY_WORD_SANCTIFY))
            caster->GetSpellHistory()->ModifyCooldown(SPELL_PRIEST_HOLY_WORD_SANCTIFY, Seconds(-3 * IN_MILLISECONDS));

        if (caster->GetSpellHistory()->HasCooldown(SPELL_PRIEST_HOLY_WORD_SERENITY))
            caster->GetSpellHistory()->ModifyCooldown(SPELL_PRIEST_HOLY_WORD_SERENITY, Seconds(-3 * IN_MILLISECONDS));
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_pri_binding_heal::HandleAfterCast);
    }
};

// Prayer of Healing - 596
class spell_pri_prayer_of_healing : public SpellScript
{
    PrepareSpellScript(spell_pri_prayer_of_healing);

    void HandleAfterCast()
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        if (caster->GetSpellHistory()->HasCooldown(SPELL_PRIEST_HOLY_WORD_SANCTIFY))
            caster->GetSpellHistory()->ModifyCooldown(SPELL_PRIEST_HOLY_WORD_SANCTIFY, Seconds(-6 * IN_MILLISECONDS));
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_pri_prayer_of_healing::HandleAfterCast);
    }
};

// Smite - 585
class spell_pri_smite : public SpellScript
{
    PrepareSpellScript(spell_pri_smite);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_SMITE_ABSORB });
    }

    void HandleHit(SpellEffIndex /*effIndex*/)
    {
        Player* caster = GetCaster()->ToPlayer();
        Unit* target = GetHitUnit();
        if (!caster || !target)
            return;

        if (!caster->ToPlayer())
            return;

        int32 dmg = GetHitDamage();

        if (caster->HasAura(SPELL_PRIEST_HOLY_WORDS) || caster->GetSpecializationId() == TALENT_SPEC_PRIEST_HOLY)
        {
            if (caster->GetSpellHistory()->HasCooldown(SPELL_PRIEST_HOLY_WORD_CHASTISE))
                caster->GetSpellHistory()->ModifyCooldown(SPELL_PRIEST_HOLY_WORD_CHASTISE, Seconds(-4 * IN_MILLISECONDS));
        }
      /*  if (caster->GetSpecializationId() == TALENT_SPEC_PRIEST_DISCIPLINE)
        {
            caster->CastCustomSpell(SPELL_PRIEST_SMITE_AURA, SPELLVALUE_BASE_POINT0, dmg, target, TRIGGERED_FULL_MASK);
            caster->CastCustomSpell(SPELL_PRIEST_SMITE_ABSORB, SPELLVALUE_BASE_POINT0, dmg, caster, TRIGGERED_FULL_MASK);
        }*/
    }

    void HandleAfterCast()
    {
        Player* caster = GetCaster()->ToPlayer();
        if (!caster)
            return;

        if (caster->GetSpecializationId() == TALENT_SPEC_PRIEST_HOLY)
        {
            if (caster->GetSpellHistory()->HasCooldown(SPELL_PRIEST_HOLY_WORD_CHASTISE))
                caster->GetSpellHistory()->ModifyCooldown(SPELL_PRIEST_HOLY_WORD_CHASTISE, Seconds(-6 * IN_MILLISECONDS));
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_pri_smite::HandleHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        AfterCast += SpellCastFn(spell_pri_smite::HandleAfterCast);
    }
};
//7.3.2.25549 END

// Voidform buffs - 194249
class spell_pri_voidform : public SpellScriptLoader
{
public:
    spell_pri_voidform() : SpellScriptLoader("spell_pri_voidform") {}

    class spell_pri_voidform_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_pri_voidform_AuraScript);

        void HandleApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            if (Unit* caster = GetCaster())
                caster->RemoveAurasDueToSpell(SPELL_PRIEST_LINGERING_INSANITY);
        }

        void HandlePeriodic(AuraEffect const* aurEff)
        {
            Unit* caster = GetCaster();
            if (!caster)
                return;

            // This spell must end when insanity hit 0
            if (caster->GetPower(POWER_INSANITY) == 0)
            {
                caster->RemoveAura(aurEff->GetBase());
                return;
            }

            int32 tick = GetAura()->GetStackAmount() - 1;
            switch (tick)
            {
            case 0:
                caster->CastSpell(caster, SPELL_PRIEST_VOIDFORM_TENTACLES, true);
                break;
            case 3:
                caster->CastSpell(caster, SPELL_PRIEST_VOIDFORM_TENTACLES + 1, true);
                break;
            case 6:
                caster->CastSpell(caster, SPELL_PRIEST_VOIDFORM_TENTACLES + 2, true);
                break;
            case 9:
                caster->CastSpell(caster, SPELL_PRIEST_VOIDFORM_TENTACLES + 3, true);
                break;
            default:
                break;
            }

            caster->CastSpell(caster, SPELL_PRIEST_VOIDFORM_BUFFS, true);
        }

        void HandleRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
        {
            Unit* caster = GetCaster();
            if (!caster)
                return;

            for (int i = 0; i < 4; ++i)
                caster->RemoveAurasDueToSpell(SPELL_PRIEST_VOIDFORM_TENTACLES + i);

            int32 haste = aurEff->GetAmount();
            CastSpellExtraArgs mod;
            mod.AddSpellMod(SPELLVALUE_BASE_POINT0, haste);

            if (AuraEffect* aEff = caster->GetAuraEffectOfRankedSpell(SPELL_PRIEST_VOIDFORM_BUFFS, EFFECT_3, caster->GetGUID()))
                mod.AddSpellMod(SPELLVALUE_BASE_POINT1, aEff->GetAmount());

          //  caster->CastCustomSpell(SPELL_PRIEST_LINGERING_INSANITY, mod, caster, TRIGGERED_FULL_MASK);
        }

        void Register() override
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_pri_voidform_AuraScript::HandlePeriodic, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER);
            AfterEffectRemove += AuraEffectRemoveFn(spell_pri_voidform_AuraScript::HandleRemove, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
            AfterEffectApply += AuraEffectApplyFn(spell_pri_voidform_AuraScript::HandleApply, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_pri_voidform_AuraScript();
    }
};

// Void Eruption - 228260
class spell_pri_void_eruption : public SpellScript
{
    PrepareSpellScript(spell_pri_void_eruption);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_VOID_ERUPTION, SPELL_PRIEST_VOID_ERUPTION_DAMAGE });
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        targets.remove_if([caster](WorldObject* target)
        {
            Unit* targ = target->ToUnit(); //Remove all non-unit targets
            if (!targ)
                return true;

            return !(targ->HasAura(SPELL_PRIEST_SHADOW_WORD_PAIN, caster->GetGUID()) || targ->HasAura(SPELL_PRIEST_VAMPIRIC_TOUCH, caster->GetGUID()));
        });
    }

    void HandleTakePower(SpellPowerCost& powerCost)
    {
        powerCost.Amount = 0;
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        Unit* target = GetHitUnit();
        if (!caster || !target)
            return;

        int8 spellid = std::rand() % 2; //there are two animations which should be random
        caster->CastSpell(target, SPELL_PRIEST_VOID_ERUPTION_DAMAGE + spellid, true);
    }

    void EnterVoidform()
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        caster->CastSpell(caster, SPELL_PRIEST_VOIDFORM_BUFFS, true);
        if (!caster->HasAura(SPELL_PRIEST_SHADOWFORM_STANCE))
            caster->CastSpell(caster, SPELL_PRIEST_SHADOWFORM_STANCE, true);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pri_void_eruption::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
        OnTakePower += SpellOnTakePowerFn(spell_pri_void_eruption::HandleTakePower);
        OnEffectHitTarget += SpellEffectFn(spell_pri_void_eruption::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        AfterCast += SpellCastFn(spell_pri_void_eruption::EnterVoidform);
    }
};

class PowerCheck
{
public:
    explicit PowerCheck(Powers const power) : _power(power) { }

    bool operator()(WorldObject* obj) const
    {
        if (Unit* target = obj->ToUnit())
            return target->GetPowerType() != _power;

        return true;
    }

private:
    Powers const _power;
};

class RaidCheck
{
public:
    explicit RaidCheck(Unit const* caster) : _caster(caster) { }

    bool operator()(WorldObject* obj) const
    {
        if (Unit* target = obj->ToUnit())
            return !_caster->IsInRaidWith(target);

        return true;
    }

private:
    Unit const* _caster;
};

// 34861 - Holy Word: Sanctify
class spell_pri_holy_word_sanctify : public SpellScript
{
    PrepareSpellScript(spell_pri_holy_word_sanctify);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if(RaidCheck(GetCaster()));
        targets.sort(Trinity::HealthPctOrderPred());
    }

    void OnActivate()
    {
        if (Player* player = GetCaster()->ToPlayer())
            player->GetSpellHistory()->ModifyCooldown(SPELL_PRIEST_HOLY_WORLD_SALVATION, Seconds(-30000));
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pri_holy_word_sanctify::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
        OnCast += SpellCastFn(spell_pri_holy_word_sanctify::OnActivate);
    }
};

// 527 - Dispel magic - Now called Purify
class spell_pri_purify : public SpellScriptLoader
{
public:
    spell_pri_purify() : SpellScriptLoader("spell_pri_purify") { }

    class spell_pri_purify_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_pri_purify_SpellScript);

        SpellCastResult CheckCast()
        {
            Unit* caster = GetCaster();
            Unit* target = GetExplTargetUnit();

            if (caster != target && target->IsFriendlyTo(caster))
                return SPELL_FAILED_BAD_TARGETS;
            return SPELL_CAST_OK;
        }

        void AfterEffectHit(SpellEffIndex /*effIndex*/)
        {
            if (GetHitUnit()->IsFriendlyTo(GetCaster()))
            {
                GetCaster()->CastSpell(GetHitUnit(), SPELL_PRIEST_DISPEL_MAGIC_HOSTILE, true);
                GetCaster()->CastSpell(GetHitUnit(), SPELL_PRIEST_CURE_DISEASE, true);
            }

        }

        void Register() override
        {
            OnCheckCast += SpellCheckCastFn(spell_pri_purify_SpellScript::CheckCast);
            OnEffectHitTarget += SpellEffectFn(spell_pri_purify_SpellScript::AfterEffectHit, EFFECT_0, SPELL_EFFECT_DISPEL);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_pri_purify_SpellScript();
    }
};

// 14769 - Improwed Power Shield (Removed from game)
class spell_pri_improved_power_word_shield : public SpellScriptLoader
{
public:
    spell_pri_improved_power_word_shield() : SpellScriptLoader("spell_pri_improved_power_word_shield") { }

    class spell_pri_improved_power_word_shield_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_pri_improved_power_word_shield_AuraScript);

        void HandleEffectCalcSpellMod(AuraEffect const* aurEff, SpellModifier*& spellMod)
        {
            if (!spellMod)
            {
                SpellModifierByClassMask* mod = new SpellModifierByClassMask(GetAura());
                spellMod->op = SpellModOp(aurEff->GetMiscValue());
                spellMod->type = SPELLMOD_PCT;
                spellMod->spellId = GetId();
                mod->mask = aurEff->GetSpellEffectInfo().SpellClassMask;
            }

            static_cast<SpellModifierByClassMask*>(spellMod)->value = aurEff->GetAmount();
        }

        void Register() override
        {
            DoEffectCalcSpellMod += AuraEffectCalcSpellModFn(spell_pri_improved_power_word_shield_AuraScript::HandleEffectCalcSpellMod, EFFECT_0, SPELL_AURA_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_pri_improved_power_word_shield_AuraScript();
    }
};

// 8122 - Pshychic Scream
class spell_pri_psychic_scream : public SpellScriptLoader
{
public:
    spell_pri_psychic_scream() : SpellScriptLoader("spell_pri_psychic_scream") {}

    class spell_pri_psychic_scream_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_pri_psychic_scream_AuraScript);

        bool Load() override
        {
           // if (Aura* fear = GetAura())
            //    fear->VariableStorage.Set("damage", (uint64)0);

            return true;
        }

        bool CheckProc(ProcEventInfo& eventInfo)
        {
            Unit* target = eventInfo.GetActionTarget();
            if (!target)
                return false;

            if (Aura* fear = GetAura())
            {
               // uint64 const dmg = fear->VariableStorage.GetValue<uint64>("damage");
              //  uint64 newdamage = eventInfo.GetDamageInfo()->GetDamage() + dmg;
              //  if (newdamage > target->CountPctFromMaxHealth(10))
                    fear->SetDuration(0);
            //    else
                  //  fear->VariableStorage.Set("damage", newdamage);
            }
            return true;
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_pri_psychic_scream_AuraScript::CheckProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_pri_psychic_scream_AuraScript();
    }
};

// 28305 - Mana Leech (Passive) (Priest Pet Aura)
class spell_pri_mana_leech : public SpellScriptLoader
{
public:
    spell_pri_mana_leech() : SpellScriptLoader("spell_pri_mana_leech") { }

    class spell_pri_mana_leech_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_pri_mana_leech_AuraScript);

    public:
        spell_pri_mana_leech_AuraScript()
        {
            _procTarget = nullptr;
        }

    private:
        bool Validate(SpellInfo const* /*spellInfo*/) override
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_PRIEST_MANA_LEECH_PROC, DIFFICULTY_NONE))
                return false;
            return true;
        }

        bool CheckProc(ProcEventInfo& /*eventInfo*/)
        {
            _procTarget = GetTarget()->GetOwner();
            return _procTarget != nullptr;
        }

        void HandleProc(AuraEffect* aurEff, ProcEventInfo& /*eventInfo*/)
        {
            PreventDefaultAction();
            GetTarget()->CastSpell(_procTarget, SPELL_PRIEST_MANA_LEECH_PROC, aurEff);
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_pri_mana_leech_AuraScript::CheckProc);
            OnEffectProc += AuraEffectProcFn(spell_pri_mana_leech_AuraScript::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
        }

    private:
        Unit* _procTarget;
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_pri_mana_leech_AuraScript();
    }
};

// 48045 - Mind_Sear_Base
class spell_pri_mind_sear_base : public SpellScript
{
    PrepareSpellScript(spell_pri_mind_sear_base);

    SpellCastResult CheckCast()
    {
        if (Unit* explTarget = GetExplTargetUnit())
            if (explTarget == GetCaster())
                return SPELL_FAILED_BAD_TARGETS;
        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_pri_mind_sear_base::CheckCast);
    }
};

// 49821 - Mind Sear
class spell_pri_mind_sear : public SpellScriptLoader
{
public:
    spell_pri_mind_sear() : SpellScriptLoader("spell_pri_mind_sear") {}

    class spell_pri_mind_sear_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_pri_mind_sear_SpellScript);

        bool Validate(SpellInfo const* /*spellInfo*/) override
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_PRIEST_MIND_SEAR_INSANITY, DIFFICULTY_NONE))
                return false;
            return true;
        }

        void HandleInsanity(SpellEffIndex /*effIndex*/)
        {
            GetCaster()->CastSpell(GetCaster(), SPELL_PRIEST_MIND_SEAR_INSANITY, true);
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_pri_mind_sear_SpellScript::HandleInsanity, EFFECT_1, SPELL_EFFECT_SCHOOL_DAMAGE);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_pri_mind_sear_SpellScript();
    }
};

class DelayedAuraRemoveEvent : public BasicEvent
{
public:
    DelayedAuraRemoveEvent(Unit* owner, uint32 spellId) : _owner(owner), _spellId(spellId) {}

    bool Execute(uint64 /*time*/, uint32 /*diff*/) override
    {
        _owner->RemoveAurasDueToSpell(_spellId);
        return true;
    }

private:
    Unit* _owner;
    uint32 _spellId;
};

// 204215 - Purge the Wicked
class spell_pri_purge_the_wicked_selector : public SpellScript
{
    PrepareSpellScript(spell_pri_purge_the_wicked_selector);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_PRIEST_PURGE_THE_WICKED_DOT,
                SPELL_PRIEST_PURGE_THE_WICKED
            });
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if(Trinity::UnitAuraCheck(true, SPELL_PRIEST_PURGE_THE_WICKED_DOT, GetCaster()->GetGUID()));
        targets.sort(Trinity::ObjectDistanceOrderPred(GetExplTargetUnit()));
        if (targets.size() > 1)
            targets.resize(1);
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        GetCaster()->AddAura(SPELL_PRIEST_PURGE_THE_WICKED_DOT, GetHitUnit());
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pri_purge_the_wicked_selector::FilterTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENEMY);
        OnEffectHitTarget += SpellEffectFn(spell_pri_purge_the_wicked_selector::HandleDummy, EFFECT_1, SPELL_EFFECT_DUMMY);
    }
};

// 47757 - Penance
// 47758 - Penance
class spell_pri_penance_triggered : public AuraScript
{
    PrepareAuraScript(spell_pri_penance_triggered);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_PRIEST_POWER_OF_THE_DARK_SIDE_AURA,
                SPELL_PRIEST_POWER_OF_THE_DARK_SIDE_MARKER
            });
    }

    void ApplyEffect(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* caster = GetCaster())
        {
            if (caster->HasAura(SPELL_PRIEST_POWER_OF_THE_DARK_SIDE_AURA))
            {
                caster->RemoveAurasDueToSpell(SPELL_PRIEST_POWER_OF_THE_DARK_SIDE_AURA);
                caster->CastSpell(caster, SPELL_PRIEST_POWER_OF_THE_DARK_SIDE_MARKER, true);
            }
        }
    }

    void RemoveEffect(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* caster = GetCaster()) // Penance has travel time we need to delay the aura remove a little bit...
            caster->m_Events.AddEvent(new DelayedAuraRemoveEvent(caster, SPELL_PRIEST_POWER_OF_THE_DARK_SIDE_MARKER), caster->m_Events.CalculateTime(1s));
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_pri_penance_triggered::ApplyEffect, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_pri_penance_triggered::RemoveEffect, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 47750 - Penance
// 47666 - Penance
class spell_pri_penance_heal_damage : public SpellScript
{
    PrepareSpellScript(spell_pri_penance_heal_damage);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_PRIEST_POWER_OF_THE_DARK_SIDE_MARKER,
                SPELL_PRIEST_PENANCE_HEAL
            });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (GetCaster()->GetAuraEffect(SPELL_PRIEST_CONTRITION, EFFECT_0))
            for (AuraApplication* auApp : GetCaster()->GetTargetAuraApplications(SPELL_PRIEST_ATONEMENT_AURA))
                GetCaster()->CastSpell(auApp->GetTarget(), SPELL_PRIEST_CONTRITION_HEAL, true);

        if (AuraEffect* powerOfTheDarkSide = GetCaster()->GetAuraEffect(SPELL_PRIEST_POWER_OF_THE_DARK_SIDE_MARKER, EFFECT_0))
        {
            if (GetSpellInfo()->Id == SPELL_PRIEST_PENANCE_HEAL)
            {
                int32 heal = GetHitHeal();
                AddPct(heal, powerOfTheDarkSide->GetAmount());
                SetHitHeal(heal);
            }
            else
            {
                int32 damage = GetHitDamage();
                AddPct(damage, powerOfTheDarkSide->GetAmount());
                SetHitDamage(damage);
            }
        }
    }

    void Register() override
    {
        if (m_scriptSpellId == SPELL_PRIEST_PENANCE_HEAL)
            OnEffectHitTarget += SpellEffectFn(spell_pri_penance_heal_damage::HandleDummy, EFFECT_0, SPELL_EFFECT_HEAL);
        if (m_scriptSpellId == SPELL_PRIEST_PENANCE_DAMAGE)
            OnEffectHitTarget += SpellEffectFn(spell_pri_penance_heal_damage::HandleDummy, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// 197862 - Archangel
class spell_pri_archangel : public SpellScript
{
    PrepareSpellScript(spell_pri_archangel);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_PRIEST_ATONEMENT_AURA
            });
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if(Trinity::UnitAuraCheck(false, SPELL_PRIEST_ATONEMENT_AURA, GetCaster()->GetGUID()));
    }

    void HandleScriptEffect(SpellEffIndex /*effIndex*/)
    {
        if (Aura* aura = GetHitUnit()->GetAura(SPELL_PRIEST_ATONEMENT_AURA, GetCaster()->GetGUID()))
            aura->RefreshDuration();
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_pri_archangel::HandleScriptEffect, EFFECT_2, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 197871 - Dark Archangel
class spell_pri_dark_archangel : public SpellScript
{
    PrepareSpellScript(spell_pri_dark_archangel);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_PRIEST_DARK_ARCHANGEL_BUFF
            });
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove(GetCaster());
        targets.remove_if(Trinity::UnitAuraCheck(false, SPELL_PRIEST_ATONEMENT_AURA, GetCaster()->GetGUID()));
    }

    void HandleScriptEffect(SpellEffIndex /*effIndex*/)
    {
        GetCaster()->CastSpell(GetHitUnit(), SPELL_PRIEST_DARK_ARCHANGEL_BUFF, true);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pri_dark_archangel::FilterTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ALLY);
        OnEffectHitTarget += SpellEffectFn(spell_pri_dark_archangel::HandleScriptEffect, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// -47569 - Phantasm
class spell_pri_phantasm : public SpellScriptLoader
{
public:
    spell_pri_phantasm() : SpellScriptLoader("spell_pri_phantasm") { }

    class spell_pri_phantasm_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_pri_phantasm_AuraScript);

        bool CheckProc(ProcEventInfo& /*eventInfo*/)
        {
            return roll_chance_i(GetEffect(EFFECT_0)->GetAmount());
        }

        void HandleEffectProc(AuraEffect* /*aurEff*/, ProcEventInfo& /*eventInfo*/)
        {
            PreventDefaultAction();
            GetTarget()->RemoveMovementImpairingAuras(DIFFICULTY_NONE);
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_pri_phantasm_AuraScript::CheckProc);
            OnEffectProc += AuraEffectProcFn(spell_pri_phantasm_AuraScript::HandleEffectProc, EFFECT_0, SPELL_AURA_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_pri_phantasm_AuraScript();
    }
};

// 33110 - Prayer of Mending Heal
class spell_pri_prayer_of_mending_heal : public SpellScriptLoader
{
public:
    spell_pri_prayer_of_mending_heal() : SpellScriptLoader("spell_pri_prayer_of_mending_heal") { }

    class spell_pri_prayer_of_mending_heal_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_pri_prayer_of_mending_heal_SpellScript);

        void HandleHeal(SpellEffIndex /*effIndex*/)
        {
            if (Unit* caster = GetOriginalCaster())
            {
                if (AuraEffect* aurEff = caster->GetAuraEffect(SPELL_PRIEST_T9_HEALING_2P, EFFECT_0))
                {
                    int32 heal = GetHitHeal();
                    AddPct(heal, aurEff->GetAmount());
                    SetHitHeal(heal);
                }
            }
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_pri_prayer_of_mending_heal_SpellScript::HandleHeal, EFFECT_0, SPELL_EFFECT_HEAL);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_pri_prayer_of_mending_heal_SpellScript();
    }
};

// 139 - Renew
class spell_pri_renew : public SpellScriptLoader
{
public:
    spell_pri_renew() : SpellScriptLoader("spell_pri_renew") { }

    class spell_pri_renew_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_pri_renew_AuraScript);

        bool Load() override
        {
            return GetCaster() && GetCaster()->GetTypeId() == TYPEID_PLAYER;
        }

        void HandleApplyEffect(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
        {
            if (Unit* caster = GetCaster())
            {
                // Reduse the GCD of Holy Word: Sanctify by 2 seconds
                if (caster->GetSpellHistory()->HasCooldown(SPELL_PRIEST_HOLY_WORD_SANCTIFY))
                    caster->GetSpellHistory()->ModifyCooldown(SPELL_PRIEST_HOLY_WORD_SANCTIFY, Seconds(-2 * IN_MILLISECONDS));

                // Divine Touch
                if (AuraEffect const* empoweredRenewAurEff = caster->GetDummyAuraEffect(SPELLFAMILY_PRIEST, PRIEST_ICON_ID_DIVINE_TOUCH_TALENT, EFFECT_0))
                {
                    uint32 heal = caster->SpellHealingBonusDone(GetTarget(), GetSpellInfo(), aurEff->GetAmount(), DOT, aurEff->GetSpellEffectInfo());
                    heal = GetTarget()->SpellHealingBonusTaken(caster, GetSpellInfo(), heal, DOT);
                    int32 basepoints0 = CalculatePct(int32(heal) * aurEff->GetTotalTicks(), empoweredRenewAurEff->GetAmount());
                    CastSpellExtraArgs args;
                    args.AddSpellBP0(basepoints0);
                    args.SetTriggerFlags(TRIGGERED_FULL_MASK);
                    args.SetTriggeringAura(aurEff);
                    caster->CastSpell(GetTarget(), SPELL_PRIEST_DIVINE_TOUCH, args);
                }
            }
        }

        void Register() override
        {
            OnEffectApply += AuraEffectApplyFn(spell_pri_renew_AuraScript::HandleApplyEffect, EFFECT_0, SPELL_AURA_PERIODIC_HEAL, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_pri_renew_AuraScript();
    }
};

// 32379 - Shadow Word Death
class spell_pri_shadow_word_death : public SpellScript
{
    PrepareSpellScript(spell_pri_shadow_word_death);

    void HandleDamage(SpellEffIndex /*effIndex*/)
    {
        if (Unit* target = GetHitUnit())
        {
            if (target->GetHealth() < uint64(GetHitDamage()))
                GetCaster()->CastSpell(GetCaster(), SPELL_PRIEST_SHADOW_WORD_DEATH_ENERGIZE_KILL, true);
            else
                GetCaster()->ModifyPower(POWER_INSANITY, GetSpellInfo()->GetEffect(EFFECT_2).BasePoints);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_pri_shadow_word_death::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// 232698 - Shadowform
class spell_pri_shadowform : public SpellScriptLoader
{
public:
    spell_pri_shadowform() : SpellScriptLoader("spell_pri_shadowform") { }

    class spell_pri_shadowform_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_pri_shadowform_AuraScript);

        bool Validate(SpellInfo const* /*spellInfo*/) override
        {
            return ValidateSpellInfo({ SPELL_PRIEST_SHADOWFORM_VISUAL_WITHOUT_GLYPH, SPELL_PRIEST_SHADOWFORM_VISUAL_WITH_GLYPH });
        }

        void HandleEffectApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            GetTarget()->CastSpell(GetTarget(), GetTarget()->HasAura(SPELL_PRIEST_GLYPH_OF_SHADOW) ? SPELL_PRIEST_SHADOWFORM_VISUAL_WITH_GLYPH : SPELL_PRIEST_SHADOWFORM_VISUAL_WITHOUT_GLYPH, true);
        }

        void HandleEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            GetTarget()->RemoveAurasDueToSpell(GetTarget()->HasAura(SPELL_PRIEST_GLYPH_OF_SHADOW) ? SPELL_PRIEST_SHADOWFORM_VISUAL_WITH_GLYPH : SPELL_PRIEST_SHADOWFORM_VISUAL_WITHOUT_GLYPH);
        }

        void Register() override
        {
            AfterEffectApply += AuraEffectApplyFn(spell_pri_shadowform_AuraScript::HandleEffectApply, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            AfterEffectRemove += AuraEffectRemoveFn(spell_pri_shadowform_AuraScript::HandleEffectRemove, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_pri_shadowform_AuraScript();
    }
};

// 73325
class spell_pri_leap_of_faith : public SpellScriptLoader
{

public:
    spell_pri_leap_of_faith() : SpellScriptLoader("spell_pri_leap_of_faith") {}

    class spell_pri_leap_of_faith_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_pri_leap_of_faith_SpellScript);

        bool Validate(SpellInfo const* /*spellInfo*/) override
        {
            return sSpellMgr->GetSpellInfo(SPELL_PRIEST_LEAP_OF_FAITH_GLYPH, DIFFICULTY_NONE)
                && sSpellMgr->GetSpellInfo(SPELL_PRIEST_LEAP_OF_FAITH_EFFECT, DIFFICULTY_NONE);
        }

        void HandleScript(SpellEffIndex /* effIndex */)
        {
            Unit* caster = GetCaster();
            if (!caster)
                return;

            if (caster->HasAura(SPELL_PRIEST_LEAP_OF_FAITH_GLYPH))
                GetHitUnit()->RemoveMovementImpairingAuras(DIFFICULTY_NONE);

            GetHitUnit()->CastSpell(caster, SPELL_PRIEST_LEAP_OF_FAITH_EFFECT, true);
        }

        void HandleOnHit()
        {
            if (Player* _player = GetCaster()->ToPlayer())
            {
                if (Unit* target = GetHitUnit())
                {
                    target->CastSpell(_player, SPELL_PRIEST_LEAP_OF_FAITH_JUMP, true);

                    if (_player->HasAura(SPELL_PRIEST_BODY_AND_SOUL_AURA)) _player->CastSpell(target, SPELL_PRIEST_BODY_AND_SOUL_SPEED, true);
                }
            }
        }

        void Register() override
        {
            OnHit += SpellHitFn(spell_pri_leap_of_faith_SpellScript::HandleOnHit);
            OnEffectHitTarget += SpellEffectFn(spell_pri_leap_of_faith_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_pri_leap_of_faith_SpellScript();
    }
};

class spell_pri_spirit_shell : public SpellScriptLoader
{
public:
    spell_pri_spirit_shell() : SpellScriptLoader("spell_pri_spirit_shell") { }

    class spell_pri_spirit_shell_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_pri_spirit_shell_SpellScript);

        void HandleOnHit()
        {
            if (Player* _player = GetCaster()->ToPlayer())
            {
                if (Unit* target = GetHitUnit())
                {
                    if (_player->HasAura(SPELL_PRIEST_SPIRIT_SHELL_AURA))
                    {
                        int32 bp = GetHitHeal();

                        SetHitHeal(0);

                        if (AuraEffect* shell = _player->GetAuraEffect(114908, EFFECT_0))
                            shell->SetAmount(std::min(shell->GetAmount() + bp, (int32)_player->CountPctFromMaxHealth(60)));
                        else
                        {
                            CastSpellExtraArgs args;
                            args.AddSpellBP0(bp);
                            args.SetTriggerFlags(TRIGGERED_FULL_MASK);
                            _player->CastSpell(target, SPELL_PRIEST_SPIRIT_SHELL_ABSORPTION, args);
                        }
                    }
                }
            }
        }

        void Register() override
        {
            OnHit += SpellHitFn(spell_pri_spirit_shell_SpellScript::HandleOnHit);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_pri_spirit_shell_SpellScript();
    }
};

class spell_pri_strength_of_soul : public SpellScriptLoader
{
public:
    spell_pri_strength_of_soul() : SpellScriptLoader("spell_pri_strength_of_soul") { }

    class spell_pri_strength_of_soul_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_pri_strength_of_soul_SpellScript);

        void HandleOnHit()
        {
            // TODO
        }

        void Register() override
        {
            OnHit += SpellHitFn(spell_pri_strength_of_soul_SpellScript::HandleOnHit);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_pri_strength_of_soul_SpellScript();
    }
};

class spell_pri_void_shift : public SpellScriptLoader
{
public:
    spell_pri_void_shift() : SpellScriptLoader("spell_pri_void_shift") { }

    class spell_pri_void_shift_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_pri_void_shift_SpellScript);

        bool Validate(SpellInfo const* /*spellInfo*/) override
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_PRIEST_VOID_SHIFT, DIFFICULTY_NONE))
                return false;
            return true;
        }

        SpellCastResult CheckTarget()
        {
            if (GetExplTargetUnit())
                if (GetExplTargetUnit()->GetTypeId() != TYPEID_PLAYER)
                    return SPELL_FAILED_BAD_TARGETS;

            return SPELL_CAST_OK;
        }

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            if (Player* _player = GetCaster()->ToPlayer())
            {
                if (Unit* target = GetHitUnit())
                {
                    int64 playerPct;
                    int64 targetPct;

                    playerPct = _player->GetHealthPct();
                    targetPct = target->GetHealthPct();

                    if (playerPct < 25)
                        playerPct = 25;
                    if (targetPct < 25)
                        targetPct = 25;

                    playerPct /= 100;
                    targetPct /= 100;

                    _player->SetHealth(_player->GetMaxHealth() * targetPct);
                    target->SetHealth(target->GetMaxHealth() * playerPct);
                }
            }
        }

        void Register() override
        {
            OnCheckCast += SpellCheckCastFn(spell_pri_void_shift_SpellScript::CheckTarget);
            OnEffectHitTarget += SpellEffectFn(spell_pri_void_shift_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_pri_void_shift_SpellScript;
    }
};

class spell_pri_fade : public SpellScriptLoader
{
public:
    spell_pri_fade() : SpellScriptLoader("spell_pri_fade") { }

    class spell_pri_fade_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_pri_fade_SpellScript);

        void HandleGlyph()
        {
            Unit* caster = GetCaster();
            if (caster->HasAura(159628)) // Glyph of Mass dispel
                caster->CastSpell(caster, 159630, true);
        }

        void Register() override
        {
            OnHit += SpellHitFn(spell_pri_fade_SpellScript::HandleGlyph);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_pri_fade_SpellScript();
    }
};

// Glyph of Shadow - 107906
class spell_pri_glyph_of_shadow : public SpellScriptLoader
{
public:
    spell_pri_glyph_of_shadow() : SpellScriptLoader("spell_pri_glyph_of_shadow") {}

    class spell_pri_glyph_of_shadow_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_pri_glyph_of_shadow_AuraScript);

        bool Validate(SpellInfo const* /*spellInfo*/) override
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_PRIEST_SHADOWFORM_VISUAL_WITH_GLYPH, DIFFICULTY_NONE))
                return false;
            return true;
        }

        void HandleApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            Unit* caster = GetCaster();
            if (!caster)
                return;

            caster->RemoveAurasDueToSpell(SPELL_PRIEST_SHADOWFORM_VISUAL_WITHOUT_GLYPH);
            caster->CastSpell(caster, SPELL_PRIEST_SHADOWFORM_VISUAL_WITH_GLYPH, true);
        }

        void HandleRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            Unit* caster = GetCaster();
            if (!caster)
                return;

            caster->RemoveAurasDueToSpell(SPELL_PRIEST_SHADOWFORM_VISUAL_WITH_GLYPH);
            caster->CastSpell(caster, SPELL_PRIEST_SHADOWFORM_VISUAL_WITHOUT_GLYPH, true);
        }

        void Register() override
        {
            OnEffectApply += AuraEffectApplyFn(spell_pri_glyph_of_shadow_AuraScript::HandleApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            OnEffectRemove += AuraEffectRemoveFn(spell_pri_glyph_of_shadow_AuraScript::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_pri_glyph_of_shadow_AuraScript();
    }
};

// Last Update 6.2.3
// Spirit of Redemption (Shapeshift) - 27827
class spell_pri_spirit_of_redemption_form : public AuraScript
{
    PrepareAuraScript(spell_pri_spirit_of_redemption_form);

    enum eSpells
    {
        SpiritOfRedemptionImmunity = 62371,
        SpiritOfRedemptionForm = 27795
    };

    void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
    {
        Unit* l_Target = GetTarget();

        l_Target->RemoveAura(eSpells::SpiritOfRedemptionForm);
        l_Target->RemoveAura(eSpells::SpiritOfRedemptionImmunity);
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_pri_spirit_of_redemption_form::AfterRemove, EFFECT_0, SPELL_AURA_WATER_BREATHING, AURA_EFFECT_HANDLE_REAL);
    }
};

// Smite Absorb - 208771
class spell_pri_smite_absorb : public SpellScriptLoader
{
public:
    spell_pri_smite_absorb() : SpellScriptLoader("spell_pri_smite_absorb") {}

    class spell_pri_smite_absorb_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_pri_smite_absorb_AuraScript);

        void HandleAbsorb(AuraEffect* /*aurEff*/, DamageInfo& dmgInfo, uint32& absorbAmount)
        {
            Unit* caster = GetCaster();
            Unit* attacker = dmgInfo.GetAttacker();
            if (!caster || !attacker)
                return;

            if (!attacker->HasAura(SPELL_PRIEST_SMITE_AURA, caster->GetGUID()))
                absorbAmount = 0;
            else
            {
                if (Aura* aur = attacker->GetAura(SPELL_PRIEST_SMITE_AURA, caster->GetGUID()))
                {
                    if (AuraEffect* aurEff = aur->GetEffect(EFFECT_0))
                    {
                        int32 absorb = std::max(0, aurEff->GetAmount() - int32(dmgInfo.GetDamage()));
                        if (absorb <= 0)
                        {
                            absorbAmount = aurEff->GetAmount();
                            aur->Remove();
                        }
                        else
                        {
                            aurEff->SetAmount(absorb);
                            aur->SetNeedClientUpdateForTargets();
                        }
                    }
                }
            }
        }

        void Register() override
        {
            OnEffectAbsorb += AuraEffectAbsorbFn(spell_pri_smite_absorb_AuraScript::HandleAbsorb, EFFECT_0);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_pri_smite_absorb_AuraScript();
    }
};

// Focused Will - 45243
class spell_pri_focused_will : public SpellScriptLoader
{
public:
    spell_pri_focused_will() : SpellScriptLoader("spell_pri_focused_will") {}

    class spell_pri_focused_will_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_pri_focused_will_AuraScript);

        bool Validate(SpellInfo const* /*spellInfo*/) override
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_PRIEST_FOCUSED_WILL_BUFF, DIFFICULTY_NONE))
                return false;
            return true;
        }

        bool HandleProc(ProcEventInfo& eventInfo)
        {
            Unit* caster = GetCaster();
            if (!caster)
                return false;

            if (eventInfo.GetDamageInfo()->GetAttackType() == BASE_ATTACK || eventInfo.GetDamageInfo()->GetAttackType() == OFF_ATTACK)
            {
                caster->CastSpell(caster, SPELL_PRIEST_FOCUSED_WILL_BUFF, true);
                return true;
            }

            return false;
        }

        void PreventAction(AuraEffect* /*aurEff*/, ProcEventInfo& /*eventInfo*/)
        {
            PreventDefaultAction();
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_pri_focused_will_AuraScript::HandleProc);
            OnEffectProc += AuraEffectProcFn(spell_pri_focused_will_AuraScript::PreventAction, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_pri_focused_will_AuraScript();
    }
};

// Void Bolt - 234746
class spell_pri_void_bolt : public SpellScript
{
    PrepareSpellScript(spell_pri_void_bolt);

    void HandleEffectScriptEffect(SpellEffIndex /*effIndex*/)
    {
        if (Aura* voidBoltDurationBuffAura = GetCaster()->GetAura(SPELL_PRIEST_VOID_BOLT_DURATION))
        {
            if (Unit* unit = GetHitUnit())
            {
                uint32 durationIncreaseMs = voidBoltDurationBuffAura->GetEffect(EFFECT_0)->GetBaseAmount();

                if (Aura* pain = unit->GetAura(SPELL_PRIEST_SHADOW_WORD_PAIN, GetCaster()->GetGUID()))
                    pain->ModDuration(durationIncreaseMs);

                if (Aura* vampiricTouch = unit->GetAura(SPELL_PRIEST_VAMPIRIC_TOUCH, GetCaster()->GetGUID()))
                    vampiricTouch->ModDuration(durationIncreaseMs);
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_pri_void_bolt::HandleEffectScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// Void Torrent - 205065
class aura_pri_void_torrent : public AuraScript
{
    PrepareAuraScript(aura_pri_void_torrent);

    void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->CastSpell(GetTarget(), SPELL_PRIEST_VOID_TORRENT_PREVENT_REGEN, true);
    }

    void HandleRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->RemoveAurasDueToSpell(SPELL_PRIEST_VOID_TORRENT_PREVENT_REGEN);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(aura_pri_void_torrent::OnApply, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
        AfterEffectRemove += AuraEffectRemoveFn(aura_pri_void_torrent::HandleRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
    }
};

// Angelic Feather areatrigger - created by SPELL_PRIEST_ANGELIC_FEATHER_AREATRIGGER
// AreaTriggerID - 337
struct at_pri_angelic_feather : AreaTriggerAI
{
    at_pri_angelic_feather(AreaTrigger* areatrigger) : AreaTriggerAI(areatrigger) { }

    void OnInitialize() override
    {
        if (Unit* caster = at->GetCaster())
        {
            std::vector<AreaTrigger*> areaTriggers = caster->GetAreaTriggers(SPELL_PRIEST_ANGELIC_FEATHER_AREATRIGGER);

            if (areaTriggers.size() >= 3)
                areaTriggers.front()->SetDuration(0);
        }
    }

    void OnUnitEnter(Unit* unit) override
    {
        if (Unit* caster = at->GetCaster())
        {
            if (caster->IsFriendlyTo(unit) && unit->IsPlayer())
            {
                // If target already has aura, increase duration to max 130% of initial duration
                caster->CastSpell(unit, SPELL_PRIEST_ANGELIC_FEATHER_AURA, true);
                at->SetDuration(0);
            }
        }
    }
};

// Power Word: Barrier - 62618
// AreaTriggerID - 1489
struct at_pri_power_word_barrier : AreaTriggerAI
{
    at_pri_power_word_barrier(AreaTrigger* areatrigger) : AreaTriggerAI(areatrigger) { }

    void OnUnitEnter(Unit* unit) override
    {
        Unit* caster = at->GetCaster();

        if (!caster || !unit)
            return;

        if (!caster->ToPlayer())
            return;

        if (caster->IsFriendlyTo(unit))
            caster->CastSpell(unit, SPELL_PRIEST_POWER_WORD_BARRIER_BUFF, true);
    }

    void OnUnitExit(Unit* unit) override
    {
        Unit* caster = at->GetCaster();

        if (!caster || !unit)
            return;

        if (!caster->ToPlayer())
            return;

        if (unit->HasAura(SPELL_PRIEST_POWER_WORD_BARRIER_BUFF, caster->GetGUID()))
            unit->RemoveAurasDueToSpell(SPELL_PRIEST_POWER_WORD_BARRIER_BUFF, caster->GetGUID());
    }
};

// Holy Nova - 132157
class spell_pri_holy_nova : public SpellScript
{
    PrepareSpellScript(spell_pri_holy_nova);

    void HandleOnHit()
    {
        Unit* caster = GetCaster();
        Unit* target = GetHitUnit();
        if (!caster || !target)
            return;

        if (target)
            if (roll_chance_f(20))
                caster->GetSpellHistory()->ResetCooldown(SPELL_PRIEST_HOLY_FIRE, true);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_pri_holy_nova::HandleOnHit);
    }
};

// 265202 - Holy World : Salvation
class spell_pri_holy_word_salvation : public SpellScript
{
    PrepareSpellScript(spell_pri_holy_word_salvation);


    void HandleOnHit()
    {
        if (Unit* target = GetHitUnit())
        {
            GetCaster()->AddAura(SPELL_PRIEST_RENEW, target);
            GetCaster()->CastSpell(target, SPELL_PRIEST_PRAYER_OF_MENDING, true);
        }
    }

    void HandleAfterCast()
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        std::list<Player*> friendlyList;
        caster->GetPlayerListInGrid(friendlyList, 40.0f);
        for (auto& friendPlayers : friendlyList)
        {
            if (friendPlayers->IsFriendlyTo(caster))
            {
                caster->CastSpell(friendPlayers, SPELL_PRIEST_RENEW, true);
            }
        }
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_pri_holy_word_salvation::HandleAfterCast);
        OnHit += SpellHitFn(spell_pri_holy_word_salvation::HandleOnHit);
    }
};

// 2050 - Holy Word: Serenity
class spell_pri_holy_word_serenity : public SpellScript
{
    PrepareSpellScript(spell_pri_holy_word_serenity);

    void OnActivate()
    {
        if (Player* player = GetCaster()->ToPlayer())
            player->GetSpellHistory()->ModifyCooldown(SPELL_PRIEST_HOLY_WORLD_SALVATION, Seconds(-30000));
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_pri_holy_word_serenity::OnActivate);
    }
};

//263165
class spell_priest_void_torrent : public AuraScript
{
    PrepareAuraScript(spell_priest_void_torrent);

    void OnTick(AuraEffect const* /*aurEff*/)
    {
        if (Unit* caster = GetCaster())
            caster->ModifyPower(POWER_INSANITY, +600);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_priest_void_torrent::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
    }
};

//263346
class spell_pri_dark_void : public SpellScript
{
    PrepareSpellScript(spell_pri_dark_void);

    void HandleOnHit()
    {
        Unit* caster = GetCaster();
        Unit* target = GetHitUnit();
        if (!caster || !target)
            return;

        caster->CastSpell(target, SPELL_PRIEST_SHADOW_WORD_PAIN, true);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_pri_dark_void::HandleOnHit);
    }
};

//373035 10xxx
class spell_protector_of_the_frail : public SpellScriptLoader
{
public:
    spell_protector_of_the_frail() : SpellScriptLoader("spell_protector_of_the_frail") { }

    class spell_protector_of_the_frail_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_protector_of_the_frail_AuraScript);

        void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            if (Unit* caster = GetCaster())
            {
                if (Aura* aura = caster->GetAura(33206)) // Pain Suppression spell ID
                    aura->SetCharges(aura->GetCharges() + 1);
            }
        }

        void OnProc(AuraEffect* /*aurEff*/, ProcEventInfo& eventInfo)
        {
            if (eventInfo.GetSpellInfo()->Id == 17) // Power Word: Shield spell ID
            {
                if (Unit* caster = GetCaster())
                {
                    if (Aura* aura = caster->GetAura(33206)) // Pain Suppression spell ID
                        aura->ModDuration(3000);
                }
            }
        }

        void Register() override
        {
            OnEffectApply += AuraEffectApplyFn(spell_protector_of_the_frail_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            OnEffectProc += AuraEffectProcFn(spell_protector_of_the_frail_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_protector_of_the_frail_AuraScript();
    }
};

//373049 10xxx
class spell_indemnity : public SpellScriptLoader
{
public:
    spell_indemnity() : SpellScriptLoader("spell_indemnity") { }

    class spell_indemnity_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_indemnity_AuraScript);

        void OnProc(AuraEffect* /*aurEff*/, ProcEventInfo& eventInfo)
        {
            if (eventInfo.GetSpellInfo()->Id == 17) // Power Word: Shield spell ID
            {
                if (Unit* caster = GetCaster())
                {
                    if (Aura* aura = eventInfo.GetProcTarget()->GetAura(194384)) // Atonement spell ID
                        aura->ModDuration(2000);
                }
            }
        }

        void Register() override
        {
            OnEffectProc += AuraEffectProcFn(spell_indemnity_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_indemnity_AuraScript();
    }
};

//390787 10XXX
class spell_weal_and_woe : public SpellScriptLoader
{
public:
    spell_weal_and_woe() : SpellScriptLoader("spell_weal_and_woe") { }

    class spell_weal_and_woe_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_weal_and_woe_AuraScript);

        void OnProc(AuraEffect* /*aurEff*/, ProcEventInfo& eventInfo)
        {
            if (eventInfo.GetSpellInfo()->Id == 47540) // Penance spell ID
            {
                if (Unit* caster = GetCaster())
                {
                    int32 currentStack = GetStackAmount();
                    if (currentStack < 7)
                    {
                        ModStackAmount(1);
                        caster->CastSpell(caster, 390788, true); // Weal and Woe effect trigger spell ID
                    }
                }
            }
        }

        void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            if (Unit* caster = GetCaster())
            {
                int32 currentStack = GetStackAmount();
                if (currentStack == 0)
                    return;

                caster->RemoveAura(390788); // Weal and Woe effect trigger spell ID
            }
        }

        void Register() override
        {
            OnEffectProc += AuraEffectProcFn(spell_weal_and_woe_AuraScript::OnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            OnEffectRemove += AuraEffectRemoveFn(spell_weal_and_woe_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_weal_and_woe_AuraScript();
    }
};

//373481 10xxx
class spell_pri_power_word_life : public SpellScript
{
    PrepareSpellScript(spell_pri_power_word_life);

    void HandleOnHit()
    {
        if (Unit* caster = GetCaster())
        {
            if (Unit* target = GetHitUnit())
            {
                AuraEffect const* aurEff = nullptr;
                int32 baseHeal = GetSpellInfo()->GetEffect(EFFECT_0).BasePoints;
                int32 heal = caster->SpellHealingBonusDone(target, GetSpellInfo(), baseHeal, HEAL, aurEff->GetSpellEffectInfo());
                int32 finalHeal = heal;

                if (target->GetHealthPct() <= 35.0f)
                {
                    finalHeal = heal * 5;
                    caster->GetSpellHistory()->ReduceChargeCooldown(373481, -20000);
                }

                SetHitHeal(finalHeal);
            }
        }
    }

    void Register()
    {
        OnHit += SpellHitFn(spell_pri_power_word_life::HandleOnHit);
    }
};

//390676 10xxxx
class spell_pri_inspiration : public SpellScript
{
    PrepareSpellScript(spell_pri_inspiration);

    void HandleOnHit()
    {
        if (Unit* caster = GetCaster())
        {
            if (Unit* target = GetHitUnit())
            {
                // Check if the heal was critical
                if (GetHitHeal() > 0 && IsCriticalHeal(caster, GetSpellInfo(), GetHitHeal()))
                {
                    // Apply the physical damage reduction aura
                    caster->AddAura(390677, target);
                }
            }
        }
    }

    bool IsCriticalHeal(Unit* caster, SpellInfo const* spellInfo, int32 heal)
    {
        float crit_chance = caster->SpellCriticalHealingBonus(caster, GetSpellInfo(), GetHitHeal(), caster);
        return roll_chance_f(crit_chance);
    }

    void Register()
    {
        OnHit += SpellHitFn(spell_pri_inspiration::HandleOnHit);
    }
};

// 121536 - Angelic Feather talent
class spell_pri_angelic_feather_trigger : public SpellScript
{
    PrepareSpellScript(spell_pri_angelic_feather_trigger);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_ANGELIC_FEATHER_AREATRIGGER });
    }

    void HandleEffectDummy(SpellEffIndex /*effIndex*/)
    {
        Position destPos = GetHitDest()->GetPosition();
        float radius = GetEffectInfo().CalcRadius();

        // Caster is prioritary
        if (GetCaster()->IsWithinDist2d(&destPos, radius))
        {
            GetCaster()->CastSpell(GetCaster(), SPELL_PRIEST_ANGELIC_FEATHER_AURA, true);
        }
        else
        {
            CastSpellExtraArgs args;
            args.TriggerFlags = TRIGGERED_FULL_MASK;
            args.CastDifficulty = GetCastDifficulty();
            GetCaster()->CastSpell(destPos, SPELL_PRIEST_ANGELIC_FEATHER_AREATRIGGER, args);
        }
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_pri_angelic_feather_trigger::HandleEffectDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Angelic Feather areatrigger - created by SPELL_PRIEST_ANGELIC_FEATHER_AREATRIGGER
struct areatrigger_pri_angelic_feather : AreaTriggerAI
{
    areatrigger_pri_angelic_feather(AreaTrigger* areatrigger) : AreaTriggerAI(areatrigger) { }

    // Called when the AreaTrigger has just been initialized, just before added to map
    void OnInitialize() override
    {
        if (Unit* caster = at->GetCaster())
        {
            std::vector<AreaTrigger*> areaTriggers = caster->GetAreaTriggers(SPELL_PRIEST_ANGELIC_FEATHER_AREATRIGGER);

            if (areaTriggers.size() >= 3)
                areaTriggers.front()->SetDuration(0);
        }
    }

    void OnUnitEnter(Unit* unit) override
    {
        if (Unit* caster = at->GetCaster())
        {
            if (caster->IsFriendlyTo(unit))
            {
                // If target already has aura, increase duration to max 130% of initial duration
                caster->CastSpell(unit, SPELL_PRIEST_ANGELIC_FEATHER_AURA, true);
                at->SetDuration(0);
            }
        }
    }
};

// 26169 - Oracle Healing Bonus
class spell_pri_aq_3p_bonus : public AuraScript
{
    PrepareAuraScript(spell_pri_aq_3p_bonus);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_ORACULAR_HEAL });
    }

    void HandleProc(AuraEffect* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        Unit* caster = eventInfo.GetActor();
        if (caster == eventInfo.GetProcTarget())
            return;

        HealInfo* healInfo = eventInfo.GetHealInfo();
        if (!healInfo || !healInfo->GetHeal())
            return;

        CastSpellExtraArgs args(aurEff);
        args.AddSpellBP0(CalculatePct(static_cast<int32>(healInfo->GetHeal()), 10));
        caster->CastSpell(caster, SPELL_PRIEST_ORACULAR_HEAL, args);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_pri_aq_3p_bonus::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 81749 - Atonement
class spell_pri_atonement : public AuraScript
{
    PrepareAuraScript(spell_pri_atonement);

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_ATONEMENT_HEAL, SPELL_PRIEST_SINS_OF_THE_MANY })
            && ValidateSpellEffect({ { spellInfo->Id, EFFECT_1 }, { SPELL_PRIEST_SINS_OF_THE_MANY, EFFECT_2 } });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return eventInfo.GetDamageInfo() != nullptr;
    }

    void HandleProc(AuraEffect* aurEff, ProcEventInfo& eventInfo)
    {
        DamageInfo* damageInfo = eventInfo.GetDamageInfo();
        CastSpellExtraArgs args(aurEff);
        args.AddSpellMod(SPELLVALUE_BASE_POINT0, CalculatePct(damageInfo->GetDamage(), aurEff->GetAmount()));
        _appliedAtonements.erase(std::remove_if(_appliedAtonements.begin(), _appliedAtonements.end(), [this, &args](ObjectGuid const& targetGuid)
            {
                if (Unit* target = ObjectAccessor::GetUnit(*GetTarget(), targetGuid))
                {
                    if (target->GetExactDist(GetTarget()) < GetEffectInfo(EFFECT_1).CalcValue())
                        GetTarget()->CastSpell(target, SPELL_PRIEST_ATONEMENT_HEAL, args);

                    return false;
                }
                return true;
            }), _appliedAtonements.end());
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_pri_atonement::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_pri_atonement::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }

    std::vector<ObjectGuid> _appliedAtonements;

public:
    void AddAtonementTarget(ObjectGuid const& target)
    {
        _appliedAtonements.push_back(target);

        UpdateSinsOfTheManyValue();
    }

    void RemoveAtonementTarget(ObjectGuid const& target)
    {
        _appliedAtonements.erase(std::remove(_appliedAtonements.begin(), _appliedAtonements.end(), target), _appliedAtonements.end());

        UpdateSinsOfTheManyValue();
    }

    void UpdateSinsOfTheManyValue()
    {
        // Note: the damage dimish starts at the 6th application as of 10.0.5.
        constexpr std::array<float, 20> damageByStack = { 40.0f, 40.0f, 40.0f, 40.0f, 40.0f, 35.0f, 30.0f, 25.0f, 20.0f, 15.0f, 11.0f, 8.0f, 5.0f, 4.0f, 3.0f, 2.5f, 2.0f, 1.5f, 1.25f, 1.0f };

        for (SpellEffIndex effectIndex : { EFFECT_0, EFFECT_1, EFFECT_2 })
            if (AuraEffect* sinOfTheMany = GetUnitOwner()->GetAuraEffect(SPELL_PRIEST_SINS_OF_THE_MANY, effectIndex))
                sinOfTheMany->ChangeAmount(damageByStack[std::min(_appliedAtonements.size(), damageByStack.size() - 1)]);
    }
};

// 194384, 214206 - Atonement
class spell_pri_atonement_triggered : public AuraScript
{
    PrepareAuraScript(spell_pri_atonement_triggered);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_ATONEMENT });
    }

    void HandleOnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        RegisterHelper<&spell_pri_atonement::AddAtonementTarget>();
    }

    void HandleOnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        RegisterHelper<&spell_pri_atonement::RemoveAtonementTarget>();
    }

    template<void(spell_pri_atonement::* func)(ObjectGuid const&)>
    void RegisterHelper()
    {
        if (Unit* caster = GetCaster())
            if (Aura* atonement = caster->GetAura(SPELL_PRIEST_ATONEMENT))
                if (spell_pri_atonement* script = atonement->GetScript<spell_pri_atonement>())
                    (script->*func)(GetTarget()->GetGUID());
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_pri_atonement_triggered::HandleOnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_pri_atonement_triggered::HandleOnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 204883 - Circle of Healing
class spell_pri_circle_of_healing : public SpellScript
{
    PrepareSpellScript(spell_pri_circle_of_healing);

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellEffect({ { spellInfo->Id, EFFECT_1 } });
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        // Note: we must remove one since target is always chosen.
        uint32 const maxTargets = uint32(GetSpellInfo()->GetEffect(EFFECT_1).CalcValue(GetCaster()) - 1);

        Trinity::SelectRandomInjuredTargets(targets, maxTargets, true);

        if (Unit* explicitTarget = GetExplTargetUnit())
            targets.push_front(explicitTarget);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pri_circle_of_healing::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
    }
};

// 64844 - Divine Hymn
class spell_pri_divine_hymn : public SpellScript
{
    PrepareSpellScript(spell_pri_divine_hymn);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if(RaidCheck(GetCaster()));

        uint32 const maxTargets = 3;

        if (targets.size() > maxTargets)
        {
            targets.sort(Trinity::HealthPctOrderPred());
            targets.resize(maxTargets);
        }
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pri_divine_hymn::FilterTargets, EFFECT_ALL, TARGET_UNIT_SRC_AREA_ALLY);
    }
};

// 47788 - Guardian Spirit
class spell_pri_guardian_spirit : public AuraScript
{
    PrepareAuraScript(spell_pri_guardian_spirit);

    uint32 healPct = 0;

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_GUARDIAN_SPIRIT_HEAL }) && ValidateSpellEffect({ { spellInfo->Id, EFFECT_1 } });
    }

    bool Load() override
    {
        healPct = GetEffectInfo(EFFECT_1).CalcValue();
        return true;
    }

    void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
    {
        // Set absorbtion amount to unlimited
        amount = -1;
    }

    void Absorb(AuraEffect* /*aurEff*/, DamageInfo& dmgInfo, uint32& absorbAmount)
    {
        Unit* target = GetTarget();
        if (dmgInfo.GetDamage() < target->GetHealth())
            return;

        int32 healAmount = int32(target->CountPctFromMaxHealth(healPct));
        // remove the aura now, we don't want 40% healing bonus
        Remove(AURA_REMOVE_BY_ENEMY_SPELL);
        CastSpellExtraArgs args(TRIGGERED_FULL_MASK);
        args.AddSpellBP0(healAmount);
        target->CastSpell(target, SPELL_PRIEST_GUARDIAN_SPIRIT_HEAL, args);
        absorbAmount = dmgInfo.GetDamage();
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pri_guardian_spirit::CalculateAmount, EFFECT_1, SPELL_AURA_SCHOOL_ABSORB);
        OnEffectAbsorb += AuraEffectAbsorbFn(spell_pri_guardian_spirit::Absorb, EFFECT_1);
    }
};

// 391154 - Holy Mending
class spell_pri_holy_mending : public AuraScript
{
    PrepareAuraScript(spell_pri_holy_mending);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_RENEW, SPELL_PRIEST_HOLY_MENDING_HEAL });
    }

    bool CheckProc(AuraEffect const* /*aurEff*/, ProcEventInfo& procInfo)
    {
        return procInfo.GetProcTarget()->HasAura(SPELL_PRIEST_RENEW, procInfo.GetActor()->GetGUID());
    }

    void HandleProc(AuraEffect* aurEff, ProcEventInfo& eventInfo)
    {
        eventInfo.GetActor()->CastSpell(eventInfo.GetProcTarget(), SPELL_PRIEST_HOLY_MENDING_HEAL, CastSpellExtraArgs(aurEff));
    }

    void Register() override
    {
        DoCheckEffectProc += AuraCheckEffectProcFn(spell_pri_holy_mending::CheckProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
        OnEffectProc += AuraEffectProcFn(spell_pri_holy_mending::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 63733 - Holy Words
class spell_pri_holy_words : public AuraScript
{
    PrepareAuraScript(spell_pri_holy_words);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_PRIEST_HEAL,
                SPELL_PRIEST_FLASH_HEAL,
                SPELL_PRIEST_PRAYER_OF_HEALING,
                SPELL_PRIEST_RENEW,
                SPELL_PRIEST_SMITE,
                SPELL_PRIEST_HOLY_WORD_CHASTISE,
                SPELL_PRIEST_HOLY_WORD_SANCTIFY,
                SPELL_PRIEST_HOLY_WORD_SERENITY
            }) && ValidateSpellEffect(
                {
                    { SPELL_PRIEST_HOLY_WORD_SERENITY, EFFECT_1 },
                    { SPELL_PRIEST_HOLY_WORD_SANCTIFY, EFFECT_3 },
                    { SPELL_PRIEST_HOLY_WORD_CHASTISE, EFFECT_1 }
                });
    }

    void HandleProc(AuraEffect* /*aurEff*/, ProcEventInfo& eventInfo)
    {
        SpellInfo const* spellInfo = eventInfo.GetSpellInfo();
        if (!spellInfo)
            return;

        uint32 targetSpellId;
        SpellEffIndex cdReductionEffIndex;
        switch (spellInfo->Id)
        {
        case SPELL_PRIEST_HEAL:
        case SPELL_PRIEST_FLASH_HEAL: // reduce Holy Word: Serenity cd by 6 seconds
            targetSpellId = SPELL_PRIEST_HOLY_WORD_SERENITY;
            cdReductionEffIndex = EFFECT_1;
            // cdReduction = sSpellMgr->GetSpellInfo(SPELL_PRIEST_HOLY_WORD_SERENITY, GetCastDifficulty())->GetEffect(EFFECT_1)->CalcValue(player);
            break;
        case SPELL_PRIEST_PRAYER_OF_HEALING: // reduce Holy Word: Sanctify cd by 6 seconds
            targetSpellId = SPELL_PRIEST_HOLY_WORD_SANCTIFY;
            cdReductionEffIndex = EFFECT_2;
            break;
        case SPELL_PRIEST_RENEW: // reuce Holy Word: Sanctify cd by 2 seconds
            targetSpellId = SPELL_PRIEST_HOLY_WORD_SANCTIFY;
            cdReductionEffIndex = EFFECT_3;
            break;
        case SPELL_PRIEST_SMITE: // reduce Holy Word: Chastise cd by 4 seconds
            targetSpellId = SPELL_PRIEST_HOLY_WORD_CHASTISE;
            cdReductionEffIndex = EFFECT_1;
            break;
        default:
            TC_LOG_WARN("spells.priest", "HolyWords aura has been proced by an unknown spell: {}", GetSpellInfo()->Id);
            return;
        }

        SpellInfo const* targetSpellInfo = sSpellMgr->AssertSpellInfo(targetSpellId, GetCastDifficulty());
        int32 cdReduction = targetSpellInfo->GetEffect(cdReductionEffIndex).CalcValue(GetTarget());
        GetTarget()->GetSpellHistory()->ModifyCooldown(targetSpellInfo, Seconds(-cdReduction), true);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_pri_holy_words::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 40438 - Priest Tier 6 Trinket
class spell_pri_item_t6_trinket : public AuraScript
{
    PrepareAuraScript(spell_pri_item_t6_trinket);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_DIVINE_BLESSING, SPELL_PRIEST_DIVINE_WRATH });
    }

    void HandleProc(AuraEffect* /*aurEff*/, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        Unit* caster = eventInfo.GetActor();
        if (eventInfo.GetSpellTypeMask() & PROC_SPELL_TYPE_HEAL)
            caster->CastSpell(nullptr, SPELL_PRIEST_DIVINE_BLESSING, true);

        if (eventInfo.GetSpellTypeMask() & PROC_SPELL_TYPE_DAMAGE)
            caster->CastSpell(nullptr, SPELL_PRIEST_DIVINE_WRATH, true);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_pri_item_t6_trinket::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 92833 - Leap of Faith
class spell_pri_leap_of_faith_effect_trigger : public SpellScript
{
    PrepareSpellScript(spell_pri_leap_of_faith_effect_trigger);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_LEAP_OF_FAITH_EFFECT });
    }

    void HandleEffectDummy(SpellEffIndex /*effIndex*/)
    {
        Position destPos = GetHitDest()->GetPosition();

        SpellCastTargets targets;
        targets.SetDst(destPos);
        targets.SetUnitTarget(GetCaster());
        GetHitUnit()->CastSpell(std::move(targets), GetEffectValue(), GetCastDifficulty());
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_pri_leap_of_faith_effect_trigger::HandleEffectDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 1706 - Levitate
class spell_pri_levitate : public SpellScript
{
    PrepareSpellScript(spell_pri_levitate);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_LEVITATE_EFFECT });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        GetCaster()->CastSpell(GetHitUnit(), SPELL_PRIEST_LEVITATE_EFFECT, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_pri_levitate::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 205369 - Mind Bomb
class spell_pri_mind_bomb : public AuraScript
{
    PrepareAuraScript(spell_pri_mind_bomb);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_MIND_BOMB_STUN });
    }

    void RemoveEffect(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_DEATH || GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_EXPIRE)
            if (Unit* caster = GetCaster())
                caster->CastSpell(GetTarget()->GetPosition(), SPELL_PRIEST_MIND_BOMB_STUN, true);
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_pri_mind_bomb::RemoveEffect, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 390686 - Painful Punishment
class spell_pri_painful_punishment : public AuraScript
{
    PrepareAuraScript(spell_pri_painful_punishment);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo
        ({
            SPELL_PRIEST_SHADOW_WORD_PAIN,
            SPELL_PRIEST_PURGE_THE_WICKED_PERIODIC
            });
    }

    void HandleEffectProc(AuraEffect* aurEff, ProcEventInfo& eventInfo)
    {
        Unit* caster = eventInfo.GetActor();
        Unit* target = eventInfo.GetActionTarget();
        if (!caster || !target)
            return;

        int32 additionalDuration = aurEff->GetAmount();

        if (Aura* shadowWordPain = target->GetOwnedAura(SPELL_PRIEST_SHADOW_WORD_PAIN, caster->GetGUID()))
            shadowWordPain->SetDuration(shadowWordPain->GetDuration() + additionalDuration);

        if (Aura* purgeTheWicked = target->GetOwnedAura(SPELL_PRIEST_PURGE_THE_WICKED_PERIODIC, caster->GetGUID()))
            purgeTheWicked->SetDuration(purgeTheWicked->GetDuration() + additionalDuration);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_pri_painful_punishment::HandleEffectProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 47540 - Penance
// 400169 - Dark Reprimand
class spell_pri_penance : public SpellScript
{
    PrepareSpellScript(spell_pri_penance);

public:
    spell_pri_penance(uint32 damageSpellId, uint32 healingSpellId) : _damageSpellId(damageSpellId), _healingSpellId(healingSpellId)
    {
    }

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ _damageSpellId, _healingSpellId });
    }

    SpellCastResult CheckCast()
    {
        Unit* caster = GetCaster();

        if (Unit* target = GetExplTargetUnit())
        {
            if (!caster->IsFriendlyTo(target))
            {
                if (!caster->IsValidAttackTarget(target))
                    return SPELL_FAILED_BAD_TARGETS;

                if (!caster->isInFront(target))
                    return SPELL_FAILED_UNIT_NOT_INFRONT;
            }
        }

        return SPELL_CAST_OK;
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();

        if (Unit* target = GetHitUnit())
        {
            if (caster->IsFriendlyTo(target))
                caster->CastSpell(target, _healingSpellId, CastSpellExtraArgs().SetTriggeringSpell(GetSpell()));
            else
                caster->CastSpell(target, _damageSpellId, CastSpellExtraArgs().SetTriggeringSpell(GetSpell()));
        }
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_pri_penance::CheckCast);
        OnEffectHitTarget += SpellEffectFn(spell_pri_penance::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }

private:
    uint32 _damageSpellId;
    uint32 _healingSpellId;
};

// 47758 - Penance (Channel Damage), 47757 - Penance (Channel Healing)
// 373129 - Dark Reprimand (Channel Damage), 400171 - Dark Reprimand (Channel Healing)
class spell_pri_penance_or_dark_reprimand_channeled : public AuraScript
{
    PrepareAuraScript(spell_pri_penance_or_dark_reprimand_channeled);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_POWER_OF_THE_DARK_SIDE });
    }

    void HandleOnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* caster = GetCaster())
            caster->RemoveAura(SPELL_PRIEST_POWER_OF_THE_DARK_SIDE);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_pri_penance_or_dark_reprimand_channeled::HandleOnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 198069 - Power of the Dark Side
class spell_pri_power_of_the_dark_side : public AuraScript
{
    PrepareAuraScript(spell_pri_power_of_the_dark_side);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_POWER_OF_THE_DARK_SIDE_TINT });
    }

    void HandleOnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* caster = GetCaster())
            caster->CastSpell(caster, SPELL_PRIEST_POWER_OF_THE_DARK_SIDE_TINT, true);
    }

    void HandleOnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* caster = GetCaster())
            caster->RemoveAura(SPELL_PRIEST_POWER_OF_THE_DARK_SIDE_TINT);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_pri_power_of_the_dark_side::HandleOnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_pri_power_of_the_dark_side::HandleOnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 47666 - Penance (Damage)
// 373130 - Dark Reprimand (Damage)
class spell_pri_power_of_the_dark_side_damage_bonus : public SpellScript
{
    PrepareSpellScript(spell_pri_power_of_the_dark_side_damage_bonus);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_POWER_OF_THE_DARK_SIDE });
    }

    void HandleLaunchTarget(SpellEffIndex effIndex)
    {
        if (AuraEffect* powerOfTheDarkSide = GetCaster()->GetAuraEffect(SPELL_PRIEST_POWER_OF_THE_DARK_SIDE, EFFECT_0))
        {
            PreventHitDefaultEffect(effIndex);

            float damageBonus = GetCaster()->SpellDamageBonusDone(GetHitUnit(), GetSpellInfo(), GetEffectValue(), SPELL_DIRECT_DAMAGE, GetEffectInfo());
            float value = damageBonus + damageBonus * GetEffectVariance();
            value *= 1.0f + (powerOfTheDarkSide->GetAmount() / 100.0f);
            value = GetHitUnit()->SpellDamageBonusTaken(GetCaster(), GetSpellInfo(), value, SPELL_DIRECT_DAMAGE);
            SetHitDamage(value);
        }
    }

    void Register() override
    {
        OnEffectLaunchTarget += SpellEffectFn(spell_pri_power_of_the_dark_side_damage_bonus::HandleLaunchTarget, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// 47750 - Penance (Healing)
// 400187 - Dark Reprimand (Healing)
class spell_pri_power_of_the_dark_side_healing_bonus : public SpellScript
{
    PrepareSpellScript(spell_pri_power_of_the_dark_side_healing_bonus);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_POWER_OF_THE_DARK_SIDE });
    }

    void HandleLaunchTarget(SpellEffIndex effIndex)
    {
        if (AuraEffect* powerOfTheDarkSide = GetCaster()->GetAuraEffect(SPELL_PRIEST_POWER_OF_THE_DARK_SIDE, EFFECT_0))
        {
            PreventHitDefaultEffect(effIndex);

            float healingBonus = GetCaster()->SpellHealingBonusDone(GetHitUnit(), GetSpellInfo(), GetEffectValue(), HEAL, GetEffectInfo());
            float value = healingBonus + healingBonus * GetEffectVariance();
            value *= 1.0f + (powerOfTheDarkSide->GetAmount() / 100.0f);
            value = GetHitUnit()->SpellHealingBonusTaken(GetCaster(), GetSpellInfo(), value, HEAL);
            SetHitHeal(value);
        }
    }

    void Register() override
    {
        OnEffectLaunchTarget += SpellEffectFn(spell_pri_power_of_the_dark_side_healing_bonus::HandleLaunchTarget, EFFECT_0, SPELL_EFFECT_HEAL);
    }
};

// 194509 - Power Word: Radiance
class spell_pri_power_word_radiance : public SpellScript
{
    PrepareSpellScript(spell_pri_power_word_radiance);

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_ATONEMENT, SPELL_PRIEST_ATONEMENT_TRIGGERED, SPELL_PRIEST_TRINITY })
            && ValidateSpellEffect({ { spellInfo->Id, EFFECT_3 } });
    }

    void OnTargetSelect(std::list<WorldObject*>& targets)
    {
        uint32 maxTargets = GetEffectInfo(EFFECT_2).CalcValue(GetCaster()) + 1; // adding 1 for explicit target unit
        if (targets.size() > maxTargets)
        {
            Unit* explTarget = GetExplTargetUnit();

            // Sort targets so units with no atonement are first, then units who are injured, then oher units
            // Make sure explicit target unit is first
            targets.sort([this, explTarget](WorldObject* lhs, WorldObject* rhs)
                {
                    if (lhs == explTarget) // explTarget > anything: always true
                        return true;
                    if (rhs == explTarget) // anything > explTarget: always false
                        return false;
                    return MakeSortTuple(lhs) > MakeSortTuple(rhs);
                });

            targets.resize(maxTargets);
        }
    }

    void HandleEffectHitTarget(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        if (caster->HasAura(SPELL_PRIEST_TRINITY))
            return;

        uint32 durationPct = GetEffectInfo(EFFECT_3).CalcValue(caster);
        if (caster->HasAura(SPELL_PRIEST_ATONEMENT))
            caster->CastSpell(GetHitUnit(), SPELL_PRIEST_ATONEMENT_TRIGGERED, CastSpellExtraArgs(SPELLVALUE_DURATION_PCT, durationPct).SetTriggerFlags(TRIGGERED_FULL_MASK));
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pri_power_word_radiance::OnTargetSelect, EFFECT_1, TARGET_UNIT_DEST_AREA_ALLY);
        OnEffectHitTarget += SpellEffectFn(spell_pri_power_word_radiance::HandleEffectHitTarget, EFFECT_1, SPELL_EFFECT_HEAL);
    }

private:
    std::tuple<bool, bool> MakeSortTuple(WorldObject* obj)
    {
        return std::make_tuple(IsUnitWithNoAtonement(obj), IsUnitInjured(obj));
    }

    // Returns true if obj is a unit but has no atonement
    bool IsUnitWithNoAtonement(WorldObject* obj)
    {
        Unit* unit = obj->ToUnit();
        return unit && !unit->HasAura(SPELL_PRIEST_ATONEMENT_TRIGGERED, GetCaster()->GetGUID());
    }

    // Returns true if obj is a unit and is injured
    static bool IsUnitInjured(WorldObject* obj)
    {
        Unit* unit = obj->ToUnit();
        return unit && !unit->IsFullHealth();
    }
};

// 17 - Power Word: Shield
class spell_pri_power_word_shield : public SpellScript
{
    PrepareSpellScript(spell_pri_power_word_shield);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_WEAKENED_SOUL });
    }

    SpellCastResult CheckCast()
    {
        Unit* caster = GetCaster();

        if (Unit* target = GetExplTargetUnit())
            if (!caster->HasAura(SPELL_PRIEST_RAPTURE))
                if (target->HasAura(SPELL_PRIEST_WEAKENED_SOUL, caster->GetGUID()))
                    return SPELL_FAILED_BAD_TARGETS;

        return SPELL_CAST_OK;
    }

    void HandleEffectHit()
    {
        Unit* caster = GetCaster();

        if (Unit* target = GetHitUnit())
            if (!caster->HasAura(SPELL_PRIEST_RAPTURE))
                caster->CastSpell(target, SPELL_PRIEST_WEAKENED_SOUL, true);
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_pri_power_word_shield::CheckCast);
        AfterHit += SpellHitFn(spell_pri_power_word_shield::HandleEffectHit);
    }
};

class spell_pri_power_word_shield_aura : public AuraScript
{
    PrepareAuraScript(spell_pri_power_word_shield_aura);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo
        ({
            SPELL_PRIEST_BODY_AND_SOUL,
            SPELL_PRIEST_BODY_AND_SOUL_SPEED,
            SPELL_PRIEST_STRENGTH_OF_SOUL,
            SPELL_PRIEST_STRENGTH_OF_SOUL_EFFECT,
            SPELL_PRIEST_RENEWED_HOPE,
            SPELL_PRIEST_RENEWED_HOPE_EFFECT,
            SPELL_PRIEST_VOID_SHIELD,
            SPELL_PRIEST_VOID_SHIELD_EFFECT,
            SPELL_PRIEST_ATONEMENT,
            SPELL_PRIEST_TRINITY,
            SPELL_PRIEST_ATONEMENT_TRIGGERED,
            SPELL_PRIEST_ATONEMENT_TRIGGERED_TRINITY,
            SPELL_PRIEST_SHIELD_DISCIPLINE_PASSIVE,
            SPELL_PRIEST_SHIELD_DISCIPLINE_ENERGIZE,
            SPELL_PRIEST_RAPTURE,
            SPELL_PRIEST_MASTERY_GRACE
            });
    }

    void CalculateAmount(AuraEffect const* /*auraEffect*/, int32& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = false;

        if (Unit* caster = GetCaster())
        {
            float amountF = caster->SpellBaseDamageBonusDone(GetSpellInfo()->GetSchoolMask()) * 1.65f;

            if (Player* player = caster->ToPlayer())
            {
                AddPct(amountF, player->GetRatingBonusValue(CR_VERSATILITY_DAMAGE_DONE));

                if (AuraEffect const* mastery = caster->GetAuraEffect(SPELL_PRIEST_MASTERY_GRACE, EFFECT_0))
                    if (GetUnitOwner()->HasAura(SPELL_PRIEST_ATONEMENT_TRIGGERED) || GetUnitOwner()->HasAura(SPELL_PRIEST_ATONEMENT_TRIGGERED_TRINITY))
                        AddPct(amountF, mastery->GetAmount());
            }

            if (AuraEffect const* rapture = caster->GetAuraEffect(SPELL_PRIEST_RAPTURE, EFFECT_1))
                AddPct(amountF, rapture->GetAmount());

            amount = amountF;
        }
    }

    void HandleOnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* caster = GetCaster();
        Unit* target = GetTarget();

        if (!caster)
            return;

        if (caster->HasAura(SPELL_PRIEST_BODY_AND_SOUL))
            caster->CastSpell(target, SPELL_PRIEST_BODY_AND_SOUL_SPEED, true);

        if (caster->HasAura(SPELL_PRIEST_STRENGTH_OF_SOUL))
            caster->CastSpell(target, SPELL_PRIEST_STRENGTH_OF_SOUL_EFFECT, true);

        if (caster->HasAura(SPELL_PRIEST_RENEWED_HOPE))
            caster->CastSpell(target, SPELL_PRIEST_RENEWED_HOPE_EFFECT, true);

        if (caster->HasAura(SPELL_PRIEST_VOID_SHIELD) && caster == target)
            caster->CastSpell(target, SPELL_PRIEST_VOID_SHIELD_EFFECT, true);

        if (caster->HasAura(SPELL_PRIEST_ATONEMENT))
            caster->CastSpell(target, caster->HasAura(SPELL_PRIEST_TRINITY) ? SPELL_PRIEST_ATONEMENT_TRIGGERED_TRINITY : SPELL_PRIEST_ATONEMENT_TRIGGERED, true);
    }

    void HandleOnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->RemoveAura(SPELL_PRIEST_STRENGTH_OF_SOUL_EFFECT);

        if (Unit* caster = GetCaster())
            if (GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_ENEMY_SPELL && caster->HasAura(SPELL_PRIEST_SHIELD_DISCIPLINE_PASSIVE))
                caster->CastSpell(caster, SPELL_PRIEST_SHIELD_DISCIPLINE_ENERGIZE, true);
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pri_power_word_shield_aura::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
        AfterEffectApply += AuraEffectApplyFn(spell_pri_power_word_shield_aura::HandleOnApply, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
        AfterEffectRemove += AuraEffectRemoveFn(spell_pri_power_word_shield_aura::HandleOnRemove, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL);
    }
};

// 129250 - Power Word: Solace
class spell_pri_power_word_solace : public SpellScript
{
    PrepareSpellScript(spell_pri_power_word_solace);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_POWER_WORD_SOLACE_ENERGIZE });
    }

    void RestoreMana(SpellEffIndex /*effIndex*/)
    {
        GetCaster()->CastSpell(GetCaster(), SPELL_PRIEST_POWER_WORD_SOLACE_ENERGIZE,
            CastSpellExtraArgs(TRIGGERED_IGNORE_CAST_IN_PROGRESS).SetTriggeringSpell(GetSpell())
            .AddSpellMod(SPELLVALUE_BASE_POINT0, GetEffectValue() / 100));
    }

    void Register() override
    {
        OnEffectLaunch += SpellEffectFn(spell_pri_power_word_solace::RestoreMana, EFFECT_1, SPELL_EFFECT_DUMMY);
    }
};

// Base class used by various prayer of mending spells
class spell_pri_prayer_of_mending_SpellScriptBase : public SpellScript
{
public:
    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_PRAYER_OF_MENDING_HEAL, SPELL_PRIEST_PRAYER_OF_MENDING_AURA })
            && !sSpellMgr->AssertSpellInfo(SPELL_PRIEST_PRAYER_OF_MENDING_HEAL, DIFFICULTY_NONE)->GetEffects().empty();
    }

    bool Load() override
    {
        _spellInfoHeal = sSpellMgr->AssertSpellInfo(SPELL_PRIEST_PRAYER_OF_MENDING_HEAL, DIFFICULTY_NONE);
        _healEffectDummy = &_spellInfoHeal->GetEffect(EFFECT_0);
        return true;
    }

    void CastPrayerOfMendingAura(Unit* caster, Unit* target, uint8 stack)
    {
        uint32 basePoints = caster->SpellHealingBonusDone(target, _spellInfoHeal, _healEffectDummy->CalcValue(caster), HEAL, *_healEffectDummy);
        CastSpellExtraArgs args;
        args.TriggerFlags = TRIGGERED_FULL_MASK;
        args.AddSpellMod(SPELLVALUE_AURA_STACK, stack);
        args.AddSpellMod(SPELLVALUE_BASE_POINT0, basePoints);
        caster->CastSpell(target, SPELL_PRIEST_PRAYER_OF_MENDING_AURA, args);
    }

protected:
    SpellInfo const* _spellInfoHeal;
    SpellEffectInfo const* _healEffectDummy;
};

// 33076 - Prayer of Mending
class spell_pri_prayer_of_mending : public spell_pri_prayer_of_mending_SpellScriptBase
{
    PrepareSpellScript(spell_pri_prayer_of_mending);

    void HandleEffectDummy(SpellEffIndex /*effIndex*/)
    {
        CastPrayerOfMendingAura(GetCaster(), GetHitUnit(), GetEffectValue());
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_pri_prayer_of_mending::HandleEffectDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 41635 - Prayer of Mending (Aura) - SPELL_PRIEST_PRAYER_OF_MENDING_AURA
class spell_pri_prayer_of_mending_aura : public AuraScript
{
    PrepareAuraScript(spell_pri_prayer_of_mending_aura);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo
        ({
            SPELL_PRIEST_PRAYER_OF_MENDING_HEAL,
            SPELL_PRIEST_PRAYER_OF_MENDING_JUMP,
            SPELL_PRIEST_SAY_YOUR_PRAYERS
            })
            && ValidateSpellEffect({ { SPELL_PRIEST_SAY_YOUR_PRAYERS, EFFECT_0 } });
    }

    void HandleHeal(AuraEffect* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        // Caster: player (priest) that cast the Prayer of Mending
        // Target: player that currently has Prayer of Mending aura on him
        Unit* target = GetTarget();
        if (Unit* caster = GetCaster())
        {
            // Cast the spell to heal the owner
            caster->CastSpell(target, SPELL_PRIEST_PRAYER_OF_MENDING_HEAL, aurEff);

            // Only cast jump if stack is higher than 0
            int32 stackAmount = GetStackAmount();
            if (stackAmount > 1)
            {
                CastSpellExtraArgs args(aurEff);
                args.OriginalCaster = caster->GetGUID();

                int32 newStackAmount = stackAmount - 1;
                if (AuraEffect* sayYourPrayers = caster->GetAuraEffect(SPELL_PRIEST_SAY_YOUR_PRAYERS, EFFECT_0))
                    if (roll_chance_i(sayYourPrayers->GetAmount()))
                        ++newStackAmount;

                args.AddSpellMod(SPELLVALUE_BASE_POINT0, newStackAmount);

                target->CastSpell(target, SPELL_PRIEST_PRAYER_OF_MENDING_JUMP, args);
            }

            Remove();
        }
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_pri_prayer_of_mending_aura::HandleHeal, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 155793 - Prayer of Mending (Jump) - SPELL_PRIEST_PRAYER_OF_MENDING_JUMP
class spell_pri_prayer_of_mending_jump : public spell_pri_prayer_of_mending_SpellScriptBase
{
    PrepareSpellScript(spell_pri_prayer_of_mending_jump);

    void OnTargetSelect(std::list<WorldObject*>& targets)
    {
        // Find the best target - prefer players over pets
        bool foundPlayer = false;
        for (WorldObject* worldObject : targets)
        {
            if (worldObject->IsPlayer())
            {
                foundPlayer = true;
                break;
            }
        }

        if (foundPlayer)
            targets.remove_if(Trinity::ObjectTypeIdCheck(TYPEID_PLAYER, false));

        // choose one random target from targets
        if (targets.size() > 1)
        {
            WorldObject* selected = Trinity::Containers::SelectRandomContainerElement(targets);
            targets.clear();
            targets.push_back(selected);
        }
    }

    void HandleJump(SpellEffIndex /*effIndex*/)
    {
        Unit* origCaster = GetOriginalCaster(); // the one that started the prayer of mending chain
        Unit* target = GetHitUnit(); // the target we decided the aura should jump to

        if (origCaster)
            CastPrayerOfMendingAura(origCaster, target, GetEffectValue());
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pri_prayer_of_mending_jump::OnTargetSelect, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
        OnEffectHitTarget += SpellEffectFn(spell_pri_prayer_of_mending_jump::HandleJump, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 204197 - Purge the Wicked
// Called by Penance - 47540, Dark Reprimand - 400169
class spell_pri_purge_the_wicked : public SpellScript
{
    PrepareSpellScript(spell_pri_purge_the_wicked);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo
        ({
            SPELL_PRIEST_PURGE_THE_WICKED_PERIODIC,
            SPELL_PRIEST_PURGE_THE_WICKED_DUMMY
            });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        Unit* target = GetHitUnit();

        if (target->HasAura(SPELL_PRIEST_PURGE_THE_WICKED_PERIODIC, caster->GetGUID()))
            caster->CastSpell(target, SPELL_PRIEST_PURGE_THE_WICKED_DUMMY, TRIGGERED_IGNORE_GCD | TRIGGERED_IGNORE_CAST_IN_PROGRESS);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_pri_purge_the_wicked::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 204215 - Purge the Wicked
class spell_pri_purge_the_wicked_dummy : public SpellScript
{
    PrepareSpellScript(spell_pri_purge_the_wicked_dummy);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_PURGE_THE_WICKED_PERIODIC, SPELL_PRIEST_REVEL_IN_PURITY })
            && ValidateSpellEffect({ { SPELL_PRIEST_REVEL_IN_PURITY, EFFECT_1 } });
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        Unit* caster = GetCaster();
        Unit* explTarget = GetExplTargetUnit();

        targets.remove_if([&](WorldObject* object) -> bool
            {
                // Note: we must remove any non-unit target, the explicit target and any other target that may be under any crowd control aura.
                Unit* target = object->ToUnit();
                return !target || target == explTarget || target->HasBreakableByDamageCrowdControlAura();
            });

        if (targets.empty())
            return;

        // Note: there's no SPELL_EFFECT_DUMMY with BasePoints 1 in any of the spells related to use as reference so we hardcode the value.
        uint32 spreadCount = 1;

        // Note: we must sort our list of targets whose priority is 1) aura, 2) distance, and 3) duration.
        targets.sort([&](WorldObject const* lhs, WorldObject const* rhs) -> bool
            {
                Unit const* targetA = lhs->ToUnit();
                Unit const* targetB = rhs->ToUnit();

                Aura* auraA = targetA->GetAura(SPELL_PRIEST_PURGE_THE_WICKED_PERIODIC, caster->GetGUID());
                Aura* auraB = targetB->GetAura(SPELL_PRIEST_PURGE_THE_WICKED_PERIODIC, caster->GetGUID());

                if (!auraA)
                {
                    if (auraB)
                        return true;
                    return explTarget->GetExactDist(targetA) < explTarget->GetExactDist(targetB);
                }
                if (!auraB)
                    return false;

                return auraA->GetDuration() < auraB->GetDuration();
            });

        // Note: Revel in Purity talent.
        if (caster->HasAura(SPELL_PRIEST_REVEL_IN_PURITY))
            spreadCount += sSpellMgr->AssertSpellInfo(SPELL_PRIEST_REVEL_IN_PURITY, DIFFICULTY_NONE)->GetEffect(EFFECT_1).CalcValue(GetCaster());

        if (targets.size() > spreadCount)
            targets.resize(spreadCount);
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        Unit* target = GetHitUnit();

        caster->CastSpell(target, SPELL_PRIEST_PURGE_THE_WICKED_PERIODIC, TRIGGERED_IGNORE_GCD | TRIGGERED_IGNORE_CAST_IN_PROGRESS);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pri_purge_the_wicked_dummy::FilterTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENEMY);
        OnEffectHitTarget += SpellEffectFn(spell_pri_purge_the_wicked_dummy::HandleDummy, EFFECT_1, SPELL_EFFECT_DUMMY);
    }
};

// 47536 - Rapture
class spell_pri_rapture : public SpellScript
{
    PrepareSpellScript(spell_pri_rapture);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_POWER_WORD_SHIELD });
    }

    void HandleEffectDummy(SpellEffIndex /*effIndex*/)
    {
        _raptureTarget = GetHitUnit()->GetGUID();
    }

    void HandleAfterCast()
    {
        Unit* caster = GetCaster();

        if (Unit* target = ObjectAccessor::GetUnit(*caster, _raptureTarget))
            caster->CastSpell(target, SPELL_PRIEST_POWER_WORD_SHIELD,
                CastSpellExtraArgs(TRIGGERED_IGNORE_GCD | TRIGGERED_IGNORE_POWER_AND_REAGENT_COST | TRIGGERED_IGNORE_CAST_IN_PROGRESS)
                .SetTriggeringSpell(GetSpell()));
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_pri_rapture::HandleEffectDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        AfterCast += SpellCastFn(spell_pri_rapture::HandleAfterCast);
    }

private:
    ObjectGuid _raptureTarget;
};

// 280391 - Sins of the Many
class spell_pri_sins_of_the_many : public AuraScript
{
    PrepareAuraScript(spell_pri_sins_of_the_many);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_SINS_OF_THE_MANY });
    }

    void HandleOnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->CastSpell(GetTarget(), SPELL_PRIEST_SINS_OF_THE_MANY, true);
    }

    void HandleOnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->RemoveAura(SPELL_PRIEST_SINS_OF_THE_MANY);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_pri_sins_of_the_many::HandleOnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_pri_sins_of_the_many::HandleOnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 20711 - Spirit of Redemption
class spell_pri_spirit_of_redemption : public AuraScript
{
    PrepareAuraScript(spell_pri_spirit_of_redemption);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_SPIRIT_OF_REDEMPTION });
    }

    void HandleAbsorb(AuraEffect* aurEff, DamageInfo& dmgInfo, uint32& absorbAmount)
    {
        Unit* target = GetTarget();
        target->CastSpell(target, SPELL_PRIEST_SPIRIT_OF_REDEMPTION, aurEff);
        target->SetFullHealth();

        absorbAmount = dmgInfo.GetDamage();
    }

    void Register() override
    {
        OnEffectAbsorb += AuraEffectAbsorbOverkillFn(spell_pri_spirit_of_redemption::HandleAbsorb, EFFECT_0);
    }
};

// 28809 - Greater Heal
class spell_pri_t3_4p_bonus : public AuraScript
{
    PrepareAuraScript(spell_pri_t3_4p_bonus);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_ARMOR_OF_FAITH });
    }

    void HandleProc(AuraEffect* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        eventInfo.GetActor()->CastSpell(eventInfo.GetProcTarget(), SPELL_PRIEST_ARMOR_OF_FAITH, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_pri_t3_4p_bonus::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 37594 - Greater Heal Refund
class spell_pri_t5_heal_2p_bonus : public AuraScript
{
    PrepareAuraScript(spell_pri_t5_heal_2p_bonus);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_ITEM_EFFICIENCY });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (HealInfo* healInfo = eventInfo.GetHealInfo())
            if (Unit* healTarget = healInfo->GetTarget())
                if (healInfo->GetEffectiveHeal())
                    if (healTarget->GetHealth() >= healTarget->GetMaxHealth())
                        return true;

        return false;
    }

    void HandleProc(AuraEffect* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        GetTarget()->CastSpell(GetTarget(), SPELL_PRIEST_ITEM_EFFICIENCY, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_pri_t5_heal_2p_bonus::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_pri_t5_heal_2p_bonus::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 70770 - Item - Priest T10 Healer 2P Bonus
class spell_pri_t10_heal_2p_bonus : public AuraScript
{
    PrepareAuraScript(spell_pri_t10_heal_2p_bonus);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_BLESSED_HEALING });
    }

    void HandleProc(AuraEffect* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        HealInfo* healInfo = eventInfo.GetHealInfo();
        if (!healInfo || !healInfo->GetHeal())
            return;

        SpellInfo const* spellInfo = sSpellMgr->AssertSpellInfo(SPELL_PRIEST_BLESSED_HEALING, GetCastDifficulty());
        int32 amount = CalculatePct(static_cast<int32>(healInfo->GetHeal()), aurEff->GetAmount());

        ASSERT(spellInfo->GetMaxTicks() > 0);
        amount /= spellInfo->GetMaxTicks();

        Unit* caster = eventInfo.GetActor();
        Unit* target = eventInfo.GetProcTarget();

        CastSpellExtraArgs args(aurEff);
        args.AddSpellBP0(amount);
        caster->CastSpell(target, SPELL_PRIEST_BLESSED_HEALING, args);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_pri_t10_heal_2p_bonus::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 109142 - Twist of Fate (Shadow)
// 265259 - Twist of Fate (Discipline)
class spell_pri_twist_of_fate : public AuraScript
{
    PrepareAuraScript(spell_pri_twist_of_fate);

    bool CheckProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        return eventInfo.GetProcTarget()->GetHealthPct() < aurEff->GetAmount();
    }

    void Register() override
    {
        DoCheckEffectProc += AuraCheckEffectProcFn(spell_pri_twist_of_fate::CheckProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 15286 - Vampiric Embrace
class spell_pri_vampiric_embrace : public AuraScript
{
    PrepareAuraScript(spell_pri_vampiric_embrace);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_VAMPIRIC_EMBRACE_HEAL });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        // Not proc from Mind Sear
        return !(eventInfo.GetProcSpell()->GetSpellInfo()->SpellFamilyFlags[1] & 0x80000);
    }

    void HandleEffectProc(AuraEffect* aurEff, ProcEventInfo& eventInfo)
    {
        //PreventDefaultAction();
        DamageInfo* damageInfo = eventInfo.GetDamageInfo();
        if (!damageInfo || !damageInfo->GetDamage())
            return;

        int32 selfHeal = int32(CalculatePct(damageInfo->GetDamage(), aurEff->GetAmount()));
        int32 teamHeal = selfHeal / 2;

        CastSpellExtraArgs args(aurEff);
        args.AddSpellMod(SPELLVALUE_BASE_POINT0, teamHeal);
        args.AddSpellMod(SPELLVALUE_BASE_POINT1, selfHeal);
        GetTarget()->CastSpell(nullptr, SPELL_PRIEST_VAMPIRIC_EMBRACE_HEAL, args);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_pri_vampiric_embrace::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_pri_vampiric_embrace::HandleEffectProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 15290 - Vampiric Embrace (heal)
class spell_pri_vampiric_embrace_target : public SpellScript
{
    PrepareSpellScript(spell_pri_vampiric_embrace_target);

    void FilterTargets(std::list<WorldObject*>& unitList)
    {
        unitList.remove(GetCaster());
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pri_vampiric_embrace_target::FilterTargets, EFFECT_0, TARGET_UNIT_CASTER_AREA_PARTY);
    }
};

// 34914 - Vampiric Touch
class spell_pri_vampiric_touch : public AuraScript
{
    PrepareAuraScript(spell_pri_vampiric_touch);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_VAMPIRIC_TOUCH_DISPEL, SPELL_GEN_REPLENISHMENT });
    }

    void HandleDispel(DispelInfo* /*dispelInfo*/)
    {
        if (Unit* caster = GetCaster())
        {
            if (Unit* target = GetUnitOwner())
            {
                if (AuraEffect const* aurEff = GetEffect(EFFECT_1))
                {
                    // backfire damage
                    int32 bp = aurEff->GetAmount();
                    bp = target->SpellDamageBonusTaken(caster, aurEff->GetSpellInfo(), bp, DOT);
                    bp *= 8;

                    CastSpellExtraArgs args(aurEff);
                    args.AddSpellBP0(bp);
                    caster->CastSpell(target, SPELL_PRIEST_VAMPIRIC_TOUCH_DISPEL, args);
                }
            }
        }
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return eventInfo.GetProcTarget() == GetCaster();
    }

    void HandleEffectProc(AuraEffect* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        eventInfo.GetProcTarget()->CastSpell(nullptr, SPELL_GEN_REPLENISHMENT, aurEff);
    }

    void Register() override
    {
        AfterDispel += AuraDispelFn(spell_pri_vampiric_touch::HandleDispel);
        DoCheckProc += AuraCheckProcFn(spell_pri_vampiric_touch::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_pri_vampiric_touch::HandleEffectProc, EFFECT_2, SPELL_AURA_DUMMY);
    }
};

// 122121 - Divine Star (Shadow)
class spell_pri_divine_star_shadow : public SpellScript
{
    PrepareSpellScript(spell_pri_divine_star_shadow);

    void HandleHitTarget(SpellEffIndex effIndex)
    {
        Unit* caster = GetCaster();

        if (caster->GetPowerType() != GetEffectInfo().MiscValue)
            PreventHitDefaultEffect(effIndex);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_pri_divine_star_shadow::HandleHitTarget, EFFECT_2, SPELL_EFFECT_ENERGIZE);
    }
};

// 110744 - Divine Star (Holy)
// 122121 - Divine Star (Shadow)
struct areatrigger_pri_divine_star : AreaTriggerAI
{
    areatrigger_pri_divine_star(AreaTrigger* areatrigger) : AreaTriggerAI(areatrigger), _maxTravelDistance(0.0f) { }

    void OnInitialize() override
    {
        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(at->GetSpellId(), DIFFICULTY_NONE);
        if (!spellInfo)
            return;

        if (spellInfo->GetEffects().size() <= EFFECT_1)
            return;

        Unit* caster = at->GetCaster();
        if (!caster)
            return;

        _casterCurrentPosition = caster->GetPosition();

        // Note: max. distance at which the Divine Star can travel to is EFFECT_1's BasePoints yards.
        _maxTravelDistance = float(spellInfo->GetEffect(EFFECT_1).CalcValue(caster));

        Position destPos = _casterCurrentPosition;
        at->MovePositionToFirstCollision(destPos, _maxTravelDistance, 0.0f);

        PathGenerator firstPath(at);
        firstPath.CalculatePath(destPos.GetPositionX(), destPos.GetPositionY(), destPos.GetPositionZ(), false);

        G3D::Vector3 const& endPoint = firstPath.GetPath().back();

        // Note: it takes 1000ms to reach EFFECT_1's BasePoints yards, so it takes (1000 / EFFECT_1's BasePoints)ms to run 1 yard.
        at->InitSplines(firstPath.GetPath(), at->GetDistance(endPoint.x, endPoint.y, endPoint.z) * float(1000 / _maxTravelDistance));
    }

    void OnUpdate(uint32 diff) override
    {
        _scheduler.Update(diff);
    }

    void OnUnitEnter(Unit* unit) override
    {
        HandleUnitEnterExit(unit);
    }

    void OnUnitExit(Unit* unit) override
    {
        // Note: this ensures any unit receives a second hit if they happen to be inside the AT when Divine Star starts its return path.
        HandleUnitEnterExit(unit);
    }

    void HandleUnitEnterExit(Unit* unit)
    {
        Unit* caster = at->GetCaster();
        if (!caster)
            return;

        if (std::find(_affectedUnits.begin(), _affectedUnits.end(), unit->GetGUID()) != _affectedUnits.end())
            return;

        constexpr TriggerCastFlags TriggerFlags = TRIGGERED_IGNORE_GCD | TRIGGERED_IGNORE_CAST_IN_PROGRESS;

        if (caster->IsValidAttackTarget(unit))
            caster->CastSpell(unit, at->GetSpellId() == SPELL_PRIEST_DIVINE_STAR_SHADOW ? SPELL_PRIEST_DIVINE_STAR_SHADOW_DAMAGE : SPELL_PRIEST_DIVINE_STAR_HOLY_DAMAGE,
                TriggerFlags);
        else if (caster->IsValidAssistTarget(unit))
            caster->CastSpell(unit, at->GetSpellId() == SPELL_PRIEST_DIVINE_STAR_SHADOW ? SPELL_PRIEST_DIVINE_STAR_SHADOW_HEAL : SPELL_PRIEST_DIVINE_STAR_HOLY_HEAL,
                TriggerFlags);

        _affectedUnits.push_back(unit->GetGUID());
    }

    void OnDestinationReached() override
    {
        Unit* caster = at->GetCaster();
        if (!caster)
            return;

        if (at->GetDistance(_casterCurrentPosition) > 0.05f)
        {
            _affectedUnits.clear();

            ReturnToCaster();
        }
        else
            at->Remove();
    }

    void ReturnToCaster()
    {
        _scheduler.Schedule(0ms, [this](TaskContext task)
            {
                Unit* caster = at->GetCaster();
                if (!caster)
                    return;

                _casterCurrentPosition = caster->GetPosition();

                Movement::PointsArray returnSplinePoints;

                returnSplinePoints.push_back(PositionToVector3(at));
                returnSplinePoints.push_back(PositionToVector3(at));
                returnSplinePoints.push_back(PositionToVector3(caster));
                returnSplinePoints.push_back(PositionToVector3(caster));

                at->InitSplines(returnSplinePoints, uint32(at->GetDistance(caster) / _maxTravelDistance * 1000));

                task.Repeat(250ms);
            });
    }

private:
    TaskScheduler _scheduler;
    Position _casterCurrentPosition;
    std::vector<ObjectGuid> _affectedUnits;
    float _maxTravelDistance;
};

// 391339 - Empowered Renew
class spell_pri_empowered_renew : public AuraScript
{
    PrepareAuraScript(spell_pri_empowered_renew);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_RENEW, SPELL_PRIEST_EMPOWERED_RENEW_HEAL })
            && ValidateSpellEffect({ { SPELL_PRIEST_RENEW, EFFECT_0 } })
            && sSpellMgr->AssertSpellInfo(SPELL_PRIEST_RENEW, DIFFICULTY_NONE)->GetEffect(EFFECT_0).IsAura(SPELL_AURA_PERIODIC_HEAL);
    }

    void HandleProc(AuraEffect* aurEff, ProcEventInfo& eventInfo)
    {
        Unit* caster = eventInfo.GetActor();
        Unit* target = eventInfo.GetProcTarget();

        SpellInfo const* renewSpellInfo = sSpellMgr->AssertSpellInfo(SPELL_PRIEST_RENEW, GetCastDifficulty());
        SpellEffectInfo const& renewEffect = renewSpellInfo->GetEffect(EFFECT_0);
        int32 estimatedTotalHeal = AuraEffect::CalculateEstimatedfTotalPeriodicAmount(caster, target, renewSpellInfo, renewEffect, renewEffect.CalcValue(caster), 1);
        int32 healAmount = CalculatePct(estimatedTotalHeal, aurEff->GetAmount());

        caster->CastSpell(target, SPELL_PRIEST_EMPOWERED_RENEW_HEAL, CastSpellExtraArgs(aurEff).AddSpellBP0(healAmount));
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_pri_empowered_renew::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 120644 - Halo (Shadow)
class spell_pri_halo_shadow : public SpellScript
{
    PrepareSpellScript(spell_pri_halo_shadow);

    void HandleHitTarget(SpellEffIndex effIndex)
    {
        Unit* caster = GetCaster();

        if (caster->GetPowerType() != GetEffectInfo().MiscValue)
            PreventHitDefaultEffect(effIndex);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_pri_halo_shadow::HandleHitTarget, EFFECT_1, SPELL_EFFECT_ENERGIZE);
    }
};

// 120517 - Halo (Holy)
// 120644 - Halo (Shadow)
struct areatrigger_pri_halo : AreaTriggerAI
{
    areatrigger_pri_halo(AreaTrigger* areatrigger) : AreaTriggerAI(areatrigger) {}

    void OnUnitEnter(Unit* unit) override
    {
        if (Unit* caster = at->GetCaster())
        {
            if (caster->IsValidAttackTarget(unit))
                caster->CastSpell(unit, at->GetSpellId() == SPELL_PRIEST_HALO_SHADOW ? SPELL_PRIEST_HALO_SHADOW_DAMAGE : SPELL_PRIEST_HALO_HOLY_DAMAGE,
                    TRIGGERED_IGNORE_GCD | TRIGGERED_IGNORE_CAST_IN_PROGRESS);
            else if (caster->IsValidAssistTarget(unit))
                caster->CastSpell(unit, at->GetSpellId() == SPELL_PRIEST_HALO_SHADOW ? SPELL_PRIEST_HALO_SHADOW_HEAL : SPELL_PRIEST_HALO_HOLY_HEAL,
                    TRIGGERED_IGNORE_GCD | TRIGGERED_IGNORE_CAST_IN_PROGRESS);
        }
    }
};

// 200128 - Trail of Light
class spell_pri_trail_of_light : public AuraScript
{
    PrepareAuraScript(spell_pri_trail_of_light);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_TRAIL_OF_LIGHT_HEAL });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (_healQueue.empty() || _healQueue.back() != eventInfo.GetActionTarget()->GetGUID())
            _healQueue.push(eventInfo.GetActionTarget()->GetGUID());

        if (_healQueue.size() > 2)
            _healQueue.pop();

        if (_healQueue.size() == 2)
            return true;

        return false;
    }

    void HandleOnProc(AuraEffect* aurEff, ProcEventInfo& eventInfo)
    {
        Unit* caster = GetTarget();
        Unit* oldTarget = ObjectAccessor::GetUnit(*caster, _healQueue.front());
        if (!oldTarget)
            return;

        // Note: old target may not be friendly anymore due to charm and faction change effects.
        if (!caster->IsValidAssistTarget(oldTarget))
            return;

        SpellInfo const* healSpellInfo = sSpellMgr->GetSpellInfo(SPELL_PRIEST_TRAIL_OF_LIGHT_HEAL, DIFFICULTY_NONE);
        if (!healSpellInfo)
            return;

        // Note: distance may be greater than the heal's spell range.
        if (!caster->IsWithinDist(oldTarget, healSpellInfo->GetMaxRange(true, caster)))
            return;

        uint32 healAmount = CalculatePct(eventInfo.GetHealInfo()->GetHeal(), aurEff->GetAmount());

        caster->CastSpell(oldTarget, SPELL_PRIEST_TRAIL_OF_LIGHT_HEAL, CastSpellExtraArgs(aurEff).AddSpellBP0(healAmount));
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_pri_trail_of_light::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_pri_trail_of_light::HandleOnProc, EFFECT_0, SPELL_AURA_DUMMY);
    }

private:
    std::queue<ObjectGuid> _healQueue;
};

// 391387 - Answered Prayers
class spell_pri_answered_prayers : public AuraScript
{
    PrepareAuraScript(spell_pri_answered_prayers);

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_ANSWERED_PRAYERS, SPELL_PRIEST_APOTHEOSIS })
            && spellInfo->GetEffects().size() > EFFECT_1;
    }

    void HandleOnProc(AuraEffect* aurEff, ProcEventInfo& eventInfo)
    {
        Milliseconds extraDuration = 0ms;
        if (AuraEffect const* durationEffect = GetEffect(EFFECT_1))
            extraDuration = Seconds(durationEffect->GetAmount());

        Unit* target = eventInfo.GetActor();

        Aura* answeredPrayers = target->GetAura(SPELL_PRIEST_ANSWERED_PRAYERS);

        // Note: if caster has no aura, we must cast it first.
        if (!answeredPrayers)
            target->CastSpell(target, SPELL_PRIEST_ANSWERED_PRAYERS, true);
        else
        {
            // Note: there's no BaseValue dummy that we can use as reference, so we hardcode the increasing stack value.
            answeredPrayers->ModStackAmount(1);

            // Note: if current stacks match max. stacks, trigger Apotheosis.
            if (answeredPrayers->GetStackAmount() != aurEff->GetAmount())
                return;

            answeredPrayers->Remove();

            if (Aura* apotheosis = GetTarget()->GetAura(SPELL_PRIEST_APOTHEOSIS))
            {
                apotheosis->SetDuration(apotheosis->GetDuration() + extraDuration.count());
                apotheosis->SetMaxDuration(apotheosis->GetMaxDuration() + extraDuration.count());
            }
            else
                target->CastSpell(target, SPELL_PRIEST_APOTHEOSIS,
                    CastSpellExtraArgs(TriggerCastFlags(TRIGGERED_IGNORE_CAST_IN_PROGRESS | TRIGGERED_IGNORE_SPELL_AND_CATEGORY_CD))
                    .SetTriggeringSpell(eventInfo.GetProcSpell())
                    .AddSpellMod(SPELLVALUE_DURATION, extraDuration.count()));
        }
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_pri_answered_prayers::HandleOnProc, EFFECT_0, SPELL_AURA_ADD_FLAT_MODIFIER_BY_SPELL_LABEL);
    }
};

void AddSC_DekkCore_priest_spell_scripts()
{
    RegisterAreaTriggerAI(at_pri_angelic_feather);
    RegisterAreaTriggerAI(at_pri_power_word_barrier);
    new spell_pri_psychic_scream();
    new spell_pri_smite_absorb();
    new spell_pri_focused_will();
    RegisterSpellScript(spell_pri_holy_word_sanctify);
    new spell_pri_purify();
    new spell_pri_fade();
    new spell_pri_glyph_of_shadow();
    new spell_pri_leap_of_faith();
    new spell_pri_mana_leech();
    new spell_pri_mind_sear();
    RegisterSpellScript(spell_pri_penance_triggered);
    RegisterSpellScript(spell_pri_penance_heal_damage);
    RegisterSpellScript(spell_pri_purge_the_wicked_selector);
    RegisterSpellScript(spell_pri_mind_sear_base);
    new spell_pri_phantasm();
    new spell_pri_prayer_of_mending_heal();
    new spell_pri_renew();
    RegisterSpellScript(spell_pri_shadow_word_death);
    new spell_pri_shadowform();
    new spell_pri_spirit_shell();
    new spell_pri_strength_of_soul();
    RegisterSpellScript(spell_pri_void_eruption);
    new spell_pri_void_shift();
    new spell_pri_voidform();
    RegisterSpellScript(spell_pri_shadowy_apparitions);
    RegisterSpellScript(spell_pri_spirit_of_redemption_form);
    RegisterSpellScript(spell_pri_holy_word_chastise);
    RegisterSpellScript(spell_pri_smite);
    RegisterSpellScript(spell_pri_heal_flash_heal);
    RegisterSpellScript(spell_pri_prayer_of_healing);
    RegisterSpellScript(spell_pri_binding_heal);
    RegisterSpellScript(spell_pri_void_bolt);
    RegisterSpellScript(spell_pri_archangel);
    RegisterSpellScript(spell_pri_clarity_of_will);
    RegisterSpellScript(aura_pri_void_torrent);
    RegisterSpellScript(spell_pri_dark_archangel);
    RegisterSpellScript(spell_pri_mind_blast);
    RegisterSpellScript(spell_pri_shadowy_insight);
    RegisterSpellScript(spell_pri_holy_nova);
    RegisterSpellScript(spell_pri_holy_word_salvation);
    RegisterSpellScript(spell_pri_holy_word_serenity);
    RegisterSpellScript(spell_priest_void_torrent);
    RegisterSpellScript(spell_pri_dark_void);
    new spell_protector_of_the_frail();
    new spell_indemnity();
    new spell_weal_and_woe();
    new spell_pri_power_word_life();
    new spell_pri_inspiration();
    RegisterSpellScript(spell_pri_angelic_feather_trigger);
    RegisterAreaTriggerAI(areatrigger_pri_angelic_feather);
    RegisterSpellScript(spell_pri_aq_3p_bonus);
    RegisterSpellScript(spell_pri_atonement);
    RegisterSpellScript(spell_pri_atonement_triggered);
    RegisterSpellScript(spell_pri_circle_of_healing);
    RegisterSpellScript(spell_pri_divine_hymn);
    RegisterSpellScript(spell_pri_divine_star_shadow);
    RegisterSpellScript(spell_pri_empowered_renew);
    RegisterSpellScript(spell_pri_guardian_spirit);
    RegisterSpellScript(spell_pri_halo_shadow);
    RegisterSpellScript(spell_pri_holy_words);
    RegisterSpellScript(spell_pri_item_t6_trinket);
    RegisterSpellScript(spell_pri_leap_of_faith_effect_trigger);
    RegisterSpellScript(spell_pri_levitate);
    RegisterSpellScript(spell_pri_mind_bomb);
    RegisterSpellScript(spell_pri_painful_punishment);
    RegisterSpellScriptWithArgs(spell_pri_penance, "spell_pri_penance", SPELL_PRIEST_PENANCE_CHANNEL_DAMAGE, SPELL_PRIEST_PENANCE_CHANNEL_HEALING);
    RegisterSpellScriptWithArgs(spell_pri_penance, "spell_pri_dark_reprimand", SPELL_PRIEST_DARK_REPRIMAND_CHANNEL_DAMAGE, SPELL_PRIEST_DARK_REPRIMAND_CHANNEL_HEALING);
    RegisterSpellScript(spell_pri_penance_or_dark_reprimand_channeled);
    RegisterSpellScript(spell_pri_power_of_the_dark_side);
    RegisterSpellScript(spell_pri_power_of_the_dark_side_damage_bonus);
    RegisterSpellScript(spell_pri_power_of_the_dark_side_healing_bonus);
    RegisterSpellScript(spell_pri_power_word_radiance);
    RegisterSpellAndAuraScriptPair(spell_pri_power_word_shield, spell_pri_power_word_shield_aura);
    RegisterSpellScript(spell_pri_power_word_solace);
    RegisterSpellScript(spell_pri_prayer_of_mending);
    RegisterSpellScript(spell_pri_prayer_of_mending_aura);
    RegisterSpellScript(spell_pri_prayer_of_mending_jump);
    RegisterSpellScript(spell_pri_purge_the_wicked);
    RegisterSpellScript(spell_pri_purge_the_wicked_dummy);
    RegisterSpellScript(spell_pri_rapture);
    RegisterSpellScript(spell_pri_sins_of_the_many);
    RegisterSpellScript(spell_pri_spirit_of_redemption);
    RegisterSpellScript(spell_pri_t3_4p_bonus);
    RegisterSpellScript(spell_pri_t5_heal_2p_bonus);
    RegisterSpellScript(spell_pri_t10_heal_2p_bonus);
    RegisterSpellScript(spell_pri_twist_of_fate);
    RegisterSpellScript(spell_pri_vampiric_embrace);
    RegisterSpellScript(spell_pri_vampiric_embrace_target);
    RegisterSpellScript(spell_pri_vampiric_touch);
    RegisterAreaTriggerAI(areatrigger_pri_divine_star);
    RegisterAreaTriggerAI(areatrigger_pri_halo);
    RegisterSpellScript(spell_pri_trail_of_light);
    RegisterSpellScript(spell_pri_answered_prayers);
}
