#include "GameObject.h"
#include "GameObjectAI.h"
#include "PhasingHandler.h"
#include "ScriptMgr.h"
#include "ScriptedEscortAI.h"
#include "ScriptedGossip.h"
#include "ObjectMgr.h"


enum ShadowLandsIntro
{
    QUEST_CHILLING_SUMMONS_A          = 60545,
    QUEST_CHILLING_SUMMONS_H          = 61874,
    QUEST_THROUGH_SHATTERED_SKY       = 59751,

    SPELL_DARK_ABDUCTION_MOVIE        = 325721,
    SPELL_SHATTERED_SKY_SKYBOX        = 328755,
    SPELL_DEATHGATE_SUMMON            = 358872,
    SPELL_HOLDING_SHARD               = 326477,
    SPELL_SHARD_OF_DOMINATION_01      = 328866,
    SPELL_SHARD_OF_DOMINATION_02      = 328874,
    SPELL_SHARD_OF_DOMINATION_03      = 328876,
    SPELL_SHARD_OF_DOMINATION_04      = 328877,
    SPELL_SHARD_BASE_FX               = 328888,
    SPELL_SHARD_OF_DOMINATION_MAIN    = 328892,
    SPELL_CARRYING_SHARD_01           = 329185,
    SPELL_CARRYING_SHARD_02           = 329254,
    SPELL_CARRYING_SHARD_03           = 329255,
    SPELL_CARRYING_SHARD_04           = 329257,
    SPELL_CREDIT_SHARD_01             = 403908,
    SPELL_CREDIT_SHARD_02             = 403909,
    SPELL_CREDIT_SHARD_03             = 403910,
    SPELL_CREDIT_SHARD_04             = 403911,

    MOVIE_DARK_ABDUCTION              = 937,

    SCENE_THROUGH_SHATTERED_SKY       = 329462,

    NPC_HIGHLORD_DARION_MOGRAINE1     = 176554,
    NPC_HIGHLORD_DARION_MOGRAINE2     = 169070,

    AREA_ACHERUS                      = 13539,
    AREA_THE_FORZEN_THRONE            = 10359,
};


class player_area_forzen_Throne : public PlayerScript
{
public:
    player_area_forzen_Throne() : PlayerScript("player_area_forzen_Throne") { }

    void OnLogin(Player* player, bool /*firstLogin*/) override
    {
        if (player->GetAreaId() == AREA_THE_FORZEN_THRONE && player->GetLevel() >= 50)
        {
                if (!player->GetPhaseShift().HasPhase(10010))
                 PhasingHandler::AddPhase(player, 10010, true);
        }        
    }
    
    void OnUpdateArea(Player * player, uint32 newArea, uint32 /*oldArea*/) override
    {
        if (newArea == AREA_THE_FORZEN_THRONE && player->GetLevel() >= 50)
        {
                if (!player->GetPhaseShift().HasPhase(10010))
                 PhasingHandler::AddPhase(player, 10010, true);
        }
    }

};

//a_chilling_summons_q60454_61874_part1
class npc_highlord_darion_mograine_176554 : public CreatureScript
{
public:
    npc_highlord_darion_mograine_176554() : CreatureScript("npc_highlord_darion_mograine_176554") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_highlord_darion_mograine_176554AI(creature);
    }

    struct npc_highlord_darion_mograine_176554AI : public ScriptedAI
    {
        npc_highlord_darion_mograine_176554AI(Creature* creature) : ScriptedAI(creature)
        {
            Reset();
        }

        bool say;
        bool kEventStart;
        bool hEventStart;
        bool khEventStart;
        bool hhEventStart;
        uint32 tEvent;
        uint8 kPhase;
        uint8 hPhase;
        uint8 khPhase;
        uint8 hhPhase;

        void Reset() override
        {
            say = false;
            kEventStart = false;
            hEventStart = false;
            khEventStart = false;
            hhEventStart = false;

            tEvent = 1000;
            kPhase = 0;
            hPhase = 10;
            khPhase = 20;
            hhPhase = 30;
        }

        void MoveInLineOfSight(Unit* who) override
        {
            if (Player* player = who->ToPlayer())
                if (player->IsInDist(me, 30.0f))
                {
                    if (player->HasQuest(QUEST_CHILLING_SUMMONS_A) || player->HasQuest(QUEST_CHILLING_SUMMONS_H))
                    {
                        if (Creature* npc = GetClosestCreatureWithEntry(me, 171789, 15.0f))
                        {
                            if (!say)
                            {
                                if (player->GetClass() == CLASS_DEATH_KNIGHT)
                                {
                                    Talk(0);
                                    me->AddDelayedEvent(4000, [this, player]() -> void
                                        {
                                            Talk(1);        //to_stormwind_txt
                                        });
                                }

                                if (player->GetClass() != CLASS_DEATH_KNIGHT)
                                {
                                    Talk(4);
                                    me->AddDelayedEvent(4000, [this, player]() -> void
                                        {
                                            Talk(1);        //to_stormwind_txt
                                        });
                                }
                                say = true;
                            }
                        }
                        if (Creature* npc = GetClosestCreatureWithEntry(me, 171791, 15.0f))
                        {
                            if (!say)
                            {
                                if (player->GetClass() == CLASS_DEATH_KNIGHT)
                                {
                                    Talk(0);
                                    me->AddDelayedEvent(4000, [this, player]() -> void
                                        {
                                            Talk(2);        //to_stormwind_txt
                                        });
                                }

                                if (player->GetClass() != CLASS_DEATH_KNIGHT)
                                {
                                    Talk(4);
                                    me->AddDelayedEvent(4000, [this, player]() -> void
                                        {
                                            Talk(2);        //to_stormwind_txt
                                        });
                                }
                                say = true;
                            }
                           
                        }      
                    }
                }
        }

        void UpdateAI(uint32 diff) override
        {
            if (kPhase > 5)
            {
                Reset();
              return;
            }

            if (hPhase > 15)
            {
                Reset();
              return;
            }

            if (khPhase > 25)
            {
                Reset();
                return;
            }

            if (hhPhase > 35)
            {
                Reset();
                return;
            }

            if (kEventStart)                     //alliance_qust_a_chilling_summons_dailog_start
            {
                if (tEvent <= diff)
                {
                    switch (kPhase)
                    {
                    case (0):
                        JumpToNextStep(2000);
                        break;

                    case (1):
                        Talk(3);
                        JumpToNextStep(11000);
                        break;

                    case (2):
                        if (Creature* npc = GetClosestCreatureWithEntry(me, 61839, 50.0f))
                            me->SetFacingToObject(npc);
                        JumpToNextStep(2000);
                        break;

                    case (3):
                        me->CastSpell(me, SPELL_DEATHGATE_SUMMON);
                        me->SummonCreature(171039, Position(-8479.98f, 395.578f, 115.841f, 3.22238f), TEMPSUMMON_TIMED_DESPAWN, 1min, 0U);
                        JumpToNextStep(3000);
                        break;

                    case (4):
                        me->CastStop();
                        Talk(6);
                        JumpToNextStep(10000);
                        break;

                    case (5):
                        me->SetWalk(true);
                        me->GetMotionMaster()->MovePoint(0, -8479.98f, 395.578f, 115.841f);
                        me->DespawnOrUnsummon(4s);
                        me->SetRespawnDelay(60);
                        break;
                    }
                }
                else tEvent -= diff;
            }


            if (hEventStart)
            {
                if (tEvent <= diff)
                {
                    switch (hPhase)
                    {
                    case (10):
                        JumpToNextStep(2000);
                        break;

                    case (11):
                        Talk(5);
                        JumpToNextStep(11000);
                        break;

                    case (12):
                        if (Creature* npc = GetClosestCreatureWithEntry(me, 61839, 50.0f))
                            me->SetFacingToObject(npc);
                        JumpToNextStep(2000);
                        break;

                    case (13):
                        me->CastSpell(me, SPELL_DEATHGATE_SUMMON);
                        me->SummonCreature(171039, Position(-8479.98f, 395.578f, 115.841f, 3.22238f), TEMPSUMMON_TIMED_DESPAWN, 1min, 0U);
                        JumpToNextStep(3000);
                        break;

                    case (14):
                        me->CastStop();
                        Talk(6);
                        JumpToNextStep(10000);
                        break;

                    case (15):
                        me->SetWalk(true);
                        me->GetMotionMaster()->MovePoint(0, -8479.98f, 395.578f, 115.841f);
                        me->DespawnOrUnsummon(4s);
                        me->SetRespawnDelay(60);
                        break;
                    }
                }
                else tEvent -= diff;
            }

            if (khEventStart)                //horde_qust_a_chilling_summons_dailog_start
            {
                if (tEvent <= diff)
                {
                    switch (khPhase)
                    {
                    case (20):
                        JumpToNextStep(2000);
                        break;

                    case (21):
                        Talk(3);
                        JumpToNextStep(11000);
                        break;

                    case (22):
                        if (Creature* npc = GetClosestCreatureWithEntry(me, 173281, 50.0f))
                            me->SetFacingToObject(npc);
                        JumpToNextStep(2000);
                        break;

                    case (23):
                        me->CastSpell(me, SPELL_DEATHGATE_SUMMON);
                        me->SummonCreature(171039, Position(1594.55f, -4393.88f, 17.64001f, 4.1116f), TEMPSUMMON_TIMED_DESPAWN, 1min, 0U);
                        JumpToNextStep(3000);
                        break;

                    case (24):
                        me->CastStop();
                        Talk(6);
                        JumpToNextStep(10000);
                        break;

                    case (25):
                        me->SetWalk(true);
                        me->GetMotionMaster()->MovePoint(0, 1594.55f, -4393.88f, 17.64001f);
                        me->DespawnOrUnsummon(4s);
                        me->SetRespawnDelay(60);
                        break;
                    }
                }
                else tEvent -= diff;
            }

            if (hhEventStart)
            {
                if (tEvent <= diff)
                {
                    switch (hhPhase)
                    {
                    case (30):
                        JumpToNextStep(2000);
                        break;

                    case (31):
                        Talk(5);
                        JumpToNextStep(11000);
                        break;

                    case (32):
                        if (Creature* npc = GetClosestCreatureWithEntry(me, 173281, 50.0f))
                            me->SetFacingToObject(npc);
                        JumpToNextStep(2000);
                        break;

                    case (33):
                        me->CastSpell(me, SPELL_DEATHGATE_SUMMON);
                        me->SummonCreature(171039, Position(1594.55f, -4393.88f, 17.64001f, 4.1116f), TEMPSUMMON_TIMED_DESPAWN, 1min, 0U);
                        JumpToNextStep(3000);
                        break;

                    case (34):
                        me->CastStop();
                        Talk(6);
                        JumpToNextStep(10000);
                        break;

                    case (35):
                        me->SetWalk(true);
                        me->GetMotionMaster()->MovePoint(0, 1594.55f, -4393.88f, 17.64001f);
                        me->DespawnOrUnsummon(4s);
                        me->SetRespawnDelay(60);
                        break;
                    }
                }
                else tEvent -= diff;
            }
        }

        void JumpToNextStep(uint32 uiTimer)
        {
            tEvent = uiTimer;
            ++kPhase;
            ++hPhase;
            ++khPhase;
            ++hhPhase;
        }
    };
};

