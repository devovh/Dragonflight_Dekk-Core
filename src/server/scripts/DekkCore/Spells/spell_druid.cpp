/*
 * This file is part of DekkCore Team Devs 
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
  * Scripts for spells with SPELLFAMILY_DRUID and SPELLFAMILY_GENERIC spells used by druid players.
  * Ordered alphabetically using scriptname.
  * Scriptnames of files in this file should be prefixed with "spell_dru_".
  */

#include "AreaTrigger.h"
#include "AreaTriggerAI.h"
#include "CellImpl.h"
#include "Containers.h"
#include "DynamicObject.h"
#include "PhasingHandler.h"
#include "Player.h"
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "SharedDefines.h"
#include "Spell.h"
#include "SpellAuraEffects.h"
#include "SpellHistory.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include "SpellScript.h"
#include "SpellPackets.h"
#include "Unit.h"
#include "GridNotifiers.h"

enum DruidSpells
{
    SPELL_DRUID_THRASH_BEAR_PERIODIC_DAMAGE = 192090,
    SPELL_DRUID_MAUL = 6807,
    SPELL_DRUID_BLESSING_OF_THE_ANCIENTS = 202360,
    SPELL_DRUID_BLESSING_OF_ELUNE = 202737,
    SPELL_DRUID_BLESSING_OF_ANSHE = 202739,
    SPELL_DRUID_STARLORD_DUMMY = 202345,
    SPELL_DRUID_STARLORD_SOLAR = 202416,
    SPELL_DRUID_STARLORD_LUNAR = 202423,
    SPELL_DRUID_GLYPH_OF_STARS = 114301,
    SPELL_DRUID_CHOSEN_OF_ELUNE = 102560,
    SPELL_DRUID_BLUE_COLOR = 108268,
    SPELL_DRUID_SHADOWY_GHOST = 165114,
    SPELL_DRUID_GORE = 210706,
    SPELL_DRUID_YSERA_GIFT = 145108,
    SPELL_DRUID_YSERAS_GIFT_HEAL_SELF = 145109,
    SPELL_DRUID_YSERAS_GIFT_HEAL_PARTY = 145110,
    SPELL_DRUID_REJUVENATION = 774,
    SPELL_DRUID_HEALING_TOUCH = 5185,
    SPELL_DRUID_SWIFTMEND = 18562,
    SPELL_DRUID_TRAVEL_FORM = 783,
    SPELL_DRUID_FELINE_SWIFTNESS = 131768,
    SPELL_DRUID_SHRED = 5221,
    SPELL_DRUID_RAKE = 1822,
    SPELL_DRUID_RIP = 1079,
    SPELL_DRUID_FEROCIOUS_BITE = 22568,
    SPELL_DRUID_MOONFIRE_CAT = 155625,
    SPELL_DRUID_SWIPE_CAT = 106785,
    SPELL_DRUID_SABERTOOTH = 202031,
    SPELL_DRUID_FORMS_TRINKET_BEAR = 37340,
    SPELL_DRUID_FORMS_TRINKET_CAT = 37341,
    SPELL_DRUID_FORMS_TRINKET_MOONKIN = 37343,
    SPELL_DRUID_FORMS_TRINKET_NONE = 37344,
    SPELL_DRUID_FORMS_TRINKET_TREE = 37342,
    SPELL_DRUID_GLYPH_OF_INNERVATE = 54833,
    SPELL_DRUID_INCREASED_MOONFIRE_DURATION = 38414,
    SPELL_DRUID_GLYPH_OF_STARFIRE = 54846,
    SPELL_DRUID_ITEM_T8_BALANCE_RELIC = 64950,
    SPELL_DRUID_STAMPEDE = 81022,
    SPELL_DRUID_STAMPEDE_BAER_RANK_1 = 81016,
    SPELL_DRUID_STAMPEDE_CAT_RANK_1 = 81021,
    SPELL_DRUID_STAMPEDE_CAT_STATE = 109881,
    SPELL_DRUID_FERAL_CHARGE_BEAR = 16979,
    SPELL_DRUID_FERAL_CHARGE_CAT = 49376,
    SPELL_DRUID_GLYPH_OF_TYPHOON = 62135,
    SPELL_DRUID_EXHILARATE = 28742,
    SPELL_DRUID_INFUSION = 37238,
    SPELL_DRUID_BLESSING_OF_THE_CLAW = 28750,
    SPELL_DRUID_BLESSING_OF_REMULOS = 40445,
    SPELL_DRUID_BLESSING_OF_ELUNE_40446 = 40446,
    SPELL_DRUID_BLESSING_OF_CENARIUS = 40452,
    SPELL_DRUID_LANGUISH = 71023,
    SPELL_DRUID_REJUVENATION_T10_PROC = 70691,
    SPELL_DRUID_SHOOTING_STARS = 93400,
    SPELL_DRUID_SOLAR_ECLIPSE = 48517,
    SPELL_DRUID_SOLAR_ECLIPSE_MARKER = 67483, // Will make the yellow arrow on eclipse bar point to the yellow side (solar)
    SPELL_DRUID_LUNAR_ECLIPSE = 48518,
    SPELL_DRUID_LUNAR_ECLIPSE_MARKER = 67484, // Will make the yellow arrow on eclipse bar point to the blue side (lunar)
    SPELL_DRUID_WRATH = 5176,
    SPELL_DRUID_EUPHORIA = 152222,
    SPELL_DRUID_STARFIRE = 2912,
    SPELL_DRUID_SOLAR_BEAM = 78675,
    SPELL_DRUID_SOLAR_BEAM_SILENCE = 81261,
    SPELL_DRU_REGROWTH = 8936,
    SPELL_DRU_NATURES_SWIFTNESS = 132158,
    SPELL_DRU_WILD_GROWTH = 48438,
    SPELL_DRU_REJUVINATION = 774,
    SPELL_DRU_BLOODTALONS = 155672,
    SPELL_DRU_BLOODTALONS_TRIGGERED = 145152,
    SPELL_DRU_MOMENT_OF_CLARITY = 236068,
    SPELL_DRU_CLEARCASTING = 16870,
    SPELL_DRUID_OMEN_OF_CLARITY = 113043,
    SPELL_ZANDALARI_TROLL_DRUID_SABERTUSK_CAT_SHAPESHIFT = 287362,
    SPELL_ZANDALARI_PTERRODAX_FLY_SHAPESHIFT = 265524,
    SPELL_ZANDALARI_TRAVEL_FORM_RAPTOR_SHAPESHIFT = 271899,
    SPELL_ZANDALARI_BEAR_FORM_ANKYLODON_SHAPESHIFT = 262551,
    MODEL_ZANDALARI_BEAR_FORM = 84869,
    //SPELL_ZANDALARI_TROLL_AQUATIC_FORM                    = ?
    SPELL_FERAL_FRENZY_MAIN = 274837,
    SPELL_FERAL_FRENZY_BLEED = 274838,
    SPELL_DRU_ECLIPSE = 279619,
    SPELL_DRU_SOLAR_EMPOWEREMENT = 164545,
    SPELL_DRU_LUNAR_EMPOWEREMENT = 164547,
    SPELL_DRU_STARLORD = 203245,
    SPELL_DRU_STARLORD_BUFF = 279709,
    SPELL_DRU_ABUNDANCE = 207383,
    SPELL_DRU_ABUNDANCE_BUFF = 207640,
    SPELL_DRU_SOUL_OF_THE_FOREST_RESTO = 158478,
    SPELL_DRU_SOUL_OF_THE_FOREST_RESTO_BUFF = 114108,
    SPELL_DRU_CULTIVATION = 200390,
    SPELL_DRU_CULTIVATION_HEAL = 200389,
    SPELL_DRU_GERMINATION = 155675,
    SPELL_DRU_GERMINATION_HOT = 155777,
    SPELL_DRU_GLYPH_OF_REJUVENATION = 17076,
    SPELL_DRU_GLYPH_OF_REJUVENATION_EFFECT = 96206,
    SPELL_DRU_SPRING_BLOSSOMS = 207385,
    SPELL_DRU_SPRING_BLOSSOMS_HEAL = 207386,
    SPELL_DRU_PHOTOSYNTHESIS = 274902,
    SPELL_DRU_PHOTOSYNTHESIS_MOD_HEAL_TICKS = 274906,
    SPELL_DRU_LUNAR_BEAM_DAMAGE_HEAL = 204069,
    SPELL_DRU_URSOL_VORTEX_PULL = 118283,
    SPELL_DRU_URSOL_VORTEX_DEBUFF = 127797,
    SPELL_DRU_MASS_ENTANGLEMENT = 102359,
    SPELL_DRU_GALACTIC_GUARDIAN = 203964,
    SPELL_DRU_GALACTIC_GAURDIAN_MOD_MOONFIRE = 213708,
    SPELL_DRU_PREDATOR = 202021,
    SPELL_DRU_TIGER_FURY = 5217,
    SPELL_DRUID_BALANCE_T10_BONUS = 70718,
    SPELL_DRUID_BALANCE_T10_BONUS_PROC = 70721,

    SPELL_DRUID_BLOOD_FRENZY_AURA = 203962,
    SPELL_DRUID_BLOOD_FRENZY_RAGE_GAIN = 203961,
    SPELL_DRUID_BRAMBLES_DAMAGE_AURA = 213709,
    SPELL_DRUID_BRAMBLES_PASSIVE = 203953,
    SPELL_DRUID_BRAMBLES_REFLECT = 203958,
    SPELL_DRUID_BRISTLING_FUR_GAIN_RAGE = 204031,
  
    SPELL_DRUID_CURIOUS_BRAMBLEPATCH = 330670,
    SPELL_DRUID_DASH = 1850,
    SPELL_DRUID_EARTHWARDEN_AURA = 203975,
    SPELL_DRUID_ECLIPSE_DUMMY = 79577,
    SPELL_DRUID_ECLIPSE_LUNAR_AURA = 48518,
    SPELL_DRUID_ECLIPSE_LUNAR_SPELL_CNT = 326055,
    SPELL_DRUID_ECLIPSE_OOC = 329910,
    SPELL_DRUID_ECLIPSE_SOLAR_AURA = 48517,
    SPELL_DRUID_ECLIPSE_SOLAR_SPELL_CNT = 326053,
    SPELL_DRUID_ENTANGLING_ROOTS = 339,
    SPELL_DRUID_FORM_AQUATIC_PASSIVE = 276012,
    SPELL_DRUID_FORM_AQUATIC = 1066,
    SPELL_DRUID_FORM_FLIGHT = 33943,
    SPELL_DRUID_FORM_STAG = 165961,
    SPELL_DRUID_FORM_SWIFT_FLIGHT = 40120,
    SPELL_DRUID_GALACTIC_GUARDIAN_AURA = 213708,
    SPELL_DRUID_GLYPH_OF_STARS_VISUAL = 114302,
    SPELL_DRUID_GORE_PROC = 93622,
    SPELL_DRUID_GROWL = 6795,
    SPELL_DRUID_IDOL_OF_FERAL_SHADOWS = 34241,
    SPELL_DRUID_IDOL_OF_WORSHIP = 60774,
    SPELL_DRUID_INCARNATION_KING_OF_THE_JUNGLE = 102543,
    SPELL_DRUID_INNERVATE = 29166,
    SPELL_DRUID_INNERVATE_RANK_2 = 326228,
    SPELL_DRUID_LIFEBLOOM_FINAL_HEAL = 33778,
    SPELL_DRUID_LUNAR_INSPIRATION_OVERRIDE = 155627,
    SPELL_DRUID_MANGLE = 33917,
    SPELL_DRUID_MASS_ENTANGLEMENT = 102359,
    SPELL_DRUID_MOONFIRE_DAMAGE = 164812,
    SPELL_DRUID_PROWL = 5215,
 
    SPELL_DRUID_RESTORATION_T10_2P_BONUS = 70658,
    SPELL_DRUID_SAVAGE_ROAR = 62071,
    SPELL_DRUID_SKULL_BASH_CHARGE = 221514,
    SPELL_DRUID_SKULL_BASH_INTERRUPT = 93985,
    SPELL_DRUID_SUNFIRE_DAMAGE = 164815,
    SPELL_DRUID_SURVIVAL_INSTINCTS = 50322,
    SPELL_DRUID_SHOOTING_STARS_DAMAGE = 202497,

    SPELL_DRUID_THRASH_BEAR = 77758,
    SPELL_DRUID_THRASH_CAT = 106830,
    SPELL_DRUID_THRASH_BEAR_AURA = 192090,
    SPELL_DRUID_GROVE_TRENDING_TRIGGER = 383192,
    SPELL_DRUID_GROVE_TRENDING = 383193,

    SPELL_DRU_NURTURING_DORMANCY = 392099
};

enum ShapeshiftFormSpells
{
    SPELL_DRUID_BEAR_FORM = 5487,
    SPELL_DRUID_CAT_FORM = 768,
    SPELL_DRUID_MOONKIN_FORM = 24858,
    SPELL_DRUID_MOONKIN_FORM_TALENT = 197625,
    SPELL_DRUID_INCARNATION_TREE_OF_LIFE = 33891,
    SPELL_DRUID_INCARNATION_KING_OF_JUNGLE = 102543
};

enum SoulOfTheForestSpells
{
    SPELL_DRUID_SOUL_OF_THE_FOREST_RESTO_TALENT = 158478,
    SPELL_DRUID_SOUL_OF_THE_FOREST_RESTO = 114108
};

enum EfflorescenceSpells
{
    SPELL_DRUID_EFFLORESCENCE_HEAL = 81269,
    SPELL_DRUID_EFFLORESCENCE_DUMMY = 81262
};

enum GoreSpells
{
    SPELL_DRUID_THRASH = 106832,
    SPELL_DRUID_MOONFIRE = 8921,
    SPELL_DRUID_SWIPE = 213764
};

enum DruidFormSounds
{
    SOUND_STAG_FORM = 2651,
    SOUND_CAT_FORM = 4121,
    SOUND_AQUATIC_FORM = 6574,
    SOUND_DIRE_BEAR_FORM = 5735,
    SOUND_BEAR_FORM = 5735,
    SOUND_FLIGHT_FORM_EPIC = 10893,
    SOUND_FLIGHT_FORM = 10893,
    SOUND_MOONKIN_FORM_RESTORATION = 11748,
    SOUND_MOONKIN_FORM = 11748
};

// Maul (Bear Form) - 6807
class spell_dru_maul_bear : public SpellScript
{
    PrepareSpellScript(spell_dru_maul_bear);

