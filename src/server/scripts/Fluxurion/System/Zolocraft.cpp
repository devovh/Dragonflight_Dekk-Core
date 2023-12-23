/*
 * Copyright 2023 Fluxurion
 */

/*
..::ZOLOCRAFT (modified solocraft) by Fluxurion (2023)::..

To worldserver.conf:

###################################################################################################
#
#   ..::ZOLOCRAFT (modified solocraft) by Fluxurion (2023)::..
#
#   ZoloCraft is an aura with rewrited solocraft, added more functions and more stat increase.
# - All stat increase handled by an aura.
# - Player instantly can be resurrected after killed by a creature or committed suicide in dungeons or raids.
# - Checking for ZoloCraft Aura when a player enters a dungeon or raid.
# - Checking for ZoloCraft Aura when a player dies.
# - Checking for ZoloCraft Aura when a player logs in inside a dungeon or raid.
# - Available in Normal Dungeon, Heroic Dungeon, Mythic Dungeon, Mythic+ Dungeon, Raid, 25Man Raid, Heroic Raid, Mythic Raid
# - Increasing the following stats:
#     basic stats: spell power, agi, str, int, stam, armor
#     ratings: crit, hit, versatility, leech, etc..
# - After apply the player gets full hp and full mana.
# - It has option to remove cooldowns and cast times while it's applied.
# - Extra buffs can be applied. e.g.: night fae blessings, inspired, amplified, augment rune, well fed..etc
#
#   Enable = 1, Disable = 0, Duration is in minute.
#

ZoloCraft.Enable = 1
ZoloCraft.StatMultiplier = 10
ZoloCraft.RatingMultiplier = 5
ZoloCraft.InstantRess = 1
ZoloCraft.CooldownCheat = 1
ZoloCraft.CastTimeCheat = 1
ZoloCraft.ExtraBuffs = 0
ZoloCraft.ExtraBuffs.Duration = 120
ZoloCraft.TeachForGMs = 1

#
###################################################################################################

To database:

DELETE FROM `spell_script_names` WHERE `spell_id`=167349 AND `ScriptName`='spell_zolocraft_aura_167349';
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES (167349, 'spell_zolocraft_aura_167349');

*/


#include "Chat.h"
#include "Config.h"
#include "Group.h"
#include "InstanceScript.h"
#include "Log.h"
#include "Map.h"
#include "Player.h"
#include "Pet.h"
#include "ScriptMgr.h"
#include "SpellHistory.h"
#include "SpellMgr.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "Unit.h"
#include "WorldSession.h"

enum ZoloCraft
{
    // Spells
    ZoloCraftAura = 167349, // 333187 another can be
    // Spell Visuals
    Deaths_Due = 104151, 
    Elysian_Decree = 102131, // good
    Convoke_the_Spirits = 96283, // not good
    BigPurpleSpark = 95899, // good
    FleshcraftOrb = 96659, // not good
    Radiant_Spark = 90436,
    Faeline_Stomp = 97433,
    Decimating_Bolt_Casting = 97003, // good
    Decimating_Bolt_Shoot = 97416, // small not good
    WaveWithAnimaInHand = 97419, // ok
    ArcaneBuffEffect = 97504, // ok
    BigBlueSplash = 95243,
    FaeTransfusionBlueMagicSmoke = 98148, // very good
    GreenForceOnBody = 112374, // ok
    WindRunnerDarkJumpingUp = 110618,

    // using on activate:  GreenForceOnBody FaeTransfusionBlueMagicSmoke Elysian_Decree
    // using on remove: WaveWithAnimaInHand
};

class zolocraft_player_instance_handler : public PlayerScript {
public:
	zolocraft_player_instance_handler() : PlayerScript("zolocraft_player_instance_handler")
	{
		TC_LOG_INFO("entities.player", "[ZoloCraft] zolocraft_player_instance_handler Loaded");
	}

    // if the player has ZoloCraft Aura his cooldowns are remain for ZoloCraft.ExtraBuffs.Duration's value like 2hour avenging wrath etc
    void OnSpellCast(Player* player, Spell* spell, bool /*skipCheck*/) override
    {
        if (player->HasAura(ZoloCraftAura))
        {
            const SpellInfo* spinf = spell->GetSpellInfo();
            if (spinf->RecoveryTime > 59 && spinf->HasEffect(SpellEffectName::SPELL_EFFECT_APPLY_AURA))
            {
                spell->SetSpellValue(SpellValueMod::SPELLVALUE_DURATION, sConfigMgr->GetIntDefault("ZoloCraft.ExtraBuffs.Duration", 120) * 60000);
            }
        }
    }

