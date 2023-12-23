/*
 * Copyright 2021 Thordekk
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
#include "Player.h"
#include "Creature.h"
#include "CreatureAI.h"
#include "ScriptedEscortAI.h"
#include "ObjectMgr.h"
#include "GameObject.h"
#include "GameObjectAI.h"
#include "ObjectAccessor.h"
#include "PhasingHandler.h"
#include "TemporarySummon.h"
#include "ScriptedGossip.h"

enum TheMaw
{
    QUEST_THE_AFFLICTORS_KEY = 59760,
    QUEST_AN_UNDESERVED_FATE = 59761,
    QUEST_FROM_THE_MOUTH_OF_MADNESS = 59776,
    QUEST_BY_AND_DOWN_THE_RIVER = 59762,
    QUEST_DRAW_OUT_THE_DARKNESS = 60644,
    QUEST_STAND_AS_ONE = 59770,

    QUEST_LINK_TO_THE_MAW = 63661,
    QUEST_MYSTERIES_OF_THE_MAW = 63662,
    QUEST_KORTHIA_THE_CITY_OF_SECRETS = 63663,

    SPELL_SHOCKWAVE = 342875,
    SPELL_FROSTBOLT = 279564,
    SPELL_THROW_AXE = 331012,
    SPELL_RESIDUAL_BLAST = 353397,
    SPELL_CLOAK_OF_VENARI = 308270,
    SPELL_ANIMA_SHROUD = 350924,

    SCENE_BY_AND_DOWN_THE_RIVER      = 327206,    //package 2646_3019
    SCENE_STAND_AS_ONE               = 328173,    //package 2647_2993,
    SCENE_CAMPAIGN_CH2_VENARIFLY     = 351059,    //packageID 3227, sceneID 2721
    SCENE_ENTRY_INTO_KORTHIA         = 350949,    //packageID 3306, sceneID 2719

    NPC_HELYA_165543,
    NPC_VENARI1                      = 162804,
    NPC_VENARI2                      = 177202,
    NPC_MAWSWORN_HOUNDMASTER         = 174826,
    NPC_MAWSWORN_BRUTALIZER          = 177809,
    NPC_SPELL_BUNNY                  = 177640,
    NPC_EYE_OF_THE_JAILER            = 177832,

    ENCOUNTER_HARBINGER_NUMBER = 3,

    GO_TENTACLE = 355354,
    GO_WATER_WALL = 355356,

    //By and Down the River
    EVENT_RIVER_PHASE1 = 100,
    EVENT_RIVER_PHASE2,
    EVENT_RIVER_PHASE3,
    EVENT_RIVER_PHASE4,

    EVENT_STAND_PHASE1 = 200,
    EVENT_STAND_PHASE2,
    EVENT_STAND_PHASE3,
    EVENT_STAND_PHASE4,

    EVENT_VENARI_WAYPOINT_START    = 300,
    EVENT_VENARI_WAYPOINT_DIALOG,
    EVENT_VENARI_WAYPOINT_THEEYE,

    EVENT_VENARI_SUMMON_MAWSWORN   = 400,

    EVENT_RESIDUAL_BLAST_START     = 500,
};

 //167833
class npc_Anduin_Wrynn_167833 : public CreatureScript
{
public:
    npc_Anduin_Wrynn_167833() : CreatureScript("npc_Anduin_Wrynn_167833") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_Anduin_Wrynn_167833AI(creature);
    }

    struct npc_Anduin_Wrynn_167833AI : public EscortAI
    {
        npc_Anduin_Wrynn_167833AI(Creature* creature) : EscortAI(creature) { Reset(); }

        ObjectGuid m_playerGUID;
        bool say59776;
        bool say59762;
        bool scene59762;

        void Reset() override
        {
            m_playerGUID.Clear();
            say59776 = false;
            say59762 = false;
            scene59762 = false;
        }

        void OnQuestAccept(Player* player, Quest const* quest) override
        {
            if (quest->GetQuestId() == QUEST_THE_AFFLICTORS_KEY || quest->GetQuestId() == QUEST_AN_UNDESERVED_FATE)
            {
                if (player->GetPhaseShift().HasPhase(10044)) PhasingHandler::RemovePhase(player, 10044, true);

                if (Creature* thrall = GetClosestCreatureWithEntry(me, 167176, 20.0f))
                    if (Creature* jaina = GetClosestCreatureWithEntry(me, 167154, 20.0f))
                    {
                        thrall->ForcedDespawn();
                        jaina->ForcedDespawn();
                    }
                me->AddDelayedEvent(500, [this, player]()
                    {
                        player->SummonCreature(167154, Position(5359.56f, 7627.21f, 4897.56f, 0.543354f), TEMPSUMMON_TIMED_DESPAWN, 900s);
                        player->SummonCreature(167176, Position(5357.76f, 7631.48f, 4897.81f, 5.95475f), TEMPSUMMON_TIMED_DESPAWN, 900s);
                    });
            }

            if (quest->GetQuestId() == QUEST_FROM_THE_MOUTH_OF_MADNESS)
            {
                if (Creature* jaina = GetClosestCreatureWithEntry(me, 166980, 20.0f))
                    if (Creature* thrall = GetClosestCreatureWithEntry(me, 166981, 20.0f))
                    {
                        jaina->AI()->Talk(37);
                        jaina->SetWalk(true);
                        jaina->GetMotionMaster()->MovePoint(0, 5351.14f, 7619.03f, 4897.45f, true);
                        player->GetScheduler().Schedule(Milliseconds(8000), [this, jaina](TaskContext context)
                            {
                                me->SetWalk(true);
                                me->GetMotionMaster()->MovePoint(0, 5352.14f, 7618.03f, 4897.45f, true);
                                jaina->CastSpell(jaina, 329892);

                            }).Schedule(Milliseconds(10000), [this, jaina, thrall, player](TaskContext context)
                                {
                                    if (!player->GetPhaseShift().HasPhase(10045)) PhasingHandler::AddPhase(player, 10045, true);
                                    thrall->SetWalk(true);
                                    thrall->GetMotionMaster()->MovePoint(0, 5354.14f, 7620.03f, 4897.45f, true);
                                }).Schedule(Milliseconds(12000), [this, jaina](TaskContext context)
                                    {
                                        jaina->CastSpell(jaina, 325412);
                                        jaina->ForcedDespawn(1000);
                                    }).Schedule(Milliseconds(15000), [this, thrall](TaskContext context)
                                        {
                                            thrall->CastSpell(thrall, 325412);
                                            thrall->ForcedDespawn(1000);
                                            me->CastSpell(me, 325412);
                                            me->ForcedDespawn(1000, 60s);
                                        });
                                    say59776 = false;
                    }

            }

            if (quest->GetQuestId() == QUEST_STAND_AS_ONE)
            {
                if (Creature* jaina = GetClosestCreatureWithEntry(me, 166980, 20.0f))
                {
                    jaina->AI()->Talk(51);
                    player->GetScheduler().Schedule(7s, [this, jaina](TaskContext context)
                        {
                            jaina->AI()->Talk(52);
                        });
                }
            }

        }

        void MoveInLineOfSight(Unit* target) override
        {
            if (Player* player = target->ToPlayer())
            {
                if (player->GetDistance2d(me) <= 5.0f)
                {
                    if (!say59776)
                    {
                        if (player->GetQuestObjectiveProgress(QUEST_FROM_THE_MOUTH_OF_MADNESS, 1))
                        {
                           if(me->GetPhaseShift().HasPhase(10045))
                            if (Creature* thrall = GetClosestCreatureWithEntry(me, 166981, 10.0f))
                                if (Creature* jaina = GetClosestCreatureWithEntry(me, 166980, 10.0f))
                                {
                                    jaina->CastSpell(jaina, 329892);
                                    player->GetScheduler().Schedule(Milliseconds(3000), [this, player, jaina, thrall](TaskContext context)
                                        {
                                            if (player->GetPhaseShift().HasPhase(10044)) PhasingHandler::RemovePhase(player, 10044, true);
                                            jaina->CastStop();
                                            jaina->GetMotionMaster()->MovePoint(0, 4710.85f, 7661.53f, 4772.86f, true);
                                            me->AI()->Talk(2);
                                            me->CastSpell(thrall, 342797);
                                        }).Schedule(Milliseconds(9000), [this, thrall](TaskContext context)
                                            {
                                                thrall->AI()->Talk(8);
                                            }).Schedule(Milliseconds(15000), [this, thrall](TaskContext context)
                                                {
                                                    me->GetMotionMaster()->MovePoint(0, 4705.44f, 7657.02f, 4772.66f, true);
                                                    thrall->GetMotionMaster()->MovePoint(0, 4712.64f, 7665.03f, 4773.44f, true);
                                                });
                                }
                        }
                    }
                    say59776 = true;
                }

                if (player->GetDistance2d(me) <= 20.0f)
                {
                    m_playerGUID = player->GetGUID();
                    if (!say59762)
                    {
                        if (player->GetQuestObjectiveProgress(QUEST_BY_AND_DOWN_THE_RIVER, 0) && !player->GetQuestObjectiveProgress(QUEST_BY_AND_DOWN_THE_RIVER, 2))
                        {
                            if (Creature* thrall = GetClosestCreatureWithEntry(me, 166981, 30.0f))
                                if (Creature* jaina = GetClosestCreatureWithEntry(me, 166980, 30.0f))
                                {
                                    jaina->AI()->Talk(39);
                                    me->AddDelayedEvent(4000, [this, thrall]()
                                        {
                                            thrall->AI()->Talk(9);
                                        });
                                    me->AddDelayedEvent(8000, [this]()
                                        {
                                            me->AI()->Talk(3);
                                        });
                                    me->AddDelayedEvent(10000, [this, thrall, jaina]()
                                        {
                                            thrall->GetMotionMaster()->MoveFollow(me, PET_FOLLOW_DIST, 60);
                                            jaina->GetMotionMaster()->MoveFollow(me, PET_FOLLOW_DIST, 180);
                                            me->GetMotionMaster()->MovePoint(0, 4647.81f, 7453.61f, 4786.61f, true);
                                        });
                                    me->AddDelayedEvent(17000, [this, player]()
                                        {
                                            me->GetMotionMaster()->MovePoint(0, 4605.62f, 7454.01f, 4787.33f, true);
                                        });
                                    me->AddDelayedEvent(23000, [this, thrall, jaina]()
                                        {
                                            if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                                            {
                                                player->KilledMonsterCredit(171646);
                                                thrall->ForcedDespawn(1000, 60s);
                                                jaina->ForcedDespawn(1000, 60s);
                                                me->ForcedDespawn(1000, 60s);
                                            }
                                        });
                                }
                        }
                        say59762 = true;
                    }
                }

                if (player->GetDistance2d(me) <= 5.0f)
                {
                    m_playerGUID = player->GetGUID();
                    if (!scene59762)
                    {
                        if (player->GetQuestStatus(QUEST_BY_AND_DOWN_THE_RIVER) == QUEST_STATUS_COMPLETE)
                        {
                            if (Creature* thrall = GetClosestCreatureWithEntry(me, 166981, 10.0f))
                                if (Creature* jaina = GetClosestCreatureWithEntry(me, 166980, 15.0f))
                                {
                                   thrall->AI()->Talk(10);
                                    me->AddDelayedEvent(6000, [this, jaina]()
                                        {
                                            jaina->AI()->Talk(40);
                                        });
                                    me->AddDelayedEvent(9000, [this]()
                                        {
                                            me->AI()->Talk(4);
                                        });
                                    me->AddDelayedEvent(12000, [this, thrall, jaina]()
                                        {
                                            if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                                            {
                                                player->CastSpell(player, SCENE_BY_AND_DOWN_THE_RIVER, true);
                                            }
                                        });
                                }
                        }
                        scene59762 = true;
                    }
                }
                
            }
        }
    };
};

//167176_npc_thrall
class npc_thrall_167176 : public CreatureScript
{
public:
    npc_thrall_167176() : CreatureScript("npc_thrall_167176") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_thrall_167176AI(creature);
    }
    struct npc_thrall_167176AI : public EscortAI
    {
        npc_thrall_167176AI(Creature* creature) : EscortAI(creature) { Reset(); }

    private:
        EventMap _events;

        void Reset() override
        {
            _events.Reset();
            me->SetReactState(REACT_AGGRESSIVE);
        }


        void IsSummonedBy(WorldObject* unit) override
        {
            if (Player* player = unit->ToPlayer())
            {
                me->SetOwnerGUID(player->GetGUID());                   //set player onwer
                me->GetMotionMaster()->MoveFollow(player, PET_FOLLOW_DIST, 60.f);
                if (int32 playerFaction = player->GetFaction())
                    me->SetFaction(playerFaction);

                if (int32 playerAttackPower = player->GetTotalAttackPowerValue(BASE_ATTACK) * 2.0f)
                    me->SetAttackPower(playerAttackPower);
            }
        }

        void MoveInLineOfSight(Unit* target) override
        {
            Position _positon = target->GetPosition();

            if (Player* player = target->ToPlayer())
            {
                if (target->GetDistance2d(me) > 20.0f)
                    me->UpdatePosition(_positon, false);
            }

            if (Creature* creature = target->ToCreature())
            {
                if (creature->IsInDist(me, 15.0f))
                {
                    if (creature->IsAlive())
                        if (creature->IsHostileToPlayers())
                        {
                            me->GetThreatManager().AddThreat(creature, 1000.0f);
                            creature->GetThreatManager().AddThreat(me, 1000.0f);
                            me->Attack(creature, true);
                        }
                }
            }
        }

        void JustEngagedWith(Unit* victim) override
        {
            _events.ScheduleEvent(SPELL_SHOCKWAVE, 1s);
        }

        void UpdateAI(uint32 diff) override
        {

            if (!UpdateVictim())
                return;

            if (!me->GetOwner())
                me->ForcedDespawn();

            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case SPELL_SHOCKWAVE:
                {
                    me->CastSpell(me, SPELL_SHOCKWAVE, true);
                    _events.Repeat(15s);
                    break;
                }
                }
            }
            DoMeleeAttackIfReady();
        }

    };
};

//167154
class npc_lady_jaina_proudmoore_167154 : public CreatureScript
{
public:
    npc_lady_jaina_proudmoore_167154() : CreatureScript("npc_lady_jaina_proudmoore_167154") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_lady_jaina_proudmoore_167154AI(creature);
    }
    struct npc_lady_jaina_proudmoore_167154AI : public EscortAI
    {
        npc_lady_jaina_proudmoore_167154AI(Creature* creature) : EscortAI(creature) { Reset(); }


    private:
        EventMap _events;

        void Reset() override
        {
            _events.Reset();
            me->SetReactState(REACT_AGGRESSIVE);
        }

        void JustEngagedWith(Unit* victim) override
        {
            _events.ScheduleEvent(SPELL_FROSTBOLT, 1s);
        }

        void IsSummonedBy(WorldObject* unit) override
        {
            if (Player* player = unit->ToPlayer())
            {
                me->SetOwnerGUID(player->GetGUID());                   //set player onwer
                me->GetMotionMaster()->MoveFollow(player, 2.0f, 90.f);
                if (int32 playerFaction = player->GetFaction())
                    me->SetFaction(playerFaction);

                if (int32 playerAttackPower = player->GetTotalAttackPowerValue(BASE_ATTACK) * 2.0f)
                    me->SetAttackPower(playerAttackPower);
            }
        }

        void MoveInLineOfSight(Unit* target) override
        {
            Position _positon = target->GetPosition();

            if (Player* player = target->ToPlayer())
            {
                if (target->GetDistance2d(me) > 20.0f)
                    me->UpdatePosition(_positon, false);
            }

            if (Creature* creature = target->ToCreature())
            {
                if (creature->IsInDist(me, 15.0f))
                {
                    if (creature->IsAlive())
                        if (creature->IsHostileToPlayers())
                        {
                            me->GetThreatManager().AddThreat(creature, 1000.0f);
                            creature->GetThreatManager().AddThreat(me, 1000.0f);
                            me->Attack(creature, true);
                        }
                }
            }

        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            if (!me->GetOwner())
                me->ForcedDespawn();

            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case SPELL_FROSTBOLT:
                {
                    if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0))
                        if (me->IsWithinCombatRange(target, 40.f))
                            me->CastSpell(target, SPELL_FROSTBOLT, true);
                    _events.Repeat(15s);
                    break;
                }
                }
            }
            DoMeleeAttackIfReady();
        }
    };
};

//167906_npc_anduin
struct npc_Anduin_Wrynn_167906 : public EscortAI
{
    npc_Anduin_Wrynn_167906(Creature* creature) : EscortAI(creature) { Reset(); }

private:
    EventMap _events;

    void Reset() override
    {
        _events.Reset();
        me->SetReactState(REACT_AGGRESSIVE);
    }


    void IsSummonedBy(WorldObject* unit) override
    {
        if (Player* player = unit->ToPlayer())
        {
            me->SetOwnerGUID(player->GetGUID());                   //set player onwer
            me->GetMotionMaster()->MoveFollow(player, PET_FOLLOW_DIST,70.f);
            if (int32 playerFaction = player->GetFaction())
                me->SetFaction(playerFaction);

            if (int32 playerAttackPower = player->GetTotalAttackPowerValue(BASE_ATTACK) * 2.0f)
                me->SetAttackPower(playerAttackPower);
        }
    }

    void MoveInLineOfSight(Unit* target) override
    {
        Position _positon = target->GetPosition();

        if (Player* player = target->ToPlayer())
        {
            if (target->GetDistance2d(me) > 20.0f)
                me->UpdatePosition(_positon, false);
        }

        if (Creature* creature = target->ToCreature())
        {
            if (creature->IsInDist(me, 15.0f))
            {
                if (creature->IsAlive())
                    if (creature->IsHostileToPlayers())
                    {
                        me->GetThreatManager().AddThreat(creature, 1000.0f);
                        creature->GetThreatManager().AddThreat(me, 1000.0f);
                        me->Attack(creature, true);
                    }
            }
        }
    }
};

//165862_Ebon_Blade
struct npc_Ebon_Blade_guid345873 : public ScriptedAI
{
    npc_Ebon_Blade_guid345873(Creature* creature) : ScriptedAI(creature) { Reset(); }

    bool say59762;

    void Reset() override
    {
        say59762 = false;
    }

    void MoveInLineOfSight(Unit* target) override
    {
        if (Player* player = target->ToPlayer())
            if (target->GetDistance2d(me) <= 20.0f)
                if (!player->GetQuestObjectiveProgress(QUEST_BY_AND_DOWN_THE_RIVER, 0))
                {
                    if (Creature* thrall = GetClosestCreatureWithEntry(me, 167176, 30.0f))
                        if (Creature* jaina = GetClosestCreatureWithEntry(me, 167154, 30.0f))
                            if (!say59762)
                            {
                                jaina->AI()->Talk(1);
                                me->AddDelayedEvent(5000, [this, thrall]()
                                    {
                                        thrall->AI()->Talk(1);
                                    });
                                me->AddDelayedEvent(12000, [this, jaina]()
                                    {
                                        jaina->AI()->Talk(2);
                                    });
                                me->AddDelayedEvent(21000, [this, jaina]()
                                    {
                                        jaina->AI()->Talk(3);
                                    });
                                say59762 = true;
                            }

                }
    }
};


//165543_npc_helya
struct npc_helya_165543 : public ScriptedAI
{
    npc_helya_165543(Creature* creature) : ScriptedAI(creature) { Reset(); }

    EventMap _events;
    ObjectGuid m_playerGUID;
    bool say59762;

    void Reset() override
    {
        _events.Reset();
        say59762 = false;
    }

    void MoveInLineOfSight(Unit* target) override
    {
        if (Player* player = target->ToPlayer())
        {
            if (target->GetDistance2d(me) <= 35.0f)
                if (!player->GetQuestObjectiveProgress(QUEST_BY_AND_DOWN_THE_RIVER, 0) && !player->GetQuestObjectiveProgress(QUEST_BY_AND_DOWN_THE_RIVER, 2))
                {
                    m_playerGUID = player->GetGUID();
                    player->KilledMonsterCredit(166980);
                    if (Creature* thrall = GetClosestCreatureWithEntry(me, 167176, 50.0f))
                        if (Creature* jaina = GetClosestCreatureWithEntry(me, 167154, 50.0f))
                            if (Creature* anduin = GetClosestCreatureWithEntry(me, 167906, 50.0f))
                            {
                                thrall->GetMotionMaster()->MovePoint(0, 4721.98f, 7416.36f, 4817.71f, true, 5.9845f);
                                jaina->GetMotionMaster()->MovePoint(0, 4719.93f, 7426.33f, 4817.71f, true, 5.8242f);
                                anduin->GetMotionMaster()->MovePoint(0, 4728.49f, 7429.68f, 4817.71f, true, 4.9682f);
                                if (!say59762)
                                {
                                    me->AddDelayedEvent(5000, [this, thrall]()
                                        {
                                            thrall->AI()->Talk(2);
                                        });
                                    me->AddDelayedEvent(11000, [this, thrall]()
                                        {
                                            me->SetAIAnimKitId(0);
                                            std::list<GameObject*> oList = me->FindNearestGameObjects(GO_WATER_WALL, 80.0f);
                                            for (std::list<GameObject*>::const_iterator itr = oList.begin(); itr != oList.end(); ++itr)
                                            {
                                                if (GameObject* waterWall = *itr)
                                                {
                                                    waterWall->SetGoState(GO_STATE_READY);
                                                }
                                            }

                                            std::list<GameObject*> tenList = me->FindNearestGameObjects(GO_TENTACLE, 80.0f);
                                            for (std::list<GameObject*>::const_iterator itr = tenList.begin(); itr != tenList.end(); ++itr)
                                            {
                                                if (GameObject* tentacle = *itr)
                                                {
                                                    tentacle->SetAnimKitId(1004, false);
                                                }
                                            }
                                            _events.ScheduleEvent(EVENT_RIVER_PHASE1, 3s);
                                        });
                                    say59762 = true;
                                }
                            }
                }
        }
    }

    void UpdateAI(uint32 diff) override
    {
        _events.Update(diff);

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_RIVER_PHASE1:
                me->AI()->Talk(0);
                _events.ScheduleEvent(EVENT_RIVER_PHASE2, 6s);
                break;

            case EVENT_RIVER_PHASE2:
                if (Creature* jaina = GetClosestCreatureWithEntry(me, 167154, 50.0f))
                    jaina->AI()->Talk(4);
                me->AddDelayedEvent(8000, [this]()
                    {
                        me->AI()->Talk(1);
                    });
                    _events.ScheduleEvent(EVENT_RIVER_PHASE3, 18s);
                break;

            case EVENT_RIVER_PHASE3:
                if (Creature* thrall = GetClosestCreatureWithEntry(me, 167176, 50.0f))
                {
                    thrall->AI()->Talk(3);
                    me->AddDelayedEvent(2000, [this, thrall]()
                        {
                            thrall->CastSpell(me, SPELL_THROW_AXE);
                        });
                    me->AddDelayedEvent(5000, [this, thrall]()
                        {
                            me->AI()->Talk(2);
                        });
                }
                _events.ScheduleEvent(EVENT_RIVER_PHASE4, 7s);
                break;

            case EVENT_RIVER_PHASE4:
                if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                {
                    if (Creature* thrall = GetClosestCreatureWithEntry(me, 167176, 50.0f))
                    {
                        thrall->GetMotionMaster()->MoveJump(4694.79f, 7419.66f, 4822.58f, 5.8211f, 19.2911f, 19.2911f);
                        thrall->ForcedDespawn(3000);
                    }
                    if (Creature* jaina = GetClosestCreatureWithEntry(me, 167154, 50.0f))
                    {
                        jaina->GetMotionMaster()->MoveJump(4707.11f, 7437.51f, 4826.29f, 5.8211f, 19.2911f, 19.2911f);
                        jaina->ForcedDespawn(3000);
                    }
                    if (Creature* anduin = GetClosestCreatureWithEntry(me, 167906, 50.0f))
                    {
                        anduin->GetMotionMaster()->MoveJump(4707.01f, 7442.91f, 4826.29f, 5.8211f, 19.2911f, 19.2911f);
                        anduin->ForcedDespawn(3000);
                    }
                    player->GetMotionMaster()->MoveJump(4703.11f, 7431.51f, 4826.29f, 5.8211f, 19.2911f, 19.2911f);

                    me->AddDelayedEvent(4000, [this]()
                        {
                            me->SetAIAnimKitId(1455);
                            std::list<GameObject*> oList = me->FindNearestGameObjects(GO_WATER_WALL, 80.0f);
                            for (std::list<GameObject*>::const_iterator itr = oList.begin(); itr != oList.end(); ++itr)
                            {
                                if (GameObject* waterWall = *itr)
                                {
                                    waterWall->SetGoState(GO_STATE_ACTIVE);
                                }
                            }

                            std::list<GameObject*> tenList = me->FindNearestGameObjects(GO_TENTACLE, 80.0f);
                            for (std::list<GameObject*>::const_iterator itr = tenList.begin(); itr != tenList.end(); ++itr)
                            {
                                if (GameObject* tentacle = *itr)
                                {
                                    tentacle->SetAnimKitId(1455, false);
                                }
                            }
                            say59762 = false;
                        });
                }
                break;

            }
        }
    }
};

//168162_Baine_Bloodhoof
class npc_Baine_Bloodhoof_168162 : public CreatureScript
{
public:
    npc_Baine_Bloodhoof_168162() : CreatureScript("npc_Baine_Bloodhoof_168162") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_Baine_Bloodhoof_168162AI(creature);
    }
    struct npc_Baine_Bloodhoof_168162AI : public ScriptedAI
    {
        npc_Baine_Bloodhoof_168162AI(Creature* creature) : ScriptedAI(creature) { Reset(); }

    private:
        bool say59762;
        ObjectGuid thrallGUID;
        ObjectGuid anduinGUID;

        void Reset() override
        {
            say59762 = false;
            thrallGUID.Clear();
            anduinGUID.Clear();
        }

        void OnQuestReward(Player* player, Quest const* quest, LootItemType /*type*/, uint32 /*opt*/) override
        {
            if (quest->GetQuestId() == QUEST_DRAW_OUT_THE_DARKNESS)
            {
                if (Creature* jaina = GetClosestCreatureWithEntry(player, 166980, 15.0f))
                    jaina->AI()->Talk(42);
                player->GetScheduler().Schedule(7s, [this](TaskContext context)
                    {
                        me->AI()->Talk(2);
                    }).Schedule(12s, [this, player](TaskContext context)
                        {
                            if (player->GetPhaseShift().HasPhase(10042)) PhasingHandler::RemovePhase(player, 10042, true);
                            if (!player->GetPhaseShift().HasPhase(10043)) PhasingHandler::AddPhase(player, 10043, true);
                        });
            }
        }

        void MoveInLineOfSight(Unit* target) override
        {
            if (Player* player = target->ToPlayer())
            {
                if (player->GetDistance2d(me) <= 20.0f)
                {
                    if (Creature* thrall = GetClosestCreatureWithEntry(player, 166981, 20.0f))
                        thrallGUID = thrall->GetGUID();
                    if (Creature* anduin = GetClosestCreatureWithEntry(player, 167833, 20.0f))
                        anduinGUID = anduin->GetGUID();
                    if (!say59762)
                    {
                        if (player->GetQuestStatus(QUEST_BY_AND_DOWN_THE_RIVER) == QUEST_STATUS_COMPLETE)
                        {
                            me->AddDelayedEvent(4000, [this]()
                                {
                                    if (Creature* anduin = ObjectAccessor::GetCreature(*me, anduinGUID))
                                    {
                                        anduin->AI()->Talk(5);
                                    }
                                });
                            me->AddDelayedEvent(11000, [this]()
                                {
                                    if (Creature* thrall = ObjectAccessor::GetCreature(*me, thrallGUID))
                                    {
                                        thrall->AI()->Talk(11);
                                    }
                                });
                        }
                    }
                    say59762 = true;
                }
            }
        }
    };
};

