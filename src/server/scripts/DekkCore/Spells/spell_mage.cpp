/*
 * This file is part of the DekkCore Project.
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
 * Scripts for spells with SPELLFAMILY_MAGE and SPELLFAMILY_GENERIC spells used by mage players.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_mage_".
 */

#include "AreaTrigger.h"
#include "AreaTriggerAI.h"
#include "Creature.h"
#include "CombatAI.h"
#include "EventMap.h"
#include "GridNotifiers.h"
#include "Group.h"
#include "MotionMaster.h"
#include "ObjectAccessor.h"
#include "PhasingHandler.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "SpellAuraEffects.h"
#include "SpellHistory.h"
#include "SpellMgr.h"
#include "SpellScript.h"
#include "TemporarySummon.h"

enum MageSpells
{
    SPELL_MAGE_COLD_SNAP = 235219,
    SPELL_MAGE_FROST_NOVA = 122,
    SPELL_MAGE_CONE_OF_COLD = 120,
    SPELL_MAGE_CONE_OF_COLD_SLOW = 212792,
    SPELL_MAGE_ICE_BARRIER = 11426,
    SPELL_MAGE_ICE_BLOCK = 45438,
    SPELL_MAGE_GLACIAL_INSULATION = 235297,
    SPELL_MAGE_BONE_CHILLING = 205027,
    SPELL_MAGE_BONE_CHILLING_BUFF = 205766,
    SPELL_MAGE_CHILLED = 205708,
    SPELL_MAGE_ICE_LANCE = 30455,
    SPELL_MAGE_ICE_LANCE_TRIGGER = 228598,
    SPELL_MAGE_THERMAL_VOID = 155149,
    SPELL_MAGE_ICY_VEINS = 12472,
    SPELL_MAGE_GLACIAL_SPIKE = 199786,
    SPELL_MAGE_ICICLE_PERIODIC_TRIGGER = 148023,
    SPELL_MAGE_FLURRY_DEBUFF_PROC = 228354,
    SPELL_MAGE_FLURRY = 44614,
    SPELL_MAGE_FLURRY_DAMAGE = 228672,
    SPELL_MAGE_FLURRY_CHILL_PROC = 228358,
    SPELL_MAGE_FLURRY_VISUAL = 228596,
    SPELL_MAGE_SHIELD_OF_ALODI = 195354,
    SPELL_MAGE_BRAIN_FREEZE = 190447,
    SPELL_MAGE_BRAIN_FREEZE_AURA = 190446,
    SPELL_MAGE_BRAIN_FREEZE_IMPROVED = 231584,
    SPELL_MAGE_EBONBOLT_DAMAGE = 257538,
    SPELL_MAGE_JOUSTER = 214626,
    SPELL_MAGE_CHAIN_REACTION = 195419,
    SPELL_MAGE_CHILLED_TO_THE_CORE = 195448,
    SPELL_MAGE_GLARITY_OF_THOUGHT = 195351,
    SPELL_MAGE_ICE_NOVA = 157997,
    SPELL_MAGE_FROZEN_TOUCH = 205030,
    SPELL_MAGE_FROZEN_ORB = 84714,
    SPELL_MAGE_FROZEN_ORB_DAMAGE = 84721,
    SPELL_MAGE_BLIZZARD_RANK_2 = 236662,
    SPELL_MAGE_UNSTABLE_MAGIC = 157976,
    SPELL_MAGE_UNSTABLE_MAGIC_DAMAGE_FIRE = 157977,
    SPELL_MAGE_UNSTABLE_MAGIC_DAMAGE_FROST = 157978,
    SPELL_MAGE_UNSTABLE_MAGIC_DAMAGE_ARCANE = 157979,
    SPELL_MAGE_FINGERS_OF_FROST = 112965,
    SPELL_MAGE_FINGERS_OF_FROST_AURA = 44544,
    SPELL_MAGE_FINGERS_OF_FROST_VISUAL_UI = 126084,
    SPELL_MAGE_FROST_BOMB_AURA = 112948,
    SPELL_MAGE_FROST_BOMB_TRIGGERED = 113092,
    SPELL_MAGE_FROSTBOLT = 116,
    SPELL_MAGE_FROSTBOLT_TRIGGER = 228597,
    SPELL_BLAZING_BARRIER_TRIGGER = 235314,
    SPELL_MAGE_SCORCH = 2948,
    SPELL_MAGE_FIREBALL = 133,
    SPELL_MAGE_FIRE_BLAST = 108853,
    SPELL_MAGE_FLAMESTRIKE = 2120,
    SPELL_MAGE_PYROBLAST = 11366,
    SPELL_MAGE_PHOENIX_FLAMES = 194466,
    SPELL_MAGE_DRAGON_BREATH = 31661,
    SPELL_MAGE_PYROMANIAC = 205020,
    SPELL_MAGE_ALEXSTRASZAS_FURY = 235870,
    SPELL_MAGE_LIVING_BOMB_DAMAGE = 44461,
    SPELL_MAGE_LIVING_BOMB_DOT = 217694,
    SPELL_MAGE_METEOR_DAMAGE = 153564,
    SPELL_MAGE_METEOR_TIMER = 177345,
    SPELL_MAGE_METEOR_VISUAL = 174556,
    SPELL_MAGE_METEOR_BURN = 155158,
    SPELL_MAGE_COMET_STORM = 153595,
    SPELL_MAGE_COMET_STORM_DAMAGE = 153596,
    SPELL_MAGE_COMET_STORM_VISUAL = 242210,
    SPELL_MAGE_POLYMORPH_CRITTERMORPH = 120091,
    SPELL_MAGE_HEATING_UP = 48107,
    SPELL_MAGE_HOT_STREAK = 48108,
    SPELL_MAGE_ENHANCED_PYROTECHNICS_AURA = 157644,
    SPELL_MAGE_INCANTERS_FLOW_BUFF = 116267,
    SPELL_MAGE_RUNE_OF_POWER_BUFF = 116014,
    SPELL_MAGE_OVERPOWERED = 155147,
    SPELL_MAGE_ARCANE_POWER = 12042,
    SPELL_MAGE_CHRONO_SHIFT = 235711,
    SPELL_MAGE_CHRONO_SHIFT_SLOW = 236299,
    SPELL_MAGE_CHRONO_SHIFT_BUFF = 236298,
    SPELL_MAGE_ARCANE_BLAST = 30451,
    SPELL_MAGE_ARCANE_BARRAGE = 44425,
    SPELL_MAGE_ARCANE_BARRAGE_TRIGGERED = 241241,
    SPELL_MAGE_PRESENCE_OF_MIND = 205025,
    SPELL_MAGE_ARCANE_MISSILES_VISUAL_TWO = 79808,
    SPELL_MAGE_ARCANE_MISSILES_VISUAL_ONE = 170571,
    SPELL_MAGE_ARCANE_MISSILES_VISUAL_THREE = 170572,
    SPELL_MAGE_ARCANE_MISSILES_TRIGGER = 7268,
    SPELL_MAGE_ARCANE_MISSILES = 5143,
    SPELL_MAGE_ARCANE_MISSILES_POWER = 208030,
    SPELL_MAGE_ARCANE_MISSILES_CHARGES = 79683,
    SPELL_MAGE_ARCANE_ORB_DAMAGE = 153640,
    SPELL_MAGE_ARCANE_AMPLIFICATION = 236628,
    SPELL_MAGE_RING_OF_FROST_FREEZE = 82691,
    SPELL_MAGE_RING_OF_FROST_IMMUNE = 91264,
    SPELL_MAGE_RING_OF_FROST = 113724,
    SPELL_MAGE_FIRE_MAGE_PASSIVE = 137019,
    SPELL_MAGE_FIRE_ON = 205029,
    SPELL_MAGE_FIRESTARTER = 205026,
    SPELL_MAGE_CAUTERIZE = 87023,
    SPELL_MAGE_MIRROR_IMAGE_SUMMON = 321686,
    SPELL_MAGE_COMBUSTION = 190319,
    SPELL_MAGE_WATER_JET = 135029,
    SPELL_MAGE_ICE_FLOES = 108839,
    SPELL_MAGE_CONJURE_REFRESHMENT_GROUP = 167145,
    SPELL_MAGE_CONJURE_REFRESHMENT_SOLO = 116136,
    SPELL_MAGE_HYPOTHERMIA = 41425,
    SPELL_INFERNO = 253220,
    SPELL_MAGE_BLAZING_BARRIER = 235313,
    SPELL_MAGE_BLAZING_SOUL = 235365,
    SPELL_MAGE_CONTROLLED_BURN = 205033,
    SPELL_MAGE_FLAME_PATCH = 205037,
    SPELL_MAGE_FLAME_PATCH_TRIGGER = 205470,
    SPELL_MAGE_FLAME_PATCH_AOE_DMG = 205472,
    SPELL_MAGE_CINDERSTORM = 198929,
    SPELL_MAGE_CINDERSTORM_DMG = 198928,
    SPELL_MAGE_IGNITE_DOT = 12654,
    SPELL_MAGE_REVERBERATE = 281482,
    SPELL_MAGE_RESONANCE = 205028,
    SPELL_MAGE_CLEARCASTING_BUFF = 277726, // Channel quicker
    SPELL_MAGE_CLEARCASTING_EFFECT = 263725, // Removing the costs
    SPELL_MAGE_CLEARCASTING_PVP_STACK_EFFECT = 276743, // Costs and is stackable
    SPELL_MAGE_ARCANE_EMPOWERMENT = 276741,
    SPELL_MAGE_MANA_SHIELD_TALENT = 235463,
    SPELL_MAGE_MANA_SHIELD_BURN = 235470,
    SPELL_MAGE_RULE_OF_THREES = 264354,
    SPELL_MAGE_RULE_OF_THREES_BUFF = 264774,
    SPELL_MAGE_SPLITTING_ICE = 56377,
    SPELL_ARCANE_CHARGE = 36032,
    SPELL_MAGE_SQUIRREL_FORM = 32813,
    SPELL_MAGE_GIRAFFE_FORM = 32816,
    SPELL_MAGE_SERPENT_FORM = 32817,
    SPELL_MAGE_DRAGONHAWK_FORM = 32818,
    SPELL_MAGE_WORGEN_FORM = 32819,
    SPELL_MAGE_SHEEP_FORM = 32820,
    //Azerite Traits
    SPELL_MAGE_WILDFIRE = 288755, //partially
    SPELL_BLASTER_MASTER = 274596, //ok
    SPELL_BLASTER_MASTER_MASTERY_BUFF = 274598,//ok
    SPELL_CAUTERIZING_BLINK_PROC = 280177,
    SPELL_IMPASSIVE_VISAGE_HEAL = 270117,
    SPELL_FIREMIND_TRIGGER = 278539, //ok
    SPELL_FIREMIND_MOD_INTELECT = 279715,//ok
    SPELL_PACKED_ICE_TRIGGER = 272968,
    SPELL_GLACIAL_ASSAULT_TRIGGER = 279854,//ok
    SPELL_HEART_OF_DARKNESS_TRIGGER = 317137,
    SPELL_EQUIPOISE_TRIGGER = 286027, //ok
    SPELL_EQUIPOISE_INCREASE_ARCANE_BLAST_DAMAGE = 264352, //ok
    SPELL_EQUIPOISE_REDUCE_MANA_COST_ARCANE_BLAST = 264353, //ok
    SPELL_FLASH_FREEZE_TRIGGER = 288164,
    SPELL_GUTRIPER_TRIGGER = 266937,
    SPELL_ARCANE_PUMMELING_TRIGGER = 270669,
    SPELL_ELEMENTAL_WHIRL_TRIGGER = 263984,
    SPELL_GALVANIZING_SPARK_TRIGGER = 278536,//not ok
    SPELL_OVERWHELMING_POWER_TRIGGER = 266180,
    SPELL_VAMPIRIC_SPEED_TRIGGER = 268599,
    SPELL_VAMPIRIC_SPEED_HEAL = 269238,
    SPELL_VAMPIRIC_SPEED_SPEED = 269239,
    SPELL_ELDRITCH_WARDING_TRIGGER = 274379,
    SPELL_BLOOD_SIPHON_TRIGGER = 264108, //ok
    SPELL_ARCANE_PRESSURE_TRIGGER = 274594, //ok
    SPELL_FLAMES_OF_ALACRITY_TRIGGER = 272932,
    SPELL_MAGE_PRISMATIC_CLOAK = 198064,
    SPELL_MAGE_PRISMATIC_CLOAK_BUFF = 198065,
    SPELL_MAGE_CHAIN_REACTION_BFA = 278309,
    SPELL_MAGE_CHAIN_REACTION_MOD_LANCE = 278310,
    SPELL_SNOWDRIFT = 389794,
    SPELL_FROZEN_IN_ICE = 71042,
    SPELL_ALTER_TIME = 108978,
    SPELL_MAGE_ICE_NINE = 214664,
    SPELL_MAGE_BLACK_ICE = 195615,
    SPELL_MAGE_ICICLE_DAMAGE = 148022,
    SPELL_MAGE_ICICLE_AURA = 205473,
    SPELL_MAGE_GLACIAL_SPIKE_PROC = 199844,
    SPELL_INCANTATION_OF_SWIFTNESS = 382293,
    SPELL_MAGE_INVISIBILITY = 66,
    SPELL_MAGE_FLASH_FREEZE = 379993,
    SPELL_MAGE_FREEZING_COLD = 386763,
    SPELL_MAGE_SHIFTING_POWER = 314791,
    SPELL_MAGE_ARCANE_SURGE = 365350,
    SPELL_MAGE_TIME_WARP = 80353,
    SPELL_MAGE_DISPLACEMENT = 389713,
    SPELL_MAGE_WILDFIRE_TALENT = 383489,
    SPELL_MAGE_WILDFIRE_TALENT_TRIGGER = 383490,
    SPELL_MAGE_WILDFIRE_SELF = 383492,
    SPELL_MAGE_WILDFIRE_OTHERS = 383493,
    SPELL_MAGE_ALTER_TIME_AURA = 110909,
    SPELL_MAGE_ALTER_TIME_VISUAL = 347402,
    SPELL_MAGE_ARCANE_ALTER_TIME_AURA = 342246,
    SPELL_MAGE_ARCANE_BARRAGE_ENERGIZE = 321529,
    SPELL_MAGE_ARCANE_BARRAGE_R3 = 321526,
    SPELL_MAGE_ARCANE_CHARGE = 36032,
    SPELL_MAGE_ARCANE_MAGE = 137021,
    SPELL_MAGE_BLAZING_BARRIER_TRIGGER = 235314,
    SPELL_MAGE_BLINK = 1953,
    SPELL_MAGE_BLIZZARD_DAMAGE = 190357,
    SPELL_MAGE_BLIZZARD_SLOW = 12486,
    SPELL_MAGE_CAUTERIZE_DOT = 87023,
    SPELL_MAGE_CAUTERIZED = 87024,
    SPELL_MAGE_CONJURE_REFRESHMENT = 116136,
    SPELL_MAGE_CONJURE_REFRESHMENT_TABLE = 167145,
    SPELL_MAGE_EVERWARM_SOCKS = 320913,
    SPELL_MAGE_IGNITE = 12654,
    SPELL_MAGE_INCANTERS_FLOW = 116267,
    SPELL_MAGE_LIVING_BOMB_EXPLOSION = 44461,
    SPELL_MAGE_LIVING_BOMB_PERIODIC = 217694,
    SPELL_MAGE_MANA_SURGE = 37445,
    SPELL_MAGE_MASTER_OF_TIME = 342249,
    SPELL_MAGE_RAY_OF_FROST_BONUS = 208141,
    SPELL_MAGE_RAY_OF_FROST_FINGERS_OF_FROST = 269748,
    SPELL_MAGE_RING_OF_FROST_DUMMY = 91264,
    SPELL_MAGE_RING_OF_FROST_SUMMON = 113724,
    SPELL_MAGE_CHAIN_REACTION_DUMMY = 278309,
    SPELL_MAGE_TOUCH_OF_THE_MAGI_EXPLODE = 210833,
    SPELL_MAGE_ETHEREAL_BLINK = 410939,
    SPELL_MAGE_FEEL_THE_BURN = 383391,
    SPELL_MAGE_RADIANT_SPARK_PROC_BLOCKER = 376105,
    SPELL_MAGE_SHIMMER = 212653,
    SPELL_MAGE_SLOW = 31589,
    SPELL_MAGE_SUPERNOVA = 157980,
    SplittingIce = 56377,
    IciclesStack = 205473,
    IciclesDamage = 148022,
    MasteryIcicles = 76613
};

enum TemporalDisplacementSpells
{
    SPELL_MAGE_TEMPORAL_DISPLACEMENT = 80354,
    SPELL_HUNTER_INSANITY = 95809,
    SPELL_PRIEST_SHADOW_WORD_DEATH = 32409,
    SPELL_SHAMAN_EXHAUSTION = 57723,
    SPELL_SHAMAN_SATED = 57724,
    SPELL_PET_NETHERWINDS_FATIGUED = 160455
};

class playerscript_mage_arcane : public PlayerScript
{
public:
    playerscript_mage_arcane() : PlayerScript("playerscript_mage_arcane") {}

    void OnModifyPower(Player* player, Powers power, int32 oldValue, int32& newValue, bool /*regen*/, bool after)
    {
        if (!after)
            return;

        if (power != POWER_ARCANE_CHARGES)
            return;

        // Going up in charges is handled by aura 190427
        // Decreasing power seems weird clientside does not always match serverside power amount (client stays at 1, server is at 0)
        if (newValue)
        {
            if (Aura* arcaneCharge = player->GetAura(SPELL_ARCANE_CHARGE))
                arcaneCharge->SetStackAmount(newValue);
        }
        else
            player->RemoveAurasDueToSpell(SPELL_ARCANE_CHARGE);

        if (player->HasAura(SPELL_MAGE_RULE_OF_THREES))
            if (newValue == 3 && oldValue == 2)
                player->CastSpell(player, SPELL_MAGE_RULE_OF_THREES_BUFF, true);
    }
};