	void OnLogin(Player* player, bool /*firstLogin*/) override
	{
        if (sConfigMgr->GetBoolDefault("ZoloCraft.Enable", false))
        {
            ChatHandler(player->GetSession()).SendSysMessage("[ZoloCraft] Power Increase System is enabled in Dungeons & Raids!");

            if (sConfigMgr->GetBoolDefault("ZoloCraft.TeachForGMs", true) && !player->HasSpell(ZoloCraftAura))
                player->LearnSpell(ZoloCraftAura, false);
        }
        else
        {
            if (player->HasAura(ZoloCraftAura))
                player->RemoveAurasDueToSpell(ZoloCraftAura);
        }

		CheckForZolocraft(player);
	}

	void OnLogout(Player* player) override
	{
		if (player->HasAura(ZoloCraftAura))
			player->RemoveAurasDueToSpell(ZoloCraftAura);
	}

	// When a player changes map (e.g.: entering a dungeon) we check if the player is eligible for ZoloCraft
	void OnMapChanged(Player* player) override
	{
		CheckForZolocraft(player);
	}

	// If the player dies by a creature in an instance we check if the player is eligible for InstantRess and ZoloCraft
	void OnPlayerKilledByCreature(Creature* /*killer*/, Player* player) override
	{
		Map* map = player->GetMap();
		int difficulty = CalculateDifficulty(map, player);

		// if InstantRess is enabled in config and the player is in instance
		if (sConfigMgr->GetBoolDefault("ZoloCraft.InstantRess", false) && difficulty > 1)
			player->ResurrectPlayer(100);

		// + check for ZoloCraft
		CheckForZolocraft(player);
	}

	// When a player committed suicide (xD yeah it happens)
	void OnPVPKill(Player* killer, Player* killed) override
	{
		Map* map = killed->GetMap();
		int difficulty = CalculateDifficulty(map, killed);

		if (killer == killed)
		{
			// if InstantRess is enabled in config and the player is in instance
			if (sConfigMgr->GetBoolDefault("ZoloCraft.InstantRess", false) && difficulty > 1)
				killed->ResurrectPlayer(100);

			// + check for ZoloCraft
			CheckForZolocraft(killed);
		}
	}

	// Check if the player is eligible for ZoloCraft
	void CheckForZolocraft(Player* player)
	{
        if (sConfigMgr->GetBoolDefault("ZoloCraft.Enable", false))
        {
            Map* map = player->GetMap();
            float difficulty = CalculateDifficulty(map, player);

            // If the map is NOT instance and the player HAS the ZoloCraftAura we remove
            if (difficulty > 1)
                player->CastSpell(player, ZoloCraftAura, true);
            else
                player->RemoveAurasDueToSpell(ZoloCraftAura);
        }
	}

	float CalculateDifficulty(Map* map, Player* player)
	{
		float difficulty = 1.f;
		if (map)
		{
			if (map->IsDungeon())
				difficulty = 1.5f;

			if (map->IsHeroic())
				difficulty = 2.f; // timewalking dungeon, 

			if (map->GetId() == 1779)
				difficulty = 2.5f; // invasion point, 

			if (map->IsRaid())
				difficulty = 3.f;

			if (map->Is25ManRaid())
				difficulty = 3.5f;
		}

        // commented out (too overpowered)
        // Difficulty for GM's while not in any instance
        //if (player->CanBeGameMaster() && difficulty == 1)
            //difficulty = 100;

		return difficulty;
	}
};

// ZoloCraftAura AuraScript - to handle zolocraft switching with aura
class spell_zolocraft_aura_167349 : public AuraScript
{
	PrepareAuraScript(spell_zolocraft_aura_167349);