//Place Blade 353170
struct go_place_blade_353170 : public GameObjectAI
{
    go_place_blade_353170(GameObject* go) : GameObjectAI(go) {}

    bool OnGossipHello(Player* player) override
    {
        if (player->GetQuestStatus(QUEST_DRAW_OUT_THE_DARKNESS) == QUEST_STATUS_INCOMPLETE)
        {
            me->SummonCreature(169759, Position(4521.91f, 7447.32f, 4794.35f, 4.22683f), TEMPSUMMON_TIMED_DESPAWN, 60s);
            me->DespawnOrUnsummon(3s, 60s);
        }
        return true;
    }
};

//169759
struct npc_withering_presence : public ScriptedAI
{
    npc_withering_presence(Creature* creature) : ScriptedAI(creature) { Reset(); }

private:
    ObjectGuid jainaGUID;
    ObjectGuid anduinGUID;
    ObjectGuid thrallGUID;
    ObjectGuid baineGUID;
    bool say60644;

    void Reset() override
    {
        jainaGUID.Clear();
        anduinGUID.Clear();
        thrallGUID.Clear();
        baineGUID.Clear();
        say60644 = false;
    }

    void JustEngagedWith(Unit* victim) override
    {
        if (Player* player = victim->ToPlayer())
        {
            if (!me->GetPhaseShift().HasPhase(10042)) PhasingHandler::AddPhase(me, 10042, true);

            if (Creature* jaina = me->FindNearestCreature(166980, 15.0f))
                jainaGUID = jaina->GetGUID();
            if (Creature* thrall = me->FindNearestCreature(166981, 15.0f))
            {
                thrallGUID = thrall->GetGUID();
                thrall->SetFaction(35);
                thrall->SetUnitFlag(UNIT_FLAG_IMMUNE_TO_NPC);
                thrall->AI()->Talk(23);
            }
            if (Creature* anduin = me->FindNearestCreature(167833, 15.0f))
            {
                anduinGUID = anduin->GetGUID();
                me->AddDelayedEvent(6000, [this, anduin]()
                    {
                        anduin->AI()->Talk(6);
                    });
                me->AddDelayedEvent(10000, [this, anduin]()
                    {
                        anduin->AI()->Talk(7);
                    });

            }
            if (Creature* baine = me->FindNearestCreature(168162, 15.0f))
                baineGUID = baine->GetGUID();
        }
    }

