/*
 * Copyright DekkCore Team 2023
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

#include "Player.h"
#include "Creature.h"
#include "CreatureAI.h"
#include "ObjectMgr.h"
#include "GameObject.h"
#include "GameObjectAI.h"
#include "MiscPackets.h"
#include "ObjectAccessor.h"
#include "PhasingHandler.h"
#include "ScriptMgr.h"
#include "ScriptedEscortAI.h"
#include "ObjectMgr.h"
#include "SpellInfo.h"
#include "SpellScript.h"
#include "TemporarySummon.h"
#include "ScriptedGossip.h"

enum Oribos
{
    QUEST_INTO_THE_MAW = 59181,
    QUEST_STRANGER_IN_AN_EVEN_STRANGERLAND = 60129,
    QUEST_IF_U_WANT_PEACE = 57386,
    QUEST_THE_FIRST_MOVE = 63576,
    QUEST_A_GATHERING_OF_COVENANTS = 63856,
    QUEST_VOICES_OF_THE_ETERNAL = 63857,
    QUEST_TETHER_TO_HOME = 60150,
    QUEST_SEEK_THE_ASCENDED = 59773,
    QUEST_NO_PLACE_FOR_THE_LIVING = 60148,
    QUEST_AUDIENCE_WITH_THE_ARBITER = 60149,
    QUEST_A_DOORWAY_THROUGH_THE_VEIL = 60151,
    QUEST_THE_ETERNAL_CITY = 60152,
    QUEST_UNDERSTANDING_THE_SHADOWLANDS = 60154,
    QUEST_THE_PATH_TO_BASTION = 60156,
    QUEST_THE_BATTLE_OF_ARDENWEALD = 63578,
    QUEST_CANT_TURN_OUR_BACKS = 63638,
    QUEST_REPORT_TO_ORIBOS = 63639,
    QUEST_OPENING_THE_MAW = 63660,
    QUEST_LINK_TO_THE_MAW = 63661,
    QUEST_OPENING_TO_ORIBOS = 63665,
    QUEST_CHARGE_OF_THE_COVENANTS = 64007,
    QUEST_INTO_THE_VAULT = 63725,

    SCENE_ORIBOS_JUMP_INTO_MAW_PRK = 3000,
    SCENE_ORIBOS_TO_THE_MAW_SKYJUMP = 2812,
    SCENE_NO_PLACE_FOR_THE_LIVING = 342832,  //packageID 3065, sceneID 2608
    SCENE_QUEST_TETHER_TO_HOME = 355835,  //packageID 2939, sceneID 2571
    SCENE_QUEST_GATE_BASTION = 344996,  //packageID 2639, sceneID 3112
    SCENE_QUEST_VOICES_OF_THE_ETERNAL = 351659,  //packageID 3295, sceneID 2784

    MOVIE_AUDIENCE_WITH_THE_ARBITER = 345622, //MovieId 945
    MOVIE_KINGSMOURNE = 349936, //MovieId 949

    SPELL_FLY_TO_MALDRAXXUS = 334503,
    SPELL_FLY_TO_BASTION = 342908,
    SPELL_TELE_TO_ARBITER_ROOM = 328075,
    SPELL_TELE_TO_RING_OF_FALES = 328076,
    SPELL_CHAINELING = 353370,
    SPELL_ANIMA_EXTRAPOLATION = 353378,
    SPELL_MAW_JUMP_TO_COCYRUS = 328867,
    SPELL_RALLY_THE_TROOPS = 353258,
    SPELL_CROWD = 350802,
    SPELL_TELEPORT_EFFECT = 355673,
    SPELL_IN_BETWEEN_TUNNDEL = 323977,  // flight_tunnel_aura

    ACTION_BATTLE_OF_ARDENWEALD_TALK_START = 10000,
    ACTION_CHARGE_OF_THE_COVENANTS_DILOG = 20000,

    EVENT_NO_PLACE_FOR_THE_LIVING_PHASE1 = 1,
    EVENT_AUDIENCE_WITH_THE_ARBITER_PHASE1 = 2,
    EVENT_BATTLE_OF_ARDENWEALD_TALK_START = 200,

    NPC_BOLVAR = 177230,
    NPC_TAL_INARA = 177200,
    NPC_KELIA = 177574,
    NPC_BONESMITH_HEIRMIR = 177234,
    NPC_BARONESS_DRAKA = 177587,
    NPC_POLEMARCH_ADRESTES = 177136,

};

// npc_polemarch_adrestes_171787
class npc_polemarch_adrestes_171787 : public CreatureScript
{
public:
    npc_polemarch_adrestes_171787() : CreatureScript("npc_polemarch_adrestes_171787") { }

    struct npc_polemarch_adrestes_171787AI : public ScriptedAI
    {
        npc_polemarch_adrestes_171787AI(Creature* creature) : ScriptedAI(creature) {}

        bool OnGossipHello(Player* player) override
        {
            if (player->HasQuest(62000) || player->HasQuest(57878)) // This menu show only if u have Choosing Your Purpose quest.
            {
                AddGossipItemFor(player, GossipOptionNpc::PetitionVendor, "Tell me again of the virtues of the Kyrian!", GOSSIP_SENDER_MAIN, 0);
                AddGossipItemFor(player, GossipOptionNpc::PetitionVendor, "Can I please test the Kyrian covenant abilities again?", GOSSIP_SENDER_MAIN, 1);
                AddGossipItemFor(player, GossipOptionNpc::PetitionVendor, "Could you please reset the cooldown on my ability so I can train further?", GOSSIP_SENDER_MAIN, 2);
                AddGossipItemFor(player, GossipOptionNpc::PetitionVendor, "What adventures await me if I join your covenant?", GOSSIP_SENDER_MAIN, 3);
                SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, me->GetGUID());
            }

            return true;
        }

        bool OnGossipSelect(Player* player, uint32 sender, uint32 action) override
        {
            switch (action)
            {
            case 0:
                player->KilledMonsterCredit(171787);
                player->GetSession()->SendCovenantPreview(player->GetGUID(), 1);  // kyrian
                CloseGossipMenuFor(player);
                break;
            }
            return true;
        }

    };

    CreatureAI* GetAI(Creature* creature) const  override
    {
        return new npc_polemarch_adrestes_171787AI(creature);
    }
};

// npc_general_draven_171589
class npc_general_draven_171589 : public CreatureScript
{
public:
    npc_general_draven_171589() : CreatureScript("npc_general_draven_171589") { }

    struct npc_general_draven_171589AI : public ScriptedAI
    {
        npc_general_draven_171589AI(Creature* creature) : ScriptedAI(creature) {}

        bool OnGossipHello(Player* player) override
        {
            if (player->HasQuest(62000) || player->HasQuest(57878)) // This menu show only if u have Choosing Your Purpose quest.
            {
                AddGossipItemFor(player, GossipOptionNpc::PetitionVendor, "How could the Venthyr benefit me?", GOSSIP_SENDER_MAIN, 0);
                AddGossipItemFor(player, GossipOptionNpc::PetitionVendor, "Can I please test the Venthyr covenant abilities again?", GOSSIP_SENDER_MAIN, 1);
                AddGossipItemFor(player, GossipOptionNpc::PetitionVendor, "Could you please reset the cooldown on my ability so I can train further?", GOSSIP_SENDER_MAIN, 2);
                AddGossipItemFor(player, GossipOptionNpc::PetitionVendor, "What adventures await me if I join your covenant?", GOSSIP_SENDER_MAIN, 3);
                SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, me->GetGUID());
            }

            return true;
        }

        bool OnGossipSelect(Player* player, uint32 sender, uint32 action) override
        {
            switch (action)
            {
            case 0: 
                player->KilledMonsterCredit(171589);
                player->GetSession()->SendCovenantPreview(player->GetGUID(), 2);  // venthyr
                CloseGossipMenuFor(player);
                break;
            }
            return true;
        }

    };

    CreatureAI* GetAI(Creature* creature) const  override
    {
        return new npc_general_draven_171589AI(creature);
    }
};

// npc_lady_moonberry_171795
class npc_lady_moonberry_171795 : public CreatureScript
{
public:
    npc_lady_moonberry_171795() : CreatureScript("npc_lady_moonberry_171795") { }

    struct npc_lady_moonberry_171795AI : public ScriptedAI
    {
        npc_lady_moonberry_171795AI(Creature* creature) : ScriptedAI(creature) {}

        bool OnGossipHello(Player* player) override
        {
            if (player->HasQuest(62000) || player->HasQuest(57878)) // This menu show only if u have Choosing Your Purpose quest.
            {
                AddGossipItemFor(player, GossipOptionNpc::PetitionVendor, "Remind me of the Night Fae!", GOSSIP_SENDER_MAIN, 0);
                AddGossipItemFor(player, GossipOptionNpc::PetitionVendor, "Can I please test the Night Fae covenant abilities again?", GOSSIP_SENDER_MAIN, 1);
                AddGossipItemFor(player, GossipOptionNpc::PetitionVendor, "Could you please reset the cooldown on my ability so I can train further?", GOSSIP_SENDER_MAIN, 2);
                AddGossipItemFor(player, GossipOptionNpc::PetitionVendor, "What adventures await me if I join your covenant?", GOSSIP_SENDER_MAIN, 3);
                SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, me->GetGUID());
            }

            return true;
        }

        bool OnGossipSelect(Player* player, uint32 sender, uint32 action) override
        {
            switch (action)
            {
            case 0: 
                player->KilledMonsterCredit(171795);
                player->GetSession()->SendCovenantPreview(player->GetGUID(), 3);  // night fae
                CloseGossipMenuFor(player);
                break;
            }
            return true;
        }

    };

    CreatureAI* GetAI(Creature* creature) const  override
    {
        return new npc_lady_moonberry_171795AI(creature);
    }
};

// npc_secutor_mevix_171821
class npc_secutor_mevix_171821 : public CreatureScript
{
public:
    npc_secutor_mevix_171821() : CreatureScript("npc_secutor_mevix_171821") { }

    struct npc_secutor_mevix_171821AI : public ScriptedAI
    {
        npc_secutor_mevix_171821AI(Creature* creature) : ScriptedAI(creature) {}

        bool OnGossipHello(Player* player) override
        {
            if (player->HasQuest(62000) || player->HasQuest(57878)) // This menu show only if u have Choosing Your Purpose quest.
            {
                AddGossipItemFor(player, GossipOptionNpc::PetitionVendor, "What are the strengths of the Necrolords?", GOSSIP_SENDER_MAIN, 0);
                AddGossipItemFor(player, GossipOptionNpc::PetitionVendor, "Can I please test the Necrolord covenant abilities again?", GOSSIP_SENDER_MAIN, 1);
                AddGossipItemFor(player, GossipOptionNpc::PetitionVendor, "Could you please reset the cooldown on my ability so I can train further?", GOSSIP_SENDER_MAIN, 2);
                AddGossipItemFor(player, GossipOptionNpc::PetitionVendor, "What adventures await me if I join your covenant?", GOSSIP_SENDER_MAIN, 3);
                SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, me->GetGUID());
            }

            return true;
        }

        bool OnGossipSelect(Player* player, uint32 sender, uint32 action) override
        {
            switch (action)
            {
            case 0: 
                player->KilledMonsterCredit(171821);
                player->GetSession()->SendCovenantPreview(player->GetGUID(), 4);  // necrolords
                CloseGossipMenuFor(player);
                break;
            }
            return true;
        }

    };

    CreatureAI* GetAI(Creature* creature) const  override
    {
        return new npc_secutor_mevix_171821AI(creature);
    }
};

//168252 Protector Captain
struct npc_protector_captain_168252 : public EscortAI
{
    npc_protector_captain_168252(Creature* creature) : EscortAI(creature) { Reset(); }

private:
    ObjectGuid m_playerGUID;
    ObjectGuid protectorGUID;
    bool say60129;

    void Reset() override
    {
        m_playerGUID.Clear();
        protectorGUID.Clear();
        say60129 = false;
    }

    bool OnGossipHello(Player* player) override
    {
        if (me->IsQuestGiver())
            player->PrepareQuestMenu(me->GetGUID());

        if (!player->GetQuestObjectiveProgress(QUEST_STRANGER_IN_AN_EVEN_STRANGERLAND, 0))
            AddGossipItemFor(player, GossipOptionNpc::None, "Where am I? Have I escaped the Maw?", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 0);

        SendGossipMenuFor(player, player->GetGossipTextId(me), me->GetGUID());

        return true;
    }

    bool OnGossipSelect(Player* player, uint32 /*sender*/, uint32 gossipListId) override
    {
        uint32 const action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);
        ClearGossipMenuFor(player);
        switch (action)
        {
        case GOSSIP_ACTION_INFO_DEF + 0:
            CloseGossipMenuFor(player);
            m_playerGUID = player->GetGUID();
            if (Creature* protector = GetClosestCreatureWithEntry(me, 172532, 20.0f))
                protectorGUID = protector->GetGUID();
            player->GetScheduler().Schedule(Milliseconds(3000), [this, player](TaskContext context)
                {
                    me->AI()->Talk(1);
                    Start(true, player->GetGUID());
                    if (Creature* protector = ObjectAccessor::GetCreature(*me, protectorGUID))
                    {
                        protector->SetAIAnimKitId(0);
                        protector->GetMotionMaster()->MoveFollow(me, 1.0f, PET_FOLLOW_ANGLE, {}, MOTION_SLOT_ACTIVE);
                    }
                });
            player->KilledMonsterCredit(168252);

            say60129 = false;
            break;
        }
        return true;
    }

    void WaypointReached(uint32 pointId, uint32 pathId) override
    {
        switch (pointId)
        {
        case 7:
            SetEscortPaused(true);
            {
                if (HasEscortState(STATE_ESCORT_PAUSED))
                {
                    if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                        if (Creature* protector = ObjectAccessor::GetCreature(*me, protectorGUID))
                        {
                            player->GetScheduler().Schedule(Milliseconds(5000), [this, player](TaskContext context)
                                {
                                    player->KilledMonsterCredit(164579);
                                    if (Creature* delen = GetClosestCreatureWithEntry(me, 167425, 20.0f))
                                        delen->AI()->Talk(0);
                                }).Schedule(Milliseconds(14000), [this](TaskContext context)
                                    {
                                        if (Creature* sher = GetClosestCreatureWithEntry(me, 167424, 20.0f))
                                            sher->AI()->Talk(0);
                                    }).Schedule(Milliseconds(20000), [this, protector](TaskContext context)
                                        {
                                            protector->AI()->Talk(1);
                                        }).Schedule(Milliseconds(29000), [this](TaskContext context)
                                            {
                                                if (Creature* delen = GetClosestCreatureWithEntry(me, 167425, 20.0f))
                                                    delen->AI()->Talk(1);
                                            }).Schedule(Milliseconds(40000), [this, protector](TaskContext context)
                                                {
                                                    if (Creature* delen = GetClosestCreatureWithEntry(me, 167425, 20.0f))
                                                    {
                                                        delen->SetNpcFlag(UNIT_NPC_FLAG_GOSSIP);
                                                        delen->RemoveNpcFlag(UNIT_NPC_FLAG_QUESTGIVER);
                                                    }
                                                    protector->DespawnOrUnsummon(5s, 30s);
                                                    me->DespawnOrUnsummon(5s, 30s);
                                                });
                        }
                    SetPauseTimer(60s);
                    SetEscortPaused(false);
                }
            }
            break;
        }
    }

    void MoveInLineOfSight(Unit* target) override
    {
        if (!target->IsPlayer())
            return;

        if (Player* player = target->ToPlayer())
            if (target->GetDistance2d(me) <= 10.0f)
                if (!player->GetQuestObjectiveProgress(QUEST_STRANGER_IN_AN_EVEN_STRANGERLAND, 0))
                    if (!say60129)
                    {
                        if (Creature* protector = me->FindNearestCreature(172532, 15.0f))
                            protector->AI()->Talk(0);
                        player->GetScheduler().Schedule(Milliseconds(7000), [this](TaskContext context)
                            {
                                me->AI()->Talk(0);
                            });
                        say60129 = true;
                    }
    }

};

