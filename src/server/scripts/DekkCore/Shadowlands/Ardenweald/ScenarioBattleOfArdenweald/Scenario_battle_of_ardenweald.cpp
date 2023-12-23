/*
 * Copyright 2021 TrinityCore
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

#include "CriteriaHandler.h"
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "Scenario.h"
#include "Scenario_battle_of_ardenweald.h"
#include "Vehicle.h"
#include "TemporarySummon.h"

ObjectData const creatureData[] =
{
    { NPC_REALMBREAKER,   DATA_REALMBREAKER },
    { 0,                  0                 }
};

class Scenario_battle_of_ardenweald : public InstanceMapScript
{
public:
    Scenario_battle_of_ardenweald() : InstanceMapScript("Scenario_battle_of_ardenweald", 2464) { }

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new Scenario_battle_of_ardenweald_InstanceMapScript(map);
    }

    struct Scenario_battle_of_ardenweald_InstanceMapScript : public InstanceScript
    {
        Scenario_battle_of_ardenweald_InstanceMapScript(InstanceMap* map) : InstanceScript(map)
        {
            SetHeaders(DataHeader);
            SetBossNumber(EncounterCount);
            LoadObjectData(creatureData, nullptr);
            playerEnter = false;
        }

    private:
        bool playerEnter;

        void OnPlayerEnter(Player* player) override
        {
            if (!player)
                return;

            if (player->GetGUID() == ObjectGuid::Empty)
                return;


            if (player->GetAreaId() == 11510)
                if (player->GetQuestObjectiveProgress(QUEST_THE_BATTLE_OF_ARDENWEALD, 2) && !player->GetQuestObjectiveProgress(QUEST_THE_BATTLE_OF_ARDENWEALD, 1))
                {
                    if (Scenario* scenario = player->GetScenario())
                    {
                        if (!player->GetScenario()->IsCompletedStep(sScenarioStepStore.LookupEntry(SCENARIO_BOA_INDEX_0)))
                            if (scenario->GetStep() == sScenarioStepStore.LookupEntry(SCENARIO_BOA_INDEX_0))
                            {
                                player->SetCanFly(true);
                                player->AddDelayedEvent(3000, [this, player]() -> void
                                    {
                                        player->SummonCreature(178291, player->GetPosition(), TEMPSUMMON_MANUAL_DESPAWN);
                                    });
                            }
                    }
                }
        }
    };

};

class scene_battle_of_ardenweald : public SceneScript
{
public:
    scene_battle_of_ardenweald() : SceneScript("scene_battle_of_ardenweald") { }

    void OnSceneStart(Player* player, uint32 /*sceneInstanceID*/, SceneTemplate const* sceneTemplate) override
    {
        if (sceneTemplate->ScenePackageId == 3273)
            player->NearTeleportTo(Position(-6102.81f, 833.546f, 5451.18f, 2.5043f), false);
    }

    // Called when a scene is canceled
    void OnSceneCancel(Player* player, uint32 /*sceneInstanceID*/, SceneTemplate const* sceneTemplate) override
    {
        if (sceneTemplate->ScenePackageId == 3273)
            Finish(player);
    }

    // Called when a scene is completed
    void OnSceneComplete(Player* player, uint32 /*sceneInstanceID*/, SceneTemplate const* sceneTemplate) override
    {
        if (sceneTemplate->ScenePackageId == 3273)
            Finish(player);
    }

    void Finish(Player* player)
    {
        if (Creature* winterqueen = GetClosestCreatureWithEntry(player, 177280, 50.0f))
        {
            winterqueen->AddAura(65050, winterqueen);
            winterqueen->DespawnOrUnsummon(5s, 120s);
            if (Creature* sylvanas = player->SummonCreature(177114, Position(-6130.22f, 855.108f, 5451.64f, 0.175181f), TEMPSUMMON_TIMED_DESPAWN, 120s))
                sylvanas->RemoveAura(49414);
        }
    }
};


// 178291 Anima Wyrm
struct npc_anima_wyrm_178291 : public EscortAI
{
public:
    npc_anima_wyrm_178291(Creature* creature) : EscortAI(creature) { }

private:
    Vehicle* vehicle;
    uint32   m_eventPhase;
    ObjectGuid m_playerGUID;
    ObjectGuid plagueGUID;
    ObjectGuid kleiaGUID;

    void Reset() override
    {
        m_eventPhase = 0;
        m_playerGUID = ObjectGuid::Empty;
        plagueGUID = ObjectGuid::Empty;
        kleiaGUID = ObjectGuid::Empty;
    }

    void IsSummonedBy(WorldObject* summoner) override
    {
        m_playerGUID = summoner->GetGUID();

        if (Player* player = summoner->ToPlayer())
        {
            if (player->HasQuest(QUEST_THE_BATTLE_OF_ARDENWEALD))
            {
                vehicle = me->GetVehicleKit();
                player->EnterVehicle(me);
                if (Creature* plague = me->SummonCreature(177261, -5488.37f, 596.306f, 5582.41f, 3.6468f, TEMPSUMMON_MANUAL_DESPAWN))
                    plagueGUID = plague->GetGUID();
                if (Creature* kleia = me->SummonCreature(177123, -5498.37f, 606.306f, 5584.41f, 3.6468f, TEMPSUMMON_MANUAL_DESPAWN))
                    kleiaGUID = kleia->GetGUID();
            }
        }
    }

    void PassengerBoarded(Unit* passenger, int8 /*seat*/, bool apply) override
    {
        if (passenger->GetTypeId() == TYPEID_PLAYER)
        {
            if (Player* player = passenger->ToPlayer())
                if (player->HasQuest(QUEST_THE_BATTLE_OF_ARDENWEALD))
                {
                    if (player->GetAreaId() == 10565)
                    {
                        m_eventPhase = 1;
                        me->GetMotionMaster()->MovePath(17829101, false);
                    }
                    if (player->GetAreaId() == 11510)
                    {
                        m_eventPhase = 2;
                        me->GetMotionMaster()->MovePath(17829102, false);
                        if (Creature* plague = ObjectAccessor::GetCreature(*me, plagueGUID))
                            plague->GetMotionMaster()->MovePath(17726101, false);
                        if (Creature* kleia = ObjectAccessor::GetCreature(*me, kleiaGUID))
                            kleia->GetMotionMaster()->MovePath(17712301, false);
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
            if (m_eventPhase == 1 && id == 7)
            {
                if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                {
                    player->CastSpell(player, SPELL_TELEPORT_VISUAL_ARDENWEALD);
                    me->AddDelayedEvent(2000, [player]()
                        {
                            player->TeleportTo(2464, -5491.37f, 600.306f, 5583.41f, 3.6468f, TELE_TO_NONE);
                        });
                }
            }
            else if (m_eventPhase == 2 && id == 1)
            {
                if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                    Conversation::CreateConversation(17023, player, player->GetPosition(), { player->GetGUID() });
            }
            else if (m_eventPhase == 2 && id == 8)
            {
                if (vehicle = me->GetVehicleKit())
                {
                    if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                    {
                        if (Quest const* quest = sObjectMgr->GetQuestTemplate(QUEST_THE_BATTLE_OF_ARDENWEALD))
                            for (QuestObjective const& obj : quest->GetObjectives())
                                if (obj.ObjectID == 91074)
                                {
                                    uint16 slot = player->FindQuestSlot(QUEST_THE_BATTLE_OF_ARDENWEALD);
                                    if (!player->IsQuestObjectiveComplete(slot, quest, obj)) // just make it complete once
                                    {
                                        player->SetQuestObjectiveData(obj, 1);
                                        player->SendQuestUpdateAddCredit(quest, ObjectGuid::Empty, obj, 1);
                                    }
                                }
                        player->SetCanFly(false);
                    }
                    if (Creature* plague = ObjectAccessor::GetCreature(*me, plagueGUID))
                        plague->DespawnOrUnsummon(3s);
                    if (Creature* kleia = ObjectAccessor::GetCreature(*me, kleiaGUID))
                        kleia->DespawnOrUnsummon(3s);
                    vehicle->RemoveAllPassengers();
                    me->DespawnOrUnsummon(3s);
                }
            }
            break;
        }
        }
    }
};

//177962 Lady Moonberry
struct npc_lady_moonberry_177962 : public  EscortAI
{
public:
    npc_lady_moonberry_177962(Creature* creature) : EscortAI(creature) {}

private:
    ObjectGuid m_playerGUID;