    void DamageTaken(Unit* target, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/) override
    {
        if (target->ToPlayer())
            if (me->GetHealthPct() <= 30.0f)
                if (Creature* jaina = ObjectAccessor::GetCreature(*me, jainaGUID))
                    if (!say60644)
                    {
                        jaina->AI()->Talk(41);
                        say60644 = true;
                    }
    }

    void JustDied(Unit* killer) override
    {
        if (Player* player = killer->ToPlayer())
        {
            if (Creature* anduin = ObjectAccessor::GetCreature(*me, anduinGUID))
                if (Creature* baine = ObjectAccessor::GetCreature(*me, baineGUID))
                    if (Creature* thrall = ObjectAccessor::GetCreature(*me, thrallGUID))
                        if (Creature* jaina = ObjectAccessor::GetCreature(*me, jainaGUID))
                        {
                            anduin->AI()->Talk(8);
                            me->AddDelayedEvent(4000, [this, anduin]()
                                {
                                    anduin->AI()->Talk(9);
                                });
                            me->AddDelayedEvent(8000, [this, baine]()
                                {
                                    baine->AI()->Talk(0);
                                });
                            me->AddDelayedEvent(14000, [this, thrall, jaina, anduin, baine]()
                                {
                                    baine->SetAIAnimKitId(0);
                                    jaina->SetAIAnimKitId(0);
                                    anduin->SetAIAnimKitId(0);
                                    thrall->SetStandState(UNIT_STAND_STATE_STAND);
                                    thrall->AI()->Talk(24);
                                });
                            me->AddDelayedEvent(20000, [this, baine, player]()
                                {
                                    baine->SetFacingToObject(player);
                                    baine->AI()->Talk(1, player);
                                });
                        }
            if (player->HasQuest(QUEST_DRAW_OUT_THE_DARKNESS))
            {
                player->ForceCompleteQuest(QUEST_DRAW_OUT_THE_DARKNESS);
            }
        }
    }
};


