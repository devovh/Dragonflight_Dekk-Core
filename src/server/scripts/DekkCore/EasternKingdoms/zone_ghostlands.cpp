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
#include "GameObject.h"
#include "Player.h"
#include "ScriptedEscortAI.h"
#include "ScriptedGossip.h"
#include "TemporarySummon.h"
#include "WorldSession.h"

/*######

## npc_ranger_lilatha
######*/

enum RangerLilatha
{
    SAY_START                           = 0,
    SAY_PROGRESS1                       = 1,
    SAY_PROGRESS2                       = 2,
    SAY_PROGRESS3                       = 3,
    SAY_END1                            = 4,
    SAY_END2                            = 5,
    SAY_CAPTAIN_ANSWER                  = 0,
    QUEST_ESCAPE_FROM_THE_CATACOMBS     = 9212,
    GO_CAGE                             = 181152,
    NPC_CAPTAIN_HELIOS                  = 16220,
    NPC_MUMMIFIED_HEADHUNTER            = 16342,
    NPC_SHADOWPINE_ORACLE               = 16343
};

class npc_ranger_lilatha : public CreatureScript
{
public:
    npc_ranger_lilatha() : CreatureScript("npc_ranger_lilatha") { }

    struct npc_ranger_lilathaAI : public EscortAI
    {
        npc_ranger_lilathaAI(Creature* creature) : EscortAI(creature) { }

        void WaypointReached(uint32 waypointId, uint32 /*pathId*/) override
        {
            Player* player = GetPlayerForEscort();
            if (!player)
                return;

            switch (waypointId)
            {
                case 0:
                    me->SetStandState(UNIT_STAND_STATE_STAND);
                    if (GameObject* Cage = me->FindNearestGameObject(GO_CAGE, 20))
                        Cage->SetGoState(GO_STATE_ACTIVE);
                    Talk(SAY_START, player);
                    break;
                case 5:
                    Talk(SAY_PROGRESS1, player);
                    break;
                case 11:
                    Talk(SAY_PROGRESS2, player);
                    me->SetFacingTo(4.762841f);
                    break;
                case 18:
                    {
                        Talk(SAY_PROGRESS3, player);
                        TempSummon* Summ1 = me->SummonCreature(NPC_MUMMIFIED_HEADHUNTER, 7627.083984f, -7532.538086f, 152.128616f, 1.082733f, TEMPSUMMON_DEAD_DESPAWN, 0s);
                        TempSummon* Summ2 = me->SummonCreature(NPC_SHADOWPINE_ORACLE, 7620.432129f, -7532.550293f, 152.454865f, 0.827478f, TEMPSUMMON_DEAD_DESPAWN, 0s);
                        if (Summ1 && Summ2)
                        {
                            Summ1->Attack(me, true);
                            Summ2->Attack(player, true);
                        }
                        AttackStart(Summ1);
                    }
                    break;
                case 19:
                    me->SetWalk(false);
                    break;
                case 25:
                    me->SetWalk(true);
                    break;
                case 30:
                    player->GroupEventHappens(QUEST_ESCAPE_FROM_THE_CATACOMBS, me);
                    break;
                case 32:
                    me->SetFacingTo(2.978281f);
                    Talk(SAY_END1, player);
                    break;
                case 33:
                    me->SetFacingTo(5.858011f);
                    Talk(SAY_END2, player);
                    Creature* CaptainHelios = me->FindNearestCreature(NPC_CAPTAIN_HELIOS, 50);
                    if (CaptainHelios)
                        CaptainHelios->AI()->Talk(SAY_CAPTAIN_ANSWER, player);
                    break;
            }
        }

        void Reset() override
        {
            if (GameObject* Cage = me->FindNearestGameObject(GO_CAGE, 20))
                Cage->SetGoState(GO_STATE_READY);
        }

        void OnQuestAccept(Player* player, Quest const* quest) override
        {
            if (quest->GetQuestId() == QUEST_ESCAPE_FROM_THE_CATACOMBS)
            {
                me->SetFaction(FACTION_ESCORTEE_N_NEUTRAL_PASSIVE);
                Start(true, player->GetGUID());
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_ranger_lilathaAI(creature);
    }

};

/*######
## npc_rathis_tomber
######*/

class npc_rathis_tomber : public CreatureScript
{
public:
    npc_rathis_tomber() : CreatureScript("npc_rathis_tomber") {}

    struct npc_rathis_tomberAI : public ScriptedAI
    {
        npc_rathis_tomberAI(Creature* creature) : ScriptedAI(creature) { }

        bool OnGossipSelect(Player* player, uint32 sender, uint32 action) override
        {
            player->PlayerTalkClass->ClearMenus();
            if (action == GOSSIP_ACTION_TRADE)
                player->GetSession()->SendListInventory(me->GetGUID());
            return true;
        }

        bool OnGossipHello(Player* player) override
        {
            if (me->IsQuestGiver())
                player->PrepareQuestMenu(me->GetGUID());

            if (me->IsVendor() && player->GetQuestRewardStatus(9152))
            {
                AddGossipItemFor(player, GossipOptionNpc::Vendor, GOSSIP_TEXT_BROWSE_GOODS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);
                SendGossipMenuFor(player, player->GetGossipTextId(me), me->GetGUID());
            }
            else
                SendGossipMenuFor(player, player->GetGossipTextId(me), me->GetGUID());

            return true;
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_rathis_tomberAI(creature);
    }
};
 
// https://www.wowhead.com/npc=150257/candle todo script forhttps://www.wowhead.com/quest=53882/writing-on-the-wall#english-comments

void AddSC_DekkCore_ghostlands()
{
    new npc_ranger_lilatha();
    new npc_rathis_tomber();
}