// npc_death_gate_to_acherus_171039
class npc_death_gate_to_acherus_171039 : public CreatureScript
{
public:
    npc_death_gate_to_acherus_171039() : CreatureScript("npc_death_gate_to_acherus_171039") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_death_gate_to_acherus_171039AI(creature);
    }

    struct npc_death_gate_to_acherus_171039AI : public ScriptedAI
    {
        npc_death_gate_to_acherus_171039AI(Creature* creature) : ScriptedAI(creature) {}

        bool OnGossipHello(Player* player) override
      
            {
                player->KilledMonsterCredit(173229);
                player->TeleportTo(2147, -642.370f, 2208.391f, 550.713f, 0.044f);
                return true;
        }

    };
};

//171789,171791
class npc_chilling_summons_commander : public CreatureScript
{
public:
    npc_chilling_summons_commander() : CreatureScript("npc_chilling_summons_commander") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_chilling_summons_commanderAI(creature);
    }

    struct npc_chilling_summons_commanderAI : public ScriptedAI
    {
        npc_chilling_summons_commanderAI(Creature* creature) : ScriptedAI(creature) {}

        bool OnGossipHello(Player* player) override
        {
            AddGossipItemFor(player, GossipOptionNpc::None, "Tell me what happened.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 0);
            SendGossipMenuFor(player, 41731, me->GetGUID());
            player->PrepareQuestMenu(me->GetGUID());

            return true;
        }

        bool OnGossipSelect(Player* player, uint32 /*menuId*/, uint32 gossipListId ) override
        {
            if (gossipListId == 0)
            {
                /*
                player->UpdateQuestObjectiveProgress(QUEST_OBJECTIVE_CRITERIA_TREE, 87208, 1);
                player->UpdateQuestObjectiveProgress(QUEST_OBJECTIVE_CRITERIA_TREE, 87225, 1);
                CloseGossipMenuFor(player);
                if ((player->GetQuestObjectiveProgress(QUEST_CHILLING_SUMMONS_A, 1) == 1) || (player->GetQuestObjectiveProgress(QUEST_CHILLING_SUMMONS_H, 1) == 1))
                {
                    player->CastSpell(player, SPELL_DARK_ABDUCTION_MOVIE, true);
                }
                */
                if (Quest const* quest = sObjectMgr->GetQuestTemplate(QUEST_CHILLING_SUMMONS_A))
                    for (QuestObjective const& obj : quest->GetObjectives())
                        if (obj.ObjectID == 87208) // Learn about the fate of your leaders
                        {
                            uint16 slot = player->FindQuestSlot(QUEST_CHILLING_SUMMONS_A);
                            if (!player->IsQuestObjectiveComplete(slot, quest, obj)) // just make it complete once
                            {
                                player->SetQuestObjectiveData(obj, 1);
                                player->SendQuestUpdateAddCredit(quest, ObjectGuid::Empty, obj, 1);
                                player->CastSpell(player, SPELL_DARK_ABDUCTION_MOVIE, true);
                            }
                        }
                if (Quest const* quest = sObjectMgr->GetQuestTemplate(QUEST_CHILLING_SUMMONS_H))
                    for (QuestObjective const& obj : quest->GetObjectives())
                        if (obj.ObjectID == 87225) // Learn about the fate of your leaders
                        {
                            uint16 slot = player->FindQuestSlot(QUEST_CHILLING_SUMMONS_H);
                            if (!player->IsQuestObjectiveComplete(slot, quest, obj)) // just make it complete once
                            {
                                player->SetQuestObjectiveData(obj, 1);
                                player->SendQuestUpdateAddCredit(quest, ObjectGuid::Empty, obj, 1);
                                player->CastSpell(player, SPELL_DARK_ABDUCTION_MOVIE, true);
                            }
                        }
                //player->KilledMonsterCredit(173229);
                //player->KilledMonsterCredit(171789);
                // player->TeleportTo(2147, -633.468f, 2210.952f, 550.705f, 0.09f);

            }
            return true;
        }
    };
};

class player_credit_to_dark_abduction : public PlayerScript
{
public:
    player_credit_to_dark_abduction() : PlayerScript("player_credit_to_dark_abduction") { }

