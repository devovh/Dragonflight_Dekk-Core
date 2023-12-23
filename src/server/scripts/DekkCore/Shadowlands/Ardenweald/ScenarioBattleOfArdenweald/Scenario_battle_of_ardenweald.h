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

#ifndef DEF_SBOAW_H
#define DEF_SBOAW_H

#define DataHeader    "SBOAW"


uint8 rally_point_num = 0;
uint8 save_seed_num = 0;
uint8  death_point = 0;

uint32 const EncounterCount = 3;

enum Data
{
    DATA_INGRA_MALOCH = 1,
    DATA_MISTCALLER = 2,
    DATA_TREDOVA = 3,

    DATA_MAZE_NEXT_DOOR = 4,
    DATA_MAZE_DOOR_REACHED = 5,
    DATA_MISTCALLER_CLUE_KILLED = 6,

    DATA_FRAXIN = 7,
    DATA_REALMBREAKER = 8,
    DATA_REALMBREAKER_DEAD = 9,
    DATA_INVOKER_DEAD = 10,

};

enum Creatures
{
    NPC_KLEIA = 177123,
    NPC_KLEIA2 = 177574,
    NPC_LOAR_HERNE = 177272,
    NPC_MISTBLADE = 177205,
    NPC_KORAYN = 177275,
    NPC_NIYA = 177273,
    NPC_MOONBERRY2 = 177464,
    NPC_DREAMWEAVER = 177274,
    NPC_DRAVEN2 = 177854,

    NPC_MAWSWORN_PILLAGER = 177164,
    NPC_MAWSWORN_ERADICATOR = 178539,
    NPC_WILD_HUNTER_GUARDIAN = 178717,
    NPC_WILD_HUNTER_ENCHANTER = 178659,
    NPC_SINFALL_DEFENDER = 178611,
    NPC_MALDRAXXI_GLADIATOR = 178610,
    NPC_KYRIAN_ASCENDANT = 178591,
    NPC_MALDRAXXI_LINEGUARD = 178605,

    NPC_REALMBREAKER = 177600,
    NPC_INVOKER = 177601,
    NPC_DESTROYER = 177165,
};

enum ScenarioStep
{
    SCENARIO_BOA_INDEX_0 = 4903,  //Bumpy Arrival
    SCENARIO_BOA_INDEX_1 = 4810,  //Deploy the Decoys
    SCENARIO_BOA_INDEX_2 = 4811,  //Maw Invasion
    SCENARIO_BOA_INDEX_3 = 4812,  //Maw Invasion
    SCENARIO_BOA_INDEX_4 = 4901,  //Air Reinforcements
    SCENARIO_BOA_INDEX_5 = 4813,  //A Wing and a Slime
    SCENARIO_BOA_INDEX_6 = 4814,  //To the Top of the Falls
    SCENARIO_BOA_INDEX_7 = 4815,  //Rebirth, in Peril
    SCENARIO_BOA_INDEX_8 = 4817,  //The Forest Answers
    SCENARIO_BOA_INDEX_9 = 4902,  //The Forest Answers
};

enum Spells
{
    SPELL_TELEPORT_VISUAL_ARDENWEALD = 351744,
    SPELL_ARCANE_SHIELD = 352555,
    SPELL_TRANSFORMING_WINTER_QUEEN = 350175,
    SPELL_EMPOWER_TRANSFORMING = 350463,
    SPELL_INJURED_AURA = 350661,
    SPELL_SYLVANAS_PINNED_PLAYER = 350569,
    SPELL_SYLVANAS_BANSHEE_LEAP = 350581,
    SPELL_KLEIA_SHIELD = 350582,
    SPELL_BANSHEE_FORM = 350564,
    SPELL_BANSHEE_KILL = 350552,
    SPELL_ARCANE_BARRIER = 352976,
    SPELL_KYRIAN_AOE_HEAL = 352521,
    SPELL_RESCUING = 350281,
    SPELL_FINISH_THE_BATTLE_OF_ARDENWEALD = 349960,

    SPELL_SCENE_DISPELLING_DECOYS = 353985,   //packageID 3273, sceneID 2770
    SPELL_MOVIE_TYRANDE_VS_SYLBANAS = 359204,   //MovieId 951

};

enum Quests
{
    QUEST_THE_BATTLE_OF_ARDENWEALD = 63578,
};

enum objectGuid
{
    PLAYER_GUID = 9999,
};

enum ActionAndEvent
{
    ACTION_ANIMACONE_START_MOVE = 1000,
    ACTION_ATTACK_SOULBURNER,

    EVENT_DEPLOY_THE_DECOYS_PHASE1 = 2000,
    EVENT_DEPLOY_THE_DECOYS_PHASE2,
    EVENT_DEPLOY_THE_DECOYS_PHASE3,
    EVENT_DEPLOY_THE_DECOYS_PHASE4,
    EVENT_DEPLOY_THE_DECOYS_PHASE5,
    EVENT_DEPLOY_THE_DECOYS_PHASE6,

    EVENT_BANSHEE_QUEEN_PHASE1 = 3000,

    EVENT_TO_THE_TOP_OF_THE_FALLS_PHASE1 = 4000,
    EVENT_TO_THE_TOP_OF_THE_FALLS_PHASE2,
    EVENT_TO_THE_TOP_OF_THE_FALLS_PHASE3,
    EVENT_TO_THE_TOP_OF_THE_FALLS_PHASE4,

    EVENT_THE_FOREST_ANSWERS_SOUTH = 5000,
    EVENT_THE_FOREST_ANSWERS_NORTH,
};

#endif