//168478 Eroded Waystone
struct npc_eroded_waystone_168478 : public ScriptedAI
{
    npc_eroded_waystone_168478(Creature* creature) : ScriptedAI(creature) { Reset(); }

private:
    ObjectGuid m_playerGUID;
    ObjectGuid anduinGUID;
    ObjectGuid baineGUID;
    ObjectGuid thrallGUID;
    ObjectGuid jainaGUID;
    ObjectGuid harbingerGUID[ENCOUNTER_HARBINGER_NUMBER];
    EventMap _events;

    void Reset() override
    {
        m_playerGUID.Clear();
        anduinGUID.Clear();
        baineGUID.Clear();
        thrallGUID.Clear();
        jainaGUID.Clear();
        _events.Reset();

        for (uint8 i = 0; i < ENCOUNTER_HARBINGER_NUMBER; ++i)
        {
            if (Creature* temp = ObjectAccessor::GetCreature(*me, harbingerGUID[i]))
                temp->setDeathState(JUST_DIED);
            harbingerGUID[i].Clear();
        }
    }

    void OnQuestReward(Player* player, Quest const* quest, LootItemType /*type*/, uint32 /*opt*/) override
    {
        if (quest->GetQuestId() == QUEST_STAND_AS_ONE)
        {
            if (Creature* jaina = GetClosestCreatureWithEntry(me, 166980, 50.0f))
                jaina->AI()->Talk(56);
            player->GetScheduler().Schedule(Milliseconds(7000), [this, player](TaskContext context)
                {
                    std::list<Creature*> cList = me->FindNearestCreatures(168585, 50.0f);
                    for (std::list<Creature*>::const_iterator itr = cList.begin(); itr != cList.end(); ++itr)
                    {
                        if (Creature* harbringer = *itr)
                        {
                            harbringer->ForcedDespawn(1000);
                        }
                    }
                    std::list<Creature*> cList1 = me->FindNearestCreatures(168586, 50.0f);
                    for (std::list<Creature*>::const_iterator itr = cList1.begin(); itr != cList1.end(); ++itr)
                    {
                        if (Creature* guard = *itr)
                        {
                            guard->ForcedDespawn(1000);
                        }
                    }
                    std::list<Creature*> cList2 = me->FindNearestCreatures(168588, 50.0f);
                    for (std::list<Creature*>::const_iterator itr = cList2.begin(); itr != cList2.end(); ++itr)
                    {
                        if (Creature* sorcerer = *itr)
                        {
                            sorcerer->ForcedDespawn(1000);
                        }
                    }
                    if (Creature* jaina = ObjectAccessor::GetCreature(*me, jainaGUID))
                        if (Creature* thrall = ObjectAccessor::GetCreature(*me, thrallGUID))
                            if (Creature* anduin = ObjectAccessor::GetCreature(*me, anduinGUID))
                                if (Creature* baine = ObjectAccessor::GetCreature(*me, baineGUID))
                                {
                                    jaina->GetMotionMaster()->MoveTargetedHome();
                                    thrall->GetMotionMaster()->MoveTargetedHome();
                                    anduin->GetMotionMaster()->MoveTargetedHome();
                                    baine->GetMotionMaster()->MoveTargetedHome();
                                }
                    player->CastSpell(player, SCENE_STAND_AS_ONE, true);
                    me->RemoveAura(327871);
                    me->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
                    me->RemoveNpcFlag(UNIT_NPC_FLAG_QUESTGIVER);
                    me->SetNpcFlag(UNIT_NPC_FLAG_SPELLCLICK);
                });
        }
    }
    

