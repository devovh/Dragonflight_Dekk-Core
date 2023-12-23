/*
 * Copyright 2023 Fluxurion
 */

#include "Creature.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "Config.h"
#include "Chat.h"
#include "ScriptedGossip.h"
#include "GameTime.h"
#include "PhasingHandler.h"
#include "ScriptedCreature.h"
#include "Object.h"
#include "ChallengeModeMgr.h"
#include "Item.h"
#include "WorldStateMgr.h"

struct KeyStoneGenerator_npc : public ScriptedAI
{
    KeyStoneGenerator_npc(Creature* creature) : ScriptedAI(creature) { }

    bool OnGossipHello(Player* player) override
    {
        ClearGossipMenuFor(player);
        AddGossipItemFor(player, GossipOptionNpc::Binder, "Generate a Mythic Keystone", GOSSIP_SENDER_MAIN, 0);
        AddGossipItemFor(player, GossipOptionNpc::Binder, "Remove my Mythic Kestone", GOSSIP_SENDER_MAIN, 2);
        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, me->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, uint32 sender, uint32 action) override
    {
        int challengeLevel = urand(1, 20);

        switch (action)
        {
        case 0:
            player->AddChallengeKey(sChallengeModeMgr->GetRandomChallengeId(), challengeLevel);

            if (Item* keystone = player->GetItemByEntry(180653))
            {
                keystone->SetModifier(ITEM_MODIFIER_CHALLENGE_MAP_CHALLENGE_MODE_ID, Trinity::Containers::SelectRandomContainerElement(sDB2Manager.GetChallngeMaps()));
                keystone->SetModifier(ITEM_MODIFIER_CHALLENGE_KEYSTONE_LEVEL, challengeLevel);
                if (challengeLevel > 3)
                    keystone->SetModifier(ITEM_MODIFIER_CHALLENGE_KEYSTONE_AFFIX_ID_1, sWorldStateMgr->GetValue(WS_CHALLENGE_AFFIXE1_RESET_TIME, nullptr));
                if (challengeLevel > 6)
                    keystone->SetModifier(ITEM_MODIFIER_CHALLENGE_KEYSTONE_AFFIX_ID_2, sWorldStateMgr->GetValue(WS_CHALLENGE_AFFIXE2_RESET_TIME, nullptr));
                if (challengeLevel > 9)
                    keystone->SetModifier(ITEM_MODIFIER_CHALLENGE_KEYSTONE_AFFIX_ID_3, sWorldStateMgr->GetValue(WS_CHALLENGE_AFFIXE3_RESET_TIME, nullptr));
                if (challengeLevel > 12)
                    keystone->SetModifier(ITEM_MODIFIER_CHALLENGE_KEYSTONE_AFFIX_ID_4, sWorldStateMgr->GetValue(WS_CHALLENGE_AFFIXE4_RESET_TIME, nullptr));

                keystone->SetState(ITEM_CHANGED, player);
            }

            me->SendPlaySpellVisual(player, 100337, SPELL_MISS_NONE, 0, 10, false);
            player->PlayDirectSound(18489, player);

            CloseGossipMenuFor(player);
        break;
        case 1:
            if (player->HasItemCount(180653, 1, true))
                player->DestroyItemCount(180653, 1, true, false);

            player->SendPlaySpellVisual(player, 97417, SPELL_MISS_NONE, 0, 10, false);
            player->PlayDirectSound(107487, player);

            CloseGossipMenuFor(player);
        break;
        }
        return true;
    }

};

void AddSC_KeyStoneGenerator()
{
    RegisterCreatureAI(KeyStoneGenerator_npc);
}