    void Reset() override
    {
        m_playerGUID = ObjectGuid::Empty;
    }


    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
        {
            if (player->IsInDist(me, 30.0f))
            {
                if (Scenario* scenario = player->GetScenario())
                {
                    if (scenario->GetStep() == sScenarioStepStore.LookupEntry(SCENARIO_BOA_INDEX_0))
                    {
                        if (Creature* jaina = me->FindNearestCreature(178503, 100.0f))
                            jaina->RemoveAura(SPELL_ARCANE_SHIELD);
                        me->RemoveNpcFlag(UNIT_NPC_FLAG_QUESTGIVER);
                        scenario->SetStepState(sScenarioStepStore.LookupEntry(SCENARIO_BOA_INDEX_0), SCENARIO_STEP_IN_PROGRESS);
                        scenario->CompleteStep(sScenarioStepStore.LookupEntry(SCENARIO_BOA_INDEX_0));
                        scenario->SetStep(sScenarioStepStore.LookupEntry(SCENARIO_BOA_INDEX_1));
                        Talk(0);
                    }
                }
            }
        }
    }

    bool OnGossipSelect(Player* player, uint32 MenuID, uint32 gossipListId) override
    {
        if (gossipListId == 0)
        {
            m_playerGUID = player->GetGUID();
            player->PrepareQuestMenu(me->GetGUID());
            ClearGossipMenuFor(player);
            SendGossipMenuFor(player, 42791, me->GetGUID());  // Show Next gossip and should not close
            Talk(1);
            if (Creature* animacone = me->FindNearestCreature(178524, 50.0f))
                animacone->AI()->DoAction(ACTION_ANIMACONE_START_MOVE);
            me->AddDelayedEvent(9500, [this]()
                {
                    if (Creature* herne = me->FindNearestCreature(NPC_LOAR_HERNE, 50.0f))
                        herne->AI()->Talk(0);
                });
        }

        return true;
    }
};

Position const animacone_path[2] =
{
    {-5957.78f, 681.862f, 5501.97f, 0},
    {-5932.66f, 665.711f, 5496.78f, 0},
};

Position const sparklepuff_path[2] =
{
    {-5910.78f, 681.912f, 5496.62f, 0},
    {-5895.87f, 707.669f, 5496.54f, 0},
};

Position const wildhunt_path_right[6] =
{
    {-5939.76f, 696.635f, 5502.95f, 0},
    {-5930.92f, 682.947f, 5500.31f, 0},
    {-5921.71f, 664.764f, 5495.39f, 0},
    {-5926.09f, 645.624f, 5493.41f, 0},
    {-5921.31f, 613.824f, 5489.31f, 0},
    {-5931.98f, 560.731f, 5474.44f, 0},
};

Position const wildhunt_path_rightup[6] =
{
    {-5939.76f, 696.635f, 5506.95f, 0},
    {-5930.92f, 682.947f, 5504.31f, 0},
    {-5921.71f, 664.764f, 5499.39f, 0},
    {-5926.09f, 645.624f, 5497.41f, 0},
    {-5921.31f, 613.824f, 5493.31f, 0},
    {-5931.98f, 560.731f, 5478.44f, 0},
};

Position const wildhunt_path_center[6] =
{
    {-5937.85f, 674.561f, 5497.79f, 0},
    {-5925.74f, 653.912f, 5494.06f, 0},
    {-5928.58f, 630.745f, 5490.63f, 0},
    {-5927.55f, 593.852f, 5484.16f, 0},
    {-5938.66f, 555.456f, 5472.21f, 0},
    {-5936.66f, 553.456f, 5472.21f, 0},
};


Position const wildhunt_path_left[6] =
{
    {-5969.14f, 665.423f, 5501.12f, 0},
    {-5954.97f, 653.051f, 5502.49f, 0},
    {-5930.16f, 650.278f, 5494.01f, 0},
    {-5931.94f, 617.088f, 5487.62f, 0},
    {-5929.31f, 593.301f, 5483.62f, 0},
    {-5940.88f, 566.501f, 5474.28f, 0},

};

static Position const* wildhunt_path[3] = { wildhunt_path_right, wildhunt_path_center, wildhunt_path_left };

//178524 nanimacone 
struct npc_animacone_178524 : public  ScriptedAI
{
public:
    npc_animacone_178524(Creature* creature) : ScriptedAI(creature) {}

private:
    ObjectGuid m_playerGUID;
    ObjectGuid m_niyaGUID;
    ObjectGuid m_koraynGUID;
    ObjectGuid m_mistbladeGUID;
    ObjectGuid m_dravenGUID;
    ObjectGuid m_kleiaGUID;
    ObjectGuid m_plagueGUID;
    ObjectGuid m_herneGUID;
    EventMap _events;
    std::list<ObjectGuid> m_guardianList;
    std::list<ObjectGuid> m_archerList;
    std::list<ObjectGuid> m_nightbladeList;

    void Reset() override
    {
        _events.Reset();
        m_playerGUID.Clear();
        m_niyaGUID.Clear();
        m_koraynGUID.Clear();
        m_mistbladeGUID.Clear();
        m_dravenGUID.Clear();
        m_kleiaGUID.Clear();
        m_plagueGUID.Clear();
        m_herneGUID.Clear();
    }

    void DoAction(int32 action) override
    {
        if (action == ACTION_ANIMACONE_START_MOVE)
        {
            me->GetMotionMaster()->MoveSmoothPath(1001, animacone_path, 2, false, true);
        }
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        switch (id)
        {
        case 1001:
            me->RemoveVehicleKit();
            me->SetNpcFlag(UNIT_NPC_FLAG_SPELLCLICK);
            break;
        }
    }