    void OnSpellClick(Unit* clicker, bool /*result*/) override
    {
        if (Player* player = clicker->ToPlayer())
        {
            if (player->GetQuestStatus(QUEST_STAND_AS_ONE) == QUEST_STATUS_INCOMPLETE)
                if (!player->GetQuestObjectiveProgress(QUEST_STAND_AS_ONE, 0) && (!player->GetQuestObjectiveProgress(QUEST_STAND_AS_ONE, 1)))
                {
                    me->CastSpell(me, 327871);
                    m_playerGUID = player->GetGUID();
                    if (Creature* anduin = GetClosestCreatureWithEntry(me, 167833, 20.0f))
                        anduinGUID = anduin->GetGUID();
                    if (Creature* baine = GetClosestCreatureWithEntry(me, 168162, 20.0f))
                        baineGUID = baine->GetGUID();
                    if (Creature* thrall = GetClosestCreatureWithEntry(me, 166981, 20.0f))
                        thrallGUID = thrall->GetGUID();
                    if (Creature* jaina = GetClosestCreatureWithEntry(me, 166980, 20.0f))
                        jainaGUID = jaina->GetGUID();

                    if (player->GetQuestStatus(QUEST_STAND_AS_ONE) == QUEST_STATUS_INCOMPLETE)
                    {
                        _events.ScheduleEvent(EVENT_STAND_PHASE1, 3s);
                    }
                }
        }
    }