//167425 overseer_kah_delen
struct npc_overseer_kah_delen_167425 : public EscortAI
{
public:
    npc_overseer_kah_delen_167425(Creature* creature) : EscortAI(creature) { Reset(); }

    EventMap _events;

private:
    ObjectGuid m_playerGUID;

    void Reset() override
    {
        _events.Reset();
        m_playerGUID.Clear();
    }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (player->ToPlayer())
            m_playerGUID = player->GetGUID();
        if (quest->GetQuestId() == QUEST_NO_PLACE_FOR_THE_LIVING)
        {
            me->RemoveNpcFlag(UNIT_NPC_FLAG_QUESTGIVER);
            me->SetNpcFlag(UNIT_NPC_FLAG_GOSSIP);
        }
    }

    bool OnGossipSelect(Player* player, uint32 MenuID, uint32 gossipListId) override
    {
        ClearGossipMenuFor(player);
        if (MenuID == 25571)
        {
            switch (gossipListId)
            {
            case 0:
                CloseGossipMenuFor(player);
                me->AI()->Talk(2);
                player->KilledMonsterCredit(167425);
                me->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
                me->SetNpcFlag(UNIT_NPC_FLAG_QUESTGIVER);
                break;

            case 1:
                CloseGossipMenuFor(player);
                player->CastSpell(player, SCENE_NO_PLACE_FOR_THE_LIVING);
                player->KilledMonsterCredit(167425);
                me->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
                me->SetNpcFlag(UNIT_NPC_FLAG_QUESTGIVER);
                break;
            }
        }
        return true;
    }

    void MoveInLineOfSight(Unit* target) override
    {
        if (!target->IsPlayer())
            return;

        if (Player* player = target->ToPlayer())
            if (target->GetDistance2d(me) <= 10.0f)
                if (player->GetQuestStatus(QUEST_STRANGER_IN_AN_EVEN_STRANGERLAND) == QUEST_STATUS_REWARDED && player->GetQuestStatus(QUEST_NO_PLACE_FOR_THE_LIVING) == QUEST_STATUS_NONE || player->GetQuestStatus(QUEST_AUDIENCE_WITH_THE_ARBITER) == QUEST_STATUS_COMPLETE || player->GetQuestStatus(QUEST_AUDIENCE_WITH_THE_ARBITER) == QUEST_STATUS_REWARDED)
                {
                    me->SetNpcFlag(UNIT_NPC_FLAG_QUESTGIVER);
                }
    }

    void UpdateAI(uint32 diff) override
    {
        EscortAI::UpdateAI(diff);

        _events.Update(diff);

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_NO_PLACE_FOR_THE_LIVING_PHASE1:
                me->AI()->Talk(3);
                me->AddDelayedEvent(16000, [this]()
                    {
                        if (Creature* inara = GetClosestCreatureWithEntry(me, 167486, 20.0f))
                        {
                            inara->AI()->Talk(0);
                        }
                    });
                me->AddDelayedEvent(30000, [this]()
                    {
                        if (Creature* sher = GetClosestCreatureWithEntry(me, 167424, 20.0f))
                        {
                            sher->AI()->Talk(1);
                        }
                    });
                me->AddDelayedEvent(40000, [this]()
                    {
                        if (Creature* inara = GetClosestCreatureWithEntry(me, 167486, 20.0f))
                        {
                            inara->AI()->Talk(1);
                        }
                    });
                me->AddDelayedEvent(54000, [this]()
                    {
                        if (Creature* sher = GetClosestCreatureWithEntry(me, 167424, 20.0f))
                        {
                            sher->AI()->Talk(2);
                        }
                    });
                me->AddDelayedEvent(58000, [this]()
                    {
                        if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                        {
                            if (Creature* inara = GetClosestCreatureWithEntry(me, 167486, 20.0f))
                            {
                                inara->DespawnOrUnsummon();
                                if (!player->GetPhaseShift().HasPhase(10062)) PhasingHandler::AddPhase(player, 10062, true);
                            }
                            player->KilledMonsterCredit(167486);
                            me->AI()->Talk(4);
                        }
                    });
                break;
            }
        }
    }

};