// Chrono Shift - 235711
class spell_mage_chrono_shift : public AuraScript
{
    PrepareAuraScript(spell_mage_chrono_shift);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        bool _spellCanProc = (eventInfo.GetSpellInfo()->Id == SPELL_MAGE_ARCANE_BARRAGE || eventInfo.GetSpellInfo()->Id == SPELL_MAGE_ARCANE_BARRAGE_TRIGGERED);

        if (_spellCanProc)
            return true;
        return false;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_mage_chrono_shift::CheckProc);
    }
};

// Arcane Missiles - 5143
class spell_mage_arcane_missiles : public AuraScript
{
    PrepareAuraScript(spell_mage_arcane_missiles);

    void OnApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        //@TODO: Remove when proc system can handle arcane missiles.....
        caster->RemoveAura(SPELL_MAGE_CLEARCASTING_BUFF);
        caster->RemoveAura(SPELL_MAGE_CLEARCASTING_EFFECT);
        if (Aura* pvpClearcast = caster->GetAura(SPELL_MAGE_CLEARCASTING_PVP_STACK_EFFECT))
            pvpClearcast->ModStackAmount(-1);
        caster->RemoveAura(SPELL_MAGE_RULE_OF_THREES_BUFF);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_mage_arcane_missiles::OnApply, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
    }
};

// Arcane Missiles Damage - 7268
class spell_mage_arcane_missiles_damage :public SpellScript
{
    PrepareSpellScript(spell_mage_arcane_missiles_damage);

    void CheckTarget(WorldObject*& target)
    {
        if (target == GetCaster())
            target = nullptr;
    }

    void Register() override
    {
        OnObjectTargetSelect += SpellObjectTargetSelectFn(spell_mage_arcane_missiles_damage::CheckTarget, EFFECT_0, TARGET_UNIT_CHANNEL_TARGET);
    }
};

// Clearcasting - 79684
class spell_mage_clearcasting : public AuraScript
{
    PrepareAuraScript(spell_mage_clearcasting);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (int32 eff0 = GetSpellInfo()->GetEffect(EFFECT_0).CalcValue())
        {
            int32 reqManaToSpent = 0;
            int32 manaUsed = 0;

            // For each ${$c*100/$s1} mana you spend, you have a 1% chance
            // Means: I cast a spell which costs 1000 Mana, for every 500 mana used I have 1% chance =  2% chance to proc
            for (SpellPowerCost powerCost : GetSpellInfo()->CalcPowerCost(GetCaster(), GetSpellInfo()->GetSchoolMask()))
                if (powerCost.Power == POWER_MANA)
                    reqManaToSpent = powerCost.Amount * 100 / eff0;

            // Something changed in DBC, Clearcasting should cost 1% of base mana 8.0.1
            if (reqManaToSpent == 0)
                return false;

            for (SpellPowerCost powerCost : eventInfo.GetSpellInfo()->CalcPowerCost(GetCaster(), eventInfo.GetSpellInfo()->GetSchoolMask()))
                if (powerCost.Power == POWER_MANA)
                    manaUsed = powerCost.Amount;

            int32 chance = std::floor(manaUsed / reqManaToSpent) * 1;
            return roll_chance_i(chance);
        }

        return false;
    }

    void HandleProc(AuraEffect* /*aurEff*/, ProcEventInfo& eventInfo)
    {
        Unit* actor = eventInfo.GetActor();
        actor->CastSpell(actor, SPELL_MAGE_CLEARCASTING_BUFF, true);
        if (actor->HasAura(SPELL_MAGE_ARCANE_EMPOWERMENT))
            actor->CastSpell(actor, SPELL_MAGE_CLEARCASTING_PVP_STACK_EFFECT, true);
        else
            actor->CastSpell(actor, SPELL_MAGE_CLEARCASTING_EFFECT, true);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_mage_clearcasting::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_mage_clearcasting::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// Arcane Blast - 30451
class spell_mage_arcane_blast : public SpellScript
{
    PrepareSpellScript(spell_mage_arcane_blast);

    void DoCast()
    {
        if (Unit* caster = GetCaster())
            if (Aura* threes = caster->GetAura(SPELL_MAGE_RULE_OF_THREES_BUFF))
                threes->Remove();
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_mage_arcane_blast::DoCast);
    }
};

// Presence of mind - 205025
class spell_mage_presence_of_mind : public AuraScript
{
    PrepareAuraScript(spell_mage_presence_of_mind);

    bool HandleProc(ProcEventInfo& eventInfo)
    {
        if (eventInfo.GetSpellInfo()->Id == SPELL_MAGE_ARCANE_BLAST)
            return true;
        return false;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_mage_presence_of_mind::HandleProc);
    }
};

class CheckArcaneBarrageImpactPredicate
{
public:
    CheckArcaneBarrageImpactPredicate(Unit* caster, Unit* mainTarget) : _caster(caster), _mainTarget(mainTarget) {}

    bool operator()(Unit* target)
    {
        if (!_caster || !_mainTarget)
            return true;

        if (!_caster->IsValidAttackTarget(target))
            return true;

        if (!target->IsWithinLOSInMap(_caster))
            return true;

        if (!_caster->isInFront(target))
            return true;

        if (target->GetGUID() == _caster->GetGUID())
            return true;

        if (target->GetGUID() == _mainTarget->GetGUID())
            return true;

        return false;
    }

private:
    Unit* _caster;
    Unit* _mainTarget;
};

// Fire Blast - 108853
class spell_mage_fire_blast : public SpellScript
{
    PrepareSpellScript(spell_mage_fire_blast);

    void HandleHit(SpellEffIndex /*effIndex*/)
    {
        // this is already handled by Pyroblast Clearcasting Driver - 44448
        /*bool procCheck = false;

        if (Unit* caster = GetCaster())
        {
            if (!caster->HasAura(SPELL_MAGE_HEATING_UP) && !caster->HasAura(SPELL_MAGE_HOT_STREAK))
            {
                caster->CastSpell(caster, SPELL_MAGE_HEATING_UP, true);
                procCheck = true;
            }


            if (caster->HasAura(SPELL_MAGE_HEATING_UP) && !caster->HasAura(SPELL_MAGE_HOT_STREAK) && !procCheck)
            {
                caster->RemoveAurasDueToSpell(SPELL_MAGE_HEATING_UP);
                caster->CastSpell(caster, SPELL_MAGE_HOT_STREAK, true);
            }
        }*/
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_mage_fire_blast::HandleHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// Enhanced Pyrotechnics - 157642
class spell_mage_enhanced_pyrotechnics : public AuraScript
{
    PrepareAuraScript(spell_mage_enhanced_pyrotechnics);

    bool HandleProc(ProcEventInfo& eventInfo)
    {
        Unit* caster = GetCaster();

        if (eventInfo.GetSpellInfo()->Id == SPELL_MAGE_FIREBALL)
        {
            if ((eventInfo.GetHitMask() & PROC_HIT_CRITICAL))
            {
                if (caster->HasAura(SPELL_MAGE_ENHANCED_PYROTECHNICS_AURA))
                    caster->RemoveAurasDueToSpell(SPELL_MAGE_ENHANCED_PYROTECHNICS_AURA);
                return false;
            }
            return true;
        }
        return false;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_mage_enhanced_pyrotechnics::HandleProc);
    }
};

struct auraData
{
    auraData(uint32 id, int32 duration) : m_id(id), m_duration(duration) {}
    uint32 m_id;
    int32 m_duration;
};

const uint32 icicles[5][3]
{
    {148012, 148017, 148013},
    {148013, 148018, 148014},
    {148014, 148019, 148015},
    {148015, 148020, 148016},
    {148016, 148021, 148012}
};

// Meteor - 153561
class spell_mage_meteor : public SpellScript
{
    PrepareSpellScript(spell_mage_meteor);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_METEOR_DAMAGE });
    }

    void HandleDummy()
    {
        Unit* caster = GetCaster();
        WorldLocation const* dest = GetExplTargetDest();
        if (!caster || !dest)
            return;

        caster->CastSpell(Position(dest->GetPositionX(), dest->GetPositionY(), dest->GetPositionZ()), SPELL_MAGE_METEOR_TIMER, true);
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_mage_meteor::HandleDummy);
    }
};

// Meteor Damage - 153564
class spell_mage_meteor_damage : public SpellScript
{
    PrepareSpellScript(spell_mage_meteor_damage);

    int32 _targets;

    void HandleHit(SpellEffIndex /*effIndex*/)
    {
        Unit* unit = GetHitUnit();
        if (!unit)
            return;

        SetHitDamage(GetHitDamage() / _targets);
    }

    void CountTargets(std::list<WorldObject*>& targets)
    {
        _targets = targets.size();
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_mage_meteor_damage::HandleHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_mage_meteor_damage::CountTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
    }
};

// Frenetic Speed - 236058
class spell_mage_frenetic_speed : public AuraScript
{
    PrepareAuraScript(spell_mage_frenetic_speed);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return eventInfo.GetSpellInfo()->Id == SPELL_MAGE_SCORCH;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_mage_frenetic_speed::CheckProc);
    }
};

// Conflagration - 205023
class spell_mage_conflagration : public AuraScript
{
    PrepareAuraScript(spell_mage_conflagration);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return eventInfo.GetSpellInfo() && eventInfo.GetSpellInfo()->Id == SPELL_MAGE_FIREBALL;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_mage_conflagration::CheckProc);
    }
};

// Pyroblast 11366
class spell_mage_pyroblast : public SpellScript
{
    PrepareSpellScript(spell_mage_pyroblast);

    void HandleOnHit(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        if (caster->HasAura(SPELL_MAGE_HOT_STREAK))
        {
            caster->RemoveAurasDueToSpell(SPELL_MAGE_HOT_STREAK);

            if (caster->HasAura(SPELL_MAGE_PYROMANIAC))
                if (AuraEffect* pyromaniacEff0 = caster->GetAuraEffect(SPELL_MAGE_PYROMANIAC, EFFECT_0))
                    if (roll_chance_i(pyromaniacEff0->GetAmount()))
                    {
                        if (caster->HasAura(SPELL_MAGE_HEATING_UP))
                            caster->RemoveAurasDueToSpell(SPELL_MAGE_HEATING_UP);

                        caster->CastSpell(caster, SPELL_MAGE_HOT_STREAK, true);
                    }
        }
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_mage_pyroblast::HandleOnHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// Flamestrike 2120
class spell_mage_flamestrike : public SpellScript
{
    PrepareSpellScript(spell_mage_flamestrike);

    void HandleOnHit(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        if (caster->HasAura(SPELL_MAGE_HOT_STREAK))
        {
            caster->RemoveAurasDueToSpell(SPELL_MAGE_HOT_STREAK);

            if (caster->HasAura(SPELL_MAGE_PYROMANIAC))
                if (AuraEffect* pyromaniacEff0 = caster->GetAuraEffect(SPELL_MAGE_PYROMANIAC, EFFECT_0))
                    if (roll_chance_i(pyromaniacEff0->GetAmount()))
                    {
                        if (caster->HasAura(SPELL_MAGE_HEATING_UP))
                            caster->RemoveAurasDueToSpell(SPELL_MAGE_HEATING_UP);

                        caster->CastSpell(caster, SPELL_MAGE_HOT_STREAK, true);
                    }
        }
    }

    void HandleDummy()
    {
        Unit* caster = GetCaster();
        WorldLocation const* dest = GetExplTargetDest();
        if (!caster || !dest)
            return;

        if (caster->HasAura(SPELL_MAGE_FLAME_PATCH))
            if (WorldLocation const* dest = GetExplTargetDest())
                caster->CastSpell(dest->GetPosition(), SPELL_MAGE_FLAME_PATCH_TRIGGER, true);
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_mage_flamestrike::HandleOnHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        AfterCast += SpellCastFn(spell_mage_flamestrike::HandleDummy);
    }
};

// Kindling - 155148
class spell_mage_kindling : public AuraScript
{
    PrepareAuraScript(spell_mage_kindling);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return eventInfo.GetSpellInfo()->Id == SPELL_MAGE_FIREBALL || eventInfo.GetSpellInfo()->Id == SPELL_MAGE_FIRE_BLAST || eventInfo.GetSpellInfo()->Id == SPELL_MAGE_PHOENIX_FLAMES;
    }

    void HandleProc(AuraEffect* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        caster->GetSpellHistory()->ModifyCooldown(SPELL_MAGE_COMBUSTION, -Seconds(aurEff->GetAmount()));
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_mage_kindling::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_mage_kindling::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// Pyroblast Clearcasting Driver - 44448
class spell_mage_pyroblast_clearcasting_driver : public AuraScript
{
    PrepareAuraScript(spell_mage_pyroblast_clearcasting_driver);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        Unit* caster = GetCaster();

        bool _spellCanProc = (eventInfo.GetSpellInfo()->Id == SPELL_MAGE_SCORCH || eventInfo.GetSpellInfo()->Id == SPELL_MAGE_FIREBALL || eventInfo.GetSpellInfo()->Id == SPELL_MAGE_FIRE_BLAST || eventInfo.GetSpellInfo()->Id == SPELL_MAGE_FLAMESTRIKE || eventInfo.GetSpellInfo()->Id == SPELL_MAGE_PYROBLAST || eventInfo.GetSpellInfo()->Id == SPELL_MAGE_PHOENIX_FLAMES || (eventInfo.GetSpellInfo()->Id == SPELL_MAGE_DRAGON_BREATH && caster->HasAura(SPELL_MAGE_ALEXSTRASZAS_FURY)));

        if (_spellCanProc)
            return true;
        return false;
    }

    void HandleProc(AuraEffect* /*aurEff*/, ProcEventInfo& eventInfo)
    {
        bool procCheck = false;

        Unit* caster = GetCaster();

        if ((eventInfo.GetHitMask() & PROC_HIT_NORMAL))
        {
            if (caster->HasAura(SPELL_MAGE_HEATING_UP))
                caster->RemoveAurasDueToSpell(SPELL_MAGE_HEATING_UP);
            return;
        }

        if (!caster->HasAura(SPELL_MAGE_HEATING_UP) && !caster->HasAura(SPELL_MAGE_HOT_STREAK))
        {
            caster->CastSpell(caster, SPELL_MAGE_HEATING_UP, true);

            procCheck = true;

            if (AuraEffect* burn = caster->GetAuraEffect(SPELL_MAGE_CONTROLLED_BURN, EFFECT_0))
                if (roll_chance_i(burn->GetAmount()))
                    procCheck = false;
        }


        if (caster->HasAura(SPELL_MAGE_HEATING_UP) && !caster->HasAura(SPELL_MAGE_HOT_STREAK) && !procCheck)
        {
            caster->RemoveAurasDueToSpell(SPELL_MAGE_HEATING_UP);
            caster->CastSpell(caster, SPELL_MAGE_HOT_STREAK, true);
        }
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_mage_pyroblast_clearcasting_driver::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
        DoCheckProc += AuraCheckProcFn(spell_mage_pyroblast_clearcasting_driver::CheckProc);
    }
};

// Chilled - 205708
class spell_mage_chilled : public AuraScript
{
    PrepareAuraScript(spell_mage_chilled);

    void HandleApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        if (caster->HasAura(SPELL_MAGE_BONE_CHILLING))
        {
            //@TODO REDUCE BONE CHILLING DAMAGE PER STACK TO 0.5% from 1%
            caster->CastSpell(caster, SPELL_MAGE_BONE_CHILLING_BUFF, true);
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_mage_chilled::HandleApply, EFFECT_0, SPELL_AURA_MOD_DECREASE_SPEED, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
    }
};

// Flurry - 44614
class spell_mage_flurry : public SpellScript
{
    PrepareSpellScript(spell_mage_flurry);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        Unit* target = GetHitUnit();
        bool isImproved = false;
        if (!caster || !target)
            return;

        if (caster->HasAura(SPELL_MAGE_BRAIN_FREEZE_AURA))
        {
            caster->RemoveAura(SPELL_MAGE_BRAIN_FREEZE_AURA);
            if (caster->HasSpell(SPELL_MAGE_BRAIN_FREEZE_IMPROVED))
                isImproved = true;
        }

        ObjectGuid targetGuid = target->GetGUID();
        if (targetGuid != ObjectGuid::Empty)
            for (uint8 i = 1; i < 3; ++i) // basepoint value is 3 all the time, so, set it 3 because sometimes it won't read
            {
                caster->GetScheduler().Schedule(Milliseconds(i * 250), [targetGuid, isImproved](TaskContext context)
                {
                    if (Unit* caster = GetContextUnit())
                    {
                        if (Unit* target = ObjectAccessor::GetUnit(*caster, targetGuid))
                        {
                            caster->CastSpell(target, SPELL_MAGE_FLURRY_VISUAL, false);

                            if (isImproved)
                                caster->CastSpell(target, SPELL_MAGE_FLURRY_CHILL_PROC, false);
                        }
                    }
                });
            }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_mage_flurry::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Jouster - 214626
class spell_mage_jouster : public AuraScript
{
    PrepareAuraScript(spell_mage_jouster);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return eventInfo.GetSpellInfo()->Id == SPELL_MAGE_ICE_LANCE;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_mage_jouster::CheckProc);
    }
};

// Jouster Buff - 195391
class spell_mage_jouster_buff : public AuraScript
{
    PrepareAuraScript(spell_mage_jouster_buff);

    void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        if (AuraEffect* jousterRank = caster->GetAuraEffect(SPELL_MAGE_JOUSTER, EFFECT_0))
            amount = jousterRank->GetAmount();
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mage_jouster_buff::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN);
    }
};

