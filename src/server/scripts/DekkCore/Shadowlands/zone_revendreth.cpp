/*
 * Copyright 2021
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

#include "Creature.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "Config.h"
#include "Chat.h"
#include "GameObjectAI.h"
#include "ScriptedGossip.h"
#include "ScriptedCreature.h"
#include "MiscPackets.h"
#include "NPCPackets.h"
#include "GarrisonPackets.h"

 // 175772 Rahel <Keeper of Renown>
struct Rahel : public ScriptedAI
{
    Rahel(Creature* creature) : ScriptedAI(creature) { }

    bool OnGossipHello(Player* player) override
    {
        ClearGossipMenuFor(player);
        AddGossipItemFor(player, GossipOptionNpc::Binder, "Show me Covenant renown", GOSSIP_SENDER_MAIN, 0);
        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, me->GetGUID());
        return true;
    }
    bool OnGossipSelect(Player* player, uint32 sender, uint32 action) override
    {
        switch (action)
        {
        case 0:
            player->GetSession()->SendCovenantRenownSendCatchUpState(false);
            CloseGossipMenuFor(player);
            break;
        }
        return true;
    }

};


void AddSC_zone_revendreth()
{
    RegisterCreatureAI(Rahel);
}
