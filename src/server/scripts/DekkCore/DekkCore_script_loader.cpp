/*
 * Copyright 2022 DekkCore
 * 	// Project Zereth
 */
 
// Info: Call all dekkcore script here..
 

/* ######### Continents ######## > */
void Add_DekkCore_DragonflightScripts();
void Add_DekkCore_ShadowlandsScripts();
void Add_DekkCore_BFAScripts();
void Add_DekkCore_LegionScripts();
void Add_DekkCore_EasternKingdomsScripts();
void Add_DekkCore_PandariaScripts();
/* < ######### Continents ######## */


/* ######### Spells ######## > */
void AddSC_DekkCore_warrior_spell_scripts();
void AddSC_DekkCore_warlock_spell_scripts();
void AddSC_DekkCore_shaman_spell_scripts();
void AddSC_DekkCore_rogue_spell_scripts();
void AddSC_DekkCore_priest_spell_scripts();
void AddSC_DekkCore_paladin_spell_scripts();
void AddSC_DekkCore_monk_spell_scripts();
void AddSC_DekkCore_mage_spell_scripts();
void AddSC_DekkCore_hunter_spell_scripts();
void AddSC_DekkCore_druid_spell_scripts();
void AddSC_DekkCore_deathknight_spell_scripts();
void AddSC_DekkCore_demon_hunter_spell_scripts();
void AddSC_DekkCore_generic_spell_scripts();
void AddSC_DekkCore_evoker_spell_scripts();
void AddSC_dragonriding_spell_scripts();
/* < ######### Spells ########  */


/* ######### Others ######## > */
void AddSC_garrison_instance();
void AddSC_Allied_Race_Scripts();
/* < ######### Others ########  */


/* ######### Custom ######## > */
void AddCustom_npc();
void AddCustom_playerscript();
void AddSC_Custom_Spells();
void AddSC_Custom_Commands();
void AddSC_Azgath_Custom_Scripts();
/* < ######### Custom ########  */

// Scenarios 
void AddSC_stormwind_extraction();
void AddSC_scenario_stormwind_extraction();
void AddSC_scenario_zuldazar();
void AddSC_instance_scenario_zuldazar();
void AddSC_uncharted_island_tutorial();
void AddSC_scenario_uncharted_island_tutorial();
void AddSC_scenario_snowblossom_village();
void AddSC_scenario_draenor_shadowmoon_valley();
void AddSC_scenario_frostfire_finale();
void AddSC_scenario_talador_iron_horde_finale();
void AddSC_scenario_grommashar();
void AddSC_scenario_gorgrond_finale_1();
void AddSC_scenario_verdant_wilds();
void AddSC_scenario_ungol_ruins();
void AddSC_brokenIslands();
void AddSC_instance_broken_islands();
void AddSC_instance_a_brewing_storm();
void AddSC_a_brewing_storm();
void AddSC_instance_troves_of_the_thunder_king();
void AddSC_troves_of_the_thunder_king();
void AddSC_DH_vengeance_art_scenario();
void AddSC_arena_of_annihiliation();
void AddSC_instance_instance_arena_of_annihiliation();
void AddSC_instance_secrets_of_ragefire();
void AddSC_scenario_the_secrets_of_ragefire();
void AddSC_secrets_of_ragefire();

// Draenor
void AddSC_frostfire_ridge();
void AddSC_shadowmoon_draenor();
void AddSC_garrison_level_1();
void AddSC_garrison_level_2();
void AddSC_garrison_level_3();
void AddSC_spells_garrison();
void AddSC_instance_everbloom(); // Everbloom
void AddSC_the_everbloom();
void AddSC_boss_ancient_protectors();
void AddSC_boss_sol();
void AddSC_boss_witherbark();
void AddSC_boss_xeritac();
void AddSC_boss_yalnu();
void AddSC_auchindoun(); //Auchindom
void AddSC_boss_azaakel();
void AddSC_boss_kaathar();
void AddSC_boss_nyami();
void AddSC_boss_teronogor();
void AddSC_instance_auchindoun();
void AddSC_tanaan_intro_blackrock(); //Tanaan Intro
void AddSC_tanaan_intro_bleeding_hollow();
void AddSC_tanaan_intro_finale();
void AddSC_tanaan_intro_global();
void AddSC_tanaan_intro_portal();
void AddSC_tanaan_intro_shadowmoon();
void AddSC_tanaan_intro_shattered_hand();
void AddSC_tanaan_jungle();
void AddSC_bonemaw();
void AddSC_nerzul();
void AddSC_shadowmoon_burial_grounds();
void AddSC_sadana();
void AddSC_nhalish();
void AddSC_instance_shadowmoon_burial_grounds();
void AddSC_boss_grimrail();
void AddSC_boss_nokgar();
void AddSC_boss_oshir();
void AddSC_boss_skulloc();
void AddSC_instance_iron_docks();
void AddSC_iron_docks();
void AddSC_boss_nitrogg_thundertower(); //GRIMRAIL DEPOT
void AddSC_RocketsparkandBorka();
void AddSC_boss_skylord_tovra();
void AddSC_grimrail_depot();
void AddSC_instance_grimrail_depot();
void AddSC_areatrigger_Bloodmaul(); //BloodMaul Slag Mines
void AddSC_boss_forgemaster_gogduh();
void AddSC_boss_gugrokk();
void AddSC_boss_roltall();
void AddSC_boss_SlaveWatcherCrushto();
void AddSC_instance_Bloodmaul();
void AddSC_mob_Bloodmaul();
void AddSC_spell_Bloodmaul();
void AddSC_gorgrond();
void AddSC_spires_of_arak();
void AddSC_talador();

void AddBrawlersGuildScripts();

