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

#include "Conversation.h"
#include "Creature.h"
#include "ObjectMgr.h"
#include "PhasingHandler.h"
#include "Player.h"
#include "ScriptedCreature.h"
#include "ScriptedEscortAI.h"
#include "ScriptedGossip.h"
#include "TemporarySummon.h"
#include "GameObjectAI.h"

enum StormwindQuests
{
    AREA_STORMWIND_CITY                        = 1519,

    SPELL_KILL_CREDIT_REPORT_ANDUIN            = 269581,
    SPELL_CONVERSATION_TIDES_OF_WAR_MAIN       = 240612,
    SPELL_CONVERSATION_TIDES_OF_WAR_POST_MOVIE = 274244,
    SPELL_STORMWIND_TO_BORALUS_TRANSITION      = 240872,
    SPELL_INVISIBLE                            = 65050,

    NPC_TIDES_OF_WAR_JAINA = 120590,
    NPC_VISION_OF_SAILOR_MEMORY = 139645,
    NPC_ANDUIN_WRYNN = 120756,
    NPC_GENN_GREYMANE = 45253,
    NPC_MASTER_MATHIAS_SHAW = 139636,

    QUEST_THE_ALLIANCE_WAY = 30988,    //Pandaria Quest
    QUEST_AN_OLD_PIT_FIGHTER = 30989,
    QUEST_TIDES_OF_WAR = 46727,
    QUEST_THE_NATION_OF_KULTIRAS = 46728,
    QUEST_THE_MISSION = 29548,
    QEUST_VICTERS_SPOILS = 42982,
    QUEST_WHERE_KINGS_WALK = 13188,
    QUEST_A_CHILLING_SUMMONS = 60545,
    QUEST_THE_DARK_PORTAL = 34398,
    QUEST_THE_LEGION_RETURNS = 40519,
    QUEST_TO_THE_DRAGON_ISLES = 67700,
};
enum MapsID
{
    MAP_BLASTED_LANDS_PHASED = 1190
};

Position blastedlandsteleportpos{ -11279.2f, -3639.3f, 200.3f, 0.56f };



class sl_start_a_chilling_summons : public PlayerScript
{
public:
    sl_start_a_chilling_summons() : PlayerScript("sl_start_a_chilling_summons") { }

    void OnLogin(Player* player, bool /*firstLogin*/) override
    {
        if (player->GetZoneId() == AREA_STORMWIND_CITY && player->GetLevel() >= 50)
        {
            if (!CLASS_DEATH_KNIGHT || !CLASS_DEMON_HUNTER)
                return;

            if (player->GetClass() == CLASS_DEATH_KNIGHT)
            {
                if (const Quest* quest = sObjectMgr->GetQuestTemplate(QUEST_A_CHILLING_SUMMONS))
                    if (player->GetQuestStatus(QUEST_A_CHILLING_SUMMONS) == QUEST_STATUS_NONE)
                    {
                        player->AddQuest(quest, nullptr);
                    }

                if (player->HasQuest(QUEST_A_CHILLING_SUMMONS))
                {
                    player->GetScheduler().Schedule(Milliseconds(10000), [this, player](TaskContext context)
                        {
                            player->PlayConversation(16006);
                        });
                }
            }

            if (player->GetClass() == CLASS_DEMON_HUNTER)
            {
                if (const Quest* quest = sObjectMgr->GetQuestTemplate(QUEST_A_CHILLING_SUMMONS))
                    if (player->GetQuestStatus(QUEST_A_CHILLING_SUMMONS) == QUEST_STATUS_NONE)
                    {
                        player->AddQuest(quest, nullptr);
                    }

                if (player->HasQuest(QUEST_A_CHILLING_SUMMONS))
                {
                    player->GetScheduler().Schedule(Milliseconds(10000), [this, player](TaskContext context)
                        {
                            player->PlayConversation(16007);
                        });
                }
            }
        }
    }

};

class npc_tides_of_war_q46727 : public CreatureScript
{
public:
    npc_tides_of_war_q46727() : CreatureScript("npc_tides_of_war_q46727") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_tides_of_war_q46727AI(creature);
    }

    struct npc_tides_of_war_q46727AI : public ScriptedAI
    {
        npc_tides_of_war_q46727AI(Creature* creature) : ScriptedAI(creature) {}

        void OnQuestAccept(Player* player, Quest const* quest) override
        {
            if (player->HasQuest(QUEST_TIDES_OF_WAR))
            {
                me->AddDelayedEvent(3000, [this, player]() -> void
                    {
                        player->PlayConversation(4818);
                    });
            }
        }

    };

};

