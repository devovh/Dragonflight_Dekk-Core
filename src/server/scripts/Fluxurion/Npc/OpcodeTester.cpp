/*
 * Copyright 2023 Fluxurion
 */

#include "Creature.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "Config.h"
#include "Chat.h"
#include "ScriptedGossip.h"
#include "ScriptedCreature.h"
#include "MiscPackets.h"
#include "NPCPackets.h"
#include "GarrisonPackets.h"


struct OpcodeTesterNpc : public ScriptedAI
{
    OpcodeTesterNpc(Creature* creature) : ScriptedAI(creature) { }

    bool OnGossipHello(Player* player) override
    {
        ClearGossipMenuFor(player);
        AddGossipItemFor(player, GossipOptionNpc::None, "OPENGARRISONUI", GOSSIP_SENDER_MAIN, 0);
        AddGossipItemFor(player, GossipOptionNpc::None, "OpenNpcAnimaUI 2", GOSSIP_SENDER_MAIN, 1);
        AddGossipItemFor(player, GossipOptionNpc::None, "GetlandingpageshipmentResponse", GOSSIP_SENDER_MAIN, 2);
        AddGossipItemFor(player, GossipOptionNpc::None, "CovenantRenowOpenNpc", GOSSIP_SENDER_MAIN, 3);
        AddGossipItemFor(player, GossipOptionNpc::None, "Give Reservoir Anima", GOSSIP_SENDER_MAIN, 4);
        AddGossipItemFor(player, GossipOptionNpc::None, "AdventureMapOpenNpc", GOSSIP_SENDER_MAIN, 5);
        AddGossipItemFor(player, GossipOptionNpc::None, "GarrisonOpenMissionNpc", GOSSIP_SENDER_MAIN, 6);
        AddGossipItemFor(player, GossipOptionNpc::None, "Test Add Transmog Set", GOSSIP_SENDER_MAIN, 7);
        AddGossipItemFor(player, GossipOptionNpc::None, "Test AddItemWithToast", GOSSIP_SENDER_MAIN, 8);
        AddGossipItemFor(player, GossipOptionNpc::None, "Test Trading Post", GOSSIP_SENDER_MAIN, 9);
        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, me->GetGUID());
        return true;
    }

    void HandleUIItemInteractionOpenNpc(Player* player)
    {
        /*
            ObjectGuid guid;

            WorldPackets::NPC::NPCInteractionOpenResult npcInteraction;
            npcInteraction.Npc = guid;
            npcInteraction.InteractionType = PlayerInteractionType::AdventureMap;
            npcInteraction.Success = true;
            player->GetSession()->SendPacket(npcInteraction.Write());
        */
    }

    void TestOp(Player* player)
    {
        /*
        WorldPackets::Garrison::DeleteExpiredMissionsResult response;
        response.GarrTypeID = 3;
        response.LegionUnkBit = true;
        player->GetSession()->SendPacket(response.Write());
        */
    }

    void CovenantRenownOpcode(Player* player)
    {
        /*
        WorldPackets::Garrison::DeleteExpiredMissionsResult response;
        response.GarrTypeID = 9;
        response.LegionUnkBit = false;
        player->GetSession()->SendPacket(response.Write());
        */
    }

    void TestAdventureMapOpenNpc(Player* player)
    {
        /*
        WorldPackets::Garrison::DeleteExpiredMissionsResult response;
        response.GarrTypeID = 111;
        response.LegionUnkBit = false;
        player->GetSession()->SendPacket(response.Write());
        */
    }

    void TestGarrisonOpenMissionNpc(Player* player)
    {
        /*
        WorldPackets::Garrison::GarrisonCompleteMissionResult send;
        send.Result = 0;
        send.MissionRecID = 0;
        player->GetSession()->SendPacket(send.Write());
        */
    }

    void testsss(Player* player)
    {

    }

    bool OnGossipSelect(Player* player, uint32 sender, uint32 action) override
    {
        WorldPackets::NPC::NPCInteractionOpenResult npcInteraction;
        npcInteraction.Npc = me->GetGUID();
        npcInteraction.InteractionType = PlayerInteractionType::PerksProgramVendor;
        npcInteraction.Success = true;

        switch (action)
        {
        case 0:
            HandleUIItemInteractionOpenNpc(player);
            CloseGossipMenuFor(player);
            break;
        case 1:
            TestOp(player);
            CloseGossipMenuFor(player);
            break;
        case 2:
            CovenantRenownOpcode(player);
            CloseGossipMenuFor(player);
            break;
        case 3:
            TestAdventureMapOpenNpc(player);
            CloseGossipMenuFor(player);
            break;
        case 4:
            TestGarrisonOpenMissionNpc(player);
            CloseGossipMenuFor(player);
            break;
        case 5:
            testsss(player);
            CloseGossipMenuFor(player);
            break;
        case 6:
           // TestStartNpc(player);
            CloseGossipMenuFor(player);
            break;
        case 7:
            player->GetSession()->GetCollectionMgr()->AddTransmogSet(2291);
            CloseGossipMenuFor(player);
            break;
        case 8:
            player->AddItemWithToast(152503, 1, 6771);
            CloseGossipMenuFor(player);
            break;
        case 9:
            player->SendDirectMessage(npcInteraction.Write());
            CloseGossipMenuFor(player);
            break;
        }
        return true;
    }

};


void AddSC_OpcodeTesterNpc()
{
    RegisterCreatureAI(OpcodeTesterNpc);
}
