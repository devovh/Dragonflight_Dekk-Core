/*
 * Copyright (C) Dekk Core
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
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "Player.h"
#include "WorldSession.h"

/*######
## npc_the_scourge_cauldron
######*/

class npc_the_scourge_cauldron : public CreatureScript
{
public:
    npc_the_scourge_cauldron() : CreatureScript("npc_the_scourge_cauldron") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_the_scourge_cauldronAI(creature);
    }

    struct npc_the_scourge_cauldronAI : public ScriptedAI
    {
        npc_the_scourge_cauldronAI(Creature* creature) : ScriptedAI(creature) { }

        void Reset() override { }

        void JustEngagedWith(Unit* who) override { }

        void DoDie()
        {
            //summoner dies here
            me->KillSelf();
            //override any database `spawntimesecs` to prevent duplicated summons
            uint32 rTime = me->GetRespawnDelay();
            if (rTime < 600)
                me->SetRespawnDelay(600);
        }

        void MoveInLineOfSight(Unit* who) override
        {
            if (!who)
                return;

            Player* player = who->ToPlayer();
            if (!player)
                return;

            switch (me->GetAreaId())
            {
                case 199:                                   //felstone
                    if (player->GetQuestStatus(5216) == QUEST_STATUS_INCOMPLETE ||
                        player->GetQuestStatus(5229) == QUEST_STATUS_INCOMPLETE)
                    {
                        me->SummonCreature(11075, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 10min);
                        DoDie();
                    }
                    break;
                case 200:                                   //dalson
                    if (player->GetQuestStatus(5219) == QUEST_STATUS_INCOMPLETE ||
                        player->GetQuestStatus(5231) == QUEST_STATUS_INCOMPLETE)
                    {
                        me->SummonCreature(11077, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 10min);
                        DoDie();
                    }
                    break;
                case 201:                                   //gahrron
                    if (player->GetQuestStatus(5225) == QUEST_STATUS_INCOMPLETE ||
                        player->GetQuestStatus(5235) == QUEST_STATUS_INCOMPLETE)
                    {
                        me->SummonCreature(11078, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 10min);
                        DoDie();
                    }
                    break;
                case 202:                                   //writhing
                    if (player->GetQuestStatus(5222) == QUEST_STATUS_INCOMPLETE ||
                        player->GetQuestStatus(5233) == QUEST_STATUS_INCOMPLETE)
                    {
                        me->SummonCreature(11076, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 10min);
                        DoDie();
                    }
                    break;
            }
        }
    };
};

/*######
##    npcs_andorhal_tower
######*/

enum AndorhalTower
{
    GO_BEACON_TORCH                             = 176093
};

class npc_andorhal_tower : public CreatureScript
{
public:
    npc_andorhal_tower() : CreatureScript("npc_andorhal_tower") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_andorhal_towerAI(creature);
    }

    struct npc_andorhal_towerAI : public ScriptedAI
    {
        npc_andorhal_towerAI(Creature* creature) : ScriptedAI(creature)
        {
            SetCombatMovement(false);
        }

        void MoveInLineOfSight(Unit* who) override
        {
            if (!who || who->GetTypeId() != TYPEID_PLAYER)
                return;

            if (me->FindNearestGameObject(GO_BEACON_TORCH, 10.0f))
                if (Player* player = who->ToPlayer())
                    player->KilledMonsterCredit(me->GetEntry(), me->GetGUID());
        }
    };
};

enum DithersAndArbington
{
    GOSSIP_ITEM_ID_FELSTONE_FIELD = 0,
    GOSSIP_ITEM_ID_DALSON_S_TEARS = 1,
    GOSSIP_ITEM_ID_WRITHING_HAUNT = 2,
    GOSSIP_ITEM_ID_GAHRRON_S_WITH = 3,
    GOSSIP_MENU_ID_LETS_GET_TO_WORK = 3223,
    GOSSIP_MENU_ID_VITREOUS_FOCUSER = 3229,
    NPC_TEXT_OSSEOUS_AGITATORS = 3980,
    NPC_TEXT_SOMATIC_INTENSIFIERS_1 = 3981,
    NPC_TEXT_SOMATIC_INTENSIFIERS_2 = 3982,
    NPC_TEXT_ECTOPLASMIC_RESONATORS = 3983,
    NPC_TEXT_LET_S_GET_TO_WORK = 3985,
    QUEST_MISSION_ACCOMPLISHED_H = 5237,
    QUEST_MISSION_ACCOMPLISHED_A = 5238,
    CREATE_ITEM_VITREOUS_FOCUSER = 17529
};

class npcs_dithers_and_arbington : public CreatureScript
{
public:
    npcs_dithers_and_arbington() : CreatureScript("npcs_dithers_and_arbington") { }

    struct npcs_dithers_and_arbingtonAI : public ScriptedAI
    {
        npcs_dithers_and_arbingtonAI(Creature* creature) : ScriptedAI(creature) { }