    int config_statmultiplier = sConfigMgr->GetIntDefault("ZoloCraft.StatMultiplier", 2);
    int config_ratingmultiplier = sConfigMgr->GetIntDefault("ZoloCraft.RatingMultiplier", 10);
    int extrabuff_duration = sConfigMgr->GetIntDefault("ZoloCraft.ExtraBuffs.Duration", 120);
    std::list <uint32> extrabuffs
    {
                328282, // Blessing of Spring (Night Fae)
                328620, // Blessing of Summer (Night Fae)
                328622, // Blessing of Autumn (Night Fae)
                328281, // Blessing of Winter (Night Fae)
                224001, // Defiled Augmentation (Level 60)
                215607, // Well Fed (Level 60)
                215598, // Holy Enchantment
                215387, // Polished Armor
                214336, // Imbued
                214337, // Amplified
                214338, // Enhanced
                235727, // Inspired
                199368, // Legacy of the Ravencrest
    };
    float final_multiplier_stat, final_multiplier_rat, base_agi, base_int, base_stam, base_str,
        base_spellpower, base_manaregen, base_mastery, base_multistrike, base_lifesteal,
        base_avoidance, base_dodge, base_parry, base_block, base_defense, base_armor,
        base_hitm, base_hitr, base_hits, base_critm, base_critr, base_crits, base_speed,
        base_hastem, base_haster, base_hastes, base_versa_dmgdone, base_versa_healdone, base_versa_dmgtaken;

	void OnApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
	{
		if (Unit* caster = GetCaster())
			if (caster->IsPlayer())
			{
				Player* player = caster->ToPlayer();
				Map* map = player->GetMap();
				int difficulty = CalculateDifficulty(map, player);
				int numInGroup = GetNumInGroup(player);

				ApplyZoloCraft(player, map, difficulty, numInGroup);
			}
	}

	void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
	{
		if (Unit* caster = GetCaster())
			if (caster->IsPlayer())
			{
				Player* player = caster->ToPlayer();
				Map* map = player->GetMap();
				int difficulty = CalculateDifficulty(map, player);
				int numInGroup = GetNumInGroup(player);

				ClearZoloCraft(player, map, difficulty, numInGroup);
			}
	}