// Chain Reaction - 195419
class spell_mage_chain_reaction : public AuraScript
{
    PrepareAuraScript(spell_mage_chain_reaction);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return eventInfo.GetSpellInfo()->Id == SPELL_MAGE_FROSTBOLT || eventInfo.GetSpellInfo()->Id == SPELL_MAGE_FROSTBOLT_TRIGGER;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_mage_chain_reaction::CheckProc);
    }
};

// Chilled to the Core - 195448
class spell_mage_chilled_to_the_core : public AuraScript
{
    PrepareAuraScript(spell_mage_chilled_to_the_core);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return eventInfo.GetSpellInfo()->Id == SPELL_MAGE_ICY_VEINS;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_mage_chilled_to_the_core::CheckProc);
    }
};

// Combustion - 190319
class spell_mage_combustion : public SpellScriptLoader
{
public:
    spell_mage_combustion() : SpellScriptLoader("spell_mage_combustion") {}

    class spell_mage_combustion_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_mage_combustion_AuraScript);

        void CalcAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
        {
            Unit* caster = GetCaster();
            if (!caster)
                return;

            if (!caster->IsPlayer())
                return;

            int32 crit = caster->ToPlayer()->GetRatingBonusValue(CR_CRIT_SPELL);
            amount += crit;
        }

        void HandleRemove(AuraEffect const* /*aurEffect*/, AuraEffectHandleModes /*mode*/)
        {
            GetCaster()->RemoveAurasDueToSpell(SPELL_INFERNO);
        }

        void Register() override
        {
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mage_combustion_AuraScript::CalcAmount, EFFECT_1, SPELL_AURA_MOD_RATING);
            OnEffectRemove += AuraEffectRemoveFn(spell_mage_combustion_AuraScript::HandleRemove, EFFECT_1, SPELL_AURA_MOD_RATING, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_mage_combustion_AuraScript();
    }
};

// Fire mage (passive) - 137019
class spell_mage_fire_mage_passive : public SpellScriptLoader
{
public:
    spell_mage_fire_mage_passive() : SpellScriptLoader("spell_mage_fire_mage_passive") {}

    class spell_mage_fire_mage_passive_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_mage_fire_mage_passive_AuraScript);

        bool Validate(SpellInfo const* /*spellInfo*/) override
        {
           // if (!sSpellMgr->GetSpellInfo(SPELL_MAGE_FIRE_MAGE_PASSIVE, DIFFICULTY_NONE) ||
            //    !sSpellMgr->GetSpellInfo(SPELL_MAGE_FIRE_BLAST, DIFFICULTY_NONE))
              //  return false;
            return true;
        }

    public:

        spell_mage_fire_mage_passive_AuraScript() {}

    private:

        SpellModifier* mod = nullptr;

        void HandleApply(AuraEffect const* aurEffect, AuraEffectHandleModes /*mode*/)
        {
            Player* player = GetCaster()->ToPlayer();
            if (!player)
                return;

            SpellModifierByClassMask* mod = new SpellModifierByClassMask(aurEffect->GetBase());
            mod->op = SpellModOp::CritChance;
            mod->type = SPELLMOD_FLAT;
            mod->spellId = SPELL_MAGE_FIRE_MAGE_PASSIVE;
            mod->value = 200;
            mod->mask[0] = 0x2;

            player->AddSpellMod(mod, true);
        }

        void HandleRemove(AuraEffect const* /*aurEffect*/, AuraEffectHandleModes /*mode*/)
        {
            Player* player = GetCaster()->ToPlayer();
            if (!player)
                return;

            if (mod)
                player->AddSpellMod(mod, false);
        }

        void Register() override
        {
            OnEffectApply += AuraEffectApplyFn(spell_mage_fire_mage_passive_AuraScript::HandleApply, EFFECT_4, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            OnEffectRemove += AuraEffectRemoveFn(spell_mage_fire_mage_passive_AuraScript::HandleRemove, EFFECT_4, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_mage_fire_mage_passive_AuraScript();
    }
};

// Flame On - 205029
class spell_mage_fire_on : public SpellScriptLoader
{
public:
    spell_mage_fire_on() : SpellScriptLoader("spell_mage_fire_on") { }

    class spell_mage_fire_on_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_mage_fire_on_SpellScript);

        bool Validate(SpellInfo const* /*spellInfo*/) override
        {
            return ValidateSpellInfo({
                    SPELL_MAGE_FIRE_ON,
                    SPELL_MAGE_FIRE_BLAST
                });
        }

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            Unit* caster = GetCaster();
            Unit* target = GetHitUnit();
            if (!caster || !target || caster->GetTypeId() != TYPEID_PLAYER)
                return;

           // caster->ToPlayer()->GetSpellHistory()->ResetCharges(sSpellMgr->GetSpellInfo(SPELL_MAGE_FIRE_BLAST, DIFFICULTY_NONE)->ChargeCategoryId);
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_mage_fire_on_SpellScript::HandleDummy, EFFECT_2, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_mage_fire_on_SpellScript();
    }
};

// Mirror Image - 55342
class spell_mage_mirror_image_summon : public SpellScriptLoader
{
public:
    spell_mage_mirror_image_summon() : SpellScriptLoader("spell_mage_mirror_image_summon") { }

    class spell_mage_mirror_image_summon_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_mage_mirror_image_summon_SpellScript);

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            if (Unit* caster = GetCaster())
            {
                caster->CastSpell(caster, SPELL_MAGE_MIRROR_IMAGE_SUMMON, true);
                caster->CastSpell(caster, SPELL_MAGE_MIRROR_IMAGE_SUMMON, true);
                caster->CastSpell(caster, SPELL_MAGE_MIRROR_IMAGE_SUMMON, true);
            }
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_mage_mirror_image_summon_SpellScript::HandleDummy, EFFECT_1, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_mage_mirror_image_summon_SpellScript();
    }
};

// Meteor - 177345
// AreaTriggerID - 3467
class at_mage_meteor_timer : public AreaTriggerEntityScript
{
public:
    at_mage_meteor_timer() : AreaTriggerEntityScript("at_mage_meteor_timer") {}

    struct at_mage_meteor_timerAI : AreaTriggerAI
    {
        at_mage_meteor_timerAI(AreaTrigger* areatrigger) : AreaTriggerAI(areatrigger) { }

        void OnCreate() override
        {
            Unit* caster = at->GetCaster();
            if (!caster)
                return;

            if (TempSummon* tempSumm = caster->SummonCreature(WORLD_TRIGGER, at->GetPosition(), TEMPSUMMON_TIMED_DESPAWN, 5s))
            {
                tempSumm->SetFaction(caster->GetFaction());
                tempSumm->SetSummonerGUID(caster->GetGUID());
                PhasingHandler::InheritPhaseShift(tempSumm, caster);
                caster->CastSpell(tempSumm, SPELL_MAGE_METEOR_VISUAL, true);
            }

        }

        void OnRemove() override
        {
            Unit* caster = at->GetCaster();
            if (!caster)
                return;

            if (TempSummon* tempSumm = caster->SummonCreature(WORLD_TRIGGER, at->GetPosition(), TEMPSUMMON_TIMED_DESPAWN, 5s))
            {
                tempSumm->SetFaction(caster->GetFaction());
                tempSumm->SetSummonerGUID(caster->GetGUID());
                PhasingHandler::InheritPhaseShift(tempSumm, caster);
                caster->CastSpell(tempSumm, SPELL_MAGE_METEOR_DAMAGE, true);
            }
        }
    };

    AreaTriggerAI* GetAI(AreaTrigger* areatrigger) const override
    {
        return new at_mage_meteor_timerAI(areatrigger);
    }
};

// Meteor Burn - 175396
// AreaTriggerID - 1712
class at_mage_meteor_burn : public AreaTriggerEntityScript
{
public:
    at_mage_meteor_burn() : AreaTriggerEntityScript("at_mage_meteor_burn") { }

    struct at_mage_meteor_burnAI : AreaTriggerAI
    {
        at_mage_meteor_burnAI(AreaTrigger* areatrigger) : AreaTriggerAI(areatrigger) { }

        void OnUnitEnter(Unit* unit) override
        {
            Unit* caster = at->GetCaster();

            if (!caster || !unit)
                return;

            if (caster->GetTypeId() != TYPEID_PLAYER)
                return;

            if (caster->IsValidAttackTarget(unit))
                caster->CastSpell(unit, SPELL_MAGE_METEOR_BURN, true);
        }

        void OnUnitExit(Unit* unit) override
        {
            Unit* caster = at->GetCaster();

            if (!caster || !unit)
                return;

            if (caster->GetTypeId() != TYPEID_PLAYER)
                return;

            if (Aura* meteor = unit->GetAura(SPELL_MAGE_METEOR_BURN, caster->GetGUID()))
                meteor->SetDuration(0);
        }
    };

    AreaTriggerAI* GetAI(AreaTrigger* areatrigger) const override
    {
        return new at_mage_meteor_burnAI(areatrigger);
    }
};

// Blizzard - 190356
// AreaTriggerID - 4658
class at_mage_blizzard : public AreaTriggerEntityScript
{
public:
    at_mage_blizzard() : AreaTriggerEntityScript("at_mage_blizzard") { }

    struct at_mage_blizzardAI : AreaTriggerAI
    {
        at_mage_blizzardAI(AreaTrigger* areatrigger) : AreaTriggerAI(areatrigger)
        {
            timeInterval = 1000;
        }

        int32 timeInterval;

        enum UsingSpells
        {
            SPELL_MAGE_BLIZZARD_DAMAGE = 190357
        };

        void OnCreate() override
        {
            at->SetDuration(8000);
        }

        void OnUpdate(uint32 diff) override
        {
            Unit* caster = at->GetCaster();

            if (!caster)
                return;

            if (!caster->IsPlayer())
                return;

            timeInterval += diff;
            if (timeInterval < 1000)
                return;

            if (TempSummon* tempSumm = caster->SummonCreature(WORLD_TRIGGER, at->GetPosition(), TEMPSUMMON_TIMED_DESPAWN, 8100ms))
            {
                tempSumm->SetFaction(caster->GetFaction());
                tempSumm->SetSummonerGUID(caster->GetGUID());
                PhasingHandler::InheritPhaseShift(tempSumm, caster);
                caster->CastSpell(tempSumm, SPELL_MAGE_BLIZZARD_DAMAGE, true);
            }

            timeInterval -= 1000;
        }
    };

    AreaTriggerAI* GetAI(AreaTrigger* areatrigger) const override
    {
        return new at_mage_blizzardAI(areatrigger);
    }
};

// Rune of Power - 116011
// AreaTriggerID - 304
struct at_mage_rune_of_power : AreaTriggerAI
{
    at_mage_rune_of_power(AreaTrigger* areatrigger) : AreaTriggerAI(areatrigger) { }

    enum UsingSpells
    {
        SPELL_MAGE_RUNE_OF_POWER_AURA = 116014
    };

    void OnCreate() override
    {
        //at->SetSpellXSpellVisualId(25943);
    }

    void OnUnitEnter(Unit* unit) override
    {
        if (Unit* caster = at->GetCaster())
            if (unit->GetGUID() == caster->GetGUID())
                caster->CastSpell(unit, SPELL_MAGE_RUNE_OF_POWER_AURA, true);
    }

    void OnUnitExit(Unit* unit) override
    {
        if (unit->HasAura(SPELL_MAGE_RUNE_OF_POWER_AURA))
            unit->RemoveAurasDueToSpell(SPELL_MAGE_RUNE_OF_POWER_AURA);
    }
};

// Frozen Orb - 84714
// AreaTriggerID - 8661
struct at_mage_frozen_orb : AreaTriggerAI
{
    at_mage_frozen_orb(AreaTrigger* areatrigger) : AreaTriggerAI(areatrigger)
    {
        damageInterval = 500;
    }

    uint32 damageInterval;
    bool procDone = false;

    void OnInitialize() override
    {
        Unit* caster = at->GetCaster();
        if (!caster)
            return;

        Position pos = caster->GetPosition();

        at->MovePositionToFirstCollision(pos, 40.0f, 0.0f);
        at->SetDestination(pos, 4000);
    }

    void OnCreate() override
    {
        //at->SetSpellXSpellVisualId(40291);
    }

    void OnUpdate(uint32 diff) override
    {
        Unit* caster = at->GetCaster();
        if (!caster || !caster->IsPlayer())
            return;

        if (damageInterval <= diff)
        {
            if (!procDone)
            {
                for (ObjectGuid guid : at->GetInsideUnits())
                {
                    if (Unit* unit = ObjectAccessor::GetUnit(*caster, guid))
                    {
                        if (caster->IsValidAttackTarget(unit))
                        {
                            if (caster->HasAura(SPELL_MAGE_FINGERS_OF_FROST_AURA))
                                caster->CastSpell(caster, SPELL_MAGE_FINGERS_OF_FROST_VISUAL_UI, true);

                            caster->CastSpell(caster, SPELL_MAGE_FINGERS_OF_FROST_AURA, true);

                           // at->UpdateTimeToTarget(8000); TODO
                            procDone = true;
                            break;
                        }
                    }
                }
            }

            caster->CastSpell(at->GetPosition(), SPELL_MAGE_FROZEN_ORB_DAMAGE, true);
            damageInterval = 500;
        }
        else
            damageInterval -= diff;
    }
};

// Arcane Orb - 153626
// AreaTriggerID - 1612
struct at_mage_arcane_orb : AreaTriggerAI
{
    at_mage_arcane_orb(AreaTrigger* areatrigger) : AreaTriggerAI(areatrigger) { }

    void OnUnitEnter(Unit* unit) override
    {
        if (Unit* caster = at->GetCaster())
            if (caster->IsValidAttackTarget(unit))
                caster->CastSpell(unit, SPELL_MAGE_ARCANE_ORB_DAMAGE, true);
    }
};

// 31216 - Mirror Image
class npc_mirror_image : public CreatureScript
{
public:
    npc_mirror_image() : CreatureScript("npc_mirror_image") { }

    enum eSpells
    {
        SPELL_MAGE_FROSTBOLT = 59638,
        SPELL_MAGE_FIREBALL = 133,
        SPELL_MAGE_ARCANE_BLAST = 30451,
        SPELL_MAGE_GLYPH = 63093,
        SPELL_INITIALIZE_IMAGES = 102284,
        SPELL_CLONE_CASTER = 60352,
        SPELL_INHERIT_MASTER_THREAT = 58838
    };

    struct npc_mirror_imageAI : CasterAI
    {
        npc_mirror_imageAI(Creature* creature) : CasterAI(creature) { }

        void IsSummonedBy(WorldObject* owner) override
        {
            if (!owner || !owner->IsPlayer())
                return;

            if (!me->HasUnitState(UnitState::UNIT_STATE_FOLLOW))
            {
                me->SetOwnerGUID(ObjectGuid::Empty);
                owner->ToPlayer()->SetMinion((Minion*)me, true);
                me->NearTeleportTo(owner->GetNearPosition(5.f, me->GetFollowAngle()), false);
                //me->GetMotionMaster()->Clear();
                //me->GetMotionMaster()->MoveFollow(owner->ToUnit(), PET_FOLLOW_DIST, me->GetFollowAngle());
            }

           // me->SetMaxPower(me->GetPowerType(), owner->GetMaxPower(me->GetPowerType()));
            me->SetFullPower(me->GetPowerType());
            me->SetMaxHealth(owner->ToUnit()->GetMaxHealth());
            me->SetHealth(owner->ToUnit()->GetHealth());
            me->SetReactState(ReactStates::REACT_DEFENSIVE);

            me->CastSpell(owner, SPELL_INHERIT_MASTER_THREAT, true);

            // here mirror image casts on summoner spell (not present in client dbc) 49866
            // here should be auras (not present in client dbc): 35657, 35658, 35659, 35660 selfcasted by mirror images (stats related?)

            for (uint32 attackType = 0; attackType < WeaponAttackType::MAX_ATTACK; ++attackType)
            {
                WeaponAttackType attackTypeEnum = static_cast<WeaponAttackType>(attackType);
                me->SetBaseWeaponDamage(attackTypeEnum, WeaponDamageRange::MAXDAMAGE, owner->ToUnit()->GetWeaponDamageRange(attackTypeEnum, WeaponDamageRange::MAXDAMAGE));
                me->SetBaseWeaponDamage(attackTypeEnum, WeaponDamageRange::MINDAMAGE, owner->ToUnit()->GetWeaponDamageRange(attackTypeEnum, WeaponDamageRange::MINDAMAGE));
            }

            me->UpdateAttackPowerAndDamage();
        }

        void JustEngagedWith(Unit* who) override
        {
            Unit* owner = me->GetOwner();
            if (!owner)
                return;

            Player* ownerPlayer = owner->ToPlayer();
            if (!ownerPlayer)
                return;

            eSpells spellId = eSpells::SPELL_MAGE_FROSTBOLT;
            switch (ownerPlayer->GetSpecializationId())
            {
            case TALENT_SPEC_MAGE_ARCANE:
                spellId = eSpells::SPELL_MAGE_ARCANE_BLAST;
                break;
            case TALENT_SPEC_MAGE_FIRE:
                spellId = eSpells::SPELL_MAGE_FIREBALL;
                break;
            default:
                break;
            }

            _events.ScheduleEvent(spellId, 0s); ///< Schedule cast
            //me->GetMotionMaster()->Clear();
        }