// #################################################### //
// Add them to AddDekkCoreScripts(){ .. } without void. //
// #################################################### //

void AddDekkCoreScripts()
{
    /* ######### Continents ######## > */
    Add_DekkCore_DragonflightScripts();
	Add_DekkCore_ShadowlandsScripts();
	Add_DekkCore_BFAScripts();
    Add_DekkCore_LegionScripts();
    Add_DekkCore_EasternKingdomsScripts();
    Add_DekkCore_PandariaScripts();
	/* < ######### Continents ######## */
	
	/* ######### Spells ######## > */
    AddSC_DekkCore_warrior_spell_scripts();
    AddSC_DekkCore_warlock_spell_scripts();
    AddSC_DekkCore_shaman_spell_scripts();
    AddSC_DekkCore_rogue_spell_scripts();
    AddSC_DekkCore_priest_spell_scripts();
    AddSC_DekkCore_paladin_spell_scripts();
    AddSC_DekkCore_monk_spell_scripts();
    AddSC_DekkCore_mage_spell_scripts();
    AddSC_DekkCore_hunter_spell_scripts();
    AddSC_DekkCore_druid_spell_scripts();
    AddSC_DekkCore_deathknight_spell_scripts();
    AddSC_DekkCore_demon_hunter_spell_scripts();
    AddSC_DekkCore_generic_spell_scripts();
    AddSC_DekkCore_evoker_spell_scripts();
    AddSC_dragonriding_spell_scripts();
    /* < ######### Spells ######## */
	

    /* ######### Others ######## > */
    AddSC_garrison_instance();
    AddSC_Allied_Race_Scripts();
    /* < ######### Others ########  */


    /* ######### Custom ######## > */
    AddCustom_npc();
    AddCustom_playerscript();
    AddSC_Custom_Spells();
    AddSC_Custom_Commands();
	AddSC_Azgath_Custom_Scripts();
    /* < ######### Custom ########  */

    //Scenarios
    AddSC_stormwind_extraction();
    AddSC_scenario_stormwind_extraction();
    AddSC_scenario_zuldazar();
    AddSC_instance_scenario_zuldazar();
    AddSC_uncharted_island_tutorial();
    AddSC_scenario_uncharted_island_tutorial();
    AddSC_scenario_snowblossom_village();
    AddSC_scenario_draenor_shadowmoon_valley();
    AddSC_scenario_frostfire_finale();
    AddSC_scenario_talador_iron_horde_finale();
    AddSC_scenario_grommashar();
    AddSC_scenario_gorgrond_finale_1();
    AddSC_scenario_verdant_wilds();
    AddSC_scenario_ungol_ruins();
    AddSC_brokenIslands();
    AddSC_instance_broken_islands();
    AddSC_instance_a_brewing_storm();
    AddSC_a_brewing_storm();
    AddSC_instance_troves_of_the_thunder_king();
    AddSC_troves_of_the_thunder_king();
    AddSC_DH_vengeance_art_scenario();
    AddSC_arena_of_annihiliation();
    AddSC_instance_instance_arena_of_annihiliation();
    AddSC_instance_secrets_of_ragefire();
    AddSC_scenario_the_secrets_of_ragefire();
    AddSC_secrets_of_ragefire();

    //Draenor
    AddSC_frostfire_ridge();
    AddSC_shadowmoon_draenor();
    AddSC_garrison_level_1();
    AddSC_garrison_level_2();
    AddSC_garrison_level_3();
    AddSC_spells_garrison();
    AddSC_instance_everbloom(); // Everbloom
    AddSC_the_everbloom();
    AddSC_boss_ancient_protectors();
    AddSC_boss_sol();
    AddSC_boss_witherbark();
    AddSC_boss_xeritac();
    AddSC_boss_yalnu();
    AddSC_auchindoun(); //Auchindom
    AddSC_boss_azaakel();
    AddSC_boss_kaathar();
    AddSC_boss_nyami();
    AddSC_boss_teronogor();
    AddSC_instance_auchindoun();
    AddSC_tanaan_intro_blackrock(); //Tanaan Intro
    AddSC_tanaan_intro_bleeding_hollow();
    AddSC_tanaan_intro_finale();
    AddSC_tanaan_intro_global();
    AddSC_tanaan_intro_portal();
    AddSC_tanaan_intro_shadowmoon();
    AddSC_tanaan_intro_shattered_hand();
    AddSC_tanaan_jungle();
    AddSC_bonemaw(); //Shadowmoon Burial Grounds
    AddSC_nerzul();
    AddSC_shadowmoon_burial_grounds();
    AddSC_sadana();
    AddSC_nhalish();
    AddSC_instance_shadowmoon_burial_grounds();
    AddSC_boss_grimrail(); //Iron Docks
    AddSC_boss_nokgar();
    AddSC_boss_oshir();
    AddSC_boss_skulloc();
    AddSC_instance_iron_docks();
    AddSC_iron_docks();
    AddSC_boss_nitrogg_thundertower();
    AddSC_RocketsparkandBorka();
    AddSC_boss_skylord_tovra();
    AddSC_grimrail_depot();
    AddSC_instance_grimrail_depot();
    AddSC_areatrigger_Bloodmaul(); //BloodMaul Slag Mines
    AddSC_boss_forgemaster_gogduh();
    AddSC_boss_gugrokk();
    AddSC_boss_roltall();
    AddSC_boss_SlaveWatcherCrushto();
    AddSC_instance_Bloodmaul();
    AddSC_mob_Bloodmaul();
    AddSC_spell_Bloodmaul();
    AddSC_gorgrond();
    AddSC_spires_of_arak();
    AddSC_talador();

    // Brawlers guild
    AddBrawlersGuildScripts();
}

