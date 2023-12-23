/*
 * Copyright 2021 DEKKCORE
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

#ifndef DEF_UNDERROT_H_
#define DEF_UNDERROT_H_

#include "CreatureAIImpl.h"

#define DataHeader "Underrot"
#define UnderrotScriptName "instance_underrot"

uint32 const EncounterCount = 4;

enum EncounterData
{
    // Encounters
    DATA_ELDER_LEAXA = 0,
    DATA_CRAGMAW_THE_INFESTED,
    DATA_SPORECALLER_ZANCHA,
    DATA_UNBOUND_ABOMINATION,
    DATA_CRAGMAW_CRAWG_EATING,

    DATA_FACELESS_CORRUPTOR_1,
    DATA_FACELESS_CORRUPTOR_2,
    DATA_EVENT_HERZEL,
    DATA_BOSS_HERZEL,
};

enum UnderrotCreatureIds
{
    NPC_BLOODSWORN_DEFILER          = 144306,
    NPC_VOLATILE_POD                = 139127,
    NPC_TITAN_KEEPER_HEZREL         = 134419,
    NPC_BLOOD_VISAGE                = 137103,
    NPC_ROTTING_SPORE               = 137458,
    SUMMON_GROUP_BLOODSWORN_DEFILER = 1,
    // Bosses
    BOSS_ELDER_LEAXA = 131318,
    BOSS_SPORECALLER_ZANCHA = 131383,
    BOSS_CRAGMAW_THE_INFESTED = 131817,
    BOSS_UNBOUND_ABOMINATION = 133007
};

enum UnderrotGameObjectIds
{
    GOB_PYRAMID_WEB             = 296384,
    GO_PYRAMID_DOOR_UNBOUND_ABOMINATION_ENTRANCE = 296385,
    GO_WALL_DOOR_SHORTCUT_ENTRANCE = 295356
};

template <class AI, class T>
inline AI* GetUnderrotAI(T* obj)
{
    return GetInstanceAI<AI>(obj, UnderrotScriptName);
}

#define RegisterUnderrotCreatureAI(ai_name) RegisterCreatureAIWithFactory(ai_name, GetUnderrotAI)

#endif