    void OnHit(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        Unit* target = GetHitUnit();
        if (!caster || !target)
            return;
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_dru_maul_bear::OnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnEffectHitTarget += SpellEffectFn(spell_dru_maul_bear::OnHit, EFFECT_1, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// Blessing of the Ancients - 202360
class spell_dru_blessing_of_the_ancients : public SpellScript
{
    PrepareSpellScript(spell_dru_blessing_of_the_ancients);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        uint32 removeAura = GetCaster()->HasAura(SPELL_DRUID_BLESSING_OF_ELUNE) ? SPELL_DRUID_BLESSING_OF_ELUNE : SPELL_DRUID_BLESSING_OF_ANSHE;
        uint32 addAura = GetCaster()->HasAura(SPELL_DRUID_BLESSING_OF_ELUNE) ? SPELL_DRUID_BLESSING_OF_ANSHE : SPELL_DRUID_BLESSING_OF_ELUNE;

        GetCaster()->RemoveAurasDueToSpell(removeAura);
        GetCaster()->CastSpell(nullptr, addAura, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_dru_blessing_of_the_ancients::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

//7.3.2.25549 END

// Efflorescence - 145205
class spell_dru_efflorescence : public SpellScript
{
    PrepareSpellScript(spell_dru_efflorescence);

    enum eCreature
    {
        NPC_EFFLORESCENCE = 47649
    };

    void HandleSummon()
    {
        if (Unit* caster = GetCaster())
            if (Creature* efflorescence = caster->GetSummonedCreatureByEntry(NPC_EFFLORESCENCE))
                efflorescence->DespawnOrUnsummon();
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_dru_efflorescence::HandleSummon);
    }
};

// Efflorescence (Aura) - 81262
class spell_dru_efflorescence_aura : public AuraScript
{
    PrepareAuraScript(spell_dru_efflorescence_aura);

    void HandleHeal(AuraEffect const* /*aurEff*/)
    {
        if (GetCaster() && GetCaster()->GetOwner())
        {
            GetCaster()->GetOwner()->CastSpell(GetCaster()->GetPosition(), SPELL_DRUID_EFFLORESCENCE_HEAL);
            std::list<Player*> playerList;
            GetCaster()->GetPlayerListInGrid(playerList, 11.2f);
            for (auto& targets : playerList)
            {
                if (GetCaster()->GetOwner()->HasAura(SPELL_DRU_SPRING_BLOSSOMS))
                    if (!targets->HasAura(SPELL_DRU_SPRING_BLOSSOMS_HEAL))
                        GetCaster()->GetOwner()->CastSpell(targets, SPELL_DRU_SPRING_BLOSSOMS_HEAL, true);
            }
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_dru_efflorescence_aura::HandleHeal, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// Efflorescence (Heal) - 81269
class spell_dru_efflorescence_heal : public SpellScript
{
    PrepareSpellScript(spell_dru_efflorescence_heal);

    void SortTargets(std::list<WorldObject*>& targets)
    {
        targets.sort(Trinity::HealthPctOrderPred());

        if (targets.size() > 3)
            targets.resize(3);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dru_efflorescence_heal::SortTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
    }
};

// 159286 Primal Fury
// @Version : 7.1.0.22908
class spell_dru_primal_fury : public SpellScriptLoader
{
public:
    spell_dru_primal_fury() : SpellScriptLoader("spell_dru_primal_fury") { }

    class spell_dru_primal_fury_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_dru_primal_fury_AuraScript);

        bool CheckProc(ProcEventInfo& eventInfo)
        {
            bool _spellCanProc = (eventInfo.GetSpellInfo()->Id == SPELL_DRUID_SHRED || eventInfo.GetSpellInfo()->Id == SPELL_DRUID_RAKE || eventInfo.GetSpellInfo()->Id == SPELL_DRUID_SWIPE_CAT || eventInfo.GetSpellInfo()->Id == SPELL_DRUID_MOONFIRE_CAT);

            if ((eventInfo.GetHitMask() & PROC_HIT_CRITICAL) && _spellCanProc)
                return true;

            return false;
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_dru_primal_fury_AuraScript::CheckProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_dru_primal_fury_AuraScript();
    }
};

enum PredatorySwiftnessSpells
{
    SPELL_DRUID_PREDATORY_SWIFTNESS = 16974,
    SPELL_DRUID_PREDATORY_SWIFTNESS_AURA = 69369
};

// Predatory Swiftness - 16974
// @Called : Maim - 22570, Ferocious Bite - 22568, Rip - 1079, Savage Roar - 1079
// @Version : 7.1.0.22908
class spell_dru_predatory_swiftness : public SpellScriptLoader
{
public:
    spell_dru_predatory_swiftness() : SpellScriptLoader("spell_dru_predatory_swiftness") { }

    class spell_dru_predatory_swiftness_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_dru_predatory_swiftness_SpellScript);

        uint8 _cp;

        bool Load() override
        {
            _cp = GetCaster()->GetPower(POWER_COMBO_POINTS);
            return true;
        }

        SpellCastResult CheckCast()
        {
            if (GetCaster())
            {
                if (GetCaster()->GetTypeId() != TYPEID_PLAYER)
                    return SPELL_FAILED_DONT_REPORT;

                if (!GetCaster()->ToPlayer()->GetPower(POWER_COMBO_POINTS))
                    return SPELL_FAILED_NO_COMBO_POINTS;
            }
            else
                return SPELL_FAILED_DONT_REPORT;

            return SPELL_CAST_OK;
        }

        void HandleOnHit()
        {
            if (Player* player = GetCaster()->ToPlayer())
                if (player->HasAura(SPELL_DRUID_PREDATORY_SWIFTNESS) && roll_chance_i(20 * _cp))
                    player->CastSpell(player, SPELL_DRUID_PREDATORY_SWIFTNESS_AURA, true);
        }

        void Register() override
        {
            OnCheckCast += SpellCheckCastFn(spell_dru_predatory_swiftness_SpellScript::CheckCast);
            AfterHit += SpellHitFn(spell_dru_predatory_swiftness_SpellScript::HandleOnHit);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_dru_predatory_swiftness_SpellScript();
    }
};

// Predatory Swiftness (Aura) - 69369
// @Called : Entangling Roots - 339, Rebirth - 20484, Regrowth - 8936
// @Version : 7.1.0.22908
class spell_dru_predatory_swiftness_aura : public SpellScriptLoader
{
public:
    spell_dru_predatory_swiftness_aura() : SpellScriptLoader("spell_dru_predatory_swiftness_aura") { }

    class spell_dru_predatory_swiftness_aura_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_dru_predatory_swiftness_aura_SpellScript);

        void HandleAfterHit()
        {
            if (Player* player = GetCaster()->ToPlayer())
            {
                if (player->HasAura(SPELL_DRUID_PREDATORY_SWIFTNESS_AURA))
                    player->RemoveAurasDueToSpell(SPELL_DRUID_PREDATORY_SWIFTNESS_AURA);
            }
        }

        void Register() override
        {
            AfterHit += SpellHitFn(spell_dru_predatory_swiftness_aura_SpellScript::HandleAfterHit);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_dru_predatory_swiftness_aura_SpellScript();
    }
};

enum BalanceAffinitySpells
{
    SPELL_DRUID_STARSURGE = 78674,
    SPELL_DRUID_SUNFIRE = 93402,
    SPELL_DRUID_LUNAR_STRIKE = 194153,
    SPELL_DRUID_SOLAR_WRATH = 190984
};

// 197488 Balance Affinity (Feral, Guardian) -
// @Version : 7.1.0.22908
class spell_dru_balance_affinity_dps : public SpellScriptLoader
{
public:
    spell_dru_balance_affinity_dps() : SpellScriptLoader("spell_dru_balance_affinity_dps") {}

    class spell_dru_balance_affinity_dps_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_dru_balance_affinity_dps_AuraScript);

        void LearnSpells(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            Unit* caster = GetCaster();
            if (!caster)
                return;

            if (Player* player = caster->ToPlayer())
            {
                player->AddTemporarySpell(SPELL_DRUID_MOONKIN_FORM);
                player->AddTemporarySpell(SPELL_DRUID_STARSURGE);
                player->AddTemporarySpell(SPELL_DRUID_LUNAR_STRIKE);
                player->AddTemporarySpell(SPELL_DRUID_SOLAR_WRATH);
                player->AddTemporarySpell(SPELL_DRUID_SUNFIRE);
            }
        }

        void UnlearnSpells(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            Unit* caster = GetCaster();
            if (!caster)
                return;

            if (Player* player = caster->ToPlayer())
            {
                player->RemoveTemporarySpell(SPELL_DRUID_MOONKIN_FORM);
                player->RemoveTemporarySpell(SPELL_DRUID_STARSURGE);
                player->RemoveTemporarySpell(SPELL_DRUID_LUNAR_STRIKE);
                player->RemoveTemporarySpell(SPELL_DRUID_SOLAR_WRATH);
                player->RemoveTemporarySpell(SPELL_DRUID_SUNFIRE);
            }
        }

        void Register() override
        {
            OnEffectRemove += AuraEffectRemoveFn(spell_dru_balance_affinity_dps_AuraScript::UnlearnSpells, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            OnEffectApply += AuraEffectApplyFn(spell_dru_balance_affinity_dps_AuraScript::LearnSpells, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_dru_balance_affinity_dps_AuraScript();
    }
};

// 197632 Balance Affinity (Restoration) -
// @Version : 7.1.0.22908
class spell_dru_balance_affinity_resto : public SpellScriptLoader
{
public:
    spell_dru_balance_affinity_resto() : SpellScriptLoader("spell_dru_balance_affinity_resto") {}

    class spell_dru_balance_affinity_resto_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_dru_balance_affinity_resto_AuraScript);

        void LearnSpells(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            Unit* caster = GetCaster();
            if (!caster)
                return;

            if (Player* player = caster->ToPlayer())
            {
                player->AddTemporarySpell(SPELL_DRUID_MOONKIN_FORM);
                player->AddTemporarySpell(SPELL_DRUID_STARSURGE);
                player->AddTemporarySpell(SPELL_DRUID_LUNAR_STRIKE);
            }
        }

        void UnlearnSpells(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            Unit* caster = GetCaster();
            if (!caster)
                return;

            if (Player* player = caster->ToPlayer())
            {
                player->RemoveTemporarySpell(SPELL_DRUID_MOONKIN_FORM);
                player->RemoveTemporarySpell(SPELL_DRUID_STARSURGE);
                player->RemoveTemporarySpell(SPELL_DRUID_LUNAR_STRIKE);
            }
        }

        void Register() override
        {
            OnEffectRemove += AuraEffectRemoveFn(spell_dru_balance_affinity_resto_AuraScript::UnlearnSpells, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            OnEffectApply += AuraEffectApplyFn(spell_dru_balance_affinity_resto_AuraScript::LearnSpells, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_dru_balance_affinity_resto_AuraScript();
    }
};

// 102560 Incarnation : Chosen of Elune -
// @Version : 7.1.0.22908
class spell_dru_incarnation_chosen_of_elune : public SpellScriptLoader
{
public:
    spell_dru_incarnation_chosen_of_elune() : SpellScriptLoader("spell_dru_incarnation_chosen_of_elune") { }

    class spell_dru_incarnation_chosen_of_elune_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_dru_incarnation_chosen_of_elune_SpellScript);

        void OnActivate()
        {
            if (Player* player = GetCaster()->ToPlayer())
                if (!player->HasAura(SPELL_DRUID_MOONKIN_FORM))
                    player->CastSpell(player, SPELL_DRUID_MOONKIN_FORM, true);
        }

        void Register() override
        {
            OnCast += SpellCastFn(spell_dru_incarnation_chosen_of_elune_SpellScript::OnActivate);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_dru_incarnation_chosen_of_elune_SpellScript();
    }
};

// 102543 Incarnation : King of the Jungle -
// @Version : 7.1.0.22908
class spell_dru_incarnation_king_of_the_jungle : public SpellScriptLoader
{
public:
    spell_dru_incarnation_king_of_the_jungle() : SpellScriptLoader("spell_dru_incarnation_king_of_the_jungle") { }

    class spell_dru_incarnation_king_of_the_jungle_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_dru_incarnation_king_of_the_jungle_SpellScript);

        void OnActivate()
        {
            if (Player* player = GetCaster()->ToPlayer())
                if (!player->HasAura(SPELL_DRUID_CAT_FORM))
                    player->CastSpell(player, SPELL_DRUID_CAT_FORM, true);
        }

        void Register() override
        {
            OnCast += SpellCastFn(spell_dru_incarnation_king_of_the_jungle_SpellScript::OnActivate);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_dru_incarnation_king_of_the_jungle_SpellScript();
    }
};

// 102558 Incarnation : Guardian of Ursoc -
// @Version : 7.1.0.22908
class spell_dru_incarnation_guardian_of_ursoc : public SpellScriptLoader
{
public:
    spell_dru_incarnation_guardian_of_ursoc() : SpellScriptLoader("spell_dru_incarnation_guardian_of_ursoc") { }

    class spell_dru_incarnation_guardian_of_ursoc_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_dru_incarnation_guardian_of_ursoc_SpellScript);

        void OnActivate()
        {
            if (Player* player = GetCaster()->ToPlayer())
                if (!player->HasAura(SPELL_DRUID_BEAR_FORM))
                    player->CastSpell(player, SPELL_DRUID_BEAR_FORM, true);
        }

        void Register() override
        {
            OnCast += SpellCastFn(spell_dru_incarnation_guardian_of_ursoc_SpellScript::OnActivate);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_dru_incarnation_guardian_of_ursoc_SpellScript();
    }
};

// 102383 Wild Charge (Moonkin) -
// @Version : 7.1.0.22908
class spell_dru_wild_charge_moonkin : public SpellScriptLoader
{
public:
    spell_dru_wild_charge_moonkin() : SpellScriptLoader("spell_dru_wild_charge_moonkin") { }

    class spell_dru_wild_charge_moonkin_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_dru_wild_charge_moonkin_SpellScript);

        SpellCastResult CheckFight()
        {
            if (GetCaster())
            {
                if (!GetCaster()->IsInCombat())
                    return SPELL_FAILED_DONT_REPORT;
            }
            else
                return SPELL_FAILED_DONT_REPORT;

            return SPELL_CAST_OK;
        }

        void Register() override
        {
            OnCheckCast += SpellCheckCastFn(spell_dru_wild_charge_moonkin_SpellScript::CheckFight);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_dru_wild_charge_moonkin_SpellScript();
    }
};

enum CatFormSpells
{
    SPELL_DRUID_FORM_CAT_INCREASE_SPEED = 113636,
    SPELL_DRUID_CATFORM_OVERRIDE = 48629,
    SPELL_DRUID_LUNAR_INSPIRATION = 155580,
    SPELL_DRUID_MOONFIRE_CAT_OVERRIDE = 155627
};

enum DruidFlamesSpells
{
    SPELL_DRUID_DRUID_OF_THE_FLAMES = 99245,
    SPELL_DRUID_BURNING_ESSENCE = 138927,
    MODEL_DRUID_OF_THE_FLAMES = 38150
};

enum BearFormSpells
{
    SPELL_DRUID_BEARFORM_OVERRIDE = 106829,
    SPELL_DRUID_STAMPEDING_ROAR = 106898,
    SPELL_DRUID_STAMPEDING_ROAR_BEAR_OVERRIDE = 106899
};

// Bear form - 5487
// @Called : Bear Form (Thrash/Swipe) - 106829, Bear Form (Stampeding Roar) - 106899
// @Version : 7.1.0.22908
class spell_dru_bear_form : public SpellScriptLoader
{
public:
    spell_dru_bear_form() : SpellScriptLoader("spell_dru_bear_form") {}

    class spell_dru_bear_form_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_dru_bear_form_AuraScript);

        void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            Unit* caster = GetCaster();
            if (!caster)
                return;

            caster->CastSpell(caster, SPELL_DRUID_BEARFORM_OVERRIDE, true);

            caster->PlayDirectSound(5735); // bear sounds

            if (caster->HasSpell(SPELL_DRUID_STAMPEDING_ROAR))
                caster->CastSpell(caster, SPELL_DRUID_STAMPEDING_ROAR_BEAR_OVERRIDE, true);
        }

        void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            Unit* caster = GetCaster();
            if (!caster)
                return;

            caster->RemoveAurasDueToSpell(SPELL_DRUID_BEARFORM_OVERRIDE);

            if (caster->HasSpell(SPELL_DRUID_STAMPEDING_ROAR))
                caster->RemoveAurasDueToSpell(SPELL_DRUID_STAMPEDING_ROAR_BEAR_OVERRIDE);
        }

        void Register() override
        {
            OnEffectApply += AuraEffectApplyFn(spell_dru_bear_form_AuraScript::OnApply, EFFECT_0, SPELL_AURA_MOD_SHAPESHIFT, AURA_EFFECT_HANDLE_REAL);
            OnEffectRemove += AuraEffectRemoveFn(spell_dru_bear_form_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_MOD_SHAPESHIFT, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_dru_bear_form_AuraScript();
    }
};

enum SkullBashSpells
{
    SPELL_DRUID_SKULL_BASH_INTERUPT = 93985
};

// Activate Cat Form
// @Called : Dash - 1850, Prowl - 5215, Displacer Beast - 102280
// @Version : 7.1.0.22908
class spell_dru_activate_cat_form : public SpellScriptLoader
{
public:
    spell_dru_activate_cat_form() : SpellScriptLoader("spell_dru_activate_cat_form") { }

    class spell_dru_activate_cat_form_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_dru_activate_cat_form_SpellScript);

        void HandleOnHit()
        {
            Unit* caster = GetCaster();
            if (!caster)
                return;

            if (!caster->HasAura(SPELL_DRUID_CAT_FORM))
                caster->CastSpell(caster, SPELL_DRUID_CAT_FORM, true);
        }

        void Register() override
        {
            OnHit += SpellHitFn(spell_dru_activate_cat_form_SpellScript::HandleOnHit);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_dru_activate_cat_form_SpellScript();
    }
};

// Infected wound - 48484
// @Version : 7.1.0.22908
class spell_dru_infected_wound : public SpellScriptLoader
{
public:
    spell_dru_infected_wound() : SpellScriptLoader("spell_dru_infected_wound") { }

    class spell_dru_infected_wound_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_dru_infected_wound_AuraScript);

        bool CheckProc(ProcEventInfo& eventInfo)
        {
            if (eventInfo.GetSpellInfo()->Id == SPELL_DRUID_RAKE)
                return true;

            return false;
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_dru_infected_wound_AuraScript::CheckProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_dru_infected_wound_AuraScript();
    }
};

// 145108 - Ysera's Gift
class spell_dru_yseras_gift : public AuraScript
{
    PrepareAuraScript(spell_dru_yseras_gift);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo
        ({
            SPELL_DRUID_YSERAS_GIFT_HEAL_SELF,
            SPELL_DRUID_YSERAS_GIFT_HEAL_PARTY
            });
    }

    void HandleEffectPeriodic(AuraEffect const* aurEff)
    {
        int32 healAmount = int32(GetTarget()->CountPctFromMaxHealth(aurEff->GetAmount()));

        if (!GetTarget()->IsFullHealth())
            GetTarget()->CastSpell(GetTarget(), SPELL_DRUID_YSERAS_GIFT_HEAL_SELF, CastSpellExtraArgs(aurEff).AddSpellBP0(healAmount));
        else
            GetTarget()->CastSpell(GetTarget(), SPELL_DRUID_YSERAS_GIFT_HEAL_PARTY, CastSpellExtraArgs(aurEff).AddSpellBP0(healAmount));
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_dru_yseras_gift::HandleEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 145110 - Ysera's Gift (heal)
class spell_dru_yseras_gift_group_heal : public SpellScript
{
    PrepareSpellScript(spell_dru_yseras_gift_group_heal);

    void SelectTargets(std::list<WorldObject*>& targets)
    {
        Trinity::SelectRandomInjuredTargets(targets, 1, true);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dru_yseras_gift_group_heal::SelectTargets, EFFECT_0, TARGET_UNIT_CASTER_AREA_RAID);
    }
};

enum RakeSpells
{
    SPELL_DRUID_RAKE_STUN = 163505
};

// Rake - 1822
class spell_dru_rake : public SpellScript
{
    PrepareSpellScript(spell_dru_rake);

    bool Load() override
    {
        Unit* caster = GetCaster();
        if (caster->HasAuraType(SPELL_AURA_MOD_STEALTH))
            m_stealthed = true;

        return true;
    }

    void HandleOnHit(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        Unit* target = GetExplTargetUnit();
        if (!caster || !target)
            return;

        // While stealthed or have Incarnation: King of the Jungle aura, deal 100% increased damage
        if (m_stealthed || caster->HasAura(SPELL_DRUID_INCARNATION_KING_OF_JUNGLE))
            SetHitDamage(GetHitDamage() * 2);

        // Only stun if the caster was in stealth
        if (m_stealthed)
            caster->CastSpell(target, SPELL_DRUID_RAKE_STUN, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_dru_rake::HandleOnHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }

private:
    bool m_stealthed = false;
};

enum MaimSpells
{
    SPELL_DRUID_MAIM_STUN = 203123
};

// Maim - 22570
class spell_dru_maim : public SpellScript
{
    PrepareSpellScript(spell_dru_maim);

    void HandleTakePower(SpellPowerCost& powerCost)
    {
        if (powerCost.Power == POWER_COMBO_POINTS)
            _usedComboPoints = powerCost.Amount;
    }

    void AfterCast()
    {
        Unit* target = GetExplTargetUnit();
        if (!target)
            return;

        GetCaster()->CastSpell(target, SPELL_DRUID_MAIM_STUN, true);

        if (Aura* maimStun = target->GetAura(SPELL_DRUID_MAIM_STUN, GetCaster()->GetGUID()))
            maimStun->SetDuration(_usedComboPoints * 1000);
    }