    void UpdateAI(uint32 diff) override
    {
        _events.Update(diff);

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_STAND_PHASE1:
                if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                {
                    if (Creature* jaina = ObjectAccessor::GetCreature(*me, jainaGUID))
                        if (Creature* thrall = ObjectAccessor::GetCreature(*me, thrallGUID))
                            if (Creature* anduin = ObjectAccessor::GetCreature(*me, anduinGUID))
                                if (Creature* baine = ObjectAccessor::GetCreature(*me, baineGUID))
                                {
                                    if (int32 playerFaction = player->GetFaction())
                                    {
                                        jaina->SetFaction(playerFaction);
                                        jaina->RemoveUnitFlag(UNIT_FLAG_IMMUNE_TO_PC);
                                        jaina->RemoveUnitFlag(UNIT_FLAG_IMMUNE_TO_NPC);
                                        jaina->GetMotionMaster()->MovePoint(0, 4551.43f, 6898.67f, 4867.24f, false, 3.7543f);
                                        thrall->SetFaction(playerFaction);
                                        thrall->RemoveUnitFlag(UNIT_FLAG_IMMUNE_TO_PC);
                                        thrall->RemoveUnitFlag(UNIT_FLAG_IMMUNE_TO_NPC);
                                        thrall->GetMotionMaster()->MovePoint(0, 4578.83f, 6893.12f, 4867.24f, false, 4.6967f);
                                        anduin->SetFaction(playerFaction);
                                        anduin->RemoveUnitFlag(UNIT_FLAG_IMMUNE_TO_PC);
                                        anduin->RemoveUnitFlag(UNIT_FLAG_IMMUNE_TO_NPC);
                                        anduin->GetMotionMaster()->MovePoint(0, 4566.94f, 6897.89f, 4867.24f, false, 4.1666f);
                                        baine->SetFaction(playerFaction);
                                        baine->RemoveUnitFlag(UNIT_FLAG_IMMUNE_TO_PC);
                                        baine->RemoveUnitFlag(UNIT_FLAG_IMMUNE_TO_NPC);
                                        baine->GetMotionMaster()->MovePoint(0, 4560.06f, 6896.54f, 4867.24f, false, 3.4748f);
                                    }

                                    if (player->GetQuestStatus(QUEST_STAND_AS_ONE) == QUEST_STATUS_INCOMPLETE)
                                    {
                                        player->GetScheduler().Schedule(Milliseconds(3000), [this, jaina, player](TaskContext context)
                                            {
                                                if (!player->GetQuestObjectiveProgress(QUEST_STAND_AS_ONE, 0) && (!player->GetQuestObjectiveProgress(QUEST_STAND_AS_ONE, 1)))
                                                {
                                                    SpawnHarbinger(jaina);
                                                    for (uint8 i = 0; i < ENCOUNTER_HARBINGER_NUMBER; ++i)
                                                    {
                                                        if (Creature* temp = ObjectAccessor::GetCreature(*me, harbingerGUID[i]))
                                                            harbingerGUID[i].Clear();
                                                    }
                                                    context.Repeat(50s);
                                                }
                                                else if (player->GetQuestObjectiveProgress(QUEST_STAND_AS_ONE, 0) && (!player->GetQuestObjectiveProgress(QUEST_STAND_AS_ONE, 1)))
                                                    return;
                                            }).Schedule(Milliseconds(4000), [this, thrall](TaskContext context)
                                                {
                                                    SpawnHarbinger(thrall);
                                                    for (uint8 i = 0; i < ENCOUNTER_HARBINGER_NUMBER; ++i)
                                                    {
                                                        if (Creature* temp = ObjectAccessor::GetCreature(*me, harbingerGUID[i]))
                                                            harbingerGUID[i].Clear();
                                                    }
                                                }).Schedule(Milliseconds(5000), [this, anduin, player](TaskContext context)
                                                    {
                                                        if (!player->GetQuestObjectiveProgress(QUEST_STAND_AS_ONE, 0) && (!player->GetQuestObjectiveProgress(QUEST_STAND_AS_ONE, 1)))
                                                        {
                                                            SpawnHarbinger(anduin);
                                                            for (uint8 i = 0; i < ENCOUNTER_HARBINGER_NUMBER; ++i)
                                                            {
                                                                if (Creature* temp = ObjectAccessor::GetCreature(*me, harbingerGUID[i]))
                                                                    harbingerGUID[i].Clear();
                                                            }
                                                            context.Repeat(50s);
                                                        }
                                                        else if (player->GetQuestObjectiveProgress(QUEST_STAND_AS_ONE, 0) && (!player->GetQuestObjectiveProgress(QUEST_STAND_AS_ONE, 1)))
                                                            return;
                                                    }).Schedule(Milliseconds(6000), [this, baine, player](TaskContext context)
                                                        {
                                                            if (!player->GetQuestObjectiveProgress(QUEST_STAND_AS_ONE, 0) && (!player->GetQuestObjectiveProgress(QUEST_STAND_AS_ONE, 1)))
                                                            {
                                                                SpawnHarbinger(baine);
                                                                context.Repeat(50s);
                                                            }
                                                            else if (player->GetQuestObjectiveProgress(QUEST_STAND_AS_ONE, 0) && (!player->GetQuestObjectiveProgress(QUEST_STAND_AS_ONE, 1)))
                                                                return;
                                                        });
                                    }
                                }
                }
                _events.ScheduleEvent(EVENT_STAND_PHASE2, 30s);
                break;

            case EVENT_STAND_PHASE2:
                if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                {
                    if (Creature* baine = ObjectAccessor::GetCreature(*me, baineGUID))
                    {
                        baine->AI()->Talk(8);
                    }
                    me->AddDelayedEvent(5000, [this, player]()
                        {
                            player->PlayConversation(14777);
                        });

                    me->AddDelayedEvent(18000, [this, player]()
                        {
                            if (Creature* jaina = ObjectAccessor::GetCreature(*me, jainaGUID))
                            {
                                jaina->AI()->Talk(53);
                            }
                        });
                }
                _events.ScheduleEvent(EVENT_STAND_PHASE3, 25s);
                break;

            case EVENT_STAND_PHASE3:
                if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                {
                    player->PlayConversation(14779);
                    if (Creature* thrall = ObjectAccessor::GetCreature(*me, thrallGUID))
                    {
                        me->AddDelayedEvent(6000, [this, thrall, player]()
                            {
                                if (!player->GetPhaseShift().HasPhase(10041)) PhasingHandler::AddPhase(player, 10041,true);
                                thrall->AI()->Talk(28);
                            });
                    }
                    if (Creature* jaina = ObjectAccessor::GetCreature(*me, jainaGUID))
                    {
                        player->GetScheduler().Schedule(Milliseconds(15000), [this, jaina](TaskContext context)
                            {
                                jaina->AI()->Talk(54);
                            });
                    }
                    me->AddDelayedEvent(28000, [this, player]()
                        {
                            player->PlayConversation(14782);
                            player->KilledMonsterCredit(168478);
                        });
                }
                _events.ScheduleEvent(EVENT_STAND_PHASE4, 40s);
                break;

            case EVENT_STAND_PHASE4:
                if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                {
                    if (Creature* jaina = ObjectAccessor::GetCreature(*me, jainaGUID))
                        if (Creature* baine = ObjectAccessor::GetCreature(*me, baineGUID))
                        {
                            baine->AI()->Talk(9);
                       
                            player->GetScheduler().Schedule(Milliseconds(7000), [this, jaina, player](TaskContext context)
                                {
                                    jaina->AI()->Talk(55, player);
                                    player->ForceCompleteQuest(QUEST_STAND_AS_ONE);
                                    me->RemoveNpcFlag(UNIT_NPC_FLAG_SPELLCLICK);
                                    me->SetNpcFlag(UNIT_NPC_FLAG_QUESTGIVER | UNIT_NPC_FLAG_GOSSIP);
                                });
                        }
                }
                break;
       
            }
        }
    }

    void SpawnHarbinger(Creature* npc)
    {
        Unit* temp = nullptr;
        uint32 enemy;

        for (uint8 i = 0; i < ENCOUNTER_HARBINGER_NUMBER; ++i)
        {
            switch (urand(0, 2))
            {
            case 0:
                enemy = 168585; //harbinger
                break;
            case 1:
                enemy = 168588; //guard
                break;
            case 2:
                enemy = 168586; //sorcerer
                break;
            default:
                break;
            }

            temp = ObjectAccessor::GetCreature(*npc, harbingerGUID[i]);
            if (!temp)
            {
                temp = npc->SummonCreature(enemy, npc->GetPosition().GetPositionWithOffset({ float(rand32() % 10), float(rand32() % 10), 0.0f, 0.0f }), TEMPSUMMON_TIMED_DESPAWN, 180s);
                temp->SetFaction(14);
                if (temp->IsInDist(npc, 20.0f))
                {
                    if (temp->GetEntry() == 168585 || temp->GetEntry() == 168588 || temp->GetEntry() == 168586)
                    {
                        npc->GetThreatManager().AddThreat(temp, 1000.0f);
                        npc->Attack(temp, true);
                    }
                }
                harbingerGUID[i] = temp->GetGUID();
            }
        }
    }

};

// 162804 Ve'nari
struct npc_venari_maw_162804 : public ScriptedAI
{
public:
    npc_venari_maw_162804(Creature* creature) : ScriptedAI(creature) { }
private:
    bool say63661;

    void Reset() override
    {
        say63661 = false;
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
        {
            if (player->IsInDist(me, 20.0f))
                if (!say63661)
                    if (player->GetQuestStatus(QUEST_LINK_TO_THE_MAW) == QUEST_STATUS_COMPLETE)
                    {
                        say63661 = true;
                        Talk(0);
                    }
        }
    }