class npc_anduin_wrynn_107574 : public CreatureScript
{
public:
    npc_anduin_wrynn_107574() : CreatureScript("npc_anduin_wrynn_107574") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_anduin_wrynn_107574AI(creature);
    }

    struct npc_anduin_wrynn_107574AI : public EscortAI
    {
        npc_anduin_wrynn_107574AI(Creature* creature) : EscortAI(creature)
        {
            Reset();
        }

        bool EventStart;
        uint32 uiStep;
        uint32 uiPhase_timer;
        ObjectGuid m_playerGUID;

        void Reset() override
        {
            EventStart = false;
            uiStep = 0;
            uiPhase_timer = 3000;
            m_playerGUID.Clear();
        }


        void MoveInLineOfSight(Unit* who) override
        {
            if (Player* player = who->ToPlayer())
                if (player->IsInDist(me, 100.0f))
                    if (player->HasQuest(QUEST_TIDES_OF_WAR) || player->HasQuest(QUEST_THE_NATION_OF_KULTIRAS))
                     {
                        player->AddAura(SPELL_INVISIBLE, me);
                        if (Creature* anduin = player->FindNearestCreature(NPC_ANDUIN_WRYNN, 50.0f))
                        anduin->RemoveAura(SPELL_INVISIBLE);
                     }
        }

        void OnQuestAccept(Player* player, Quest const* quest) override
        {
            if (player->HasQuest(QEUST_VICTERS_SPOILS))
            {
                player->CastSpell(player, 218029, true);
                player->CastSpell(player, 218023, true);
            }
            if (player->HasQuest(QUEST_THE_ALLIANCE_WAY))
            {
                m_playerGUID = player->GetGUID();
                if (Creature* aysa = me->FindNearestCreature(60566, 30.0f))
                    aysa->ForcedDespawn();
                if (TempSummon* aysa2 = me->SummonCreature(60566, Position(-8370.22f, 236.653f, 155.746f, 5.5363f), TEMPSUMMON_TIMED_DESPAWN, 60s))
                    aysa2->GetMotionMaster()->MoveFollow(me, 1.f, 210);
                if (Creature* jojo = me->FindNearestCreature(60567, 30.0f))
                    jojo->ForcedDespawn();
                if (TempSummon* jojo2 = me->SummonCreature(60567, Position(-8365.39f, 240.071f, 155.776f, 5.0651f), TEMPSUMMON_TIMED_DESPAWN, 60s))
                    jojo2->GetMotionMaster()->MoveFollow(me, 3.f, 160);
                me->RemoveAura(281643);
                Start(true);
            }
        }
        void WaypointReached(uint32 waypointId, uint32 /*pathId*/) override
        {
            switch (waypointId)
            {
            case 1:
                Talk(4);
                break;
            case 2:
                Talk(5);
                break;
            case 3:
                Talk(6);
                break;
            case 4:
                Talk(7);
                me->AddDelayedEvent(5000, [this]() -> void
                    {
                        if (Creature* aysa = me->FindNearestCreature(60566, 30.0f))
                            aysa->AI()->Talk(5);
                    });
                break;
            case 6:
                Talk(8);
                break;
            case 7:
                Talk(9);
                break;
            case 9:
                Talk(10);
                me->AddDelayedEvent(6000, [this]() -> void
                    {
                        if (Creature* aysa = me->FindNearestCreature(60566, 30.0f))
                            aysa->AI()->Talk(6);
                    });
                break;
            case 10:
                Talk(11);
                break;
            case 11:
                Talk(12);
                break;
            case 12:
                Talk(13);
                me->AddDelayedEvent(4500, [this]() -> void
                    {
                        Talk(14);
                    });
                me->AddDelayedEvent(12500, [this]() -> void
                    {
                        Talk(15);
                    });
                me->AddDelayedEvent(22500, [this]() -> void
                    {
                        Talk(16);
                    });
                break;
            case 20:
                SetEscortPaused(true);
                Talk(17);
                me->AddDelayedEvent(3500, [this]() -> void
                    {
                        if (Creature* aysa = me->FindNearestCreature(60566, 30.0f))
                            aysa->AI()->Talk(7);
                    });
                me->AddDelayedEvent(7500, [this]() -> void
                    {
                        if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                        {
                            player->KilledMonsterCredit(61798);
                            me->SetNpcFlag(UNIT_NPC_FLAG_QUESTGIVER);
                            me->SetFacingToObject(player);
                            Talk(18, player);
                        }
                    });
                break;;
            }
        }

        void OnQuestReward(Player* player, Quest const* quest, LootItemType type, uint32 /*opt*/) override
        {
            if (quest->GetQuestId() == QUEST_WHERE_KINGS_WALK)
            {
                uiStep = 0;
                EventStart = true;
            }
        }

        void UpdateAI(uint32 diff) override
        {
            EscortAI::UpdateAI(diff);

            if (!EventStart)
                return;

            if (uiStep > 4)
            {
                Reset();
                return;
            }

            if (uiPhase_timer <= diff)
            {
                switch (uiStep)
                {
                case 0:
                    JumpToNextStep(3000);
                    break;

                case 1:
                    Talk(0);
                    JumpToNextStep(6000);
                    break;

                case 2:
                    Talk(1);
                    JumpToNextStep(10000);
                    break;

                case 3:
                    Talk(2);
                    JumpToNextStep(8000);
                    break;

                case 4:
                    Talk(3);
                    JumpToNextStep(3000);
                    break;
                }
            }
            else uiPhase_timer -= diff;
        }

        void JumpToNextStep(uint32 uiTimer)
        {
            uiPhase_timer = uiTimer;
            ++uiStep;
        }

    };
};