        void EnterEvadeMode(EvadeReason /*reason*/) override
        {
            if (me->IsInEvadeMode() || !me->IsAlive())
                return;

            Unit* owner = !me->GetOwnerGUID().IsEmpty() ? me->GetOwner() : nullptr;
            if (!owner)
            {
                return;
            }

            me->CombatStop(true);

            if (owner && !me->HasUnitState(UNIT_STATE_FOLLOW))
            {
                me->SetOwnerGUID(ObjectGuid::Empty);
                owner->ToPlayer()->SetMinion((Minion*)me, true);
                me->NearTeleportTo(owner->GetNearPosition(me->GetFollowDistance(), me->GetFollowAngle()), false);
                //me->GetMotionMaster()->Clear();
               // me->GetMotionMaster()->MoveFollow(owner->ToUnit(), PET_FOLLOW_DIST, me->GetFollowAngle());
            }
        }

        void Reset() override
        {
            if (Unit* owner = me->GetOwner())
            {
                owner->CastSpell(me, SPELL_INITIALIZE_IMAGES, true);
                owner->CastSpell(me, SPELL_CLONE_CASTER, true);
            }
        }

        bool CanAIAttack(Unit const* target) const override
        {
            /// Am I supposed to attack this target? (ie. do not attack polymorphed target)
            return target && !target->HasBreakableByDamageCrowdControlAura();
        }

        void UpdateAI(uint32 diff) override
        {
            _events.Update(diff);

            Unit* l_Victim = me->GetVictim();
            if (l_Victim)
            {
                if (me->GetSheath() != SHEATH_STATE_RANGED)
                {
                    me->SetSheath(SHEATH_STATE_RANGED);
                }

                if (CanAIAttack(l_Victim))
                {
                    /// If not already casting, cast! ("I'm a cast machine")
                    if (!me->HasUnitState(UNIT_STATE_CASTING))
                    {
                        if (uint32 spellId = _events.ExecuteEvent())
                        {
                            DoCast(spellId);

                            uint32 castTime = me->GetCurrentSpellCastTime(spellId);

                            std::chrono::seconds castTimeSeconds(castTime);

                            _events.ScheduleEvent(spellId, (5s),  sSpellMgr->GetSpellInfo(spellId, DIFFICULTY_NONE)->ProcCooldown);
                        }
                    }
                }
                else
                {
                    /// My victim has changed state, I shouldn't attack it anymore
                    if (me->HasUnitState(UNIT_STATE_CASTING))
                        me->CastStop();

                    me->AI()->EnterEvadeMode();
                }
            }
            else
            {
                if (me->GetSheath() != SHEATH_STATE_UNARMED)
                {
                    me->SetSheath(SHEATH_STATE_UNARMED);
                }

                /// Let's choose a new target
                Unit* target = me->SelectVictim();
                if (!target)
                {
                    /// No target? Let's see if our owner has a better target for us
                    if (Unit* owner = me->GetOwner())
                    {
                        Unit* ownerVictim = owner->GetVictim();
                        if (ownerVictim && me->CanCreatureAttack(ownerVictim))
                            target = ownerVictim;
                    }
                }

                if (target && me->Attack(target, false))
                {
                    // Clear distracted state on attacking
                    if (me->HasUnitState(UNIT_STATE_DISTRACTED))
                    {
                        me->ClearUnitState(UNIT_STATE_DISTRACTED);
                    }
                }
            }
        }
    };

   private:
       EventMap _events;

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_mirror_imageAI(creature);
    }
};

// Flame Patch
// AreaTriggerID - 6122
struct at_mage_flame_patch : AreaTriggerAI
{
    at_mage_flame_patch(AreaTrigger* areatrigger) : AreaTriggerAI(areatrigger) { }


    void OnCreate() override
    {
        timeInterval = 1000;
    }

    int32 timeInterval;

    void OnUpdate(uint32 diff) override
    {
        Unit* caster = at->GetCaster();

        if (!caster)
            return;

        if (caster->GetTypeId() != TYPEID_PLAYER)
            return;

        timeInterval += diff;
        if (timeInterval < 1000)
            return;

        caster->CastSpell(at->GetPosition(), SPELL_MAGE_FLAME_PATCH_AOE_DMG, true);

        timeInterval -= 1000;
    }
};

// Cinderstorm - 198929
// AreaTriggerID - 5487
struct at_mage_cinderstorm : AreaTriggerAI
{
    at_mage_cinderstorm(AreaTrigger* areatrigger) : AreaTriggerAI(areatrigger) { }

    void OnUnitEnter(Unit* unit) override
    {
        if (Unit* caster = at->GetCaster())
            if (caster->IsValidAttackTarget(unit))
                caster->CastSpell(unit, SPELL_MAGE_CINDERSTORM_DMG, true);
    }
};

// Cinderstorm - 198928
class spell_mage_cinderstorm : public SpellScript
{
    PrepareSpellScript(spell_mage_cinderstorm);

    void HandleDamage(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        Unit* target = GetHitUnit();
        if (!caster || !target)
            return;

        if (target->HasAura(SPELL_MAGE_IGNITE_DOT))
        {
        //    int32 pct = sSpellMgr->GetSpellInfo(SPELL_MAGE_CINDERSTORM, DIFFICULTY_NONE)->GetEffect(EFFECT_0).CalcValue(caster);
            int32 dmg = GetHitDamage();
           // AddPct(dmg, pct);
            SetHitDamage(dmg);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_mage_cinderstorm::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// 257537 - Ebonbolt
class spell_mage_ebonbolt : public SpellScript
{
    PrepareSpellScript(spell_mage_ebonbolt);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_SPLITTING_ICE,
                SPELL_MAGE_EBONBOLT_DAMAGE,
                SPELL_MAGE_BRAIN_FREEZE_AURA
            });
    }

    void DoCast()
    {
        GetCaster()->CastSpell(GetCaster(), SPELL_MAGE_BRAIN_FREEZE_AURA, true);
    }

    void DoEffectHitTarget(SpellEffIndex /*effIndex*/)
    {
        Unit* explTarget = GetExplTargetUnit();
        Unit* hitUnit = GetHitUnit();
        if (!hitUnit || !explTarget)
            return;

        if (GetCaster()->HasAura(SPELL_MAGE_SPLITTING_ICE))
            GetCaster()->VariableStorage.Set<ObjectGuid>("explTarget", explTarget->GetGUID());

        GetCaster()->CastSpell(hitUnit, SPELL_MAGE_EBONBOLT_DAMAGE, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_mage_ebonbolt::DoEffectHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnCast += SpellCastFn(spell_mage_ebonbolt::DoCast);
    }
};

// 257538 - Ebonbolt Damage
class spell_mage_ebonbolt_damage : public SpellScript
{
    PrepareSpellScript(spell_mage_ebonbolt_damage);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_SPLITTING_ICE
            });
    }

    void DoEffectHitTarget(SpellEffIndex /*effIndex*/)
    {
        Unit* hitUnit = GetHitUnit();
        ObjectGuid primaryTarget = GetCaster()->VariableStorage.GetValue<ObjectGuid>("explTarget");
        int32 damage = GetHitDamage();
        if (!hitUnit || !primaryTarget)
            return;

       // if (int32 eff1 = sSpellMgr->GetSpellInfo(SPELL_MAGE_SPLITTING_ICE, DIFFICULTY_NONE)->GetEffect(EFFECT_1).CalcValue())
          //  if (hitUnit->GetGUID() != primaryTarget)
            //    SetHitDamage(CalculatePct(damage, eff1));
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_mage_ebonbolt_damage::DoEffectHitTarget, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// Firestarter - 203283
class spell_mage_firestarter_pvp : public AuraScript
{
    PrepareAuraScript(spell_mage_firestarter_pvp);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return eventInfo.GetSpellInfo()->Id == SPELL_MAGE_FIREBALL;
    }

    void HandleProc(AuraEffect* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        caster->GetSpellHistory()->ModifyCooldown(SPELL_MAGE_COMBUSTION, -Seconds(-aurEff->GetAmount() - 5000));
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_mage_firestarter_pvp::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_mage_firestarter_pvp::HandleProc, EFFECT_1, SPELL_AURA_DUMMY);
    }
};

//1953
class spell_mage_blink : public SpellScript
{
    PrepareSpellScript(spell_mage_blink);

    void HandleLeap()
    {
        Unit* caster = GetCaster();

        if (GetCaster()->HasAura(SPELL_MAGE_BLAZING_SOUL))
            GetCaster()->AddAura(SPELL_MAGE_BLAZING_BARRIER, caster);

        if (GetCaster()->HasAura(SPELL_MAGE_PRISMATIC_CLOAK))
            GetCaster()->AddAura(SPELL_MAGE_PRISMATIC_CLOAK_BUFF, caster);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_mage_blink::HandleLeap);
    }
};

//389794
class spell_mage_snowdrift : public SpellScriptLoader
{
public:
    spell_mage_snowdrift() : SpellScriptLoader("spell_mage_snowdrift") { }

    class spell_mage_snowdrift_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_mage_snowdrift_AuraScript);

        void OnTick(AuraEffect const* aurEff)
        {
            Unit* target = GetTarget();
            Unit* caster = GetCaster();

            if (!target || !caster)
                return;

            // Slow enemies by 70%
            target->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_DECREASE_SPEED, true);
            target->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_SPEED_SLOW_ALL, true);
            target->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_ROOT, true);

            // Deal (20% of Spell power) Frost damage every 1 sec
            int32 damage = caster->SpellDamageBonusDone(target, aurEff->GetSpellInfo(), 0, DOT, aurEff->GetSpellEffectInfo(), GetStackAmount()) * aurEff->GetAmount();
            damage = target->SpellDamageBonusTaken(caster, aurEff->GetSpellInfo(), damage, DOT);
            caster->DealDamage(target, target, damage, NULL, DOT, SPELL_SCHOOL_MASK_FROST, aurEff->GetSpellInfo(), false);

            // Check if target has been caught in Snowdrift for 3 sec consecutively
            if (aurEff->GetTickNumber() >= 3)
            {
                // Apply Frozen in Ice and stun for 4 sec
                target->CastSpell(target, SPELL_FROZEN_IN_ICE, true);
                target->RemoveAura(SPELL_SNOWDRIFT);
            }
        }

        void Register()
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_mage_snowdrift_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_mage_snowdrift_AuraScript();
    }
};


//108978
class spell_mage_alter_time : public SpellScriptLoader
{
public:
    spell_mage_alter_time() : SpellScriptLoader("spell_mage_alter_time") { }

    class spell_mage_alter_time_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_mage_alter_time_SpellScript);

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            Unit* caster = GetCaster();
            Unit* target = GetHitUnit();

            if (!caster || !target)
                return;

            // Check if the spell has been cast before
            if (Aura* alterTime = target->GetAura(SPELL_ALTER_TIME))
            {
                // Check if the target has moved a long distance
                if (target->GetDistance(alterTime->GetCaster()) > 50.0f)
                {
                    target->RemoveAura(SPELL_ALTER_TIME);
                    return;
                }

                // Check if the target has died
                if (target->isDead())
                {
                    target->RemoveAura(SPELL_ALTER_TIME);
                    return;
                }

                // Return the target to their location and health from when the spell was first cast
                target->SetHealth(alterTime->GetEffect(0)->GetAmount());
                target->NearTeleportTo(alterTime->GetCaster()->GetPositionX(), alterTime->GetCaster()->GetPositionY(), alterTime->GetCaster()->GetPositionZ(), alterTime->GetCaster()->GetOrientation());
                target->RemoveAura(SPELL_ALTER_TIME);
            }
            else
            {
                // Save the target's current location and health
                caster->AddAura(SPELL_ALTER_TIME, target);
                target->SetAuraStack(SPELL_ALTER_TIME, target, target->GetHealth());
            }
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_mage_alter_time_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_mage_alter_time_SpellScript();
    }
};

//390218 10xx
class spell_mage_overflowing_energy : public SpellScriptLoader
{
public:
    spell_mage_overflowing_energy() : SpellScriptLoader("spell_mage_overflowing_energy") { }

    class spell_mage_overflowing_energy_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_mage_overflowing_energy_AuraScript);

        bool CheckProc(ProcEventInfo& eventInfo)
        {
            if (eventInfo.GetSpellInfo()->Id == 390218)
                return false;

            if (eventInfo.GetHitMask() & PROC_HIT_CRITICAL)
                return false;

            if (!eventInfo.GetDamageInfo()->GetSpellInfo())
                return false;

            return true;
        }

        void HandleProc(AuraEffect* aurEff, ProcEventInfo& eventInfo)
        {
            PreventDefaultAction();

            int32 amount = aurEff->GetAmount();
            if (eventInfo.GetDamageInfo()->GetSpellInfo()->Id == 390218)
                amount = 0;

            Unit* target = GetTarget();

            GetTarget()->CastSpell(target, 390218, SPELLVALUE_AURA_STACK);
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_mage_overflowing_energy_AuraScript::CheckProc);
            OnEffectProc += AuraEffectProcFn(spell_mage_overflowing_energy_AuraScript::HandleProc, EFFECT_0, SPELL_AURA_MOD_SPELL_CRIT_CHANCE);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_mage_overflowing_energy_AuraScript();
    }
};

const int IcicleAuras[5] = { 214124, 214125, 214126, 214127, 214130 };
const int IcicleHits[5] = { 148017, 148018, 148019, 148020, 148021 };
// Mastery: Icicles - 76613
class spell_mastery_icicles_proc : public AuraScript
{
    PrepareAuraScript(spell_mastery_icicles_proc);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        bool _spellCanProc = (eventInfo.GetSpellInfo()->Id == SPELL_MAGE_FROSTBOLT || eventInfo.GetSpellInfo()->Id == SPELL_MAGE_FROSTBOLT_TRIGGER);