//173615 Tal-Inara
struct npc_tal_inara_173615 : public ScriptedAI
{
public:
    npc_tal_inara_173615(Creature* creature) : ScriptedAI(creature) { Reset(); }

    EventMap _events60149;

private:
    ObjectGuid m_playerGUID;

    void Reset() override
    {
        _events60149.Reset();
    }

    bool OnGossipHello(Player* player) override
    {
        if (me->IsQuestGiver())
            player->PrepareQuestMenu(me->GetGUID());
        if (player->HasQuest(QUEST_AUDIENCE_WITH_THE_ARBITER))
        {
            if (!player->GetQuestObjectiveProgress(QUEST_AUDIENCE_WITH_THE_ARBITER, 1))
                AddGossipItemFor(player, GossipOptionNpc::None, "What is this place?", 26362, GOSSIP_ACTION_INFO_DEF + 1);

            if (player->GetQuestObjectiveProgress(QUEST_AUDIENCE_WITH_THE_ARBITER, 1) && !player->GetQuestObjectiveProgress(QUEST_AUDIENCE_WITH_THE_ARBITER, 2))
                AddGossipItemFor(player, GossipOptionNpc::None, "I am ready to return.", 26362, GOSSIP_ACTION_INFO_DEF + 2);
        }
        if (player->HasQuest(QUEST_VOICES_OF_THE_ETERNAL))
        {
            if (!player->GetQuestObjectiveProgress(QUEST_VOICES_OF_THE_ETERNAL, 0))
            {
                ClearGossipMenuFor(player);
                SendGossipMenuFor(player, 42835, me->GetGUID());  // Show Next gossip and should not close
                AddGossipItemFor(player, GossipOptionNpc::None, "I'm ready to begin.", 26362, GOSSIP_ACTION_INFO_DEF + 3);
            }

            if (player->GetQuestObjectiveProgress(QUEST_VOICES_OF_THE_ETERNAL, 0) && !player->GetQuestObjectiveProgress(QUEST_VOICES_OF_THE_ETERNAL, 1))
            {
                ClearGossipMenuFor(player);
                SendGossipMenuFor(player, 42835, me->GetGUID());  // Show Next gossip and should not close
                AddGossipItemFor(player, GossipOptionNpc::None, "<Leave the Arbiter's Chamber.>", 26362, GOSSIP_ACTION_INFO_DEF + 4);
            }
        }
        SendGossipMenuFor(player, player->GetGossipTextId(me), me->GetGUID());

        return true;
    }

    bool OnGossipSelect(Player* player, uint32 MenuID, uint32 gossipListId) override
    {
        uint32 const action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);
        ClearGossipMenuFor(player);
        switch (action)
        {
        case GOSSIP_ACTION_INFO_DEF + 1:
            CloseGossipMenuFor(player);
            m_playerGUID = player->GetGUID();
            me->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
            _events60149.ScheduleEvent(EVENT_AUDIENCE_WITH_THE_ARBITER_PHASE1, 3s);
            break;

        case GOSSIP_ACTION_INFO_DEF + 2:
            CloseGossipMenuFor(player);
            player->ForceCompleteQuest(QUEST_AUDIENCE_WITH_THE_ARBITER);
            player->CastSpell(player, 328076);
            break;

        case GOSSIP_ACTION_INFO_DEF + 3:
            CloseGossipMenuFor(player);
            player->KilledMonsterCredit(177571);
            player->CastSpell(player, SCENE_QUEST_VOICES_OF_THE_ETERNAL);
            break;

        case GOSSIP_ACTION_INFO_DEF + 4:
            CloseGossipMenuFor(player);
            player->ForceCompleteQuest(QUEST_VOICES_OF_THE_ETERNAL);
            player->CastSpell(player, SPELL_TELE_TO_RING_OF_FALES);
            break;
        }
        return true;
    }

    void UpdateAI(uint32 diff) override
    {
        _events60149.Update(diff);

        while (uint32 eventId = _events60149.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_AUDIENCE_WITH_THE_ARBITER_PHASE1:
                Talk(0);
                me->AddDelayedEvent(6000, [this]() { Talk(1); });
                me->AddDelayedEvent(24000, [this]() { Talk(2); });
                me->AddDelayedEvent(35000, [this]()
                    {
                        if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                        {
                            me->SetNpcFlag(UNIT_NPC_FLAG_GOSSIP);
                            player->CastSpell(player, MOVIE_AUDIENCE_WITH_THE_ARBITER);
                            player->KilledMonsterCredit(167425);
                        }
                    });
                break;
            }
        }
    }
};

//159478 Tal-Inara
struct npc_tal_inara_159478 : public ScriptedAI
{
public:
    npc_tal_inara_159478(Creature* creature) : ScriptedAI(creature) {}

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_LINK_TO_THE_MAW)
        {
            Talk(6);
            player->GetScheduler().Schedule(Milliseconds(9000), [this, player](TaskContext context)
                {
                    if (Creature* bolvar = GetClosestCreatureWithEntry(player, 164079, 20.0f))
                        bolvar->AI()->Talk(6);
                });
        }
    }

    bool OnGossipHello(Player* player) override
    {
        if (me->IsQuestGiver())
            player->PrepareQuestMenu(me->GetGUID());
        if (player->GetQuestStatus(QUEST_A_GATHERING_OF_COVENANTS) == QUEST_STATUS_INCOMPLETE)
        {
            AddGossipItemFor(player, GossipOptionNpc::None, "I am ready", 26283, GOSSIP_ACTION_INFO_DEF + 0);
        }
        SendGossipMenuFor(player, player->GetGossipTextId(me), me->GetGUID());

        if (player->GetQuestStatus(QUEST_OPENING_THE_MAW) == QUEST_STATUS_INCOMPLETE) //gossip txt appear to top
        {
            AddGossipItemFor(player, GossipOptionNpc::None, "I'm prepared for you to examine my anima.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            SendGossipMenuFor(player, 42760, me->GetGUID());
            player->PrepareQuestMenu(me->GetGUID());
        }

        return true;
    }

    bool OnGossipSelect(Player* player, uint32 MenuID, uint32 gossipListId) override
    {
        uint32 const action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);
        ClearGossipMenuFor(player);
        switch (action)
        {
        case GOSSIP_ACTION_INFO_DEF + 0:
            CloseGossipMenuFor(player);
            player->ForceCompleteQuest(QUEST_A_GATHERING_OF_COVENANTS);
            player->CastSpell(player, SPELL_TELE_TO_ARBITER_ROOM);
            break;

        case GOSSIP_ACTION_INFO_DEF + 1:
            CloseGossipMenuFor(player);
            me->SetFacingToObject(player);
            me->CastSpell(me, SPELL_CHAINELING, false);
            player->AddAura(SPELL_ANIMA_EXTRAPOLATION, player);
            Talk(2);
            player->GetScheduler().Schedule(Milliseconds(12000), [this](TaskContext context)
                {
                    Talk(3);
                });
            player->GetScheduler().Schedule(Milliseconds(21000), [this](TaskContext context)
                {
                    Talk(4);
                });
            player->GetScheduler().Schedule(Milliseconds(33000), [this, player](TaskContext context)
                {
                    Talk(5);
                    player->KilledMonsterCredit(179107);
                });
            player->GetScheduler().Schedule(Milliseconds(40000), [this](TaskContext context)
                {
                    me->RemoveAura(SPELL_CHAINELING);
                });
            break;
        }
        return true;
    }

};