    void OnMovieComplete(Player* player, uint32 movieId)
    {
        if (player->HasQuest(QUEST_CHILLING_SUMMONS_A))
        {
            if (Creature* mograine = GetClosestCreatureWithEntry(player, NPC_HIGHLORD_DARION_MOGRAINE1, 30.0f))
            {
                CAST_AI(npc_highlord_darion_mograine_176554::npc_highlord_darion_mograine_176554AI, mograine->AI())->hEventStart = true; // Start Event
            }

            
            if (player->GetClass() == CLASS_DEATH_KNIGHT)
            {
                if (movieId == MOVIE_DARK_ABDUCTION)
                {
                    if (Creature* mograine = GetClosestCreatureWithEntry(player, NPC_HIGHLORD_DARION_MOGRAINE1, 30.0f))
                    {
                        CAST_AI(npc_highlord_darion_mograine_176554::npc_highlord_darion_mograine_176554AI, mograine->AI())->kEventStart = true; // Start Event
                    }
                }
            }

            if (player->GetClass() != CLASS_DEATH_KNIGHT)
            {
                if (movieId == MOVIE_DARK_ABDUCTION)
                {
                    if (Creature* mograine = GetClosestCreatureWithEntry(player, NPC_HIGHLORD_DARION_MOGRAINE1, 30.0f))
                    {
                        CAST_AI(npc_highlord_darion_mograine_176554::npc_highlord_darion_mograine_176554AI, mograine->AI())->hEventStart = true; // Start Event
                    }
                }
            }
            
        }

        if (player->HasQuest(QUEST_CHILLING_SUMMONS_H))
        {
            if (movieId == MOVIE_DARK_ABDUCTION)
            {
                if (Creature* mograine = GetClosestCreatureWithEntry(player, NPC_HIGHLORD_DARION_MOGRAINE1, 30.0f))
                {
                    CAST_AI(npc_highlord_darion_mograine_176554::npc_highlord_darion_mograine_176554AI, mograine->AI())->khEventStart = true; // Start Event
                }
            }

            
            if (player->GetClass() == CLASS_DEATH_KNIGHT)
            {
                if (movieId == MOVIE_DARK_ABDUCTION)
                {
                    if (Creature* mograine = GetClosestCreatureWithEntry(player, NPC_HIGHLORD_DARION_MOGRAINE1, 30.0f))
                    {
                        CAST_AI(npc_highlord_darion_mograine_176554::npc_highlord_darion_mograine_176554AI, mograine->AI())->khEventStart = true; // Start Event
                    }
                }
            }

            if (player->GetClass() != CLASS_DEATH_KNIGHT)
            {
                if (movieId == MOVIE_DARK_ABDUCTION)
                {
                    if (Creature* mograine = GetClosestCreatureWithEntry(player, NPC_HIGHLORD_DARION_MOGRAINE1, 30.0f))
                    {
                        CAST_AI(npc_highlord_darion_mograine_176554::npc_highlord_darion_mograine_176554AI, mograine->AI())->hhEventStart = true; // Start Event
                    }
                }
            }
            
        }
    }
};

static const Position mograinePositions[3] =
{
    { -604.294f, 2208.37f, 550.513f },
    { -561.862f, 2209.21f, 539.338f },
    { -548.972f, 2211.22f, 539.277f }
};

//a_chilling_summons_q60454_61874_part2
class npc_highlord_darion_mograine_169070 : public CreatureScript
{
public:
    npc_highlord_darion_mograine_169070() : CreatureScript("npc_highlord_darion_mograine_169070") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_highlord_darion_mograine_169070AI(creature);
    }

    struct npc_highlord_darion_mograine_169070AI : public EscortAI
    {
        npc_highlord_darion_mograine_169070AI(Creature* creature) : EscortAI(creature)
        {
            Reset();
        }

    private:
        bool dailog;
        bool EventStart1;
        bool EventStart2;
  
        uint32 tEvent;
        uint8  Phase1;
        uint8  Phase2;

        void Reset() override
        {
            dailog = false;
            EventStart1 = false;
            EventStart2 = false;

            tEvent = 1000;
            Phase1 = 0;
            Phase2 = 10;
        }

        void MoveInLineOfSight(Unit* who) override
        {
            if (Player* player = who->ToPlayer())
                if (player->IsInDist(me, 10.0f))
                {
                    if (player->GetAreaId() == AREA_ACHERUS)
                    if (player->HasQuest(QUEST_CHILLING_SUMMONS_A) || player->HasQuest(QUEST_CHILLING_SUMMONS_H))
                    {
                        if (!dailog)
                        {
                            me->SetWalk(true);
                            me->GetMotionMaster()->MovePoint(0, mograinePositions[0]);
                            dailog = true;
                            EventStart1 = true;
                        }
                    }
                }
        }

        void UpdateAI(uint32 diff) override
        {
            if (Phase1 == 4)
            {
                EventStart2 = true;
                me->GetMotionMaster()->MovePoint(1, mograinePositions[1]);
                Phase2 = 10;
            }

            if (Phase2 > 13)
            {
                Reset();
                return;
            }

            if (EventStart1)
            {
                if (tEvent <= diff)
                {
                    switch (Phase1)
                    {
                    case 0:
                    {
                        Talk(0);
                        JumpToNextStep(12000);
                        break;
                    }
                    case 1:
                    {
                        Talk(1);
                        JumpToNextStep(8000);
                        break;
                    }
                    case 2:
                    {
                        Talk(2);
                        JumpToNextStep(12000);
                        break;
                    case 3:
                    {
                        JumpToNextStep(5000);
                        break;
                    }
                    }
                    }
                }
                else tEvent -= diff;

                if (EventStart2)
                {
                    if (tEvent <= diff)
                    {
                        switch (Phase2)
                        {
                        case 10:
                        {
                            Talk(3);
                            JumpToNextStep(10000);
                            break;
                        }
                        case 11:
                        {
                            Talk(4);
                            JumpToNextStep(8000);
                            break;
                        }
                        case 12:
                        {
                            me->GetMotionMaster()->MovePoint(2, mograinePositions[2]);
                            JumpToNextStep(8000);
                            break;
                        }
                        case 13:
                            me->DespawnOrUnsummon(5s);
                            me->SetRespawnDelay(60);
                            break;
                        }
                    }
                    else tEvent -= diff;
                }
            }
        }

        void JumpToNextStep(uint32 uiTimer)
        {
            tEvent = uiTimer;
            ++Phase1;
            ++Phase2;
        }
    };
};

//115634
struct npc_deathbringer_rise_teleport_controller : public ScriptedAI
{
    npc_deathbringer_rise_teleport_controller(Creature* c) : ScriptedAI(c) { }

    void MoveInLineOfSight(Unit* target) override
    {
        if (!target->IsPlayer())
            return;

        if (target->GetDistance2d(me) <= 6.0f)
            if (Player* player = target->ToPlayer())
            {
                player->TeleportTo(2147, 559.735f, -2125.220f, 840.856f, 3.176f);
                
                if (player->HasQuest(QUEST_CHILLING_SUMMONS_H) || player->HasQuest(QUEST_CHILLING_SUMMONS_A))
                {
                    if (Quest const* quest = sObjectMgr->GetQuestTemplate(QUEST_CHILLING_SUMMONS_H))
                        for (QuestObjective const& obj : quest->GetObjectives())
                        if (obj.ObjectID == 170749) // Use the teleporter to go to the Frozen Throne
                        {
                            uint16 slot = player->FindQuestSlot(QUEST_CHILLING_SUMMONS_H);
                            player->ForceCompleteQuest(QUEST_CHILLING_SUMMONS_H);
                            if (!player->IsQuestObjectiveComplete(slot, quest, obj)) // just make it complete once
                            {
                                player->SetQuestObjectiveData(obj, 1);
                                player->SendQuestUpdateAddCredit(quest, ObjectGuid::Empty, obj, 1);
                            }
                        }
                    if (Quest const* quest = sObjectMgr->GetQuestTemplate(QUEST_CHILLING_SUMMONS_A))
                         for (QuestObjective const& obj : quest->GetObjectives())
                         if (obj.ObjectID == 170749) // Use the teleporter to go to the Frozen Throne
                         {
                             uint16 slot = player->FindQuestSlot(QUEST_CHILLING_SUMMONS_A);
                             player->ForceCompleteQuest(QUEST_CHILLING_SUMMONS_A);
                             if (!player->IsQuestObjectiveComplete(slot, quest, obj)) // just make it complete once
                             {
                                player->SetQuestObjectiveData(obj, 1);
                                player->SendQuestUpdateAddCredit(quest, ObjectGuid::Empty, obj, 1);
                             }
                         }
                }
            }
    }
};