    void Register() override
    {
        OnTakePower += SpellOnTakePowerFn(spell_dru_maim::HandleTakePower);
        AfterHit += SpellHitFn(spell_dru_maim::AfterCast);
    }

private:
    int32 _usedComboPoints = 0;
};

enum BloodTalonsSpells
{
    SPELL_BLOODTALONS = 155672,
    SPELL_BLOODTALONS_AURA = 145152
};

enum DruidForms
{
    DRUID_AQUATIC_FORM = 1066,
    DRUID_FLIGHT_FORM = 33943,
    DRUID_STAG_FORM = 165961,
    DRUID_SWIFT_FLIGHT_FORM = 40120
};

// 24858  Moonkin Form
// 102560 Chosen of Elune
// 197625
class aura_dru_astral_form : public AuraScript
{
    PrepareAuraScript(aura_dru_astral_form);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_GLYPH_OF_STARS });
    }

    void AfterApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* target = GetTarget();

        if (target->HasAura(SPELL_DRUID_MOONKIN_FORM))
            target->SetDisplayId(target->GetModelForForm(FORM_MOONKIN_FORM, SPELL_DRUID_MOONKIN_FORM));

        if (target->HasAura(SPELL_DRUID_MOONKIN_FORM_TALENT))
            target->SetDisplayId(target->GetModelForForm(FORM_MOONKIN_FORM, SPELL_DRUID_MOONKIN_FORM_TALENT));

        target->PlayDirectSound(11748); // moonkin sounds
        if (target->HasAura(SPELL_DRUID_GLYPH_OF_STARS))
        {
            target->SetDisplayId(target->GetNativeDisplayId());
            target->AddAura(SPELL_DRUID_BLUE_COLOR, target);
            target->AddAura(SPELL_DRUID_SHADOWY_GHOST, target);
            target->CastSpell(target, sSpellMgr->GetSpellInfo(SPELL_DRUID_GLYPH_OF_STARS, DIFFICULTY_NONE)->GetEffect(EFFECT_0).BasePoints, true);
        }
    }

    void AfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* target = GetTarget();

        target->SetDisplayId(target->GetNativeDisplayId());

        if (target->HasAura(SPELL_DRUID_MOONKIN_FORM) || target->HasAura(SPELL_DRUID_CHOSEN_OF_ELUNE))
            return;

        target->RemoveAura(sSpellMgr->GetSpellInfo(SPELL_DRUID_GLYPH_OF_STARS, DIFFICULTY_NONE)->GetEffect(EFFECT_0).BasePoints);
        target->RemoveAura(SPELL_DRUID_BLUE_COLOR);
        target->RemoveAura(SPELL_DRUID_SHADOWY_GHOST);
    }

    void Register() override
    {
        switch (m_scriptSpellId)
        {
        case 197625:
        case 24858:
            AfterEffectApply += AuraEffectApplyFn(aura_dru_astral_form::AfterApply, EFFECT_1, SPELL_AURA_MOD_SHAPESHIFT, AURA_EFFECT_HANDLE_REAL);
            AfterEffectRemove += AuraEffectRemoveFn(aura_dru_astral_form::AfterRemove, EFFECT_1, SPELL_AURA_MOD_SHAPESHIFT, AURA_EFFECT_HANDLE_REAL);
            break;
        case 102560:
            AfterEffectApply += AuraEffectApplyFn(aura_dru_astral_form::AfterApply, EFFECT_1, SPELL_AURA_ADD_PCT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
            AfterEffectRemove += AuraEffectRemoveFn(aura_dru_astral_form::AfterRemove, EFFECT_1, SPELL_AURA_ADD_PCT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
            break;
        }
    }
};

// 197492 - Restoration Affinity
class aura_dru_restoration_affinity : public AuraScript
{
    PrepareAuraScript(aura_dru_restoration_affinity);

    const std::vector<uint32> LearnedSpells =
    {
        SPELL_DRUID_YSERA_GIFT,
        SPELL_DRUID_REJUVENATION,
        SPELL_DRUID_HEALING_TOUCH,
        SPELL_DRUID_SWIFTMEND
    };

    void AfterApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Player* target = GetTarget()->ToPlayer())
            for (uint32 spellId : LearnedSpells)
                target->LearnSpell(spellId, false);
    }

    void AfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Player* target = GetTarget()->ToPlayer())
            for (uint32 spellId : LearnedSpells)
                target->RemoveSpell(spellId);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(aura_dru_restoration_affinity::AfterApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        AfterEffectRemove += AuraEffectRemoveFn(aura_dru_restoration_affinity::AfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 202157 - Feral Affinity
class aura_dru_feral_affinity : public AuraScript
{
    PrepareAuraScript(aura_dru_feral_affinity);

    const std::vector<uint32> LearnedSpells =
    {
        SPELL_DRUID_FELINE_SWIFTNESS,
        SPELL_DRUID_SHRED,
        SPELL_DRUID_RAKE,
        SPELL_DRUID_RIP,
        SPELL_DRUID_FEROCIOUS_BITE,
        SPELL_DRUID_SWIPE_CAT
    };

    void AfterApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Player* target = GetTarget()->ToPlayer())
            for (uint32 spellId : LearnedSpells)
                target->LearnSpell(spellId, false);
    }

    void AfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Player* target = GetTarget()->ToPlayer())
            for (uint32 spellId : LearnedSpells)
                target->RemoveSpell(spellId);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(aura_dru_feral_affinity::AfterApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        AfterEffectRemove += AuraEffectRemoveFn(aura_dru_feral_affinity::AfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 22842 - Frenzied Regeneration
class aura_dru_frenzied_regeneration : public AuraScript
{
    PrepareAuraScript(aura_dru_frenzied_regeneration);

    void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
    {
        if (Aura* frenzied = GetCaster()->GetAura(22842))
            frenzied->GetMaxDuration();
        uint64 healAmount = CalculatePct(GetCaster()->GetDamageOverLastSeconds(5), 50);
        uint64 minHealAmount = CalculatePct(GetCaster()->GetMaxHealth(), 5);
        healAmount = std::max(healAmount, minHealAmount);
        amount = (int32)healAmount;
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(aura_dru_frenzied_regeneration::CalculateAmount, EFFECT_0, SPELL_AURA_OBS_MOD_HEALTH);
    }
};

enum StarfallSpells
{
    SPELL_DRUID_STARFALL_DAMAGE = 191037,
    SPELL_DRUID_STELLAR_EMPOWERMENT = 197637
};

// Starfall - 191034
// AreaTriggerID - 9482
struct at_dru_starfall : AreaTriggerAI
{
    int32 timeInterval;

    at_dru_starfall(AreaTrigger* areatrigger) : AreaTriggerAI(areatrigger)
    {
        // How often should the action be executed
        areatrigger->SetPeriodicProcTimer(850);
    }

    void OnPeriodicProc() override
    {
        if (Unit* caster = at->GetCaster())
            for (ObjectGuid objguid : at->GetInsideUnits())
                if (Unit* unit = ObjectAccessor::GetUnit(*caster, objguid))
                    if (caster->IsValidAttackTarget(unit))
                        if (unit->IsInCombat())
                        {
                            caster->CastSpell(unit, SPELL_DRUID_STARFALL_DAMAGE, true);
                            caster->CastSpell(unit, SPELL_DRUID_STELLAR_EMPOWERMENT, true);
                        }
    }
};

enum UrsolsVortexSpells
{
    SPELL_DRUID_URSOLS_VORTEX_SLOW = 127797,
    SPELL_DRUID_URSOLS_VORTEX_PULL = 118283
};

// Efflorescence
// NPC Id - 47649
class npc_dru_efflorescence : public ScriptedAI
{
public:
    npc_dru_efflorescence(Creature* creature) : ScriptedAI(creature) {}

    void Reset() override
    {
        me->CastSpell(me, SPELL_DRUID_EFFLORESCENCE_DUMMY, true);
        me->SetUnitFlag(UnitFlags(UNIT_FLAG_NON_ATTACKABLE));
        me->SetUnitFlag(UnitFlags(UNIT_FLAG_UNINTERACTIBLE));
        me->SetUnitFlag(UnitFlags(UNIT_FLAG_REMOVE_CLIENT_CONTROL));
        me->SetReactState(REACT_PASSIVE);
    }
};

// Charm Woodland Creature - 127757
class aura_dru_charm_woodland_creature : public AuraScript
{
    PrepareAuraScript(aura_dru_charm_woodland_creature);

    void OnApply(const AuraEffect* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        // Make targeted creature follow the player - Using pet's default dist and angle
        //if (Unit* caster = GetCaster())
            //if (Unit* target = GetTarget())
                //target->GetMotionMaster()->MoveFollow(caster, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
    }

    void OnRemove(const AuraEffect* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        //if (Unit* target = GetTarget())
            //if (target->GetMotionMaster()->GetCurrentMovementGeneratorType() == FOLLOW_MOTION_TYPE)
                //target->GetMotionMaster()->MovementExpired(true); // reset movement
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(aura_dru_charm_woodland_creature::OnApply, EFFECT_0, SPELL_AURA_AOE_CHARM, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(aura_dru_charm_woodland_creature::OnRemove, EFFECT_0, SPELL_AURA_AOE_CHARM, AURA_EFFECT_HANDLE_REAL);
    }
};

// Swipe - 106785
class spell_dru_swipe : public SpellScript
{
    PrepareSpellScript(spell_dru_swipe);

    void HandleOnHit(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        Unit* target = GetHitUnit();
        if (!caster || !target)
            return;

        int32 damage = GetHitDamage();
        int32 casterLevel = caster->GetLevelForTarget(caster);

        // This prevent awarding multiple Combo Points when multiple targets hit with Swipe AoE
        if (m_awardComboPoint)
            // Awards the caster 1 Combo Point (get value from the spell data)
            caster->ModifyPower(POWER_COMBO_POINTS, sSpellMgr->GetSpellInfo(SPELL_DRUID_SWIPE_CAT, DIFFICULTY_NONE)->GetEffect(EFFECT_0).BasePoints);

        // If caster is level >= 44 and the target is bleeding, deals 20% increased damage (get value from the spell data)
        if ((casterLevel >= 44) && target->HasAuraState(AURA_STATE_BLEED))
            AddPct(damage, sSpellMgr->GetSpellInfo(SPELL_DRUID_SWIPE_CAT, DIFFICULTY_NONE)->GetEffect(EFFECT_1).BasePoints);

        SetHitDamage(damage);

        m_awardComboPoint = false;
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_dru_swipe::HandleOnHit, EFFECT_1, SPELL_EFFECT_DUMMY);
    }

private:
    bool m_awardComboPoint = true;
};

// Brutal Slash - 202028
class spell_dru_brutal_slash : public SpellScript
{
    PrepareSpellScript(spell_dru_brutal_slash);

    void HandleOnHit()
    {
        Unit* caster = GetCaster();
        Unit* target = GetHitUnit();
        if (!caster || !target)
            return;

        // This prevent awarding multiple Combo Points when multiple targets hit with Brutal Slash AoE
        if (m_awardComboPoint)
            // Awards the caster 1 Combo Point (get value from the spell data)
            caster->ModifyPower(POWER_COMBO_POINTS, sSpellMgr->GetSpellInfo(SPELL_DRUID_SWIPE_CAT, DIFFICULTY_NONE)->GetEffect(EFFECT_0).BasePoints);

        m_awardComboPoint = false;
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_dru_brutal_slash::HandleOnHit);
    }

private:
    bool m_awardComboPoint = true;
};

// Thrash (Cat Form) - 106830
class spell_dru_thrash_cat : public SpellScript
{
    PrepareSpellScript(spell_dru_thrash_cat);

    void HandleOnEffectHitTarget(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        Unit* target = GetHitUnit();
        if (!caster || !target)
            return;

        // This prevent awarding multiple Combo Points when multiple targets hit with Thrash AoE
        if (m_awardComboPoint)
            // Awards the caster 1 Combo Point
            caster->ModifyPower(POWER_COMBO_POINTS, 1);

        m_awardComboPoint = false;
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_dru_thrash_cat::HandleOnEffectHitTarget, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }

private:
    bool m_awardComboPoint = true;
};

// Shred - 5221
class spell_dru_shred : public SpellScript
{
    PrepareSpellScript(spell_dru_shred);

    bool Load() override
    {
        Unit* caster = GetCaster();

        if (caster->HasAuraType(SPELL_AURA_MOD_STEALTH))
            m_stealthed = true;

        if (caster->HasAura(SPELL_DRUID_INCARNATION_KING_OF_JUNGLE))
            m_incarnation = true;

        m_casterLevel = caster->GetLevelForTarget(caster);

        return true;
    }

    void HandleCritChance(Unit* /*victim*/, float& chance)
    {
        // If caster is level >= 56, While stealthed or have Incarnation: King of the Jungle aura,
        // Double the chance to critically strike
        if ((m_casterLevel >= 56) && (m_stealthed || m_incarnation))
            chance *= 2.0f;
    }

    void HandleOnHit()
    {
        Unit* caster = GetCaster();
        Unit* target = GetHitUnit();
        if (!caster || !target)
            return;

        int32 damage = GetHitDamage();

        caster->ModifyPower(POWER_COMBO_POINTS, 1);

        // If caster is level >= 56, While stealthed or have Incarnation: King of the Jungle aura,
        // deals 50% increased damage (get value from the spell data)
        if ((caster->HasAura(231057)) && (m_stealthed || m_incarnation))
            AddPct(damage, sSpellMgr->GetSpellInfo(SPELL_DRUID_SHRED, DIFFICULTY_NONE)->GetEffect(EFFECT_2).BasePoints);

        SetHitDamage(damage);
    }

    void Register() override
    {
        //OnCalcCritChance += SpellOnCalcCritChanceFn(spell_dru_shred::HandleCritChance);
        OnHit += SpellHitFn(spell_dru_shred::HandleOnHit);
    }

private:
    bool m_stealthed = false;
    bool m_incarnation = false;
    int32 m_casterLevel;
};

// 8936 - Regrowth
class spell_dru_regrowth : public SpellScript
{
    PrepareSpellScript(spell_dru_regrowth);

    void HandleTakePower(SpellPowerCost& powerCost)
    {
        if (GetCaster()->HasAura(SPELL_DRU_CLEARCASTING))
            powerCost.Amount = 0;
    }

    void HandleHealEffect(SpellEffIndex /*effIndex*/)
    {
        if (GetCaster()->HasAura(SPELL_DRU_BLOODTALONS))
            GetCaster()->AddAura(SPELL_DRU_BLOODTALONS_TRIGGERED, GetCaster());

        if (Aura* clearcasting = GetCaster()->GetAura(SPELL_DRU_CLEARCASTING))
            if (GetCaster()->HasAura(SPELL_DRU_MOMENT_OF_CLARITY))
            {
                int32 amount = clearcasting->GetEffect(EFFECT_0)->GetAmount();
                clearcasting->GetEffect(EFFECT_0)->SetAmount(amount - 1);
                if (amount == -102)
                    GetCaster()->RemoveAurasDueToSpell(SPELL_DRU_CLEARCASTING);
            }
            else
            {
                GetCaster()->RemoveAurasDueToSpell(SPELL_DRU_CLEARCASTING);
            }
    }

    void HandleCast()
    {
        if (GetCaster()->HasAura(SPELL_DRU_NATURES_SWIFTNESS))
            GetCaster()->RemoveAura(SPELL_DRU_NATURES_SWIFTNESS);

        if (GetCaster()->HasAura(SPELL_DRU_CLEARCASTING))
            GetCaster()->RemoveAura(SPELL_DRU_CLEARCASTING);
    }

    void Register() override
    {
        OnTakePower += SpellOnTakePowerFn(spell_dru_regrowth::HandleTakePower);
        OnEffectHitTarget += SpellEffectFn(spell_dru_regrowth::HandleHealEffect, EFFECT_0, SPELL_EFFECT_HEAL);
        OnCast += SpellCastFn(spell_dru_regrowth::HandleCast);
    }
};

// 20484 - Rebirth
class spell_dru_rebirth : public SpellScript
{
    PrepareSpellScript(spell_dru_rebirth);

    void HandleCast()
    {
        if (GetCaster()->HasAura(SPELL_DRU_NATURES_SWIFTNESS))
            GetCaster()->RemoveAura(SPELL_DRU_NATURES_SWIFTNESS);
    }

    void HandleTrigger()
    {
        if (Unit* target = GetHitUnit())
            // Blizz seems to just apply aura without bothering to cast
            GetCaster()->AddAura(GetSpellInfo()->ExcludeTargetAuraSpell, target);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_dru_rebirth::HandleCast);
        AfterHit += SpellHitFn(spell_dru_rebirth::HandleTrigger);
    }
};

// 197721 - Flourish
class spell_dru_flourish : public SpellScriptLoader
{
public:
    spell_dru_flourish() : SpellScriptLoader("spell_dru_flourish") { }


    class spell_dru_flourish_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_dru_flourish_SpellScript);

        void HandleHit(SpellEffIndex /*effIndex*/)
        {
            if (!GetCaster() || !GetHitUnit())
                return;

            std::list<AuraEffect*> const& auraEffects = GetHitUnit()->GetAuraEffectsByType(SPELL_AURA_PERIODIC_HEAL);

            for (AuraEffect* auraEffect : auraEffects)
                if (auraEffect->GetCasterGUID() == GetCaster()->GetGUID())
                    if (Aura* healAura = auraEffect->GetBase())
                        healAura->SetDuration(healAura->GetDuration() + GetEffectValue() * IN_MILLISECONDS);
        }

        void FilterTargets(std::list<WorldObject*>& targets)
        {
            std::list<WorldObject*> tempTargets;
            for (WorldObject* target : targets)
            {
                if (target->IsPlayer())
                    if (target->ToUnit()->HasAuraTypeWithCaster(SPELL_AURA_PERIODIC_HEAL, GetCaster()->GetGUID()))
                        tempTargets.push_back(target);
            }

            if (!tempTargets.empty())
            {
                targets.clear();
                for (WorldObject* target : tempTargets)
                    targets.push_back(target);

            }
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_dru_flourish_SpellScript::HandleHit, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dru_flourish_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_dru_flourish_SpellScript();
    }
};

// 18562 - swiftmend
class spell_dru_swiftmend : public SpellScriptLoader
{
public:
    spell_dru_swiftmend() : SpellScriptLoader("spell_dru_swiftmend") { }


    class spell_dru_swiftmend_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_dru_swiftmend_SpellScript);

        enum Spells
        {
            SPELL_DRUID_SOUL_OF_THE_FOREST = 158478,
            SPELL_DRUID_SOUL_OF_THE_FOREST_TRIGGERED = 114108,
            SPELL_DRUID_REFORESTATION = 392356
        };


        void HandleHit(SpellEffIndex /*effIndex*/)
        {
            if (Unit* caster = GetCaster())
            {
                if (caster->HasAura(SPELL_DRUID_SOUL_OF_THE_FOREST))
                {
                    caster->AddAura(SPELL_DRUID_SOUL_OF_THE_FOREST_TRIGGERED, caster);
                }

                if (caster->HasAura(SPELL_DRU_REJUVINATION))
                {
                    caster->RemoveAura(SPELL_DRU_REJUVINATION);
                }
                else if (caster->HasAura(SPELL_DRU_REGROWTH))
                {
                    caster->RemoveAura(SPELL_DRU_REGROWTH);
                }
                else if (caster->HasAura(SPELL_DRU_WILD_GROWTH))
                {
                    caster->RemoveAura(SPELL_DRU_REGROWTH);
                }

                if (caster->HasAura(SPELL_DRUID_REFORESTATION))
                {
                    AuraEffect* Reforestation = caster->GetAuraEffect(SPELL_DRUID_REFORESTATION, EFFECT_0);
                    if (Reforestation)
                    {
                        if (Reforestation->GetAmount() > 2 || Reforestation->GetAmount() < 0)
                            Reforestation->SetAmount(2);

                        if (Reforestation->GetAmount() <= 0)
                        {
                            caster->CastSpell(caster, SPELL_DRUID_INCARNATION_TREE_OF_LIFE, true);
                            Reforestation->SetAmount(3);
                            if (Aura* tree = caster->GetAura(SPELL_DRUID_INCARNATION_TREE_OF_LIFE))
                            {
                                tree->SetDuration(9000, true);
                            }
                        }
                        else
                        {
                            Reforestation->SetAmount(Reforestation->GetAmount() - 1);
                        }
                    }
                }
            }
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_dru_swiftmend_SpellScript::HandleHit, EFFECT_0, SPELL_EFFECT_HEAL);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_dru_swiftmend_SpellScript();
    }
};

// 102793 - Ursol's Vortex
class spell_dru_ursols_vortex : public SpellScriptLoader
{
public:
    spell_dru_ursols_vortex() : SpellScriptLoader("spell_dru_ursols_vortex") { }


    class spell_dru_ursols_vortex_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_dru_ursols_vortex_SpellScript);

        enum Spells
        {
            SPELL_DRUID_URSOLS_VORTEX_SLOW = 127797,
        };


        void HandleHit(SpellEffIndex /*effIndex*/)
        {
            if (Unit* caster = GetCaster())
                caster->AddAura(SPELL_DRUID_URSOLS_VORTEX_SLOW, GetHitUnit());
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_dru_ursols_vortex_SpellScript::HandleHit, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_dru_ursols_vortex_SpellScript();
    }
};

// 102351 - Cenarion Ward
class spell_dru_cenarion_ward : public SpellScriptLoader
{
public:
    spell_dru_cenarion_ward() : SpellScriptLoader("spell_dru_cenarion_ward") { }

    class spell_dru_cenarion_ward_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_dru_cenarion_ward_AuraScript);

        enum Spells
        {
            SPELL_DRUID_CENARION_WARD_TRIGGERED = 102352,
        };

        bool Validate(SpellInfo const* /*spellInfo*/) override
        {
            return ValidateSpellInfo({ SPELL_DRUID_CENARION_WARD_TRIGGERED });
        }

        void HandleProc(AuraEffect* aurEff, ProcEventInfo& eventInfo)
        {
            PreventDefaultAction();

            if (!GetCaster() || !eventInfo.GetActionTarget())
                return;

            GetCaster()->CastSpell(eventInfo.GetActionTarget(), SPELL_DRUID_CENARION_WARD_TRIGGERED, true);
        }

        void Register() override
        {
            OnEffectProc += AuraEffectProcFn(spell_dru_cenarion_ward_AuraScript::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_dru_cenarion_ward_AuraScript();
    }
};

// 102352 - Cenarion Ward
class spell_dru_cenarion_ward_hot : public SpellScriptLoader
{
public:
    spell_dru_cenarion_ward_hot() : SpellScriptLoader("spell_dru_cenarion_ward_hot") { }

    class spell_dru_cenarion_ward_hot_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_dru_cenarion_ward_hot_AuraScript);

        void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
        {
            if (!GetCaster())
                return;

            amount = CalculatePct(GetCaster()->SpellBaseHealingBonusDone(SPELL_SCHOOL_MASK_NATURE), 220) / 4;
        }

        void Register() override
        {
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dru_cenarion_ward_hot_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_PERIODIC_HEAL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_dru_cenarion_ward_hot_AuraScript();
    }
};

// 54845 - Glyph of Starfire
class spell_dru_glyph_of_starfire_proc : public SpellScriptLoader
{
public:
    spell_dru_glyph_of_starfire_proc() : SpellScriptLoader("spell_dru_glyph_of_starfire_proc") { }