        if (_spellCanProc)
            return true;
        return false;
    }

    void OnProc(AuraEffect* /*aurEff*/, ProcEventInfo& eventInfo)
    {
        Unit* target = eventInfo.GetDamageInfo()->GetVictim();
        Unit* caster = eventInfo.GetDamageInfo()->GetAttacker();
        if (!target || !caster)
            return;

        Player* player = caster->ToPlayer();

        if (!player)
            return;

        // Calculate damage
        int32 hitDamage = eventInfo.GetDamageInfo()->GetDamage() + eventInfo.GetDamageInfo()->GetAbsorb();

        // if hitDamage == 0 we have a miss, so we need to except this variant
        if (hitDamage != 0)
        {
            bool icilesAddSecond = false;

            if (caster->HasAura(SPELL_MAGE_ICE_NINE))
            {
                if (roll_chance_i(20))
                    icilesAddSecond = true;
            }

            hitDamage *= (player->m_activePlayerData->Mastery * 2.25f) / 100.0f;

            // Prevent huge hits on player after hitting low level creatures
            if (player->GetLevel() > target->GetLevel())
                hitDamage = std::min(int32(hitDamage), int32(target->GetMaxHealth()));

            // We need to get the first free icicle slot
            int8 icicleFreeSlot = -1; // -1 means no free slot
            int8 icicleSecondFreeSlot = -1; // -1 means no free slot
            for (int8 l_I = 0; l_I < 5; ++l_I)
            {
                if (!player->HasAura(IcicleAuras[l_I]))
                {
                    icicleFreeSlot = l_I;
                    if (icilesAddSecond && icicleFreeSlot != 5)
                        icicleSecondFreeSlot = l_I + 1;
                    break;
                }
            }

            switch (icicleFreeSlot)
            {
            case -1:
            {
                // We need to find the icicle with the smallest duration.
                int8 smallestIcicle = 0;
                int32 minDuration = 0xFFFFFF;
                for (int8 i = 0; i < 5; i++)
                {
                    if (Aura* tmpCurrentAura = player->GetAura(IcicleAuras[i]))
                    {
                        if (minDuration > tmpCurrentAura->GetDuration())
                        {
                            minDuration = tmpCurrentAura->GetDuration();
                            smallestIcicle = i;
                        }
                    }
                }

                // Launch the icicle with the smallest duration
                if (AuraEffect* currentIcicleAuraEffect = player->GetAuraEffect(IcicleAuras[smallestIcicle], EFFECT_0))
                {
                    float basePoints = currentIcicleAuraEffect->GetAmount();

                    if (caster->HasAura(SPELL_MAGE_BLACK_ICE))
                    {
                        if (roll_chance_i(20))
                            basePoints *= 2;
                    }

                    CastSpellExtraArgs IcicleArgs(SPELLVALUE_BASE_POINT0, basePoints);
                    IcicleArgs.SetOriginalCaster(GetCaster()->GetGUID());

                    player->CastSpell(target, IcicleHits[smallestIcicle], true);
                    player->CastSpell(target, SPELL_MAGE_ICICLE_DAMAGE, IcicleArgs);
                    player->RemoveAura(IcicleAuras[smallestIcicle]);
                }

                icicleFreeSlot = smallestIcicle;
                // No break because we'll add the icicle in the next case
            }
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            {

                if (Aura* currentIcicleAura = player->AddAura(IcicleAuras[icicleFreeSlot], player))
                {
                    if (AuraEffect* effect = currentIcicleAura->GetEffect(EFFECT_0))
                        effect->SetAmount(hitDamage);

                    player->AddAura(SPELL_MAGE_ICICLE_AURA, player);

                    if (caster->HasSpell(SPELL_MAGE_GLACIAL_SPIKE))
                    {
                        if (Aura* glacialSpikeProc = player->GetAura(SPELL_MAGE_ICICLE_AURA))
                        {
                            if (glacialSpikeProc->GetStackAmount() == 5)
                                player->CastSpell(player, SPELL_MAGE_GLACIAL_SPIKE_PROC, true);
                        }
                    }
                }
                break;
            }
            }

            switch (icicleSecondFreeSlot)
            {
            case -1:
            {
                if (icilesAddSecond)
                {
                    // We need to find the icicle with the smallest duration.
                    int8 smallestIcicle = 0;
                    int32 minDuration = 0xFFFFFF;
                    for (int8 i = 0; i < 5; i++)
                    {
                        if (Aura* tmpCurrentAura = player->GetAura(IcicleAuras[i]))
                        {
                            if (minDuration > tmpCurrentAura->GetDuration())
                            {
                                minDuration = tmpCurrentAura->GetDuration();
                                smallestIcicle = i;
                            }
                        }
                    }

                    // Launch the icicle with the smallest duration
                    if (AuraEffect* currentIcicleAuraEffect = player->GetAuraEffect(IcicleAuras[smallestIcicle], EFFECT_0))
                    {
                        float basePoints = currentIcicleAuraEffect->GetAmount();

                        if (caster->HasAura(SPELL_MAGE_BLACK_ICE))
                        {
                            if (roll_chance_i(20))
                                basePoints *= 2;
                        }

                        CastSpellExtraArgs IcicleArgs(SPELLVALUE_BASE_POINT0, basePoints);
                        IcicleArgs.SetOriginalCaster(GetCaster()->GetGUID());

                        player->CastSpell(target, IcicleHits[smallestIcicle], true);
                        player->CastSpell(target, SPELL_MAGE_ICICLE_DAMAGE, IcicleArgs);
                        player->RemoveAura(IcicleAuras[smallestIcicle]);
                    }

                    icicleSecondFreeSlot = smallestIcicle;
                    // No break because we'll add the icicle in the next case
                }
            }
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            {
                if (Aura* currentIcicleAura = player->AddAura(IcicleAuras[icicleSecondFreeSlot], player))
                {
                    if (AuraEffect* effect = currentIcicleAura->GetEffect(EFFECT_0))
                        effect->SetAmount(hitDamage);

                    player->AddAura(SPELL_MAGE_ICICLE_AURA, player);

                    if (caster->HasSpell(SPELL_MAGE_GLACIAL_SPIKE))
                    {
                        if (Aura* glacialSpikeProc = player->GetAura(SPELL_MAGE_ICICLE_AURA))
                        {
                            if (glacialSpikeProc->GetStackAmount() == 5)
                                player->CastSpell(player, SPELL_MAGE_GLACIAL_SPIKE_PROC, true);
                        }
                    }
                }
                break;
            }
            }
        }
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_mastery_icicles_proc::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_mastery_icicles_proc::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// Icicles (Aura Remove) - (214124, 214125, 214126, 214127, 214130)
class spell_mastery_icicles_mod_aura : public AuraScript
{
    PrepareAuraScript(spell_mastery_icicles_mod_aura);

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* caster = GetCaster();
        if (!caster)
        {
            return;
        }

        if (Aura* aur = caster->GetAura(SPELL_MAGE_ICICLE_AURA))
        {
            aur->ModStackAmount(-1);
        }

        if (caster->HasAura(SPELL_MAGE_GLACIAL_SPIKE_PROC))
        {
            caster->RemoveAura(SPELL_MAGE_GLACIAL_SPIKE_PROC);
        }
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_mastery_icicles_mod_aura::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Icicles (periodic) - 148023
class spell_mastery_icicles_periodic : public AuraScript
{
    PrepareAuraScript(spell_mastery_icicles_periodic);

    uint32 icicles[5];
    int32 icicleCount;

    void OnApply(AuraEffect const* /*l_AurEff*/, AuraEffectHandleModes /*l_Mode*/)
    {
        icicleCount = 0;
        if (Unit* caster = GetCaster())
        {
            for (uint32 l_I = 0; l_I < 5; ++l_I)
            {
                if (caster->HasAura(IcicleAuras[l_I]))
                    icicles[icicleCount++] = IcicleAuras[l_I];
            }
        }
    }

    void OnTick(AuraEffect const* /*aurEff*/)
    {
        Unit* caster = GetCaster();
        if (!caster)
        {
            return;
        }

        AuraEffect* aura = caster->GetAuraEffect(GetSpellInfo()->Id, EFFECT_0);
        if (!aura)
        {
            return;
        }

        // Maybe not the good target selection ...
        Unit* target = ObjectAccessor::GetUnit(*caster, caster->Variables.GetValue<ObjectGuid>("IciclesTarget"));
        if (!target)
        {
            return;
        }
        if (target->IsAlive())
        {
            int32 amount = aura->GetAmount();
            if (Aura* currentIcicleAura = caster->GetAura(icicles[amount]))
            {
                float basePoints = currentIcicleAura->GetEffect(0)->GetAmount();

                if (caster->HasAura(SPELL_MAGE_BLACK_ICE))
                {
                    if (roll_chance_i(20))
                        basePoints *= 2;
                }

                CastSpellExtraArgs IcicleArgs(SPELLVALUE_BASE_POINT0, basePoints);
                IcicleArgs.SetTriggeringAura(currentIcicleAura->GetEffect(0));
                IcicleArgs.SetOriginalCaster(GetCaster()->GetGUID());

                caster->CastSpell(target, IcicleHits[amount], true);
                caster->CastSpell(target, SPELL_MAGE_ICICLE_DAMAGE, IcicleArgs);
                caster->RemoveAura(IcicleAuras[amount]);
            }

            if (++amount >= icicleCount)
                caster->RemoveAura(aura->GetBase());
            else
                aura->SetAmount(amount);
        }
        else
        {
            caster->RemoveAura(aura->GetBase());
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_mastery_icicles_periodic::OnApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_mastery_icicles_periodic::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// Frozen Orb (damage) - 84721
class spell_mage_frozen_orb : public SpellScript
{
    PrepareSpellScript(spell_mage_frozen_orb);

    void HandleHit(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        Unit* target = GetHitUnit();
        if (!caster || !target)
            return;

        caster->CastSpell(target, SPELL_MAGE_CHILLED, true);

        // Fingers of Frost
        if (caster->HasSpell(SPELL_MAGE_FINGERS_OF_FROST))
        {
            float fingersFrostChance = 10.0f;

            if (caster->HasAura(SPELL_MAGE_FROZEN_TOUCH))
            {
                if (AuraEffect* frozenEff0 = caster->GetAuraEffect(SPELL_MAGE_FROZEN_TOUCH, EFFECT_0))
                {
                    int32 pct = frozenEff0->GetAmount();
                    AddPct(fingersFrostChance, pct);
                }
            }

            if (roll_chance_f(fingersFrostChance))
            {
                caster->CastSpell(caster, SPELL_MAGE_FINGERS_OF_FROST_VISUAL_UI, true);
                caster->CastSpell(caster, SPELL_MAGE_FINGERS_OF_FROST_AURA, true);
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_mage_frozen_orb::HandleHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// Ice Nova | Supernova - 157997 | 157980
class spell_mage_nova_talent : public SpellScript
{
    PrepareSpellScript(spell_mage_nova_talent);

    void HandleOnHit()
    {
        Unit* caster = GetCaster();
        Unit* target = GetHitUnit();
        Unit* explTarget = GetExplTargetUnit();
        if (!target || !caster || !explTarget)
            return;

        if (int32 eff2 = GetSpellInfo()->GetEffect(EFFECT_2).CalcValue())
        {
            int32 dmg = GetHitDamage();
            if (target == explTarget)
                dmg = CalculatePct(dmg, eff2);
            SetHitDamage(dmg);
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_mage_nova_talent::HandleOnHit);
    }
};

//378901 10.0.0
class spell_mage_snap_freeze : public SpellScript
{
    PrepareSpellScript(spell_mage_snap_freeze);

    void HandleHit(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
        {
            if (caster->HasAura(SPELL_MAGE_ICY_VEINS))
            {
                caster->CastSpell(caster, SPELL_MAGE_BRAIN_FREEZE, true);
                caster->CastSpell(caster, SPELL_MAGE_FINGERS_OF_FROST, true);
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_mage_snap_freeze::HandleHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

//279854
class spell_mage_glacial_assault : public SpellScriptLoader
{
public:
    spell_mage_glacial_assault() : SpellScriptLoader("spell_mage_glacial_assault") { }

    class spell_mage_glacial_assault_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_mage_glacial_assault_AuraScript);

        bool CheckProc(ProcEventInfo& eventInfo)
        {
            return eventInfo.GetActor()->HasAura(SPELL_MAGE_FLURRY);
        }

        void HandleProc(AuraEffect* aurEff, ProcEventInfo& eventInfo)
        {
            PreventDefaultAction();
            GetTarget()->CastSpell(eventInfo.GetProcTarget(), SPELL_GLACIAL_ASSAULT_TRIGGER, aurEff);
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_mage_glacial_assault_AuraScript::CheckProc);
            OnEffectProc += AuraEffectProcFn(spell_mage_glacial_assault_AuraScript::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_mage_glacial_assault_AuraScript();
    }
};

//382293 10xxxx
class spell_mage_incantation_of_swiftness : public SpellScriptLoader
{
public:
    spell_mage_incantation_of_swiftness() : SpellScriptLoader("spell_mage_incantation_of_swiftness") { }

    class spell_mage_incantation_of_swiftness_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_mage_incantation_of_swiftness_AuraScript);

        bool CheckProc(ProcEventInfo& eventInfo)
        {
            return eventInfo.GetActor()->HasAura(SPELL_MAGE_INVISIBILITY);
        }

        void HandleProc(AuraEffect* aurEff, ProcEventInfo& eventInfo)
        {
            PreventDefaultAction();
            GetTarget()->CastSpell(eventInfo.GetActor(), SPELL_INCANTATION_OF_SWIFTNESS, aurEff);
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_mage_incantation_of_swiftness_AuraScript::CheckProc);
            OnEffectProc += AuraEffectProcFn(spell_mage_incantation_of_swiftness_AuraScript::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_mage_incantation_of_swiftness_AuraScript();
    }
};

// Ice Floes - 108839
class spell_mage_ice_floes : public AuraScript
{
    PrepareAuraScript(spell_mage_ice_floes);

    void HandleAfterProc(ProcEventInfo& eventInfo)
    {
        if (eventInfo.GetSpellInfo() && eventInfo.GetSpellInfo()->CalcCastTime())
            if (Unit* caster = GetCaster())
                if (Aura* iceFloes = caster->GetAura(SPELL_MAGE_ICE_FLOES))
                    iceFloes->ModStackAmount(-1);
    }

    void Register() override
    {
        AfterProc += AuraProcFn(spell_mage_ice_floes::HandleAfterProc);
    }
};

//381706 10xxx
class spell_mage_snowstorm : public SpellScript
{
    PrepareSpellScript(spell_mage_snowstorm);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_CONE_OF_COLD });
    }

    void HandleOnHit()
    {
        if (Unit* caster = GetCaster())
        {
            if (roll_chance_i(30))
            {
                if (Aura* snowstorm = caster->GetAura(GetSpellInfo()->Id))
                {
                    if (snowstorm->GetStackAmount() < 30)
                    {
                        snowstorm->ModStackAmount(1);
                        snowstorm->RefreshDuration();
                    }
                }
                else
                    caster->AddAura(GetSpellInfo()->Id, caster);
            }
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_mage_snowstorm::HandleOnHit);
    }
};

class spell_mage_flash_freeze : public SpellScript
{
    PrepareSpellScript(spell_mage_flash_freeze);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_ICICLE_AURA, SPELL_MAGE_FINGERS_OF_FROST });
    }

    void HandleOnHit()
    {
        if (Unit* caster = GetCaster())
        {
            if (Aura* aura = caster->GetAura(SPELL_MAGE_FLASH_FREEZE))
            {
                int32 damagePct = aura->GetStackAmount() * 10;
                SetHitDamage(GetHitDamage() + GetHitDamage() * damagePct / 100);
            }

            if (roll_chance_i(5))
                caster->CastSpell(caster, SPELL_MAGE_FINGERS_OF_FROST, true);
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_mage_flash_freeze::HandleOnHit);
    }
};

//386763 10xx
class spell_mage_freezing_cold : public SpellScript
{
    PrepareSpellScript(spell_mage_freezing_cold);

    void HandleOnHit()
    {
        if (Unit* target = GetHitUnit())
        {
            if (GetCaster()->HasAura(SPELL_MAGE_FREEZING_COLD))
            {
                target->AddAura(SPELL_MAGE_FREEZING_COLD, target);
                target->SetControlled(true, UNIT_STATE_ROOT);
            }
        }
    }

    void HandleAfterHit()
    {
        if (Unit* target = GetHitUnit())
        {
            if (!target->HasAura(SPELL_MAGE_FREEZING_COLD))
            {
                target->AddAura(SPELL_MAGE_FREEZING_COLD, target);
                target->SetControlled(false, UNIT_STATE_ROOT);
                target->AddAura(SPELL_MAGE_FREEZING_COLD, target);
            }
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_mage_freezing_cold::HandleOnHit);
        AfterHit += SpellHitFn(spell_mage_freezing_cold::HandleAfterHit);
    }
};

class spell_mage_shifting_power_aura : public AuraScript
{
    PrepareAuraScript(spell_mage_shifting_power_aura);

    void HandleEffectPeriodic(AuraEffect const* aurEff)
    {
        if (Unit* caster = GetCaster())
        {
            if (caster->HasAura(SPELL_MAGE_SHIFTING_POWER))
            {
                int32 cooldownReduction = 10 / aurEff->GetTickNumber();
                caster->GetSpellHistory()->ModifyCooldown(SPELL_MAGE_SHIFTING_POWER, -Seconds(cooldownReduction));
            }
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_mage_shifting_power_aura::HandleEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
    }
};

//383243 10xxx
class spell_mage_time_anomaly : public SpellScript
{
    PrepareSpellScript(spell_mage_time_anomaly);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Player* caster = GetCaster()->ToPlayer();
        if (!caster->ToPlayer())
            return;
        {
            if (caster->GetSpecializationId() == TALENT_SPEC_MAGE_ARCANE)
            {
                caster->CastSpell(caster, SPELL_MAGE_ARCANE_SURGE, true);
                caster->CastSpell(caster, SPELL_MAGE_CLEARCASTING_EFFECT, true);
            }
            else if (caster->GetSpecializationId() == TALENT_SPEC_MAGE_FIRE)
            {
                caster->CastSpell(caster, SPELL_MAGE_COMBUSTION, true);
                caster->CastSpell(caster, SPELL_MAGE_FIRE_BLAST, true);
            }
            else if (caster->GetSpecializationId() == TALENT_SPEC_MAGE_FROST)
            {
                caster->CastSpell(caster, SPELL_MAGE_ICY_VEINS, true);
                caster->CastSpell(caster, SPELL_MAGE_FINGERS_OF_FROST, true);
            }
            else
                caster->CastSpell(caster, SPELL_MAGE_TIME_WARP, true);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_mage_time_anomaly::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 12654 - Ignite DOT
class spell_mage_ignite_periodic : public AuraScript
{
    PrepareAuraScript(spell_mage_ignite_periodic);

    void CalculateRefreshedDot(AuraEffect const* /*aurEff*/, int32& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = false;
        NewDotDmg = amount;
    }

    void HandlePeriodic(AuraEffect const* aurEff)
    {
        const_cast<AuraEffect*>(aurEff)->SetAmount(NewDotDmg);
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mage_ignite_periodic::CalculateRefreshedDot, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_mage_ignite_periodic::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
    }

private:

    int32 NewDotDmg = 1;
};

// 383490 - Wildfire Talent SPELL_MAGE_WILDFIRE_TALENT_TRIGGER
class spell_mage_wildfire_talent : public AuraScript
{
    PrepareAuraScript(spell_mage_wildfire_talent);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_WILDFIRE_SELF, SPELL_MAGE_WILDFIRE_OTHERS, SPELL_MAGE_WILDFIRE_TALENT_TRIGGER });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return eventInfo.GetProcTarget() != nullptr && GetCaster()->HasAura(SPELL_MAGE_COMBUSTION);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_mage_wildfire_talent::CheckProc);
    }
};

// 383492 - Wildfire SPELL_MAGE_WILDFIRE_SELF
class spell_mage_wildfire : public AuraScript
{
    PrepareAuraScript(spell_mage_wildfire);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_WILDFIRE_TALENT, SPELL_MAGE_WILDFIRE_SELF, SPELL_MAGE_WILDFIRE_OTHERS });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return eventInfo.GetProcTarget() != nullptr;
    }

    void CalculateRefreshedDot(AuraEffect const* aurEff, int32& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = false;
        if (Player* owner = GetCaster()->ToPlayer())
        {
            uint8 TalentRank = owner->GetTalentRankFromSpellID(SPELL_MAGE_WILDFIRE_TALENT);
            int32 CritSpell = 3 * int32(TalentRank);
            
            amount += CritSpell;
        }
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_mage_wildfire::CheckProc);
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mage_wildfire::CalculateRefreshedDot, EFFECT_0, SPELL_AURA_MOD_CRIT_PCT);
    }
};

// 383493 - Wildfire (buff periodic) SPELL_MAGE_WILDFIRE_OTHERS
class spell_mage_wildfire_periodic : public AuraScript
{
    PrepareAuraScript(spell_mage_wildfire_periodic);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_WILDFIRE_TALENT, SPELL_MAGE_WILDFIRE_SELF, SPELL_MAGE_WILDFIRE_OTHERS });
    }

    void CalculateRefreshedDot(AuraEffect const* aurEff, int32& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = false;
        if (Player* owner = GetCaster()->ToPlayer())
        {
            int32 CritSpell = 1;
            amount += CritSpell;
        }
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mage_wildfire_periodic::CalculateRefreshedDot, EFFECT_0, SPELL_AURA_MOD_CRIT_PCT);
    }
};