//169076
class npc_highlord_bolvar_fordragon_169076 : public CreatureScript
{
public:
    npc_highlord_bolvar_fordragon_169076() : CreatureScript("npc_highlord_bolvar_fordragon_169076") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_highlord_bolvar_fordragon_169076AI(creature);
    }

    struct npc_highlord_bolvar_fordragon_169076AI : public ScriptedAI
    {
        npc_highlord_bolvar_fordragon_169076AI(Creature* c) : ScriptedAI(c) { Reset(); }

    private:
        bool dailog;
        bool EventStart;
        bool EventStart2;
       
        uint32 tEvent;
        uint8  Phase;
        uint8  Phase2;

        ObjectGuid m_playerGUID;

        ObjectGuid uiTyrandeGUID;
        ObjectGuid uiGennGUID;
        ObjectGuid uiMuradinGUID;
        ObjectGuid uiAlleGUID;
        ObjectGuid uiGelbinGUID;

        ObjectGuid uiTheronGUID;
        ObjectGuid uiCaliaGUID;
        ObjectGuid uiFirstGUID;
        ObjectGuid uiRokhanGUID;
        ObjectGuid uiGazlowoGUID;

        void Reset() override
        {
            dailog = false;
            EventStart = false;
            EventStart2 = false;
            tEvent = 1000;
            Phase = 0;
            Phase2 = 20;
            m_playerGUID.Clear();

            uiTyrandeGUID.Clear();
            uiGennGUID.Clear();
            uiMuradinGUID.Clear();
            uiAlleGUID.Clear();
            uiGelbinGUID.Clear();
            uiTheronGUID.Clear();
            uiCaliaGUID.Clear();
            uiFirstGUID.Clear();
            uiRokhanGUID.Clear();
            uiGazlowoGUID.Clear();
        }

        void MoveInLineOfSight(Unit* who) override
        {
            if (Player* player = who->ToPlayer())
                if (player->IsInDist(me, 30.0f))
                {
                    if (player->HasQuest(QUEST_CHILLING_SUMMONS_A) || player->HasQuest(QUEST_CHILLING_SUMMONS_H))
                    {
                        
                        if (!dailog)
                        {
                                m_playerGUID = player->GetGUID();                   // write playerguid in empty_guid 
                            if (Creature* tyrande = GetClosestCreatureWithEntry(me, 169072, 50.0f))
                                uiTyrandeGUID = tyrande->GetGUID();
                            if (Creature* genn = GetClosestCreatureWithEntry(me, 169071, 50.0f))
                                uiGennGUID = genn->GetGUID();
                            if (Creature* muradin = GetClosestCreatureWithEntry(me, 170763, 50.0f))
                                uiMuradinGUID = muradin->GetGUID();
                            if (Creature* alle = GetClosestCreatureWithEntry(me, 170762, 50.0f))
                                uiAlleGUID = alle->GetGUID();
                            if (Creature* gelbin = GetClosestCreatureWithEntry(me, 170761, 50.0f))
                                uiGelbinGUID = gelbin->GetGUID();
                            if (Creature* theron = GetClosestCreatureWithEntry(me, 169073, 50.0f))
                                uiTheronGUID = theron->GetGUID();
                            if (Creature* calia = GetClosestCreatureWithEntry(me, 169075, 50.0f))
                                uiCaliaGUID = calia->GetGUID();
                            if (Creature* first = GetClosestCreatureWithEntry(me, 170759, 50.0f))
                                uiFirstGUID = first->GetGUID();
                            if (Creature* rokhan = GetClosestCreatureWithEntry(me, 170758, 50.0f))
                                uiRokhanGUID = rokhan->GetGUID();
                            if (Creature* gazlowo = GetClosestCreatureWithEntry(me, 170760, 50.0f))
                                uiGazlowoGUID = gazlowo->GetGUID();

                            dailog = true;
                            //EventStart = true;
                        }
                        

                        if (!EventStart) // fluxurion mechanic (sorry i had to do this, old mechanic stuck befire calia talk)
                        {
                            EventStart = true;

                            if (Creature* theron = ObjectAccessor::GetCreature(*me, uiTheronGUID))
                                theron->AI()->Talk(0);

                            me->AddDelayedEvent(10000, [this]() -> void // old case 1
                                {
                                    if (Creature* theron = ObjectAccessor::GetCreature(*me, uiTheronGUID))
                                        theron->AI()->Talk(1);
                                });
                            me->AddDelayedEvent(23000, [this]() -> void // old case 2
                                {
                                    if (Creature* tyrande = ObjectAccessor::GetCreature(*me, uiTyrandeGUID))
                                        tyrande->AI()->Talk(0);
                                });
                            me->AddDelayedEvent(30500, [this]() -> void // old case 3
                                {
                                    if (Creature* genn = ObjectAccessor::GetCreature(*me, uiGennGUID))
                                        genn->AI()->Talk(0);
                                });
                            me->AddDelayedEvent(43000, [this]() -> void // old case 4
                                {
                                    if (Creature* calia = ObjectAccessor::GetCreature(*me, uiCaliaGUID))
                                        calia->AI()->Talk(0);
                                });
                            me->AddDelayedEvent(51000, [this]() -> void // old case 5
                                {
                                    if (Creature* npc = ObjectAccessor::GetCreature(*me, uiTyrandeGUID))
                                    {
                                        npc->HandleEmoteCommand(EMOTE_STATE_READY_UNARMED);
                                        if (Creature* theron = GetClosestCreatureWithEntry(me, 169073, 30.0f))
                                            npc->SetFacingToObject(theron);
                                    }
                                    if (Creature* npc = ObjectAccessor::GetCreature(*me, uiGennGUID))
                                    {
                                        npc->HandleEmoteCommand(EMOTE_STATE_READY_UNARMED);
                                        if (Creature* theron = GetClosestCreatureWithEntry(me, 169073, 30.0f))
                                            npc->SetFacingToObject(theron);
                                    }
                                    if (Creature* npc = ObjectAccessor::GetCreature(*me, uiMuradinGUID))
                                    {
                                        npc->HandleEmoteCommand(EMOTE_STATE_READY_UNARMED);
                                        if (Creature* theron = GetClosestCreatureWithEntry(me, 169073, 30.0f))
                                            npc->SetFacingToObject(theron);
                                    }
                                    if (Creature* npc = ObjectAccessor::GetCreature(*me, uiAlleGUID))
                                    {
                                        npc->HandleEmoteCommand(EMOTE_STATE_READY_UNARMED);
                                        if (Creature* theron = GetClosestCreatureWithEntry(me, 169073, 30.0f))
                                            npc->SetFacingToObject(theron);
                                    }
                                    if (Creature* npc = ObjectAccessor::GetCreature(*me, uiGelbinGUID))
                                    {
                                        npc->HandleEmoteCommand(EMOTE_STATE_READY_UNARMED);
                                        if (Creature* theron = GetClosestCreatureWithEntry(me, 169073, 30.0f))
                                            npc->SetFacingToObject(theron);
                                    }

                                    //horde
                                    if (Creature* npc = ObjectAccessor::GetCreature(*me, uiTheronGUID))
                                    {
                                        npc->HandleEmoteCommand(EMOTE_STATE_READY_UNARMED);
                                        if (Creature* tyrande = GetClosestCreatureWithEntry(me, 169072, 30.0f))
                                            npc->SetFacingToObject(tyrande);
                                    }
                                    if (Creature* npc = ObjectAccessor::GetCreature(*me, uiCaliaGUID))
                                    {
                                        npc->HandleEmoteCommand(EMOTE_STATE_READY_UNARMED);
                                        if (Creature* tyrande = GetClosestCreatureWithEntry(me, 169072, 30.0f))
                                            npc->SetFacingToObject(tyrande);
                                    }
                                    if (Creature* npc = ObjectAccessor::GetCreature(*me, uiFirstGUID))
                                    {
                                        npc->HandleEmoteCommand(EMOTE_STATE_READY_UNARMED);
                                        if (Creature* tyrande = GetClosestCreatureWithEntry(me, 169072, 30.0f))
                                            npc->SetFacingToObject(tyrande);
                                    }
                                    if (Creature* npc = ObjectAccessor::GetCreature(*me, uiRokhanGUID))
                                    {
                                        npc->HandleEmoteCommand(EMOTE_STATE_READY_UNARMED);
                                        if (Creature* tyrande = GetClosestCreatureWithEntry(me, 169072, 30.0f))
                                            npc->SetFacingToObject(tyrande);
                                    }
                                    if (Creature* npc = ObjectAccessor::GetCreature(*me, uiGazlowoGUID))
                                    {
                                        npc->HandleEmoteCommand(EMOTE_STATE_READY_UNARMED);
                                        if (Creature* tyrande = GetClosestCreatureWithEntry(me, 169072, 30.0f))
                                            npc->SetFacingToObject(tyrande);
                                    }
                                });
                            me->AddDelayedEvent(53000, [this]() -> void // old case 6
                                {
                                    if (Creature* tyrande = ObjectAccessor::GetCreature(*me, uiTyrandeGUID))
                                        tyrande->AI()->Talk(1);
                                });
                            me->AddDelayedEvent(60500, [this]() -> void // old case 7
                                {
                                    if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                                        player->KilledMonsterCredit(170749);
                                    Talk(0);
                                });
                            me->AddDelayedEvent(67500, [this]() -> void // old case 8 + start event2
                                {
                                    Talk(1);
                                });
                        }

                    }

                }
        }

        /*
        void UpdateAI(uint32 diff) override
        {
            if (Phase > 8)
            {
                if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                player->GetScheduler().Schedule(60s, [this](TaskContext context)
                    {
                        Reset();
                    });
                return;
            }

            if (Phase2 > 23)
            {
                Reset();
            return;
            }

            if (EventStart)
            {
                if (tEvent <= diff)
                {
                    switch (Phase)
                    {
                    case 0:
                    {
                        if (Creature* theron = ObjectAccessor::GetCreature(*me, uiTheronGUID))
                            theron->AI()->Talk(0);
                        JumpToNextStep(10000);
                        break;
                    }
                    case 1:
                    {
                        if (Creature* theron = ObjectAccessor::GetCreature(*me, uiTheronGUID))
                            theron->AI()->Talk(1);
                        JumpToNextStep(12000);
                        break;
                    }
                    case 2:
                    {
                        if (Creature* tyrande = ObjectAccessor::GetCreature(*me, uiTyrandeGUID))
                            tyrande->AI()->Talk(0);
                        JumpToNextStep(8000);
                        break;
                    }
                    case 3:
                    {
                        if (Creature* genn = ObjectAccessor::GetCreature(*me, uiGennGUID))
                            genn->AI()->Talk(0);
                        JumpToNextStep(13000);
                        break;
                    }
                    case 4:
                    {
                        if (Creature* calia = ObjectAccessor::GetCreature(*me, uiCaliaGUID))
                            calia->AI()->Talk(0);
                        JumpToNextStep(8000);
                        break;
                    }
                    case 5:
                    {
                        if (Creature* npc = ObjectAccessor::GetCreature(*me, uiTyrandeGUID))
                        {
                            npc->HandleEmoteCommand(EMOTE_STATE_READY_UNARMED);
                            if (Creature* theron = GetClosestCreatureWithEntry(me, 169073, 30.0f))
                                npc->SetFacingToObject(theron);
                        }
                        if (Creature* npc = ObjectAccessor::GetCreature(*me, uiGennGUID))
                        {
                            npc->HandleEmoteCommand(EMOTE_STATE_READY_UNARMED);
                            if (Creature* theron = GetClosestCreatureWithEntry(me, 169073, 30.0f))
                                npc->SetFacingToObject(theron);
                        }
                        if (Creature* npc = ObjectAccessor::GetCreature(*me, uiMuradinGUID))
                        {
                            npc->HandleEmoteCommand(EMOTE_STATE_READY_UNARMED);
                            if (Creature* theron = GetClosestCreatureWithEntry(me, 169073, 30.0f))
                                npc->SetFacingToObject(theron);
                        }
                        if (Creature* npc = ObjectAccessor::GetCreature(*me, uiAlleGUID))
                        {
                            npc->HandleEmoteCommand(EMOTE_STATE_READY_UNARMED);
                            if (Creature* theron = GetClosestCreatureWithEntry(me, 169073, 30.0f))
                                npc->SetFacingToObject(theron);
                        }
                        if (Creature* npc = ObjectAccessor::GetCreature(*me, uiGelbinGUID))
                        {
                            npc->HandleEmoteCommand(EMOTE_STATE_READY_UNARMED);
                            if (Creature* theron = GetClosestCreatureWithEntry(me, 169073, 30.0f))
                                npc->SetFacingToObject(theron);
                        }

                        //horde
                        if (Creature* npc = ObjectAccessor::GetCreature(*me, uiTheronGUID))
                        {
                            npc->HandleEmoteCommand(EMOTE_STATE_READY_UNARMED);
                            if (Creature* tyrande = GetClosestCreatureWithEntry(me, 169072, 30.0f))
                                npc->SetFacingToObject(tyrande);
                        }
                        if (Creature* npc = ObjectAccessor::GetCreature(*me, uiCaliaGUID))
                        {
                            npc->HandleEmoteCommand(EMOTE_STATE_READY_UNARMED);
                            if (Creature* tyrande = GetClosestCreatureWithEntry(me, 169072, 30.0f))
                                npc->SetFacingToObject(tyrande);
                        }
                        if (Creature* npc = ObjectAccessor::GetCreature(*me, uiFirstGUID))
                        {
                            npc->HandleEmoteCommand(EMOTE_STATE_READY_UNARMED);
                            if (Creature* tyrande = GetClosestCreatureWithEntry(me, 169072, 30.0f))
                                npc->SetFacingToObject(tyrande);
                        }
                        if (Creature* npc = ObjectAccessor::GetCreature(*me, uiRokhanGUID))
                        {
                            npc->HandleEmoteCommand(EMOTE_STATE_READY_UNARMED);
                            if (Creature* tyrande = GetClosestCreatureWithEntry(me, 169072, 30.0f))
                                npc->SetFacingToObject(tyrande);
                        }
                        if (Creature* npc = ObjectAccessor::GetCreature(*me, uiGazlowoGUID))
                        {
                            npc->HandleEmoteCommand(EMOTE_STATE_READY_UNARMED);
                            if (Creature* tyrande = GetClosestCreatureWithEntry(me, 169072, 30.0f))
                                npc->SetFacingToObject(tyrande);
                        }
                        JumpToNextStep(2000);
                        break;

                    case 6:
                    {
                        if (Creature* tyrande = ObjectAccessor::GetCreature(*me, uiTyrandeGUID))
                            tyrande->AI()->Talk(1);
                        JumpToNextStep(7500);
                        break;
                    }
                    case 7:
                    {
                        if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                            player->KilledMonsterCredit(170749);
                        Talk(0);
                        JumpToNextStep(7000);
                        break;
                    }case 8:
                    {
                        Talk(1);
                        JumpToNextStep(10000);
                        break;
                    }
                    }
                    }
                }
                else tEvent -= diff;
            }

            if (EventStart2)         //horde_qust_through_shattered_sky_dailog_start_59751
            {
                if (tEvent <= diff)
                {
                    switch (Phase2)
                    {
                    case 20:
                    {
                        Talk(2);
                        JumpToNextStep(11000);
                        break;
                    }
                    case 21:
                    {
                        Talk(3);
                        JumpToNextStep(14500);
                        break;
                    }
                    case 22:
                    {
                        Talk(4);
                        JumpToNextStep(11500);
                        break;
                    }
                    case 23:
                    {
                        Talk(5);
                        if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                        player->KilledMonsterCredit(169076);
                        JumpToNextStep(8000);
                        break;
                    }
                    }
                }
                else tEvent -= diff;
            }
            
        }
        */
                   
        void OnQuestAccept(Player* player, Quest const* quest) override
        {
            if (quest->GetQuestId() == QUEST_THROUGH_SHATTERED_SKY)
            {
                m_playerGUID = player->GetGUID();
                if (Creature* tyrande = GetClosestCreatureWithEntry(me, 169072, 50.0f))
                    uiTyrandeGUID = tyrande->GetGUID();
                if (Creature* genn = GetClosestCreatureWithEntry(me, 169071, 50.0f))
                    uiGennGUID = genn->GetGUID();
                if (Creature* theron = GetClosestCreatureWithEntry(me, 169073, 50.0f))
                    uiTheronGUID = theron->GetGUID();
                if (Creature* darion = GetClosestCreatureWithEntry(me, 169070, 50.0f))
                    uiTheronGUID = darion->GetGUID();

                //EventStart2 = true;
                // old mechanic event2
                Talk(2);
                me->AddDelayedEvent(11000, [this]() -> void // old case 21
                    {
                        Talk(3);
                    });
                me->AddDelayedEvent(25500, [this]() -> void // old case 22
                    {
                        Talk(4);
                    });
                me->AddDelayedEvent(36500, [this]() -> void // old case 23
                    {
                        Talk(5);
                        if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                        {
                            player->KilledMonsterCredit(169076);
                            PhasingHandler::RemovePhase(player, 10010, true);
                            PhasingHandler::AddPhase(player, 10020, true);
                        }
                    });
            }
        }

        void JumpToNextStep(uint32 uiTimer)
        {
            tEvent = uiTimer;
            ++Phase;
            ++Phase2;
        }
    };
};