    void OnSpellClick(Unit* clicker, bool /*result*/) override
    {
        if (Player* player = clicker->ToPlayer())
        {
            m_playerGUID = player->GetGUID();
            if (Scenario* scenario = player->GetScenario())
                if (scenario->GetStep() == sScenarioStepStore.LookupEntry(SCENARIO_BOA_INDEX_1))
                    _events.ScheduleEvent(EVENT_DEPLOY_THE_DECOYS_PHASE1, 1s);
            me->RemoveNpcFlag(UNIT_NPC_FLAG_SPELLCLICK);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        _events.Update(diff);

        while (uint32 eventId = _events.ExecuteEvent())
        {
            if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
            {
                switch (eventId)
                {
                case EVENT_DEPLOY_THE_DECOYS_PHASE1:
                {
                    player->CastSpell(player, SPELL_TRANSFORMING_WINTER_QUEEN, false);
                    me->AddDelayedEvent(8000, [this]()
                        {
                            if (Creature* sparklepuff = me->FindNearestCreature(177277, 20.0f))
                                sparklepuff->CastSpell(sparklepuff, SPELL_EMPOWER_TRANSFORMING, false);
                            me->RemoveNpcFlag(UNIT_NPC_FLAG_SPELLCLICK);
                        });
                    me->AddDelayedEvent(12000, [this]()
                        {
                            if (Creature* sparklepuff = me->FindNearestCreature(177277, 20.0f))
                            {
                                sparklepuff->SetDisplayId(95678, 1.0f);
                                sparklepuff->CastSpell(sparklepuff, SPELL_EMPOWER_TRANSFORMING, false);
                            }
                        });
                    _events.ScheduleEvent(EVENT_DEPLOY_THE_DECOYS_PHASE2, 18s);
                }
                break;
                case EVENT_DEPLOY_THE_DECOYS_PHASE2:
                {
                    if (Scenario* scenario = player->GetScenario())
                    {
                        if (scenario->GetStep() == sScenarioStepStore.LookupEntry(SCENARIO_BOA_INDEX_1))
                        {
                            scenario->CompleteStep(sScenarioStepStore.LookupEntry(SCENARIO_BOA_INDEX_1));
                            scenario->SetStep(sScenarioStepStore.LookupEntry(SCENARIO_BOA_INDEX_2));
                            Conversation::CreateConversation(17016, player, player->GetPosition(), { player->GetGUID() });  //Sylvanas say.
                        }

                        if (Creature* marastar = me->FindNearestCreature(177478, 20.0f))
                            if (Creature* seaharper = me->FindNearestCreature(177479, 20.0f))
                                if (Creature* sparklepuff = me->FindNearestCreature(177277, 20.0f))
                                {
                                    sparklepuff->GetMotionMaster()->MoveSmoothPath(1002, sparklepuff_path, 2, true, false);
                                    marastar->ForcedDespawn(35000);
                                    seaharper->ForcedDespawn(35000);
                                    sparklepuff->DespawnOrUnsummon(12s, 60s);
                                }
                        me->DespawnOrUnsummon(35s, 80s);

                    }
                    _events.ScheduleEvent(EVENT_DEPLOY_THE_DECOYS_PHASE3, 8s);
                }
                break;
                case EVENT_DEPLOY_THE_DECOYS_PHASE3:
                {
                    WildhuntPath(2001, m_nightbladeList, 178719);
                    WildhuntPath(2002, m_guardianList, 178749);
                    WildhuntPath(2003, m_archerList, 178748);
                    if (Creature* niya = me->FindNearestCreature(NPC_NIYA, 50.0f))
                        if (Creature* mistblade = me->FindNearestCreature(NPC_MISTBLADE, 50.0f))
                            if (Creature* herne = me->FindNearestCreature(NPC_LOAR_HERNE, 50.0f))
                            {
                                m_niyaGUID = niya->GetGUID();
                                m_mistbladeGUID = mistblade->GetGUID();
                                m_herneGUID = herne->GetGUID();
                                niya->GetMotionMaster()->MoveSmoothPath(0, wildhunt_path_left, 6, false, false);
                                niya->DespawnOrUnsummon(20s, 120s);
                                mistblade->GetMotionMaster()->MoveSmoothPath(0, wildhunt_path_center, 6, false, false);
                                mistblade->DespawnOrUnsummon(20s, 120s);
                                herne->GetMotionMaster()->MoveSmoothPath(0, wildhunt_path_right, 6, false, false);
                                herne->AI()->Talk(1);
                                herne->DespawnOrUnsummon(20s, 120s);

                            }

                    _events.ScheduleEvent(EVENT_DEPLOY_THE_DECOYS_PHASE4, 3s);
                }
                break;
                case EVENT_DEPLOY_THE_DECOYS_PHASE4:
                {
                    if (Creature* korayn = me->FindNearestCreature(NPC_KORAYN, 50.0f))
                        if (Creature* draven = me->FindNearestCreature(177250, 50.0f))
                            if (Creature* kleia = me->FindNearestCreature(177123, 50.0f))
                                if (Creature* plague = me->FindNearestCreature(177261, 70.0f))
                                {
                                    m_koraynGUID = korayn->GetGUID();
                                    korayn->DespawnOrUnsummon(20s, 120s);
                                    m_dravenGUID = draven->GetGUID();
                                    draven->DespawnOrUnsummon(20s, 120s);
                                    m_kleiaGUID = kleia->GetGUID();
                                    kleia->DespawnOrUnsummon(20s, 120s);
                                    m_plagueGUID = plague->GetGUID();
                                    plague->DespawnOrUnsummon(20s, 120s);
                                    korayn->GetMotionMaster()->MoveSmoothPath(0, wildhunt_path_left, 6, false, false);
                                    draven->GetMotionMaster()->MoveSmoothPath(0, wildhunt_path_center, 6, false, false);
                                    kleia->GetMotionMaster()->MoveSmoothPath(0, wildhunt_path_rightup, 6, false, true);
                                    kleia->AI()->Talk(1);
                                    plague->GetMotionMaster()->MoveSmoothPath(0, wildhunt_path_rightup, 6, false, true);
                                }

                    _events.ScheduleEvent(EVENT_DEPLOY_THE_DECOYS_PHASE5, 4s);
                }
                break;
                case EVENT_DEPLOY_THE_DECOYS_PHASE5:
                {
                    if (Creature* plague = ObjectAccessor::GetCreature(*me, m_plagueGUID))
                        plague->AI()->Talk(0);
                    _events.ScheduleEvent(EVENT_DEPLOY_THE_DECOYS_PHASE6, 8s);
                }
                break;
                case EVENT_DEPLOY_THE_DECOYS_PHASE6:
                {
                    if (Creature* kleia = ObjectAccessor::GetCreature(*me, m_kleiaGUID))
                        kleia->AI()->Talk(2);
                    m_nightbladeList.clear();
                    m_guardianList.clear();
                    m_archerList.clear();
                }
                break;
                }
            }
        }
    }

    void WildhuntPath(uint32 pointid, std::list<ObjectGuid> m_List, uint32 entry)
    {
        if (m_List.empty())
        {
            std::list<Creature*> creatureList = me->FindNearestCreatures(entry, 100.0f);
            for (std::list<Creature*>::const_iterator itr = creatureList.begin(); itr != creatureList.end(); ++itr)
                if (Creature* wildhunt = (*itr))
                    m_List.push_back(wildhunt->GetGUID());
        }

        for (std::list<ObjectGuid>::iterator itr = m_List.begin(); itr != m_List.end(); ++itr)
            if (Creature* creature = ObjectAccessor::GetCreature(*me, (*itr)))
            {
                creature->GetMotionMaster()->MoveSmoothPath(pointid, wildhunt_path[urand(0, 2)], 6, false, false);
                creature->DespawnOrUnsummon(18s, 120s);
            }
    }
};

uint8 increase_death = 0;

//177285 177162 178744 npc_boa_stage3_increasebar
struct npc_boa_stage3_increasebar_i : public  ScriptedAI
{
public:
    npc_boa_stage3_increasebar_i(Creature* creature) : ScriptedAI(creature) {}

    void JustDied(Unit* killer) override
    {
        if (Player* player = killer->ToPlayer())
        {
            if (increase_death >= 3)
            {
                player->UpdateCriteria(CriteriaType::AnyoneTriggerGameEventScenario, 0, 0, 0, player);
                increase_death = 0;
                return;
            }
            else increase_death = increase_death + 1;

        }
    }
    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
        {
            if (player->IsInDist(me, 30.0f))
            {
                if (Scenario* scenario = player->GetScenario())
                {
                    if (scenario->IsCompletedStep(sScenarioStepStore.LookupEntry(SCENARIO_BOA_INDEX_2)))
                    {
                        scenario->SetStepState(sScenarioStepStore.LookupEntry(SCENARIO_BOA_INDEX_2), SCENARIO_STEP_DONE);
                        scenario->CompleteStep(sScenarioStepStore.LookupEntry(SCENARIO_BOA_INDEX_2));
                        scenario->SetStep(sScenarioStepStore.LookupEntry(SCENARIO_BOA_INDEX_3));
                    }
                }
            }
        }
    }
};

//177820 178747 npc_boa_stage3_increasebar
struct npc_boa_stage3_increasebar_ii : public  ScriptedAI
{
public:
    npc_boa_stage3_increasebar_ii(Creature* creature) : ScriptedAI(creature) {}

    void OnSpellClick(Unit* clicker, bool /*result*/) override
    {
        if (Player* player = clicker->ToPlayer())
        {
            if (me->GetEntry() == 177820)
            {
                me->RemoveNpcFlag(UNIT_NPC_FLAG_SPELLCLICK);
                me->ForcedDespawn(5000);
                if (increase_death >= 3)
                {
                    player->UpdateCriteria(CriteriaType::AnyoneTriggerGameEventScenario, 0, 0, 0, player);
                    increase_death = 0;
                    return;
                }
                else increase_death = increase_death + 1;
            }

            if (me->GetEntry() == 178747)
            {
                if (increase_death >= 3)
                {
                    player->UpdateCriteria(CriteriaType::AnyoneTriggerGameEventScenario, 0, 0, 0, player);
                    increase_death = 0;
                    return;
                }
                else increase_death = increase_death + 1;
                me->RemoveNpcFlag(UNIT_NPC_FLAG_SPELLCLICK);
                me->RemoveUnitFlag(UNIT_FLAG_IMMUNE_TO_NPC);
                me->RemoveAura(SPELL_INJURED_AURA);
                me->GetMotionMaster()->MoveFollow(player, PET_FOLLOW_DIST, player->GetFollowAngle(), {}, MOTION_SLOT_ACTIVE);
                me->ForcedDespawn(20000);
                Talk(0);
            }
        }
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
        {
            if (player->IsInDist(me, 30.0f))
            {
                if (Scenario* scenario = player->GetScenario())
                {
                    if (scenario->IsCompletedStep(sScenarioStepStore.LookupEntry(SCENARIO_BOA_INDEX_2)))
                    {
                        scenario->SetStepState(sScenarioStepStore.LookupEntry(SCENARIO_BOA_INDEX_2), SCENARIO_STEP_DONE);
                        scenario->CompleteStep(sScenarioStepStore.LookupEntry(SCENARIO_BOA_INDEX_2));
                        scenario->SetStep(sScenarioStepStore.LookupEntry(SCENARIO_BOA_INDEX_3));
                    }
                }
            }
        }
    }
};

struct npc_winter_queen_177280 : public  ScriptedAI
{
public:
    npc_winter_queen_177280(Creature* creature) : ScriptedAI(creature) {}
private:
    bool scene_played;