// 120756 q_46727_tides_of_war
class npc_anduin_wrynn_120756 : public CreatureScript
{
public:
    npc_anduin_wrynn_120756() : CreatureScript("npc_anduin_wrynn_120756") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_anduin_wrynn_120756AI(creature);
    }

    struct npc_anduin_wrynn_120756AI : public ScriptedAI
    {
        npc_anduin_wrynn_120756AI(Creature* creature) : ScriptedAI(creature)
        {
            Reset();
        }

        bool EventStart;
        bool EventStart2;
        uint32 uiStep1;
        uint32 uiStep2;
        uint32 uiPhase_timer;

        ObjectGuid m_playerGUID;
        ObjectGuid uiJainaGUID;
        ObjectGuid uiShawGUID;
        ObjectGuid uiGennGUID;

        void Reset() override
        {
            EventStart = false;
            EventStart2 = false;
            uiStep1 = 0;
            uiStep2 = 0;
            uiPhase_timer = 3000;

            uiJainaGUID.Clear();
            uiShawGUID.Clear();
            uiGennGUID.Clear();
        }

        void OnQuestAccept(Player* player, Quest const* quest) override
        {
            if (player->HasQuest(QUEST_THE_NATION_OF_KULTIRAS))
            {
                        if (Creature* jaina = GetClosestCreatureWithEntry(me, NPC_TIDES_OF_WAR_JAINA, 30.0f))
                            uiJainaGUID = jaina->GetGUID();
                        if (Creature* shaw = GetClosestCreatureWithEntry(me, NPC_MASTER_MATHIAS_SHAW, 30.0f))
                            uiShawGUID = shaw->GetGUID();
                        if (Creature* genn = GetClosestCreatureWithEntry(me, NPC_GENN_GREYMANE, 30.0f))
                            uiGennGUID = genn->GetGUID();

                        uiStep2 = 30;
                        EventStart2 = true;
            }
        }

        void MoveInLineOfSight(Unit* unit) override
        {
            Player* player = unit->ToPlayer();

            if (!player)
                return;

            if (!player->HasQuest(QUEST_TIDES_OF_WAR) || player->GetQuestObjectiveProgress(QUEST_TIDES_OF_WAR, 0))
                return;

            if (player->IsInDist(me, 30.0f))
            {
                m_playerGUID = player->GetGUID();                   // write playerguid in empty_guid

                if (player->GetQuestStatus(QUEST_TIDES_OF_WAR) == QUEST_STATUS_INCOMPLETE)
                {
                    if (Creature* jaina = GetClosestCreatureWithEntry(me, NPC_TIDES_OF_WAR_JAINA, 30.0f))
                        uiJainaGUID = jaina->GetGUID();
                    if (Creature* shaw = GetClosestCreatureWithEntry(me, NPC_MASTER_MATHIAS_SHAW, 30.0f))
                        uiShawGUID = shaw->GetGUID();
                    if (Creature* genn = GetClosestCreatureWithEntry(me, NPC_GENN_GREYMANE, 30.0f))
                        uiGennGUID = genn->GetGUID();

                    player->CastSpell(player, SPELL_KILL_CREDIT_REPORT_ANDUIN, true);
                    uiStep1 = 0;
                    EventStart = true;
                }
            }
        }

        void UpdateAI(uint32 diff) override
        {
            if (uiStep1 > 28)
            {
                Reset();
                return;
            }

            if (uiStep2 > 38)
            {
                Reset();
                return;
            }

            if (EventStart)
            {
                if (uiPhase_timer <= diff)  //quest 46727 dialog_start
                {
                    switch (uiStep1)
                    {
                    case 0:
                        if (Creature* jaina = ObjectAccessor::GetCreature(*me, uiJainaGUID))
                            jaina->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
                        JumpToNextStep(3000);
                        break;

                    case 1:
                        Talk(0);
                        JumpToNextStep(5000);
                        break;

                    case 2:
                        if (Creature* shaw = ObjectAccessor::GetCreature(*me, uiShawGUID))
                            shaw->AI()->Talk(0);
                        JumpToNextStep(6000);
                        break;

                    case 3:
                        if (Creature* shaw = ObjectAccessor::GetCreature(*me, uiShawGUID))
                            shaw->AI()->Talk(1);
                        JumpToNextStep(3000);
                        break;

                    case 4:
                        Talk(1);
                        JumpToNextStep(6000);
                        break;

                    case 5:
                        if (Creature* shaw = ObjectAccessor::GetCreature(*me, uiShawGUID))
                            shaw->AI()->Talk(2);
                        JumpToNextStep(3000);
                        break;

                    case 6:
                        if (Creature* shaw = ObjectAccessor::GetCreature(*me, uiShawGUID))
                            shaw->AI()->Talk(3);
                        JumpToNextStep(6000);
                        break;

                    case 7:
                        if (Creature* shaw = ObjectAccessor::GetCreature(*me, uiShawGUID))
                            shaw->AI()->Talk(4);
                        JumpToNextStep(4000);
                        break;

                    case 8:
                        if (Creature* genn = ObjectAccessor::GetCreature(*me, uiGennGUID))
                            genn->AI()->Talk(0);
                        JumpToNextStep(5000);
                        break;

                    case 9:
                        if (Creature* shaw = ObjectAccessor::GetCreature(*me, uiShawGUID))
                            shaw->AI()->Talk(5);
                        JumpToNextStep(6000);
                        break;

                    case 10:
                        if (Creature* jaina = ObjectAccessor::GetCreature(*me, uiJainaGUID))
                        {
                            jaina->SetWalk(true);
                            jaina->GetMotionMaster()->MovePoint(0, -8372.204f, 246.4205f, 155.347122f, false, 3.7752f);
                        }
                        JumpToNextStep(3000);
                        break;

                    case 11:
                        if (Creature* jaina = ObjectAccessor::GetCreature(*me, uiJainaGUID))
                        {
                            if (Creature* sailor = GetClosestCreatureWithEntry(me, 139642, 20.0f))
                                jaina->SetFacingToObject(sailor);
                            jaina->AI()->Talk(0);
                        }
                        JumpToNextStep(7000);
                        break;

                    case 12:
                        if (Creature* jaina = ObjectAccessor::GetCreature(*me, uiJainaGUID))
                            jaina->AI()->Talk(1);
                        JumpToNextStep(4000);
                        break;

                    case 13:
                        if (Creature* jaina = ObjectAccessor::GetCreature(*me, uiJainaGUID))
                            jaina->CastSpell(jaina, 54219, true);
                        JumpToNextStep(3000);
                        break;

                    case 14:
                        if (Creature* memory = GetClosestCreatureWithEntry(me, NPC_VISION_OF_SAILOR_MEMORY, 50.0f))
                            memory->RemoveAura(SPELL_INVISIBLE);
                        JumpToNextStep(2000);
                        break;

                    case 15:
                        if (Creature* jaina = ObjectAccessor::GetCreature(*me, uiJainaGUID))
                            jaina->CastStop();
                        JumpToNextStep(2000);
                        break;

                    case 16:
                        if (Creature* memory = GetClosestCreatureWithEntry(me, NPC_VISION_OF_SAILOR_MEMORY, 50.0f))
                            memory->AddAura(SPELL_INVISIBLE, memory);
                        JumpToNextStep(1000);
                        break;

                    case 17:
                        if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                        {
                            player->GetScheduler().Schedule(1s, [this, player](TaskContext context)
                                {
                                    player->CastSpell(player, SPELL_CONVERSATION_TIDES_OF_WAR_POST_MOVIE, true);
                                });
                        }
                        JumpToNextStep(32000);
                        break;

                    case 18:
                        Talk(2);
                        JumpToNextStep(7000);
                        break;

                    case 19:
                        if (Creature* jaina = ObjectAccessor::GetCreature(*me, uiJainaGUID))
                        {
                            jaina->SetFacingToObject(me);
                            jaina->AI()->Talk(2);
                        }
                        JumpToNextStep(4000);
                        break;

                    case 20:
                        if (Creature* jaina = ObjectAccessor::GetCreature(*me, uiJainaGUID))
                            jaina->AI()->Talk(3);
                        JumpToNextStep(5000);
                        break;

                    case 21:
                        if (Creature* jaina = ObjectAccessor::GetCreature(*me, uiJainaGUID))
                            jaina->AI()->Talk(4);
                        JumpToNextStep(4000);
                        break;

                    case 22:
                        if (Creature* genn = ObjectAccessor::GetCreature(*me, uiGennGUID))
                            genn->AI()->Talk(1);
                        JumpToNextStep(5000);
                        break;

                    case 23:
                        if (Creature* jaina = ObjectAccessor::GetCreature(*me, uiJainaGUID))
                            jaina->AI()->Talk(5);
                        JumpToNextStep(6000);
                        break;

                    case 24:
                        Talk(3);
                        JumpToNextStep(4000);
                        break;

                    case 25:
                        Talk(4);
                        JumpToNextStep(4000);
                        break;

                    case 26:
                        if (Creature* jaina = ObjectAccessor::GetCreature(*me, uiJainaGUID))
                            jaina->AI()->Talk(6);
                        JumpToNextStep(4000);
                        break;

                    case 27:
                        if (Creature* jaina = ObjectAccessor::GetCreature(*me, uiJainaGUID))
                        {
                            jaina->GetMotionMaster()->MovePoint(0, -8368.67f, 244.602f, 155.347f, false, 3.79513f);
                        }
                        JumpToNextStep(3000);
                        break;

                    case 28:
                        Talk(5);
                        JumpToNextStep(4000);
                        break;
                    }
                }
                else uiPhase_timer -= diff;
            }

            if (EventStart2)
            {
                if (uiPhase_timer <= diff) //quest 46728 dialog_start
                {
                    switch (uiStep2)
                    {
                    case 30:
                        if (Creature* jaina = ObjectAccessor::GetCreature(*me, uiJainaGUID))
                            jaina->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
                        JumpToNextStep(3000);
                        break;

                    case 31:
                        if (Creature* jaina = ObjectAccessor::GetCreature(*me, uiJainaGUID))
                            jaina->AI()->Talk(7);
                        JumpToNextStep(6000);
                        break;

                    case 32:
                        if (Creature* jaina = ObjectAccessor::GetCreature(*me, uiJainaGUID))
                        {
                            jaina->CastSpell(jaina, 54219, true);
                            jaina->ForcedDespawn(3000);
                        }
                        JumpToNextStep(1000);
                        break;

                    case 33:
                        Talk(6);
                        JumpToNextStep(4000);
                        break;

                    case 34:
                        if (Creature* genn = ObjectAccessor::GetCreature(*me, uiGennGUID))
                            genn->AI()->Talk(2);
                        JumpToNextStep(3000);
                        break;

                    case 35:
                        Talk(7);
                        JumpToNextStep(4000);
                        break;

                    case 36:
                        Talk(8);
                        JumpToNextStep(5000);
                        break;

                    case 37:
                        Talk(9);
                        JumpToNextStep(3000);
                        break;

                    case 38:
                        if (Creature* genn = ObjectAccessor::GetCreature(*me, uiGennGUID))
                            genn->AI()->Talk(3);
                        JumpToNextStep(3000);
                        break;
                    }
                }
                else uiPhase_timer -= diff;
            }
        }

        void JumpToNextStep(uint32 uiTimer)
        {
            uiPhase_timer = uiTimer;
            ++uiStep1;
            ++uiStep2;
        }
    };
};