//170153 Lady Jaina Proudmoore
struct npc_jaina_proudmoore_170153 : public ScriptedAI
{
public:
    npc_jaina_proudmoore_170153(Creature* creature) : ScriptedAI(creature) { Reset(); }

    bool OnGossipHello(Player* player) override
    {
        if (me->IsQuestGiver())
            player->PrepareQuestMenu(me->GetGUID());

        AddGossipItemFor(player, GossipOptionNpc::None, "How did you escape the Maw?", 25899, GOSSIP_ACTION_INFO_DEF + 0);

        SendGossipMenuFor(player, player->GetGossipTextId(me), me->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, uint32 MenuID, uint32 gossipListId) override
    {
        if (gossipListId == 0)
        {
            player->PrepareQuestMenu(me->GetGUID());
            ClearGossipMenuFor(player);
            SendGossipMenuFor(player, 41010, me->GetGUID());  // Show Next gossip and should not close
        }
        return true;
    }

};

//171128 Thrall
struct npc_thralle_171128 : public ScriptedAI
{
public:
    npc_thralle_171128(Creature* creature) : ScriptedAI(creature) { Reset(); }

    bool OnGossipHello(Player* player) override
    {
        player->PrepareQuestMenu(me->GetGUID());
        AddGossipItemFor(player, GossipOptionNpc::None, "How did you escape the Maw?", 26543, GOSSIP_ACTION_INFO_DEF + 0);
        SendGossipMenuFor(player, player->GetGossipTextId(me), me->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, uint32 MenuID, uint32 gossipListId) override
    {
        if (gossipListId == 0)
        {
            player->PrepareQuestMenu(me->GetGUID());
            ClearGossipMenuFor(player);
            SendGossipMenuFor(player, 42138, me->GetGUID());  // Show Next gossip and should not close
        }
        return true;
    }

};

//164079 Highlord Bolvar Fordragon
struct npc_highlord_bolvar_fordragon_164079 : public ScriptedAI
{
public:
    npc_highlord_bolvar_fordragon_164079(Creature* creature) : ScriptedAI(creature) { Initialize(); }

private:
    bool say60152;
    bool say63639;
    EventMap m_events;
    TaskScheduler _scheduler;

    void Initialize()
    {
        m_events.Reset();
    }

    void Reset() override
    {
        say60152 = false;
        say63639 = false;
    }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_A_DOORWAY_THROUGH_THE_VEIL)
        {
            if (Creature* sher = GetClosestCreatureWithEntry(player, 167424, 20.0f))
                sher->AI()->Talk(3);
            player->GetScheduler().Schedule(Milliseconds(13000), [this, player](TaskContext context)
                {
                    if (Creature* delen = GetClosestCreatureWithEntry(player, 167425, 20.0f))
                        delen->AI()->Talk(5);
                }).Schedule(Milliseconds(25000), [this, player](TaskContext context)
                    {
                        me->AI()->Talk(0, player);
                    });
        }
    }


    void OnQuestReward(Player* player, Quest const* quest, LootItemType type, uint32 /*opt*/) override
    {
        if (quest->GetQuestId() == QUEST_THE_FIRST_MOVE)
        {
            if (Creature* tal = me->FindNearestCreature(159478, 20.0f))
                if (tal->GetSpawnId() == 346028)
                {
                    player->GetScheduler().Schedule(Milliseconds(3000), [tal](TaskContext context)
                        {
                            tal->AI()->Talk(0);
                        });
                }
        }
    }

    void MoveInLineOfSight(Unit* target) override
    {
        if (!target->IsPlayer())
            return;

        if (Player* player = target->ToPlayer())
            if (target->GetDistance2d(me) <= 15.0f)
            {
                if (player->GetQuestStatus(QUEST_THE_ETERNAL_CITY) == QUEST_STATUS_INCOMPLETE
                    && !player->GetQuestObjectiveProgress(QUEST_THE_ETERNAL_CITY, 4)
                    && player->GetQuestObjectiveProgress(QUEST_THE_ETERNAL_CITY, 5))
                {
                    if (!say60152)
                    {
                        me->AI()->Talk(1);
                        player->GetScheduler().Schedule(Milliseconds(13000), [this, player](TaskContext context)
                            {
                                player->KilledMonsterCredit(167424);
                                if (Creature* inara = GetClosestCreatureWithEntry(player, 167486, 20.0f))
                                    inara->AI()->Talk(3);
                            });
                        say60152 = true;
                    }
                }
                if (player->GetQuestStatus(QUEST_REPORT_TO_ORIBOS) == QUEST_STATUS_COMPLETE)
                {
                    if (!say63639)
                    {
                        me->AI()->Talk(5);
                        player->GetScheduler().Schedule(Milliseconds(8000), [this, player](TaskContext context)
                            {
                                player->KilledMonsterCredit(167424);
                                if (Creature* inara = GetClosestCreatureWithEntry(player, 159478, 20.0f))
                                    inara->AI()->Talk(1);
                            });
                        say63639 = true;
                    }
                }
            }

    }

    void DoAction(int32 action) override
    {
        if (action == ACTION_BATTLE_OF_ARDENWEALD_TALK_START)
        {
            m_events.ScheduleEvent(EVENT_BATTLE_OF_ARDENWEALD_TALK_START, 2s);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        _scheduler.Update(diff);

        m_events.Update(diff);

        while (uint32 eventId = m_events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_BATTLE_OF_ARDENWEALD_TALK_START:
            {
                Talk(3);
                _scheduler.Schedule(Milliseconds(10000), [this](TaskContext context)
                    {
                        if (Creature* darion = me->FindNearestCreature(170640, 30.0f))
                            darion->AI()->Talk(0);
                    }).Schedule(Milliseconds(13000), [this](TaskContext context)
                        {
                            if (Creature* thral = me->FindNearestCreature(171128, 30.0f))
                                thral->AI()->Talk(0);
                        }).Schedule(Milliseconds(20000), [this](TaskContext context)
                            {
                                me->GetMotionMaster()->MovePath(16407901, false);
                                if (Creature* jaina = me->FindNearestCreature(170153, 30.0f))
                                {
                                    jaina->AI()->Talk(0);
                                    jaina->GetMotionMaster()->MovePath(16407901, false);
                                    jaina->DespawnOrUnsummon(30s, 60s);
                                }
                            }).Schedule(Milliseconds(28000), [this](TaskContext context)
                                {
                                    Talk(4);
                                });
                            break;
            }
            }
        }
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        switch (type)
        {
        case WAYPOINT_MOTION_TYPE:
        {

            if (id == 7)
            {
                me->DespawnOrUnsummon(3s, 60s);
                break;
            }
        }
        }
    }
};

//172378 Ebon Blade Acolyte
struct npc_ebon_blade_acolyte_172378 : public EscortAI
{
public:
    npc_ebon_blade_acolyte_172378(Creature* creature) : EscortAI(creature) { Reset(); }

private:
    ObjectGuid acolyteGUID;
    ObjectGuid m_playerGUID;

    void Reset() override
    {
        acolyteGUID.Clear();
        m_playerGUID.Clear();
    }

    bool OnGossipSelect(Player* player, uint32 MenuID, uint32 gossipListId) override
    {
        ClearGossipMenuFor(player);
        if (MenuID == 26160)
        {
            switch (gossipListId)
            {
            case 0:
                CloseGossipMenuFor(player);
                player->KilledMonsterCredit(172378);
                m_playerGUID = player->GetGUID();
                me->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
                if (Creature* acolyte = GetClosestCreatureWithEntry(player, 172377, 20.0f))
                {
                    if (!acolyte->GetPhaseShift().HasPhase(10060)) PhasingHandler::AddPhase(acolyte, 10060, true);
                    if (!me->GetPhaseShift().HasPhase(10060)) PhasingHandler::AddPhase(me, 10060, true);
                    acolyteGUID = acolyte->GetGUID();
                    acolyte->GetMotionMaster()->MoveFollow(me, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE, {}, MOTION_SLOT_ACTIVE);
                }
                LoadPath(me->GetEntry());
                Start(true);
                break;
            }
        }
        return true;
    }