    void Reset() override
    {
        scene_played = false;
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
            if (player->IsInDist(me, 50.0f))
            {
                if (Scenario* scenario = player->GetScenario())
                {
                    if (!scene_played)
                    {
                        if (scenario->GetStep() == sScenarioStepStore.LookupEntry(SCENARIO_BOA_INDEX_3))
                        {
                            scene_played = true;
                            player->CastSpell(player, SPELL_SCENE_DISPELLING_DECOYS);
                        }
                    }
                }
            }
    }
};


static const Position SylvanasPositions[4] =
{
    { -6161.75f, 857.801f, 5463.71f, 6.142f },
    { -6141.11f, 879.569f, 5471.89f, 0.221f },
    { -6200.17f, 841.566f, 5521.25f, 3.173f },
    { -6275.61f, 822.631f, 5619.05f, 4.033f },
};

static const Position plaguePositions[2] =
{
    { -6157.61f, 816.401f, 5463.41f, 1.531f },
    { -6145.47f, 843.981f, 5451.52f, 1.052f },
};


// 177114 Sylvanas Windrunner
struct npc_sylvanas_windrunner_177114 : public  ScriptedAI
{
public:
    npc_sylvanas_windrunner_177114(Creature* creature) : ScriptedAI(creature) {}

private:
    bool event_start;
    EventMap _events;
    ObjectGuid m_playerGUID;
    ObjectGuid kleiaGUID;
    ObjectGuid herneGUID;

    void Reset() override
    {
        m_playerGUID.Clear();
        herneGUID.Clear();
        kleiaGUID.Clear();
        _events.Reset();
        event_start = false;
    }

    void UpdateAI(uint32 diff) override
    {
        _events.Update(diff);

        while (uint32 eventId = _events.ExecuteEvent())
        {
            if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
            {
                switch (eventId)
                {
                case   EVENT_BANSHEE_QUEEN_PHASE1:
                {
                    player->CastSpell(player, SPELL_SYLVANAS_PINNED_PLAYER, false);

                    me->AddDelayedEvent(5000, [this, player]() -> void
                        {
                            if (Creature* kleia = me->FindNearestCreature(177123, 60.0f))
                            {
                                kleia->SetWalk(false);
                                kleia->GetMotionMaster()->MovePoint(0, player->GetPosition(), true);
                            }
                            me->CastSpell(me, SPELL_SYLVANAS_BANSHEE_LEAP, false);
                        });
                    me->AddDelayedEvent(8000, [this]() -> void
                        {
                            if (Creature* kleia = me->FindNearestCreature(177123, 60.0f))
                            {
                                kleia->AI()->Talk(3);
                                kleia->SetStandState(UNIT_STAND_STATE_KNEEL);
                                kleia->CastSpell(kleia, SPELL_KLEIA_SHIELD, false);

                            }
                            me->CastSpell(me, SPELL_BANSHEE_FORM, false);
                        });
                    me->AddDelayedEvent(14000, [this, player]() -> void
                        {
                            if (Creature* kleia = me->FindNearestCreature(177123, 60.0f))
                                if (Creature* herne = me->FindNearestCreature(NPC_LOAR_HERNE, 60.0f))
                                    if (Creature* niya = me->FindNearestCreature(NPC_NIYA, 60.0f))
                                        if (Creature* korayn = me->FindNearestCreature(NPC_KORAYN, 60.0f))
                                        {
                                            herneGUID = herne->GetGUID();
                                            kleiaGUID = kleia->GetGUID();
                                            Talk(1);
                                            me->GetMotionMaster()->MoveSmoothPath(1002, SylvanasPositions, 4, false, true);
                                            player->SummonCreature(177261, -6124.75f, 787.124f, 5484.71f, 2.243f, TEMPSUMMON_MANUAL_DESPAWN);
                                            herne->AddAura(29266, herne);
                                            niya->AddAura(29266, niya);
                                            korayn->AddAura(29266, korayn);
                                            herne->DespawnOrUnsummon(30s, 5s);
                                            niya->DespawnOrUnsummon(30s, 5s);
                                            korayn->DespawnOrUnsummon(30s, 5s);
                                        }
                            if (Scenario* scenario = player->GetScenario())
                            {
                                scenario->CompleteStep(sScenarioStepStore.LookupEntry(SCENARIO_BOA_INDEX_3));
                                scenario->SetStep(sScenarioStepStore.LookupEntry(SCENARIO_BOA_INDEX_4));
                            }
                        });
                    me->AddDelayedEvent(20000, [this, player]() -> void
                        {
                            if (Creature* kleia = ObjectAccessor::GetCreature(*me, kleiaGUID))
                            {
                                kleia->SetHover(false);
                                kleia->SetFlying(false);
                                kleia->SetCanFly(false);
                                kleia->GetMotionMaster()->MoveTargetedHome();
                            }
                        });
                    me->AddDelayedEvent(23000, [this]() -> void
                        {
                            if (Creature* kleia = ObjectAccessor::GetCreature(*me, kleiaGUID))
                            {
                                kleia->CastSpell(kleia, SPELL_KYRIAN_AOE_HEAL, false);
                                if (Creature* herne = ObjectAccessor::GetCreature(*me, herneGUID))
                                    herne->AI()->Talk(2);
                            }
                        });
                }
                break;
                }
            }

        }
    }

    void IsSummonedBy(WorldObject* summoner) override
    {
        m_playerGUID = summoner->GetGUID();
    }

    void JustEngagedWith(Unit* victim) override
    {
        if (Player* player = victim->ToPlayer())
        {
            if (!event_start)
            {
                event_start = true;
                Talk(0);
                me->SetUnitFlag(UNIT_FLAG_IMMUNE_TO_PC);
                me->CombatStop();
                _events.ScheduleEvent(EVENT_BANSHEE_QUEEN_PHASE1, 1s);
            }
        }
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        switch (id)
        {
        case 1002:
            me->ForcedDespawn(15000);
            break;
        }
    }
};

// 177261 Plague
struct npc_plague_177261 : public  ScriptedAI
{
public:
    npc_plague_177261(Creature* creature) : ScriptedAI(creature) {}

    void IsSummonedBy(WorldObject* summoner) override
    {
        if (Player* player = summoner->ToPlayer())
        {
            if (player->GetAreaId() == 12244)
            {
                me->SetNpcFlag(UNIT_NPC_FLAG_SPELLCLICK);
                me->GetMotionMaster()->MoveSmoothPath(1003, plaguePositions, 2, false, true);
            }
        }
    }

    void OnSpellClick(Unit* clicker, bool /*spellClickHandled*/) override
    {
        if (Player* player = clicker->ToPlayer())
        {
            player->SummonCreature(177583, player->GetPosition(), TEMPSUMMON_MANUAL_DESPAWN);
            if (Scenario* scenario = player->GetScenario())
            {
                scenario->CompleteStep(sScenarioStepStore.LookupEntry(SCENARIO_BOA_INDEX_4));
                scenario->SetStep(sScenarioStepStore.LookupEntry(SCENARIO_BOA_INDEX_5));
            }
            me->ForcedDespawn(1000);
        }
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        switch (id)
        {
        case 1003:
            Talk(1);
            break;
        }
    }
};

// 177583 Trained Flayedwing
struct npc_trained_flayedwing_177583 : public  ScriptedAI
{
public:
    npc_trained_flayedwing_177583(Creature* creature) : ScriptedAI(creature) { Reset(); }

private:
    Vehicle* vehicle;
    EventMap _events;
    ObjectGuid m_playerGUID;
    ObjectGuid yseraGUID;
    uint8    m_gamePhase;
    bool     m_isInitialised;
    TaskScheduler _scheduler;

    void OnCharmed(bool /*apply*/) override { }

    void Reset() override
    {
        _events.Reset();
        m_playerGUID.Clear();
        yseraGUID.Clear();
        m_gamePhase = 0;
        m_isInitialised = false;
    }

    void IsSummonedBy(WorldObject* summoner) override
    {
        m_playerGUID = summoner->GetGUID();

        if (Player* player = summoner->ToPlayer())
        {
            if (player->HasQuest(QUEST_THE_BATTLE_OF_ARDENWEALD))
            {
                vehicle = me->GetVehicleKit();
                player->EnterVehicle(me);
                me->SummonCreatureGroup(0);
                if (Creature* ysera = player->SummonCreature(177502, -6213.34f, 830.273f, 5555.11f, 3.10799f, TEMPSUMMON_TIMED_DESPAWN, 120s))
                    yseraGUID = ysera->GetGUID();
            }
        }
    }

