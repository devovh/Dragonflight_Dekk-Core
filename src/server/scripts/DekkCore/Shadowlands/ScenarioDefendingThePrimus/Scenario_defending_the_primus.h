/*
 * Copyright 2023 DekkCore Team Devs
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

#ifndef DEF_SDTP_H
#define DEF_SDTP_H

#define DataHeader    "SDTP"

uint32 const EncounterCount = 1;

enum Data
{
    DATA_PRIMUS,
    DATA_HEIRMIR,
    TYPE_MAWSWORN,
    DATA_MAWSWORN_KILLED,

    ENCOUNTER_MAWSWORN_NUMBER    = 5,

    GUID_PLAYER                  = 9999,
};

enum Creatures
{
    NPC_PRIMUS       = 177570,
    NPC_HEIRMIR      = 178774,
    NPC_ANIMA_CELL   = 178453,
    NPC_THE_JAILER   = 175662,
    NPC_ANDUIN       = 178372,
    NPC_MAW_PORTAL   = 177417,
};

enum MobData
{
    NPC_MAWSWORN_RIPPER         = 179191,
    NPC_MAWSWORN_RAVAGER        = 179249,
    NPC_MAWSWORN_DESTRUCTOR     = 178445,
};

MobData mawswornData[3] =
{
    NPC_MAWSWORN_RIPPER,
    NPC_MAWSWORN_RAVAGER,
    NPC_MAWSWORN_DESTRUCTOR,
};

enum ScenarioStep
{
    SCENARIO_SPEAK_TO_THE_PRIMUS_STAGE_1                    = 4845,
    SCENARIO_DEFEAT_INCOMING_MAWSWORN_FORCES_STAGE_2        = 4846,
};

enum Spells
{
    SPELL_PRIMUS_CASTING                                    = 352856,
    SPELL_BONDS_OF_BROTHERHOOD                              = 352810,
    SPELL_CURIOUS_MIASMA                                    = 308201,
    SPELL_GORM_CHAINS                                       = 352630,
    SPELL_NECROTIC_RITUAL                                   = 305513,
    SPELL_CHAINED                                           = 358155,
    SPELL_ROOT_SELF                                         = 355868,
    SPELL_MASS_RECLAIM                                      = 352112,

    SCENE_EXIT_PRIMUS_SCENARIO                              = 352224, //packageID 3313, sceneID 2747
};

enum Quests
{
    QUEST_UNTANGLING_THE_SIGIL            = 63726,
};

enum ActionAndEvent
{
    EVENT_SPEAK_WITH_THE_PRIMUS           = 100,

    ACTION_MAWSWORN_KILLED               = 200,
};

Position const anima_cell_pos[8] =
{
    {2691.33f, 2099.46f, 318.11f,  0.00431f},
    {2704.12f, 2085.98f, 317.95f,  1.50291f},
    {2712.94f, 2090.97f, 317.48f,  2.33544f},
    {2696.31f, 2108.31f, 317.48f,  5.32624f},
    {2695.91f, 2090.21f, 317.48f,  0.61934f},
    {2713.08f, 2107.87f, 317.48f,  3.98085f},
    {2704.49f, 2112.78f, 317.78f,  4.44031f},
    {2717.05f, 2099.22f, 317.48f,  3.16953f},
};

#endif
