///*
// * Copyright 2021
// *
// * This program is free software; you can redistribute it and/or modify it
// * under the terms of the GNU General Public License as published by the
// * Free Software Foundation; either version 2 of the License, or (at your
// * option) any later version.
// *
// * This program is distributed in the hope that it will be useful, but WITHOUT
// * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
// * more details.
// *
// * You should have received a copy of the GNU General Public License along
// * with this program. If not, see <http://www.gnu.org/licenses/>.
// */
//
//#include "Creature.h"
//#include "Player.h"
//#include "ScriptMgr.h"
//#include "Config.h"
//#include "Chat.h"
//#include "GameObjectAI.h"
//#include "ScriptedGossip.h"
//#include "ScriptedCreature.h"
//#include "MiscPackets.h"
//#include "NPCPackets.h"
//#include "GarrisonPackets.h"
//
//enum quests
//{
//    QUEST_FURTHER_RESEARCH_DEALIC = 65432,
//};
//
////181397 
//class npc_Cypher_Console : public CreatureScript
//{
//public:
//    npc_Cypher_Console() : CreatureScript("npc_Cypher_Console") { }
//
//    CreatureAI* GetAI(Creature* creature) const override
//    {
//        return new npc_Cypher_ConsoleAI(creature);
//    }
//
//    struct npc_Cypher_ConsoleAI : public ScriptedAI
//    {
//        npc_Cypher_ConsoleAI(Creature* creature) : ScriptedAI(creature) { }
//
//        bool OnGossipHello(Player* player) override
//        {
//            if (me->IsQuestGiver())
//                player->PrepareQuestMenu(me->GetGUID());
//
//            if (player->HasQuest(QUEST_FURTHER_RESEARCH_DEALIC))
//            {
//                player->ForceCompleteQuest(QUEST_FURTHER_RESEARCH_DEALIC);
//            }
//
//            AddGossipItemFor(player, GossipOptionNpc::Binder, "Show me the Console", GOSSIP_SENDER_MAIN, 0);
//            SendGossipMenuFor(player, player->GetGossipTextId(me), me->GetGUID());
//
//            return true;
//        }
//
//        void TestOp(Player* player)
//        {
//            WorldPackets::Garrison::GarrisonOpenTalentNpc send;
//            send.NpcGUID = player->GetGUID();
//            send.GarrTalentTreeID = 474;
//            send.FriendshipFactionID = 0;
//            player->GetSession()->SendPacket(send.Write());
//        }
//
//        bool OnGossipSelect(Player* player, uint32 sender, uint32 action) override
//        {
//            switch (action)
//            {
//            case 0:
//                TestOp(player);
//                CloseGossipMenuFor(player);
//                break;
//            }
//            return true;
//        }
//    };
//};
//
//
//void AddSC_zone_zerethmorthis()
//{
//    new npc_Cypher_Console();
//}