    void PassengerBoarded(Unit* who, int8 /*seatId*/, bool apply) override
    {
        if (apply)
        {
            if (Player* player = who->ToPlayer())
            {
                player->SetCanFly(true);
                me->SetFlying(true);
                me->SetDisableGravity(true);
                player->SetClientControl(me, true);
                me->GetMotionMaster()->MovePath(17758300, false);
                m_gamePhase = 1;
            }
        }
    }

    void UpdateAI(uint32 diff) override
    {
        _scheduler.Update(diff);
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        if (type == WAYPOINT_MOTION_TYPE)
        {
            if (m_gamePhase == 1 && id == 1)
            {
                if (Creature* ysera = ObjectAccessor::GetCreature(*me, yseraGUID))
                {
                    ysera->CastSpell(ysera, 350951, false);
                    _scheduler.Schedule(12s, [ysera](TaskContext task)
                        {
                            ysera->CastSpell(ysera, 350951, false);
                            task.Repeat(12s);
                        });
                }
            }
            else if (m_gamePhase == 1 && id == 9)
            {
                me->SummonCreature(177113, -6268.31f, 906.159f, 5597.94f, 4.5109f, TEMPSUMMON_TIMED_DESPAWN, 120s);
                if (Unit* creature1 = me->GetVehicleKit()->GetPassenger(0))
                    if (Creature* plague = creature1->ToCreature())
                        plague->AI()->Talk(0);
                _scheduler.Schedule(5s, [this](TaskContext task)
                    {
                        if (Creature* ysera = ObjectAccessor::GetCreature(*me, yseraGUID))
                            ysera->AI()->Talk(0);
                    });
            }
            else if (m_gamePhase == 1 && id == 13)
            {
                if (Unit* creature1 = me->GetVehicleKit()->GetPassenger(0))
                    if (Creature* plague = creature1->ToCreature())
                        plague->AI()->Talk(1);
            }
            else if (m_gamePhase == 1 && id == 16)
            {

                if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                    if (Scenario* scenario = player->GetScenario())
                    {
                        player->SummonCreature(177123, -6264.51f, 942.156f, 5591.05f, 4.342f, TEMPSUMMON_TIMED_DESPAWN, 360s);
                        player->SummonCreature(NPC_MISTBLADE, -6252.54f, 933.321f, 5589.11f, 3.961f, TEMPSUMMON_TIMED_DESPAWN, 360s);
                        player->SummonCreature(177250, -6258.84f, 939.239f, 5589.92f, 4.236f, TEMPSUMMON_TIMED_DESPAWN, 360s);

                        scenario->CompleteStep(sScenarioStepStore.LookupEntry(SCENARIO_BOA_INDEX_5));
                        scenario->SetStep(sScenarioStepStore.LookupEntry(SCENARIO_BOA_INDEX_6));
                    }
                if (Vehicle* bat = me->GetVehicleKit())
                    bat->RemoveAllPassengers();
                me->ForcedDespawn(1000);
            }

        }
    }
};

static const Position runFraxinPositions = { -6268.31f, 906.159f, 5597.94f, 4.5109f };
static const Position spawnNpcPositions = { -6253.12f, 944.156f, 5588.75f, 4.2871f };

// 177123 Kleia
struct npc_Kleia_177123 : public  EscortAI
{
public:
    npc_Kleia_177123(Creature* creature) : EscortAI(creature) {}
private:
    EventMap _events;
    ObjectGuid m_playerGUID;
    ObjectGuid mistbladeGUID;
    ObjectGuid dravenGUID;

    void Reset() override
    {
        _events.Reset();
        m_playerGUID.Clear();
        mistbladeGUID.Clear();
        dravenGUID.Clear();
    }

    void EnterEvadeMode(EvadeReason /*reason*/) override {}

    void IsSummonedBy(WorldObject* summoner) override
    {
        m_playerGUID = summoner->GetGUID();
        _events.ScheduleEvent(EVENT_TO_THE_TOP_OF_THE_FALLS_PHASE1, 2s);
    }

    void DoAction(int32 action) override
    {
        switch (action)
        {
        case EVENT_TO_THE_TOP_OF_THE_FALLS_PHASE4:
        {
            _events.ScheduleEvent(EVENT_TO_THE_TOP_OF_THE_FALLS_PHASE4, 1s);
        }
        break;
        }
    }

    void UpdateAI(uint32 diff) override
    {
        ScriptedAI::UpdateAI(diff);
        _events.Update(diff);

        while (uint32 eventId = _events.ExecuteEvent())
        {
            if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
            {
                switch (eventId)
                {
                case EVENT_TO_THE_TOP_OF_THE_FALLS_PHASE1:
                {
                    if (Creature* soulburner = me->FindNearestCreature(177159, 100.0f, true))
                        if (Creature* queen_decoy = me->FindNearestCreature(177113, 100.0f, true))
                        {
                            soulburner->HandleEmoteCommand(EMOTE_ONESHOT_ATTACK_UNARMED);
                            queen_decoy->KillSelf();
                            queen_decoy->ForcedDespawn(1500);
                        }
                    _events.ScheduleEvent(EVENT_TO_THE_TOP_OF_THE_FALLS_PHASE2, 2s);
                    break;
                }
                case EVENT_TO_THE_TOP_OF_THE_FALLS_PHASE2:
                {
                    Talk(4);
                    if (Creature* mistblade = me->FindNearestCreature(NPC_MISTBLADE, 50.0f, true))
                        if (Creature* draven = me->FindNearestCreature(177250, 50.0f, true))
                        {
                            me->SetWalk(false);
                            mistblade->SetWalk(false);
                            draven->SetWalk(false);
                            me->GetMotionMaster()->MovePoint(0, runFraxinPositions, true);
                            mistblade->GetMotionMaster()->MovePoint(0, runFraxinPositions, true);
                            draven->GetMotionMaster()->MovePoint(0, runFraxinPositions, true);
                            if (int32 playerFaction = player->GetFaction())
                            {
                                me->SetFaction(playerFaction);
                                mistblade->SetFaction(playerFaction);
                                draven->SetFaction(playerFaction);
                            }
                        }
                    _events.ScheduleEvent(EVENT_TO_THE_TOP_OF_THE_FALLS_PHASE3, 4s);
                    break;
                }
                case EVENT_TO_THE_TOP_OF_THE_FALLS_PHASE3:
                {
                    if (Creature* mistblade = me->FindNearestCreature(NPC_MISTBLADE, 50.0f, true))
                        if (Creature* draven = me->FindNearestCreature(177250, 50.0f, true))
                            if (Creature* soulburner = me->FindNearestCreature(177159, 30.0f, true))
                            {
                                soulburner->SetFaction(14);
                                soulburner->RemoveUnitFlag(UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);
                                soulburner->GetThreatManager().AddThreat(draven, 10000.0f);
                                me->SetFlying(false);
                                me->SetDisableGravity(false);
                                me->GetThreatManager().AddThreat(soulburner, 10000.0f);
                                draven->GetThreatManager().AddThreat(soulburner, 10000.0f);
                                mistblade->GetThreatManager().AddThreat(soulburner, 10000.0f);
                                AttackStart(soulburner);
                                mistblade->AI()->DoAction(ACTION_ATTACK_SOULBURNER);
                                draven->AI()->DoAction(ACTION_ATTACK_SOULBURNER);
                            }
                }
                break;

                case EVENT_TO_THE_TOP_OF_THE_FALLS_PHASE4:
                {
                    Conversation::CreateConversation(17017, player, player->GetPosition(), { player->GetGUID() });  //Sylvanas say.

                    if (Scenario* scenario = player->GetScenario())
                    {
                        scenario->CompleteStep(sScenarioStepStore.LookupEntry(SCENARIO_BOA_INDEX_6));
                        scenario->SetStep(sScenarioStepStore.LookupEntry(SCENARIO_BOA_INDEX_7));
                    }
                    if (Creature* mistblade = me->FindNearestCreature(NPC_MISTBLADE, 50.0f, true))
                        if (Creature* draven = me->FindNearestCreature(177250, 50.0f, true))
                        {
                            me->SetFlying(false);
                            me->GetMotionMaster()->MovePoint(0, -6274.86f, 911.531f, 5589.21f, true, 1.3418f);
                            mistblade->GetMotionMaster()->MovePoint(0, -6268.64f, 910.59f, 5587.89f, true, 1.2633f);
                            draven->GetMotionMaster()->MovePoint(0, -6263.74f, 909.214f, 5588.07f, true, 1.6285f);

                            if (Creature* Moonberry = player->SummonCreature(177141, spawnNpcPositions, TEMPSUMMON_TIMED_DESPAWN, 120s))
                            {
                                Moonberry->SetWalk(false);
                                Moonberry->GetMotionMaster()->MovePoint(0, -6270.96f, 897.036f, 5588.59f, true, 1.64033f);
                                me->AddDelayedEvent(10000, [Moonberry]() -> void
                                    {
                                        Moonberry->AI()->Talk(0);
                                    });
                            }
                            if (Creature* Dreamweaver = player->SummonCreature(177274, spawnNpcPositions, TEMPSUMMON_TIMED_DESPAWN, 120s))
                            {
                                Dreamweaver->SetWalk(false);
                                Dreamweaver->GetMotionMaster()->MovePoint(0, -6264.82f, 916.117f, 5588.16f, true, 1.38507f);
                            }
                            if (Creature* Shandris = player->SummonCreature(178642, spawnNpcPositions, TEMPSUMMON_TIMED_DESPAWN, 120s))
                            {
                                Shandris->SetWalk(false);
                                Shandris->RemoveAura(352592);
                                Shandris->GetMotionMaster()->MovePoint(0, -6271.37f, 921.019f, 5589.52f, true, 1.77779f);
                            }
                            if (Creature* Jaina = player->SummonCreature(177231, spawnNpcPositions, TEMPSUMMON_TIMED_DESPAWN, 120s))
                            {
                                Jaina->SetWalk(false);
                                Jaina->GetMotionMaster()->MovePoint(0, -6262.74f, 920.163f, 5588.59f, true, 1.56966f);
                                me->AddDelayedEvent(7000, [Jaina]() -> void
                                    {
                                        Jaina->AddAura(SPELL_ARCANE_BARRIER, Jaina);
                                    });
                            }
                        }
                }
                break;
                }

            }
        }
        DoMeleeAttackIfReady();
    }
};

