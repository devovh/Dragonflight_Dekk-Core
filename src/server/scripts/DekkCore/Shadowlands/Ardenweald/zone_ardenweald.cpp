
/*
 * Copyright (C) 2021
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

#include "PhasingHandler.h"
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "TemporarySummon.h"

enum ardenwealdAreaId
{
    AREA_ARDENWEALD = 11510,
};

enum ardenwealdQuests
{
    QUEST_THE_BATTLE_OF_ARDENWEALD = 63578,
    QUEST_CANT_TURN_OUR_BACKS = 63638,
    QUEST_THE_HEART_OF_ARDENWEALD = 63904,
    QUEST_REPORT_TO_ORIBOS = 63639,
};

enum ardenwealdSpells
{
    SPELL_FINISH_THE_BATTLE_OF_ARDENWEALD = 349960,

    SPELL_SCENE_ARDENWEALD_ANDUIN_TAKES_THE_SIGIL = 353843,     //packageID 3296, sceneID 2786
};

class player_ardenweald : public PlayerScript
{
public:
    player_ardenweald() : PlayerScript("player_ardenweald") { }

    void OnPlayerRepop(Player* player) override
    {
        OnCheckPhase(player);
    }

    void OnPlayerResurrect(Player* player) override
    {
        OnCheckPhase(player);
    }

    void OnQuestStatusChange(Player* player, uint32 questId) override
    {
        OnCheckPhase(player);
    }

    void OnUpdateArea(Player* player, uint32 newArea, uint32 /*oldArea*/) override
    {
        OnCheckPhase(player);

        player->GetScheduler().Schedule(Milliseconds(2000), [player](TaskContext context)
            {
                if (player->GetQuestStatus(QUEST_THE_BATTLE_OF_ARDENWEALD) == QUEST_STATUS_COMPLETE || player->GetQuestStatus(QUEST_THE_BATTLE_OF_ARDENWEALD) == QUEST_STATUS_REWARDED)
                    if (!player->GetPhaseShift().HasPhase(11510)) PhasingHandler::AddPhase(player, 11510, true);
            });
    }

    void OnCheckPhase(Player* player)
    {
        if (player->GetMapId() != 2222)
            return;

        if (player->GetAreaId() == AREA_ARDENWEALD) //Ardernweald
        {
            if (player->GetQuestStatus(QUEST_THE_BATTLE_OF_ARDENWEALD) == QUEST_STATUS_COMPLETE || player->GetQuestStatus(QUEST_THE_BATTLE_OF_ARDENWEALD) == QUEST_STATUS_REWARDED)
            {
                if (!player->GetPhaseShift().HasPhase(11510)) PhasingHandler::AddPhase(player, 11510, true);
            }
            if (player->GetQuestStatus(QUEST_CANT_TURN_OUR_BACKS) == QUEST_STATUS_COMPLETE || player->GetQuestStatus(QUEST_CANT_TURN_OUR_BACKS) == QUEST_STATUS_REWARDED)
            {
                if (player->GetPhaseShift().HasPhase(11510)) PhasingHandler::RemovePhase(player, 11510, true);
            }
        }
    }

    void OnMovieComplete(Player* player, uint32 movieId) override
    {
        if (movieId == 951)
            player->CastSpell(player, SPELL_FINISH_THE_BATTLE_OF_ARDENWEALD, false);
    }

};


// 177148 lady moonberry 
struct npc_lady_moonberry_177148 : public  ScriptedAI
{
public:
    npc_lady_moonberry_177148(Creature* creature) : ScriptedAI(creature) {}

    void OnQuestReward(Player* player, Quest const* quest, LootItemType type, uint32 /*opt*/) override
    {
        if (quest->GetQuestId() == QUEST_THE_BATTLE_OF_ARDENWEALD)
        {
            if (Creature* ysera = me->FindNearestCreature(177446, 50.0f))
            {
                ysera->AI()->Talk(0);
                player->GetScheduler().Schedule(Milliseconds(5000), [this](TaskContext context)
                    {
                        me->AI()->Talk(0);
                    });
            }
        }
    }

};

// 177460 Winter Queen 
struct npc_winter_queen_177460 : public  ScriptedAI
{
public:
    npc_winter_queen_177460(Creature* creature) : ScriptedAI(creature) {}

    bool OnGossipHello(Player* player) override
    {
        if (me->IsQuestGiver())
            player->PrepareQuestMenu(me->GetGUID());
        if (player->GetQuestStatus(QUEST_CANT_TURN_OUR_BACKS) == QUEST_STATUS_INCOMPLETE)
        {
            AddGossipItemFor(player, GossipOptionNpc::None, "Is everything under control here?", 26928, GOSSIP_ACTION_INFO_DEF + 0);
        }
        SendGossipMenuFor(player, player->GetGossipTextId(me), me->GetGUID());

        return true;
    }