// 110909 - Alter Time Aura
// 342246 - Alter Time Aura
class spell_mage_alter_time_aura : public AuraScript
{
    PrepareAuraScript(spell_mage_alter_time_aura);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo
        ({
            SPELL_MAGE_ALTER_TIME_VISUAL,
            SPELL_MAGE_MASTER_OF_TIME,
            SPELL_MAGE_BLINK,
            });
    }

    void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* unit = GetTarget();
        _health = unit->GetHealth();
        _pos = unit->GetPosition();
    }

    void AfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* unit = GetTarget();
        if (unit->GetDistance(_pos) <= 100.0f && GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_EXPIRE)
        {
            unit->SetHealth(_health);
            unit->NearTeleportTo(_pos);

            if (unit->HasAura(SPELL_MAGE_MASTER_OF_TIME))
            {
                SpellInfo const* blink = sSpellMgr->AssertSpellInfo(SPELL_MAGE_BLINK, DIFFICULTY_NONE);
                unit->GetSpellHistory()->ResetCharges(blink->ChargeCategoryId);
            }
            unit->CastSpell(unit, SPELL_MAGE_ALTER_TIME_VISUAL);
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_mage_alter_time_aura::OnApply, EFFECT_0, SPELL_AURA_OVERRIDE_ACTIONBAR_SPELLS, AURA_EFFECT_HANDLE_REAL);
        AfterEffectRemove += AuraEffectRemoveFn(spell_mage_alter_time_aura::AfterRemove, EFFECT_0, SPELL_AURA_OVERRIDE_ACTIONBAR_SPELLS, AURA_EFFECT_HANDLE_REAL);
    }

private:
    uint64 _health = 0;
    Position _pos;
};

// 127140 - Alter Time Active
// 342247 - Alter Time Active
class spell_mage_alter_time_active : public SpellScript
{
    PrepareSpellScript(spell_mage_alter_time_active);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo
        ({
            SPELL_MAGE_ALTER_TIME_AURA,
            SPELL_MAGE_ARCANE_ALTER_TIME_AURA,
            });
    }

    void RemoveAlterTimeAura(SpellEffIndex /*effIndex*/)
    {
        Unit* unit = GetCaster();
        unit->RemoveAura(SPELL_MAGE_ALTER_TIME_AURA, ObjectGuid::Empty, 0, AURA_REMOVE_BY_EXPIRE);
        unit->RemoveAura(SPELL_MAGE_ARCANE_ALTER_TIME_AURA, ObjectGuid::Empty, 0, AURA_REMOVE_BY_EXPIRE);
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_mage_alter_time_active::RemoveAlterTimeAura, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 44425 - Arcane Barrage
class spell_mage_arcane_barrage : public SpellScript
{
    PrepareSpellScript(spell_mage_arcane_barrage);

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_ARCANE_BARRAGE_R3, SPELL_MAGE_ARCANE_BARRAGE_ENERGIZE })
            && ValidateSpellEffect({ { spellInfo->Id, EFFECT_1 } });
    }

    void ConsumeArcaneCharges()
    {
        Unit* caster = GetCaster();

        // Consume all arcane charges
        if (int32 arcaneCharges = -caster->ModifyPower(POWER_ARCANE_CHARGES, -caster->GetMaxPower(POWER_ARCANE_CHARGES), false))
            if (AuraEffect const* auraEffect = caster->GetAuraEffect(SPELL_MAGE_ARCANE_BARRAGE_R3, EFFECT_0, caster->GetGUID()))
                caster->CastSpell(caster, SPELL_MAGE_ARCANE_BARRAGE_ENERGIZE, { SPELLVALUE_BASE_POINT0, arcaneCharges * auraEffect->GetAmount() / 100 });
    }

    void HandleEffectHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (GetHitUnit()->GetGUID() != _primaryTarget)
            SetHitDamage(CalculatePct(GetHitDamage(), GetEffectInfo(EFFECT_1).CalcValue(GetCaster())));
    }

    void MarkPrimaryTarget(SpellEffIndex /*effIndex*/)
    {
        _primaryTarget = GetHitUnit()->GetGUID();
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_mage_arcane_barrage::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        OnEffectLaunchTarget += SpellEffectFn(spell_mage_arcane_barrage::MarkPrimaryTarget, EFFECT_1, SPELL_EFFECT_DUMMY);
        AfterCast += SpellCastFn(spell_mage_arcane_barrage::ConsumeArcaneCharges);
    }

    ObjectGuid _primaryTarget;
};

// 195302 - Arcane Charge
class spell_mage_arcane_charge_clear : public SpellScript
{
    PrepareSpellScript(spell_mage_arcane_charge_clear);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_ARCANE_CHARGE });
    }

    void RemoveArcaneCharge(SpellEffIndex /*effIndex*/)
    {
        GetHitUnit()->RemoveAurasDueToSpell(SPELL_MAGE_ARCANE_CHARGE);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_mage_arcane_charge_clear::RemoveArcaneCharge, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 1449 - Arcane Explosion
class spell_mage_arcane_explosion : public SpellScript
{
    PrepareSpellScript(spell_mage_arcane_explosion);

    bool Validate(SpellInfo const* spellInfo) override
    {
        if (!ValidateSpellInfo({ SPELL_MAGE_ARCANE_MAGE, SPELL_MAGE_REVERBERATE }))
            return false;

        if (!ValidateSpellEffect({ { spellInfo->Id, EFFECT_1 } }))
            return false;

        return spellInfo->GetEffect(EFFECT_1).IsEffect(SPELL_EFFECT_SCHOOL_DAMAGE);
    }

    void CheckRequiredAuraForBaselineEnergize(SpellEffIndex effIndex)
    {
        if (!GetUnitTargetCountForEffect(EFFECT_1) || !GetCaster()->HasAura(SPELL_MAGE_ARCANE_MAGE))
            PreventHitDefaultEffect(effIndex);
    }

    void HandleReverberate(SpellEffIndex effIndex)
    {
        bool procTriggered = [&]()
        {
            Unit const* caster = GetCaster();
            AuraEffect const* triggerChance = caster->GetAuraEffect(SPELL_MAGE_REVERBERATE, EFFECT_0);
            if (!triggerChance)
                return false;

            AuraEffect const* requiredTargets = caster->GetAuraEffect(SPELL_MAGE_REVERBERATE, EFFECT_1);
            if (!requiredTargets)
                return false;

            return GetUnitTargetCountForEffect(EFFECT_1) >= requiredTargets->GetAmount() && roll_chance_i(triggerChance->GetAmount());
        }();

        if (!procTriggered)
            PreventHitDefaultEffect(effIndex);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_mage_arcane_explosion::CheckRequiredAuraForBaselineEnergize, EFFECT_0, SPELL_EFFECT_ENERGIZE);
        OnEffectHitTarget += SpellEffectFn(spell_mage_arcane_explosion::HandleReverberate, EFFECT_2, SPELL_EFFECT_ENERGIZE);
    }
};

// 235313 - Blazing Barrier
class spell_mage_blazing_barrier : public AuraScript
{
    PrepareAuraScript(spell_mage_blazing_barrier);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_BLAZING_BARRIER_TRIGGER });
    }

    void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = false;
        if (Unit* caster = GetCaster())
            amount = int32(caster->SpellBaseHealingBonusDone(GetSpellInfo()->GetSchoolMask()) * 7.0f);
    }

    void HandleProc(AuraEffect* /*aurEff*/, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        Unit* caster = eventInfo.GetDamageInfo()->GetVictim();
        Unit* target = eventInfo.GetDamageInfo()->GetAttacker();

        if (caster && target)
            caster->CastSpell(target, SPELL_MAGE_BLAZING_BARRIER_TRIGGER, true);
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mage_blazing_barrier::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
        OnEffectProc += AuraEffectProcFn(spell_mage_blazing_barrier::HandleProc, EFFECT_1, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 190356 - Blizzard
// 4658 - AreaTrigger Create Properties
struct areatrigger_mage_blizzard : AreaTriggerAI
{
    areatrigger_mage_blizzard(AreaTrigger* areatrigger) : AreaTriggerAI(areatrigger), _tickTimer(TICK_PERIOD) { }

    static constexpr Milliseconds TICK_PERIOD = Milliseconds(1000);

    void OnUpdate(uint32 diff) override
    {
        _tickTimer -= Milliseconds(diff);

        while (_tickTimer <= 0s)
        {
            if (Unit* caster = at->GetCaster())
                caster->CastSpell(at->GetPosition(), SPELL_MAGE_BLIZZARD_DAMAGE);

            _tickTimer += TICK_PERIOD;
        }
    }

private:
    Milliseconds _tickTimer;
};

// 190357 - Blizzard (Damage)
class spell_mage_blizzard_damage : public SpellScript
{
    PrepareSpellScript(spell_mage_blizzard_damage);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_BLIZZARD_SLOW });
    }

    void HandleSlow(SpellEffIndex /*effIndex*/)
    {
        GetCaster()->CastSpell(GetHitUnit(), SPELL_MAGE_BLIZZARD_SLOW, TRIGGERED_IGNORE_CAST_IN_PROGRESS);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_mage_blizzard_damage::HandleSlow, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// 198063 - Burning Determination
class spell_mage_burning_determination : public AuraScript
{
    PrepareAuraScript(spell_mage_burning_determination);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (SpellInfo const* spellInfo = eventInfo.GetSpellInfo())
            if (spellInfo->GetAllEffectsMechanicMask() & ((1 << MECHANIC_INTERRUPT) | (1 << MECHANIC_SILENCE)))
                return true;

        return false;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_mage_burning_determination::CheckProc);
    }
};

// 86949 - Cauterize
class spell_mage_cauterize : public SpellScript
{
    PrepareSpellScript(spell_mage_cauterize);

    void SuppressSpeedBuff(SpellEffIndex effIndex)
    {
        PreventHitDefaultEffect(effIndex);
    }

    void Register() override
    {
        OnEffectLaunch += SpellEffectFn(spell_mage_cauterize::SuppressSpeedBuff, EFFECT_2, SPELL_EFFECT_TRIGGER_SPELL);
    }
};

class spell_mage_cauterize_AuraScript : public AuraScript
{
    PrepareAuraScript(spell_mage_cauterize_AuraScript);

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellEffect({ { spellInfo->Id, EFFECT_2 } }) && ValidateSpellInfo
        ({
            SPELL_MAGE_CAUTERIZE_DOT,
            SPELL_MAGE_CAUTERIZED,
            spellInfo->GetEffect(EFFECT_2).TriggerSpell
            });
    }

    void HandleAbsorb(AuraEffect* /*aurEff*/, DamageInfo& dmgInfo, uint32& absorbAmount)
    {
        Unit* caster = GetCaster();

        if (!caster)
            return;

        absorbAmount = 0;

        if (caster->ToPlayer()->HasAura(SPELL_MAGE_CAUTERIZED))
            return;

        int32 remainingHealth = caster->GetHealth() - dmgInfo.GetDamage();

        if (remainingHealth <= 0)
        {
            absorbAmount = dmgInfo.GetDamage();
            int32 cauterizeHeal = caster->CountPctFromMaxHealth(50) - caster->GetHealth();
            GetTarget()->CastSpell(GetTarget(), SPELL_MAGE_CAUTERIZE_DOT, TRIGGERED_FULL_MASK);
            caster->CastSpell(caster, SPELL_MAGE_CAUTERIZED, true);
        }
    }

    void Register() override
    {
        OnEffectAbsorb += AuraEffectAbsorbFn(spell_mage_cauterize_AuraScript::HandleAbsorb, EFFECT_0);
    }
};

// 235219 - Cold Snap
class spell_mage_cold_snap : public SpellScript
{
    PrepareSpellScript(spell_mage_cold_snap);

    static uint32 constexpr SpellsToReset[] =
    {
        SPELL_MAGE_CONE_OF_COLD,
        SPELL_MAGE_ICE_BARRIER,
        SPELL_MAGE_ICE_BLOCK,
    };

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(SpellsToReset) && ValidateSpellInfo({ SPELL_MAGE_FROST_NOVA });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        for (uint32 spellId : SpellsToReset)
            GetCaster()->GetSpellHistory()->ResetCooldown(spellId, true);

        GetCaster()->GetSpellHistory()->RestoreCharge(sSpellMgr->AssertSpellInfo(SPELL_MAGE_FROST_NOVA, GetCastDifficulty())->ChargeCategoryId);
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_mage_cold_snap::HandleDummy, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

class CometStormEvent : public BasicEvent
{
public:
    CometStormEvent(Unit* caster, ObjectGuid originalCastId, Position const& dest) : _caster(caster), _originalCastId(originalCastId), _dest(dest), _count(0) { }

    bool Execute(uint64 time, uint32 /*diff*/) override
    {
        Position destPosition = { _dest.GetPositionX() + frand(-3.0f, 3.0f), _dest.GetPositionY() + frand(-3.0f, 3.0f), _dest.GetPositionZ() };
        _caster->CastSpell(destPosition, SPELL_MAGE_COMET_STORM_VISUAL,
            CastSpellExtraArgs(TRIGGERED_IGNORE_CAST_IN_PROGRESS).SetOriginalCastId(_originalCastId));
        ++_count;

        if (_count >= 7)
            return true;

        _caster->m_Events.AddEvent(this, Milliseconds(time) + randtime(100ms, 275ms));
        return false;
    }

private:
    Unit* _caster;
    ObjectGuid _originalCastId;
    Position _dest;
    uint8 _count;
};

// 153595 - Comet Storm (launch)
class spell_mage_comet_storm : public SpellScript
{
    PrepareSpellScript(spell_mage_comet_storm);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_COMET_STORM_VISUAL });
    }

    void EffectHit(SpellEffIndex /*effIndex*/)
    {
        GetCaster()->m_Events.AddEventAtOffset(new CometStormEvent(GetCaster(), GetSpell()->m_castId, *GetHitDest()), randtime(100ms, 275ms));
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_mage_comet_storm::EffectHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 228601 - Comet Storm (damage)
class spell_mage_comet_storm_damage : public SpellScript
{
    PrepareSpellScript(spell_mage_comet_storm_damage);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_COMET_STORM_DAMAGE });
    }

    void HandleEffectHitTarget(SpellEffIndex /*effIndex*/)
    {
        GetCaster()->CastSpell(*GetHitDest(), SPELL_MAGE_COMET_STORM_DAMAGE,
            CastSpellExtraArgs(TRIGGERED_IGNORE_CAST_IN_PROGRESS).SetOriginalCastId(GetSpell()->m_originalCastId));
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_mage_comet_storm_damage::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 120 - Cone of Cold
class spell_mage_cone_of_cold : public SpellScript
{
    PrepareSpellScript(spell_mage_cone_of_cold);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_CONE_OF_COLD_SLOW });
    }

    void HandleSlow(SpellEffIndex /*effIndex*/)
    {
        GetCaster()->CastSpell(GetHitUnit(), SPELL_MAGE_CONE_OF_COLD_SLOW, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_mage_cone_of_cold::HandleSlow, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// 190336 - Conjure Refreshment
class spell_mage_conjure_refreshment : public SpellScript
{
    PrepareSpellScript(spell_mage_conjure_refreshment);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo
        ({
            SPELL_MAGE_CONJURE_REFRESHMENT,
            SPELL_MAGE_CONJURE_REFRESHMENT_TABLE
            });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (Player* caster = GetCaster()->ToPlayer())
        {
            Group* group = caster->GetGroup();
            if (group)
                caster->CastSpell(caster, SPELL_MAGE_CONJURE_REFRESHMENT_TABLE, true);
            else
                caster->CastSpell(caster, SPELL_MAGE_CONJURE_REFRESHMENT, true);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_mage_conjure_refreshment::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 112965 - Fingers of Frost
class spell_mage_fingers_of_frost : public AuraScript
{
    PrepareAuraScript(spell_mage_fingers_of_frost);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_FINGERS_OF_FROST });
    }

    bool CheckFrostboltProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        return eventInfo.GetSpellInfo() && eventInfo.GetSpellInfo()->IsAffected(SPELLFAMILY_MAGE, flag128(0, 0x2000000, 0, 0))
            && roll_chance_i(aurEff->GetAmount());
    }

    bool CheckFrozenOrbProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        return eventInfo.GetSpellInfo() && eventInfo.GetSpellInfo()->IsAffected(SPELLFAMILY_MAGE, flag128(0, 0, 0x80, 0))
            && roll_chance_i(aurEff->GetAmount());
    }

    void Trigger(AuraEffect* aurEff, ProcEventInfo& eventInfo)
    {
        eventInfo.GetActor()->CastSpell(GetTarget(), SPELL_MAGE_FINGERS_OF_FROST, aurEff);
    }

    void Register() override
    {
        DoCheckEffectProc += AuraCheckEffectProcFn(spell_mage_fingers_of_frost::CheckFrostboltProc, EFFECT_0, SPELL_AURA_DUMMY);
        DoCheckEffectProc += AuraCheckEffectProcFn(spell_mage_fingers_of_frost::CheckFrozenOrbProc, EFFECT_1, SPELL_AURA_DUMMY);
        AfterEffectProc += AuraEffectProcFn(spell_mage_fingers_of_frost::Trigger, EFFECT_0, SPELL_AURA_DUMMY);
        AfterEffectProc += AuraEffectProcFn(spell_mage_fingers_of_frost::Trigger, EFFECT_1, SPELL_AURA_DUMMY);
    }
};

// 133 - Fireball
// 11366 - Pyroblast
class spell_mage_firestarter : public SpellScript
{
    PrepareSpellScript(spell_mage_firestarter);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_FIRESTARTER });
    }

    void CalcCritChance(Unit const* victim, float& critChance)
    {
        if (AuraEffect const* aurEff = GetCaster()->GetAuraEffect(SPELL_MAGE_FIRESTARTER, EFFECT_0))
            if (victim->GetHealthPct() >= aurEff->GetAmount())
                critChance = 100.0f;
    }

    void Register() override
    {
        OnCalcCritChance += SpellOnCalcCritChanceFn(spell_mage_firestarter::CalcCritChance);
    }
};