// 177205 mistblade
struct npc_mistblade_177205 : public  ScriptedAI
{
public:
    npc_mistblade_177205(Creature* creature) : ScriptedAI(creature) {}

    void DoAction(int32 param) override
    {
        if (param == ACTION_ATTACK_SOULBURNER)
        {
            if (Creature* soulburner = me->FindNearestCreature(177159, 30.0f, true))
                AttackStart(soulburner);
        }
    }
};

// 177250 draven
struct npc_draven_177250 : public  ScriptedAI
{
public:
    npc_draven_177250(Creature* creature) : ScriptedAI(creature) {}

    void DoAction(int32 param) override
    {
        if (param == ACTION_ATTACK_SOULBURNER)
        {
            if (Creature* soulburner = me->FindNearestCreature(177159, 30.0f, true))
                AttackStart(soulburner);
        }
    }
};

// 177159 Soulburner Fraxin
class npc_soulburner_fraxin_177159 : public CreatureScript
{
public:
    npc_soulburner_fraxin_177159() : CreatureScript("npc_soulburner_fraxin_177159") { }

    struct npc_soulburner_fraxin_177159AI : public  BossAI
    {
        npc_soulburner_fraxin_177159AI(Creature* creature) : BossAI(creature, DATA_FRAXIN) { }
    private:
        ObjectGuid kleiaGUID;


        void Reset() override
        {
            kleiaGUID = ObjectGuid::Empty;
        }

        void JustEngagedWith(Unit* victim) override
        {
            if (Creature* kleia = me->FindNearestCreature(NPC_KLEIA, 50.0f, true))
                kleiaGUID = kleia->GetGUID();
        }

        void DamageTaken(Unit* attacker, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/) override
        {
            if (attacker->ToCreature())
                if (me->GetHealth() <= damage || me->GetHealthPct() <= 10.0f)
                    damage = 0;
        }

        void JustDied(Unit* killer) override
        {

            _JustDied();
            InstanceScript* instance = me->GetInstanceScript();
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            if (Creature* kleia = ObjectAccessor::GetCreature(*me, kleiaGUID))
                kleia->AI()->DoAction(EVENT_TO_THE_TOP_OF_THE_FALLS_PHASE4);
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_soulburner_fraxin_177159AI(creature);
    }
};

// 177630 Forest Guardian
struct npc_forest_guardian_177630 : public  ScriptedAI
{
public:
    npc_forest_guardian_177630(Creature* creature) : ScriptedAI(creature) {}


    uint32 waitTime;
    ObjectGuid mawswornTarget;

    void Reset() override
    {
        mawswornTarget = ObjectGuid::Empty;
        me->SetSheath(SHEATH_STATE_MELEE);
        waitTime = urand(0, 2000);
    }

    void DamageTaken(Unit* attacker, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/) override
    {
        if (attacker->ToCreature())
            if (me->GetHealth() <= damage || me->GetHealthPct() <= 80.0f)
                damage = 0;
    }

    void DamageDealt(Unit* target, uint32& damage, DamageEffectType /*damageType*/) override
    {
        if (target->ToCreature())
            if (target->GetHealth() <= damage || target->GetHealthPct() <= 80.0f)
                damage = 0;
    }

    void UpdateAI(uint32 diff) override
    {
        DoMeleeAttackIfReady();

        if (waitTime && waitTime >= diff)
        {
            waitTime -= diff;
            return;
        }

        waitTime = urand(10000, 20000);

        if (!mawswornTarget.IsEmpty())
        {
            if (Creature* mawsworn = ObjectAccessor::GetCreature(*me, mawswornTarget))
            {
                if (mawsworn->IsAlive())
                {
                    if (me->GetVictim() != mawsworn)
                    {
                        me->GetThreatManager().AddThreat(mawsworn, 1000000.0f);
                        mawsworn->GetThreatManager().AddThreat(me, 1000000.0f);
                        me->Attack(mawsworn, true);
                    }
                }
                else
                {
                    mawsworn->DespawnOrUnsummon();
                    mawswornTarget = ObjectGuid::Empty;
                }
            }
        }
        else
        {
            switch (uint8 rand = urand(0, 1))
            {
            case 0:
                SummonMawsworn(NPC_MAWSWORN_PILLAGER);
                break;
            case 1:
                SummonMawsworn(NPC_MAWSWORN_ERADICATOR);
                break;
            }
        }
    }

    void SummonMawsworn(uint32 creature)
    {

        Position mawswornPos = me->GetPosition();
        GetPositionWithDistInFront(me, 2.5f, mawswornPos);

        float z = me->GetMap()->GetHeight(me->GetPhaseShift(), mawswornPos.GetPositionX(), mawswornPos.GetPositionY(), mawswornPos.GetPositionZ());
        mawswornPos.m_positionZ = z;


        if (Creature* mawsworn = me->SummonCreature(creature, mawswornPos))

        {
            me->GetThreatManager().AddThreat(mawsworn, 1000000.0f);
            mawsworn->GetThreatManager().AddThreat(me, 1000000.0f);
            AttackStart(mawsworn);
            mawsworn->SetFacingToObject(me);

            if (mawsworn->GetEntry() == NPC_MAWSWORN_PILLAGER)
                mawsworn->SetEmoteState(EMOTE_STATE_ATTACK_UNARMED);
            else
                mawsworn->SetEmoteState(EMOTE_STATE_ATTACK1H);

            mawswornTarget = mawsworn->GetGUID();
        }
    }
};

// 178425 WildSeeds
struct npc_wildseeds : public  ScriptedAI
{
public:
    npc_wildseeds(Creature* creature) : ScriptedAI(creature) {}

    void JustAppeared() override
    {
        SummonEradicator();
    }

    void OnSpellClick(Unit* clicker, bool /*result*/) override
    {
        if (Player* player = clicker->ToPlayer())
        {
            player->CastSpell(player, SPELL_RESCUING, false);
            player->UpdateCriteria(CriteriaType::AnyoneTriggerGameEventScenario, 0, 0, 0, player);
            me->RemoveNpcFlag(UNIT_NPC_FLAG_SPELLCLICK);
            me->DespawnOrUnsummon(10s, 60s);
            save_seed_num = save_seed_num + 1;
            if (Scenario* scenario = player->GetScenario())
            {
                if (scenario->GetStepState(sScenarioStepStore.LookupEntry(SCENARIO_BOA_INDEX_7)) == ScenarioStepState::SCENARIO_STEP_IN_PROGRESS)
                {
                    if (save_seed_num >= 4)
                    {
                        Conversation::CreateConversation(17020, player, player->GetPosition(), { player->GetGUID() });
                        scenario->CompleteStep(sScenarioStepStore.LookupEntry(SCENARIO_BOA_INDEX_7));
                        scenario->SetStep(sScenarioStepStore.LookupEntry(SCENARIO_BOA_INDEX_8));
                    }
                }
            }
        }


        if (save_seed_num < 6)
        {
            switch (save_seed_num)
            {
            case 1:
                summoncovenant(NPC_WILD_HUNTER_GUARDIAN);
                break;
            case 2:
                summoncovenant(NPC_SINFALL_DEFENDER);
                break;
            case 3:
                summoncovenant(NPC_MALDRAXXI_GLADIATOR);
                break;
            case 4:
                summoncovenant(NPC_KYRIAN_ASCENDANT);
                break;
            case 5:
                summoncovenant(NPC_MALDRAXXI_LINEGUARD);
                save_seed_num = 0;
                break;
            }

        }
    }