// @TODO Rewrite levels
// 120590
class npc_jaina_tides_of_war : public CreatureScript
{
public:
    npc_jaina_tides_of_war() : CreatureScript("npc_jaina_tides_of_war") { }

    struct npc_jaina_tides_of_warAI : public ScriptedAI
    {
        npc_jaina_tides_of_warAI(Creature* creature) : ScriptedAI(creature) { }

        bool OnGossipHello(Player* player) override
        {
            ClearGossipMenuFor(player);
            AddGossipItemFor(player, GossipOptionNpc::None, "I'm ready to set sail!", GOSSIP_SENDER_MAIN, 0);
            SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, me->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, uint32 sender, uint32 action) override
        {
            switch (action)
            {
            case 0:
                if (player->HasQuest(QUEST_THE_NATION_OF_KULTIRAS))
                    player->CastSpell(player, SPELL_STORMWIND_TO_BORALUS_TRANSITION, true);
                CloseGossipMenuFor(player);
                break;
            }
            return true;
        }

    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_jaina_tides_of_warAI(creature);
    }
};


class npc_captain_rodgers_66292 : public ScriptedAI
{
public:
    npc_captain_rodgers_66292(Creature* creature) : ScriptedAI(creature) { }

    bool OnGossipSelect(Player* player, uint32 /*menuId*/, uint32 /*gossipListId*/) override
    {
        if (player->GetQuestStatus(QUEST_THE_MISSION) == QUEST_STATUS_INCOMPLETE)
        {
            player->ForceCompleteQuest(QUEST_THE_MISSION);
            player->TeleportTo(870, -676.116f, -1482.635f, 1.922f, 4.731f);
        }

        return true;
    }
};