// 321712 - Pyroblast
class spell_mage_firestarter_dots : public AuraScript
{
    PrepareAuraScript(spell_mage_firestarter_dots);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_FIRESTARTER });
    }

    void CalcCritChance(AuraEffect const* /*aurEff*/, Unit const* victim, float& critChance)
    {
        if (AuraEffect const* aurEff = GetCaster()->GetAuraEffect(SPELL_MAGE_FIRESTARTER, EFFECT_0))
            if (victim->GetHealthPct() >= aurEff->GetAmount())
                critChance = 100.0f;
    }

    void Register() override
    {
        DoEffectCalcCritChance += AuraEffectCalcCritChanceFn(spell_mage_firestarter_dots::CalcCritChance, EFFECT_ALL, SPELL_AURA_PERIODIC_DAMAGE);
    }
};

// 205029 - Flame On
class spell_mage_flame_on : public AuraScript
{
    PrepareAuraScript(spell_mage_flame_on);

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_FIRE_BLAST })
            && sSpellCategoryStore.HasRecord(sSpellMgr->AssertSpellInfo(SPELL_MAGE_FIRE_BLAST, DIFFICULTY_NONE)->ChargeCategoryId)
            && ValidateSpellEffect({ { spellInfo->Id, EFFECT_2 } });
    }

    void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = false;
        amount = -GetPctOf(GetEffectInfo(EFFECT_2).CalcValue() * IN_MILLISECONDS, sSpellCategoryStore.AssertEntry(sSpellMgr->AssertSpellInfo(SPELL_MAGE_FIRE_BLAST, DIFFICULTY_NONE)->ChargeCategoryId)->ChargeRecoveryTime);
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mage_flame_on::CalculateAmount, EFFECT_1, SPELL_AURA_CHARGE_RECOVERY_MULTIPLIER);
    }
};

// 116 - Frostbolt
class spell_mage_frostbolt : public SpellScript
{
    PrepareSpellScript(spell_mage_frostbolt);

    bool Validate(SpellInfo const* /*spell*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_CHILLED });
    }

    void HandleChilled()
    {
        if (Unit* target = GetHitUnit())
            GetCaster()->CastSpell(target, SPELL_MAGE_CHILLED, TRIGGERED_IGNORE_CAST_IN_PROGRESS);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_mage_frostbolt::HandleChilled);
    }
};

// 11426 - Ice Barrier
class spell_mage_ice_barrier : public AuraScript
{
    PrepareAuraScript(spell_mage_ice_barrier);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo
        ({
            SPELL_MAGE_CHILLED
            });
    }

    void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = false;
        if (Unit* caster = GetCaster())
            amount += int32(caster->SpellBaseHealingBonusDone(GetSpellInfo()->GetSchoolMask()) * 10.0f);
    }

    void HandleProc(AuraEffect* /*aurEff*/, ProcEventInfo& eventInfo)
    {
        Unit* caster = eventInfo.GetDamageInfo()->GetVictim();
        Unit* target = eventInfo.GetDamageInfo()->GetAttacker();

        if (caster && target)
            caster->CastSpell(target, SPELL_MAGE_CHILLED, true);
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mage_ice_barrier::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
        OnEffectProc += AuraEffectProcFn(spell_mage_ice_barrier::HandleProc, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
    }
};

// 45438 - Ice Block
class spell_mage_ice_block : public SpellScript
{
    PrepareSpellScript(spell_mage_ice_block);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_ICE_BLOCK, SPELL_MAGE_EVERWARM_SOCKS });
    }

    void PreventStunWithEverwarmSocks(WorldObject*& target)
    {
        if (GetCaster()->HasAura(SPELL_MAGE_EVERWARM_SOCKS))
            target = nullptr;
    }

    void PreventEverwarmSocks(WorldObject*& target)
    {
        if (GetCaster()->HasAura(SPELL_MAGE_EVERWARM_SOCKS))
            target = nullptr;
    }

    void Register() override
    {
        OnObjectTargetSelect += SpellObjectTargetSelectFn(spell_mage_ice_block::PreventStunWithEverwarmSocks, EFFECT_0, TARGET_UNIT_CASTER);
        OnObjectTargetSelect += SpellObjectTargetSelectFn(spell_mage_ice_block::PreventEverwarmSocks, EFFECT_5, TARGET_UNIT_CASTER);
        OnObjectTargetSelect += SpellObjectTargetSelectFn(spell_mage_ice_block::PreventEverwarmSocks, EFFECT_6, TARGET_UNIT_CASTER);
    }
};

// Ice Lance - 30455
class spell_mage_ice_lance : public SpellScript
{
    PrepareSpellScript(spell_mage_ice_lance);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo
        ({
            SPELL_MAGE_ICE_LANCE_TRIGGER,
            SPELL_MAGE_THERMAL_VOID,
            SPELL_MAGE_ICY_VEINS,
            SPELL_MAGE_CHAIN_REACTION_DUMMY,
            SPELL_MAGE_CHAIN_REACTION,
            SPELL_MAGE_FINGERS_OF_FROST
            });
    }

    void IndexTarget(SpellEffIndex /*effIndex*/)
    {
        _orderedTargets.push_back(GetHitUnit()->GetGUID());
    }

    void HandleOnHit(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        Unit* target = GetHitUnit();

        std::ptrdiff_t index = std::distance(_orderedTargets.begin(), std::find(_orderedTargets.begin(), _orderedTargets.end(), target->GetGUID()));

        if (index == 0 // only primary target triggers these benefits
            && (target->HasAuraState(AURA_STATE_FROZEN, GetSpellInfo(), caster) || caster->HasAura(SPELL_MAGE_FINGERS_OF_FROST_AURA)))
        {
            // Thermal Void
            if (Aura const* thermalVoid = caster->GetAura(SPELL_MAGE_THERMAL_VOID))
                if (!thermalVoid->GetSpellInfo()->GetEffects().empty())
                    if (Aura* icyVeins = caster->GetAura(SPELL_MAGE_ICY_VEINS))
                        icyVeins->SetDuration(icyVeins->GetDuration() + thermalVoid->GetSpellInfo()->GetEffect(EFFECT_0).CalcValue(caster) * IN_MILLISECONDS);

            // Chain Reaction
            if (caster->HasAura(SPELL_MAGE_CHAIN_REACTION_DUMMY))
                caster->CastSpell(caster, SPELL_MAGE_CHAIN_REACTION, true);
        }

        if (caster->HasAura(SPELL_MAGE_FINGERS_OF_FROST_AURA))
        {
            caster->RemoveAura(SPELL_MAGE_FINGERS_OF_FROST_AURA);
        }

        // put target index for chain value multiplier into EFFECT_1 base points, otherwise triggered spell doesn't know which damage multiplier to apply
        CastSpellExtraArgs args;
        args.TriggerFlags = TRIGGERED_FULL_MASK;
        args.AddSpellMod(SPELLVALUE_BASE_POINT1, index);
        caster->CastSpell(target, SPELL_MAGE_ICE_LANCE_TRIGGER, args);
    }

    void HandleAfterHit()
    {
        Unit* caster = GetCaster();
        Unit* target = GetHitUnit();
        if (!caster && !target)
            return;

        if (target->IsAlive() && !caster->HasSpell(SPELL_MAGE_GLACIAL_SPIKE))
        {
            caster->Variables.Set("IciclesTarget", target->GetGUID());
            caster->CastSpell(caster, SPELL_MAGE_ICICLE_PERIODIC_TRIGGER, true);
        }
    }

    void Register() override
    {
        OnEffectLaunchTarget += SpellEffectFn(spell_mage_ice_lance::IndexTarget, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        OnEffectHitTarget += SpellEffectFn(spell_mage_ice_lance::HandleOnHit, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        AfterHit += SpellHitFn(spell_mage_ice_lance::HandleAfterHit);
    }

    std::vector<ObjectGuid> _orderedTargets;
};

// 228598 - Ice Lance
class spell_mage_ice_lance_damage : public SpellScript
{
    PrepareSpellScript(spell_mage_ice_lance_damage);

    void ApplyDamageMultiplier(SpellEffIndex /*effIndex*/)
    {
        SpellValue const* spellValue = GetSpellValue();
        if (spellValue->CustomBasePointsMask & (1 << EFFECT_1))
        {
            int32 originalDamage = GetHitDamage();
            float targetIndex = float(spellValue->EffectBasePoints[EFFECT_1]);
            float multiplier = std::pow(GetEffectInfo().CalcDamageMultiplier(GetCaster(), GetSpell()), targetIndex);
            SetHitDamage(int32(originalDamage * multiplier));
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_mage_ice_lance_damage::ApplyDamageMultiplier, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// 148022 - Icicle Damage
class spell_mage_icicle_damage : public SpellScript
{
    PrepareSpellScript(spell_mage_icicle_damage);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_SPLITTING_ICE
            });
    }

    void HandleOnHit()
    {
        Unit* explTarget = GetExplTargetUnit();
        Unit* hitUnit = GetHitUnit();
        if (!hitUnit || !explTarget || explTarget->IsFriendlyTo(GetCaster()) || hitUnit->IsFriendlyTo(GetCaster()))
        {
            SetHitDamage(0);
            return;
        }

        SetHitDamage(GetSpellValue()->EffectBasePoints[EFFECT_0]);

        if (GetCaster()->HasAura(SPELL_MAGE_SPLITTING_ICE))
        {
            if (SpellInfo const* eff = sSpellMgr->GetSpellInfo(SPELL_MAGE_SPLITTING_ICE, DIFFICULTY_NONE))
                if (hitUnit != explTarget)
                    SetHitDamage(CalculatePct(GetHitDamage(), eff->GetEffect(EFFECT_1).CalcValue()));
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_mage_icicle_damage::HandleOnHit);
    }
};

// 12846 - Ignite
class spell_mage_ignite : public AuraScript
{
    PrepareAuraScript(spell_mage_ignite);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_IGNITE });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return eventInfo.GetProcTarget() != nullptr;
    }

    void HandleProc(AuraEffect* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        int32 amount = int32(CalculatePct(eventInfo.GetDamageInfo()->GetDamage(), 5.f));

        CastSpellExtraArgs args(aurEff);
        args.AddSpellMod(SPELLVALUE_BASE_POINT0, amount);
        GetTarget()->CastSpell(eventInfo.GetProcTarget(), SPELL_MAGE_IGNITE, args);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_mage_ignite::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_mage_ignite::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 37447 - Improved Mana Gems
// 61062 - Improved Mana Gems
class spell_mage_imp_mana_gems : public AuraScript
{
    PrepareAuraScript(spell_mage_imp_mana_gems);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_MANA_SURGE });
    }

    void HandleProc(AuraEffect* /*aurEff*/, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        eventInfo.GetActor()->CastSpell(nullptr, SPELL_MAGE_MANA_SURGE, true);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_mage_imp_mana_gems::HandleProc, EFFECT_1, SPELL_AURA_DUMMY);
    }
};

// 1463 - Incanter's Flow
class spell_mage_incanters_flow : public AuraScript
{
    PrepareAuraScript(spell_mage_incanters_flow);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_INCANTERS_FLOW });
    }

    void HandlePeriodicTick(AuraEffect const* /*aurEff*/)
    {
        // Incanter's flow should not cycle out of combat
        if (!GetTarget()->IsInCombat())
            return;

        if (Aura* aura = GetTarget()->GetAura(SPELL_MAGE_INCANTERS_FLOW))
        {
            uint32 stacks = aura->GetStackAmount();

            // Force always to values between 1 and 5
            if ((modifier == -1 && stacks == 1) || (modifier == 1 && stacks == 5))
            {
                modifier *= -1;
                return;
            }

            aura->ModStackAmount(modifier);
        }
        else
            GetTarget()->CastSpell(GetTarget(), SPELL_MAGE_INCANTERS_FLOW, true);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_mage_incanters_flow::HandlePeriodicTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }

private:
    int8 modifier = 1;
};

// 44457 - Living Bomb
class spell_mage_living_bomb : public SpellScript
{
    PrepareSpellScript(spell_mage_living_bomb);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_LIVING_BOMB_PERIODIC });
    }

    void HandleDummy(SpellEffIndex effIndex)
    {
        PreventHitDefaultEffect(effIndex);
        GetCaster()->CastSpell(GetHitUnit(), SPELL_MAGE_LIVING_BOMB_PERIODIC, CastSpellExtraArgs(TRIGGERED_FULL_MASK).AddSpellMod(SPELLVALUE_BASE_POINT2, 1));
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_mage_living_bomb::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 44461 - Living Bomb
class spell_mage_living_bomb_explosion : public SpellScript
{
    PrepareSpellScript(spell_mage_living_bomb_explosion);

    bool Validate(SpellInfo const* spellInfo) override
    {
        return spellInfo->NeedsExplicitUnitTarget() && ValidateSpellInfo({ SPELL_MAGE_LIVING_BOMB_PERIODIC });
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove(GetExplTargetWorldObject());
    }

    void HandleSpread(SpellEffIndex /*effIndex*/)
    {
        if (GetSpellValue()->EffectBasePoints[EFFECT_0] > 0)
            GetCaster()->CastSpell(GetHitUnit(), SPELL_MAGE_LIVING_BOMB_PERIODIC, CastSpellExtraArgs(TRIGGERED_FULL_MASK).AddSpellMod(SPELLVALUE_BASE_POINT2, 0));
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_mage_living_bomb_explosion::FilterTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENEMY);
        OnEffectHitTarget += SpellEffectFn(spell_mage_living_bomb_explosion::HandleSpread, EFFECT_1, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// 217694 - Living Bomb
class spell_mage_living_bomb_periodic : public AuraScript
{
    PrepareAuraScript(spell_mage_living_bomb_periodic);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_LIVING_BOMB_EXPLOSION });
    }

    void AfterRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        if (GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_EXPIRE)
            return;

        if (Unit* caster = GetCaster())
            caster->CastSpell(GetTarget(), SPELL_MAGE_LIVING_BOMB_EXPLOSION, CastSpellExtraArgs(TRIGGERED_FULL_MASK).AddSpellMod(SPELLVALUE_BASE_POINT0, aurEff->GetAmount()));
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_mage_living_bomb_periodic::AfterRemove, EFFECT_2, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

enum SilvermoonPolymorph
{
    NPC_AUROSALIA = 18744
};

/// @todo move out of here and rename - not a mage spell
// 32826 - Polymorph (Visual)
class spell_mage_polymorph_visual : public SpellScript
{
    PrepareSpellScript(spell_mage_polymorph_visual);

    static const uint32 PolymorhForms[6];

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(PolymorhForms);
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (Unit* target = GetCaster()->FindNearestCreature(NPC_AUROSALIA, 30.0f))
            if (target->GetTypeId() == TYPEID_UNIT)
                target->CastSpell(target, PolymorhForms[urand(0, 5)], true);
    }

    void Register() override
    {
        // add dummy effect spell handler to Polymorph visual
        OnEffectHitTarget += SpellEffectFn(spell_mage_polymorph_visual::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

uint32 const spell_mage_polymorph_visual::PolymorhForms[6] =
{
    SPELL_MAGE_SQUIRREL_FORM,
    SPELL_MAGE_GIRAFFE_FORM,
    SPELL_MAGE_SERPENT_FORM,
    SPELL_MAGE_DRAGONHAWK_FORM,
    SPELL_MAGE_WORGEN_FORM,
    SPELL_MAGE_SHEEP_FORM
};

// 235450 - Prismatic Barrier
class spell_mage_prismatic_barrier : public AuraScript
{
    PrepareAuraScript(spell_mage_prismatic_barrier);

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellEffect({ { spellInfo->Id, EFFECT_5 } });
    }

    void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = false;
        if (Unit* caster = GetCaster())
            amount = int32(CalculatePct(caster->GetMaxHealth(), GetEffectInfo(EFFECT_5).CalcValue(caster)));
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mage_prismatic_barrier::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
    }
};

// 136511 - Ring of Frost
class spell_mage_ring_of_frost : public AuraScript
{
    PrepareAuraScript(spell_mage_ring_of_frost);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_RING_OF_FROST_SUMMON, SPELL_MAGE_RING_OF_FROST_FREEZE })
            && !sSpellMgr->AssertSpellInfo(SPELL_MAGE_RING_OF_FROST_SUMMON, DIFFICULTY_NONE)->GetEffects().empty();
    }

    void HandleEffectPeriodic(AuraEffect const* /*aurEff*/)
    {
        if (TempSummon* ringOfFrost = GetRingOfFrostMinion())
            GetTarget()->CastSpell(ringOfFrost->GetPosition(), SPELL_MAGE_RING_OF_FROST_FREEZE, true);
    }

    void Apply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        std::list<TempSummon*> minions;
        GetTarget()->GetAllMinionsByEntry(minions, sSpellMgr->AssertSpellInfo(SPELL_MAGE_RING_OF_FROST_SUMMON, GetCastDifficulty())->GetEffect(EFFECT_0).MiscValue);

        // Get the last summoned RoF, save it and despawn older ones
        for (TempSummon* summon : minions)
        {
            if (TempSummon* ringOfFrost = GetRingOfFrostMinion())
            {
                if (summon->GetTimer() > ringOfFrost->GetTimer())
                {
                    ringOfFrost->DespawnOrUnsummon();
                    _ringOfFrostGUID = summon->GetGUID();
                }
                else
                    summon->DespawnOrUnsummon();
            }
            else
                _ringOfFrostGUID = summon->GetGUID();
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_mage_ring_of_frost::HandleEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
        OnEffectApply += AuraEffectApplyFn(spell_mage_ring_of_frost::Apply, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
    }

private:
    TempSummon* GetRingOfFrostMinion() const
    {
        if (Creature* creature = ObjectAccessor::GetCreature(*GetOwner(), _ringOfFrostGUID))
            return creature->ToTempSummon();
        return nullptr;
    }

    ObjectGuid _ringOfFrostGUID;
};