        bool OnGossipSelect(Player* player, uint32 sender, uint32 action) override
        {
            ClearGossipMenuFor(player);
            switch (action)
            {
            case GOSSIP_ACTION_TRADE:
                player->GetSession()->SendListInventory(me->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 1:
                AddGossipItemFor(player, GOSSIP_MENU_ID_VITREOUS_FOCUSER, 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
                SendGossipMenuFor(player, NPC_TEXT_OSSEOUS_AGITATORS, me->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 2:
                AddGossipItemFor(player, GOSSIP_MENU_ID_VITREOUS_FOCUSER, 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
                SendGossipMenuFor(player, NPC_TEXT_SOMATIC_INTENSIFIERS_1, me->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 3:
                AddGossipItemFor(player, GOSSIP_MENU_ID_VITREOUS_FOCUSER, 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
                SendGossipMenuFor(player, NPC_TEXT_SOMATIC_INTENSIFIERS_2, me->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 4:
                AddGossipItemFor(player, GOSSIP_MENU_ID_VITREOUS_FOCUSER, 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
                SendGossipMenuFor(player, NPC_TEXT_ECTOPLASMIC_RESONATORS, me->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 5:
                CloseGossipMenuFor(player);
                me->CastSpell(player, CREATE_ITEM_VITREOUS_FOCUSER, false);
                break;
            }
            return true;
        }

        bool OnGossipHello(Player* player) override
        {
            if (me->IsQuestGiver())
                player->PrepareQuestMenu(me->GetGUID());

            if (me->IsVendor())
                AddGossipItemFor(player, GossipOptionNpc::Vendor, GOSSIP_TEXT_BROWSE_GOODS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);

            if (player->GetQuestRewardStatus(QUEST_MISSION_ACCOMPLISHED_H) || player->GetQuestRewardStatus(QUEST_MISSION_ACCOMPLISHED_A))
            {
                AddGossipItemFor(player, GOSSIP_MENU_ID_LETS_GET_TO_WORK, GOSSIP_ITEM_ID_FELSTONE_FIELD, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                AddGossipItemFor(player, GOSSIP_MENU_ID_LETS_GET_TO_WORK, GOSSIP_ITEM_ID_DALSON_S_TEARS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                AddGossipItemFor(player, GOSSIP_MENU_ID_LETS_GET_TO_WORK, GOSSIP_ITEM_ID_WRITHING_HAUNT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
                AddGossipItemFor(player, GOSSIP_MENU_ID_LETS_GET_TO_WORK, GOSSIP_ITEM_ID_GAHRRON_S_WITH, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
                SendGossipMenuFor(player, NPC_TEXT_LET_S_GET_TO_WORK, me->GetGUID());
            }
            else
                SendGossipMenuFor(player, player->GetGossipTextId(me), me->GetGUID());

            return true;
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npcs_dithers_and_arbingtonAI(creature);
    }

};

enum eMyranda
{
    QUEST_SUBTERFUGE = 5862,
    QUEST_IN_DREAMS = 5944,
    SPELL_SCARLET_ILLUSION = 17961
};

#define GOSSIP_ITEM_ILLUSION    "I am ready for the illusion, Myranda."

class npc_myranda_the_hag : public CreatureScript
{
public:
    npc_myranda_the_hag() : CreatureScript("npc_myranda_the_hag") { }

    struct npc_myranda_the_hagAI : public ScriptedAI
    {
        npc_myranda_the_hagAI(Creature* creature) : ScriptedAI(creature) { }
    };
    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_myranda_the_hagAI(creature);
    }


    bool OnGossipSelect(Player* player, Creature* /*creature*/, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        if (action == GOSSIP_ACTION_INFO_DEF + 1)
        {
            CloseGossipMenuFor(player);
            player->CastSpell(player, SPELL_SCARLET_ILLUSION, false);
        }
        return true;
    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestStatus(QUEST_SUBTERFUGE) == QUEST_STATUS_COMPLETE &&
            !player->GetQuestRewardStatus(QUEST_IN_DREAMS) && !player->HasAura(SPELL_SCARLET_ILLUSION))
        {
            AddGossipItemFor(player, GossipOptionNpc::None, GOSSIP_ITEM_ILLUSION, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            SendGossipMenuFor(player, player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }
        else
            SendGossipMenuFor(player, player->GetGossipTextId(creature), creature->GetGUID());

        return true;
    }
};

// Zone 138
class zone_western_plaguelands : public ZoneScript
{
public:
    zone_western_plaguelands() : ZoneScript("zone_western_plaguelands") { }

    void OnPlayerAreaUpdate(Player* player, uint32 newAreaId, uint32 /*oldAreaId*/) override
    {
        // Check paladin class area
        if (newAreaId == 7638 && !player->IsGameMaster() && (player->GetClass() != CLASS_PALADIN || player->GetLevel() < 50)) //todo check level req
            player->NearTeleportTo(2283.882080f, -5322.789551f, 89.235878f, 2.362668f);
    }
};

void AddSC_DekkCore_western_plaguelands()
{
    new npc_the_scourge_cauldron();
    new npc_andorhal_tower();
    new npcs_dithers_and_arbington();
    new npc_myranda_the_hag();
    new zone_western_plaguelands();
}