    void WaypointReached(uint32 pointId, uint32 pathId) override
    {
        switch (pointId)
        {
        case 10:
            if (Creature* acolyte = ObjectAccessor::GetCreature(*me, acolyteGUID))
            {
                acolyte->GetMotionMaster()->MovePoint(0, -1832.54f, 1531.1f, 5274.21f, false, 4.44766f);
            }
        case 11:
            if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                if (Creature* acolyte = ObjectAccessor::GetCreature(*me, acolyteGUID))
                {
                    if (!player->GetPhaseShift().HasPhase(10063)) PhasingHandler::AddPhase(player, 10063, true);
                    player->KilledMonsterCredit(172377);
                    me->DespawnOrUnsummon(1s, 30s);
                    acolyte->DespawnOrUnsummon(1s, 30s);
                }
            break;
        }
    }
};

//167682 Ebon Blade Acolyte
struct npc_ebon_blade_acolyte_167682 : public EscortAI
{
public:
    npc_ebon_blade_acolyte_167682(Creature* creature) : EscortAI(creature) { Reset(); }

private:
    ObjectGuid acolyteGUID;
    ObjectGuid m_playerGUID;

    void Reset() override
    {
        acolyteGUID.Clear();
        m_playerGUID.Clear();
    }

    bool OnGossipSelect(Player* player, uint32 MenuID, uint32 gossipListId) override
    {
        ClearGossipMenuFor(player);
        if (MenuID == 25607)
        {
            switch (gossipListId)
            {
            case 0:
                CloseGossipMenuFor(player);
                player->KilledMonsterCredit(167682);
                m_playerGUID = player->GetGUID();
                me->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
                me->SetWalk(true);
                me->GetMotionMaster()->MovePoint(0, -1856.61f, 1542.17f, 5274.51f, false, 4.44766f);
                if (Creature* acolyte = GetClosestCreatureWithEntry(player, 167683, 20.0f))
                {
                    acolyteGUID = acolyte->GetGUID();
                    acolyte->SetWalk(true);
                    acolyte->GetMotionMaster()->MovePoint(0, -1812.32f, 1542.5f, 5274.51f, false, 5.60795f);
                    acolyte->AI()->Talk(0);
                }
                me->AddDelayedEvent(9000, [this]()
                    {
                        me->AI()->Talk(0);
                    });
                me->AddDelayedEvent(12000, [this]()
                    {
                        if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                            if (Creature* acolyte = ObjectAccessor::GetCreature(*me, acolyteGUID))
                            {
                                player->SummonGameObject(239332, -163.918f, 6917.27f, 12.6521f, 5.33958f, QuaternionData(), 0s);
                                me->CastSpell(me, 339689);
                                acolyte->CastSpell(acolyte, 339689);
                                player->ForceCompleteQuest(QUEST_A_DOORWAY_THROUGH_THE_VEIL);
                            }
                    });
                me->AddDelayedEvent(14000, [this]()
                    {
                        if (Creature* acolyte = ObjectAccessor::GetCreature(*me, acolyteGUID))
                            acolyte->SummonGameObject(353823, -1809.13f, 1537.99f, 5274.52f, 3.36304f, QuaternionData(0, 0, -0.993876f, 0.110497f), 0s);
                        me->SummonGameObject(353822, -1858.72f, 1538.27f, 5274.75f, 0.896872f, QuaternionData(0, 0, 0.433557f, 0.901126f), 0s);
                    });
                break;
            }
        }
        return true;
    }
};

//175829 Overseer Kah-Sher_q60156
struct npc_overseer_kah_sher_175829 : public EscortAI
{
public:
    npc_overseer_kah_sher_175829(Creature* creature) : EscortAI(creature) { Reset(); }

private:
    ObjectGuid m_playerGUID;

    void Reset() override
    {
        m_playerGUID.Clear();
    }
    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_THE_PATH_TO_BASTION)
        {
            m_playerGUID = player->GetGUID();
            me->AI()->Talk(0);
        }
        player->GetScheduler().Schedule(Milliseconds(10000), [this, player](TaskContext context)
            {
                LoadPath(me->GetEntry());
                Start(true);
            });

    }

    void WaypointReached(uint32 pointId, uint32 pathId) override
    {
        switch (pointId)
        {
        case 3:
            me->AI()->Talk(1);
            break;
        case 6:
            SetEscortPaused(true);
            {
                if (HasEscortState(STATE_ESCORT_PAUSED))
                {
                    if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                    {
                        if (Creature* avonavi = GetClosestCreatureWithEntry(player, 175133, 30.0f))
                            avonavi->AI()->Talk(0);
                        player->KilledMonsterCredit(166307);
                        if (!player->GetPhaseShift().HasPhase(10062)) PhasingHandler::AddPhase(player, 10062, true);
                        me->DespawnOrUnsummon(1s, 30s);
                    }
                    SetPauseTimer(2s);
                    SetEscortPaused(false);
                }
            }
            break;
        }
    }
};

// 344628 gate_bastion
class spell_open_gate_bastion : public SpellScriptLoader
{
public:
    spell_open_gate_bastion() : SpellScriptLoader("spell_open_gate_bastion") {}

    class spell_open_gate_bastion_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_open_gate_bastion_SpellScript);

        void HandleAfterCast()
        {
            if (Player* player = GetCaster()->ToPlayer())
            {
                if (player->GetQuestStatus(QUEST_THE_PATH_TO_BASTION) == QUEST_STATUS_INCOMPLETE && player->GetQuestObjectiveProgress(QUEST_THE_PATH_TO_BASTION, 0))
                {
                    player->KilledMonsterCredit(175133);
                    player->CastSpell(player, SCENE_QUEST_GATE_BASTION);
                    return;
                }
            }
        }

        void Register() override
        {
            AfterCast += SpellCastFn(spell_open_gate_bastion_SpellScript::HandleAfterCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_open_gate_bastion_SpellScript();
    }
};


//175132 pathscriberoh
class npc_pathscribe_roh_175132 : public CreatureScript
{
public:
    npc_pathscribe_roh_175132() : CreatureScript("npc_pathscribe_roh_175132") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_pathscribe_roh_175132AI(creature);
    }

    struct npc_pathscribe_roh_175132AI : public ScriptedAI
    {
        npc_pathscribe_roh_175132AI(Creature* me) : ScriptedAI(me) { }

        bool OnGossipHello(Player* player) override
        {
            if (me->IsQuestGiver())
                player->PrepareQuestMenu(me->GetGUID());
            if (player->HasQuest(QUEST_IF_U_WANT_PEACE))
            {
                AddGossipItemFor(player, GossipOptionNpc::None, "Im ready to go to Maldraxxus.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 0);
            }
            SendGossipMenuFor(player, player->GetGossipTextId(me), me->GetGUID());

            return true;
        }

        bool OnGossipSelect(Player* player, uint32 MenuID, uint32 gossipListId) override
        {
            uint32 const action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);
            ClearGossipMenuFor(player);
            if (action == GOSSIP_ACTION_INFO_DEF + 0)//Maldraxxus
            {
                player->ForceCompleteQuest(QUEST_IF_U_WANT_PEACE);
                player->CastSpell(player, SPELL_FLY_TO_MALDRAXXUS);
            }

            return true;
        }
    };
};

////175133 pathscriberoh
class npc_pathscribe_roh_175133 : public CreatureScript
{
public:
    npc_pathscribe_roh_175133() : CreatureScript("npc_pathscribe_roh_175133") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_pathscribe_roh_175133AI(creature);
    }

    struct npc_pathscribe_roh_175133AI : public ScriptedAI
    {
        npc_pathscribe_roh_175133AI(Creature* creature) : ScriptedAI(creature) { }

        bool OnGossipHello(Player* player) override
        {
            if (me->IsQuestGiver())
                player->PrepareQuestMenu(me->GetGUID());
            if (player->HasQuest(QUEST_SEEK_THE_ASCENDED))
            {
                AddGossipItemFor(player, GossipOptionNpc::None, "Im ready to go to Bastion.", 26679, GOSSIP_ACTION_INFO_DEF + 0);
            }
            SendGossipMenuFor(player, player->GetGossipTextId(me), me->GetGUID());

            return true;
        }

        bool OnGossipSelect(Player* player, uint32 sender, uint32 gossipListId) override
        {
            uint32 const action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);
            ClearGossipMenuFor(player);
            if (action == GOSSIP_ACTION_INFO_DEF + 0)//Bastion
            {
                CloseGossipMenuFor(player);
                player->ForceCompleteQuest(QUEST_SEEK_THE_ASCENDED);
                player->CastSpell(player, SPELL_FLY_TO_BASTION);
            }
            return true;
        }
    };
};