    void summoncovenant(uint32 creature)
    {
        Position covenantPos = me->GetPosition();
        GetPositionWithDistInFront(me, 40.0f, covenantPos);

        if (Creature* covenant = me->SummonCreature(creature, covenantPos))
            if (Creature* enchanter = covenant->SummonCreature(NPC_WILD_HUNTER_ENCHANTER, covenantPos))
            {
                covenant->SetFacingToObject(me);
                covenant->SetWalk(false);
                covenant->GetMotionMaster()->MovePoint(0, me->GetPosition(), true);
                covenant->AI()->Talk(0);
                enchanter->GetMotionMaster()->MoveFollow(covenant, PET_FOLLOW_DIST, covenant->GetFollowAngle());
                covenant->ForcedDespawn(30000);
                enchanter->ForcedDespawn(30000);
            }
    }

    void SummonEradicator()
    {

        Position mawswornPos = me->GetPosition();
        GetPositionWithDistInFront(me, 4.5f, mawswornPos);

        if (Creature* mawsworn = me->SummonCreature(NPC_MAWSWORN_ERADICATOR, mawswornPos))
        {
            mawsworn->SetFacingToObject(me);
            mawsworn->SetEmoteState(EMOTE_STATE_ATTACK1H);
        }
    }

};

Position const lord_herne_path[4] =
{
    {-6499.46f, 881.341f, 5629.23f, 0},
    {-6449.27f, 851.479f, 5615.71f, 0},
    {-6404.87f, 808.224f, 5614.99f, 0},
    {-6388.91f, 777.293f, 5618.72f, 4.60572f},
};

Position const lady_moonberry_path[3] =
{
    {-6405.29f, 852.775f, 5629.36f, 0},
    {-6396.57f, 805.258f, 5626.95f, 0},
    {-6392.08f, 780.643f, 5626.25f, 4.90817f},
};

// 178440 rally point
struct npc_rally_point_178440 : public  ScriptedAI
{
public:
    npc_rally_point_178440(Creature* creature) : ScriptedAI(creature) {}

private:
    EventMap _events;
    ObjectGuid m_playerGUID;
    uint8 incounter_realmbreaker_number = 3;

    void Reset() override
    {
        _events.Reset();
        m_playerGUID.Clear();
    }

    void OnSpellClick(Unit* clicker, bool /*result*/) override
    {
        if (Player* player = clicker->ToPlayer())
        {
            if (me->GetSpawnId() == 346215)
                _events.ScheduleEvent(EVENT_THE_FOREST_ANSWERS_SOUTH, 2s);
            else
                _events.ScheduleEvent(EVENT_THE_FOREST_ANSWERS_NORTH, 2s);
            player->UpdateCriteria(CriteriaType::AnyoneTriggerGameEventScenario, 0, 0, 0, player);
            me->RemoveNpcFlag(UNIT_NPC_FLAG_SPELLCLICK);
            me->DespawnOrUnsummon(30s, 60s);
            rally_point_num = rally_point_num + 1;
            if (Scenario* scenario = player->GetScenario())
            {
                if (rally_point_num >= 2)
                {
                    if (scenario->GetStepState(sScenarioStepStore.LookupEntry(SCENARIO_BOA_INDEX_8)) == ScenarioStepState::SCENARIO_STEP_IN_PROGRESS)
                    {
                        scenario->CompleteStep(sScenarioStepStore.LookupEntry(SCENARIO_BOA_INDEX_8));
                        scenario->SetStep(sScenarioStepStore.LookupEntry(SCENARIO_BOA_INDEX_9));
                    }
                }
            }
        }

        if (rally_point_num < 3)
        {
            switch (rally_point_num)
            {
            case 1:
                break;
            case 2:
                me->AddDelayedEvent(28000, [this]()
                    {
                        if (Creature* realmbreaker1 = me->SummonCreature(NPC_REALMBREAKER, -6391.21f, 760.095f, 5619.91f, 1.0183f, TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            for (uint8 uiSummon_counter = 0; uiSummon_counter < incounter_realmbreaker_number; ++uiSummon_counter)
                            {
                                Unit* temp = me->SummonCreature(NPC_REALMBREAKER, (realmbreaker1->GetPositionX() - 15) + rand32() % 5, (realmbreaker1->GetPositionY() - 15) + rand32() % 5, realmbreaker1->GetPositionZ(), 0, TEMPSUMMON_MANUAL_DESPAWN);
                            }
                        }
                    });
                rally_point_num = 0;
                break;
            }
        }
    }

    void UpdateAI(uint32 diff) override
    {
        ScriptedAI::UpdateAI(diff);
        _events.Update(diff);

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_THE_FOREST_ANSWERS_SOUTH:
            {
                if (Creature* herne = me->SummonCreature(NPC_LOAR_HERNE, -6523.11f, 896.098f, 5627.53f, 5.5756f, TEMPSUMMON_TIMED_DESPAWN, 23s))
                {
                    herne->GetMotionMaster()->MoveSmoothPath(4, lord_herne_path, 4, false, false);
                    if (Creature* Korayn = me->SummonCreature(NPC_KORAYN, -6521.11f, 894.098f, 5627.53f, 5.5756f, TEMPSUMMON_TIMED_DESPAWN, 23s))
                        Korayn->GetMotionMaster()->MoveFollow(herne, PET_FOLLOW_DIST, herne->GetFollowAngle());
                    if (Creature* Mistblade = me->SummonCreature(NPC_MISTBLADE, -6519.11f, 892.098f, 5627.53f, 5.5756f, TEMPSUMMON_TIMED_DESPAWN, 23s))
                        Mistblade->GetMotionMaster()->MoveFollow(herne, PET_FOLLOW_DIST, herne->GetFollowAngle());
                    if (Creature* Niya = me->SummonCreature(NPC_NIYA, -6525.11f, 898.098f, 5627.53f, 5.5756f, TEMPSUMMON_TIMED_DESPAWN, 23s))
                        Niya->GetMotionMaster()->MoveFollow(herne, PET_FOLLOW_DIST, herne->GetFollowAngle());

                    me->AddDelayedEvent(5000, [herne]()
                        {
                            herne->AI()->Talk(3);
                        });
                }
                me->AddDelayedEvent(23000, [this]()
                    {
                        summonherneGrup();
                    });
                break;
            }
            case EVENT_THE_FOREST_ANSWERS_NORTH:
            {
                if (Creature* moonverry = me->SummonCreature(NPC_MOONBERRY2, -6410.28f, 880.782f, 5637.51f, 4.9317f, TEMPSUMMON_TIMED_DESPAWN, 18s))
                {
                    moonverry->GetMotionMaster()->MoveSmoothPath(3, lady_moonberry_path, 3, false, true);

                    if (Creature* kleia = me->SummonCreature(NPC_KLEIA2, -6410.28f, 880.782f, 5637.51f, 4.9317f, TEMPSUMMON_TIMED_DESPAWN, 18s))
                        kleia->GetMotionMaster()->MoveFollow(moonverry, PET_FOLLOW_DIST, moonverry->GetFollowAngle());
                    if (Creature* dreamweaver = me->SummonCreature(NPC_DREAMWEAVER, -6410.28f, 880.782f, 5637.51f, 4.9317f, TEMPSUMMON_TIMED_DESPAWN, 18s))
                        dreamweaver->GetMotionMaster()->MoveFollow(moonverry, PET_FOLLOW_DIST, moonverry->GetFollowAngle());
                    if (Creature* draven = me->SummonCreature(NPC_DRAVEN2, -6410.28f, 880.782f, 5637.51f, 4.9317f, TEMPSUMMON_TIMED_DESPAWN, 18s))
                        draven->GetMotionMaster()->MoveFollow(moonverry, PET_FOLLOW_DIST, moonverry->GetFollowAngle());
                    me->AddDelayedEvent(5000, [moonverry]()
                        {
                            moonverry->AI()->Talk(0);
                        });
                }
                me->AddDelayedEvent(18000, [this]()
                    {
                        summonmoonberryGrup();
                    });
                break;
            }
            }
        }
    }