//149626 Vanguard Battlemage
class npc_vanguard_battlemage_149626 : public ScriptedAI
{
public:
    npc_vanguard_battlemage_149626(Creature* creature) : ScriptedAI(creature) { }

    bool OnGossipHello(Player* player) override
    {
        AddGossipItemFor(player, GossipOptionNpc::None, "I must help Khadgar. Send me to the Blasted Lands!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 0);
        SendGossipMenuFor(player, 23796, me->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, uint32 /*menuId*/, uint32 /*gossipListId*/) override
    {
        if (player->GetQuestStatus(QUEST_THE_DARK_PORTAL) == QUEST_STATUS_INCOMPLETE)
        {
            player->KilledMonsterCredit(149625);
            player->TeleportTo(MAP_BLASTED_LANDS_PHASED, blastedlandsteleportpos);
        }
        return true;
    }
};

// 251195
struct go_keg_of_armor_polish : public GameObjectAI
{
    go_keg_of_armor_polish(GameObject* go) : GameObjectAI(go) { }

    bool OnGossipHello(Player* player) override
    {
        player->KilledMonsterCredit(108787);

        return true;
    }
};

//251234
struct go_Light_Infused_Crystals : public GameObjectAI
{
    go_Light_Infused_Crystals(GameObject* go) : GameObjectAI(go) { }

    bool OnGossipHello(Player* player) override
    {
        player->KilledMonsterCredit(108788);

        return true;
    }
};

//251252
struct go_Dumplings : public GameObjectAI
{
    go_Dumplings(GameObject* go) : GameObjectAI(go) { }

    bool OnGossipHello(Player* player) override
    {
        player->KilledMonsterCredit(108789);

        return true;
    }
};

enum eDuelists
{
    QUEST_TO_BE_PREPARED_A = 42782,
    KILL_CREDIT_WARM_DUEL = 108722,
    EVENT_DO_CAST = 1,
    EVENT_STOP_DUEL = 2,
    DATA_START_DUEL = 10,
    SPELL_DUEL = 52996,
    SPELL_DUEL_TRIGGERED = 52990,
    SPELL_DUEL_VICTORY = 52994,
    SPELL_DUEL_FLAG = 52991,
};

std::map<uint32, uint32> const creatureAbilities
{
    { 108722,  171773 }, //Ramall Trueoak : Wrath
    { 108744,  171863 }, // Tessa Wybroff : Arcane Missiles
    { 108748,  172772 }, // Audra Stoneshield : Thunderclap
    { 108752,  172673 }, // Ciarra Neil : Holly Smite
    { 108756,  172675 }, // Navea the Purifier : Lighting Bolt
    { 108765,  198623 }, // Seona Fireweaver : Fireball
    { 113544,  171777 }, // Neejala : Starfire
    { 108723,  171777 }, // Kihra : Starfire
    { 108745,  172029 }, // Antone Sula : Mutilate
    { 108749,  172769 }, // Bridgette Hicks : Mortal Strike
    { 108753,  172670 }, // Rorin Rivershade : MindFlay
    { 108757,  171884 }, // Haagios : denounce
    { 108767,  172757 }, // Kiruud the Relentless : BloodThirst
};

#define GOSSIP_LETS_DUEL "Let's duel."

enum legionreturns
{
    QUEST_TO_BE_PREPARED_H = 44281,
    QUEST_THE_BATTLE_FOR_BROKEN_SHORE = 40518,
    PHASE_LEGION_RETURNS_H = 172,
    PHASE_169 = 169,
    PHASE_COSMETIC = 11695,
};

class npc_stormwind_duelist : public ScriptedAI
{
public:
    npc_stormwind_duelist(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        //_events.Reset();
        me->RestoreFaction();
        me->SetReactState(REACT_DEFENSIVE);
    }

    bool OnGossipHello(Player* player) override
    {
        if (player->GetQuestStatus(QUEST_TO_BE_PREPARED_A) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(QUEST_TO_BE_PREPARED_H) == QUEST_STATUS_INCOMPLETE)
            AddGossipItemFor(player, GossipOptionNpc::None, GOSSIP_LETS_DUEL, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        SendGossipMenuFor(player, player->GetGossipTextId(me), me->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, uint32 sender, uint32 gossipListId) override
    {
        player->PlayerTalkClass->ClearMenus();
        if (GOSSIP_ACTION_INFO_DEF + 1)
        {
            me->AI()->SetGUID(player->GetGUID());
            me->SetFaction(FACTION_TEMPLATE_FLAG_PVP);
            me->RemoveUnitFlag(UNIT_FLAG_IMMUNE_TO_PC);
            me->SetReactState(REACT_AGGRESSIVE);

            player->CastSpell(me, SPELL_DUEL, false);
            player->CastSpell(player, SPELL_DUEL_FLAG, true);

            me->AI()->AttackStart(player);
            CloseGossipMenuFor(player);
        }
        return true;
    }

    void JustEngagedWith(Unit* who) override {
        _events.ScheduleEvent(EVENT_DO_CAST, 1s);
    }

    void DamageTaken(Unit* attacker, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/)override
    {
        if (damage >= me->GetHealth())
        {
            damage = me->GetHealth() - 1;
            _events.Reset();
            me->RemoveAllAuras();
            me->SetFaction(35);
            me->AttackStop();
            attacker->AttackStop();
            attacker->ClearInCombat();
            attacker->ToPlayer()->KilledMonsterCredit(KILL_CREDIT_WARM_DUEL);
            _events.ScheduleEvent(EVENT_STOP_DUEL, 1s);
        }
    }

    void UpdateAI(uint32 diff) override {
        if (!UpdateVictim())
            return;

        _events.Update(diff);

        while (uint32 eventId = _events.ExecuteEvent()) {
            switch (eventId) {
            case EVENT_DO_CAST:
                DoCastVictim(creatureAbilities.at(me->GetEntry()));
                _events.RescheduleEvent(EVENT_DO_CAST, 4s);
                break;
            case EVENT_STOP_DUEL:
                me->Say("Fine duel.", LANG_UNIVERSAL, nullptr);
                me->GetMotionMaster()->MoveTargetedHome();
                break;
            default:
                break;
            }
        }
        DoMeleeAttackIfReady();
    }

    void SetGUID(ObjectGuid const& guid, int32 /*p_Index*/) override
    {
        _playerGuid = guid;
    }

private:
    EventMap _events;
    ObjectGuid _playerGuid = ObjectGuid::Empty;

};

// 108920
class npc_Captain_Angelica : public ScriptedAI
{
public:
    npc_Captain_Angelica(Creature* creature) : ScriptedAI(creature) { }

    bool OnGossipHello(Player* player) override
    {
        player->PlayerTalkClass->ClearMenus();

        //214608
        player->KilledMonsterCredit(me->GetEntry());
        player->CastSpell(player, 216356, false); //scene

        player->CastSpell(player, 227058, false); //join Broken Shore Scenario

        player->GetScheduler().Schedule(Milliseconds(10000), [this, player](TaskContext context)
            {
                player->TeleportTo(1460, 445.688f, 2074.756f, 0.154f, 0.1085f);
                player->RemoveAurasDueToSpell(216356); //scene
            });
        return true;
    };
};

// recluiter lee 107934
struct npc_recluiter_lee_107934 : public ScriptedAI
{
    npc_recluiter_lee_107934(Creature* creature) : ScriptedAI(creature) { }

    bool OnGossipHello(Player* player) override
    {
        if (me->IsQuestGiver())
            player->PrepareQuestMenu(me->GetGUID());

        if (player->HasQuest(QUEST_THE_LEGION_RETURNS))
            player->ForceCompleteQuest(QUEST_THE_LEGION_RETURNS);

        SendGossipMenuFor(player, player->GetGossipTextId(me), me->GetGUID());
        return true;
    }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_TO_BE_PREPARED_A)
        {
            PhasingHandler::AddPhase(player, PHASE_LEGION_RETURNS_H, true);
        }
    }
};

// holgar stormaxe 4311
struct npc_holgar_stormaxe : public ScriptedAI
{
    npc_holgar_stormaxe(Creature* creature) : ScriptedAI(creature) { }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_TO_BE_PREPARED_H)
        {
            PhasingHandler::AddPhase(player, PHASE_LEGION_RETURNS_H, true);
        }
    }
};