enum ShardOfDomination
{
    EVENT_VOLVAR_SAY6 = 1,
    EVENT_VOLVAR_SAY7 = 2,
    EVENT_VOLVAR_SAY8 = 3,
    EVENT_VOLVAR_SAY9 = 4,
    EVENT_VOLVAR_SAY10 = 5,
    EVENT_VOLVAR_SAY11 = 6,
    EVENT_VOLVAR_SAY12 = 7,
    EVENT_VOLVAR_SAY13 = 8,
    EVENT_VOLVAR_SAY14 = 9,
    EVENT_VOLVAR_SAY15 = 10,
    EVENT_VOLVAR_SAY16 = 11,

    EVENT_DARION_SAY5 = 12,

    EVENT_THERON_SAY2 = 13,

    EVENT_TYRANDE_SAY2 = 14,
    EVENT_TYRANDE_SAY3 = 15,

    EVENT_GENN_SAY1 = 16,
    EVENT_GENN_SAY2 = 17,
    EVENT_GENN_SAY3 = 18,

    EVENT_PORTAL_TO_MAW = 19,
};

//Shard of Domination 169095 
struct npc_shard_of_domination1 : public ScriptedAI
{
    npc_shard_of_domination1(Creature* creature) : ScriptedAI(creature)
    {
        Reset();
    }

private:
    GameObject* GetPlaceShard() { return me->FindNearestGameObject(352592, 50.0f); }
    ObjectGuid uiBolvorGUID;
    EventMap _events;
    bool EventStert;

