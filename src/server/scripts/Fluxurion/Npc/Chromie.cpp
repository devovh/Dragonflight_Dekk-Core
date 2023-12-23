/*
 * Copyright 2023 Fluxurion
 */

#include "Flux.h"
#include "ScriptMgr.h"
#include "ScriptedGossip.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "WorldPacket.h"

enum Chromie
{
    GOSSIP_MENU_SELECT_CHROMIE_TIME = 25426,
};

struct npc_chromie_167032 : public ScriptedAI
{
    npc_chromie_167032(Creature* creature) : ScriptedAI(creature) { }

    bool OnGossipHello(Player* player) override
    {
        ClearGossipMenuFor(player);

        if (me->IsQuestGiver())
            player->PrepareQuestMenu(me->GetGUID());

        if (player->GetLevel() >= 10 && player->GetLevel() < 60)
        {
            if (Fluxurion::GetChromieTime(player) == CHROMIE_TIME_CURRENT)
                AddGossipItemFor(player, GossipOptionNpc::None, "Select a timeline.", GOSSIP_SENDER_MAIN, 0);

            if (Fluxurion::GetChromieTime(player) > CHROMIE_TIME_CURRENT)
            {
                AddGossipItemFor(player, GossipOptionNpc::None, "Select a different timeline.", GOSSIP_SENDER_MAIN, 1);
                AddGossipItemFor(player, GossipOptionNpc::None, "I'd like to return to the present timeline.", GOSSIP_SENDER_MAIN, 2);
            }
        }

        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, me->GetGUID());

        return true;
    }

    bool OnGossipSelect(Player* player, uint32 sender, uint32 action) override
    {
        WorldPackets::NPC::NPCInteractionOpenResult npcInteraction;
        npcInteraction.Npc = me->GetGUID();
        npcInteraction.InteractionType = PlayerInteractionType::ChromieTime;
        npcInteraction.Success = true;

        switch (action)
        {
        case 0:
            player->SendDirectMessage(npcInteraction.Write());
            break;
        case 1:
            Fluxurion::SetChromieTime(player, CHROMIE_TIME_CURRENT);
            player->CastSpell(player, 335482, true);
            CloseGossipMenuFor(player);
            break;
        }
        return true;
    }
};

class set_chromietime_onlogin : public PlayerScript
{
public:
    set_chromietime_onlogin() : PlayerScript("set_chromietime_onlogin") { }

    void OnLogin(Player* player, bool /*loginFirst*/)
    {
        if (Fluxurion::GetChromieTime(player))
		    Fluxurion::SetChromieTime(player, Fluxurion::GetChromieTime(player));
	}
};

void AddSC_Chromie()
{
    RegisterCreatureAI(npc_chromie_167032);
    new set_chromietime_onlogin();
}