// stone_guard_mukar 113547
struct npc_stone_guard_mukar : public ScriptedAI
{
    npc_stone_guard_mukar(Creature* creature) : ScriptedAI(creature) { }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_THE_BATTLE_FOR_BROKEN_SHORE)
        {
            PhasingHandler::AddPhase(player, PHASE_LEGION_RETURNS_H, true);
            PhasingHandler::AddPhase(player, PHASE_169, true);
            PhasingHandler::AddPhase(player, PHASE_COSMETIC, true);
        }
    }
};

// captain rusoo 113118
struct npc_captain_ruso : public ScriptedAI
{
    npc_captain_ruso(Creature* creature) : ScriptedAI(creature) { }

    bool OnGossipSelect(Player* player, uint32 /*menuId*/, uint32 /*gossipListId*/) override
    {
        player->PlayerTalkClass->ClearMenus();

        player->KilledMonsterCredit(113118);
        player->CastSpell(player, 225147, false); //scene

        player->CastSpell(player, 227058, false); //join Broken Shore Scenario

        return true;
    }
};

enum
{
    QUEST_FATE_OF_THE_HORDE = 40522,
};

// eitrigg 100453
struct npc_eitrigg : public ScriptedAI
{
    npc_eitrigg(Creature* creature) : ScriptedAI(creature) { }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_FATE_OF_THE_HORDE)
        {
            player->KilledMonsterCredit(100541);
            player->KilledMonsterCredit(100552);
            player->KilledMonsterCredit(100934);
            player->KilledMonsterCredit(100985);
            PhasingHandler::RemovePhase(player, PHASE_LEGION_RETURNS_H, true);
            PhasingHandler::RemovePhase(player, PHASE_169, true);
            PhasingHandler::RemovePhase(player, PHASE_COSMETIC, true);
        }
    }
};