//177136 polemarch_adrestes
class npc_polemarch_adrestes_177136 : public CreatureScript
{
public:
    npc_polemarch_adrestes_177136() : CreatureScript("npc_polemarch_adrestes_177136") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_polemarch_adrestes_177136AI(creature);
    }

    struct npc_polemarch_adrestes_177136AI : public ScriptedAI
    {
        npc_polemarch_adrestes_177136AI(Creature* creature) : ScriptedAI(creature) { }

        bool OnGossipHello(Player* player) override
        {
            if (me->IsQuestGiver())
                player->PrepareQuestMenu(me->GetGUID());
            if (player->HasQuest(QUEST_THE_FIRST_MOVE))
            {
                AddGossipItemFor(player, GossipOptionNpc::None, "<Ask Adrestes what happened in Elysian Hold.>", 26131, GOSSIP_ACTION_INFO_DEF + 0);
            }
            SendGossipMenuFor(player, player->GetGossipTextId(me), me->GetGUID());

            return true;
        }

        bool OnGossipSelect(Player* player, uint32 MenuID, uint32 gossipListId) override
        {
            uint32 const action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);
            ClearGossipMenuFor(player);
            if (action == GOSSIP_ACTION_INFO_DEF + 0)//Maldraxxus
            {

                if (player->HasQuest(QUEST_THE_FIRST_MOVE))
                {
                    player->ForceCompleteQuest(QUEST_THE_FIRST_MOVE);
                    player->CastSpell(player, MOVIE_KINGSMOURNE);
                }

            }

            return true;
        }
    };
};

//177141 Lady Moonberry
struct npc_lady_moonberry_177141 : public  EscortAI
{
public:
    npc_lady_moonberry_177141(Creature* creature) : EscortAI(creature) {}

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_THE_BATTLE_OF_ARDENWEALD)
        {
            if (Creature* bolvar = me->FindNearestCreature(164079, 30.0f))
                bolvar->AI()->DoAction(ACTION_BATTLE_OF_ARDENWEALD_TALK_START);
            if (Creature* mistblade = me->FindNearestCreature(177205, 20.0f))
            {
                mistblade->GetMotionMaster()->MoveFollow(me, 0.5f, me->GetFollowAngle());
                mistblade->DespawnOrUnsummon(20s, 60s);
            }
            if (Creature* draven = me->FindNearestCreature(177250, 20.0f))
            {
                draven->GetMotionMaster()->MoveFollow(me, 2.0f, me->GetFollowAngle());
                draven->DespawnOrUnsummon(20s, 60s);
            }
            me->GetMotionMaster()->MovePath(17714101, false);
        }
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        switch (type)
        {
        case WAYPOINT_MOTION_TYPE:
        {
            if (id == 2)
            {
                if (Creature* mevix = me->FindNearestCreature(177126, 100.0f))
                {
                    mevix->GetMotionMaster()->MoveFollow(me, 3.0f, me->GetFollowAngle());
                    mevix->DespawnOrUnsummon(20s, 60s);
                }
                if (Creature* plague = me->FindNearestCreature(178414, 100.0f))
                {
                    plague->GetMotionMaster()->MoveFollow(me, 3.0f, me->GetFollowAngle());
                    plague->DespawnOrUnsummon(20s, 60s);
                }
                if (Creature* kleia = me->FindNearestCreature(177123, 100.0f))
                {
                    kleia->GetMotionMaster()->MoveFollow(me, 3.0f, me->GetFollowAngle());
                    kleia->DespawnOrUnsummon(20s, 60s);
                }
                break;
            }
            if (id == 9)
            {
                me->DespawnOrUnsummon(3s, 60s);
                break;
            }
        }
        }
    }

    bool OnGossipHello(Player* player) override
    {
        if (me->IsQuestGiver())
            player->PrepareQuestMenu(me->GetGUID());
        if (player->GetQuestStatus(QUEST_THE_BATTLE_OF_ARDENWEALD) == QUEST_STATUS_COMPLETE)
        {
            AddGossipItemFor(player, GossipOptionNpc::None, "What's happening?", 12609, GOSSIP_ACTION_INFO_DEF + 0);
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
            player->PrepareQuestMenu(me->GetGUID());
            ClearGossipMenuFor(player);
            SendGossipMenuFor(player, 42995, me->GetGUID());  // write npc_text ID
        }
        return true;
    }

};

// 178292 Anima Wyrm
struct npc_anima_wyrm_178292 : public ScriptedAI
{
public:
    npc_anima_wyrm_178292(Creature* creature) : ScriptedAI(creature) { }

    void OnSpellClick(Unit* clicker, bool /*spellClickHandled*/) override
    {
        if (Player* player = clicker->ToPlayer())
        {
            if (player->HasQuest(QUEST_THE_BATTLE_OF_ARDENWEALD))
            {
                player->KilledMonsterCredit(178291);
                player->SummonCreature(178291, player->GetPosition(), TEMPSUMMON_MANUAL_DESPAWN);
            }
        }
    }
};

// 170256 Go to Maw
struct npc_go_to_maw_170256 : public ScriptedAI
{
public:
    npc_go_to_maw_170256(Creature* creature) : ScriptedAI(creature) { }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
        {
            if (player->IsInDist(me, 18.0f))
            {
                if (player->GetQuestStatus(QUEST_LINK_TO_THE_MAW) == QUEST_STATUS_INCOMPLETE)
                {
                    player->CastSpell(player, SPELL_MAW_JUMP_TO_COCYRUS, false);
                }
                if (player->GetQuestStatus(QUEST_INTO_THE_VAULT) == QUEST_STATUS_REWARDED)
                {

                    player->CastSpell(player, 328941, false);
                    player->GetScheduler().Schedule(Milliseconds(2000), [player](TaskContext context)
                        {
                            player->TeleportTo(2222, 4602.91f, 6802.31f, 4882.83f, 4.30136f);
                        });
                }
            }
        }
    }
};

// 174917 Attendant Protector
Position const  SpawnAttendantProtectorGroup1[4] =
{
    {-1758.32f, 1344.44f, 5450.93f, 1.64084f },
    {-1753.55f, 1344.79f, 5450.93f, 1.55445f },
    {-1772.71f, 1360.91f, 5450.93f, 6.08618f },
    {-1772.11f, 1365.64f, 5450.93f, 6.19221f },
};

static const Position runCovenantPosition = { -1715.88f, 1401.94f, 5450.73f, 0.6479f };

Position const  talinaraPath[2] =
{
    {-1780.06f, 1330.73f, 5445.86f, 3.57643f },
    {-1787.78f, 1293.01f, 5446.91f, 4.41681f },
};
//177230 Highlord Bolvar Fordragon
struct npc_highlord_bolvar_fordragon_177230 : public ScriptedAI
{
public:
    npc_highlord_bolvar_fordragon_177230(Creature* creature) : ScriptedAI(creature) { }
private:
    ObjectGuid tal_inaraGUID;

    void Reset() override
    {
        tal_inaraGUID = ObjectGuid::Empty;
    }

    void IsSummonedBy(WorldObject* summoner) override
    {
        if (Player* player = summoner->ToPlayer())
        {
            if (Creature* talinara = me->SummonCreature(NPC_TAL_INARA, -1769.19f, 1344.77f, 5450.93f, 0.862366f, TEMPSUMMON_MANUAL_DESPAWN))
            {
                tal_inaraGUID = talinara->GetGUID();
                me->AddDelayedEvent(4000, [this, talinara]()
                    {
                        talinara->AI()->Talk(0);
                    });
                me->AddDelayedEvent(11000, [this]()
                    {
                        Talk(0);
                    });
            }
            SummonMyMember();
        }
    }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_CHARGE_OF_THE_COVENANTS)
            player->AddAura(SPELL_RALLY_THE_TROOPS, player);
    }

    void DoAction(int32 action) override
    {
        if (action == ACTION_CHARGE_OF_THE_COVENANTS_DILOG)
        {
            me->AddDelayedEvent(8000, [this]()
                {
                    me->SetWalk(true);
                    me->GetMotionMaster()->MovePoint(0, -1766.33f, 1350.83f, 5450.87f, true, 3.8293f);
                    if (Creature* kelia = me->FindNearestCreature(NPC_KELIA, 20.0f, true))
                        if (Creature* heirmir = me->FindNearestCreature(NPC_BONESMITH_HEIRMIR, 20.0f, true))
                            if (Creature* draka = me->FindNearestCreature(NPC_BARONESS_DRAKA, 20.0f, true))
                            {
                                kelia->ForcedDespawn(6000);
                                kelia->SetWalk(false);
                                kelia->GetMotionMaster()->MovePoint(0, runCovenantPosition, true);
                                heirmir->ForcedDespawn(6000);
                                heirmir->SetWalk(false);
                                heirmir->GetMotionMaster()->MovePoint(0, runCovenantPosition, true);
                                draka->ForcedDespawn(6000);
                                draka->SetWalk(false);
                                draka->GetMotionMaster()->MovePoint(0, runCovenantPosition, true);

                            }
                    std::list<Creature*> cList = me->FindNearestCreatures(NPC_POLEMARCH_ADRESTES, 20.0f);
                    for (std::list<Creature*>::const_iterator itr = cList.begin(); itr != cList.end(); ++itr)
                    {
                        if (Creature* adrestes = *itr)
                        {
                            adrestes->ForcedDespawn(6000);
                            adrestes->SetWalk(false);
                            adrestes->GetMotionMaster()->MovePoint(0, runCovenantPosition, true);
                        }
                    }
                });
            me->AddDelayedEvent(10000, [this]()
                {
                    if (Creature* talinara = ObjectAccessor::GetCreature(*me, tal_inaraGUID))
                        talinara->AI()->Talk(1);
                });
            me->AddDelayedEvent(16000, [this]()
                {
                    Talk(1);
                });
            me->AddDelayedEvent(20000, [this]()
                {
                    if (Creature* talinara = ObjectAccessor::GetCreature(*me, tal_inaraGUID))
                    {
                        talinara->GetMotionMaster()->MoveSmoothPath(1000, talinaraPath, 2, true, false);
                        talinara->ForcedDespawn(20000);
                    }
                    me->GetMotionMaster()->MovePoint(0, runCovenantPosition, true);
                    me->ForcedDespawn(6000);
                });
        }
    }

    void SummonMyMember()
    {
        for (uint32 i = 0; i < 4; ++i)
            if (Creature* AttendantProtector1 = DoSummon(174917, SpawnAttendantProtectorGroup1[i], 600s, TEMPSUMMON_TIMED_DESPAWN))
                AttendantProtector1->SetReactState(REACT_DEFENSIVE);
        me->SummonCreature(NPC_KELIA, -1772.41f, 1348.15f, 5450.93f, 0.725871f, TEMPSUMMON_TIMED_DESPAWN, 600s);
        me->SummonCreature(NPC_BONESMITH_HEIRMIR, -1772.76f, 1351.73f, 5450.93f, 0.580571f, TEMPSUMMON_TIMED_DESPAWN, 600s);
        me->SummonCreature(NPC_BARONESS_DRAKA, -1765.82f, 1344.64f, 5450.93f, 1.13427f, TEMPSUMMON_TIMED_DESPAWN, 600s);
        me->SummonCreature(NPC_POLEMARCH_ADRESTES, -1762.05f, 1344.69f, 5450.93f, 1.02038f, TEMPSUMMON_TIMED_DESPAWN, 600s);
        me->SummonCreature(NPC_POLEMARCH_ADRESTES, -1772.54f, 1356.32f, 5450.93f, 0.419547f, TEMPSUMMON_TIMED_DESPAWN, 600s);
    }
};