	void Register() override
	{
		OnEffectApply += AuraEffectApplyFn(spell_zolocraft_aura_167349::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
		AfterEffectRemove += AuraEffectRemoveFn(spell_zolocraft_aura_167349::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
	}

	// Normal Dungeon, Heroic Dungeon, Mythic Dungeon, Mythic+ Dungeon, Raid, 25Man Raid, Heroic Raid, Mythic Raid
    float CalculateDifficulty(Map* map, Player* player)
    {
        float difficulty = 1;
        if (map)
        {
            if (map->IsDungeon())
                difficulty = 1.5;

            if (map->IsHeroic())
                difficulty = 2; // timewalking dungeon, 

            if (map->GetId() == 1779)
                difficulty = 2.5; // invasion point, 

            if (map->IsRaid())
                difficulty = 3;

            if (map->Is25ManRaid())
                difficulty = 3.5;
        }

        // commented out (too overpowered)
        // Difficulty for GM's while not in any instance
        //if (player->CanBeGameMaster() && difficulty == 1)
            //difficulty = 100;

        return difficulty;
    }

	int GetNumInGroup(Player* player)
	{
		int numInGroup = 1;
		Group* group = player->GetGroup();
		if (group) {
			Group::MemberSlotList const& groupMembers = group->GetMemberSlots();
			numInGroup = groupMembers.size();
		}
		return numInGroup;
	}

	void ApplyZoloCraft(Player* player, Map* map, float difficulty, int numInGroup)
	{
		ChatHandler(player->GetSession()).PSendSysMessage("Welcome to the {}! Power Increase System activated. Difficulty: {}", map->GetMapName(), int(difficulty));

		TC_LOG_INFO("entities.player", "[ZoloCraft] Player {} entered map: {} (mapid: {}), difficulty is {}.", player->GetName().c_str(), map->GetMapName(), map->GetId(), int(difficulty));

        player->GetSpellHistory()->ResetAllCooldowns();
        player->GetSpellHistory()->ResetAllCharges();
        ChatHandler(player->GetSession()).PSendSysMessage(LANG_REMOVEALL_COOLDOWN, player->GetName().c_str());

		base_agi = (1500 / 60)* player->GetLevel(); //player->GetStat(Stats::STAT_AGILITY);
		base_int = (3000 / 60)* player->GetLevel(); //player->GetStat(Stats::STAT_INTELLECT);
		base_stam = (3000 / 60)* player->GetLevel(); //player->GetStat(Stats::STAT_STAMINA);
		base_str = (1500 / 60)* player->GetLevel(); //player->GetStat(Stats::STAT_STRENGTH);
		base_armor = (4147 / 60)* player->GetLevel(); //player->GetArmor();
		base_spellpower = (10000 / 60)* player->GetLevel(); //(player->GetTotalSpellPowerValue(SpellSchoolMask::SPELL_SCHOOL_MASK_ALL, false) + player->GetTotalSpellPowerValue(SpellSchoolMask::SPELL_SCHOOL_MASK_ALL, true)) / 2;
		base_mastery = (1000 / 60)* player->GetLevel();; //player->GetRatingMultiplier(CR_MASTERY);
		base_lifesteal = (1000 / 60)* player->GetLevel();; //player->GetRatingMultiplier(CR_LIFESTEAL);
		base_avoidance = (1000 / 60)* player->GetLevel();; //player->GetRatingMultiplier(CR_AVOIDANCE);
		base_dodge = (1000 / 60)* player->GetLevel();; //player->GetRatingMultiplier(CR_DODGE);
		base_parry = (1000 / 60)* player->GetLevel();; //player->GetRatingMultiplier(CR_PARRY);
		base_block = (1000 / 60)* player->GetLevel();; //player->GetRatingMultiplier(CR_BLOCK);
		base_defense = (1000 / 60)* player->GetLevel();; //player->GetRatingMultiplier(CR_DEFENSE_SKILL);
		base_hitm = (1000 / 60)* player->GetLevel();; //player->GetRatingMultiplier(CR_HIT_MELEE);
		base_hitr = (1000 / 60)* player->GetLevel();; //player->GetRatingMultiplier(CR_HIT_RANGED);
		base_hits = (1000 / 60)* player->GetLevel();; //player->GetRatingMultiplier(CR_HIT_SPELL);
		base_critm = (3500 / 60)* player->GetLevel();; //player->GetRatingMultiplier(CR_CRIT_MELEE);
		base_critr = (3500 / 60)* player->GetLevel();; //player->GetRatingMultiplier(CR_CRIT_RANGED);
		base_crits = (3500 / 60)* player->GetLevel();; //player->GetRatingMultiplier(CR_CRIT_SPELL);
		base_hastem = (3300 / 60)* player->GetLevel();; //player->GetRatingMultiplier(CR_HASTE_MELEE);
		base_haster = (3300 / 60)* player->GetLevel();; //player->GetRatingMultiplier(CR_HASTE_RANGED);
		base_hastes = (3300 / 60)* player->GetLevel();; //player->GetRatingMultiplier(CR_HASTE_SPELL);
		base_speed = (1000 / 60)* player->GetLevel();; //player->GetRatingMultiplier(CR_SPEED);
		base_versa_dmgdone = (4000 / 60)* player->GetLevel();; //player->GetRatingMultiplier(CR_VERSATILITY_DAMAGE_DONE);
		base_versa_healdone = (4000 / 60)* player->GetLevel();; //player->GetRatingMultiplier(CR_VERSATILITY_HEALING_DONE);
        base_versa_dmgtaken = (8000 / 60)* player->GetLevel();; //player->GetRatingMultiplier(CR_VERSATILITY_DAMAGE_TAKEN);

		final_multiplier_stat = (config_statmultiplier * difficulty) / numInGroup;
		final_multiplier_rat = (config_ratingmultiplier * difficulty) / numInGroup;

		SwitchIncreasedStats(player, true, difficulty, numInGroup);

        // cool visual effects
        GetTarget()->SendPlaySpellVisual(GetTarget(), GreenForceOnBody, 0, 0, 60, false);
        GetTarget()->SendPlaySpellVisual(GetTarget(), FaeTransfusionBlueMagicSmoke, 0, 0, 60, false);
        GetTarget()->SendPlaySpellVisual(GetTarget(), Elysian_Decree, 0, 0, 60, false);

		// Cooldown Cheat (be careful)
		if (sConfigMgr->GetBoolDefault("ZoloCraft.CooldownCheat", false)) 
		{
			player->SetCommandStatusOn(CHEAT_COOLDOWN);
			ChatHandler(player->GetSession()).PSendSysMessage("Cooldown turned off.");
		}

		// Casttime Cheat (be careful)
		if (sConfigMgr->GetBoolDefault("ZoloCraft.CastTimeCheat", false)) 
		{
			player->SetCommandStatusOn(CHEAT_CASTTIME);
			ChatHandler(player->GetSession()).PSendSysMessage("Instant cast turned on.");
		}

		// Option for extra buffs
		if (sConfigMgr->GetBoolDefault("ZoloCraft.ExtraBuffs", false)) 
		{
            // Absorb shield
            CastSpellExtraArgs absorbargs;
            absorbargs.SetTriggerFlags(TRIGGERED_FULL_MASK);
            absorbargs.AddSpellMod(SPELLVALUE_BASE_POINT0, player->GetHealth() * 100); // 100 * player's hp absorb
            absorbargs.AddSpellMod(SPELLVALUE_DURATION, extrabuff_duration * 60 * 1000); // *min

            player->CastSpell(player, 324867, absorbargs); // fleshcraft shield of covenant ability

            // All other buff spell
            CastSpellExtraArgs buffargs;
            buffargs.SetTriggerFlags(TRIGGERED_FULL_MASK);
            buffargs.AddSpellMod(SPELLVALUE_DURATION, extrabuff_duration * 60 * 1000); // *min

            for (uint32 extrabuff : extrabuffs)
                player->CastSpell(player, extrabuff, buffargs);

			ChatHandler(player->GetSession()).PSendSysMessage("|cffB400B4 Extra buffs applied!");
		}

        // Healing up
        player->SetFullHealth();

        // Filling up the mana if the player has
        if (player->GetPowerType() == POWER_MANA)
            player->SetPower(POWER_MANA, player->GetMaxPower(POWER_MANA));
	}

	void ClearZoloCraft(Player* player, Map* map, float difficulty, int numInGroup)
	{
		ChatHandler(player->GetSession()).PSendSysMessage("Power Increase System deactivated.");

		SwitchIncreasedStats(player, false, difficulty, numInGroup);

		if (sConfigMgr->GetBoolDefault("ZoloCraft.CooldownCheat", false)) 
		{
			player->SetCommandStatusOff(CHEAT_COOLDOWN);
			ChatHandler(player->GetSession()).PSendSysMessage("Cooldown turned back on.");
		}

		if (sConfigMgr->GetBoolDefault("ZoloCraft.CastTimeCheat", false)) 
		{
			player->SetCommandStatusOff(CHEAT_CASTTIME);
			ChatHandler(player->GetSession()).PSendSysMessage("Instant cast turned off.");
		}

		if (sConfigMgr->GetBoolDefault("ZoloCraft.ExtraBuffs", false)) 
		{
            for (uint32 extrabuff : extrabuffs)
			    player->RemoveAurasDueToSpell(extrabuff);

			player->RemoveAurasDueToSpell(324867);
		}

		// Refresh stats
		player->InitStatsForLevel();
		player->SetCanModifyStats(true);
		player->UpdateAllStats();

        // Healing up
        player->SetFullHealth();

        // Filling up the mana if the player has
        if (player->GetPowerType() == POWER_MANA)
            player->SetPower(POWER_MANA, player->GetMaxPower(POWER_MANA));

        GetTarget()->SendPlaySpellVisual(GetTarget(), WaveWithAnimaInHand, 0, 0, 60, false);
	}

	void SwitchIncreasedStats(Player* player, bool onoff, float difficulty, int numInGroup)
	{
		// Increasing the following stats: 
		//	  agility, strength, intellect, stamina, avoidance, dodge, parry, block, defense,
		//	  hit rating, crit rating, haste rating, multistrike, mastery, life steal, run speed, versatility

		TC_LOG_INFO("entities.player", "[ZoloCraft] Player {}'s multiplier for stats: {}, multiplier for ratings: {}.", player->GetName().c_str(), final_multiplier_stat, final_multiplier_rat);

        ChatHandler(player->GetSession()).PSendSysMessage("[ZoloCraft] Your multiplier for stats: %u, multiplier for ratings: %u.", uint32(final_multiplier_stat), uint32(final_multiplier_rat));

		// Increase basic stats
		player->HandleStatFlatModifier(UnitMods::UNIT_MOD_STAT_AGILITY, UnitModifierFlatType::TOTAL_VALUE, final_multiplier_stat * base_agi, onoff);
		player->HandleStatFlatModifier(UnitMods::UNIT_MOD_STAT_STRENGTH, UnitModifierFlatType::TOTAL_VALUE, final_multiplier_stat * base_str, onoff);
		player->HandleStatFlatModifier(UnitMods::UNIT_MOD_STAT_INTELLECT, UnitModifierFlatType::TOTAL_VALUE, final_multiplier_stat * base_int, onoff);
		player->HandleStatFlatModifier(UnitMods::UNIT_MOD_STAT_STAMINA, UnitModifierFlatType::TOTAL_VALUE, final_multiplier_stat * base_stam, onoff);
		player->HandleStatFlatModifier(UnitMods::UNIT_MOD_ARMOR, UnitModifierFlatType::TOTAL_VALUE, final_multiplier_stat * base_armor, onoff);
		// Increase spellpower + manaregen
		player->ApplySpellPowerBonus(final_multiplier_stat * base_spellpower, onoff);
		player->ApplyManaRegenBonus(final_multiplier_stat, onoff);
		// Increase defense
		player->ApplyRatingMod(CombatRating::CR_AVOIDANCE, final_multiplier_rat * base_avoidance, onoff);
		player->ApplyRatingMod(CombatRating::CR_DODGE, final_multiplier_rat * base_dodge, onoff);
		player->ApplyRatingMod(CombatRating::CR_PARRY, final_multiplier_rat * base_parry, onoff);
		player->ApplyRatingMod(CombatRating::CR_BLOCK, final_multiplier_rat * base_block, onoff);
		player->ApplyRatingMod(CombatRating::CR_DEFENSE_SKILL, final_multiplier_rat * base_defense, onoff);
		// Increase hit
		player->ApplyRatingMod(CombatRating::CR_HIT_MELEE, final_multiplier_rat * base_hitm, onoff);
		player->ApplyRatingMod(CombatRating::CR_HIT_RANGED, final_multiplier_rat * base_hitr, onoff);
		player->ApplyRatingMod(CombatRating::CR_HIT_SPELL, final_multiplier_rat * base_hits, onoff);
		// Increase crit
		player->ApplyRatingMod(CombatRating::CR_CRIT_MELEE, final_multiplier_rat * base_critm, onoff);
		player->ApplyRatingMod(CombatRating::CR_CRIT_RANGED, final_multiplier_rat * base_critr, onoff);
		player->ApplyRatingMod(CombatRating::CR_CRIT_SPELL, final_multiplier_rat * base_crits, onoff);
		// Increase haste
		player->ApplyRatingMod(CombatRating::CR_HASTE_MELEE, final_multiplier_rat * base_hastem, onoff);
		player->ApplyRatingMod(CombatRating::CR_HASTE_RANGED, final_multiplier_rat * base_haster, onoff);
		player->ApplyRatingMod(CombatRating::CR_HASTE_SPELL, final_multiplier_rat * base_hastes, onoff);
		// Increase versatility
		player->ApplyRatingMod(CombatRating::CR_VERSATILITY_DAMAGE_DONE, final_multiplier_rat * base_versa_dmgdone, onoff);
		player->ApplyRatingMod(CombatRating::CR_VERSATILITY_HEALING_DONE, final_multiplier_rat * base_versa_healdone, onoff);
		player->ApplyRatingMod(CombatRating::CR_VERSATILITY_DAMAGE_TAKEN, final_multiplier_rat * base_versa_dmgtaken, onoff);
		// Increase mastery
		player->ApplyRatingMod(CombatRating::CR_MASTERY, base_mastery * final_multiplier_rat, onoff);
		// Increase lifesteal
		player->ApplyRatingMod(CombatRating::CR_LIFESTEAL, base_lifesteal * final_multiplier_rat, onoff);
		// Increase speed
		player->ApplyRatingMod(CombatRating::CR_SPEED, base_speed * final_multiplier_rat, onoff);

	}

};


void AddSC_Zolocraft()
{
	new zolocraft_player_instance_handler();
	RegisterAuraScript(spell_zolocraft_aura_167349);
}