    class spell_dru_glyph_of_starfire_proc_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_dru_glyph_of_starfire_proc_AuraScript);

        void HandleEffectProc(AuraEffect* aurEff, ProcEventInfo& eventInfo)
        {
            PreventDefaultAction();
            GetTarget()->CastSpell(eventInfo.GetProcTarget(), SPELL_DRUID_GLYPH_OF_STARFIRE, aurEff);
        }

        void Register() override
        {
            OnEffectProc += AuraEffectProcFn(spell_dru_glyph_of_starfire_proc_AuraScript::HandleEffectProc, EFFECT_0, SPELL_AURA_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_dru_glyph_of_starfire_proc_AuraScript();
    }
};

// 61391 - Typhoon
class spell_dru_typhoon : public SpellScriptLoader
{
public:
    spell_dru_typhoon() : SpellScriptLoader("spell_dru_typhoon") { }

    class spell_dru_typhoon_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_dru_typhoon_SpellScript);

        void HandleKnockBack(SpellEffIndex effIndex)
        {
            // Glyph of Typhoon
            if (GetCaster()->HasAura(SPELL_DRUID_GLYPH_OF_TYPHOON))
                PreventHitDefaultEffect(effIndex);
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_dru_typhoon_SpellScript::HandleKnockBack, EFFECT_0, SPELL_EFFECT_KNOCK_BACK);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_dru_typhoon_SpellScript();
    }
};

// 202808 - Primal Vitality
class spell_dru_primal_vitality : public SpellScriptLoader
{
public:
    spell_dru_primal_vitality() : SpellScriptLoader("spell_dru_primal_vitality") { }

    class spell_dru_primal_vitality_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_dru_primal_vitality_AuraScript);

        enum
        {
            SPELL_DRUID_PRIMAL_VITALITY_PASSIVE = 202808,
            SPELL_DRUID_PRIMAL_VITALITY_EFFECT = 202812,
            SPELL_DRUID_PROWL = 5215
        };

        bool CheckProc(ProcEventInfo& eventInfo)
        {
            if (!eventInfo.GetSpellInfo())
                return false;

            if (!eventInfo.GetDamageInfo())
                return false;

            if (eventInfo.GetSpellInfo()->Id != SPELL_DRUID_PROWL)
                return false;

            return true;
        }

        void HandleEffectProc(AuraEffect* aurEff, ProcEventInfo& eventInfo)
        {
            PreventDefaultAction();

            if (Unit* target = eventInfo.GetProcTarget())
            {
                if (!target->HasAura(SPELL_DRUID_PRIMAL_VITALITY_EFFECT))
                    target->AddAura(SPELL_DRUID_PRIMAL_VITALITY_EFFECT, target);

            }
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_dru_primal_vitality_AuraScript::CheckProc);
            OnEffectProc += AuraEffectProcFn(spell_dru_primal_vitality_AuraScript::HandleEffectProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_dru_primal_vitality_AuraScript();
    }
};

// Nourish - 203374 <- wrong spell id + needs a rework as far as I can see, but I have no single clue about druids in this exp -- Damieninabox
class spell_dru_nourish : public SpellScriptLoader
{
public:
    spell_dru_nourish() : SpellScriptLoader("spell_dru_nourish") { }

private:
    class spell_dru_nourish_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_dru_nourish_SpellScript);

        enum
        {
            SPELL_DRUID_NOURISH_PASSIVE = 203374,
            SPELL_DRUID_REJUVENATION = 774
        };

        void HandleOnHit()
        {
            if (Unit* caster = GetCaster())
            {
                if (Unit* target = GetHitUnit())
                {
                    if (caster->HasAura(SPELL_DRUID_NOURISH_PASSIVE))
                        caster->CastSpell(target, SPELL_DRUID_REJUVENATION, true);
                }
            }
        }

        void Register() override
        {
            OnHit += SpellHitFn(spell_dru_nourish_SpellScript::HandleOnHit);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_dru_nourish_SpellScript();
    }
};

// Called by Ironfur - 192081
// Den Mother - 201522
class spell_dru_denmother : public SpellScriptLoader
{
public:
    spell_dru_denmother() : SpellScriptLoader("spell_dru_denmother") { }

private:
    class spell_dru_denmother_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_dru_denmother_SpellScript);

        enum
        {
            SPELL_DRUID_DEN_MOTHER = 201522,
            SPELL_DRUID_DEN_MOTHER_IRONFUR = 201629,
        };

        void HandleOnHit()
        {
            if (Player* _player = GetCaster()->ToPlayer())
            {
                if (_player->HasAura(SPELL_DRUID_DEN_MOTHER))
                {
                    std::list<Unit*> validTargets;
                    std::list<Unit*> groupList;

                    _player->GetPartyMembers(groupList);

                    if (groupList.empty())
                        return;

                    for (auto itr : groupList)
                    {
                        if ((itr->GetGUID() != _player->GetGUID()) && (itr->IsInRange(_player, 0, 50, true)))
                            validTargets.push_back(itr->ToUnit());
                    }

                    if (validTargets.empty())
                        return;

                    validTargets.sort(Trinity::HealthPctOrderPred());
                    auto const lowTarget = validTargets.front();

                    _player->CastSpell(lowTarget, 201629, true);
                }
            }
        }

        void Register() override
        {
            OnHit += SpellHitFn(spell_dru_denmother_SpellScript::HandleOnHit);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_dru_denmother_SpellScript();
    }
};

// Overgrowth - 203651
class spell_dru_overgrowth : public SpellScriptLoader
{
public:
    spell_dru_overgrowth() : SpellScriptLoader("spell_dru_overgrowth") { }

    class spell_dru_overgrowth_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_dru_overgrowth_SpellScript);

        enum
        {
            SPELL_DRUID_REJUVENATION = 774,
            SPELL_DRUID_WILD_GROWTH = 48438,
            SPELL_DRUID_LIFE_BLOOM = 33763,
            SPELL_DRUID_REGROWTH = 8936
        };

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            if (Unit* caster = GetCaster())
            {
                if (Unit* target = GetHitUnit())
                {
                    caster->AddAura(SPELL_DRUID_REJUVENATION, target);
                    caster->AddAura(SPELL_DRUID_WILD_GROWTH, target);
                    caster->AddAura(SPELL_DRUID_LIFE_BLOOM, target);
                    caster->AddAura(SPELL_DRUID_REGROWTH, target);
                }
            }
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_dru_overgrowth_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_dru_overgrowth_SpellScript();
    }
};

// 221514 - Skull Bash Charge
class spell_dru_skull_bash_charge : public SpellScriptLoader
{
public:
    spell_dru_skull_bash_charge() : SpellScriptLoader("spell_dru_skull_bash_charge") { }

    class spell_dru_skull_bash_charge_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_dru_skull_bash_charge_SpellScript);

        void HandleCharge(SpellEffIndex effIndex)
        {
            if (!GetCaster())
                return;

            if (!GetHitUnit())
                return;

            GetCaster()->CastSpell(GetHitUnit(), 93985, true);
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_dru_skull_bash_charge_SpellScript::HandleCharge, EFFECT_0, SPELL_EFFECT_CHARGE);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_dru_skull_bash_charge_SpellScript();
    }
};

// 157982 - Tranquility Heal
class spell_dru_tranquility_heal : public SpellScriptLoader
{
public:
    spell_dru_tranquility_heal() : SpellScriptLoader("spell_dru_tranquility_heal") { }

    class spell_dru_tranquility_heal_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_dru_tranquility_heal_SpellScript);

        void HandleHeal(SpellEffIndex /*effIndex*/)
        {
            if (!GetCaster())
                return;

            if (Unit* caster = GetCaster())
            {
                uint32 heal = CalculatePct(caster->SpellBaseHealingBonusDone(SPELL_SCHOOL_MASK_NATURE), 180);
                SetHitHeal(heal);
            }
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_dru_tranquility_heal_SpellScript::HandleHeal, EFFECT_0, SPELL_EFFECT_HEAL);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_dru_tranquility_heal_SpellScript();
    }
};

// 200389 - Cultivation
class spell_dru_cultivation : public SpellScriptLoader
{
public:
    spell_dru_cultivation() : SpellScriptLoader("spell_dru_cultivation") { }

    class spell_dru_cultivation_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_dru_cultivation_AuraScript);

        void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
        {
            if (!GetCaster())
                return;

            amount = CalculatePct(GetCaster()->SpellBaseHealingBonusDone(SPELL_SCHOOL_MASK_NATURE), 60);
        }

        void Register() override
        {
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dru_cultivation_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_PERIODIC_HEAL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_dru_cultivation_AuraScript();
    }
};

// 774 - Rejuvenation
class spell_dru_rejuvenation : public SpellScriptLoader
{
public:
    spell_dru_rejuvenation() : SpellScriptLoader("spell_dru_rejuvenation") { }

    enum Spells
    {
        SPELL_DRUID_CULTIVATION = 200390,
        SPELL_DRUID_CULTIVATION_HOT = 200389,
        SPELL_DRUID_GERMINATION = 155675,
        SPELL_DRUID_GERMINATION_HOT = 155777,
        SPELL_DRUID_ABUNDANCE = 207383,
        SPELL_DRUID_ABUNDANCE_BUFF = 207640,
        SPELL_DRUID_LUXURIANT_SOIL = 392315
    };

    class spell_dru_rejuvenation_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_dru_rejuvenation_AuraScript);

        //Posible Fixed


      // bool Validate(SpellInfo const* /*spellInfo*/) override
      // {
      //     return ValidateSpellInfo(
      //         {
      //             SPELL_DRUID_CULTIVATION,
      //             SPELL_DRUID_CULTIVATION_HOT,
      //             SPELL_DRUID_ABUNDANCE,
      //             SPELL_DRUID_ABUNDANCE_BUFF,
      //         });
      // }
      //
      // void AfterRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
      // {
      //     if (Unit* caster = GetCaster())
      //         if (caster->HasAura(SPELL_DRUID_ABUNDANCE))
      //             if (Aura* abundanceBuff = caster->GetAura(SPELL_DRUID_ABUNDANCE_BUFF))
      //                 abundanceBuff->ModStackAmount(-1);
      // }
      //
        void OnPeriodic(AuraEffect const* aurEff)
        {
            if (Unit* target = GetTarget())
            {
                if (GetCaster()->HasAura(SPELL_DRUID_LUXURIANT_SOIL))
                {
                    uint8 LuxuriantSoilRank = GetCaster()->ToPlayer()->GetTalentRankFromSpellID(SPELL_DRUID_LUXURIANT_SOIL);
                    if (roll_chance_f(LuxuriantSoilRank * 1.5))
                    {
                        Player* NearestPlayer = GetCaster()->SelectNearestPlayer(10.f);
                        GetCaster()->CastSpell(NearestPlayer, SPELL_DRUID_REJUVENATION, true);
                    }
                }
                if (GetCaster()->HasAura(SPELL_DRUID_CULTIVATION) && !target->HasAura(SPELL_DRUID_CULTIVATION_HOT) && target->HealthBelowPct(sSpellMgr->GetSpellInfo(SPELL_DRUID_CULTIVATION, DIFFICULTY_NONE)->GetEffect(EFFECT_0).BasePoints))
                {
                    GetCaster()->CastSpell(target, SPELL_DRUID_CULTIVATION_HOT, true);
                }
            }
        }
        //
        // void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
        // {
        //     if (!GetCaster())
        //         return;
        //
        //     amount = CalculatePct(GetCaster()->SpellBaseHealingBonusDone(SPELL_SCHOOL_MASK_NATURE), 60);
        // }

           //Posible Fixed

        enum Spells
        {
            GlyphofRejuvenation = 17076,
            GlyphofRejuvenationEffect = 96206
        };
        void HandleCalculateAmount(AuraEffect const* /*p_AurEff*/, int32& amount, bool& /*canBeRecalculated*/)
        {
            if (Unit* l_Caster = GetCaster())
            {
                ///If soul of the forest is activated we increase the heal by 100%
                if (l_Caster->HasAura(SPELL_DRUID_SOUL_OF_THE_FOREST_RESTO) && !l_Caster->HasAura(SPELL_DRUID_REJUVENATION))
                {
                    amount *= 2;
                    l_Caster->RemoveAura(SPELL_DRUID_SOUL_OF_THE_FOREST_RESTO);
                }
            }
        }

        void OnApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
        {
            Unit* caster = GetCaster();

            if (!caster)
                return;

            if (AuraEffect* GlyphOfRejuvenation = caster->GetAuraEffect(Spells::GlyphofRejuvenation, EFFECT_0))
            {
                GlyphOfRejuvenation->SetAmount(GlyphOfRejuvenation->GetAmount() + 1);
                if (GlyphOfRejuvenation->GetAmount() >= 3)
                    caster->CastSpell(caster, Spells::GlyphofRejuvenationEffect, true);
            }
        }

        void OnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
        {
            Unit* caster = GetCaster();

            if (!caster)
                return;

            if (AuraEffect* l_GlyphOfRejuvenation = caster->GetAuraEffect(Spells::GlyphofRejuvenation, EFFECT_0))
            {
                l_GlyphOfRejuvenation->SetAmount(l_GlyphOfRejuvenation->GetAmount() - 1);
                if (l_GlyphOfRejuvenation->GetAmount() < 3)
                    caster->RemoveAura(Spells::GlyphofRejuvenationEffect);
            }
        }

        void Register() override
        {
            // Posible Fixed
            OnEffectApply += AuraEffectApplyFn(spell_dru_rejuvenation_AuraScript::OnApply, EFFECT_0, SPELL_AURA_PERIODIC_HEAL, AURA_EFFECT_HANDLE_REAL);
            OnEffectRemove += AuraEffectRemoveFn(spell_dru_rejuvenation_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_HEAL, AURA_EFFECT_HANDLE_REAL);
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dru_rejuvenation_AuraScript::HandleCalculateAmount, EFFECT_0, SPELL_AURA_PERIODIC_HEAL);
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_dru_rejuvenation_AuraScript::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_HEAL);

            //  DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dru_rejuvenation_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_PERIODIC_HEAL);
            //  AfterEffectRemove += AuraEffectRemoveFn(spell_dru_rejuvenation_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_HEAL, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_dru_rejuvenation_AuraScript();
    }

    class spell_dru_rejuvenation_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_dru_rejuvenation_SpellScript);

        bool Validate(SpellInfo const* /*spellInfo*/) override
        {
            return ValidateSpellInfo(
                {
                    SPELL_DRUID_GERMINATION,
                    SPELL_DRUID_GERMINATION_HOT,
                    SPELL_DRUID_ABUNDANCE,
                    SPELL_DRUID_ABUNDANCE_BUFF,
                });
        }

        int32 m_RejuvenationAura = 0;
        int32 m_RejuvenationAuraAmount = 0;

        void HandleAfterHit()
        {
            Unit* caster = GetCaster();
            if (!caster)
                return;

            Unit* target = GetHitUnit();
            if (!target)
                return;

            Aura* RejuvenationAura = target->GetAura(SPELL_DRUID_REJUVENATION, caster->GetGUID());

            if (RejuvenationAura && m_RejuvenationAura > 0)
                RejuvenationAura->SetDuration(m_RejuvenationAura);

            if (AuraEffect* NewRejuvenationAuraEffect = target->GetAuraEffect(SPELL_DRUID_REJUVENATION, EFFECT_0))
            {
                if (caster->HasAura(SPELL_DRUID_SOUL_OF_THE_FOREST_RESTO))
                {
                    NewRejuvenationAuraEffect->SetAmount(NewRejuvenationAuraEffect->GetAmount() * 2);
                    caster->RemoveAura(SPELL_DRUID_SOUL_OF_THE_FOREST_RESTO);
                }
            }
            if (caster->HasAura(207383))
                caster->CastSpell(caster, SPELL_DRUID_ABUNDANCE, true);
        }

        void HandleBeforeHit(SpellMissInfo missInfo)
        {
            Unit* caster = GetCaster();
            if (!caster)
                return;

            Unit* target = GetHitUnit();
            if (!target)
                return;

            if (caster->HasAura(SPELL_DRUID_SOUL_OF_THE_FOREST_RESTO))
            {
                //      NewRejuvenationAuraEffect->SetAmount(NewRejuvenationAuraEffect->GetAmount() * 2);
                SetHitHeal(GetHitHeal() * 2);
                //      caster->RemoveAura(SPELL_DRUID_SOUL_OF_THE_FOREST_RESTO);
            }

            if (caster->HasAura(SPELL_DRU_NURTURING_DORMANCY) && target && target->HasAura(SPELL_DRUID_REJUVENATION) && target->IsFullHealth())
            {
                Aura* RejuvenationAura = target->GetAura(SPELL_DRUID_REJUVENATION, caster->GetGUID());
                int32 ClampedDuration = RejuvenationAura->GetDuration() + (2 * IN_MILLISECONDS);
                int32 MaxDuration = caster->HasAura(197721) ? 31 : 23; // Flourish
                if (ClampedDuration > (MaxDuration * IN_MILLISECONDS))
                    ClampedDuration = (MaxDuration * IN_MILLISECONDS);

                RejuvenationAura->SetDuration(ClampedDuration);
            }

            ///Germination
            if (caster->HasAura(155675) && target->HasAura(SPELL_DRUID_REJUVENATION, caster->GetGUID()))
            {
                Aura* RejuvenationAura = target->GetAura(SPELL_DRUID_REJUVENATION, caster->GetGUID());
                if (!RejuvenationAura)
                    return;

                if (!target->HasAura(155777, caster->GetGUID()))
                {
                    caster->CastSpell(target, 155777, true);
                    m_RejuvenationAura = RejuvenationAura->GetDuration();
                }
                else
                {
                    Aura* GerminationAura = target->GetAura(155777, caster->GetGUID());
                    Aura* RejuvenationAura = target->GetAura(SPELL_DRUID_REJUVENATION, caster->GetGUID());
                    if (GerminationAura && RejuvenationAura)
                    {
                        int32 GerminationDuration = GerminationAura->GetDuration();
                        int32 RejuvenationDuration = RejuvenationAura->GetDuration();
                        if (GerminationDuration > RejuvenationDuration)
                        {
                            caster->AddAura(SPELL_DRUID_REJUVENATION, target);
                        }
                        else
                        {
                            caster->CastSpell(target, 155777, true);
                            m_RejuvenationAura = RejuvenationDuration;
                        }
                    }
                }
            }
        }

        void Register()
        {
            BeforeHit += BeforeSpellHitFn(spell_dru_rejuvenation_SpellScript::HandleBeforeHit);
            AfterHit += SpellHitFn(spell_dru_rejuvenation_SpellScript::HandleAfterHit);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_dru_rejuvenation_SpellScript();
    }
};

// 58180 - Infected Wounds
class spell_dru_infected_wounds : public SpellScriptLoader
{
public:
    spell_dru_infected_wounds() : SpellScriptLoader("spell_dru_infected_wounds") { }


    class spell_dru_infected_wounds_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_dru_infected_wounds_SpellScript);

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            if (!GetCaster())
                return;

            if (GetCaster()->HasAura(GetSpellInfo()->Id))
                GetCaster()->RemoveAurasDueToSpell(GetSpellInfo()->Id);
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_dru_infected_wounds_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_dru_infected_wounds_SpellScript();
    }
};

// 190984 - Solar Wrath | 194153 - Lunar Strike
class spell_dru_blessing_of_elune : public SpellScript
{
    PrepareSpellScript(spell_dru_blessing_of_elune);

    void HandleOnHit()
    {
        Unit* caster = GetCaster();

        if (!caster)
            return;

        uint32 power = GetHitDamage();

        if (Aura* aura = caster->GetAura(202737))
            if (AuraEffect* aurEff = aura->GetEffect(EFFECT_0))
                power += CalculatePct(power, aurEff->GetAmount());

        SetHitDamage(power);
    }

    void Register()
    {
        OnHit += SpellHitFn(spell_dru_blessing_of_elune::HandleOnHit);
    }
};

// 208253 - Essence of G'Hanir
class spell_dru_essence_of_ghanir : public SpellScriptLoader
{
public:
    spell_dru_essence_of_ghanir() : SpellScriptLoader("spell_dru_essence_of_ghanir") { }