// 353260 Ralling the Troops
struct spell_Ralling_the_Troops : public SpellScript
{
    PrepareSpellScript(spell_Ralling_the_Troops);

    void HandleAfterCast()
    {
        if (!GetCaster())
            return;

        if (Player* player = GetCaster()->ToPlayer())
        {
            player->RemoveAura(SPELL_RALLY_THE_TROOPS);
            if (Creature* bolvar = player->FindNearestCreature(177230, 150.0f, true))
                bolvar->AI()->DoAction(ACTION_CHARGE_OF_THE_COVENANTS_DILOG);
            player->GetScheduler().Schedule(Milliseconds(26000), [player](TaskContext context)
                {
                    player->RemoveAura(SPELL_CROWD);
                });
        }
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_Ralling_the_Troops::HandleAfterCast);
    }
};

// 170256 Kill Credit
struct npc_go_to_korthia_179048 : public ScriptedAI
{
public:
    npc_go_to_korthia_179048(Creature* creature) : ScriptedAI(creature) { }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
        {
            if (player->IsInDist(me, 12.0f))
                if (player->GetQuestStatus(QUEST_CHARGE_OF_THE_COVENANTS) == QUEST_STATUS_COMPLETE)
                    player->TeleportTo(2222, 3266.06f, 5761.53f, 4939.06f, 1.9421f);
        }
    }
};


// 299119 Flight to Bastion
struct spell_flight_to_bastion_299119 : public SpellScript
{
    PrepareSpellScript(spell_flight_to_bastion_299119);

    void HandleAfterCast()
    {
        if (!GetCaster())
            return;

        if (Player* player = GetCaster()->ToPlayer())
        {
            player->GetScheduler().Schedule(Milliseconds(46000), [player](TaskContext context)
                {
                    player->CastSpell(player, SPELL_IN_BETWEEN_TUNNDEL, true);
                });
        }
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_flight_to_bastion_299119::HandleAfterCast);
    }
};

struct IntoTheMaw : public PlayerScript
{
public:
    IntoTheMaw() : PlayerScript("IntoTheMaw") { }

private:
    bool IntoMaw;

    void OnUpdate(Player* player, uint32 diff) override
    {
        if (!player->HasQuest(QUEST_INTO_THE_MAW) || player->GetLevel() != 60)
            return;

        IntoMaw = false;

        if (player->IsFalling() && player->GetPositionZ() <= 5416.469f && player->GetAreaId() == 13499 && !IntoMaw)
        {
            IntoMaw = true;
            player->GetSceneMgr().PlaySceneByPackageId(SCENE_ORIBOS_JUMP_INTO_MAW_PRK, SceneFlag::None);
        }
    }

    void OnMovieComplete(Player* player, uint32 movieId) override
    {
        if (movieId == 945) player->KilledMonsterCredit(175541);
    }

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

    void OnUpdateArea(Player* player, uint32 /*newArea*/, uint32 /*oldArea*/) override
    {
        OnCheckPhase(player);

        if (player->GetAreaId() == 10565) //Oribos
        {
            if (!player->HasQuest(QUEST_STRANGER_IN_AN_EVEN_STRANGERLAND))
            {
                if (const Quest* quest = sObjectMgr->GetQuestTemplate(QUEST_STRANGER_IN_AN_EVEN_STRANGERLAND))
                    if (player->GetQuestStatus(QUEST_STRANGER_IN_AN_EVEN_STRANGERLAND) == QUEST_STATUS_NONE)
                    {
                        player->AddQuest(quest, nullptr);
                    }
                player->GetScheduler().Schedule(Milliseconds(2000), [this, player](TaskContext context)
                    {
                        if (!player->GetPhaseShift().HasPhase(10060)) PhasingHandler::AddPhase(player, 10060, true);
                    });
            }
            if (player->GetQuestStatus(QUEST_STRANGER_IN_AN_EVEN_STRANGERLAND) == QUEST_STATUS_INCOMPLETE)
            {
                player->GetScheduler().Schedule(Milliseconds(3000), [this, player](TaskContext context)
                    {
                        if (!player->GetPhaseShift().HasPhase(10061)) PhasingHandler::AddPhase(player, 10061, true);
                    });
            }
            if (player->GetQuestStatus(QUEST_STRANGER_IN_AN_EVEN_STRANGERLAND) == QUEST_STATUS_REWARDED)
            {
                if (player->GetPhaseShift().HasPhase(10061)) PhasingHandler::RemovePhase(player, 10061, true);
            }
            if (player->GetQuestStatus(QUEST_A_DOORWAY_THROUGH_THE_VEIL) == QUEST_STATUS_REWARDED)
            {
                if (player->GetPhaseShift().HasPhase(10063)) PhasingHandler::RemovePhase(player, 10063, true);
                if (!player->GetPhaseShift().HasPhase(10064)) PhasingHandler::AddPhase(player, 10064, true);
            }
            if (player->GetQuestStatus(QUEST_THE_FIRST_MOVE) == QUEST_STATUS_REWARDED)
            {
                if (!player->GetPhaseShift().HasPhase(10069)) PhasingHandler::AddPhase(player, 10069, true);
                if (!player->GetPhaseShift().HasPhase(10070)) PhasingHandler::AddPhase(player, 10070, true);
            }
            if (!player->GetPhaseShift().HasPhase(10060)) PhasingHandler::AddPhase(player, 10060, true);
        }

        if (player->GetAreaId() == 13498) //Ring of Transference
        {
            if (player->HasQuest(QUEST_UNDERSTANDING_THE_SHADOWLANDS)
                || player->GetQuestStatus(QUEST_UNDERSTANDING_THE_SHADOWLANDS) == QUEST_STATUS_REWARDED)
            {
                player->GetScheduler().Schedule(Milliseconds(2000), [this, player](TaskContext context)
                    {
                        if (!player->GetPhaseShift().HasPhase(10060)) PhasingHandler::AddPhase(player, 10060, true);
                        if (!player->GetPhaseShift().HasPhase(10061)) PhasingHandler::AddPhase(player, 10061, true);
                    });
            }
            if (player->GetQuestStatus(QUEST_THE_PATH_TO_BASTION) == QUEST_STATUS_REWARDED)
            {
                if (player->GetPhaseShift().HasPhase(10061)) PhasingHandler::RemovePhase(player, 10061, true);
                if (player->GetPhaseShift().HasPhase(10062)) PhasingHandler::RemovePhase(player, 10062, true);
                if (!player->GetPhaseShift().HasPhase(10063)) PhasingHandler::AddPhase(player, 10063, true);
            }
            if (player->HasQuest(QUEST_REPORT_TO_ORIBOS) || player->GetQuestStatus(QUEST_REPORT_TO_ORIBOS) == QUEST_STATUS_REWARDED)
            {
                player->GetScheduler().Schedule(Milliseconds(2000), [this, player](TaskContext context)
                    {
                        if (!player->GetPhaseShift().HasPhase(10060)) PhasingHandler::AddPhase(player, 10060, true);
                        if (!player->GetPhaseShift().HasPhase(10069)) PhasingHandler::AddPhase(player, 10069, true);
                    });
            }

            if (player->GetQuestStatus(QUEST_OPENING_TO_ORIBOS) == QUEST_STATUS_COMPLETE)
                if (!player->FindNearestCreature(177230, 150.0f, true))
                {
                    player->CastSpell(player, SPELL_CROWD, false);
                    player->SummonCreature(NPC_BOLVAR, -1771.88f, 1345.46f, 5450.93f, 0.823097f, TEMPSUMMON_MANUAL_DESPAWN);
                }
        }

        if (player->GetAreaId() == 13437) //Ve'nari's Refuge
            if (player->GetQuestStatus(QUEST_INTO_THE_MAW) == QUEST_STATUS_INCOMPLETE)
            {
                player->ForceCompleteQuest(QUEST_INTO_THE_MAW);
                player->GetSceneMgr().PlaySceneByPackageId(SCENE_ORIBOS_TO_THE_MAW_SKYJUMP, SceneFlag::None);
            }
    }