class npc_102585_jace_for_dh_questline : public CreatureScript
{
public:
    npc_102585_jace_for_dh_questline() : CreatureScript("npc_102585_jace_for_dh_questline") { }

    struct npc_102585_jace_for_dh_questlineAI : ScriptedAI
    {
        npc_102585_jace_for_dh_questlineAI(Creature* creature) : ScriptedAI(creature) {}

        EventMap events;

        void MoveInLineOfSight(Unit* who) override
        {
            if (events.Empty())
                events.ScheduleEvent(1, 2s);

            if (who->GetTypeId() != TYPEID_PLAYER)
                return;

            Player* player = who->ToPlayer();

            if (player->GetQuestStatus(39691) != QUEST_STATUS_INCOMPLETE)
                return;

            player->KilledMonsterCredit(102585);
        }

        void UpdateAI(uint32 diff) override
        {
            events.Update(diff);

            if (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case 1:
                    events.ScheduleEvent(1, 2s);

                    std::list<Player*> list;
                    me->GetPlayerListInGrid(list, 20.0f);
                    for (Player* player : list)
                        if (player->GetQuestStatus(44471) == QUEST_STATUS_INCOMPLETE)
                            player->KilledMonsterCredit(102563);

                    break;
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_102585_jace_for_dh_questlineAI(creature);
    }
};

// Chrovo 198079
class npc_chrovo_198079 : public CreatureScript
{
public:
    npc_chrovo_198079() : CreatureScript("npc_chrovo_198079") { }