    class spell_dru_essence_of_ghanir_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_dru_essence_of_ghanir_AuraScript);

        void HandleEffectCalcSpellMod(AuraEffect const* aurEff, SpellModifier*& spellMod)
        {
            if (!spellMod)
            {
                SpellModifierByClassMask* mod = new SpellModifierByClassMask(GetAura());
                mod->op = SpellModOp::PeriodicHealingAndDamage;
                mod->type = SPELLMOD_FLAT;
                mod->spellId = GetId();
                mod->mask = aurEff->GetSpellEffectInfo().SpellClassMask;
                spellMod = mod;
            }

            static_cast<SpellModifierByClassMask*>(spellMod)->value = aurEff->GetAmount() / 7;
        }

        void Register() override
        {
            DoEffectCalcSpellMod += AuraEffectCalcSpellModFn(spell_dru_essence_of_ghanir_AuraScript::HandleEffectCalcSpellMod, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_dru_essence_of_ghanir_AuraScript();
    }
};

// 200851 - Rage of the Sleeper
class spell_dru_rage_of_the_sleeper : public SpellScriptLoader
{
public:
    spell_dru_rage_of_the_sleeper() : SpellScriptLoader("spell_dru_rage_of_the_sleeper") { }

    class spell_dru_rage_of_the_sleeper_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_dru_rage_of_the_sleeper_AuraScript);

        void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
        {
            amount = -1;
        }

        void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            if (Unit* caster = GetCaster())
                if (Unit* target = caster->GetVictim())
                    caster->CastSpell(target, 219432, true);
        }

        void Register() override
        {
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dru_rage_of_the_sleeper_AuraScript::CalculateAmount, EFFECT_1, SPELL_AURA_SCHOOL_ABSORB);
            AfterEffectRemove += AuraEffectRemoveFn(spell_dru_rage_of_the_sleeper_AuraScript::OnRemove, EFFECT_1, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_dru_rage_of_the_sleeper_AuraScript();
    }
};

// 194153 Lunar Strike
class spell_druid_lunar_strike : public SpellScript
{
    PrepareSpellScript(spell_druid_lunar_strike);

    enum Spells
    {
        SPELL_DRUID_LUNAR_STRIKE = 194153,
        SPELL_DRUID_WARRIOR_OF_ELUNE = 202425,
        SPELL_DRUID_NATURES_BALANCE = 202430,
    };

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_MOONFIRE_DAMAGE, SPELL_DRUID_WARRIOR_OF_ELUNE, SPELL_DRUID_LUNAR_STRIKE, SPELL_DRUID_NATURES_BALANCE });
    }

    void HandleHitTarget(SpellEffIndex /*effIndex*/)
    {
        Unit* explTarget = GetExplTargetUnit();
        Unit* currentTarget = GetHitUnit();

        if (!explTarget || !currentTarget)
            return;

        if (currentTarget != explTarget)
            SetHitDamage(GetHitDamage() * GetSpellInfo()->GetEffect(EFFECT_2).BasePoints / 100);

        if (GetCaster()->HasAura(SPELL_DRUID_NATURES_BALANCE))
            if (Aura* moonfireDOT = currentTarget->GetAura(SPELL_DRUID_MOONFIRE_DAMAGE, GetCaster()->GetGUID()))
            {
                int32 duration = moonfireDOT->GetDuration();
                int32 newDuration = duration + 6 * IN_MILLISECONDS;

                if (newDuration > moonfireDOT->GetMaxDuration())
                    moonfireDOT->SetMaxDuration(newDuration);

                moonfireDOT->SetDuration(newDuration);
            }

        if (GetCaster() && roll_chance_f(20) && GetCaster()->HasAura(SPELL_DRU_ECLIPSE))
            GetCaster()->CastSpell(nullptr, SPELL_DRU_SOLAR_EMPOWEREMENT, true);
    }

    void HandleHit(SpellEffIndex /*effIndex*/)
    {
        if (Aura* WarriorOfElune = GetCaster()->GetAura(SPELL_DRUID_WARRIOR_OF_ELUNE))
        {
            int32 amount = WarriorOfElune->GetEffect(EFFECT_0)->GetAmount();
            WarriorOfElune->GetEffect(EFFECT_0)->SetAmount(amount - 1);
            if (amount == -102)
                GetCaster()->RemoveAurasDueToSpell(SPELL_DRUID_WARRIOR_OF_ELUNE);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_druid_lunar_strike::HandleHitTarget, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        OnEffectHit += SpellEffectFn(spell_druid_lunar_strike::HandleHit, EFFECT_1, SPELL_EFFECT_ENERGIZE);
    }
};

// 190984 Solar Wrath
class spell_druid_solar_wrath : public SpellScript
{
    PrepareSpellScript(spell_druid_solar_wrath);

    enum Spells
    {
        SPELL_DRUID_SOLAR_WRATH = 190984,
        SPELL_DRUID_NATURES_BALANCE = 202430,
        SPELL_DRUID_SUNFIRE_DOT = 164815,
    };

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_SUNFIRE_DOT, SPELL_DRUID_SOLAR_WRATH, SPELL_DRUID_NATURES_BALANCE });
    }

    void HandleHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (Unit* target = GetHitUnit())
            if (GetCaster()->HasAura(SPELL_DRUID_NATURES_BALANCE))
                if (Aura* sunfireDOT = target->GetAura(SPELL_DRUID_SUNFIRE_DOT, GetCaster()->GetGUID()))
                {
                    int32 duration = sunfireDOT->GetDuration();
                    int32 newDuration = duration + 4 * IN_MILLISECONDS;

                    if (newDuration > sunfireDOT->GetMaxDuration())
                        sunfireDOT->SetMaxDuration(newDuration);

                    sunfireDOT->SetDuration(newDuration);
                }
        if (GetCaster() && roll_chance_f(20) && GetCaster()->HasAura(SPELL_DRU_ECLIPSE))
            GetCaster()->CastSpell(nullptr, SPELL_DRU_LUNAR_EMPOWEREMENT, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_druid_solar_wrath::HandleHitTarget, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// 203975 Earthwarden
class spell_druid_earthwarden_triggered : public AuraScript
{
    PrepareAuraScript(spell_druid_earthwarden_triggered);

    enum Spells
    {
        SPELL_DRUID_EARTHWARDEN = 203974,
        SPELL_DRUID_EARTHWARDEN_TRIGGERED = 203975,
    };

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_EARTHWARDEN, SPELL_DRUID_EARTHWARDEN_TRIGGERED });
    }

    void CalculateAmount(AuraEffect const* /*auraEffect*/, int32& amount, bool& /*canBeRecalculated*/)
    {
        amount = -1;
    }

    void Absorb(AuraEffect* auraEffect, DamageInfo& dmgInfo, uint32& absorbAmount)
    {
        if (dmgInfo.GetDamageType() == DIRECT_DAMAGE)
        {
            SpellInfo const* earthwarden = sSpellMgr->AssertSpellInfo(SPELL_DRUID_EARTHWARDEN, DIFFICULTY_NONE);

            absorbAmount = CalculatePct(dmgInfo.GetDamage(), earthwarden->GetEffect(EFFECT_0).BasePoints);
            GetCaster()->RemoveAurasDueToSpell(SPELL_DRUID_EARTHWARDEN_TRIGGERED);
        }
    }

    void Register()
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_druid_earthwarden_triggered::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
        OnEffectAbsorb += AuraEffectAbsorbFn(spell_druid_earthwarden_triggered::Absorb, EFFECT_0);
    }
};

// 203974 Earthwarden
class spell_druid_earthwarden : public AuraScript
{
    PrepareAuraScript(spell_druid_earthwarden);

    enum Spells
    {
        SPELL_DRUID_EARTHWARDEN = 203974,
        SPELL_DRUID_EARTHWARDEN_TRIGGERED = 203975,
        SPELL_DRUID_TRASH = 77758,
    };

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_EARTHWARDEN, SPELL_DRUID_EARTHWARDEN_TRIGGERED, SPELL_DRUID_TRASH });
    }

    void OnProc(AuraEffect* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        if (!GetCaster()->ToPlayer()->GetSpellHistory()->HasCooldown(SPELL_DRUID_EARTHWARDEN))
            GetCaster()->AddAura(SPELL_DRUID_EARTHWARDEN_TRIGGERED, GetCaster());
        GetCaster()->ToPlayer()->GetSpellHistory()->AddCooldown(SPELL_DRUID_EARTHWARDEN, 0, std::chrono::milliseconds(500));
    }

    void Register()
    {
        OnEffectProc += AuraEffectProcFn(spell_druid_earthwarden::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 80313 Pulverize
class spell_druid_pulverize : public SpellScript
{
    PrepareSpellScript(spell_druid_pulverize);

    enum Spells
    {
        SPELL_DRUID_PULVERIZE = 80313,
        SPELL_DRUID_TRASH_DOT_TWO_STACKS_MARKER = 158790,
        SPELL_DRUID_PULVERIZE_DAMAGE_REDUCTION_BUFF = 158792,
    };

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_PULVERIZE, SPELL_DRUID_TRASH_DOT_TWO_STACKS_MARKER });
    }

    void HandleHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (Unit* target = GetHitUnit())
        {
            target->RemoveAurasDueToSpell(SPELL_DRUID_TRASH_DOT_TWO_STACKS_MARKER);
            GetCaster()->CastSpell(target, SPELL_DRUID_PULVERIZE_DAMAGE_REDUCTION_BUFF, true);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_druid_pulverize::HandleHitTarget, EFFECT_2, SPELL_EFFECT_DUMMY);
    }
};

// 204053 Rend and Tear
class spell_druid_rend_and_tear : public AuraScript
{
    PrepareAuraScript(spell_druid_rend_and_tear);

    enum Spells
    {
        SPELL_DRUID_REND_AND_TEAR = 204053,
        SPELL_DRUID_TRASH_DOT = 192090,
    };

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_REND_AND_TEAR, SPELL_DRUID_TRASH_DOT });
    }

    void CalculateAmount(AuraEffect const* /*auraEffect*/, int32& amount, bool& /*canBeRecalculated*/)
    {
        amount = -1;
    }

    void Absorb(AuraEffect* auraEffect, DamageInfo& dmgInfo, uint32& absorbAmount)
    {
        Unit* caster = GetCaster();
        Unit* attacker = dmgInfo.GetAttacker();
        absorbAmount = 0;

        if (!caster || !attacker || !HasEffect(EFFECT_1))
            return;

        if (caster->GetShapeshiftForm() == FORM_BEAR_FORM)
            if (Aura* trashDOT = attacker->GetAura(SPELL_DRUID_TRASH_DOT, caster->GetGUID()))
                absorbAmount = CalculatePct(dmgInfo.GetDamage(), trashDOT->GetStackAmount() * GetSpellInfo()->GetEffect(EFFECT_1).BasePoints);
    }

    void HandleEffectCalcSpellMod(AuraEffect const* aurEff, SpellModifier*& spellMod)
    {
        if (!spellMod)
            return;

        static_cast<SpellModifierByClassMask*>(spellMod)->value = GetCaster()->GetShapeshiftForm() == FORM_BEAR_FORM ? aurEff->GetAmount() : 0;
    }

    void Register()
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_druid_rend_and_tear::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
        OnEffectAbsorb += AuraEffectAbsorbFn(spell_druid_rend_and_tear::Absorb, EFFECT_0);
        DoEffectCalcSpellMod += AuraEffectCalcSpellModFn(spell_druid_rend_and_tear::HandleEffectCalcSpellMod, EFFECT_1, SPELL_AURA_ADD_FLAT_MODIFIER);
        DoEffectCalcSpellMod += AuraEffectCalcSpellModFn(spell_druid_rend_and_tear::HandleEffectCalcSpellMod, EFFECT_2, SPELL_AURA_ADD_FLAT_MODIFIER);
    }
};

//Incarnation: Tree of Life 33891
class incarnation_tree_of_life : public SpellScript
{
    PrepareSpellScript(incarnation_tree_of_life);

    void HandleAfterCast()
    {
        Unit* caster = GetCaster();
        if (Aura* tree = caster->GetAura(33891))
        {
            tree->SetDuration(30000, true);
        }
    }

    void Register() override
    {
        AfterCast += SpellCastFn(incarnation_tree_of_life::HandleAfterCast);
    }
};

//274837
class spell_feral_frenzy : public SpellScript
{
    PrepareSpellScript(spell_feral_frenzy);

    void HandleOnHit()
    {
        Unit* caster = GetCaster();
        Unit* target = GetHitUnit();

        if (!caster || !target)
            return;

        this->strikes = 0;

        int32 strikeDamage = 100 / 20 + caster->m_unitData->AttackPower;

        caster->GetScheduler().Schedule(50ms, [caster, target, strikeDamage, this](TaskContext context)
            {
                if (caster->GetDistance2d(target) <= 5.0f)
                {
                    //                caster->DealDamage(target, strikeDamage, 0, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, 0, true);
                    strikes++;

                    if (this->strikes < 5)
                        context.Repeat(200ms);

                    else if (this->strikes == 5)
                    {
                        caster->CastSpell(target, SPELL_FERAL_FRENZY_BLEED, true);
                        int32 bleedDamage = 100 / 10 + caster->m_unitData->AttackPower;
                        //                    caster->DealDamage(target, bleedDamage, 0, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, 0, true);
                    }
                }
            });
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_feral_frenzy::HandleOnHit);
    }

private:
    uint8 strikes;
};

class feral_spells : public PlayerScript
{
public:
    feral_spells() : PlayerScript("feral_spells") { }

    void OnLogin(Player* player, bool) override
    {
        if (player->GetSpecializationId() != TALENT_SPEC_DRUID_CAT)
            return;

        if (player->GetLevel() >= 5 && !player->HasSpell(SPELL_DRUID_SHRED))
            player->LearnSpell(SPELL_DRUID_SHRED, false, true);

        if (player->GetLevel() >= 20 && !player->HasSpell(SPELL_DRUID_RIP))
            player->LearnSpell(SPELL_DRUID_RIP, false, true);

        if (player->GetLevel() >= 24 && !player->HasSpell(SPELL_DRUID_RAKE))
            player->LearnSpell(SPELL_DRUID_RAKE, false, true);

        if (player->GetLevel() >= 32 && !player->HasSpell(SPELL_DRUID_FEROCIOUS_BITE))
            player->LearnSpell(SPELL_DRUID_FEROCIOUS_BITE, false, true);
    }
};

//78674
class spell_dru_starsurge : public SpellScript
{
    PrepareSpellScript(spell_dru_starsurge);

    void HandleOnHit()
    {
        if (GetCaster())
            if (GetCaster()->GetAuraCount(SPELL_DRU_STARLORD_BUFF) < 3)
                GetCaster()->CastSpell(nullptr, SPELL_DRU_STARLORD_BUFF, true);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_dru_starsurge::HandleOnHit);
    }
};

//191034
class spell_dru_starfall : public SpellScript
{
    PrepareSpellScript(spell_dru_starfall);

    void HandleOnHit()
    {
        if (GetCaster())
            if (GetCaster()->GetAuraCount(SPELL_DRU_STARLORD_BUFF) < 3)
                GetCaster()->CastSpell(nullptr, SPELL_DRU_STARLORD_BUFF, true);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_dru_starfall::HandleOnHit);
    }
};

//274902
class spell_dru_photosynthesis : public AuraScript
{
    PrepareAuraScript(spell_dru_photosynthesis);

    void OnApply(const AuraEffect* /* aurEff */, AuraEffectHandleModes /*mode*/)
    {
        if (!GetCaster()->HasAura(SPELL_DRU_PHOTOSYNTHESIS_MOD_HEAL_TICKS))
            GetCaster()->AddAura(SPELL_DRU_PHOTOSYNTHESIS_MOD_HEAL_TICKS, GetCaster());
    }

    void OnRemove(const AuraEffect* /* aurEff */, AuraEffectHandleModes /*mode*/)
    {
        if (GetCaster()->HasAura(SPELL_DRU_PHOTOSYNTHESIS_MOD_HEAL_TICKS))
            GetCaster()->RemoveAura(SPELL_DRU_PHOTOSYNTHESIS_MOD_HEAL_TICKS);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_dru_photosynthesis::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_dru_photosynthesis::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

//10682, 204066
struct at_dru_lunar_beam : AreaTriggerAI
{
    at_dru_lunar_beam(AreaTrigger* at) : AreaTriggerAI(at) { }

    void OnCreate() override
    {
        at->SetPeriodicProcTimer(1000);
    }

    void OnPeriodicProc() override
    {
        if (at->GetCaster())
            at->GetCaster()->CastSpell(at->GetPosition(), SPELL_DRU_LUNAR_BEAM_DAMAGE_HEAL, true);
    }
};

//3020, 102793
struct at_dru_ursol_vortex : AreaTriggerAI
{
    at_dru_ursol_vortex(AreaTrigger* at) : AreaTriggerAI(at) { }

    void OnUnitEnter(Unit* target) override
    {
        VALIDATE_CASTER();

        if (caster->IsInCombatWith(target))
            caster->CastSpell(target, SPELL_DRU_URSOL_VORTEX_DEBUFF, true);
    }

    void OnUnitExit(Unit* target) override
    {
        target->RemoveAurasDueToSpell(SPELL_DRU_URSOL_VORTEX_DEBUFF);
        if (!_hasPull && target->IsValidAttackTarget(at->GetCaster()))
        {
            _hasPull = true;
            target->CastSpell(at->GetPosition(), SPELL_DRU_URSOL_VORTEX_PULL, true);
        }
    }

private:
    bool _hasPull = false;
};

//102359
class spell_dru_mass_entanglement : public SpellScript
{
    PrepareSpellScript(spell_dru_mass_entanglement);

    void HandleCast()
    {
        std::list<Unit*> targetList;
        GetCaster()->GetAttackableUnitListInRange(targetList, 15.0f);
        if (targetList.size())
            for (auto& targets : targetList)
            {
                GetCaster()->AddAura(SPELL_DRU_MASS_ENTANGLEMENT, targets);
            }
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_dru_mass_entanglement::HandleCast);
    }
};

class dru_predator : public PlayerScript
{
public:
    dru_predator() : PlayerScript("dru_predator") { }

    void OnPVPKill(Player* killer, Player* killed)
    {
        if (killer->GetClass() == CLASS_DRUID)
            return;

        if (!killer->HasAura(SPELL_DRU_PREDATOR))
            return;

        if (killer->GetSpellHistory()->HasCooldown(SPELL_DRU_TIGER_FURY))
            killer->GetSpellHistory()->ResetCooldown(SPELL_DRU_TIGER_FURY);
    }

    void OnCreatureKill(Player* killer, Creature* killed)
    {
        if (killer->GetClass() == CLASS_DRUID)
            return;

        if (!killer->HasAura(SPELL_DRU_PREDATOR))
            return;

        if (killer->GetSpellHistory()->HasCooldown(SPELL_DRU_TIGER_FURY))
            killer->GetSpellHistory()->ResetCooldown(SPELL_DRU_TIGER_FURY);
    }
};

// Teleport : Moonglade - 18960
class spell_dru_teleport_moonglade : public SpellScriptLoader
{
public:
    spell_dru_teleport_moonglade() : SpellScriptLoader("spell_dru_teleport_moonglade") { }

    class spell_dru_teleport_moonglade_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_dru_teleport_moonglade_SpellScript);

        void HandleAfterCast()
        {
            if (Player* _player = GetCaster()->ToPlayer())
                _player->TeleportTo(1, 7964.063f, -2491.099f, 487.83f, _player->GetOrientation());
        }

        void Register()
        {
            AfterCast += SpellCastFn(spell_dru_teleport_moonglade_SpellScript::HandleAfterCast);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_dru_teleport_moonglade_SpellScript();
    }
};
// 202430 - Nature's Balance
class spell_dru_natures_balance : public AuraScript
{
    PrepareAuraScript(spell_dru_natures_balance);

