/*
 * This file is part of DEKKCORETEAM
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

#ifndef DEF_BRACKENHIDE_HOLLOW_H
#define DEF_BRACKENHIDE_HOLLOW_H

#include "CreatureAIImpl.h"

#define DataHeader "BRH"
#define BRHScriptName "instance_brackenhide_hollow"

uint32 const EncounterCount = 4;

enum BRHDataTypes
{
    // Encounters
    DATA_RIRA_HACKLAW         = 0,
    DATA_TREE_MOUTH           = 1,
    DATA_GUTSHOT              = 2,
    DATA_DECATRIARCH_WRATHEYE = 3
};

enum BRHCreatureIds
{
  //BOSSES
    BOSS_RIRA_HACKLAW = 186122
};

template <class AI, class T>
inline AI* GetBrackenhideHollowAI(T* obj)
{
    return GetInstanceAI<AI>(obj, BRHScriptName);
}

#define RegisterBrackenhideHollowCreatureAI(ai_name) RegisterCreatureAIWithFactory(ai_name, GetBrackenhideHollowAI)

#endif