    void summonmoonberryGrup()
    {
        me->SummonCreature(NPC_MOONBERRY2, -6392.08f, 780.643f, 5626.25f, 4.90817f, TEMPSUMMON_TIMED_DESPAWN, 1200s);
        me->SummonCreature(NPC_KLEIA2, -6395.08f, 781.643f, 5626.25f, 4.90817f, TEMPSUMMON_TIMED_DESPAWN, 1200s);
        me->SummonCreature(NPC_DREAMWEAVER, -6398.08f, 782.643f, 5626.25f, 4.90817f, TEMPSUMMON_TIMED_DESPAWN, 1200s);
        me->SummonCreature(NPC_DRAVEN2, -6389.08f, 779.643f, 5626.25f, 4.90817f, TEMPSUMMON_TIMED_DESPAWN, 1200s);
    }

    void summonherneGrup()
    {
        me->SummonCreature(NPC_LOAR_HERNE, -6391.31f, 773.966f, 5619.2f, 4.70786f, TEMPSUMMON_TIMED_DESPAWN, 1200s);
        me->SummonCreature(NPC_MISTBLADE, -6388.08f, 776.976f, 5618.75f, 4.82175f, TEMPSUMMON_TIMED_DESPAWN, 1200s);
        me->SummonCreature(NPC_NIYA, -6391.31f, 777.406f, 5618.72f, 4.80997f, TEMPSUMMON_TIMED_DESPAWN, 1200s);
        me->SummonCreature(NPC_KORAYN, -6393.95f, 777.54f, 5618.63f, 4.75106f, TEMPSUMMON_TIMED_DESPAWN, 1200s);
    }
};


// 177600 Mawsworn Realmbreaker
struct npc_mawsworn_malmbreaker_177600 : public  ScriptedAI
{
public:
    npc_mawsworn_malmbreaker_177600(Creature* creature) : ScriptedAI(creature) {}

private:
    ObjectGuid m_playerGUID;
    ObjectGuid destroyerGUID;

    void Reset() override
    {
        m_playerGUID = ObjectGuid::Empty;
        destroyerGUID = ObjectGuid::Empty;
    }

    void SetData(uint32 type, uint32 data) override
    {
        if (type = !DATA_REALMBREAKER)
            return;

        if (data == DATA_REALMBREAKER_DEAD)
        {
            ++death_point;
            if (death_point < 9)
            {
                switch (death_point)
                {
                case 4:
                    if (Creature* realmbreaker1 = me->SummonCreature(NPC_REALMBREAKER, -6391.21f, 760.095f, 5619.91f, 1.0183f, TEMPSUMMON_MANUAL_DESPAWN))
                    {
                        for (uint8 uiSummon_counter = 0; uiSummon_counter < 3; ++uiSummon_counter)
                        {
                            Unit* temp = me->SummonCreature(NPC_INVOKER, (realmbreaker1->GetPositionX() - 15) + rand32() % 5, (realmbreaker1->GetPositionY() - 15) + rand32() % 5, realmbreaker1->GetPositionZ(), 0, TEMPSUMMON_MANUAL_DESPAWN);
                        }
                    }
                    break;
                case 8:
                    me->SummonCreature(NPC_DESTROYER, -6391.21f, 760.095f, 5619.91f, 1.0183f, TEMPSUMMON_MANUAL_DESPAWN);
                    if (Creature* destroyer = ObjectAccessor::GetCreature(*me, destroyerGUID))
                        destroyer->AI()->SetGUID(m_playerGUID, PLAYER_GUID);
                    death_point = 1;
                    break;
                }
            }
        }
    }

    void IsSummonedBy(WorldObject* summoner) override
    {
        if (Creature* henre = me->FindNearestCreature(NPC_LOAR_HERNE, 50.0f))
        {
            me->GetThreatManager().AddThreat(henre, 100000.0f);
            henre->GetThreatManager().AddThreat(me, 100000.0f);
            AttackStart(henre);
        }
    }

    void JustSummoned(Creature* summon) override
    {
        summons.Summon(summon);

        switch (summon->GetEntry())
        {
        case NPC_DESTROYER:
            destroyerGUID = summon->GetGUID();
            summon->AI()->SetGUID(me->GetGUID(), me->GetEntry());
            break;
        }
    }

    void DamageTaken(Unit* attacker, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/) override
    {
        if (Player* player = attacker->ToPlayer())
            m_playerGUID = player->GetGUID();
    }

    void DamageDealt(Unit* target, uint32& damage, DamageEffectType /*damageType*/) override
    {
        if (target->ToCreature())
            if (target->GetHealth() <= damage || target->GetHealthPct() <= 80.0f)
                damage = 0;
    }

    void JustDied(Unit* killer) override
    {
        me->AI()->SetData(DATA_REALMBREAKER, DATA_REALMBREAKER_DEAD);
    }
};


// 177601 Mawsworn Invoker
struct npc_mawsworn_invoker_177601 : public  ScriptedAI
{
public:
    npc_mawsworn_invoker_177601(Creature* creature) : ScriptedAI(creature) {}

    void IsSummonedBy(WorldObject* summoner) override
    {
        if (Creature* henre = me->FindNearestCreature(NPC_LOAR_HERNE, 50.0f))
        {
            me->GetThreatManager().AddThreat(henre, 100000.0f);
            henre->GetThreatManager().AddThreat(me, 100000.0f);
            AttackStart(henre);
        }
    }

    void DamageDealt(Unit* target, uint32& damage, DamageEffectType /*damageType*/) override
    {
        if (target->ToCreature())
            if (target->GetHealth() <= damage || target->GetHealthPct() <= 80.0f)
                damage = 0;
    }

    void JustDied(Unit* killer) override
    {
        if (Creature* realmbreaker = instance->GetCreature(DATA_REALMBREAKER))
            realmbreaker->AI()->SetData(DATA_REALMBREAKER, DATA_REALMBREAKER_DEAD);
    }
};

// 177165 mawsworn destroyer
struct npc_mawsworn_destroyer_177165 : public  ScriptedAI
{
public:
    npc_mawsworn_destroyer_177165(Creature* creature) : ScriptedAI(creature) {}
private:
    ObjectGuid m_playerGUID;

    void Reset() override
    {
        m_playerGUID = ObjectGuid::Empty;
    }

    void SetGUID(ObjectGuid const& guid, int32 id) override
    {
        switch (id)
        {
        case PLAYER_GUID:
        {
            m_playerGUID = guid;
            break;
        }
        }
    }

    void JustDied(Unit* killer) override
    {
        if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
        {
            if (Scenario* scenario = player->GetScenario())
                if (scenario->GetStepState(sScenarioStepStore.LookupEntry(SCENARIO_BOA_INDEX_9)))
                    scenario->CompleteStep(sScenarioStepStore.LookupEntry(SCENARIO_BOA_INDEX_9));
            player->CastSpell(player, SPELL_MOVIE_TYRANDE_VS_SYLBANAS, false);
            player->ForceCompleteQuest(QUEST_THE_BATTLE_OF_ARDENWEALD);
        }
    }

};

void AddSC_Scenario_battle_of_ardenweald()
{
    new Scenario_battle_of_ardenweald();
    new scene_battle_of_ardenweald();
    RegisterCreatureAI(npc_anima_wyrm_178291);
    RegisterCreatureAI(npc_lady_moonberry_177962);
    RegisterCreatureAI(npc_animacone_178524);
    RegisterCreatureAI(npc_boa_stage3_increasebar_i);
    RegisterCreatureAI(npc_boa_stage3_increasebar_ii);
    RegisterCreatureAI(npc_winter_queen_177280);
    RegisterCreatureAI(npc_sylvanas_windrunner_177114);
    RegisterCreatureAI(npc_plague_177261);
    RegisterCreatureAI(npc_trained_flayedwing_177583);
    RegisterCreatureAI(npc_Kleia_177123);
    RegisterCreatureAI(npc_mistblade_177205);
    RegisterCreatureAI(npc_draven_177250);
    new npc_soulburner_fraxin_177159();
    RegisterCreatureAI(npc_forest_guardian_177630);
    RegisterCreatureAI(npc_wildseeds);
    RegisterCreatureAI(npc_rally_point_178440);
    RegisterCreatureAI(npc_mawsworn_malmbreaker_177600);
    RegisterCreatureAI(npc_mawsworn_invoker_177601);
    RegisterCreatureAI(npc_mawsworn_destroyer_177165);
}
