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

#include "ScriptMgr.h"
#include "CombatAI.h"
#include "Player.h"
#include "ScriptedCreature.h"
#include "MotionMaster.h"
#include "ObjectAccessor.h"
#include "SpellInfo.h"
#include "SpellScript.h"
#include "TemporarySummon.h"

uint32 const pathSize2 = 2;
Position const BoatPath[pathSize2] =
{
    { -9356.31f, -2414.29f, 69.6370f },
    { -9425.49f, -2836.49f, 69.9875f },
};

struct npc_keeshan_riverboat : public VehicleAI
{
    npc_keeshan_riverboat(Creature* creature) : VehicleAI(creature) { }

    void Reset() override
    {
        me->GetMotionMaster()->MoveSmoothPath(pathSize2, BoatPath, pathSize2, false, true);
        me->DespawnOrUnsummon(Seconds(22), Seconds(60));
    }
};

enum BlackrockTower
{
    QUEST_TO_WIN_A_WAR = 26651,
    NPC_BLACKROCK_TOWER = 43590,
    NPC_MUNITIONS_DUMP = 43589,
};

struct npc_blackrock_tower : public ScriptedAI
{
    npc_blackrock_tower(Creature* creature) : ScriptedAI(creature) {}

    void MoveInLineOfSight(Unit* who) override
    {
        ScriptedAI::MoveInLineOfSight(who);

        if (who->IsPlayer() && who->ToPlayer()->GetQuestStatus(QUEST_TO_WIN_A_WAR) == QUEST_STATUS_INCOMPLETE)
            who->ToPlayer()->KilledMonsterCredit(NPC_BLACKROCK_TOWER);
    }
};

struct npc_munitions_dump : public ScriptedAI
{
    npc_munitions_dump(Creature* creature) : ScriptedAI(creature) {}

    void MoveInLineOfSight(Unit* who) override
    {
        ScriptedAI::MoveInLineOfSight(who);

        if (who->IsPlayer() && who->ToPlayer()->GetQuestStatus(QUEST_TO_WIN_A_WAR) == QUEST_STATUS_INCOMPLETE)
            who->ToPlayer()->KilledMonsterCredit(NPC_MUNITIONS_DUMP);
    }
};

void AddSC_DekkCore_redridge_mountains()
{
    RegisterCreatureAI(npc_keeshan_riverboat);
    RegisterCreatureAI(npc_blackrock_tower);
    RegisterCreatureAI(npc_munitions_dump);
}