    void Reset() override
    {
        me->RemoveNpcFlag(UNIT_NPC_FLAG_SPELLCLICK);
        uiBolvorGUID.Clear();
        EventStert = false;
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
            if (player->IsInDist(me, 30.0f))
            {
                if (player->HasQuest(QUEST_THROUGH_SHATTERED_SKY))
                        if (player->GetQuestObjectiveProgress(QUEST_THROUGH_SHATTERED_SKY, 0) == 1)
                           me->SetNpcFlag(UNIT_NPC_FLAG_SPELLCLICK);
            }
    }

    void OnSpellClick(Unit* clicker, bool spellClickHandled) override
    {
        if (Player* player = clicker->ToPlayer())
        {
            if (player->GetQuestObjectiveProgress(QUEST_THROUGH_SHATTERED_SKY, 0) == 1)
            {
                if (Creature* bolvor = GetClosestCreatureWithEntry(me, 169076, 40.0f))
                    uiBolvorGUID = bolvor->GetGUID();

                _events.ScheduleEvent(EVENT_VOLVAR_SAY6, 1s);
                EventStert = true;
                player->CastSpell(player, 299214, true);
                player->KilledMonsterCredit(169095);
                GetPlaceShard()->RemoveFlag(GO_FLAG_NOT_SELECTABLE);
            }
        }
    }

    void UpdateAI(uint32 diff) override
    {
        _events.Update(diff);

        if (EventStert)
        {
            while (uint32 eventId = _events.ExecuteEvent())
            {
                if (Creature* bolvor = ObjectAccessor::GetCreature(*me, uiBolvorGUID))
                {
                    switch (eventId)
                    {
                    case EVENT_VOLVAR_SAY6:
                    {
                        bolvor->AI()->Talk(6);
                        me->RemoveNpcFlag(UNIT_NPC_FLAG_SPELLCLICK);
                        _events.ScheduleEvent(EVENT_VOLVAR_SAY7, 10s);
                        break;
                    }

                    case EVENT_VOLVAR_SAY7:
                    {
                        bolvor->AI()->Talk(7);
                        _events.ScheduleEvent(EVENT_VOLVAR_SAY8, 3s);
                        break;
                    }

                    case EVENT_VOLVAR_SAY8:
                    {
                        bolvor->AI()->Talk(8);
                        me->KillSelf();
                        Reset();
                        break;
                    }
                    }
                }
            }
        }
    }

};

//Shard of Domination 169098 
struct npc_shard_of_domination2 : public ScriptedAI
{
    npc_shard_of_domination2(Creature* creature) : ScriptedAI(creature)
    {
        Reset();
    }
private:
    GameObject* GetPlaceShard() { return me->FindNearestGameObject(352593, 50.0f); }
    ObjectGuid uiBolvorGUID;
    EventMap _events;
    bool EventStert;

    void Reset() override
    {
        me->RemoveNpcFlag(UNIT_NPC_FLAG_SPELLCLICK);
        uiBolvorGUID.Clear();
        EventStert = false;
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
            if (player->IsInDist(me, 30.0f))
            {
                if (player->HasQuest(QUEST_THROUGH_SHATTERED_SKY))
                {
                    if (player->GetQuestObjectiveProgress(QUEST_THROUGH_SHATTERED_SKY, 1) == 1)
                    {
                        me->SetNpcFlag(UNIT_NPC_FLAG_SPELLCLICK);
                    }
                }
            }
    }

    void OnSpellClick(Unit* clicker, bool /*result*/) override
    {
        if (Player* player = clicker->ToPlayer())
        {
            if (player->HasQuest(QUEST_THROUGH_SHATTERED_SKY)) //GetQuestObjectiveProgress(QUEST_THROUGH_SHATTERED_SKY, 1) == 1)
            {
                if (Creature* bolvor = GetClosestCreatureWithEntry(me, 169076, 30.0f))
                    uiBolvorGUID = bolvor->GetGUID();

                player->CastSpell(player, 299214, true);
                player->KilledMonsterCredit(169098);
                GetPlaceShard()->RemoveFlag(GO_FLAG_NOT_SELECTABLE);
                _events.ScheduleEvent(EVENT_VOLVAR_SAY9, 1s);
                EventStert = true;
            }
        }
    }

