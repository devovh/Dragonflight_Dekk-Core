/*
 * Copyright 2021 AshamaneCore
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

#ifndef DEF_MOTS_H
#define DEF_MOTS_H

#define DataHeader    "MOTS"

uint32 const EncounterCount = 3;

enum Data
{
    DATA_INGRA_MALOCH           = 1,
    DATA_MISTCALLER             = 2,
    DATA_TREDOVA                = 3,

    DATA_MAZE_NEXT_DOOR         = 4,
    DATA_MAZE_DOOR_REACHED      = 5,
    DATA_MISTCALLER_CLUE_KILLED = 6,
};

enum Creatures
{
    NPC_INGRA_MALOCH                    = 164567,
    NPC_DROMAN_OULFARRAN                = 164804,
    NPC_MISTCALLER                      = 164501,

    NPC_CLUE_STALKER                    = 166188,

    NPC_MISTCALLER_FRIENDLY_END         = 170217,
    NPC_DROMAN_AT_MISTCALLER_END        = 170218,
    NPC_DROMAN_AT_TREDOVA_END           = 170229,

    NPC_COCOON_OF_LAKALI                = 165512,
    NPC_GORMLING_LARVA                  = 165560,

    NPC_DRUST_SOULCLEAVER               = 172991,
    NPC_MISTVEIL_DEFENDER_MAZE_ENTRANCE = 171772,
    NPC_MISTVEIL_DEFENDER               = 163058,
};

enum GameObjects
{
    GO_INGRA_MALOCK_EXIT_DOOR   = 355383,

    GO_MISTCALLER_EXIT_DOOR_1   = 352982,
    GO_MISTCALLER_EXIT_DOOR_2   = 353622,
    GO_MISTCALLER_EXIT_DOOR_3   = 353636,
    GO_MISTCALLER_EXIT_DOOR_4   = 353637,
    GO_MISTCALLER_EXIT_DOOR_5   = 353638,

    GO_TREDOVA_EXIT_DOOR        = 353514,
};

enum ClueStalkerBehaviour
{
    CLUE_STALKER_NO_ACTION      = 0,
    CLUE_STALKER_WRONG_CHOICE   = 1,
    CLUE_STALKER_SELECTED_CLUE  = 2,
};

enum Spells
{
    SPELL_MIST_NPC_APPEAR_MISSILE       = 335182,

    SPELL_TELEPORT_TO_MAZE_ENTRANCE     = 323881,

    SPELL_CONVERSATION_INITIAL_MAZE     = 331443,

    SPELL_CONVERSATION_MAZE_SUCCESS_1   = 331473,
    SPELL_CONVERSATION_MAZE_SUCCESS_2   = 331474,
    SPELL_CONVERSATION_MAZE_SUCCESS_3   = 331475,

    SPELL_CONVERSATION_MAZE_TAUNT_1     = 331469,
    SPELL_CONVERSATION_MAZE_TAUNT_2     = 331470,
    SPELL_CONVERSATION_MAZE_TAUNT_3     = 331471,
};

enum MotsMazeSpellVisualFlags
{
    MAZE_VISUAL_FLAG_LEAF       = 0x01,
    MAZE_VISUAL_FLAG_FLOWER     = 0x02,
    MAZE_VISUAL_FLAG_FILLED     = 0x04,
    MAZE_VISUAL_FLAG_EMPTY      = 0x08,
    MAZE_VISUAL_FLAG_CIRCLE     = 0x10,
    MAZE_VISUAL_FLAG_NO_CIRCLE  = 0x20,
};

enum MotsSpellVisualKit
{
    // Maze
    SPELL_MAZE_VISUAL_KIT_EMPTY_FLOWER          = 131477,
    SPELL_MAZE_VISUAL_KIT_EMPTY_FLOWER_CIRCLE   = 131478,
    SPELL_MAZE_VISUAL_KIT_FILLED_FLOWER         = 131479,
    SPELL_MAZE_VISUAL_KIT_FILLED_FLOWER_CIRCLE  = 131480,
    SPELL_MAZE_VISUAL_KIT_EMPTY_LEAF            = 131481,
    SPELL_MAZE_VISUAL_KIT_EMPTY_LEAF_CIRCLE     = 131482,
    SPELL_MAZE_VISUAL_KIT_FILLED_LEAF           = 131483,
    SPELL_MAZE_VISUAL_KIT_FILLED_LEAF_CIRCLE    = 131484,

    // Boss
    SPELL_BOSS_VISUAL_KIT_EMPTY_FLOWER          = 125911,
    SPELL_BOSS_VISUAL_KIT_EMPTY_FLOWER_CIRCLE   = 125920,
    SPELL_BOSS_VISUAL_KIT_FILLED_FLOWER         = 125922,
    SPELL_BOSS_VISUAL_KIT_FILLED_FLOWER_CIRCLE  = 125923,
    SPELL_BOSS_VISUAL_KIT_EMPTY_LEAF            = 125924,
    SPELL_BOSS_VISUAL_KIT_EMPTY_LEAF_CIRCLE     = 125925,
    SPELL_BOSS_VISUAL_KIT_FILLED_LEAF           = 125926,
    SPELL_BOSS_VISUAL_KIT_FILLED_LEAF_CIRCLE    = 125927,
};

const std::map<uint8, uint8> MazeSymbols =
{
    { 1, MAZE_VISUAL_FLAG_FLOWER | MAZE_VISUAL_FLAG_EMPTY  | MAZE_VISUAL_FLAG_NO_CIRCLE },
    { 2, MAZE_VISUAL_FLAG_FLOWER | MAZE_VISUAL_FLAG_EMPTY  | MAZE_VISUAL_FLAG_CIRCLE    },
    { 3, MAZE_VISUAL_FLAG_FLOWER | MAZE_VISUAL_FLAG_FILLED | MAZE_VISUAL_FLAG_NO_CIRCLE },
    { 4, MAZE_VISUAL_FLAG_FLOWER | MAZE_VISUAL_FLAG_FILLED | MAZE_VISUAL_FLAG_CIRCLE    },
    { 5, MAZE_VISUAL_FLAG_LEAF   | MAZE_VISUAL_FLAG_EMPTY  | MAZE_VISUAL_FLAG_NO_CIRCLE },
    { 6, MAZE_VISUAL_FLAG_LEAF   | MAZE_VISUAL_FLAG_EMPTY  | MAZE_VISUAL_FLAG_CIRCLE    },
    { 7, MAZE_VISUAL_FLAG_LEAF   | MAZE_VISUAL_FLAG_FILLED | MAZE_VISUAL_FLAG_NO_CIRCLE },
    { 8, MAZE_VISUAL_FLAG_LEAF   | MAZE_VISUAL_FLAG_FILLED | MAZE_VISUAL_FLAG_CIRCLE    },
};

std::vector<uint8> GenerateSymbols();

#endif
