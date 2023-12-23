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

#ifndef DEF_DEADMINES_H
#define DEF_DEADMINES_H

#include "CreatureAIImpl.h"

#define DMScriptName "instance_deadmines"
#define DataHeader "DM"

enum Bosses
{
    BOSS_ADMIRAL_RIPSNARL = 0,
    BOSS_CAPTAIN_COOKIE,
    BOSS_FOO_REAPER_5000,
    BOSS_GLUBTOK,
    BOSS_HELIX_GEARBREAKER,
    BOSS_VANESSA_VANCLEEF,
    MAX_BOSS_ENCOUNTER
};

enum DMNPCs
{
    NPC_GLUBTOK = 47162,
    NPC_HELIX_GEARBREAKER = 47296,
    NPC_LUMBERING_OAF = 47297,
    NPC_MINE_RAT = 51462,
    NPC_FOE_REAPER_5000 = 43778,
    NPC_ADMIRAL_RIPSNARL = 47626,
    NPC_CAPTAIN_COOKIE = 47739,
    NPC_VANESSA_VANCLEEF = 49541,
    NPC_VANESSA_SITTING = 49429,
    NPC_NOTE_FROM_VANESSA = 49564,
    // horde npc
    NPC_KAGTHA = 46889,
    NPC_SLINKY_SHARPSHIV = 46906,
    NPC_MISS_MAYHEM = 46902,
    NPC_MAYHEM_REAPER = 46903,
    NPC_HAND_ASSASIN = 46890,
    // alliance npc
    NPC_HORATIO_LAINE = 46612,
    NPC_INVESTIGATOR = 46614,
    NPC_DEFENDER = 50595,
    NPC_CRIME_SCENE_BOT = 46613
};

enum DMCannonState
{
    CANNON_NOT_USED,
    CANNON_GUNPOWDER_USED,
    CANNON_BLAST_INITIATED,
    PIRATES_ATTACK,
    EVENT_DONE
};

enum DMData
{
    EVENT_STATE,
    EVENT_RHAHKZOR
};

enum DMData64
{
    DATA_SMITE_CHEST,
    DATA_GLUBTOK          = 0,
    DATA_HELIX            = 1,
    DATA_FOEREAPER        = 2,
    DATA_ADMIRAL          = 3,
    DATA_CAPTAIN          = 4,
    DATA_VANESSA          = 5,
    DATA_CANNON_EVENT     = 6,
    DATA_OAF              = 7,
    DATA_VANESSA_EVENT    = 8,
    DATA_TEAM_IN_INSTANCE = 9,
};

enum DMGameObjects
{
    GO_FACTORY_DOOR                                        = 13965,
    GO_IRONCLAD_DOOR                                       = 16397,
    GO_DEFIAS_CANNON                                       = 16398,
    GO_DOOR_LEVER                                          = 101833,
    GO_MR_SMITE_CHEST                                      = 144111
};

const Position centershipPos = { -63.167f, -819.315f, 41.27f, 6.25f };
const Position notePos = { -74.3611f, -820.014f, 40.3714f, 4.01426f };
const Position vanessaPos = { -75.5851f, -819.964f, 47.0673f, 6.17846f };

template <class AI, class T>
inline AI* GetDeadminesAI(T* obj)
{
    return GetInstanceAI<AI>(obj, DMScriptName);
}

#endif