    bool OnGossipHello(Player* player) override    //npc_txt appear to top
    {
        if (me->IsQuestGiver())
        {
            player->PrepareQuestMenu(me->GetGUID());
            SendGossipMenuFor(player, player->GetGossipTextId(me), me->GetGUID());
        }

        if (player->GetQuestStatus(QUEST_LINK_TO_THE_MAW) == QUEST_STATUS_COMPLETE)
        {
            AddGossipItemFor(player, GossipOptionNpc::None, " I need your help to find a second waystone to create a permanent connection to Oribos.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 0);
            SendGossipMenuFor(player, 42761, me->GetGUID());   
            player->PrepareQuestMenu(me->GetGUID());
        }
 
        if (player->GetQuestObjectiveProgress(QUEST_MYSTERIES_OF_THE_MAW, 0) && !player->GetQuestObjectiveProgress(QUEST_MYSTERIES_OF_THE_MAW, 1))
        {
            AddGossipItemFor(player, GossipOptionNpc::None, "I'm ready, let's move.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            SendGossipMenuFor(player, 43407, me->GetGUID());
            player->PrepareQuestMenu(me->GetGUID());
        }

        return true;
    }

    bool OnGossipSelect(Player* player, uint32 MenuID, uint32 gossipListId) override
    {
        uint32 const action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);
        ClearGossipMenuFor(player);
        if (action == GOSSIP_ACTION_INFO_DEF + 0)
        {
            CloseGossipMenuFor(player);
            Talk(1);
            player->GetScheduler().Schedule(Milliseconds(13000), [this](TaskContext context)
                {
                    Talk(2);
                });
        }
        if (action == GOSSIP_ACTION_INFO_DEF + 1)
        {
            CloseGossipMenuFor(player);
            player->SummonCreature(177819, me->GetPosition(), TEMPSUMMON_MANUAL_DESPAWN);
            me->ForcedDespawn(1000);
        }
        return true;
    }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_MYSTERIES_OF_THE_MAW)
            if (!player->FindNearestCreature(177246, 100.0f))
            {
                Talk(3);
                player->SummonCreature(177246, 4520.45f, 6708.19f, 4827.26f, 1.439f, TEMPSUMMON_MANUAL_DESPAWN);
            }
    }
};

// 177640 spell_bunny
Position const  SpawnBlastBunnyGroup[11] =
{
    { 3622.22f, 5968.74f, 4857.6f,  0.814258f },
    { 3654.2f,  6030.61f, 4836.74f, 5.0672f },
    { 3686.71f, 6016.33f, 4837.61f, 3.53568f },
    { 3638.46f, 5979.37f, 4848.58f, 0.916377f },
    { 3665.04f, 5972.04f, 4845.36f, 2.45576f },
    { 3633.58f, 6013.86f, 4847.46f, 0.70431f },
    { 3642.47f, 6010.53f, 4846.76f, 5.28319f },
    { 3657.79f, 5982.79f, 4843.7f,  2.69923f },
    { 3685.78f, 6001.77f, 4841.12f, 1.45043f },
    { 3663.69f, 6024.36f, 4836.85f, 5.95076f },
    { 3702.34f, 6026.94f, 4842.99f, 2.86808f }
};

// 177819 Ve'nari
struct npc_venari_177819 : public EscortAI
{
public:
    npc_venari_177819(Creature* creature) : EscortAI(creature) { }
private:
    ObjectGuid m_raiderGUID;
    ObjectGuid m_sentryGUID;
    ObjectGuid m_playerGUID;
    ObjectGuid m_eyeGUID;
    std::list<ObjectGuid> m_spellbunnyList;
    EventMap _events;
    bool say63662;

    void Reset() override
    {
        m_raiderGUID = ObjectGuid::Empty;
        m_sentryGUID = ObjectGuid::Empty;
        m_playerGUID = ObjectGuid::Empty;
        m_eyeGUID = ObjectGuid::Empty;
        _events.Reset();
        say63662 = false;
        me->CastSpell(me, SPELL_ANIMA_SHROUD, false);
        _events.ScheduleEvent(EVENT_VENARI_WAYPOINT_START, 3s);
    }

    void JustSummoned(Creature* summon) override
    {
        summons.Summon(summon);

        if (summon->GetEntry() == NPC_SPELL_BUNNY)
            m_spellbunnyList.push_back(summon->GetGUID());

    }

    void IsSummonedBy(WorldObject* summoner) override
    {
        m_playerGUID = summoner->GetGUID();
        summoner->CastSpell(summoner, SPELL_CLOAK_OF_VENARI, false);
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
            if (Player* player = who->ToPlayer())
                if (player->GetQuestObjectiveProgress(QUEST_MYSTERIES_OF_THE_MAW, 0))
                {
                    if (player->GetDistance2d(me) >= 12.0f)
                    {
                        if (!say63662)
                        {
                            say63662 = true;
                            player->RemoveAura(SPELL_CLOAK_OF_VENARI);
                            Talk(1);
                        }
                    }
                    if (player->IsInDist(me, 12.0f) && say63662 == true)
                    {
                        say63662 = false;
                        player->CastSpell(player, SPELL_CLOAK_OF_VENARI, false);
                    }
                }
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        if (type == WAYPOINT_MOTION_TYPE)
        {
            switch (id)
            {
            case 1:
                Talk(0);
                me->SummonCreature(177525, 3912.34f, 6200.73f, 4849.44f, 4.68035f, TEMPSUMMON_TIMED_DESPAWN, 300s);  // summon control_bunny
                me->SummonCreature(177525, 3903.26f, 6192.59f, 4841.39f, 4.96702f, TEMPSUMMON_TIMED_DESPAWN, 300s);
                me->SummonCreature(177525, 3897.97f, 6189.74f, 4841.11f, 4.97487f, TEMPSUMMON_TIMED_DESPAWN, 300s);
                break;
            case 4:
            {
                if (Creature* raider = me->SummonCreature(177826, 3911.21f, 6122.03f, 4849.44f, 4.78713f, TEMPSUMMON_TIMED_DESPAWN, 180s))
                    m_raiderGUID = raider->GetGUID();
                if (Creature* sentry = me->SummonCreature(177825, 3913.3f, 6118.98f, 4850.32f, 3.75086f, TEMPSUMMON_TIMED_DESPAWN, 180s))
                    m_sentryGUID = sentry->GetGUID();
            }
            break;
            case 7:
                me->PauseMovement();
                _events.ScheduleEvent(EVENT_VENARI_WAYPOINT_DIALOG, 1s);
                break;
            case 11:
                if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                    player->CastSpell(player, SCENE_ENTRY_INTO_KORTHIA, false);
                SummonMyMember();
                me->PauseMovement();
                break;
            case 24:
                Talk(4);
                _events.ScheduleEvent(EVENT_VENARI_WAYPOINT_THEEYE, 1s);
                me->PauseMovement();
                RemoveMyMember();
                break;
            case 28:
                Talk(6);
                break;
            case 30:
                Talk(7);
                if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                {
                    player->ForceCompleteQuest(QUEST_MYSTERIES_OF_THE_MAW);
                    player->RemoveAura(SPELL_CLOAK_OF_VENARI);

                    me->RemoveAura(SPELL_ANIMA_SHROUD);
                    me->AddDelayedEvent(3000, [this, player]() -> void
                        {
                            player->SummonCreature(NPC_VENARI2, me->GetPosition(), TEMPSUMMON_TIMED_DESPAWN, 300s);
                        });
                    me->ForcedDespawn(4000);
                }
                break;
            }
        }
    }