    enum Spells
    {
        SPELL_DRUID_NATURES_BALANCE = 202430,
    };

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_NATURES_BALANCE });
    }

    void HandlePeriodic(AuraEffect const* aurEff)
    {
        Unit* caster = GetCaster();
        if (!caster || !caster->IsAlive() || caster->GetMaxPower(POWER_LUNAR_POWER) == 0)
            return;

        if (caster->IsInCombat())
        {
            int32 amount = std::max(caster->GetAuraEffect(SPELL_DRUID_NATURES_BALANCE, EFFECT_0)->GetAmount(), 0);
            // don't regen when permanent aura target has full power
            if (caster->GetPower(POWER_LUNAR_POWER) == caster->GetMaxPower(POWER_LUNAR_POWER))
                return;

            caster->ModifyPower(POWER_LUNAR_POWER, amount);
        }
        else
        {
            if (caster->GetPower(POWER_LUNAR_POWER) > 500)
                return;

            caster->SetPower(POWER_LUNAR_POWER, 500);
        }
    }

    void Register()
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_dru_natures_balance::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_ENERGIZE);
    }
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

class spell_dru_base_transformer : public SpellScript
{
    PrepareSpellScript(spell_dru_base_transformer);

protected:
    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ GetShapeshiftFormSpell() });
    }

    void HandleOnCast()
    {
        // Change into cat form
        if (GetCaster()->GetShapeshiftForm() != GetShapeshiftForm())
            GetCaster()->CastSpell(GetCaster(), GetShapeshiftFormSpell(), true);
    }

    void Register() override
    {
        BeforeCast += SpellCastFn(spell_dru_base_transformer::HandleOnCast);
    }

    virtual bool ToCatForm() const = 0;

    ShapeshiftForm GetShapeshiftForm() const { return ToCatForm() ? FORM_CAT_FORM : FORM_BEAR_FORM; }
    uint32 GetShapeshiftFormSpell() const { return ToCatForm() ? SPELL_DRUID_CAT_FORM : SPELL_DRUID_BEAR_FORM; }
};

// 22812 - Barkskin
class spell_dru_barkskin : public AuraScript
{
    PrepareAuraScript(spell_dru_barkskin);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_BRAMBLES_PASSIVE });
    }

    void HandlePeriodic(AuraEffect const* /*aurEff*/)
    {
        Unit* target = GetTarget();
        if (target->HasAura(SPELL_DRUID_BRAMBLES_PASSIVE))
            target->CastSpell(target, SPELL_DRUID_BRAMBLES_DAMAGE_AURA, true);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_dru_barkskin::HandlePeriodic, EFFECT_2, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 50334 - Berserk
class spell_dru_berserk : public spell_dru_base_transformer
{
    PrepareSpellScript(spell_dru_berserk);

    bool Validate(SpellInfo const* spellInfo) override
    {
        if (!spell_dru_base_transformer::Validate(spellInfo))
            return false;

        return ValidateSpellInfo({ SPELL_DRUID_MANGLE, SPELL_DRUID_THRASH_BEAR, SPELL_DRUID_GROWL });
    }

    void ResetCooldowns()
    {
        GetCaster()->GetSpellHistory()->ResetCooldown(SPELL_DRUID_MANGLE);
        GetCaster()->GetSpellHistory()->ResetCooldown(SPELL_DRUID_THRASH_BEAR);
        GetCaster()->GetSpellHistory()->ResetCooldown(SPELL_DRUID_GROWL);
    }

    void Register() override
    {
        spell_dru_base_transformer::Register();
        AfterCast += SpellCastFn(spell_dru_berserk::ResetCooldowns);
    }

protected:
    bool ToCatForm() const override { return false; }
};

// 203953 - Brambles - SPELL_DRUID_BRAMBLES_PASSIVE
class spell_dru_brambles : public AuraScript
{
    PrepareAuraScript(spell_dru_brambles);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_BRAMBLES_REFLECT, SPELL_DRUID_BRAMBLES_DAMAGE_AURA });
    }

    void HandleAbsorb(AuraEffect* /*aurEff*/, DamageInfo& /*dmgInfo*/, uint32& /*absorbAmount*/)
    {
        // Prevent Removal
        PreventDefaultAction();
    }

    void HandleAfterAbsorb(AuraEffect* /*aurEff*/, DamageInfo& dmgInfo, uint32& absorbAmount)
    {
        // reflect back damage to the attacker
        Unit* target = GetTarget();
        if (Unit* attacker = dmgInfo.GetAttacker())
            target->CastSpell(attacker, SPELL_DRUID_BRAMBLES_REFLECT, CastSpellExtraArgs(TRIGGERED_FULL_MASK).AddSpellMod(SPELLVALUE_BASE_POINT0, absorbAmount));
    }

    void Register() override
    {
        OnEffectAbsorb += AuraEffectAbsorbFn(spell_dru_brambles::HandleAbsorb, EFFECT_0);
        AfterEffectAbsorb += AuraEffectAbsorbFn(spell_dru_brambles::HandleAfterAbsorb, EFFECT_0);
    }
};

// 155835 - Bristling Fur
class spell_dru_bristling_fur : public AuraScript
{
    PrepareAuraScript(spell_dru_bristling_fur);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_BRISTLING_FUR_GAIN_RAGE });
    }

    void HandleProc(AuraEffect* /*aurEff*/, ProcEventInfo& eventInfo)
    {
        // BristlingFurRage = 100 * Damage / MaxHealth.
        if (DamageInfo* damageInfo = eventInfo.GetDamageInfo())
        {
            Unit* target = GetTarget();
            uint32 rage = target->GetMaxPower(POWER_RAGE) * (float)damageInfo->GetDamage() / (float)target->GetMaxHealth();
            if (rage > 0)
                target->CastSpell(target, SPELL_DRUID_BRISTLING_FUR_GAIN_RAGE, CastSpellExtraArgs(TRIGGERED_FULL_MASK).AddSpellMod(SPELLVALUE_BASE_POINT0, rage));
        }
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_dru_bristling_fur::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 768 - CatForm - SPELL_DRUID_CAT_FORM
class spell_dru_cat_form : public AuraScript
{
    PrepareAuraScript(spell_dru_cat_form);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_PROWL });
    }

    void HandleApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        GetCaster()->PlayDirectSound(SOUND_CAT_FORM); // cat sounds
    }

    void HandleAfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->RemoveOwnedAura(SPELL_DRUID_PROWL);
        GetTarget()->RemoveOwnedAura(SPELL_DRUID_DASH);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_dru_cat_form::HandleApply, EFFECT_0, SPELL_AURA_MOD_SHAPESHIFT, AURA_EFFECT_HANDLE_REAL);
        AfterEffectRemove += AuraEffectRemoveFn(spell_dru_cat_form::HandleAfterRemove, EFFECT_0, SPELL_AURA_MOD_SHAPESHIFT, AURA_EFFECT_HANDLE_REAL);
    }
};

// 1850 - Dash
class spell_dru_dash : public SpellScript
{
    PrepareSpellScript(spell_dru_dash);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (GetCaster()->GetShapeshiftForm() != FORM_CAT_FORM)
        {
            GetCaster()->CastSpell(GetCaster(), SPELL_DRUID_CAT_FORM, true);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_dru_dash::HandleDummy, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
    }
};

class spell_dru_eclipse_common
{
public:
    static void SetSpellCount(Unit* unitOwner, uint32 spellId, uint32 amount)
    {
        Aura* aura = unitOwner->GetAura(spellId);
        if (!aura)
            unitOwner->CastSpell(unitOwner, spellId, CastSpellExtraArgs(TRIGGERED_FULL_MASK).AddSpellMod(SPELLVALUE_AURA_STACK, amount));
        else
            aura->SetStackAmount(amount);
    }
};

// 48517 Eclipse (Solar) + 48518 Eclipse (Lunar)
class spell_dru_eclipse_aura : public AuraScript
{
    PrepareAuraScript(spell_dru_eclipse_aura);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_ECLIPSE_LUNAR_SPELL_CNT, SPELL_DRUID_ECLIPSE_SOLAR_SPELL_CNT, SPELL_DRUID_ECLIPSE_DUMMY });
    }

    void HandleRemoved(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        AuraEffect const* auraEffDummy = GetTarget()->GetAuraEffect(SPELL_DRUID_ECLIPSE_DUMMY, EFFECT_0);
        if (!auraEffDummy)
            return;

        uint32 spellId = GetSpellInfo()->Id == SPELL_DRUID_ECLIPSE_SOLAR_AURA ? SPELL_DRUID_ECLIPSE_LUNAR_SPELL_CNT : SPELL_DRUID_ECLIPSE_SOLAR_SPELL_CNT;
        spell_dru_eclipse_common::SetSpellCount(GetTarget(), spellId, auraEffDummy->GetAmount());
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_dru_eclipse_aura::HandleRemoved, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
    }
};

// 79577 - Eclipse - SPELL_DRUID_ECLIPSE_DUMMY
class spell_dru_eclipse_dummy : public AuraScript
{
    PrepareAuraScript(spell_dru_eclipse_dummy);

    class InitializeEclipseCountersEvent : public BasicEvent
    {
    public:
        InitializeEclipseCountersEvent(Unit* owner, uint32 count) : BasicEvent(), _owner(owner), _count(count) { }

        bool Execute(uint64, uint32) override
        {
            spell_dru_eclipse_common::SetSpellCount(_owner, SPELL_DRUID_ECLIPSE_SOLAR_SPELL_CNT, _count);
            spell_dru_eclipse_common::SetSpellCount(_owner, SPELL_DRUID_ECLIPSE_LUNAR_SPELL_CNT, _count);
            return true;
        }

    private:
        Unit* _owner;
        uint32 _count;
    };

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_ECLIPSE_SOLAR_SPELL_CNT, SPELL_DRUID_ECLIPSE_LUNAR_SPELL_CNT,
            SPELL_DRUID_ECLIPSE_SOLAR_AURA, SPELL_DRUID_ECLIPSE_LUNAR_AURA });
    }

    void HandleProc(ProcEventInfo& eventInfo)
    {
        if (SpellInfo const* spellInfo = eventInfo.GetSpellInfo())
        {
            if (spellInfo->SpellFamilyFlags & flag128(0x4, 0x0, 0x0, 0x0)) // Starfire
                OnSpellCast(SPELL_DRUID_ECLIPSE_SOLAR_SPELL_CNT, SPELL_DRUID_ECLIPSE_LUNAR_SPELL_CNT, SPELL_DRUID_ECLIPSE_SOLAR_AURA);
            else if (spellInfo->SpellFamilyFlags & flag128(0x1, 0x0, 0x0, 0x0)) // Wrath
                OnSpellCast(SPELL_DRUID_ECLIPSE_LUNAR_SPELL_CNT, SPELL_DRUID_ECLIPSE_SOLAR_SPELL_CNT, SPELL_DRUID_ECLIPSE_LUNAR_AURA);
        }
    }

    void HandleApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        // counters are applied with a delay
        GetTarget()->m_Events.AddEventAtOffset(new InitializeEclipseCountersEvent(GetTarget(), aurEff->GetAmount()), 1s);
    }

    void HandleRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->RemoveAura(SPELL_DRUID_ECLIPSE_SOLAR_SPELL_CNT);
        GetTarget()->RemoveAura(SPELL_DRUID_ECLIPSE_LUNAR_SPELL_CNT);
    }

    void OnOwnerOutOfCombat(bool isNowInCombat)
    {
        if (!isNowInCombat)
            GetTarget()->CastSpell(GetTarget(), SPELL_DRUID_ECLIPSE_OOC, TRIGGERED_FULL_MASK);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_dru_eclipse_dummy::HandleApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        AfterEffectRemove += AuraEffectApplyFn(spell_dru_eclipse_dummy::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnProc += AuraProcFn(spell_dru_eclipse_dummy::HandleProc);
        OnEnterLeaveCombat += AuraEnterLeaveCombatFn(spell_dru_eclipse_dummy::OnOwnerOutOfCombat);
    }

private:
    void OnSpellCast(uint32 cntSpellId, uint32 otherCntSpellId, uint32 eclipseAuraSpellId)
    {
        Unit* target = GetTarget();
        if (Aura* aura = target->GetAura(cntSpellId))
        {
            uint32 remaining = aura->GetStackAmount();
            if (remaining == 0)
                return;

            if (remaining > 1)
                aura->SetStackAmount(remaining - 1);
            else
            {
                // cast eclipse
                target->CastSpell(target, eclipseAuraSpellId, TRIGGERED_FULL_MASK);

                // Remove stacks from other one as well
                // reset remaining power on other spellId
                target->RemoveAura(cntSpellId);
                target->RemoveAura(otherCntSpellId);
            }
        }
    }
};

// 329910 - Eclipse out of combat - SPELL_DRUID_ECLIPSE_OOC
class spell_dru_eclipse_ooc : public AuraScript
{
    PrepareAuraScript(spell_dru_eclipse_ooc);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_ECLIPSE_DUMMY, SPELL_DRUID_ECLIPSE_SOLAR_SPELL_CNT, SPELL_DRUID_ECLIPSE_LUNAR_SPELL_CNT });
    }

    void Tick(AuraEffect const* /*aurEff*/)
    {
        Unit* owner = GetTarget();
        AuraEffect const* auraEffDummy = owner->GetAuraEffect(SPELL_DRUID_ECLIPSE_DUMMY, EFFECT_0);
        if (!auraEffDummy)
            return;

        if (!owner->IsInCombat() && (!owner->HasAura(SPELL_DRUID_ECLIPSE_SOLAR_SPELL_CNT) || !owner->HasAura(SPELL_DRUID_ECLIPSE_LUNAR_SPELL_CNT)))
        {
            // Restore 2 stacks to each spell when out of combat
            spell_dru_eclipse_common::SetSpellCount(owner, SPELL_DRUID_ECLIPSE_SOLAR_SPELL_CNT, auraEffDummy->GetAmount());
            spell_dru_eclipse_common::SetSpellCount(owner, SPELL_DRUID_ECLIPSE_LUNAR_SPELL_CNT, auraEffDummy->GetAmount());
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_dru_eclipse_ooc::Tick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 203974 - Earthwarden
class spell_dru_earthwarden : public AuraScript
{
    PrepareAuraScript(spell_dru_earthwarden);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_THRASH_CAT, SPELL_DRUID_THRASH_BEAR, SPELL_DRUID_EARTHWARDEN_AURA });
    }

    void HandleProc(AuraEffect* /*aurEff*/, ProcEventInfo& /*eventInfo*/)
    {
        Unit* target = GetTarget();
        target->CastSpell(target, SPELL_DRUID_EARTHWARDEN_AURA, true);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_dru_earthwarden::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 22568 - Ferocious Bite
class spell_dru_ferocious_bite : public SpellScript
{
    PrepareSpellScript(spell_dru_ferocious_bite);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellEffect({ { SPELL_DRUID_INCARNATION_KING_OF_THE_JUNGLE, EFFECT_1 } });
    }

    void HandleHitTargetBurn(SpellEffIndex /*effIndex*/)
    {
        int32 newValue = int32(float(GetEffectValue()) * _damageMultiplier);
        SetEffectValue(newValue);
    }

    void HandleHitTargetDmg(SpellEffIndex /*effIndex*/)
    {
        int32 newValue = int32(float(GetHitDamage()) * (1.0f + _damageMultiplier));
        SetHitDamage(newValue);
    }

    void HandleLaunchTarget(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();

        int32 maxExtraConsumedPower = GetEffectValue();

        if (AuraEffect* auraEffect = caster->GetAuraEffect(SPELL_DRUID_INCARNATION_KING_OF_THE_JUNGLE, EFFECT_1))
        {
            float multiplier = 1.0f + float(auraEffect->GetAmount()) / 100.0f;
            maxExtraConsumedPower = int32(float(maxExtraConsumedPower) * multiplier);
            SetEffectValue(maxExtraConsumedPower);
        }

        _damageMultiplier = std::min<float>(caster->GetPower(POWER_ENERGY), maxExtraConsumedPower) / maxExtraConsumedPower;
    }

    void Register() override
    {
        OnEffectLaunchTarget += SpellEffectFn(spell_dru_ferocious_bite::HandleLaunchTarget, EFFECT_1, SPELL_EFFECT_POWER_BURN);
        OnEffectHitTarget += SpellEffectFn(spell_dru_ferocious_bite::HandleHitTargetBurn, EFFECT_1, SPELL_EFFECT_POWER_BURN);
        OnEffectHitTarget += SpellEffectFn(spell_dru_ferocious_bite::HandleHitTargetDmg, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }

private:
    float _damageMultiplier = 0.0f;
};

// 37336 - Druid Forms Trinket
class spell_dru_forms_trinket : public AuraScript
{
    PrepareAuraScript(spell_dru_forms_trinket);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_DRUID_FORMS_TRINKET_BEAR,
                SPELL_DRUID_FORMS_TRINKET_CAT,
                SPELL_DRUID_FORMS_TRINKET_MOONKIN,
                SPELL_DRUID_FORMS_TRINKET_NONE,
                SPELL_DRUID_FORMS_TRINKET_TREE
            });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        Unit* target = eventInfo.GetActor();

        switch (target->GetShapeshiftForm())
        {
        case FORM_BEAR_FORM:
        case FORM_DIRE_BEAR_FORM:
        case FORM_CAT_FORM:
        case FORM_MOONKIN_FORM:
        case FORM_NONE:
        case FORM_TREE_OF_LIFE:
            return true;
        default:
            break;
        }

        return false;
    }

    void HandleProc(AuraEffect* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        Unit* target = eventInfo.GetActor();
        uint32 triggerspell = 0;

        switch (target->GetShapeshiftForm())
        {
        case FORM_BEAR_FORM:
        case FORM_DIRE_BEAR_FORM:
            triggerspell = SPELL_DRUID_FORMS_TRINKET_BEAR;
            break;
        case FORM_CAT_FORM:
            triggerspell = SPELL_DRUID_FORMS_TRINKET_CAT;
            break;
        case FORM_MOONKIN_FORM:
            triggerspell = SPELL_DRUID_FORMS_TRINKET_MOONKIN;
            break;
        case FORM_NONE:
            triggerspell = SPELL_DRUID_FORMS_TRINKET_NONE;
            break;
        case FORM_TREE_OF_LIFE:
            triggerspell = SPELL_DRUID_FORMS_TRINKET_TREE;
            break;
        default:
            return;
        }

        target->CastSpell(target, triggerspell, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_dru_forms_trinket::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_dru_forms_trinket::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 203964 - Galactic Guardian
class spell_dru_galactic_guardian : public AuraScript
{
    PrepareAuraScript(spell_dru_galactic_guardian);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_GALACTIC_GUARDIAN_AURA });
    }

    void HandleProc(AuraEffect* /*aurEff*/, ProcEventInfo& eventInfo)
    {
        if (DamageInfo* damageInfo = eventInfo.GetDamageInfo())
        {
            Unit* target = GetTarget();

            // free automatic moonfire on target
            target->CastSpell(damageInfo->GetVictim(), SPELL_DRUID_MOONFIRE_DAMAGE, true);

            // Cast aura
            target->CastSpell(damageInfo->GetVictim(), SPELL_DRUID_GALACTIC_GUARDIAN_AURA, true);
        }
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_dru_galactic_guardian::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 24858 - Moonkin Form
class spell_dru_glyph_of_stars : public AuraScript
{
    PrepareAuraScript(spell_dru_glyph_of_stars);

    bool Validate(SpellInfo const* /*spell*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_GLYPH_OF_STARS_VISUAL });
    }

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->RemoveAurasDueToSpell(SPELL_DRUID_GLYPH_OF_STARS_VISUAL);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_dru_glyph_of_stars::OnRemove, EFFECT_1, SPELL_AURA_MOD_SHAPESHIFT, AURA_EFFECT_HANDLE_REAL);
    }
};

// 210706 - Gore
class spell_dru_gore : public AuraScript
{
    PrepareAuraScript(spell_dru_gore);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_GORE_PROC, SPELL_DRUID_MANGLE });
    }

    bool CheckEffectProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        return roll_chance_i(aurEff->GetAmount());
    }

    void HandleProc(AuraEffect* /*aurEff*/, ProcEventInfo& /*procInfo*/)
    {
        Unit* owner = GetTarget();
        owner->CastSpell(owner, SPELL_DRUID_GORE_PROC);
        owner->GetSpellHistory()->ResetCooldown(SPELL_DRUID_MANGLE, true);
    }

    void Register() override
    {
        DoCheckEffectProc += AuraCheckEffectProcFn(spell_dru_gore::CheckEffectProc, EFFECT_0, SPELL_AURA_DUMMY);
        OnEffectProc += AuraEffectProcFn(spell_dru_gore::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 99 - Incapacitating Roar
class spell_dru_incapacitating_roar : public spell_dru_base_transformer
{
protected:
    bool ToCatForm() const override { return false; }
};

// 29166 - Innervate
class spell_dru_innervate : public SpellScript
{
    PrepareSpellScript(spell_dru_innervate);

    SpellCastResult CheckCast()
    {
        Player* target = Object::ToPlayer(GetExplTargetUnit());
        if (!target)
            return SPELL_FAILED_BAD_TARGETS;

        ChrSpecializationEntry const* spec = sChrSpecializationStore.LookupEntry(target->GetPrimarySpecialization());
        if (!spec || spec->Role != 1)
            return SPELL_FAILED_BAD_TARGETS;

        return SPELL_CAST_OK;
    }

    void HandleRank2()
    {
        Unit* caster = GetCaster();
        if (caster != GetHitUnit())
            if (AuraEffect const* innervateR2 = caster->GetAuraEffect(SPELL_DRUID_INNERVATE_RANK_2, EFFECT_0))
                caster->CastSpell(caster, SPELL_DRUID_INNERVATE,
                    CastSpellExtraArgs(TRIGGERED_IGNORE_SPELL_AND_CATEGORY_CD | TRIGGERED_IGNORE_CAST_IN_PROGRESS)
                    .SetTriggeringSpell(GetSpell())
                    .AddSpellMod(SPELLVALUE_BASE_POINT0, -innervateR2->GetAmount()));

    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_dru_innervate::CheckCast);
        OnHit += SpellHitFn(spell_dru_innervate::HandleRank2);
    }
};

// 40442 - Druid Tier 6 Trinket
class spell_dru_item_t6_trinket : public AuraScript
{
    PrepareAuraScript(spell_dru_item_t6_trinket);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_DRUID_BLESSING_OF_REMULOS,
                SPELL_DRUID_BLESSING_OF_ELUNE,
                SPELL_DRUID_BLESSING_OF_CENARIUS
            });
    }

    void HandleProc(AuraEffect* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        SpellInfo const* spellInfo = eventInfo.GetSpellInfo();
        if (!spellInfo)
            return;

        uint32 spellId;
        int32 chance;

        // Starfire
        if (spellInfo->SpellFamilyFlags[0] & 0x00000004)
        {
            spellId = SPELL_DRUID_BLESSING_OF_REMULOS;
            chance = 25;
        }
        // Rejuvenation
        else if (spellInfo->SpellFamilyFlags[0] & 0x00000010)
        {
            spellId = SPELL_DRUID_BLESSING_OF_ELUNE;
            chance = 25;
        }
        // Mangle (Bear) and Mangle (Cat)
        else if (spellInfo->SpellFamilyFlags[1] & 0x00000440)
        {
            spellId = SPELL_DRUID_BLESSING_OF_CENARIUS;
            chance = 40;
        }
        else
            return;

        if (roll_chance_i(chance))
            eventInfo.GetActor()->CastSpell(nullptr, spellId, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_dru_item_t6_trinket::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 33763 - Lifebloom
class spell_dru_lifebloom : public AuraScript
{
    PrepareAuraScript(spell_dru_lifebloom);

    bool Validate(SpellInfo const* /*spell*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_LIFEBLOOM_FINAL_HEAL });
    }

    void OnTick(AuraEffect const* /*aurEff*/)
    {
        if (GetCaster()->HasAura(SPELL_DRUID_OMEN_OF_CLARITY))
            if (roll_chance_f(4))
                GetCaster()->CastSpell(nullptr, SPELL_DRU_CLEARCASTING, true);
    }

    void AfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        // Final heal only on duration end
        if (GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_EXPIRE || GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_ENEMY_SPELL)
            GetCaster()->CastSpell(GetUnitOwner(), SPELL_DRUID_LIFEBLOOM_FINAL_HEAL, true);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_dru_lifebloom::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_HEAL);
        AfterEffectRemove += AuraEffectRemoveFn(spell_dru_lifebloom::AfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_HEAL, AURA_EFFECT_HANDLE_REAL);
    }
};

// 113043 - Omen of Clarity
class spell_dru_omen_of_clarity_talent : public AuraScript
{
    PrepareAuraScript(spell_dru_omen_of_clarity_talent);

    bool Validate(SpellInfo const* /*spell*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_OMEN_OF_CLARITY });
    }

    void OnTick(AuraEffect const* /*aurEff*/)
    {
        if (GetCaster()->HasAura(SPELL_DRUID_OMEN_OF_CLARITY) && GetCaster()->HasAura(33763))
            if (roll_chance_f(4))
                GetCaster()->CastSpell(nullptr, SPELL_DRU_CLEARCASTING, true);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_dru_omen_of_clarity_talent::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_HEAL);
    }
};