    struct npc_chrovo_198079AI : public ScriptedAI
    {
        npc_chrovo_198079AI(Creature* creature) : ScriptedAI(creature) { }

        bool OnGossipHello(Player* player) override
        {
            ClearGossipMenuFor(player);
            AddGossipItemFor(player, GossipOptionNpc::None, "I'm ready to go to waking shore!", GOSSIP_SENDER_MAIN, 0);
            SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, me->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, uint32 sender, uint32 action) override
        {
            switch (action)
            {
            case 0:
                if (player->HasQuest(QUEST_TO_THE_DRAGON_ISLES))
                    player->ForceCompleteQuest(QUEST_TO_THE_DRAGON_ISLES);
                    player->TeleportTo(2444, 3732.435f, -1900.060f, 6.006f, 2.6638f);

                CloseGossipMenuFor(player);
                break;
            }
            return true;
        }

    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_chrovo_198079AI(creature);
    }
};


void AddSC_DekkCore_stormwind_city()
{
    new sl_start_a_chilling_summons();
    new npc_tides_of_war_q46727();
    new npc_anduin_wrynn_107574();
    new npc_anduin_wrynn_120756();
    new npc_jaina_tides_of_war();
    RegisterCreatureAI(npc_captain_rodgers_66292);
    RegisterCreatureAI(npc_vanguard_battlemage_149626);
    RegisterGameObjectAI(go_keg_of_armor_polish);
    RegisterGameObjectAI(go_Light_Infused_Crystals);
    RegisterGameObjectAI(go_Dumplings);
    RegisterCreatureAI (npc_stormwind_duelist);
    RegisterCreatureAI(npc_Captain_Angelica);
    RegisterCreatureAI(npc_recluiter_lee_107934);
    RegisterCreatureAI(npc_holgar_stormaxe);
    RegisterCreatureAI(npc_stone_guard_mukar);
    RegisterCreatureAI (npc_captain_ruso);
    RegisterCreatureAI(npc_eitrigg);
    new npc_chrovo_198079();
}