    void UpdateAI(uint32 diff) override
    {
        _events.Update(diff);

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_VENARI_WAYPOINT_START:
                me->GetMotionMaster()->MovePath(17781901, false);
                break;
            case EVENT_VENARI_WAYPOINT_DIALOG:
                if (Creature* raider = ObjectAccessor::GetCreature(*me, m_raiderGUID))
                    raider->AI()->Talk(0);
                me->AddDelayedEvent(9500, [this]()
                    {
                        if (Creature* sentry = ObjectAccessor::GetCreature(*me, m_sentryGUID))
                            sentry->AI()->Talk(0);
                    });
                me->AddDelayedEvent(17500, [this]()
                    {
                        Talk(2);
                        me->ResumeMovement();
                    });
                break;
            case EVENT_VENARI_WAYPOINT_THEEYE:
                if (Creature* eye = me->SummonCreature(NPC_EYE_OF_THE_JAILER, 3555.66f, 5964.46f, 4885.23f, 0.575974f, TEMPSUMMON_TIMED_DESPAWN, 60s))
                {
                    eye->SetFaction(36);
                    eye->SetWalk(false);
                    m_eyeGUID = eye->GetGUID();
                    eye->GetMotionMaster()->MovePoint(0, 3565.61f, 5972.25f, 4861.61f, true, 0.6678f);
                }
                me->AddDelayedEvent(10000, [this]() -> void
                    {
                        if (Creature* eye = ObjectAccessor::GetCreature(*me, m_eyeGUID))
                            eye->GetMotionMaster()->MovePoint(0, 3603.04f, 6015.26f, 4857.23f, true, 1.6678f);
                    });
                me->AddDelayedEvent(14000, [this]() -> void
                    {
                        if (Creature* eye = ObjectAccessor::GetCreature(*me, m_eyeGUID))
                            eye->ForcedDespawn(500);
                        Talk(5);
                        me->ResumeMovement();
                    });
                break;
            }
        }
    }

    void DoAction(int32 action) override
    {
        if (action == 1)
        {
            Talk(3);
            me->ResumeMovement();
        }
    }

    void SummonMyMember()
    {
        for (uint32 i = 0; i < 11; ++i)
            if (Creature* spell = DoSummon(NPC_SPELL_BUNNY, SpawnBlastBunnyGroup[i], 300s, TEMPSUMMON_TIMED_DESPAWN))
                spell->SetReactState(REACT_DEFENSIVE);
    }

    void RemoveMyMember()
    {
        for (std::list<ObjectGuid>::const_iterator itr = m_spellbunnyList.begin(); itr != m_spellbunnyList.end(); ++itr)
            if (Creature* follower = ObjectAccessor::GetCreature(*me, (*itr)))
                follower->DespawnOrUnsummon(1s);
        m_spellbunnyList.clear();
    }
};

// 177525 control_bunny
struct npc_control_bunny_177525 : public ScriptedAI
{
public:
    npc_control_bunny_177525(Creature* creature) : ScriptedAI(creature) {}

private:
    EventMap _events;

    void IsSummonedBy(WorldObject* summoner) override
    {
        if (Creature* venari = summoner->ToCreature())
        {
            if (venari->GetEntry() == 177819)
                _events.ScheduleEvent(EVENT_VENARI_SUMMON_MAWSWORN, 1s);
        }
    }

    void UpdateAI(uint32 diff) override
    {

        _events.Update(diff);

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_VENARI_SUMMON_MAWSWORN:
                switch (uint8 rand = urand(0, 2))
                {
                case 0:
                    SummonMawsworn(NPC_MAWSWORN_HOUNDMASTER);
                    break;
                case 1:
                    SummonMawsworn(NPC_MAWSWORN_BRUTALIZER);
                    break;
                case 2:
                    SummonMawsworn(NPC_MAWSWORN_HOUNDMASTER);
                    break;
                }
                _events.Repeat(randtime(6s, 12s));
                break;
            }
        }
    }

    void SummonMawsworn(uint32 creature)
    {
        Position mawswornPos = me->GetPosition();
        if (Creature* mawsworn = me->SummonCreature(creature, mawswornPos, TEMPSUMMON_TIMED_DESPAWN, 60s))
        {
            mawsworn->SetFaction(36);
            switch (uint8 rand = urand(0, 1))
            {
            case 0:
                mawsworn->GetMotionMaster()->MovePath(17482601, false);
                break;
            case 1:
                mawsworn->GetMotionMaster()->MovePath(17482602, false);
                break;
            }
        }
    };
};



// 177640 spell_bunny
struct npc_spell_bunny_177640 : public ScriptedAI
{
public:
    npc_spell_bunny_177640(Creature* creature) : ScriptedAI(creature)
    {
        _events.ScheduleEvent(EVENT_RESIDUAL_BLAST_START, 3s);
    }

private:
    EventMap _events;

    void UpdateAI(uint32 diff) override
    {
        _events.Update(diff);

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_RESIDUAL_BLAST_START:
                me->CastSpell(me, SPELL_RESIDUAL_BLAST, false);
                _events.Repeat(randtime(2s, 5s));
                break;
            }
        }
    }
};

Position const venari_path[2] =
{
    {3392.56f, 5932.65f, 4882.67f, 0},
    {3336.1f, 5946.73f, 4881.75f, 2.7967f},
};

// 177202 Ve'nari
struct npc_venari_177202 : public ScriptedAI
{
public:
    npc_venari_177202(Creature* creature) : ScriptedAI(creature){}

    bool OnGossipHello(Player* player) override    //npc_txt appear to top
    {
        if (player->GetQuestStatus(QUEST_LINK_TO_THE_MAW) == QUEST_STATUS_REWARDED)
        {
            player->PrepareQuestMenu(me->GetGUID());
            SendGossipMenuFor(player, player->GetGossipTextId(me), me->GetGUID());
        }
            
        if ((player->GetQuestStatus(QUEST_KORTHIA_THE_CITY_OF_SECRETS) == QUEST_STATUS_INCOMPLETE) && !player->GetQuestObjectiveProgress(QUEST_KORTHIA_THE_CITY_OF_SECRETS, 1))
        {
            AddGossipItemFor(player, GossipOptionNpc::None, "Scan the area to see what exactly might be nearby.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 0);
            SendGossipMenuFor(player, 42780, me->GetGUID());
            player->PrepareQuestMenu(me->GetGUID());
        }
      
        return true;
    }

    bool OnGossipSelect(Player* player, uint32 MenuID, uint32 gossipListId) override
    {
        uint32 const action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);
        ClearGossipMenuFor(player);
        if (action == GOSSIP_ACTION_INFO_DEF + 0)
        {
            CloseGossipMenuFor(player);
            me->SetWalk(true);
            Talk(0);
            me->GetMotionMaster()->MovePoint(0, 3455.54f, 5937.98f, 4882.34f, true);
            player->GetScheduler().Schedule(Milliseconds(6000), [this, player](TaskContext context)
                {
                    Talk(1);
                    me->GetMotionMaster()->MovePoint(0, 3457.19f, 5948.67f, 4880.51f, true);
                    player->SummonCreature(177227, 3324.33f, 5951.04f, 4882.25f, 5.85749f, TEMPSUMMON_MANUAL_DESPAWN); // tal-galan
                });
            player->GetScheduler().Schedule(Milliseconds(19000), [this, player](TaskContext context)
                {
                    player->KilledMonsterCredit(177202);
                    Talk(2);
                    me->GetMotionMaster()->MovePoint(0, 3438.56f, 5941.61f, 4882.94f, true);
                    me->ForcedDespawn(15000);
                });
        }
        return true;
    }

    void DoAction(int32 action) override
    {
        if (action == 2) me->GetMotionMaster()->MoveSmoothPath(1000, venari_path, 2, false, false);
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        if (id == 1000)
        {
            Talk(3);
            if (Creature* galan = me->FindNearestCreature(177227, 20.0f, true))
                galan->AI()->DoAction(3);
        }
    }
              
};

void AddSC_zone_the_maw_part_two()
{
    new npc_Anduin_Wrynn_167833();
    new npc_thrall_167176();
    new npc_lady_jaina_proudmoore_167154();
    RegisterCreatureAI(npc_Anduin_Wrynn_167906);
    RegisterCreatureAI(npc_Ebon_Blade_guid345873);
    RegisterCreatureAI(npc_helya_165543);
    new npc_Baine_Bloodhoof_168162();
    RegisterGameObjectAI(go_place_blade_353170);
    RegisterCreatureAI(npc_withering_presence);
    RegisterCreatureAI(npc_eroded_waystone_168478);
    RegisterCreatureAI(npc_venari_maw_162804);
    RegisterCreatureAI(npc_venari_177819);
    RegisterCreatureAI(npc_control_bunny_177525);
    RegisterCreatureAI(npc_spell_bunny_177640);
    RegisterCreatureAI(npc_venari_177202);
}