// 155580 - Lunar Inspiration
class spell_dru_lunar_inspiration : public AuraScript
{
    PrepareAuraScript(spell_dru_lunar_inspiration);

    bool Validate(SpellInfo const* /*spell*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_LUNAR_INSPIRATION_OVERRIDE });
    }

    void AfterApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->CastSpell(GetTarget(), SPELL_DRUID_LUNAR_INSPIRATION_OVERRIDE, true);
    }

    void AfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->RemoveAurasDueToSpell(SPELL_DRUID_LUNAR_INSPIRATION_OVERRIDE);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_dru_lunar_inspiration::AfterApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        AfterEffectRemove += AuraEffectRemoveFn(spell_dru_lunar_inspiration::AfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

//  8921 - Moonfire
class spell_dru_moonfire : public SpellScript
{
    PrepareSpellScript(spell_dru_moonfire);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_MOONFIRE_DAMAGE });
    }

    void HandleOnHit(SpellEffIndex /*effIndex*/)
    {
        GetCaster()->CastSpell(GetHitUnit(), SPELL_DRUID_MOONFIRE_DAMAGE, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_dru_moonfire::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 16864 - Omen of Clarity
class spell_dru_omen_of_clarity : public AuraScript
{
    PrepareAuraScript(spell_dru_omen_of_clarity);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_BALANCE_T10_BONUS, SPELL_DRUID_BALANCE_T10_BONUS_PROC });
    }

    void HandleProc(AuraEffect* /*aurEff*/, ProcEventInfo& /*eventInfo*/)
    {
        Unit* target = GetTarget();
        if (target->HasAura(SPELL_DRUID_BALANCE_T10_BONUS))
            target->CastSpell(nullptr, SPELL_DRUID_BALANCE_T10_BONUS_PROC, true);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_dru_omen_of_clarity::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 5215 - Prowl
class spell_dru_prowl : public spell_dru_base_transformer
{
protected:
    bool ToCatForm() const override { return true; }
};

// 1079 - Rip
class spell_dru_rip : public AuraScript
{
    PrepareAuraScript(spell_dru_rip);

    bool Load() override
    {
        Unit* caster = GetCaster();
        return caster && caster->GetTypeId() == TYPEID_PLAYER;
    }

    void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = false;

        if (Unit* caster = GetCaster())
        {
            // 0.01 * $AP * cp
            uint32 cp = caster->ToPlayer()->GetComboPoints();

            // Idol of Feral Shadows. Can't be handled as SpellMod due its dependency from CPs
            if (AuraEffect const* auraEffIdolOfFeralShadows = caster->GetAuraEffect(SPELL_DRUID_IDOL_OF_FERAL_SHADOWS, EFFECT_0))
                amount += cp * auraEffIdolOfFeralShadows->GetAmount();
            // Idol of Worship. Can't be handled as SpellMod due its dependency from CPs
            else if (AuraEffect const* auraEffIdolOfWorship = caster->GetAuraEffect(SPELL_DRUID_IDOL_OF_WORSHIP, EFFECT_0))
                amount += cp * auraEffIdolOfWorship->GetAmount();

            amount += int32(CalculatePct(caster->GetTotalAttackPowerValue(BASE_ATTACK), cp));
        }
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dru_rip::CalculateAmount, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
    }
};

// 52610 - Savage Roar
class spell_dru_savage_roar : public SpellScript
{
    PrepareSpellScript(spell_dru_savage_roar);

    SpellCastResult CheckCast()
    {
        Unit* caster = GetCaster();
        if (caster->GetShapeshiftForm() != FORM_CAT_FORM)
            return SPELL_FAILED_ONLY_SHAPESHIFT;

        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_dru_savage_roar::CheckCast);
    }
};

class spell_dru_savage_roar_aura : public AuraScript
{
    PrepareAuraScript(spell_dru_savage_roar_aura);

    bool Validate(SpellInfo const* /*spell*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_SAVAGE_ROAR });
    }

    void AfterApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        Unit* target = GetTarget();
        target->CastSpell(target, SPELL_DRUID_SAVAGE_ROAR, CastSpellExtraArgs(aurEff)
            .SetOriginalCaster(GetCasterGUID()));
    }

    void AfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->RemoveAurasDueToSpell(SPELL_DRUID_SAVAGE_ROAR);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_dru_savage_roar_aura::AfterApply, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        AfterEffectRemove += AuraEffectRemoveFn(spell_dru_savage_roar_aura::AfterRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 106839 - Skull Bash
class spell_dru_skull_bash : public SpellScript
{
    PrepareSpellScript(spell_dru_skull_bash);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_SKULL_BASH_CHARGE, SPELL_DRUID_SKULL_BASH_INTERRUPT });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        GetCaster()->CastSpell(GetHitUnit(), SPELL_DRUID_SKULL_BASH_CHARGE, true);
        GetCaster()->CastSpell(GetHitUnit(), SPELL_DRUID_SKULL_BASH_INTERRUPT, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_dru_skull_bash::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 106898 - Stampeding Roar
class spell_dru_stampeding_roar : public spell_dru_base_transformer
{
protected:
    bool ToCatForm() const override { return false; }
};

// 50286 - Starfall (Dummy)
class spell_dru_starfall_dummy : public SpellScript
{
    PrepareSpellScript(spell_dru_starfall_dummy);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        Trinity::Containers::RandomResize(targets, 2);
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        // Shapeshifting into an animal form or mounting cancels the effect
        if (caster->GetCreatureType() == CREATURE_TYPE_BEAST || caster->IsMounted())
        {
            if (SpellInfo const* spellInfo = GetTriggeringSpell())
                caster->RemoveAurasDueToSpell(spellInfo->Id);
            return;
        }

        // Any effect which causes you to lose control of your character will supress the starfall effect.
        if (caster->HasUnitState(UNIT_STATE_CONTROLLED))
            return;

        caster->CastSpell(GetHitUnit(), uint32(GetEffectValue()), true);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dru_starfall_dummy::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
        OnEffectHitTarget += SpellEffectFn(spell_dru_starfall_dummy::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

//  93402 - Sunfire
class spell_dru_sunfire : public SpellScript
{
    PrepareSpellScript(spell_dru_sunfire);

    void HandleOnHit(SpellEffIndex /*effIndex*/)
    {
        GetCaster()->CastSpell(GetHitUnit(), SPELL_DRUID_SUNFIRE_DAMAGE, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_dru_sunfire::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 61336 - Survival Instincts
class spell_dru_survival_instincts : public AuraScript
{
    PrepareAuraScript(spell_dru_survival_instincts);

    bool Validate(SpellInfo const* /*spell*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_SURVIVAL_INSTINCTS });
    }

    void AfterApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->CastSpell(GetTarget(), SPELL_DRUID_SURVIVAL_INSTINCTS, true);
    }

    void AfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->RemoveAurasDueToSpell(SPELL_DRUID_SURVIVAL_INSTINCTS);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_dru_survival_instincts::AfterApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        AfterEffectRemove += AuraEffectRemoveFn(spell_dru_survival_instincts::AfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 40121 - Swift Flight Form (Passive)
class spell_dru_swift_flight_passive : public AuraScript
{
    PrepareAuraScript(spell_dru_swift_flight_passive);

    bool Load() override
    {
        return GetCaster()->GetTypeId() == TYPEID_PLAYER;
    }

    void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
    {
        if (Player* caster = GetCaster()->ToPlayer())
            if (caster->GetSkillValue(SKILL_RIDING) >= 375)
                amount = 310;
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dru_swift_flight_passive::CalculateAmount, EFFECT_1, SPELL_AURA_MOD_INCREASE_VEHICLE_FLIGHT_SPEED);
    }
};

// 28744 - Regrowth
class spell_dru_t3_6p_bonus : public AuraScript
{
    PrepareAuraScript(spell_dru_t3_6p_bonus);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_BLESSING_OF_THE_CLAW });
    }

    void HandleProc(AuraEffect* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        eventInfo.GetActor()->CastSpell(eventInfo.GetProcTarget(), SPELL_DRUID_BLESSING_OF_THE_CLAW, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_dru_t3_6p_bonus::HandleProc, EFFECT_0, SPELL_AURA_OVERRIDE_CLASS_SCRIPTS);
    }
};

// 28719 - Healing Touch
class spell_dru_t3_8p_bonus : public AuraScript
{
    PrepareAuraScript(spell_dru_t3_8p_bonus);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_EXHILARATE });
    }

    void HandleProc(AuraEffect* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        Spell const* spell = eventInfo.GetProcSpell();
        if (!spell)
            return;

        Unit* caster = eventInfo.GetActor();
        std::vector<SpellPowerCost> const& costs = spell->GetPowerCost();
        auto m = std::find_if(costs.begin(), costs.end(), [](SpellPowerCost const& cost) { return cost.Power == POWER_MANA; });
        if (m == costs.end())
            return;

        int32 amount = CalculatePct(m->Amount, aurEff->GetAmount());
        CastSpellExtraArgs args(aurEff);
        args.AddSpellBP0(amount);
        caster->CastSpell(nullptr, SPELL_DRUID_EXHILARATE, args);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_dru_t3_8p_bonus::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 37288 - Mana Restore
// 37295 - Mana Restore
class spell_dru_t4_2p_bonus : public AuraScript
{
    PrepareAuraScript(spell_dru_t4_2p_bonus);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_INFUSION });
    }

    void HandleProc(AuraEffect* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        eventInfo.GetActor()->CastSpell(nullptr, SPELL_DRUID_INFUSION, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_dru_t4_2p_bonus::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 70723 - Item - Druid T10 Balance 4P Bonus
class spell_dru_t10_balance_4p_bonus : public AuraScript
{
    PrepareAuraScript(spell_dru_t10_balance_4p_bonus);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_LANGUISH });
    }

    void HandleProc(AuraEffect* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        DamageInfo* damageInfo = eventInfo.GetDamageInfo();
        if (!damageInfo || !damageInfo->GetDamage())
            return;

        Unit* caster = eventInfo.GetActor();
        Unit* target = eventInfo.GetProcTarget();

        SpellInfo const* spellInfo = sSpellMgr->AssertSpellInfo(SPELL_DRUID_LANGUISH, GetCastDifficulty());
        int32 amount = CalculatePct(static_cast<int32>(damageInfo->GetDamage()), aurEff->GetAmount());

        ASSERT(spellInfo->GetMaxTicks() > 0);
        amount /= spellInfo->GetMaxTicks();

        CastSpellExtraArgs args(aurEff);
        args.AddSpellMod(SPELLVALUE_BASE_POINT0, amount);
        caster->CastSpell(target, SPELL_DRUID_LANGUISH, args);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_dru_t10_balance_4p_bonus::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 70691 - Item T10 Restoration 4P Bonus
class spell_dru_t10_restoration_4p_bonus : public SpellScript
{
    PrepareSpellScript(spell_dru_t10_restoration_4p_bonus);

    bool Load() override
    {
        return GetCaster()->GetTypeId() == TYPEID_PLAYER;
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        if (!GetCaster()->ToPlayer()->GetGroup())
        {
            targets.clear();
            targets.push_back(GetCaster());
        }
        else
        {
            targets.remove(GetExplTargetUnit());
            std::list<Unit*> tempTargets;
            for (std::list<WorldObject*>::const_iterator itr = targets.begin(); itr != targets.end(); ++itr)
                if ((*itr)->GetTypeId() == TYPEID_PLAYER && GetCaster()->IsInRaidWith((*itr)->ToUnit()))
                    tempTargets.push_back((*itr)->ToUnit());

            if (tempTargets.empty())
            {
                targets.clear();
                FinishCast(SPELL_FAILED_DONT_REPORT);
                return;
            }

            Unit* target = Trinity::Containers::SelectRandomContainerElement(tempTargets);
            targets.clear();
            targets.push_back(target);
        }
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dru_t10_restoration_4p_bonus::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
    }
};

// 70664 - Druid T10 Restoration 4P Bonus (Rejuvenation)
class spell_dru_t10_restoration_4p_bonus_dummy : public AuraScript
{
    PrepareAuraScript(spell_dru_t10_restoration_4p_bonus_dummy);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_REJUVENATION_T10_PROC });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        SpellInfo const* spellInfo = eventInfo.GetSpellInfo();
        if (!spellInfo || spellInfo->Id == SPELL_DRUID_REJUVENATION_T10_PROC)
            return false;

        HealInfo* healInfo = eventInfo.GetHealInfo();
        if (!healInfo || !healInfo->GetHeal())
            return false;

        Player* caster = eventInfo.GetActor()->ToPlayer();
        if (!caster)
            return false;

        return caster->GetGroup() || caster != eventInfo.GetProcTarget();
    }

    void HandleProc(AuraEffect* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        CastSpellExtraArgs args(aurEff);
        args.AddSpellMod(SPELLVALUE_BASE_POINT0, eventInfo.GetHealInfo()->GetHeal());
        eventInfo.GetActor()->CastSpell(nullptr, SPELL_DRUID_REJUVENATION_T10_PROC, args);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_dru_t10_restoration_4p_bonus_dummy::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_dru_t10_restoration_4p_bonus_dummy::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 77758 - Thrash
class spell_dru_thrash : public SpellScript
{
    PrepareSpellScript(spell_dru_thrash);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_THRASH_BEAR_AURA });
    }

    void HandleOnHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (Unit* hitUnit = GetHitUnit())
        {
            Unit* caster = GetCaster();

            caster->CastSpell(hitUnit, SPELL_DRUID_THRASH_BEAR_AURA, TRIGGERED_FULL_MASK);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_dru_thrash::HandleOnHitTarget, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// 192090 - Thrash (Aura) - SPELL_DRUID_THRASH_BEAR_AURA
class spell_dru_thrash_aura : public AuraScript
{
    PrepareAuraScript(spell_dru_thrash_aura);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_BLOOD_FRENZY_AURA, SPELL_DRUID_BLOOD_FRENZY_RAGE_GAIN });
    }

    void HandlePeriodic(AuraEffect const* /*aurEff*/)
    {
        if (Unit* caster = GetCaster())
            if (caster->HasAura(SPELL_DRUID_BLOOD_FRENZY_AURA))
                caster->CastSpell(caster, SPELL_DRUID_BLOOD_FRENZY_RAGE_GAIN, true);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_dru_thrash_aura::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
    }
};

// 1066 - Aquatic Form
// 33943 - Flight Form
// 40120 - Swift Flight Form
// 165961 - Stag Form
class spell_dru_travel_form : public AuraScript
{
    PrepareAuraScript(spell_dru_travel_form);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_FORM_STAG, SPELL_DRUID_FORM_AQUATIC_PASSIVE, SPELL_DRUID_FORM_AQUATIC, SPELL_DRUID_FORM_FLIGHT, SPELL_DRUID_FORM_SWIFT_FLIGHT });
    }