    void UpdateAI(uint32 diff) override
    {
        _events.Update(diff);

        if (EventStert)
        {
            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_VOLVAR_SAY9:
                {
                    if (Creature* bolvor = ObjectAccessor::GetCreature(*me, uiBolvorGUID))
                    {
                        bolvor->AI()->Talk(9);
                        bolvor->SetWalk(true);
                        bolvor->GetMotionMaster()->MovePoint(1, 498.624f, -2138.86f, 840.857f, true);
                    }
                     me->RemoveNpcFlag(UNIT_NPC_FLAG_SPELLCLICK);
                    _events.ScheduleEvent(EVENT_VOLVAR_SAY10, 12s);
                    break;
                }

                case EVENT_VOLVAR_SAY10:
                {
                    if (Creature* bolvor = ObjectAccessor::GetCreature(*me, uiBolvorGUID))
                        bolvor->AI()->Talk(10);
                    me->KillSelf();
                    Reset();
                    break;
                }
                }
            }
        }
    }
};

//Shard of Domination 169100
struct npc_shard_of_domination3 : public ScriptedAI
{
    npc_shard_of_domination3(Creature* creature) : ScriptedAI(creature)
    {
        Reset();
    }
private:
    GameObject* GetPlaceShard() { return me->FindNearestGameObject(352594, 50.0f); }
    ObjectGuid uiBolvorGUID;
    ObjectGuid uiDarionGUID;
    EventMap _events;
    bool EventStert;

    void Reset() override
    {
        me->RemoveNpcFlag(UNIT_NPC_FLAG_SPELLCLICK);
        uiBolvorGUID.Clear();
        uiDarionGUID.Clear();
        EventStert = false;
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
            if (player->IsInDist(me, 30.0f))
            {
                if (player->HasQuest(QUEST_THROUGH_SHATTERED_SKY))
                {
                    if (player->GetQuestObjectiveProgress(QUEST_THROUGH_SHATTERED_SKY, 2) == 1)
                    {
                        me->SetNpcFlag(UNIT_NPC_FLAG_SPELLCLICK);
                    }
                }
            }
    }

    void OnSpellClick(Unit* clicker, bool /*result*/)
    {
        if (Player* player = clicker->ToPlayer())
        {
            if (player->HasQuest(QUEST_THROUGH_SHATTERED_SKY)) //GetQuestObjectiveProgress(QUEST_THROUGH_SHATTERED_SKY, 2) == 1)
            {
                if (Creature* bolvor = GetClosestCreatureWithEntry(me, 169076, 30.0f))
                    uiBolvorGUID = bolvor->GetGUID();
                if (Creature* darion = GetClosestCreatureWithEntry(me, 169070, 50.0f))
                    uiDarionGUID = darion->GetGUID();

                player->CastSpell(player, 299214, true);
                player->KilledMonsterCredit(169100);
                GetPlaceShard()->RemoveFlag(GO_FLAG_NOT_SELECTABLE);
                _events.ScheduleEvent(EVENT_VOLVAR_SAY11, 1s);
                EventStert = true;
            }
        }
    }

    void UpdateAI(uint32 diff) override
    {
        _events.Update(diff);

        if (EventStert)
        {
            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_VOLVAR_SAY11:
                {
                    if (Creature* bolvor = ObjectAccessor::GetCreature(*me, uiBolvorGUID))
                    {
                        bolvor->AI()->Talk(11);
                        bolvor->SetWalk(true);
                        bolvor->GetMotionMaster()->MovePoint(2, 509.145f, -2124.31f, 840.857f, true);
                    }
                    me->RemoveNpcFlag(UNIT_NPC_FLAG_SPELLCLICK);
                    _events.ScheduleEvent(EVENT_VOLVAR_SAY12, 9s);
                    break;
                }

                case EVENT_VOLVAR_SAY12:
                {
                    if (Creature* bolvor = ObjectAccessor::GetCreature(*me, uiBolvorGUID))
                        bolvor->AI()->Talk(12);

                    if (Creature* darion = ObjectAccessor::GetCreature(*me, uiDarionGUID))
                    {
                        darion->SetWalk(true);
                        darion->GetMotionMaster()->MovePoint(1, 512.003f, -2123.49f, 840.857f, true);
                    }
                    _events.ScheduleEvent(EVENT_DARION_SAY5, 12s);;
                    break;
                }

                case EVENT_DARION_SAY5:
                {
                    if (Creature* darion = ObjectAccessor::GetCreature(*me, uiDarionGUID))
                    {
                        if (Creature* bolvor = ObjectAccessor::GetCreature(*me, uiBolvorGUID))
                        darion->SetFacingToObject(bolvor);
                        darion->AI()->Talk(5);
                    }
                    me->KillSelf();
                    Reset();
                    break;
                }
                }
            }
        }
    }
};

//Shard of Domination 169101
struct npc_shard_of_domination4 : public ScriptedAI
{
    npc_shard_of_domination4(Creature* creature) : ScriptedAI(creature)
    {
        Reset();
    }
private:
    GameObject* GetPlaceShard() { return me->FindNearestGameObject(352595, 50.0f); }
    ObjectGuid uiBolvorGUID;
    EventMap _events;
    bool EventStert;

    void Reset() override
    {
        me->RemoveNpcFlag(UNIT_NPC_FLAG_SPELLCLICK);
        uiBolvorGUID.Clear();
        EventStert = false;
    }
   
    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
            if (player->IsInDist(me, 30.0f))
            {
                if (player->HasQuest(QUEST_THROUGH_SHATTERED_SKY))
                {
                    if (player->GetQuestObjectiveProgress(QUEST_THROUGH_SHATTERED_SKY, 3) == 1)
                    {
                        me->SetNpcFlag(UNIT_NPC_FLAG_SPELLCLICK);
                    }
                }
            }
    }

    void OnSpellClick(Unit* clicker, bool /*result*/)
    {
        if (Player* player = clicker->ToPlayer())
        {
            if (player->HasQuest(QUEST_THROUGH_SHATTERED_SKY))//GetQuestObjectiveProgress(QUEST_THROUGH_SHATTERED_SKY, 3) == 1)
            {
                if (Creature* bolvor = GetClosestCreatureWithEntry(me, 169076, 30.0f))
                    uiBolvorGUID = bolvor->GetGUID();
               
                player->CastSpell(player, 299214, true);
                player->KilledMonsterCredit(169101);
                GetPlaceShard()->RemoveFlag(GO_FLAG_NOT_SELECTABLE);
                _events.ScheduleEvent(EVENT_VOLVAR_SAY13, 1s);
                EventStert = true;
            }
        }
    }

    void UpdateAI(uint32 diff) override
    {
        _events.Update(diff);

        if (EventStert)
        {
            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_VOLVAR_SAY13:
                {
                    if (Creature* bolvor = ObjectAccessor::GetCreature(*me, uiBolvorGUID))
                    {
                        bolvor->AI()->Talk(13);
                        bolvor->SetWalk(true);
                        bolvor->GetMotionMaster()->MovePoint(3, 529.391f, -2124.92f, 840.857f, true);
                    }
                    me->RemoveNpcFlag(UNIT_NPC_FLAG_SPELLCLICK);
                    _events.ScheduleEvent(EVENT_VOLVAR_SAY14, 12s);
                    break;
                }

                case EVENT_VOLVAR_SAY14:
                {
                    if (Creature* bolvor = ObjectAccessor::GetCreature(*me, uiBolvorGUID))
                    {
                        if (Creature* darion = GetClosestCreatureWithEntry(me, 169070, 50.0f))
                            bolvor->SetFacingToObject(darion);
                        bolvor->AI()->Talk(14);
                    }
                    _events.ScheduleEvent(EVENT_VOLVAR_SAY15, 12s);
                    break;

                case EVENT_VOLVAR_SAY15:
                {
                    if (Creature* bolvor = ObjectAccessor::GetCreature(*me, uiBolvorGUID))
                        bolvor->AI()->Talk(15);
                    me->KillSelf();
                    Reset();
                    break;
                }
                }
                }
            }
        }
    }
};