    void OnCheckPhase(Player* player)
    {
        if (player->GetAreaId() == 13499) //Ring of Fates
        {
            if (player->HasQuest(QUEST_STRANGER_IN_AN_EVEN_STRANGERLAND) || player->GetQuestStatus(QUEST_STRANGER_IN_AN_EVEN_STRANGERLAND) == QUEST_STATUS_REWARDED
                || player->GetQuestStatus(QUEST_NO_PLACE_FOR_THE_LIVING) == QUEST_STATUS_INCOMPLETE)
            {
                if (!player->GetPhaseShift().HasPhase(10061)) PhasingHandler::AddPhase(player, 10061, true);
            }
            if (player->GetQuestStatus(QUEST_NO_PLACE_FOR_THE_LIVING) == QUEST_STATUS_COMPLETE
                || player->GetQuestStatus(QUEST_NO_PLACE_FOR_THE_LIVING) == QUEST_STATUS_REWARDED)
            {
                if (player->GetPhaseShift().HasPhase(10061)) PhasingHandler::RemovePhase(player, 10061, true);
                if (!player->GetPhaseShift().HasPhase(10062)) PhasingHandler::AddPhase(player, 10062, true);
            }
            if (player->GetQuestStatus(QUEST_TETHER_TO_HOME) == QUEST_STATUS_COMPLETE
                || player->GetQuestStatus(QUEST_TETHER_TO_HOME) == QUEST_STATUS_REWARDED)
            {
                if (!player->GetPhaseShift().HasPhase(10063)) PhasingHandler::AddPhase(player, 10063, true);
            }
            if (player->GetQuestStatus(QUEST_A_DOORWAY_THROUGH_THE_VEIL) == QUEST_STATUS_COMPLETE
                || player->GetQuestStatus(QUEST_A_DOORWAY_THROUGH_THE_VEIL) == QUEST_STATUS_REWARDED)
            {
                if (player->GetPhaseShift().HasPhase(10062)) PhasingHandler::RemovePhase(player, 10062, true);
                if (player->GetPhaseShift().HasPhase(10063)) PhasingHandler::RemovePhase(player, 10063, true);
                if (!player->GetPhaseShift().HasPhase(10064)) PhasingHandler::AddPhase(player, 10064, true);
            }

            if (player->GetQuestStatus(QUEST_THE_PATH_TO_BASTION) == QUEST_STATUS_REWARDED)
            {
                if (const Quest* quest = sObjectMgr->GetQuestTemplate(QUEST_THE_FIRST_MOVE))
                    if (player->GetQuestStatus(QUEST_THE_FIRST_MOVE) == QUEST_STATUS_NONE)
                    {
                        player->AddQuest(quest, nullptr);
                    }
                if (player->GetPhaseShift().HasPhase(10061)) PhasingHandler::RemovePhase(player, 10061, true);
                if (!player->GetPhaseShift().HasPhase(10069)) PhasingHandler::AddPhase(player, 10069, true);
            }
            if (player->GetQuestStatus(QUEST_CANT_TURN_OUR_BACKS) == QUEST_STATUS_REWARDED
                || player->GetQuestStatus(QUEST_REPORT_TO_ORIBOS) == QUEST_STATUS_REWARDED)
            {
                if (player->GetPhaseShift().HasPhase(10061)) PhasingHandler::RemovePhase(player, 10061, true);
                if (player->GetPhaseShift().HasPhase(10069)) PhasingHandler::RemovePhase(player, 10069, true);
                if (!player->GetPhaseShift().HasPhase(10076)) PhasingHandler::AddPhase(player, 10076, true);
            }

            player->GetScheduler().Schedule(Milliseconds(2000), [this, player](TaskContext context)
                {
                    if (!player->GetPhaseShift().HasPhase(10060)) PhasingHandler::AddPhase(player, 10060, true);
                });
        }
    }
};

class scene_oribos : public SceneScript
{
public:
    scene_oribos() : SceneScript("scene_oribos") { }

    void OnSceneStart(Player* player, uint32 /*sceneInstanceID*/, SceneTemplate const* sceneTemplate) override { }

    // Called when a scene is canceled
    void OnSceneCancel(Player* player, uint32 /*sceneInstanceID*/, SceneTemplate const* sceneTemplate) override
    {
        if (sceneTemplate->ScenePackageId == 3065)
            NoPlaceForTheLivingFinish(player);
        if (sceneTemplate->ScenePackageId == 2939)
            TethertoHomeFinish(player);
        if (sceneTemplate->ScenePackageId == 3112)
            PathtoBastionFinish(player);
    }

    // Called when a scene is completed
    void OnSceneComplete(Player* player, uint32 /*sceneInstanceID*/, SceneTemplate const* sceneTemplate) override
    {
        if (sceneTemplate->ScenePackageId == 3065)
            NoPlaceForTheLivingFinish(player);
        if (sceneTemplate->ScenePackageId == 2939)
            TethertoHomeFinish(player);
        if (sceneTemplate->ScenePackageId == 3112)
            PathtoBastionFinish(player);
    }

    void NoPlaceForTheLivingFinish(Player* player)
    {
        player->SummonCreature(167486, Position(-1918.77f, 1375.15f, 5267.25f, 2.35871f), TEMPSUMMON_MANUAL_DESPAWN);
        if (Creature* delen = GetClosestCreatureWithEntry(player, 167425, 20.0f))
        {
            CAST_AI(npc_overseer_kah_delen_167425, delen->AI())->_events.ScheduleEvent(EVENT_NO_PLACE_FOR_THE_LIVING_PHASE1, 2s);; // Start Event phase
        }
    }

    void TethertoHomeFinish(Player* player)
    {
        player->ForceCompleteQuest(QUEST_TETHER_TO_HOME);
        if (!player->GetPhaseShift().HasPhase(10063)) PhasingHandler::AddPhase(player, 10063, true);
    }

    void PathtoBastionFinish(Player* player)
    {
        player->ForceCompleteQuest(QUEST_THE_PATH_TO_BASTION);
        if (player->GetPhaseShift().HasPhase(10061)) PhasingHandler::RemovePhase(player, 10061, true);
        if (player->GetPhaseShift().HasPhase(10062)) PhasingHandler::RemovePhase(player, 10062, true);
        if (!player->GetPhaseShift().HasPhase(10063)) PhasingHandler::AddPhase(player, 10063, true);
    }
};

void AddSC_zone_oribos()
{
    new npc_polemarch_adrestes_171787();
    new npc_general_draven_171589();
    new npc_lady_moonberry_171795();
    new npc_secutor_mevix_171821();
    RegisterCreatureAI(npc_protector_captain_168252);
    RegisterCreatureAI(npc_overseer_kah_delen_167425);
    RegisterCreatureAI(npc_tal_inara_173615);
    RegisterCreatureAI(npc_tal_inara_159478);
    RegisterCreatureAI(npc_jaina_proudmoore_170153);
    RegisterCreatureAI(npc_thralle_171128);
    RegisterCreatureAI(npc_highlord_bolvar_fordragon_164079);
    RegisterCreatureAI(npc_ebon_blade_acolyte_172378);
    RegisterCreatureAI(npc_ebon_blade_acolyte_167682);
    RegisterCreatureAI(npc_overseer_kah_sher_175829);
    new spell_open_gate_bastion();
    new npc_pathscribe_roh_175132();
    new npc_pathscribe_roh_175133();
    new npc_polemarch_adrestes_177136();
    RegisterCreatureAI(npc_lady_moonberry_177141);
    RegisterCreatureAI(npc_anima_wyrm_178292);
    RegisterCreatureAI(npc_go_to_maw_170256);
    RegisterCreatureAI(npc_highlord_bolvar_fordragon_177230);
    RegisterSpellScript(spell_Ralling_the_Troops);
    RegisterCreatureAI(npc_go_to_korthia_179048);
    RegisterSpellScript(spell_flight_to_bastion_299119);
    RegisterPlayerScript(IntoTheMaw);
    new scene_oribos();
}
