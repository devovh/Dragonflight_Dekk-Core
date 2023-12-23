/*
 * Copyright DekkCore team 2023
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
#ifndef DEF_SANGUINE_DEPTHS_H_
#define DEF_SANGUINE_DEPTHS_H_

#include "CreatureAIImpl.h"

#define DataHeader "SDS"
#define SDScriptName "instance_sanguine_depths"

uint32 const EncounterCount = 4;

enum SDSDataTypes
{
    // Encounters
    DATA_KRYXIS = 0,
    DATA_EXECUTOR = 1,
    DATA_BERYLLIA = 2,
    DATA_KAAL = 3
};

enum SDSCreatureIds
{
    // Bosses
    BOSS_KRYXIS   = 162100,
    BOSS_EXECUTOR = 162103,
    BOSS_BERYLLIA = 162102,
    BOSS_KAAL     = 165318,
};

enum SDSWorldstates
{
    WORLD_STATE_KRYXIS_ENCOUNTER_COMPLETE   = 18629,
    WORLD_STATE_EXECUTOR_ENCOUNTER_COMPLETE = 18628,
    WORLD_STATE_BERYLLIA_ENCOUNTER_COMPLETE = 18627,
};

template <class AI, class T>
inline AI* GetSanguineDepthsAI(T* obj)
{
    return GetInstanceAI<AI>(obj, SDScriptName);
}

#define RegisterSanguineDepthsCreatureAI(ai_name) RegisterCreatureAIWithFactory(ai_name, GetSanguineDepthsAI)

#endif