    bool OnGossipSelect(Player* player, uint32 MenuID, uint32 gossipListId) override
    {
        uint32 const action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);
        ClearGossipMenuFor(player);
        if (action == GOSSIP_ACTION_INFO_DEF + 0)
        {
            if (player->HasQuest(QUEST_CANT_TURN_OUR_BACKS))
            {
                player->KilledMonsterCredit(177460);
                me->AI()->Talk(0);
            }
        }

        return true;
    }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_THE_HEART_OF_ARDENWEALD)
        {
            Talk(1);
            player->GetScheduler().Schedule(Milliseconds(9000), [this](TaskContext context)
                {
                    Talk(2);
                });
        }
        if (quest->GetQuestId() == QUEST_REPORT_TO_ORIBOS)
        {
            Talk(3);
            player->GetScheduler().Schedule(Milliseconds(13000), [this](TaskContext context)
                {
                    Talk(4);
                });
        }
    }

};


// 178565 Highlord Bolvar Fordragon
struct npc_highlord_bolvar_fordragon_178565 : public  ScriptedAI
{
public:
    npc_highlord_bolvar_fordragon_178565(Creature* creature) : ScriptedAI(creature) {}

private:
    bool  quest_thoa_63904;
    ObjectGuid m_playerGUID;

    void Reset() override
    {
        quest_thoa_63904 = false;
        m_playerGUID = ObjectGuid::Empty;
    }

    void MoveInLineOfSight(Unit* target) override
    {
        if (!target->IsPlayer())
            return;

        if (Player* player = target->ToPlayer())
        {
            if (!quest_thoa_63904)
            {
                if (player->GetDistance2d(me) <= 20.0f)
                    if (player->HasQuest(QUEST_THE_HEART_OF_ARDENWEALD))
                    {
                        m_playerGUID = player->GetGUID();
                        quest_thoa_63904 = true;
                        Talk(0);
                    }
            }
            if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                if (!player->IsWithinDistInMap(me, 40.0f) && quest_thoa_63904)
                    if (player->HasQuest(QUEST_THE_HEART_OF_ARDENWEALD))
                        quest_thoa_63904 = false;
        }
    }


    bool OnGossipHello(Player* player) override
    {
        if (me->IsQuestGiver())
            player->PrepareQuestMenu(me->GetGUID());
        if (player->GetQuestStatus(QUEST_THE_HEART_OF_ARDENWEALD) == QUEST_STATUS_INCOMPLETE)
        {
            AddGossipItemFor(player, GossipOptionNpc::None, "Is the sigil safe?", 26930, GOSSIP_ACTION_INFO_DEF + 0);
        }
        SendGossipMenuFor(player, player->GetGossipTextId(me), me->GetGUID());

        return true;
    }

    bool OnGossipSelect(Player* player, uint32 MenuID, uint32 gossipListId) override
    {
        uint32 const action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);
        ClearGossipMenuFor(player);
        if (action == GOSSIP_ACTION_INFO_DEF + 0)
        {
            if (player->HasQuest(QUEST_THE_HEART_OF_ARDENWEALD))
            {
                player->KilledMonsterCredit(178565);
                player->CastSpell(player, SPELL_SCENE_ARDENWEALD_ANDUIN_TAKES_THE_SIGIL, false);
            }
        }

        return true;
    }
};

// 165701 Ceridwyn Flight Master
struct npc_ceridwyn_165701 : public  ScriptedAI
{
public:
    npc_ceridwyn_165701(Creature* creature) : ScriptedAI(creature) {}

    bool OnGossipHello(Player* player) override
    {
        if (me->IsQuestGiver())
            player->PrepareQuestMenu(me->GetGUID());
        if (player->HasQuest(QUEST_REPORT_TO_ORIBOS))
        {
            AddGossipItemFor(player, GossipOptionNpc::None, "I'd like to fly to Oribos.", 6944, GOSSIP_ACTION_INFO_DEF + 0);
        }
        SendGossipMenuFor(player, player->GetGossipTextId(me), me->GetGUID());

        return true;
    }

    bool OnGossipSelect(Player* player, uint32 MenuID, uint32 gossipListId) override
    {
        uint32 const action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);
        ClearGossipMenuFor(player);
        if (action == GOSSIP_ACTION_INFO_DEF + 0)
        {
            if (player->HasQuest(QUEST_REPORT_TO_ORIBOS))
                player->TeleportTo(2222, -1902.16f, 1214.65f, 5450.87f, 3.8058f, TELE_TO_NONE, false);
        }
        return true;
    }
};


void AddSC_zone_ardenweald()
{
    new player_ardenweald();
    RegisterCreatureAI(npc_lady_moonberry_177148);
    RegisterCreatureAI(npc_winter_queen_177460);
    RegisterCreatureAI(npc_highlord_bolvar_fordragon_178565);
    RegisterCreatureAI(npc_ceridwyn_165701);
}