// 82691 - Ring of Frost (freeze efect)
class spell_mage_ring_of_frost_freeze : public SpellScript
{
    PrepareSpellScript(spell_mage_ring_of_frost_freeze);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_RING_OF_FROST_SUMMON, SPELL_MAGE_RING_OF_FROST_FREEZE })
            && !sSpellMgr->AssertSpellInfo(SPELL_MAGE_RING_OF_FROST_SUMMON, DIFFICULTY_NONE)->GetEffects().empty();
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        WorldLocation const* dest = GetExplTargetDest();
        float outRadius = sSpellMgr->AssertSpellInfo(SPELL_MAGE_RING_OF_FROST_SUMMON, GetCastDifficulty())->GetEffect(EFFECT_0).CalcRadius(nullptr, SpellTargetIndex::TargetB);
        float inRadius = 6.5f;

        targets.remove_if([dest, outRadius, inRadius](WorldObject* target)
            {
                Unit* unit = target->ToUnit();
                if (!unit)
                    return true;
                return unit->HasAura(SPELL_MAGE_RING_OF_FROST_DUMMY) || unit->HasAura(SPELL_MAGE_RING_OF_FROST_FREEZE) || unit->GetExactDist(dest) > outRadius || unit->GetExactDist(dest) < inRadius;
            });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_mage_ring_of_frost_freeze::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
    }
};

class spell_mage_ring_of_frost_freeze_AuraScript : public AuraScript
{
    PrepareAuraScript(spell_mage_ring_of_frost_freeze_AuraScript);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_RING_OF_FROST_DUMMY });
    }

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_EXPIRE)
            if (GetCaster())
                GetCaster()->CastSpell(GetTarget(), SPELL_MAGE_RING_OF_FROST_DUMMY, true);
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_mage_ring_of_frost_freeze_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
    }
};

// 157980 - Supernova
class spell_mage_supernova : public SpellScript
{
    PrepareSpellScript(spell_mage_supernova);

    void HandleDamage(SpellEffIndex /*effIndex*/)
    {
        if (GetExplTargetUnit() == GetHitUnit())
        {
            uint32 damage = GetHitDamage();
            AddPct(damage, GetEffectInfo(EFFECT_0).CalcValue());
            SetHitDamage(damage);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_mage_supernova::HandleDamage, EFFECT_1, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// 210824 - Touch of the Magi (Aura)
class spell_mage_touch_of_the_magi_aura : public AuraScript
{
    PrepareAuraScript(spell_mage_touch_of_the_magi_aura);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_TOUCH_OF_THE_MAGI_EXPLODE });
    }

    void HandleProc(AuraEffect* aurEff, ProcEventInfo& eventInfo)
    {
        DamageInfo* damageInfo = eventInfo.GetDamageInfo();
        if (damageInfo)
        {
            if (damageInfo->GetAttacker() == GetCaster() && damageInfo->GetVictim() == GetTarget())
            {
                uint32 extra = CalculatePct(damageInfo->GetDamage(), 25);
                if (extra > 0)
                    aurEff->ChangeAmount(aurEff->GetAmount() + extra);
            }
        }
    }

    void AfterRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        int32 amount = aurEff->GetAmount();
        if (!amount || GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_EXPIRE)
            return;

        if (Unit* caster = GetCaster())
            caster->CastSpell(GetTarget(), SPELL_MAGE_TOUCH_OF_THE_MAGI_EXPLODE, CastSpellExtraArgs(TRIGGERED_FULL_MASK).AddSpellMod(SPELLVALUE_BASE_POINT0, amount));
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_mage_touch_of_the_magi_aura::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
        AfterEffectRemove += AuraEffectRemoveFn(spell_mage_touch_of_the_magi_aura::AfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 33395 Water Elemental's Freeze
class spell_mage_water_elemental_freeze : public SpellScript
{
    PrepareSpellScript(spell_mage_water_elemental_freeze);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_FINGERS_OF_FROST });
    }

    void HandleImprovedFreeze()
    {
        Unit* owner = GetCaster()->GetOwner();
        if (!owner)
            return;

        owner->CastSpell(owner, SPELL_MAGE_FINGERS_OF_FROST, true);
    }

    void Register() override
    {
        AfterHit += SpellHitFn(spell_mage_water_elemental_freeze::HandleImprovedFreeze);
    }
};

// 205021 - Ray of Frost
class spell_mage_ray_of_frost : public SpellScript
{
    PrepareSpellScript(spell_mage_ray_of_frost);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_RAY_OF_FROST_FINGERS_OF_FROST });
    }

    void HandleOnHit()
    {
        if (Unit* caster = GetCaster())
            caster->CastSpell(caster, SPELL_MAGE_RAY_OF_FROST_FINGERS_OF_FROST, TRIGGERED_IGNORE_CAST_IN_PROGRESS);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_mage_ray_of_frost::HandleOnHit);
    }
};

class spell_mage_ray_of_frost_aura : public AuraScript
{
    PrepareAuraScript(spell_mage_ray_of_frost_aura);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_RAY_OF_FROST_BONUS, SPELL_MAGE_RAY_OF_FROST_FINGERS_OF_FROST });
    }

    void HandleEffectPeriodic(AuraEffect const* aurEff)
    {
        if (Unit* caster = GetCaster())
        {
            if (aurEff->GetTickNumber() > 1) // First tick should deal base damage
                caster->CastSpell(caster, SPELL_MAGE_RAY_OF_FROST_BONUS, true);
        }
    }

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* caster = GetCaster())
            caster->RemoveAurasDueToSpell(SPELL_MAGE_RAY_OF_FROST_FINGERS_OF_FROST);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_mage_ray_of_frost_aura::HandleEffectPeriodic, EFFECT_1, SPELL_AURA_PERIODIC_DAMAGE);
        OnEffectRemove += AuraEffectRemoveFn(spell_mage_ray_of_frost_aura::OnRemove, EFFECT_1, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
    }
};

// 410939 - Ethereal Blink
class spell_mage_ethereal_blink : public AuraScript
{
    PrepareAuraScript(spell_mage_ethereal_blink);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_BLINK, SPELL_MAGE_SHIMMER });
    }

    void HandleProc(AuraEffect* aurEff, ProcEventInfo& procInfo)
    {
        PreventDefaultAction();

        // this proc only works for players because teleport relocation happens after an ACK
        GetTarget()->CastSpell(*procInfo.GetProcSpell()->m_targets.GetDst(), aurEff->GetSpellEffectInfo().TriggerSpell, CastSpellExtraArgs(aurEff)
            .SetTriggeringSpell(procInfo.GetProcSpell())
            .SetCustomArg(GetTarget()->GetPosition()));
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_mage_ethereal_blink::HandleProc, EFFECT_1, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 410941 - Ethereal Blink
class spell_mage_ethereal_blink_triggered : public SpellScript
{
    PrepareSpellScript(spell_mage_ethereal_blink_triggered);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_BLINK, SPELL_MAGE_SHIMMER, SPELL_MAGE_SLOW })
            && ValidateSpellEffect({ { SPELL_MAGE_ETHEREAL_BLINK, EFFECT_3 } });
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        Position const* src = std::any_cast<Position>(&GetSpell()->m_customArg);
        WorldLocation const* dst = GetExplTargetDest();
        if (!src || !dst)
        {
            targets.clear();
            return;
        }

        targets.remove_if([&](WorldObject* target)
            {
                return !target->IsInBetween(*src, *dst, (target->GetCombatReach() + GetCaster()->GetCombatReach()) / 2.0f);
            });


        AuraEffect const* reductionEffect = GetCaster()->GetAuraEffect(SPELL_MAGE_ETHEREAL_BLINK, EFFECT_2);
        if (!reductionEffect)
            return;

        Seconds reduction = Seconds(reductionEffect->GetAmount()) * targets.size();

        if (AuraEffect const* cap = GetCaster()->GetAuraEffect(SPELL_MAGE_ETHEREAL_BLINK, EFFECT_3))
            if (reduction > Seconds(cap->GetAmount()))
                reduction = Seconds(cap->GetAmount());

        if (reduction > 0s)
        {
            GetCaster()->GetSpellHistory()->ModifyCooldown(SPELL_MAGE_BLINK, -reduction);
            GetCaster()->GetSpellHistory()->ModifyCooldown(SPELL_MAGE_SHIMMER, -reduction);
        }
    }

    void TriggerSlow(SpellEffIndex /*effIndex*/)
    {
        int32 effectivenessPct = 100;
        if (AuraEffect const* effectivenessEffect = GetCaster()->GetAuraEffect(SPELL_MAGE_ETHEREAL_BLINK, EFFECT_1))
            effectivenessPct = effectivenessEffect->GetAmount();

        int32 slowPct = sSpellMgr->AssertSpellInfo(SPELL_MAGE_SLOW, DIFFICULTY_NONE)->GetEffect(EFFECT_0).CalcBaseValue(GetCaster(), GetHitUnit(), 0, -1);
        ApplyPct(slowPct, effectivenessPct);

        GetCaster()->CastSpell(GetHitUnit(), SPELL_MAGE_SLOW, CastSpellExtraArgs(GetSpell())
            .AddSpellMod(SPELLVALUE_BASE_POINT0, slowPct));
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_mage_ethereal_blink_triggered::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
        OnEffectHitTarget += SpellEffectFn(spell_mage_ethereal_blink_triggered::TriggerSlow, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 383395 - Feel the Burn
class spell_mage_feel_the_burn : public AuraScript
{
    PrepareAuraScript(spell_mage_feel_the_burn);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_FEEL_THE_BURN });
    }

    void CalcAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& canBeRecalculated)
    {
        if (Unit* caster = GetCaster())
            if (AuraEffect const* valueHolder = caster->GetAuraEffect(SPELL_MAGE_FEEL_THE_BURN, EFFECT_0))
                amount = valueHolder->GetAmount();

        canBeRecalculated = false;
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mage_feel_the_burn::CalcAmount, EFFECT_0, SPELL_AURA_MASTERY);
    }
};

// 386737 - Hyper Impact
class spell_mage_hyper_impact : public AuraScript
{
    PrepareAuraScript(spell_mage_hyper_impact);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_SUPERNOVA });
    }

    void HandleProc(AuraEffect* /*aurEff*/, ProcEventInfo& eventInfo)
    {
        eventInfo.GetActor()->CastSpell(eventInfo.GetActionTarget(), SPELL_MAGE_SUPERNOVA, true);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_mage_hyper_impact::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 376103 - Radiant Spark
class spell_mage_radiant_spark : public AuraScript
{
    PrepareAuraScript(spell_mage_radiant_spark);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_RADIANT_SPARK_PROC_BLOCKER });
    }

    bool CheckProc(AuraEffect const* /*aurEff*/, ProcEventInfo& procInfo)
    {
        return !procInfo.GetProcTarget()->HasAura(SPELL_MAGE_RADIANT_SPARK_PROC_BLOCKER, GetCasterGUID());
    }

    void HandleProc(AuraEffect* aurEff, ProcEventInfo& procInfo)
    {
        Aura* vulnerability = procInfo.GetProcTarget()->GetAura(aurEff->GetSpellEffectInfo().TriggerSpell, GetCasterGUID());
        if (vulnerability && vulnerability->GetStackAmount() == vulnerability->CalcMaxStackAmount())
        {
            PreventDefaultAction();
            vulnerability->Remove();
            GetTarget()->CastSpell(GetTarget(), SPELL_MAGE_RADIANT_SPARK_PROC_BLOCKER, true);
        }
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_mage_radiant_spark::HandleProc, EFFECT_2, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

void AddSC_DekkCore_mage_spell_scripts()
{
    new playerscript_mage_arcane();
    new spell_mage_combustion();
    new spell_mage_fire_mage_passive();
    new spell_mage_fire_on();
    new spell_mage_mirror_image_summon();
    RegisterSpellScript(spell_mage_ebonbolt);
    RegisterSpellScript(spell_mage_ebonbolt_damage);
    RegisterSpellScript(spell_mage_flurry);
    RegisterSpellScript(spell_mage_pyroblast);
    RegisterSpellScript(spell_mage_meteor);
    RegisterSpellScript(spell_mage_meteor_damage);
    RegisterSpellScript(spell_mage_fire_blast);
    RegisterSpellScript(spell_mage_arcane_missiles_damage);

    RegisterSpellScript(spell_mage_chrono_shift);
    RegisterSpellScript(spell_mage_arcane_missiles);
    RegisterSpellScript(spell_mage_conflagration);
    RegisterSpellScript(spell_mage_enhanced_pyrotechnics);
    RegisterSpellScript(spell_mage_frenetic_speed);
    RegisterSpellScript(spell_mage_kindling);
    RegisterSpellScript(spell_mage_pyroblast_clearcasting_driver);
    RegisterSpellScript(spell_mage_chilled_to_the_core);
    RegisterSpellScript(spell_mage_chain_reaction);
    RegisterSpellScript(spell_mage_jouster);
    RegisterSpellScript(spell_mage_jouster_buff);
    RegisterSpellScript(spell_mage_chilled);
    RegisterSpellScript(spell_mage_ignite_periodic);
    RegisterSpellScript(spell_mage_wildfire);
    RegisterSpellScript(spell_mage_wildfire_talent);
    RegisterSpellScript(spell_mage_wildfire_periodic);
    RegisterSpellScript(spell_mage_firestarter_pvp);
    RegisterSpellScript(spell_mage_flamestrike);
    RegisterSpellScript(spell_mage_cinderstorm);
    RegisterSpellScript(spell_mage_clearcasting);
    RegisterSpellScript(spell_mage_presence_of_mind);
    RegisterSpellScript(spell_mage_arcane_blast);
    new at_mage_meteor_timer();
    new at_mage_meteor_burn();
    new at_mage_blizzard();
    RegisterAreaTriggerAI(at_mage_rune_of_power);
    RegisterAreaTriggerAI(at_mage_frozen_orb);
    RegisterAreaTriggerAI(at_mage_arcane_orb);
    RegisterAreaTriggerAI(at_mage_flame_patch);
    RegisterAreaTriggerAI(at_mage_cinderstorm);
    new npc_mirror_image();
    RegisterSpellScript(spell_mage_blink);
    new spell_mage_snowdrift();
    new spell_mage_alter_time();
    RegisterSpellScript(spell_mage_overflowing_energy);
    RegisterAuraScript(spell_mastery_icicles_proc);
    RegisterAuraScript(spell_mastery_icicles_periodic);
    RegisterAuraScript(spell_mastery_icicles_mod_aura);
    RegisterSpellScript(spell_mage_frozen_orb);
    RegisterSpellScript(spell_mage_nova_talent);
    RegisterSpellScript(spell_mage_snap_freeze);
    new spell_mage_glacial_assault();
    new spell_mage_incantation_of_swiftness();
    RegisterAuraScript(spell_mage_ice_floes);
    new spell_mage_snowstorm();
    new spell_mage_flash_freeze();
    new spell_mage_freezing_cold();
    RegisterAuraScript(spell_mage_shifting_power_aura);
    new spell_mage_time_anomaly();
    RegisterSpellScript(spell_mage_alter_time_aura);
    RegisterSpellScript(spell_mage_alter_time_active);
    RegisterSpellScript(spell_mage_arcane_barrage);
    RegisterSpellScript(spell_mage_arcane_charge_clear);
    RegisterSpellScript(spell_mage_arcane_explosion);
    RegisterSpellScript(spell_mage_blazing_barrier);
    RegisterAreaTriggerAI(areatrigger_mage_blizzard);
    RegisterSpellScript(spell_mage_blizzard_damage);
    RegisterSpellScript(spell_mage_burning_determination);
    RegisterSpellAndAuraScriptPair(spell_mage_cauterize, spell_mage_cauterize_AuraScript);
    RegisterSpellScript(spell_mage_cold_snap);
    RegisterSpellScript(spell_mage_comet_storm);
    RegisterSpellScript(spell_mage_comet_storm_damage);
    RegisterSpellScript(spell_mage_cone_of_cold);
    RegisterSpellScript(spell_mage_conjure_refreshment);
    RegisterSpellScript(spell_mage_fingers_of_frost);
    RegisterSpellScript(spell_mage_firestarter);
    RegisterSpellScript(spell_mage_firestarter_dots);
    RegisterSpellScript(spell_mage_flame_on);
    RegisterSpellScript(spell_mage_frostbolt);
    RegisterSpellScript(spell_mage_ice_barrier);
    RegisterSpellScript(spell_mage_ice_block);
    RegisterSpellScript(spell_mage_ice_lance);
    RegisterSpellScript(spell_mage_ice_lance_damage);
    RegisterSpellScript(spell_mage_icicle_damage);
    RegisterSpellScript(spell_mage_ignite);
    RegisterSpellScript(spell_mage_imp_mana_gems);
    RegisterSpellScript(spell_mage_incanters_flow);
    RegisterSpellScript(spell_mage_living_bomb);
    RegisterSpellScript(spell_mage_living_bomb_explosion);
    RegisterSpellScript(spell_mage_living_bomb_periodic);
    RegisterSpellScript(spell_mage_polymorph_visual);
    RegisterSpellScript(spell_mage_prismatic_barrier);
    RegisterSpellScript(spell_mage_ring_of_frost);
    RegisterSpellAndAuraScriptPair(spell_mage_ring_of_frost_freeze, spell_mage_ring_of_frost_freeze_AuraScript);
    RegisterSpellScript(spell_mage_supernova);
    RegisterSpellScript(spell_mage_touch_of_the_magi_aura);
    RegisterSpellScript(spell_mage_water_elemental_freeze);
    RegisterSpellAndAuraScriptPair(spell_mage_ray_of_frost, spell_mage_ray_of_frost_aura);
    RegisterSpellScript(spell_mage_ethereal_blink);
    RegisterSpellScript(spell_mage_ethereal_blink_triggered);
    RegisterSpellScript(spell_mage_feel_the_burn);
    RegisterSpellScript(spell_mage_hyper_impact);
    RegisterSpellScript(spell_mage_radiant_spark);
}