//Shard of Domination 169109
struct npc_shard_of_domination5 : public ScriptedAI
{
    npc_shard_of_domination5(Creature* creature) : ScriptedAI(creature)
    {
        Reset();
    }

    ObjectGuid uiBolvorGUID;
    ObjectGuid uiTheronGUID;
    ObjectGuid uiTyrandeGUID;
    ObjectGuid uiGennGUID;
    ObjectGuid m_playerGUID;
    EventMap _events;
    bool EventStert;
    Position jump;

    void Reset() override
    {
        me->RemoveNpcFlag(UNIT_NPC_FLAG_SPELLCLICK);
        uiBolvorGUID.Clear();
        uiTheronGUID.Clear();
        uiTyrandeGUID.Clear();
        uiGennGUID.Clear();
        EventStert = false;
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
            if (player->IsInDist(me, 30.0f))
            {
                if (player->HasQuest(QUEST_THROUGH_SHATTERED_SKY))
                {
                    if (player->GetQuestObjectiveProgress(QUEST_THROUGH_SHATTERED_SKY, 4) == 1)
                    {
                        me->SetNpcFlag(UNIT_NPC_FLAG_SPELLCLICK);
                    }
                }
            }
    }

    void OnSpellClick(Unit* clicker, bool /*result*/)
    {
        if (Player* player = clicker->ToPlayer())
        {
            if (player->HasQuest(QUEST_THROUGH_SHATTERED_SKY)) 
            {
                m_playerGUID = player->GetGUID();
                if (Creature* bolvor = GetClosestCreatureWithEntry(me, 169076, 50.0f))
                    uiBolvorGUID = bolvor->GetGUID();
                if (Creature* theron = GetClosestCreatureWithEntry(me, 169073, 50.0f))
                    uiTheronGUID = theron->GetGUID();
                if (Creature* tyrande = GetClosestCreatureWithEntry(me, 169072, 50.0f))
                    uiTyrandeGUID = tyrande->GetGUID();
                if (Creature* genn = GetClosestCreatureWithEntry(me, 169071, 50.0f))
                    uiGennGUID = genn->GetGUID();
                player->KilledMonsterCredit(169109);
                
                EventStert = true;
                _events.ScheduleEvent(EVENT_VOLVAR_SAY16, 1s);
            }
        }
    }

    void UpdateAI(uint32 diff) override
    {
        _events.Update(diff);

        if (EventStert)
        {
            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_VOLVAR_SAY16:
                {
                    std::list<Creature*> cList = me->FindNearestCreatures(169093, 80.0f);
                    for (std::list<Creature*>::const_iterator itr = cList.begin(); itr != cList.end(); ++itr)
                    {
                        if (Creature* ritualist = *itr)
                            ritualist->CastSpell(ritualist, 329476, true);
                    }
               
                    if (Creature* bolvor = ObjectAccessor::GetCreature(*me, uiBolvorGUID))
                    {
                        bolvor->AI()->Talk(16);
                        bolvor->CastSpell(me, 329476, true);
                    }
                    me->RemoveNpcFlag(UNIT_NPC_FLAG_SPELLCLICK);
                    _events.ScheduleEvent(EVENT_THERON_SAY2, 3s);
                    break;
                }

                case EVENT_THERON_SAY2:
                {
                    if (Creature* theron = ObjectAccessor::GetCreature(*me, uiTheronGUID))
                        theron->AI()->Talk(2);
                    _events.ScheduleEvent(EVENT_GENN_SAY1, 6s);
                    break;
                }

                case EVENT_GENN_SAY1:
                {
                    if (Creature* genn = ObjectAccessor::GetCreature(*me, uiGennGUID))
                        genn->AI()->Talk(1);
                    _events.ScheduleEvent(EVENT_TYRANDE_SAY2, 5s);
                    break;
                }

                case EVENT_TYRANDE_SAY2:
                {
                    if (Creature* tyrande = ObjectAccessor::GetCreature(*me, uiTyrandeGUID))
                        tyrande->AI()->Talk(2);
                    _events.ScheduleEvent(EVENT_GENN_SAY2, 5s);
                    break;
                }

                case EVENT_GENN_SAY2:
                {
                    if (Creature* genn = ObjectAccessor::GetCreature(*me, uiGennGUID))
                        genn->AI()->Talk(2);
                    _events.ScheduleEvent(EVENT_TYRANDE_SAY3, 3s);
                    break;
                }

                case EVENT_TYRANDE_SAY3:
                {
                    if (Creature* tyrande = ObjectAccessor::GetCreature(*me, uiTyrandeGUID))
                    {
                        tyrande->AI()->Talk(3);
                        jump = Position(508.561f, -2122.04f, 840.857f, 3.55771f);
                        tyrande->GetMotionMaster()->MoveJump(jump, 15, 7);
                    }
                    _events.ScheduleEvent(EVENT_PORTAL_TO_MAW, 3s);
                    break;
                }

                case EVENT_PORTAL_TO_MAW:
                {
                    if (Creature* darion = GetClosestCreatureWithEntry(me, 169070, 50.0f))
                        darion->DespawnOrUnsummon(5s, 10s);
                    if (Creature* bolvor = ObjectAccessor::GetCreature(*me, uiBolvorGUID))
                        bolvor->DespawnOrUnsummon(5s, 10s);

                    if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                    {
                        player->TeleportTo(2364, 4143.85f, 7874.89f, 4971.02f, 5.679f);
                    }
                }
                }
            }
        }
    }
};

class go_place_shard : public GameObjectScript
{
public:
    go_place_shard() : GameObjectScript("go_place_shard") {}

    GameObjectAI* GetAI(GameObject* go) const override
    {
        return new go_place_shardAI(go);
    }

    struct go_place_shardAI : public GameObjectAI
    {
        go_place_shardAI(GameObject* go) : GameObjectAI(go) { Reset(); }

        void Reset() override
        {
           me->SetFlag(GO_FLAG_NOT_SELECTABLE);
        }

        bool OnGossipHello(Player* player) override
        {
            if (player->HasQuest(QUEST_THROUGH_SHATTERED_SKY))
            {
                player->CastStop();
            }
            return false;
        }

    };
};

class zone_frozen_throne_slintro : public ZoneScript
{
public:
    zone_frozen_throne_slintro() : ZoneScript("zone_frozen_throne_slintro") { }

    void OnPlayerEnter(Player* player)
    {
        player->CastSpell(player, SPELL_SHATTERED_SKY_SKYBOX);

        if (player->GetAreaId() == AREA_THE_FORZEN_THRONE && player->GetLevel() >= 50)
        {
            if (!player->GetPhaseShift().HasPhase(10010))
                PhasingHandler::AddPhase(player, 10010, true);
        }
    }

    void OnPlayerExit(Player* player)
    {
        player->RemoveAura(SPELL_SHATTERED_SKY_SKYBOX);
    }

    void OnPlayerDeath(Player* player)
    {
        player->ResurrectPlayer(0.3f);
        player->TeleportTo(2147, -580.370f, 2210.814f, 544.852f, 0.032f);
    }

};

void AddSC_ShadowlandsIntro()
{
    new player_area_forzen_Throne();
    new npc_highlord_darion_mograine_176554();
    new npc_chilling_summons_commander();
    new player_credit_to_dark_abduction();
    new npc_highlord_darion_mograine_169070();
    new npc_death_gate_to_acherus_171039();
    RegisterCreatureAI(npc_deathbringer_rise_teleport_controller);
    new npc_highlord_bolvar_fordragon_169076();
    RegisterCreatureAI(npc_shard_of_domination1);
    RegisterCreatureAI(npc_shard_of_domination2);
    RegisterCreatureAI(npc_shard_of_domination3);
    RegisterCreatureAI(npc_shard_of_domination4);
    RegisterCreatureAI(npc_shard_of_domination5);
    new go_place_shard();
    RegisterZoneScript(zone_frozen_throne_slintro);
}