    bool Load() override
    {
        return GetCaster()->GetTypeId() == TYPEID_PLAYER;
    }

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        // If it stays 0, it removes Travel Form dummy in AfterRemove.
        triggeredSpellId = 0;

        // We should only handle aura interrupts.
        if (GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_INTERRUPT)
            return;

        // Check what form is appropriate
        triggeredSpellId = GetFormSpellId(GetTarget()->ToPlayer(), GetCastDifficulty(), true);

        // If chosen form is current aura, just don't remove it.
        if (triggeredSpellId == m_scriptSpellId)
            PreventDefaultAction();
    }

    void AfterRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        if (triggeredSpellId == m_scriptSpellId)
            return;

        Player* player = GetTarget()->ToPlayer();

        if (triggeredSpellId) // Apply new form
            player->CastSpell(player, triggeredSpellId, aurEff);
        else // If not set, simply remove Travel Form dummy
            player->RemoveAura(SPELL_DRUID_TRAVEL_FORM);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_dru_travel_form::OnRemove, EFFECT_0, SPELL_AURA_MOD_SHAPESHIFT, AURA_EFFECT_HANDLE_REAL);
        AfterEffectRemove += AuraEffectRemoveFn(spell_dru_travel_form::AfterRemove, EFFECT_0, SPELL_AURA_MOD_SHAPESHIFT, AURA_EFFECT_HANDLE_REAL);
    }

public:
    static uint32 GetFormSpellId(Player const* player, Difficulty difficulty, bool requiresOutdoor)
    {
        // Check what form is appropriate
        if (player->HasSpell(SPELL_DRUID_FORM_AQUATIC_PASSIVE) && player->IsInWater()) // Aquatic form
            return SPELL_DRUID_FORM_AQUATIC;

        if (!player->IsInCombat() && player->GetSkillValue(SKILL_RIDING) >= 225 && CheckLocationForForm(player, difficulty, requiresOutdoor, SPELL_DRUID_FORM_FLIGHT) == SPELL_CAST_OK) // Flight form
            return player->GetSkillValue(SKILL_RIDING) >= 300 ? SPELL_DRUID_FORM_SWIFT_FLIGHT : SPELL_DRUID_FORM_FLIGHT;

        if (!player->IsInWater() && CheckLocationForForm(player, difficulty, requiresOutdoor, SPELL_DRUID_FORM_STAG) == SPELL_CAST_OK) // Stag form
            return SPELL_DRUID_FORM_STAG;

        return 0;
    }

private:
    static SpellCastResult CheckLocationForForm(Player const* targetPlayer, Difficulty difficulty, bool requireOutdoors, uint32 spell_id)
    {
        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spell_id, difficulty);

        if (requireOutdoors && !targetPlayer->IsOutdoors())
            return SPELL_FAILED_ONLY_OUTDOORS;

        return spellInfo->CheckLocation(targetPlayer->GetMapId(), targetPlayer->GetZoneId(), targetPlayer->GetAreaId(), targetPlayer);
    }

    uint32 triggeredSpellId = 0;
};

// 783 - Travel Form (dummy)
class spell_dru_travel_form_dummy : public SpellScript
{
    PrepareSpellScript(spell_dru_travel_form_dummy);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_FORM_AQUATIC_PASSIVE, SPELL_DRUID_FORM_AQUATIC, SPELL_DRUID_FORM_STAG });
    }

    SpellCastResult CheckCast()
    {
        Player* player = GetCaster()->ToPlayer();
        if (!player)
            return SPELL_FAILED_CUSTOM_ERROR;

        uint32 spellId = (player->HasSpell(SPELL_DRUID_FORM_AQUATIC_PASSIVE) && player->IsInWater()) ? SPELL_DRUID_FORM_AQUATIC : SPELL_DRUID_FORM_STAG;

        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId, GetCastDifficulty());
        return spellInfo->CheckLocation(player->GetMapId(), player->GetZoneId(), player->GetAreaId(), player);
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_dru_travel_form_dummy::CheckCast);
    }
};

class spell_dru_travel_form_dummy_aura : public AuraScript
{
    PrepareAuraScript(spell_dru_travel_form_dummy_aura);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_FORM_STAG, SPELL_DRUID_FORM_AQUATIC, SPELL_DRUID_FORM_FLIGHT, SPELL_DRUID_FORM_SWIFT_FLIGHT });
    }

    bool Load() override
    {
        return GetCaster()->GetTypeId() == TYPEID_PLAYER;
    }

    void OnApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        Player* player = GetTarget()->ToPlayer();

        // Outdoor check already passed - Travel Form (dummy) has SPELL_ATTR0_OUTDOORS_ONLY attribute.
        uint32 triggeredSpellId = spell_dru_travel_form::GetFormSpellId(player, GetCastDifficulty(), false);

        player->CastSpell(player, triggeredSpellId, aurEff);

        uint32 SoundID = SOUND_STAG_FORM;

        ShapeshiftForm CurrentForm = player->GetShapeshiftForm();
        switch (CurrentForm)
        {
        case FORM_CAT_FORM:
            SoundID = SOUND_CAT_FORM;
            break;
        case FORM_AQUATIC_FORM:
            SoundID = SOUND_AQUATIC_FORM;
            break;
        case FORM_DIRE_BEAR_FORM:
        case FORM_BEAR_FORM:
            SoundID = SOUND_DIRE_BEAR_FORM;
            break;
        case FORM_FLIGHT_FORM_EPIC:
        case FORM_FLIGHT_FORM:
            SoundID = SOUND_FLIGHT_FORM;
            break;
        case FORM_MOONKIN_FORM_RESTORATION:
        case FORM_MOONKIN_FORM:
            SoundID = SOUND_MOONKIN_FORM;
            break;
        default:
            break;
        }

        player->PlayDirectSound(SoundID);
    }

    void AfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        // No need to check remove mode, it's safe for auras to remove each other in AfterRemove hook.
        GetTarget()->RemoveAura(SPELL_DRUID_FORM_STAG);
        GetTarget()->RemoveAura(SPELL_DRUID_FORM_AQUATIC);
        GetTarget()->RemoveAura(SPELL_DRUID_FORM_FLIGHT);
        GetTarget()->RemoveAura(SPELL_DRUID_FORM_SWIFT_FLIGHT);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_dru_travel_form_dummy_aura::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        AfterEffectRemove += AuraEffectRemoveFn(spell_dru_travel_form_dummy_aura::AfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 252216 - Tiger Dash
class spell_dru_tiger_dash : public spell_dru_base_transformer
{
protected:
    bool ToCatForm() const override { return true; }
};

// 252216 - Tiger Dash (Aura)
class spell_dru_tiger_dash_aura : public AuraScript
{
    PrepareAuraScript(spell_dru_tiger_dash_aura);

    void HandlePeriodic(AuraEffect const* aurEff)
    {
        if (AuraEffect* effRunSpeed = GetEffect(EFFECT_0))
        {
            int32 reduction = aurEff->GetAmount();
            effRunSpeed->ChangeAmount(effRunSpeed->GetAmount() - reduction);
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_dru_tiger_dash_aura::HandlePeriodic, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 48438 - Wild Growth
class spell_dru_wild_growth : public SpellScript
{
    PrepareSpellScript(spell_dru_wild_growth);

    bool Validate(SpellInfo const* spellInfo) override
    {
        if (!ValidateSpellEffect({ { spellInfo->Id, EFFECT_2 } }) || spellInfo->GetEffect(EFFECT_2).IsEffect() || spellInfo->GetEffect(EFFECT_2).CalcValue() <= 0)
            return false;
        return true;
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if(RaidCheck(GetCaster()));

        uint32 const maxTargets = uint32(GetEffectInfo(EFFECT_2).CalcValue(GetCaster()));

        if (targets.size() > maxTargets)
        {
            targets.sort(Trinity::HealthPctOrderPred());
            targets.resize(maxTargets);
        }

        _targets = targets;
    }

    void SetTargets(std::list<WorldObject*>& targets)
    {
        targets = _targets;
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dru_wild_growth::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dru_wild_growth::SetTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ALLY);
    }

    std::list<WorldObject*> _targets;
};

class spell_dru_wild_growth_aura : public AuraScript
{
    PrepareAuraScript(spell_dru_wild_growth_aura);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_RESTORATION_T10_2P_BONUS });
    }

    void HandleTickUpdate(AuraEffect* aurEff)
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        // calculate from base damage, not from aurEff->GetAmount() (already modified)
        float damage = caster->CalculateSpellDamage(GetUnitOwner(), aurEff->GetSpellEffectInfo());

        // Wild Growth = first tick gains a 6% bonus, reduced by 2% each tick
        float reduction = 2.f;
        if (AuraEffect* bonus = caster->GetAuraEffect(SPELL_DRUID_RESTORATION_T10_2P_BONUS, EFFECT_0))
            reduction -= CalculatePct(reduction, bonus->GetAmount());
        reduction *= (aurEff->GetTickNumber() - 1);

        AddPct(damage, 6.f - reduction);
        aurEff->SetAmount(int32(damage));
    }

    void Register() override
    {
        OnEffectUpdatePeriodic += AuraEffectUpdatePeriodicFn(spell_dru_wild_growth_aura::HandleTickUpdate, EFFECT_0, SPELL_AURA_PERIODIC_HEAL);
    }
};

// 339 - Entangling Roots
// 102359 - Mass Entanglement
class spell_dru_entangling_roots : public AuraScript
{
    PrepareAuraScript(spell_dru_entangling_roots);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_CURIOUS_BRAMBLEPATCH, SPELL_DRUID_ENTANGLING_ROOTS, SPELL_DRUID_MASS_ENTANGLEMENT });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (SpellInfo const* spellInfo = eventInfo.GetSpellInfo())
        {
            // dont subtract dmg caused by roots from dmg required to break root
            if (spellInfo->Id == SPELL_DRUID_ENTANGLING_ROOTS || spellInfo->Id == SPELL_DRUID_MASS_ENTANGLEMENT)
                return false;
        }
        return true;
    }

    void HandleEffectPeriodic(AuraEffect const* /*aurEff*/)
    {
        if (Unit* caster = GetCaster())
        {
            if (!caster->HasAura(SPELL_DRUID_CURIOUS_BRAMBLEPATCH))
                PreventDefaultAction();
        }
    }

    void HandleApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (GetCaster()->HasAura(132158))
            GetCaster()->RemoveAura(132158);
    }

    void Register() override
    {
        OnEffectAttemptApply += AuraEffectApplyFn(spell_dru_entangling_roots::HandleApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        DoCheckProc += AuraCheckProcFn(spell_dru_entangling_roots::CheckProc);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_dru_entangling_roots::HandleEffectPeriodic, EFFECT_1, SPELL_AURA_PERIODIC_DAMAGE);
    }
};

// 383192 - Grove Tending
class spell_dru_grove_trending : public AuraScript
{
    PrepareAuraScript(spell_dru_grove_trending);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_GROVE_TRENDING_TRIGGER, SPELL_DRUID_GROVE_TRENDING });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        SpellInfo const* spellInfo = eventInfo.GetSpellInfo();
        if (!spellInfo || spellInfo->Id != SPELL_DRUID_SWIFTMEND)
            return false;

        return true;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_dru_grove_trending::CheckProc);
    }
};

// 340694 - Sudden Ambush
// 384667 - Sudden Ambush
class spell_dru_sudden_ambush : public AuraScript
{
    PrepareAuraScript(spell_dru_sudden_ambush);

    bool CheckProc(AuraEffect const* aurEff, ProcEventInfo& procInfo)
    {
        Spell const* procSpell = procInfo.GetProcSpell();
        if (!procSpell)
            return false;

        Optional<int32> comboPoints = procSpell->GetPowerTypeCostAmount(POWER_COMBO_POINTS);
        if (!comboPoints)
            return false;

        return roll_chance_i(*comboPoints * aurEff->GetAmount());
    }

    void Register() override
    {
        DoCheckEffectProc += AuraCheckEffectProcFn(spell_dru_sudden_ambush::CheckProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 164815 - Sunfire
// 164812 - Moonfire
class spell_dru_shooting_stars : public AuraScript
{
    PrepareAuraScript(spell_dru_shooting_stars);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_SHOOTING_STARS, SPELL_DRUID_SHOOTING_STARS_DAMAGE });
    }

    void OnTick(AuraEffect const* /*aurEff*/)
    {
        if (Unit* caster = GetCaster())
            if (AuraEffect const* shootingStars = caster->GetAuraEffect(SPELL_DRUID_SHOOTING_STARS, EFFECT_0))
                if (roll_chance_i(shootingStars->GetAmount()))
                    caster->CastSpell(GetTarget(), SPELL_DRUID_SHOOTING_STARS_DAMAGE, true);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_dru_shooting_stars::OnTick, EFFECT_1, SPELL_AURA_PERIODIC_DAMAGE);
    }
};

void AddSC_DekkCore_druid_spell_scripts()
{
    RegisterSpellScript(spell_dru_efflorescence);
    RegisterSpellScript(spell_dru_efflorescence_aura);
    RegisterSpellScript(spell_dru_efflorescence_heal);
    new spell_dru_primal_fury();
    new spell_dru_predatory_swiftness();
    new spell_dru_predatory_swiftness_aura();
    new spell_dru_balance_affinity_dps();
    new spell_dru_balance_affinity_resto();
    new spell_dru_incarnation_chosen_of_elune();
    new spell_dru_incarnation_king_of_the_jungle();
    new spell_dru_incarnation_guardian_of_ursoc();
    new spell_dru_wild_charge_moonkin();
    new spell_dru_bear_form();
    new spell_dru_activate_cat_form();
    new spell_dru_infected_wound();
    RegisterSpellScript(spell_dru_yseras_gift);
    RegisterSpellScript(spell_dru_yseras_gift_group_heal);
    RegisterSpellScript(spell_dru_rake);
    RegisterSpellScript(spell_dru_maim);
    RegisterSpellScript(aura_dru_charm_woodland_creature);
    RegisterSpellScript(spell_dru_swipe);
    RegisterSpellScript(spell_dru_brutal_slash);
    RegisterSpellScript(spell_dru_thrash_cat);
    RegisterSpellScript(spell_dru_shred);
    RegisterSpellScript(spell_dru_maul_bear);
    RegisterSpellScript(spell_dru_blessing_of_the_ancients);
    RegisterSpellScript(aura_dru_astral_form);
    RegisterSpellScript(aura_dru_restoration_affinity);
    RegisterSpellScript(aura_dru_feral_affinity);
    RegisterSpellScript(aura_dru_frenzied_regeneration);
    RegisterAreaTriggerAI(at_dru_starfall);
    RegisterCreatureAI(npc_dru_efflorescence);
    RegisterSpellScript(spell_dru_regrowth);
    RegisterSpellScript(spell_dru_rebirth);
    new spell_dru_flourish();
    new spell_dru_swiftmend();
    new spell_dru_ursols_vortex();
    new spell_dru_cenarion_ward();
    new spell_dru_cenarion_ward_hot();
    new spell_dru_glyph_of_starfire_proc();
    new spell_dru_typhoon();
    new spell_dru_primal_vitality();
    new spell_dru_nourish();
    new spell_dru_denmother();
    new spell_dru_overgrowth();
    new spell_dru_skull_bash_charge();
    new spell_dru_tranquility_heal();
    new spell_dru_cultivation();
    new spell_dru_rejuvenation();
    new spell_dru_infected_wounds();
    RegisterSpellScript(spell_dru_blessing_of_elune);
    new spell_dru_essence_of_ghanir();
    new spell_dru_rage_of_the_sleeper();
    RegisterSpellScript(spell_druid_lunar_strike);
    RegisterSpellScript(spell_druid_solar_wrath);
    RegisterSpellScript(spell_druid_earthwarden_triggered);
    RegisterSpellScript(spell_druid_earthwarden);
    RegisterSpellScript(spell_druid_pulverize);
    RegisterSpellScript(spell_druid_rend_and_tear);
    RegisterSpellScript(incarnation_tree_of_life);
    RegisterSpellScript(spell_feral_frenzy);
    RegisterPlayerScript(feral_spells);
    RegisterSpellScript(spell_dru_starsurge);
    RegisterSpellScript(spell_dru_starfall);
    RegisterSpellScript(spell_dru_photosynthesis);
    RegisterAreaTriggerAI(at_dru_lunar_beam);
    RegisterAreaTriggerAI(at_dru_ursol_vortex);
    RegisterSpellScript(spell_dru_mass_entanglement);
    RegisterPlayerScript(dru_predator);
    new spell_dru_teleport_moonglade();
    RegisterSpellScript(spell_dru_natures_balance);
    RegisterSpellScript(spell_dru_barkskin);
    RegisterSpellScript(spell_dru_berserk);
    RegisterSpellScript(spell_dru_brambles);
    RegisterSpellScript(spell_dru_bristling_fur);
    RegisterSpellScript(spell_dru_cat_form);
    RegisterSpellScript(spell_dru_dash);
    RegisterSpellScript(spell_dru_earthwarden);
    RegisterSpellScript(spell_dru_eclipse_aura);
    RegisterSpellScript(spell_dru_eclipse_dummy);
    RegisterSpellScript(spell_dru_eclipse_ooc);
    RegisterSpellScript(spell_dru_entangling_roots);
    RegisterSpellScript(spell_dru_grove_trending);
    RegisterSpellScript(spell_dru_ferocious_bite);
    RegisterSpellScript(spell_dru_forms_trinket);
    RegisterSpellScript(spell_dru_galactic_guardian);
    RegisterSpellScript(spell_dru_glyph_of_stars);
    RegisterSpellScript(spell_dru_gore);
    RegisterSpellScript(spell_dru_incapacitating_roar);
    RegisterSpellScript(spell_dru_innervate);
    RegisterSpellScript(spell_dru_item_t6_trinket);
    RegisterSpellScript(spell_dru_lifebloom);
    RegisterSpellScript(spell_dru_lunar_inspiration);
    RegisterAuraScript(spell_dru_omen_of_clarity_talent);
    RegisterSpellScript(spell_dru_moonfire);
    RegisterSpellScript(spell_dru_omen_of_clarity);
    RegisterSpellScript(spell_dru_prowl);
    RegisterSpellScript(spell_dru_rip);
    RegisterSpellAndAuraScriptPair(spell_dru_savage_roar, spell_dru_savage_roar_aura);
    RegisterSpellScript(spell_dru_skull_bash);
    RegisterSpellScript(spell_dru_stampeding_roar);
    RegisterSpellScript(spell_dru_starfall_dummy);
    RegisterSpellScript(spell_dru_sunfire);
    RegisterSpellScript(spell_dru_survival_instincts);
    RegisterSpellScript(spell_dru_swift_flight_passive);
    RegisterSpellScript(spell_dru_t3_6p_bonus);
    RegisterSpellScript(spell_dru_t3_8p_bonus);
    RegisterSpellScript(spell_dru_t4_2p_bonus);
    RegisterSpellScript(spell_dru_t10_balance_4p_bonus);
    RegisterSpellScript(spell_dru_t10_restoration_4p_bonus);
    RegisterSpellScript(spell_dru_t10_restoration_4p_bonus_dummy);
    RegisterSpellScript(spell_dru_thrash);
    RegisterSpellScript(spell_dru_thrash_aura);
    RegisterSpellScript(spell_dru_travel_form);
    RegisterSpellAndAuraScriptPair(spell_dru_travel_form_dummy, spell_dru_travel_form_dummy_aura);
    RegisterSpellAndAuraScriptPair(spell_dru_tiger_dash, spell_dru_tiger_dash_aura);
    RegisterSpellAndAuraScriptPair(spell_dru_wild_growth, spell_dru_wild_growth_aura);
    RegisterSpellScript(spell_dru_sudden_ambush);
    RegisterSpellScript(spell_dru_shooting_stars);
}
