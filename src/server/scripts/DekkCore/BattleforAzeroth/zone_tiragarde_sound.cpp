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

#include "Conversation.h"
#include "GameObject.h"
#include "GameObjectAI.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "ScriptedCreature.h"
#include "ScriptedEscortAI.h"
#include "ScriptedFollowerAI.h"
#include "ScriptedGossip.h"
#include "ScriptMgr.h"
#include "TemporarySummon.h"
#include "TaskScheduler.h"
#include "Vehicle.h"
#include "PhasingHandler.h"
#include "CombatAI.h"
#include "Spell.h"

enum eTiragardeQuests
{
    QUEST_DAUGHTER_OF_THE_SEA = 51341,

    QUEST_OUT_LIKE_FLYNN = 47098,
    KILL_CREDIT_GET_DRESSED = 138554,
    KILL_CREDIT_PULL_LEVER = 138553,
    KILL_CREDIT_CELL_BLOCK_DOOR = 137923,

    QUEST_GET_YOUR_BEARINGS = 47099,
    QUEST_THE_OLD_KNIGHT = 46729,
    QUEST_NATION_DIVIDED = 47189,
    QUEST_NATION_UNITED = 52151,
};

enum Intro
{
    SPELL_PROUDMOORE_KEEP_ESCORT = 269772,
    SPELL_LADY_KATHERINE_MOVIE = 241525,

    SPELL_TELEPORT_TO_TOL_DAGOR = 241526,
    SPELL_PRISONER = 272512,
    SPELL_TOL_DAGOR_WAKE_UP = 270081,

    SPELL_PUNCH_FLYNN = 264918,
    SPELL_FLYNN_KNOCKOUT_JAILER = 246555,
    SPELL_SUMMON_FLYNN_ESCORT = 246931,

    SPELL_SCENE_FLYNN_JAILBREAK = 246821,
    SPELL_SCENE_GETAWAY_BOAT_TRIGGER = 281331,
    SPELL_SCENE_NATION_DIVIDED = 269191,
    SPELL_SCENE_EXPOSING_ASHVANE = 265916,

    SPELL_GETAWAY_CONVERSATION_1 = 247230,
    SPELL_GETAWAY_CONVERSATION_2 = 247275,

    SPELL_MAINTAIN_TAELIA_SUMMON = 247532,
    SPELL_SCENE_OLD_KNIGHT = 271234,

    NPC_FLYNN_BEGIN = 121239,
    NPC_FLYNN_ESCORT = 124311,
    NPC_FLYNN_ESCAPE = 124363,
    NPC_ASHVANE_JAILER_EVENT = 124022,
    NPC_TAELIA = 124356,
    NPC_GETAWAY_BOAT_BOARDED = 124030,
    NPC_TAELIA_GET_YOUR_BEARINGS = 124630,
    NPC_CYRUS_CRESTFALL = 122370,

    GOB_PRISON_BARS = 281878,
    GOB_PRISON_GATE = 301088,
    GOB_CELL_BLOCK_GATE = 281902,

    MOVIE_LADY_KATHERINE = 859,
};

// 120922 - Lady Jaina Proudmoore
struct npc_jaina_boralus_intro : public ScriptedAI
{
    npc_jaina_boralus_intro(Creature* creature) : ScriptedAI(creature) { }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_DAUGHTER_OF_THE_SEA)
            player->CastSpell(player, SPELL_PROUDMOORE_KEEP_ESCORT, true);
        player->RemoveAurasDueToSpell(SPELL_PROUDMOORE_KEEP_ESCORT);
        player->ForceCompleteQuest(QUEST_DAUGHTER_OF_THE_SEA);
        player->TeleportTo(1643, 140.932f, -2716.540f, 30.539f, 1.106f);
    }
};

// 1954
class scene_jaina_to_proudmoore_keep : public SceneScript
{
public:
    scene_jaina_to_proudmoore_keep() : SceneScript("scene_jaina_to_proudmoore_keep") { }

    void OnSceneEnd(Player* player, uint32 /*sceneInstanceID*/, SceneTemplate const* /*sceneTemplate*/) override
    {
        player->AddMovieDelayedAction(MOVIE_LADY_KATHERINE, [player]()
            {
                player->CastSpell(player, SPELL_TELEPORT_TO_TOL_DAGOR, true);
                player->CastSpell(player, SPELL_PRISONER, true);
                player->CastSpell(player, SPELL_TOL_DAGOR_WAKE_UP, true);
            });

        player->CastSpell(player, SPELL_LADY_KATHERINE_MOVIE, true);
    }
};

// Prisoner - 272512
class aura_tol_dagor_intro_prisoner : public AuraScript
{
    PrepareAuraScript(aura_tol_dagor_intro_prisoner);

    void HandleApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->RemoveGameObjectByEntry(GOB_PRISON_BARS);
        GetTarget()->RemoveGameObjectByEntry(GOB_PRISON_GATE);

        GetTarget()->SummonGameObject(GOB_PRISON_BARS, 145.772995f, -2707.709961f, 28.818899f, 0.942667f, QuaternionData(0.f, 0.f, 0.454075f, 0.890964f), 0s);
        GetTarget()->SummonGameObject(GOB_PRISON_GATE, 146.242996f, -2699.399902f, 28.877800f, 5.663670f, QuaternionData(0.f, 0.f, -0.304828f, 0.952407f), 0s);
    }

    void HandleRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->RemoveGameObjectByEntry(GOB_PRISON_BARS);
        GetTarget()->RemoveGameObjectByEntry(GOB_PRISON_GATE);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(aura_tol_dagor_intro_prisoner::HandleApply, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
        OnEffectRemove += AuraEffectRemoveFn(aura_tol_dagor_intro_prisoner::HandleRemove, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_REAL);
    }
};

// 47098
struct quest_out_like_flynn : public QuestScript
{
    quest_out_like_flynn() : QuestScript("quest_out_like_flynn") { }

    // Called when a quest objective data change
    void OnQuestObjectiveChange(Player* player, Quest const* /*quest*/, QuestObjective const& objective, int32 /*oldAmount*/, int32 /*newAmount*/) override
    {
        if (objective.ObjectID == KILL_CREDIT_GET_DRESSED)
            player->RemoveAurasDueToSpell(SPELL_PRISONER);
        else if (objective.ObjectID == KILL_CREDIT_PULL_LEVER)
        {
            player->RemoveGameObjectByEntry(GOB_PRISON_BARS);
            player->RemoveGameObjectByEntry(GOB_PRISON_GATE);
        }
    }
};

// 121239 - Flynn Fairwind
struct npc_flynn_fairwind : public ScriptedAI
{
    npc_flynn_fairwind(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        if (me->GetAreaId() != 8978)
            me->SetNpcFlag(UNIT_NPC_FLAG_GOSSIP);
        me->SetReactState(REACT_PASSIVE);
        me->SetUnitFlag(UNIT_FLAG_NON_ATTACKABLE);
    };

    enum FlynnTalks
    {
        TALK_HERES_PLAN = 6,
        TALK_HIT_ME = 7,
        TALK_COME_ON_HIT_ME = 8,
        TALK_DONT_SHY_HIT_ME = 9,
        TALK_YOU_BRUTE = 10,
        TALK_GUARD = 11,
        TALK_HIT_THAT_LEVER = 12,
    };

    enum GuardTalks
    {
        TALK_WHATS_GOING_ON = 0,
        TALK_STOP_RIGHT_HERE = 1,
        TALK_WHAT = 2,
        TALK_NO = 3,
    };

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_OUT_LIKE_FLYNN)
        {
            if (Creature* flynn = player->SummonCreature(me->GetEntry(), me->GetPosition(), TEMPSUMMON_CORPSE_DESPAWN, 0s, 0))
            {
                flynn->AI()->SetGUID(player->GetGUID());
                me->DestroyForPlayer(player);
            }
        }
    }

    void SetGUID(ObjectGuid const& guid, int32 /*action*/) override
    {
        m_playerGUID = guid;
        me->RemoveNpcFlag(UNIT_NPC_FLAG_QUESTGIVER);
        me->SetAIAnimKitId(0);

        if (Creature* ashvaneJailer = me->SummonCreature(NPC_ASHVANE_JAILER_EVENT, 144.839996f, -2702.790039f, 28.961100f, 0.799371f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 2s))
            m_ashvaneJailerGUID = ashvaneJailer->GetGUID();

        me->GetScheduler().Schedule(1s, [this](TaskContext /*context*/)
            {
                me->GetMotionMaster()->MoveJump(142.033f, -2715.19f, 29.1884f, 0.0f, 19.2911f, 19.2911f);
            })
            .Schedule(2s, [this](TaskContext /*context*/)
                {
                    me->GetMotionMaster()->MovePoint(2, 145.070679f, -2710.949463f, 29.187674f);
                })
                .Schedule(3s, [this](TaskContext /*context*/)
                    {
                        me->SetFacingToObject(GetPlayer());
                        Talk(TALK_HERES_PLAN, GetPlayer());
                    })
                    .Schedule(5s, [this](TaskContext /*context*/)
                        {
                            Talk(TALK_HIT_ME, GetPlayer());
                            me->SetNpcFlag(UNIT_NPC_FLAG_SPELLCLICK);
                        });
    }

    void SpellHit(WorldObject* caster, SpellInfo const* spell) override

    {
        if (spell->Id != SPELL_PUNCH_FLYNN)
            return;

        me->RemoveNpcFlag(UNIT_NPC_FLAG_SPELLCLICK);
        me->HandleEmoteCommand(EMOTE_ONESHOT_BEG);
        Talk(TALK_YOU_BRUTE);

        caster->ToPlayer()->KilledMonsterCredit(me->GetEntry());

        if (Creature* ashvaneJailer = GetAshvaneJailer())
        {
            me->GetScheduler().Schedule(2s, [this](TaskContext /*context*/)
                {
                    Talk(TALK_GUARD);
                    me->SetStandState(UNIT_STAND_STATE_DEAD);
                });

            ashvaneJailer->GetScheduler().Schedule(3s, [](TaskContext context)
                {
                    GetContextUnit()->GetMotionMaster()->MovePoint(1, 147.070480f, -2705.972412f, 29.189432f);
                })
                .Schedule(4s, [](TaskContext context)
                    {
                        GetContextUnit()->SetFacingTo(4.111071f);
                    })
                    .Schedule(5s, [](TaskContext context)
                        {
                            GetContextCreature()->AI()->Talk(TALK_WHATS_GOING_ON);
                            GetContextUnit()->HandleEmoteCommand(EMOTE_ONESHOT_EXCLAMATION);
                        })
                        .Schedule(6s, [](TaskContext context)
                            {
                                GetContextUnit()->HandleEmoteCommand(EMOTE_ONESHOT_USE_STANDING);
                            })
                            .Schedule(7s, [](TaskContext context)
                                {
                                    if (GameObject* gob = GetContextUnit()->GetGameObjectByEntry(GOB_PRISON_BARS))
                                        gob->UseDoorOrButton();
                                })
                                .Schedule(8s, [](TaskContext context)
                                    {
                                        GetContextCreature()->AI()->Talk(TALK_STOP_RIGHT_HERE);
                                        GetContextUnit()->GetMotionMaster()->MovePoint(2, 143.408783f, -2710.396240f, 29.187752f);
                                        // Set hostile
                                    });

                                me->GetScheduler().Schedule(9s, [this, ashvaneJailer](TaskContext /*context*/)
                                    {
                                        me->SetStandState(UNIT_STAND_STATE_STAND);
                                        ashvaneJailer->AI()->Talk(TALK_WHAT);
                                    })
                                    .Schedule(10s, [this, ashvaneJailer](TaskContext /*context*/)
                                        {
                                            me->CastSpell(nullptr, SPELL_FLYNN_KNOCKOUT_JAILER, false);
                                            ashvaneJailer->AI()->Talk(TALK_NO);
                                        })
                                        .Schedule(11s, [this](TaskContext /*context*/)
                                            {
                                                me->GetMotionMaster()->MovePoint(3, 165.596573f, -2707.874756f, 28.877989f);
                                            })
                                            .Schedule(14s, [this](TaskContext /*context*/)
                                                {
                                                    me->SetFacingTo(2.540090f);
                                                    Talk(TALK_HIT_THAT_LEVER);
                                                    GetPlayer()->RemoveAura(SPELL_PRISONER);
                                                });
        }
    }

private:
    Player* GetPlayer() { return ObjectAccessor::GetPlayer(*me, m_playerGUID); }
    Creature* GetAshvaneJailer() { return ObjectAccessor::GetCreature(*me, m_ashvaneJailerGUID); }

    ObjectGuid m_playerGUID;
    ObjectGuid m_ashvaneJailerGUID;
};

// 271938 - Cell Block Lever
struct go_toldagor_cell_block_lever : public GameObjectAI
{
    go_toldagor_cell_block_lever(GameObject* go) : GameObjectAI(go) { }

    bool OnGossipHello(Player* player) override
    {
        player->CastSpell(player, SPELL_SCENE_FLYNN_JAILBREAK, true);
        // player->UnsummonCreatureByEntry(NPC_FLYNN_BEGIN);
        return false;
    }
};

// 1735
class scene_flynn_jailbreak : public SceneScript
{
public:
    scene_flynn_jailbreak() : SceneScript("scene_flynn_jailbreak") { }

    void OnSceneEnd(Player* player, uint32 /*sceneInstanceID*/, SceneTemplate const* /*sceneTemplate*/) override
    {
        player->CastSpell(player, SPELL_SUMMON_FLYNN_ESCORT, true);
    }
};

// 124311 - Flynn Fairwind (Quest follower)
struct npc_flynn_fairwind_follower : public FollowerAI
{
    npc_flynn_fairwind_follower(Creature* creature) : FollowerAI(creature) { }

    void Reset() override
    {
        me->SetReactState(REACT_PASSIVE);
        me->SetLevel(120);
        me->SetUnitFlag(UNIT_FLAG_NON_ATTACKABLE);
        me->GetScheduler().Schedule(1s, [this](TaskContext context)
            {
                if (me->FindNearestGameObject(GOB_CELL_BLOCK_GATE, 10.f))
                {
                    SetFollowPaused(true);
                    GetContextUnit()->GetMotionMaster()->MovePoint(1, 184.875366f, -2684.565918f, 29.504234f);
                }
                else
                    context.Repeat();
            });

        me->GetScheduler().Schedule(2s, [this](TaskContext context)
            {
                if (me->FindNearestCreature(NPC_TAELIA, 40.f))
                {
                    if (TempSummon* tempMe = me->ToTempSummon())
                        if (WorldObject* summoner = tempMe->GetSummoner())
                            if (Player* playerSummoner = summoner->ToPlayer())
                                playerSummoner->PlayConversation(8334);
                }
                else
                    context.Repeat();
            });
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (type != POINT_MOTION_TYPE || pointId != 1)
            return;

        me->SetFacingTo(5.698150f);

        me->GetScheduler().Schedule(1s, [this](TaskContext /*context*/)
            {
                me->HandleEmoteCommand(EMOTE_ONESHOT_USE_STANDING);
            })
            .Schedule(2s, [this](TaskContext /*context*/)
                {
                    if (GameObject* door = me->FindNearestGameObject(GOB_CELL_BLOCK_GATE, 10.f))
                    {
                        door->UseDoorOrButton();
                        door->DestroyForPlayer(GetLeaderForFollower());
                    }
                })
                .Schedule(4s, [this](TaskContext /*context*/)
                    {
                        SetFollowPaused(false);

                        if (GameObject* door = me->FindNearestGameObject(GOB_CELL_BLOCK_GATE, 10.f))
                            door->UseDoorOrButton();

                        if (Player* player = GetLeaderForFollower())
                            player->KilledMonsterCredit(KILL_CREDIT_CELL_BLOCK_DOOR);
                    });
    }
};

// 134922 - Kill Credit Enter Sewers
struct npc_tol_dagor_enter_sewer_credit : public ScriptedAI
{
    npc_tol_dagor_enter_sewer_credit(Creature* creature) : ScriptedAI(creature) { }

    void MoveInLineOfSight(Unit* unit) override
    {
        if (Player* player = unit->ToPlayer())
            if (player->GetDistance(me) < 5.0f)
                player->KilledMonsterCredit(me->GetEntry());
    }
};

// 8330
// 8333
// 8334
struct conversation_tol_dagor_inmate : public ConversationScript
{
    conversation_tol_dagor_inmate() : ConversationScript("conversation_tol_dagor_inmate") { }

    //void OnConversationCreate(Conversation* conversation, Unit* creator) override
    //{
    //    if (conversation->GetEntry() == 8334)
    //        if (Unit* taelia = creator->FindNearestCreature(NPC_TAELIA, 50.f))
    //            conversation->AddActor(taelia->GetGUID(), 0);

    //  /*  if (Unit* flynn = creator->GetSummonedCreatureByEntry(NPC_FLYNN_ESCORT))
    //        conversation->AddActor(flynn->GetGUID(), conversation->GetEntry() == 8334 ? 1 : 0);*/
    //}
};

// 124357 - Getaway Boat
struct npc_tol_dagor_getaway_boat : public ScriptedAI
{
    npc_tol_dagor_getaway_boat(Creature* creature) : ScriptedAI(creature) { }

    Position boatPath[6] = {
    { 240.6500f, -2812.950f, -0.052747f },
    { 245.9427f, -2807.717f,  0.052747f },
    { 272.6615f, -2792.370f, -0.052747f },
    { 353.6458f, -2743.795f,  0.052747f },
    { 366.6493f, -2540.583f, -0.052747f },
    { 396.1441f, -2403.012f, -0.052747f },
    };

    void IsSummonedBy(WorldObject* unit) override
    {
        if (Player* player = unit->ToPlayer())
        {
            me->SetReactState(REACT_PASSIVE);
            player->EnterVehicle(me, 1);
            player->KilledMonsterCredit(NPC_GETAWAY_BOAT_BOARDED);

            player->GetScheduler().Schedule(1s, [this, player](TaskContext /*context*/)
                {
                    player->PlayConversation(5336);
                    me->GetMotionMaster()->MoveSmoothPath(1, boatPath, 6, false, true);
                })
                .Schedule(36s, [player](TaskContext /*context*/)
                    {
                        // This specific scene is spawned at 0 0 0
                        Position scenePos = Position();
                        player->GetSceneMgr().PlayScene(1746, &scenePos);
                    });
        }
    }
};

// 5336
// 5340
struct conversation_tol_dagor_escape : public ConversationScript
{
    conversation_tol_dagor_escape() : ConversationScript("conversation_tol_dagor_escape") { }

    //void OnConversationCreate(Conversation* conversation, Unit* creator) override
    //{
    //    if (Vehicle* boat = creator->GetVehicle())
    //    {
    //        if (Unit* taelia = boat->GetPassenger(0))
    //            conversation->AddActor(taelia->GetGUID(), 0);

    //        // Flynn only speak during the first conversation
    //        if (conversation->GetEntry() == 5336)
    //            if (Unit* flynn = boat->GetPassenger(2))
    //                conversation->AddActor(flynn->GetGUID(), 1);
    //    }
    //}
};

// 1746
class scene_tol_dagor_getaway_boat : public SceneScript
{
public:
    scene_tol_dagor_getaway_boat() : SceneScript("scene_tol_dagor_getaway_boat") { }

    Position boatPath[4] =
    {
        { 880.6389f, -585.5486f, -0.02336364f },
    { 998.2083f, -575.0087f, -0.03875812f },
    { 1025.792f, -619.1180f, -0.03875812f },
    { 1040.462f, -631.7864f, -0.03875812f },
    };

    void OnSceneTriggerEvent(Player* player, uint32 /*sceneInstanceID*/, SceneTemplate const* /*sceneTemplate*/, std::string const& triggerName) override
    {
        if (triggerName == "TeleportToMarket")
        {
            if (Creature* vehicleBase = player->GetVehicleCreatureBase())
            {
                vehicleBase->NearTeleportTo(867.132f, -602.811f, -0.117634f, 1.536673f);
                vehicleBase->GetScheduler().Schedule(2s, [this, vehicleBase, player](TaskContext /*context*/)
                    {
                        vehicleBase->GetMotionMaster()->MoveSmoothPath(2, boatPath, 4, false, true);

                        player->CastSpell(player, SPELL_GETAWAY_CONVERSATION_2, true);
                    })
                    .Schedule(25s, [player, vehicleBase](TaskContext /*context*/)
                        {
                            DespawnAndTeleportPlayer(player, vehicleBase);
                        });
            }
        }
    }

    void OnSceneCancel(Player* player, uint32 /*sceneInstanceID*/, SceneTemplate const* /*sceneTemplate*/) override
    {
        if (Creature* vehicleBase = player->GetVehicleCreatureBase())
        {
            vehicleBase->GetScheduler().CancelAll();
            DespawnAndTeleportPlayer(player, vehicleBase);
        }
    }

private:
    static void DespawnAndTeleportPlayer(Player* player, Creature* vehicleBase)
    {
        player->ExitVehicle();
        player->NearTeleportTo(1053.48f, -627.64f, 0.54f, 2.523746f);
        vehicleBase->DespawnOrUnsummon();
    }
};

// 5360 Intro
// 5362 ferry
// 5365 bank
// 5366 fly
// 5375 tavern
// 9556 end
// 7605 harbormaster office
struct conversation_boralus_get_your_bearings : public ConversationScript
{
    conversation_boralus_get_your_bearings() : ConversationScript("conversation_boralus_get_your_bearings") { }

    /*  void OnConversationCreate(Conversation* conversation, Unit* creator) override
      {
          if (Unit* taelia = creator->GetSummonedCreatureByEntry(NPC_TAELIA_GET_YOUR_BEARINGS))
              conversation->AddActor(taelia->GetGUID(), 0);
      }*/
};

// 124630 - Taelia (Get your bearings)
struct npc_taelia_get_your_bearings : public FollowerAI
{
    npc_taelia_get_your_bearings(Creature* creature) : FollowerAI(creature) { }

    struct ConvByKillStruct
    {
        ConvByKillStruct(uint8 objectiveIndex, uint32 killCreditID, uint32 conversationID) :
            ObjectiveIndex(objectiveIndex), KillCreditID(killCreditID), ConversationID(conversationID) { }

        uint8 ObjectiveIndex = 0;
        uint32 KillCreditID = 0;
        uint32 ConversationID = 0;
    };

    std::map<uint32, ConvByKillStruct> convByKillCredit =
    {
        { 124720, ConvByKillStruct(0, 124586, 5365) },
        { 124725, ConvByKillStruct(1, 124587, 5366) },
        { 135064, ConvByKillStruct(2, 124588, 5362) },
        { 135153, ConvByKillStruct(3, 124768, 5375) },
    };

    void IsSummonedBy(WorldObject* unit) override
    {
        me->SetNpcFlag(UNIT_NPC_FLAG_QUESTGIVER);

        if (Player* player = unit->ToPlayer())
        {
            player->PlayConversation(5360);
            ObjectGuid playerGuid = player->GetGUID();

            me->GetScheduler().Schedule(1s, [this, playerGuid](TaskContext context)
                {
                    Player* player = ObjectAccessor::GetPlayer(*me, playerGuid);
                    if (!player)
                        return;

                    if (player->GetQuestStatus(QUEST_GET_YOUR_BEARINGS) == QUEST_STATUS_INCOMPLETE)
                    {
                        bool justCompletedObjective = false;
                        for (auto itr : convByKillCredit)
                            if (player->FindNearestCreature(itr.first, 10.f))
                                //    if (!player->GetQuestObjectiveData(QUEST_GET_YOUR_BEARINGS, itr.second.ObjectiveIndex))
                            {
                                player->KilledMonsterCredit(itr.second.KillCreditID);
                                player->PlayConversation(itr.second.ConversationID);
                                justCompletedObjective = true;
                            }

                        if (justCompletedObjective && player->GetQuestStatus(QUEST_GET_YOUR_BEARINGS) == QUEST_STATUS_COMPLETE)
                        {
                            player->PlayConversation(9556);
                            me->SetNpcFlag(UNIT_NPC_FLAG_QUESTGIVER);
                        }
                    }

                    if (player->GetQuestStatus(QUEST_THE_OLD_KNIGHT) == QUEST_STATUS_INCOMPLETE)
                        //   if (!player->GetQuestObjectiveData(QUEST_THE_OLD_KNIGHT, 0))
                        if (player->FindNearestCreature(NPC_CYRUS_CRESTFALL, 20.f))
                        {
                            player->CastSpell(player, SPELL_SCENE_OLD_KNIGHT, true);
                            player->KilledMonsterCredit(NPC_CYRUS_CRESTFALL);
                            player->RemoveAurasDueToSpell(SPELL_MAINTAIN_TAELIA_SUMMON);
                            return;
                        }

                    context.Repeat();
                });
        }
    }
};

// 1960
class scene_boralus_old_knight : public SceneScript
{
public:
    scene_boralus_old_knight() : SceneScript("scene_boralus_old_knight") { }

    void OnSceneEnd(Player* player, uint32 /*sceneInstanceID*/, SceneTemplate const* /*sceneTemplate*/) override
    {
        player->PlayConversation(8062);
    }
};

// 122370
class npc_cyrus_crestfall : public ScriptedAI
{
public:
    npc_cyrus_crestfall(Creature* creature) : ScriptedAI(creature) { }

    bool OnGossipSelect(Player* plr, uint32 /*sender*/, uint32 /*action*/) override
    {
        if (plr->HasQuest(QUEST_THE_OLD_KNIGHT))
        {
            plr->KilledMonsterCredit(122370);
            plr->KilledMonsterCredit(137009);
            plr->PlayConversation(7653);
            CloseGossipMenuFor(plr);
        }

        /* quest - Send the Fleet 56043 */
        if (plr->HasQuest(56043))
        {
            CloseGossipMenuFor(plr);
            plr->ForceCompleteQuest(56043);
            Talk(0);

            plr->GetScheduler().Schedule(Seconds(3), [plr](TaskContext /*context*/)
                {
                    // tele to nazjatar
                    WorldLocation location(1718, 166.361f, -476.148f, -29.146f, 6.267f);
                    plr->TeleportTo(location);

                    // boat ride movie
                    plr->SendMovieStart(883);
                });
        }

        return true;
    }
};

// 7653
struct conversation_cyrus_story : public ConversationScript
{
    conversation_cyrus_story() : ConversationScript("conversation_cyrus_story") { }

    void OnConversationRemove(Conversation* /*conversation*/, Unit* creator) override
    {
        if (creator)
            if (Player* player = creator->ToPlayer())
                player->KilledMonsterCredit(137877);
    }
};

// 137066
class npc_boralus_portal_maga : public ScriptedAI
{
public:
    npc_boralus_portal_maga(Creature* creature) : ScriptedAI(creature) { }

    bool OnGossipSelect(Player* player, uint32 /*menuId*/, uint32 /*gossipListId*/) override
    {
        KillCreditMe(player);

        return true;
    }
};

// 121235
class npc_taelia_harbormaster : public ScriptedAI
{
public:
    npc_taelia_harbormaster(Creature* creature) : ScriptedAI(creature) { }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_NATION_DIVIDED)
        {
            player->CastSpell(player, SPELL_SCENE_NATION_DIVIDED, true);
            player->ForceCompleteQuest(QUEST_NATION_DIVIDED);
        }
        else if (quest->GetQuestId() == 47099)
            player->CastSpell(player, 247528, true);
    }

    bool OnGossipSelect(Player* player, uint32 /*menuId*/, uint32 /*gossipListId*/) override
    {
        player->CastSpell(player, SPELL_SCENE_NATION_DIVIDED, true);

        return true;
    }
};

// 139522
class npc_boralus_adventure_map : public ScriptedAI
{
public:
    npc_boralus_adventure_map(Creature* creature) : ScriptedAI(creature) { }

    bool OnGossipHello(Player* player) override
    {
        KillCreditMe(player);

        return true;
    }
};

// 126158
class npc_flynn_allured : public ScriptedAI
{
public:
    npc_flynn_allured(Creature* creature) : ScriptedAI(creature) { }

    enum
    {
        QUEST_ALLURED_ID = 48419,
        QUEST_LOVESICK_ID = 48505,
        SPELL_SUMMON_FLYNN_ESCORT_ID = 251037
    };

    void MoveInLineOfSight(Unit* unit) override
    {
        if (Player* player = unit->ToPlayer())
            if (player->GetQuestStatus(QUEST_ALLURED_ID) == QUEST_STATUS_INCOMPLETE)
                KillCreditMe(player);
    }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_LOVESICK_ID)
        {
            player->KilledMonsterCredit(126490);
            me->DestroyForPlayer(player);
            player->CastSpell(player, SPELL_SUMMON_FLYNN_ESCORT_ID, true);
        }
    }
};

// 126157
class npc_lugeia : public ScriptedAI
{
public:
    npc_lugeia(Creature* creature) : ScriptedAI(creature) { }

    enum
    {
        NPC_FLYNN_ENTRY = 126158,
        SPELL_LOVESTRUCK = 245526,
        SPELL_BROKEN_HEART = 250911
    };

    void JustDied(Unit* /*killer*/) override
    {
        if (Creature* flynn = me->FindNearestCreature(NPC_FLYNN_ENTRY, 20.f))
        {
            flynn->RemoveAura(SPELL_LOVESTRUCK);
            flynn->CastSpell(flynn, SPELL_BROKEN_HEART, true);
            flynn->GetMotionMaster()->MovePoint(0, -1386.29f, -1663.99f, 0.111337f);
            flynn->ForcedDespawn(4500);
        }
    }
};

/// TODO Make Flynn wait for player
/// TODO Cast the good spells at the right time
// 126490
class npc_flynn_lovesick_escort : public EscortAI
{
public:
    npc_flynn_lovesick_escort(Creature* creature) : EscortAI(creature) { }

    enum
    {
        SPELL_LOVESICK = 250911,
        SPELL_FLYNN_FLASK = 251027,
        SPELL_DRUNK = 251260,
        SPELL_THROW_FLASK = 251065
    };

    void IsSummonedBy(WorldObject* summoner) override
    {
        LoadPath(me->GetEntry());
        Start(false, summoner->GetGUID());
        SetEscortPaused(true);
        me->GetScheduler().Schedule(5s, [this](TaskContext /*context*/)
            {
                me->CastSpell(me, SPELL_LOVESICK, true);
            }).Schedule(8s, [](TaskContext /*context*/)
                {
                    //TalkToEscortPlayer(0);
                }).Schedule(10s, [this](TaskContext /*context*/)
                    {
                        me->CastSpell(me, SPELL_FLYNN_FLASK, true);
                    }).Schedule(17s, [this](TaskContext /*context*/)
                        {
                            SetEscortPaused(false);
                        });
    }

    void WaypointReached(uint32 /*waypointId*/, uint32 pointId) override
    {
        switch (pointId)
        {
        case 0:
            //  TalkToEscortPlayer(1);
            break;
        case 1:
            //TalkToEscortPlayer(2);
            SetEscortPaused(true);
            me->GetScheduler().Schedule(7s, [this](TaskContext /*context*/)
                {
                    //TalkToEscortPlayer(3);
                    SetEscortPaused(false);
                });
            break;
        case 6:
            //  TalkToEscortPlayer(4);
            break;
        case 12:
            //    TalkToEscortPlayer(5);
            SetEscortPaused(true);
            me->GetScheduler().Schedule(7s, [this](TaskContext /*context*/)
                {
                    SetEscortPaused(false);
                    //  TalkToEscortPlayer(6);
                });
            me->CastSpell(me, SPELL_DRUNK, true);
            break;
        case 16:
            //  TalkToEscortPlayer(7);
            break;
        case 31:
            // TalkToEscortPlayer(9);
            break;
        case 40:
            //  TalkToEscortPlayer(10);
            break;
        case 45:
            //  TalkToEscortPlayer(11);
            break;
        case 46:
            //  TalkToEscortPlayer(12);
            KillCreditMe(GetPlayerForEscort());
            break;
        default:
            break;
        }
    }

    void LastWaypointReached() //override
    {
        if (Player* player = GetPlayerForEscort())
        {
            KillCreditMe(player);
        }
    }
};

//128349 Hilde Firebreaker
class npc_hilde_firebreaker_queststarter : public ScriptedAI
{
public:
    enum
    {
        QUEST_BACKUP_WILL_I_PACK = 49260,
    };

    npc_hilde_firebreaker_queststarter(Creature* creature) : ScriptedAI(creature) { }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() != QUEST_BACKUP_WILL_I_PACK)
            return;

        players.push_back(player);

        if (_ongoing)
            return;

        players.clear();
        players.push_back(player);

        if (Creature* hilde2 = me->SummonCreature(129841, me->GetPosition(), TEMPSUMMON_CORPSE_DESPAWN))
            hilde2->AI()->SetGUID(player ? player->GetGUID() : ObjectGuid::Empty);

        me->SetUnitFlag(UNIT_FLAG_IMMUNE_TO_NPC);
        me->AddAura(65050, me);

        _ongoing = true;
    }

    void OnQuestReward(Player* player, Quest const* quest, LootItemType /*type*/, uint32 /*opt*/) override
    {
        if (quest->GetQuestId() == QUEST_BACKUP_WILL_I_PACK)
        {
            me->SetWalk(true);
            Talk(0);
            me->GetMotionMaster()->MovePoint(1001, 1103.37f, 257.04f, 16.6979f, true);
        }
    }

    void SummonedCreatureDespawn(Creature* /*creature*/) override
    {
        _ongoing = false;
    }

    void SummonedCreatureDies(Creature* /*creature*/, Unit* /*unit*/) override
    {
        // Fail the quest
        for (Player* player : players)
            if (player && player->IsInWorld())
                player->FailQuest(QUEST_BACKUP_WILL_I_PACK);

        _ongoing = false;
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        if (id == 1001) me->DespawnOrUnsummon(1s, 120s);
    }

private:
    bool _ongoing = false;
    std::vector<Player*> players;
};

//129841 Hilde Firebreaker
class npc_hilde_firebreaker_protect : public EscortAI
{
public:
    enum
    {
        QUEST_BACKUP_WILL_I_PACK = 49260,
        NPC_LIVING_ARTEFACT = 128405,
        NPC_ANGERED_REVENANT = 128591,
        NPC_FALLEN_KEEPER = 128608,
        NPC_DEFEND_FIREBREAKER_KILLCREDIT = 128709
    };

    npc_hilde_firebreaker_protect(Creature* creature) : EscortAI(creature)
    {
        pos[0] = Position(1108.739990f, 261.151001f, 17.821600f, 1.603710f); // 128405
        pos[1] = Position(1115.000000f, 261.557007f, 18.138300f, 1.946000f); // 128591
        pos[2] = Position(1123.180054f, 269.458008f, 17.009701f, 1.762220f); // 128405
        pos[3] = Position(1123.770020f, 275.671997f, 17.066000f, 3.344580f); // 128591
        pos[4] = Position(1123.689941f, 278.740997f, 18.338100f, 3.654940f); // 128405
        pos[5] = Position(1119.719971f, 265.915009f, 17.663000f, 2.204700f); // 128608
        pos[6] = Position(1109.19f, 280.738f, 18.3761f, 2.2218f);            // hilde_first_pos
        pos[7] = Position(1114.68f, 274.511f, 17.9051f, 4.3311f);            // hilde_second_pos
    }

    void Reset() override
    {
        m_playerGUID = ObjectGuid::Empty;
        o_hildeGUID = ObjectGuid::Empty;
    }


    void SetGUID(ObjectGuid const& guid, int32 id) override
    {
        m_playerGUID = guid;
    }

    void IsSummonedBy(WorldObject* summoner) override
    {
        me->SetWalk(true);
        me->SetUnitFlag(UNIT_FLAG_IMMUNE_TO_NPC);
        o_hildeGUID = summoner->GetGUID();
        me->GetMotionMaster()->MovePoint(1001, pos[6], true, 2.2218f);
    }

    void SummonedCreatureDies(Creature* /*creature*/, Unit* /*unit*/) override
    {
        _numberOfSummonsAlive--;
        if (_numberOfSummonsAlive == 0)
        {
            // Next wave or despawn if finished
            _numberOfWaveCleaned++;

            switch (_numberOfWaveCleaned)
            {
            case 1:
                _numberOfSummonsAlive = 3;
                if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                {
                    if (TempSummon* ts = me->SummonCreature(NPC_LIVING_ARTEFACT, pos[2], TEMPSUMMON_CORPSE_DESPAWN)) ts->AI()->AttackStart(player);
                    if (TempSummon* ts = me->SummonCreature(NPC_ANGERED_REVENANT, pos[3], TEMPSUMMON_CORPSE_DESPAWN)) ts->AI()->AttackStart(player);
                    if (TempSummon* ts = me->SummonCreature(NPC_LIVING_ARTEFACT, pos[4], TEMPSUMMON_CORPSE_DESPAWN)) ts->AI()->AttackStart(player);
                }
                Talk(2);
                me->AddDelayedEvent(15000, [this]() ->void { Talk(3); });
                break;
            case 2:
                _numberOfSummonsAlive = 1;
                if (TempSummon* ts = me->SummonCreature(NPC_FALLEN_KEEPER, pos[5], TEMPSUMMON_CORPSE_DESPAWN)) ts->AI()->AttackStart(me);
                Talk(4);
                break;
            case 3:
                Talk(5);
                me->HandleEmoteCommand(EMOTE_STATE_NONE);
                me->GetMotionMaster()->MovePoint(1002, pos[7], true, 4.3311f);
                break;
            default:
                break;
            }
        }
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        if (id == 1001)
        {
            _numberOfSummonsAlive = 2;
            me->HandleEmoteCommand(EMOTE_STATE_WORK);
            if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
            {
                if (TempSummon* ts = me->SummonCreature(NPC_LIVING_ARTEFACT, pos[0], TEMPSUMMON_DEAD_DESPAWN)) ts->AI()->AttackStart(player);
                if (TempSummon* ts = me->SummonCreature(NPC_ANGERED_REVENANT, pos[1], TEMPSUMMON_DEAD_DESPAWN)) ts->AI()->AttackStart(player);
            }
            Talk(0);
            me->AddDelayedEvent(15000, [this]() ->void { Talk(1); });
        }
        if (id == 1002)
        {
            if (Creature* hilde = ObjectAccessor::GetCreature(*me, o_hildeGUID))
                hilde->RemoveAura(65050);
            if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                player->KilledMonsterCredit(NPC_DEFEND_FIREBREAKER_KILLCREDIT);
            me->ForcedDespawn();
        }
    }

private:
    int _numberOfWaveCleaned = 0;
    int _numberOfSummonsAlive = 0;
    Position pos[8];
    ObjectGuid m_playerGUID;
    ObjectGuid o_hildeGUID;
};

// 131684
class npc_penny_hardwick : public ScriptedAI
{
public:
    npc_penny_hardwick(Creature* creature) : ScriptedAI(creature) { }

    enum
    {
        QUEST_A_VERY_PRECIOUS_CARGO = 50002,
        QUEST_HOLD_MY_HAND = 50005,
        NPC_PENNY_KILLCREDIT = 132725,
        SPELL_CANCEL_ESCORT_PENNY = 259926,
        SPELL_ESCORTING_PENNY_HARDWICK = 259909
    };

    void MoveInLineOfSight(Unit* unit) override
    {
        if (Player* player = unit->ToPlayer())
        {
            float x = me->GetPositionX();
            float y = me->GetPositionY();
            float z = me->GetPositionZ();

            if (fabs(x - 534.932007f) < .1 && fabs(y - 870.984009f) < .1 && fabs(z - 7.821800f) < .1f) // ensuring we don't take the quest form ending npc
                if (player->GetQuestStatus(QUEST_A_VERY_PRECIOUS_CARGO) == QUEST_STATUS_INCOMPLETE)
                    player->KilledMonsterCredit(NPC_PENNY_KILLCREDIT);
        }
    }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_HOLD_MY_HAND)
        {
            player->CastSpell(player, SPELL_CANCEL_ESCORT_PENNY);
            player->CastSpell(player, SPELL_ESCORTING_PENNY_HARDWICK);
        }
    }
};

///TODO Make Penny wait at far
// 131748
class npc_penny_hardwick_escort : public EscortAI
{
public:
    npc_penny_hardwick_escort(Creature* creature) : EscortAI(creature) { }

    enum
    {
        SPELL_CANCEL_ESCORT_PENNY = 259926,
        QUEST_HOLD_MY_HAND = 50005
    };

    void IsSummonedBy(WorldObject* summoner) override
    {
        LoadPath(me->GetEntry());
        Start(false, summoner->GetGUID());
        SetDespawnAtFar(true);
        SetDespawnAtEnd(true);
    }

    /* NEED TO IMPLEMENT LastWaypointReached function FROM ASHAMANECORE or rewrite to actual tc
    void LastWaypointReached(uint32 waypointId, uint32 pathId) override
    {
        if (Player* player = GetPlayerForEscort())
        {
            KillCreditMe(player);
        }
    }
    */

    void JustDied(Unit* /*killer*/) override
    {
        if (Player* player = GetPlayerForEscort())
        {
            if (player->GetQuestStatus(QUEST_HOLD_MY_HAND) != QUEST_STATUS_COMPLETE)
                player->FailQuest(QUEST_HOLD_MY_HAND);
            player->CastSpell(player, SPELL_CANCEL_ESCORT_PENNY);
        }
    }

    void JustAppeared() override
    {
        if (Player* player = GetPlayerForEscort())
        {
            player->FailQuest(QUEST_HOLD_MY_HAND);
            player->CastSpell(player, SPELL_CANCEL_ESCORT_PENNY);
        }
    }

};

//143068, Riding Macaw
struct npc_riding_macaw : public ScriptedAI
{
    npc_riding_macaw(Creature* creature) : ScriptedAI(creature) { }

    enum MyEnums
    {
        QUEST_RODRIGOS_REVENGE = 49403,
        NPC_RIDING_MACAW_PATROL = 143096
    };

    bool OnGossipHello(Player* player) override
    {
        if (player->GetQuestStatus(QUEST_RODRIGOS_REVENGE) == QUEST_STATUS_INCOMPLETE)
            player->SummonCreature(NPC_RIDING_MACAW_PATROL, player->GetPosition(), TEMPSUMMON_MANUAL_DESPAWN);

        return true;
    }
};

// 143096
class npc_riding_macaw_patrol : public EscortAI
{
public:
    enum
    {
        SPELL_SUMMON_RIDING_MACAW = 279049,
        QUEST_RODRIGO_REVENGE = 49403,
        PHASE_RIDING_MACAW = 11659,
    };

    npc_riding_macaw_patrol(Creature* creature) : EscortAI(creature)
    {
        me->SetCanFly(true);
    }

    void OnCharmed(bool isNew) override
    {
        // Make sure the basic cleanup of OnCharmed is done to avoid looping problems
        if (me->IsCharmed() && !me->IsEngaged())
            JustEngagedWith(nullptr);
        ScriptedAI::OnCharmed(isNew);
    }

    void EnterEvadeMode(EvadeReason /*why*/) override { }

    void IsSummonedBy(WorldObject* summoner) override
    {
        if (summoner)
        {
            PhasingHandler::AddPhase(summoner, PHASE_RIDING_MACAW, true);
            me->AddAura(SPELL_SUMMON_RIDING_MACAW, me); // Add the phase shift aura
            _scheduler.Schedule(Seconds(1s), [this, summoner](TaskContext /*task*/)
                {
                    summoner->CastSpell(me, VEHICLE_SPELL_RIDE_HARDCODED);
                }).Schedule(2s, [this, summoner](TaskContext /*context*/)
                    {
                        LoadPath(me->GetEntry());
                        Start(false, summoner->GetGUID());
                    });
        }
    }

    void PassengerBoarded(Unit* passenger, int8 /*seatId*/, bool apply) override
    {
        if (!apply)
        {
            if (passenger)
            {
                PhasingHandler::RemovePhase(passenger, PHASE_RIDING_MACAW, true);
                passenger->RemoveAurasDueToSpell(SPELL_SUMMON_RIDING_MACAW);
                me->ForcedDespawn();
            }
        }
    }
private:
    TaskScheduler _scheduler;
};

// 142721 - Ralston Karn
class npc_ralston_karn : public ScriptedAI
{
public:
    enum
    {
        QUEST_TO_THE_FRONT = 53194,
        NPC_YVERA_DAWNWING_KILLCREDIT = 143380,
        SPELL_TELEPORT_TO_STROMGARDE = 279518
    };

    npc_ralston_karn(Creature* creature) : ScriptedAI(creature) { }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_TO_THE_FRONT)
        {
            player->KilledMonsterCredit(NPC_YVERA_DAWNWING_KILLCREDIT);
            player->CastSpell(player, SPELL_TELEPORT_TO_STROMGARDE);
        }
    }
};

class boralus_harbor : public PlayerScript
{
public:
    boralus_harbor() : PlayerScript("boralus_harbor") { }

    uint32 timer = 100;

    void OnUpdate(Player* plr, uint32 diff) override
    {
        if (timer <= diff && (plr->GetZoneId() == 8717 || (plr->GetZoneId() == 8567 && plr->GetPhaseShift().HasPhase(180))))
        {
            PhasingHandler::RemovePhase(plr, 180, true);
            if (plr->HasAura(78517))
                plr->RemoveAura(78517);
        }
        else
            timer -= diff;
    }
};

// @TODO Rewrite levels
class old_knight_check : public PlayerScript
{
public:
    old_knight_check() : PlayerScript("old_knight_check") { }

    void OnLogin(Player* plr, bool /*firstLogin*/)
    {
        if (plr->GetZoneId() == 8717 || (plr->GetZoneId() == 8567 && plr->GetQuestStatus(QUEST_THE_OLD_KNIGHT) != QUEST_STATUS_REWARDED && plr->GetLevel() >= 110 && plr->GetTeam() == ALLIANCE && plr->GetQuestStatus(QUEST_GET_YOUR_BEARINGS) == QUEST_STATUS_REWARDED))
        {
            if (const Quest* qu = sObjectMgr->GetQuestTemplate(QUEST_THE_OLD_KNIGHT))
                plr->AddQuest(qu, nullptr);
        }
    }
};

enum ExposeAshvaneEnums
{
    QUEST_MAKE_OUR_CASE = 50787,
};

//121144
class npc_katherine_proudmoore_121144 : public ScriptedAI
{
public:
    npc_katherine_proudmoore_121144(Creature* c) : ScriptedAI(c) { }

    void MoveInLineOfSight(Unit* u) override
    {
        if (u->IsPlayer())
            if (Player* plr = u->ToPlayer())
            {
                if (plr->GetQuestStatus(QUEST_NATION_UNITED) == QUEST_STATUS_INCOMPLETE)
                    plr->ForceCompleteQuest(QUEST_NATION_UNITED);

                if (plr->GetQuestStatus(QUEST_MAKE_OUR_CASE) == QUEST_STATUS_INCOMPLETE)
                    if (plr->IsInDist(me, 30.0f))
                    {
                        plr->ForceCompleteQuest(QUEST_MAKE_OUR_CASE);
                        plr->CastSpell(plr, SPELL_SCENE_EXPOSING_ASHVANE, true);
                    }
            }
    }
};

enum WarCampaign80
{
    //Alliance
    QUEST_HEART_OF_DARKNESS = 51088, //Nazmir
    QUEST_VOYAGE_TO_THE_WEST = 51283, //Voldun
    QUEST_THE_ABYSSAL_SCEPTER = 54171, //Zuldazar
    QUEST_OVERSEAS_ASSASINATION = 52026, //Voldun
};

//135681
struct npc_grand_admiral_jes_tereth_135681 : public ScriptedAI
{
    npc_grand_admiral_jes_tereth_135681(Creature* creature) : ScriptedAI(creature) { }

    bool OnGossipHello(Player* player) override
    {
        player->PrepareQuestMenu(player->GetGUID());
        ClearGossipMenuFor(player);
        AddGossipItemFor(player, GossipOptionNpc::GlyphMaster, "Set sail for Vol'dun.!", GOSSIP_SENDER_MAIN, 0);
        AddGossipItemFor(player, GossipOptionNpc::GlyphMaster, "Set sail for Nazmir.!", GOSSIP_SENDER_MAIN, 1);
        AddGossipItemFor(player, GossipOptionNpc::GlyphMaster, "Set sail for Zuldazar.!", GOSSIP_SENDER_MAIN, 2);

        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, me->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, uint32 /*sender*/, uint32 action) override
    {
        switch (action)
        {
        case 0: //Set sail for Vol'dun.

            player->TeleportTo(1642, 2831.0f, 4264.0f, 7.5f, 4.77f);

            if (player->HasQuest(QUEST_OVERSEAS_ASSASINATION))
                player->ForceCompleteQuest(QUEST_OVERSEAS_ASSASINATION);

            if (player->HasQuest(QUEST_VOYAGE_TO_THE_WEST))
                player->ForceCompleteQuest(QUEST_VOYAGE_TO_THE_WEST);
            CloseGossipMenuFor(player);
            break;
        case 1: //Set sail for Nazmir.

            player->TeleportTo(1642, 2130.0f, 193.0f, 0.19f, 2.48f);

            if (player->HasQuest(QUEST_HEART_OF_DARKNESS))
                player->KilledMonsterCredit(136433);
            CloseGossipMenuFor(player);
            break;
        case 2://Set sail for Zuldazar.

            player->TeleportTo(1642, -2618.0f, 2269.0f, 12.9f, 4.98f);

            CloseGossipMenuFor(player);
            break;
        }
        return true;
    }
};


// 130556, injured-marine
struct npc_injured_marine : public ScriptedAI
{
    npc_injured_marine(Creature* creature) : ScriptedAI(creature) { }

    bool clicked;

    enum myEnums
    {
        QUEST_PATCHING_UP_THE_REAR = 49733,
        NPC_KILL_CREDIT_MARINE_HEALED = 130671,
    };

    void Reset() override
    {
        clicked = false;
    }

    bool OnGossipHello(Player* player) override
    {
        if (player->GetQuestStatus(QUEST_PATCHING_UP_THE_REAR) == QUEST_STATUS_INCOMPLETE)
        {
            if (!clicked)
            {
                player->KilledMonsterCredit(NPC_KILL_CREDIT_MARINE_HEALED);
                Talk(rand() % 5);
                me->DespawnOrUnsummon(10s);
                clicked = true;
            }
        }

        return true;
    }
};

//254830, releasing
class spell_releasing : public SpellScript
{
    PrepareSpellScript(spell_releasing);

    bool killed;

public:
    spell_releasing()
    {
        killed = false;
    }

    enum MyEnums
    {
        QUEST_IM_A_DRUID_NOT_A_PRIEST = 49233,
        NPC_CURSED_RAIDER = 128286,
        NPC_KILL_CREDIT_SKLETONS_TURNED = 128446
    };

    void HandleScript()
    {
        Unit* caster = GetCaster();
        Unit* target = GetHitUnit();

        if (!caster || !target)
            return;

        if (Player* player = caster->ToPlayer())
            if (player->GetQuestStatus(QUEST_IM_A_DRUID_NOT_A_PRIEST) == QUEST_STATUS_INCOMPLETE)
                if (Creature* creature = target->ToCreature())
                    if (!killed)
                        if (creature->GetEntry() == NPC_CURSED_RAIDER)
                        {
                            player->GetScheduler().Schedule(Milliseconds(3000), [](TaskContext context)
                                {
                                    if (Player* player = GetContextPlayer())
                                        player->KilledMonsterCredit(NPC_KILL_CREDIT_SKLETONS_TURNED);
                                });

                            creature->DespawnOrUnsummon(3s);
                            killed = true;
                        }
    }

    void Register() override
    {
        AfterHit += SpellHitFn(spell_releasing::HandleScript);
    }
};

// 127016, Stoat-Den
struct npc_stoat_den : public ScriptedAI
{
    npc_stoat_den(Creature* creature) : ScriptedAI(creature) { }

    enum myEnums
    {
        QUEST_THE_STOAT_HUNT = 48077,
        NPC_FAINTSTEP_STOAT = 125327,
    };

    void OnSpellClick(Unit* clicker, bool /*result*/) override
    {
        if (Player* player = clicker->ToPlayer())
        {
            if (player->GetQuestStatus(QUEST_THE_STOAT_HUNT) == QUEST_STATUS_INCOMPLETE)
            {
                me->SummonCreature(NPC_FAINTSTEP_STOAT, me->GetPosition(), TEMPSUMMON_MANUAL_DESPAWN);
                me->SummonCreature(NPC_FAINTSTEP_STOAT, me->GetPositionX(), me->GetPositionY() + urand(2, 3), me->GetPositionZ(), 0, TEMPSUMMON_MANUAL_DESPAWN);
                me->SummonCreature(NPC_FAINTSTEP_STOAT, me->GetPositionX(), me->GetPositionY() - urand(2, 3), me->GetPositionZ(), 0, TEMPSUMMON_MANUAL_DESPAWN);
                me->DespawnOrUnsummon(10ms);
            }
        }
    }
};

//251902, shoot-bola
class spell_shoot_bola : public SpellScript
{
    PrepareSpellScript(spell_shoot_bola);

    enum MyEnums
    {
        QUEST_BOLAS_AND_BIRDS = 48616,
        NPC_HOLLOWBEAK_FALCON = 126626,
        NPC_FALCON_SHOT_KILL_CREDIT = 127013,
    };

    void HandleScript()
    {
        Unit* caster = GetCaster();
        Unit* target = GetHitUnit();

        if (!caster || !target)
            return;

        if (Player* player = caster->ToPlayer())
            if (player->GetQuestStatus(QUEST_BOLAS_AND_BIRDS) == QUEST_STATUS_INCOMPLETE)
                if (Creature* creature = target->ToCreature())
                    if (creature->GetEntry() == NPC_HOLLOWBEAK_FALCON)
                    {
                        player->KilledMonsterCredit(NPC_FALCON_SHOT_KILL_CREDIT);
                        creature->KillSelf();
                    }
    }

    void Register() override
    {
        AfterHit += SpellHitFn(spell_shoot_bola::HandleScript);
    }
};

struct npc_be_our_guest_quest : public ScriptedAI
{
    npc_be_our_guest_quest(Creature* creature) : ScriptedAI(creature) { }

    enum MyEnums
    {
        QUEST_BE_OUR_GUEST = 48005,
    };

    bool OnGossipHello(Player* player) override
    {
        if (player->GetQuestStatus(QUEST_BE_OUR_GUEST) == QUEST_STATUS_INCOMPLETE)
            player->KilledMonsterCredit(me->GetEntry());

        return false;
    }
};

enum QuestBeginnerEquitationEnums
{
    QUEST_BEGINNER_EQUITATION = 48004,
    QUEST_BEST_IN_SHOW = 49036,
    QUEST_SHOW_ME_WHAT_YOU_HAVE_GOT = 48939,
    NPC_LORD_ALDARIUS_NORWINGTON = 124802,
    NPC_COOPER_MOUNT = 124402
};

//127718, Cooper
struct npc_cooper_main : public ScriptedAI
{
    npc_cooper_main(Creature* creature) : ScriptedAI(creature) { }

    void OnSpellClick(Unit* clicker, bool /*result*/) override
    {
        if (Player* player = clicker->ToPlayer())
        {
            if (player->GetQuestStatus(QUEST_BEST_IN_SHOW) == QUEST_STATUS_INCOMPLETE)
                player->ForceCompleteQuest(QUEST_BEST_IN_SHOW);

            if (player->GetQuestStatus(QUEST_SHOW_ME_WHAT_YOU_HAVE_GOT) == QUEST_STATUS_INCOMPLETE)
                player->ForceCompleteQuest(QUEST_SHOW_ME_WHAT_YOU_HAVE_GOT);

            if (player->GetQuestStatus(QUEST_BEGINNER_EQUITATION) == QUEST_STATUS_INCOMPLETE)
            {
                player->ForceCompleteQuest(QUEST_BEGINNER_EQUITATION);

                if (Creature* aldarius = me->FindNearestCreature(NPC_LORD_ALDARIUS_NORWINGTON, 100.0f))
                    aldarius->AI()->Talk(0);
            }
        }
    }
};

//124402, Cooper
struct npc_cooper_mount : public VehicleAI
{
    npc_cooper_mount(Creature* creature) : VehicleAI(creature) { }

    void IsSummonedBy(WorldObject* summoner) override
    {
        if (Player* player = summoner->ToPlayer())
        {
            if (player->GetQuestStatus(QUEST_BEGINNER_EQUITATION) == QUEST_STATUS_INCOMPLETE)
            {
                me->GetVehicleKit();
                player->EnterVehicle(me);
            }
        }
    }

    void OnSpellClick(Unit* clicker, bool /*result*/) override
    {
        if (Player* player = clicker->ToPlayer())
        {
            if (player->GetQuestStatus(QUEST_BEGINNER_EQUITATION) == QUEST_STATUS_INCOMPLETE)
            {
                me->GetVehicleKit();
                player->EnterVehicle(me);
            }
        }
    }
};

//124802, Lord Aldrius Norwington
struct npc_lord_aldarius_norwington : public ScriptedAI
{
    npc_lord_aldarius_norwington(Creature* creature) : ScriptedAI(creature) { }

    bool talked;

    void Reset() override
    {
        ScriptedAI::Reset();
        talked = false;
    }

    void OnQuestReward(Player* player, Quest const* quest, LootItemType /*type*/, uint32 /*opt*/) override
    {
        if (quest->GetQuestId() == QUEST_BEGINNER_EQUITATION)
        {
            Talk(3);
            player->ExitVehicle();
            std::list<Creature*> targetsCreatures;
            player->GetCreatureListWithEntryInGrid(targetsCreatures, NPC_COOPER_MOUNT, 100.0f);
            for (auto& target : targetsCreatures)
                target->DespawnOrUnsummon(10ms);
        }
    }

    void OnQuestAccept(Player* /*player*/, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_BEGINNER_EQUITATION)
            Talk(1);
    }
};

enum QuestEquineRetrievalEnums
{
    QUEST_EQUINE_RETRIEVAL = 48087,
};

//127534, Rose
struct npc_rose_main : public ScriptedAI
{
    npc_rose_main(Creature* creature) : ScriptedAI(creature) { }

    void OnSpellClick(Unit* clicker, bool /*result*/) override
    {
        if (Player* player = clicker->ToPlayer())
        {
            if (player->GetQuestStatus(QUEST_EQUINE_RETRIEVAL) == QUEST_STATUS_INCOMPLETE)
                player->ForceCompleteQuest(QUEST_EQUINE_RETRIEVAL);
        }
    }
};

//127541, Rose
struct npc_rose_mount : public VehicleAI
{
    npc_rose_mount(Creature* creature) : VehicleAI(creature) { }

    void IsSummonedBy(WorldObject* summoner) override
    {
        if (Player* player = summoner->ToPlayer())
        {
            if (player->GetQuestStatus(QUEST_EQUINE_RETRIEVAL) == QUEST_STATUS_INCOMPLETE)
            {
                me->GetVehicleKit();
                player->EnterVehicle(me);
            }
        }
    }
};

class kultiras_phase_manager : public PlayerScript
{
public:
    kultiras_phase_manager() : PlayerScript("kultiras_phase_manager") { }

    enum QuestNoPartyEnums
    {
        QUEST_NO_PARTY_LIKE_A_TROGG_PARTY = 48088,
        QUEST_TIRAGARDE_SOUND = 47960,
        QUEST_RODRIGOS_REVENGE = 49403,
        PHASE_ID_TROGGS_INTER_CITY = 11659,
        MAP_KULTIRAS_PHASING_AREA = 9011,
        HARBORMASTERS_OFFICE_AREA = 9802
    };

    uint32 PHASE_UPDATE_DELAY = 5000; // phase update delay in milliseconds
    uint32 PHASE_UPDATE_DELAY_DIFF = PHASE_UPDATE_DELAY;

    void OnUpdate(Player* player, uint32 diff) override
    {
        if (player->GetAreaId() == MAP_KULTIRAS_PHASING_AREA)
        {
            if (PHASE_UPDATE_DELAY_DIFF <= diff)
            {
                if (player->GetQuestStatus(QUEST_NO_PARTY_LIKE_A_TROGG_PARTY) == QUEST_STATUS_INCOMPLETE)
                    PhasingHandler::AddPhase(player, PHASE_ID_TROGGS_INTER_CITY, true);
                else if (player->GetQuestStatus(QUEST_NO_PARTY_LIKE_A_TROGG_PARTY) == QUEST_STATUS_COMPLETE)
                    PhasingHandler::RemovePhase(player, PHASE_ID_TROGGS_INTER_CITY, true);

                PHASE_UPDATE_DELAY_DIFF = PHASE_UPDATE_DELAY;
            }
            else
                PHASE_UPDATE_DELAY_DIFF -= diff;
        }
        else if (player->GetAreaId() == HARBORMASTERS_OFFICE_AREA)
        {
            if (PHASE_UPDATE_DELAY_DIFF <= diff)
            {
                if (player->GetQuestStatus(QUEST_TIRAGARDE_SOUND) == QUEST_STATUS_NONE)
                {
                    if (player->GetLevel() >= 30)
                    {
                        if (const Quest* quest_1 = sObjectMgr->GetQuestTemplate(QUEST_TIRAGARDE_SOUND))
                        {
                            player->AddQuest(quest_1, nullptr);
                            player->ForceCompleteQuest(QUEST_TIRAGARDE_SOUND);
                        }
                    }
                }

                PHASE_UPDATE_DELAY_DIFF = PHASE_UPDATE_DELAY;
            }
            else
                PHASE_UPDATE_DELAY_DIFF -= diff;
        }
    }
};

//253855, Battlehorn of the Mountain
class spell_battlehorn_of_the_mountain : public SpellScript
{
    PrepareSpellScript(spell_battlehorn_of_the_mountain);

    enum MyEnums
    {
        QUEST_MOUNTAIN_SOUNDS = 48089,
        NPC_KULLTIRAS_EMPOWERED_KILL_CREDIT = 127940,
        NPC_KULLTIRAN_NOBLE_1 = 127562,
        NPC_KULLTIRAN_NOBLE_2 = 127563,
        NPC_KULLTIRAN_NOBLE_3 = 127564,
        NPC_KULLTIRAN_NOBLE_4 = 127565,
        NPC_KULLTIRAN_NOBLE_5 = 127566,
    };

    void HandleScript()
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        if (Player* player = caster->ToPlayer())
            if (player->GetQuestStatus(QUEST_MOUNTAIN_SOUNDS) == QUEST_STATUS_INCOMPLETE)
            {
                std::list<Creature*> targetsCreatures;
                player->GetCreatureListWithEntryInGrid(targetsCreatures, NPC_KULLTIRAN_NOBLE_1, 15.0f);
                player->GetCreatureListWithEntryInGrid(targetsCreatures, NPC_KULLTIRAN_NOBLE_2, 15.0f);
                player->GetCreatureListWithEntryInGrid(targetsCreatures, NPC_KULLTIRAN_NOBLE_3, 15.0f);
                player->GetCreatureListWithEntryInGrid(targetsCreatures, NPC_KULLTIRAN_NOBLE_4, 15.0f);
                player->GetCreatureListWithEntryInGrid(targetsCreatures, NPC_KULLTIRAN_NOBLE_5, 15.0f);
                for (auto& target : targetsCreatures)
                {
                    player->KilledMonsterCredit(NPC_KULLTIRAS_EMPOWERED_KILL_CREDIT);
                    target->DespawnOrUnsummon(10ms);
                }
            }
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_battlehorn_of_the_mountain::HandleScript);
    }
};

//256323, Shaving
class spell_shaving : public SpellScript
{
    PrepareSpellScript(spell_shaving);

    bool shaved;

public:
    spell_shaving()
    {
        shaved = false;
    }

    enum MyEnums
    {
        QUEST_HOLD_STILL = 49394,
        NPC_GENTLE_GOAT = 129110,
        ITEM_GOAT_FUR = 155681,
    };

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        if (!caster || shaved)
            return;

        if (Player* player = caster->ToPlayer())
            if (player->GetQuestStatus(QUEST_HOLD_STILL) == QUEST_STATUS_INCOMPLETE)
            {
                player->AddItem(ITEM_GOAT_FUR, 1);
                shaved = true;
            }
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_shaving::HandleScript, EFFECT_1, SPELL_EFFECT_CREATE_LOOT);
    }
};

enum QuestRoughneckRidersEnums
{
    QUEST_ROUGHNECK_RIDERS = 49417,
    NPC_GREATFEATHER = 137128,
    WAYPOINT_ID = 13712801,
    EVENT_START_FLY = 1,
    ACTION_FLY
};

//137128, Greatfeather
struct npc_greatfeather : public VehicleAI
{
    npc_greatfeather(Creature* creature) : VehicleAI(creature) { }

    ObjectGuid  m_playerGUID;
    EventMap    m_events;

    void Reset() override
    {
        m_events.Reset();
        m_playerGUID = ObjectGuid::Empty;
    }

    void GetPlayersForQuest()
    {
        Map::PlayerList const& playerList = me->GetMap()->GetPlayers();
        if (!playerList.isEmpty())
            for (Map::PlayerList::const_iterator i = playerList.begin(); i != playerList.end(); ++i)
                if (Player* player = i->GetSource())
                    if (player->GetDistance2d(me) < 60.0f)
                        m_playerGUID = player->GetGUID();
    }

    void DoAction(int32 const action) override
    {
        if (action == ACTION_FLY)
        {
            me->GetVehicleKit();
            GetPlayersForQuest();
            if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                player->EnterVehicle(me);
            m_events.ScheduleEvent(EVENT_START_FLY, 10s);
        }
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        if (type == WAYPOINT_MOTION_TYPE && id == 4)
        {
            if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                if (player->GetQuestStatus(QUEST_ROUGHNECK_RIDERS) == QUEST_STATUS_INCOMPLETE)
                    if (player->GetDistance2d(me) < 30.0f)
                        player->ForceCompleteQuest(QUEST_ROUGHNECK_RIDERS);
            me->DespawnOrUnsummon(100ms);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        m_events.Update(diff);

        while (uint32 eventId = m_events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_START_FLY:
                me->GetMotionMaster()->MovePath(WAYPOINT_ID, false);
                break;
            default:
                break;
            }
        }
    }
};

//256465, Tether Shot
class spell_tether_shot : public SpellScript
{
    PrepareSpellScript(spell_tether_shot);

    void HandleScript()
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        if (Player* player = caster->ToPlayer())
            if (player->GetQuestStatus(QUEST_ROUGHNECK_RIDERS) == QUEST_STATUS_INCOMPLETE)
            {
                std::list<Creature*> targetsCreatures;
                player->GetCreatureListWithEntryInGrid(targetsCreatures, NPC_GREATFEATHER, 60.0f);
                for (auto& target : targetsCreatures)
                {
                    target->AI()->DoAction(ACTION_FLY);
                    break;
                }
            }
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_tether_shot::HandleScript);
    }
};

//122671, Cagney
struct npc_cagney : public ScriptedAI
{
    npc_cagney(Creature* creature) : ScriptedAI(creature) { }

    enum MyEnums
    {
        QUEST_SUSPICIOUS_SHIPMENTS = 47486,
    };

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_SUSPICIOUS_SHIPMENTS)
            player->ForceCompleteQuest(QUEST_SUSPICIOUS_SHIPMENTS);
    }
};

//128377, Beachcomber Bob
struct npc_beachcomber_bob : public ScriptedAI
{
    npc_beachcomber_bob(Creature* creature) : ScriptedAI(creature) { }

    enum MyEnums
    {
        QUEST_STOW_AND_GO = 47489,
    };

    void MoveInLineOfSight(Unit* unit) override
    {
        if (Player* player = unit->ToPlayer())
            if (player->GetDistance(me) < 50.0f)
                if (player->GetQuestStatus(QUEST_STOW_AND_GO) == QUEST_STATUS_INCOMPLETE)
                    player->ForceCompleteQuest(QUEST_STOW_AND_GO);
    }
};

struct npc_my_favorite_things_quest : public ScriptedAI
{
    npc_my_favorite_things_quest(Creature* creature) : ScriptedAI(creature) { }

    enum MyEnums
    {
        QUEST_MY_FAVORITE_THINGS = 49178,
    };

    bool OnGossipHello(Player* player) override
    {
        if (player->GetQuestStatus(QUEST_MY_FAVORITE_THINGS) == QUEST_STATUS_INCOMPLETE)
            player->KilledMonsterCredit(me->GetEntry());

        return false;
    }
};

enum FlynnFairwindEnums
{
    QUEST_DRESS_TO_IMPRESS = 49239,
    QUEST_FAIRWINDS_FRIENDS = 49404,
    SPELL_IRONTIDE_REQRUIT = 254873,
    KILL_CREDIT_ENTER_ARENA = 130102,
    KILL_CREDIT_UNTIE_FLYNN = 130081,
    KILL_CREDIT_GAMBLE_WITH_HARLAN = 130106,
};

//126620, Flynn Fairwind
struct npc_flynn_fairwind_126620 : public ScriptedAI
{
    npc_flynn_fairwind_126620(Creature* creature) : ScriptedAI(creature) { }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_DRESS_TO_IMPRESS)
            player->CastSpell(player, SPELL_IRONTIDE_REQRUIT, true);
    }
};

//130081, Flynn Fairwind
struct npc_flynn_fairwind_130081 : public ScriptedAI
{
    npc_flynn_fairwind_130081(Creature* creature) : ScriptedAI(creature) { }

    bool  clicked;

    void Reset() override
    {
        clicked = false;
    }

    void MoveInLineOfSight(Unit* unit) override
    {
        if (Player* player = unit->ToPlayer())
            if (player->GetDistance(me) < 20.0f)
                if (player->GetQuestStatus(QUEST_FAIRWINDS_FRIENDS) == QUEST_STATUS_INCOMPLETE)
                {
                    player->KilledMonsterCredit(KILL_CREDIT_ENTER_ARENA);

                    if (player->HasAura(SPELL_IRONTIDE_REQRUIT))
                        player->RemoveAura(SPELL_IRONTIDE_REQRUIT);
                }
    }

    void OnSpellClick(Unit* clicker, bool /*result*/) override
    {
        if (!clicked)
            if (Player* player = clicker->ToPlayer())
            {
                if (player->GetQuestStatus(QUEST_FAIRWINDS_FRIENDS) == QUEST_STATUS_INCOMPLETE)
                {
                    player->KilledMonsterCredit(KILL_CREDIT_UNTIE_FLYNN);
                    clicked = true;

                    me->GetScheduler().Schedule(Milliseconds(5000), [player](TaskContext context)
                        {
                            player->KilledMonsterCredit(KILL_CREDIT_GAMBLE_WITH_HARLAN);
                            GetContextCreature()->DespawnOrUnsummon(100ms);
                        });
                }
            }
    }
};

//278308, Irontide Recruiting Poster
struct go_irontide_recruiting_poster : public GameObjectAI
{
    go_irontide_recruiting_poster(GameObject* go) : GameObjectAI(go) { }

    bool clicked;

    enum MyEnums
    {
        QUEST_RECRUITING_EFFORTS = 49400,
        GO_IRONTIDE_RECRUITING_POSTER = 278308
    };

    void Reset() override
    {
        clicked = false;
    }

    bool OnGossipHello(Player* player) override
    {
        if (!clicked)
            if (player->GetQuestStatus(QUEST_RECRUITING_EFFORTS) == QUEST_STATUS_INCOMPLETE)
            {
                clicked = true;
                player->KillCreditGO(GO_IRONTIDE_RECRUITING_POSTER);
            }

        return true;
    }
};

//140958, Greatfeather
struct npc_galeheart : public VehicleAI
{
    npc_galeheart(Creature* creature) : VehicleAI(creature) { }

    ObjectGuid  m_playerGUID;
    EventMap    m_events;

    enum MyEnums
    {
        QUEST_DEFENDERS_OF_DAELIN_GATE = 49405,
        NPC_GALEHEART = 130158,
        WAYPOINT_ID_GALEHEART = 14095801
    };

    void Reset() override
    {
        m_events.Reset();
        m_playerGUID = ObjectGuid::Empty;
    }

    void IsSummonedBy(WorldObject* summoner) override
    {
        if (Player* player = summoner->ToPlayer())
        {
            if (player->GetQuestStatus(QUEST_DEFENDERS_OF_DAELIN_GATE) == QUEST_STATUS_INCOMPLETE)
            {
                me->GetVehicleKit();
                player->EnterVehicle(me);
                player->KilledMonsterCredit(NPC_GALEHEART);
                m_events.ScheduleEvent(EVENT_START_FLY, 10s);
            }
        }
    }

    void GetPlayersForQuest()
    {
        Map::PlayerList const& playerList = me->GetMap()->GetPlayers();
        if (!playerList.isEmpty())
            for (Map::PlayerList::const_iterator i = playerList.begin(); i != playerList.end(); ++i)
                if (Player* player = i->GetSource())
                    if (player->GetDistance2d(me) < 20.0f)
                        m_playerGUID = player->GetGUID();
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        if (type == WAYPOINT_MOTION_TYPE && id == 5)
        {
            GetPlayersForQuest();
            if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                if (player->GetQuestStatus(QUEST_DEFENDERS_OF_DAELIN_GATE) == QUEST_STATUS_INCOMPLETE)
                    if (player->GetDistance2d(me) < 20.0f)
                        player->ForceCompleteQuest(QUEST_DEFENDERS_OF_DAELIN_GATE);
            me->DespawnOrUnsummon(100ms);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        m_events.Update(diff);

        while (uint32 eventId = m_events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_START_FLY:
                me->GetMotionMaster()->MovePath(WAYPOINT_ID_GALEHEART, false);
                break;
            default:
                break;
            }
        }
    }
};

//131220, Vigil Hill Cannon
struct npc_vigil_hill_cannon : public VehicleAI
{
    npc_vigil_hill_cannon(Creature* creature) : VehicleAI(creature) { }

    bool OnGossipHello(Player* player) override
    {
        me->GetVehicleKit();
        player->EnterVehicle(me);

        return true;
    }
};

enum ScratchyEnums
{
    QUEST_CAT_ON_A_HOT_COPPER_ROOF = 49757,
    NPC_SCRATCHY = 130746,
    NPC_MEREDITH = 131654,
    SPELL_SCRATCHED = 258718
};

//131654, Meredith
struct npc_meredith : public ScriptedAI
{
    npc_meredith(Creature* creature) : ScriptedAI(creature) { }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_CAT_ON_A_HOT_COPPER_ROOF)
        {
            Talk(0);
            me->GetScheduler().Schedule(Milliseconds(5000), [player](TaskContext /*context*/)
                {
                    player->NearTeleportTo(-720.645f, 626.148f, 58.362f, 5.374f, false);
                });
        }
    }

    void OnQuestReward(Player* /*player*/, Quest const* quest, LootItemType /*type*/, uint32 /*opt*/) override
    {
        if (quest->GetQuestId() == QUEST_CAT_ON_A_HOT_COPPER_ROOF)
        {
            Talk(2);
            me->GetScheduler().Schedule(Milliseconds(4000), [](TaskContext context)
                {
                    GetContextCreature()->AI()->Talk(3);
                });
        }
    }
};

//130746, Scratchy
struct npc_scratchy : public ScriptedAI
{
    npc_scratchy(Creature* creature) : ScriptedAI(creature) { }

    bool scratched;

    void Reset() override
    {
        scratched = false;
    }

    void OnSpellClick(Unit* clicker, bool /*result*/) override
    {
        if (Player* player = clicker->ToPlayer())
        {
            if (player->GetQuestStatus(QUEST_CAT_ON_A_HOT_COPPER_ROOF) == QUEST_STATUS_INCOMPLETE)
            {
                if (!scratched)
                {
                    scratched = true;
                    me->CastSpell(player, SPELL_SCRATCHED, false);
                    return;
                }
                else
                {
                    if (Creature* meredith = me->FindNearestCreature(NPC_MEREDITH, 100.0f, true))
                    {
                        meredith->AI()->Talk(1);
                        player->ForceCompleteQuest(QUEST_CAT_ON_A_HOT_COPPER_ROOF);
                        me->DespawnOrUnsummon();
                        return;
                    }
                }
            }
        }
    }
};

enum ProudmooreBattleStandardEnums
{
    QUEST_FOR_KUL_TIRAS = 49736,
    NPC_VIGIL_HILL_MARINE = 131324,
    NPC_VIGIL_HILL_MILITIA = 131325,
    KILL_CREDIT_INSPIRE_KUL_TIRANS = 146407,
    SPELL_RPOUDMOORE_BATTLE_STANDARD = 259357
};

//156520, Proudmoore Battle Standard
class item_proudmoore_battle_standard : public ItemScript
{
public:
    item_proudmoore_battle_standard() : ItemScript("item_proudmoore_battle_standard") { }

    bool OnUse(Player* player, Item* /*item*/, SpellCastTargets const& /*targets*/, ObjectGuid /*castId*/) override
    {
        if (player->GetAreaId() != 9378)
            return true;

        std::list<Creature*> spellTargetsList;
        player->GetCreatureListWithEntryInGrid(spellTargetsList, NPC_VIGIL_HILL_MARINE, 10.0f);
        player->GetCreatureListWithEntryInGrid(spellTargetsList, NPC_VIGIL_HILL_MILITIA, 10.0f);
        for (auto& target : spellTargetsList)
            if (!target->HasAura(SPELL_RPOUDMOORE_BATTLE_STANDARD))
                if (player->GetQuestStatus(QUEST_FOR_KUL_TIRAS) == QUEST_STATUS_INCOMPLETE)
                    player->KilledMonsterCredit(NPC_VIGIL_HILL_MARINE);

        return false;
    }
};

//142393, Taelia
struct npc_taelia_142393 : public ScriptedAI
{
    npc_taelia_142393(Creature* creature) : ScriptedAI(creature) { }

    enum ProudmooreBattleStandardEnums
    {
        QUEST_RIGHTEOUS_RETRIBUTION = 49741,
    };

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_RIGHTEOUS_RETRIBUTION)
            player->ForceCompleteQuest(QUEST_RIGHTEOUS_RETRIBUTION);
    }
};

enum GaleheartEnums
{
    QUEST_BEARERS_OF_BAD_NEWS = 50110,
    NPC_GALEHEART = 142407,
    WAYPOINT_ID_GALEHEART = 14240701,
    EVENT_START_FLY_GALEHEART = 1
};

//124727, Galeheart
struct npc_galeheart_124727 : public ScriptedAI
{
    npc_galeheart_124727(Creature* creature) : ScriptedAI(creature) { }

    bool OnGossipHello(Player* player) override
    {
        if (player->GetQuestStatus(QUEST_BEARERS_OF_BAD_NEWS) == QUEST_STATUS_INCOMPLETE)
            player->SummonCreature(NPC_GALEHEART, player->GetPosition(), TEMPSUMMON_MANUAL_DESPAWN);

        return true;
    }
};

//142407, Galeheart
struct npc_galeheart_142407 : public VehicleAI
{
    npc_galeheart_142407(Creature* creature) : VehicleAI(creature) { }

    ObjectGuid  m_playerGUID;
    EventMap    m_events;

    void Reset() override
    {
        m_events.Reset();
        m_playerGUID = ObjectGuid::Empty;
    }

    void IsSummonedBy(WorldObject* summoner) override
    {
        if (Player* player = summoner->ToPlayer())
        {
            if (player->GetQuestStatus(QUEST_BEARERS_OF_BAD_NEWS) == QUEST_STATUS_INCOMPLETE)
            {
                me->GetVehicleKit();
                player->EnterVehicle(me);
                player->KilledMonsterCredit(NPC_GALEHEART);
                m_events.ScheduleEvent(EVENT_START_FLY, 10s);
            }
        }
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        if (type == WAYPOINT_MOTION_TYPE && id == 4)
            me->DespawnOrUnsummon(100ms);
    }

    void UpdateAI(uint32 diff) override
    {
        m_events.Update(diff);

        while (uint32 eventId = m_events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_START_FLY_GALEHEART:
                me->GetMotionMaster()->MovePath(WAYPOINT_ID_GALEHEART, false);
                break;
            default:
                break;
            }
        }
    }
};

// 135075, Proudmoore Guard
class npc_proudmoore_guard : public ScriptedAI
{
public:
    npc_proudmoore_guard(Creature* creature) : ScriptedAI(creature) { }

    bool rallied;

    enum MyEnums
    {
        QUEST_PREPARE_FOR_TROUBLE = 50795,
    };

    void Reset() override
    {
        rallied = false;
    }

    bool OnGossipSelect(Player* player, uint32 /*menuId*/, uint32 /*gossipListId*/) override
    {
        if (player->GetQuestStatus(QUEST_PREPARE_FOR_TROUBLE) == QUEST_STATUS_INCOMPLETE)
            if (!rallied)
            {
                rallied = true;
                Talk(rand() % 3);
                CloseGossipMenuFor(player);
                player->KilledMonsterCredit(me->GetEntry());
                me->GetScheduler().Schedule(Milliseconds(120000), [](TaskContext context)
                    {
                        GetContextCreature()->AI()->Reset();
                    });
            }

        return true;
    }
};

enum HotPursuitEnums
{
    QUEST_HOT_PURSUIT = 50790,
    NPC_PROUDMOORE_CHARGER = 135683
};

//135683, Proudmoore Charger
struct npc_proudmoore_charger : public VehicleAI
{
    npc_proudmoore_charger(Creature* creature) : VehicleAI(creature) { }

    void IsSummonedBy(WorldObject* summoner) override
    {
        if (Player* player = summoner->ToPlayer())
        {
            if (player->GetQuestStatus(QUEST_HOT_PURSUIT) == QUEST_STATUS_INCOMPLETE)
            {
                me->GetVehicleKit();
                player->EnterVehicle(me);
                player->ForceCompleteQuest(QUEST_HOT_PURSUIT);
            }
        }
    }
};

//135259, Taelia
class npc_taelia_135259 : public ScriptedAI
{
public:
    npc_taelia_135259(Creature* creature) : ScriptedAI(creature) { }

    void OnQuestAccept(Player* /*player*/, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_HOT_PURSUIT)
            Talk(0);
    }

    void OnQuestReward(Player* /*player*/, Quest const* quest, LootItemType /*type*/, uint32 /*opt*/) override
    {
        if (quest->GetQuestId() == QUEST_HOT_PURSUIT)
        {
            Talk(1);
            me->GetScheduler().Schedule(Milliseconds(5000), [](TaskContext context)
                {
                    GetContextCreature()->AI()->Talk(2);
                });

            if (Creature* proudmooreCharger = me->FindNearestCreature(NPC_PROUDMOORE_CHARGER, 100.0f))
                proudmooreCharger->DespawnOrUnsummon(10ms);
        }
    }
};

//128941, 128979, Nauseated Villager
class npc_nauseated_villager : public ScriptedAI
{
public:
    npc_nauseated_villager(Creature* creature) : ScriptedAI(creature) { }

    enum MyEnums
    {
        QUEST_ALGAE_SHAKES = 49292,
        NPC_INFECTIOUS_GUTWORM = 129046,
        NPC_SICK_VILLAGER_PROXY = 128960,
        SPELL_SICK = 255503
    };

    void OnSpellClick(Unit* clicker, bool /*result*/) override
    {
        if (Player* player = clicker->ToPlayer())
        {
            if (player->GetQuestStatus(QUEST_ALGAE_SHAKES) == QUEST_STATUS_INCOMPLETE)
            {
                me->GetScheduler().Schedule(Milliseconds(2500), [player](TaskContext context)
                    {
                        Creature* me = GetContextCreature();

                        me->AI()->Talk(0);
                        me->SetAIAnimKitId(0);
                        me->RemoveAurasDueToSpell(SPELL_SICK);
                        player->KilledMonsterCredit(NPC_SICK_VILLAGER_PROXY);
                        if (TempSummon* ts1 = me->SummonCreature(NPC_INFECTIOUS_GUTWORM, me->GetPositionX(), me->GetPositionY() + urand(2, 3), me->GetPositionZ(), 0, TEMPSUMMON_CORPSE_DESPAWN))
                            ts1->AI()->AttackStart(player);
                        if (TempSummon* ts2 = me->SummonCreature(NPC_INFECTIOUS_GUTWORM, me->GetPositionX(), me->GetPositionY() - urand(2, 3), me->GetPositionZ(), 0, TEMPSUMMON_CORPSE_DESPAWN))
                            ts2->AI()->AttackStart(player);
                        me->GetMotionMaster()->MoveFleeing(player, 3s);
                    });
                me->DespawnOrUnsummon(5s);
            }
        }
    }
};

//126060, Barrel of Fish
class npc_barrel_of_fish : public ScriptedAI
{
public:
    npc_barrel_of_fish(Creature* creature) : ScriptedAI(creature) { }

    enum MyEnums
    {
        QUEST_TAINTED_SHIPMENTS = 48354,
        NPC_INFESTING_BRAINWORM = 126135,
        NPC_BARREL_CHECKED_KILL_CREDIT = 126136,
    };

    void OnSpellClick(Unit* clicker, bool /*result*/) override
    {
        if (Player* player = clicker->ToPlayer())
        {
            if (player->GetQuestStatus(QUEST_TAINTED_SHIPMENTS) == QUEST_STATUS_INCOMPLETE)
            {
                me->GetScheduler().Schedule(Milliseconds(1500), [player](TaskContext context)
                    {
                        Creature* me = GetContextCreature();

                        me->DespawnOrUnsummon(100ms);
                        player->KilledMonsterCredit(NPC_BARREL_CHECKED_KILL_CREDIT);
                        if (TempSummon* ts1 = me->SummonCreature(NPC_INFESTING_BRAINWORM, me->GetPosition(), TEMPSUMMON_CORPSE_DESPAWN))
                            ts1->AI()->AttackStart(player);
                    });
            }
        }
    }
};

//125955, Anglepoint Fishpacker
class npc_anglepoint_fishpacker : public ScriptedAI
{
public:
    npc_anglepoint_fishpacker(Creature* creature) : ScriptedAI(creature) { }

    enum MyEnums
    {
        QUEST_EVACUATE_THE_PREMISES = 48355,
        NPC_ANGLEPOINT_FISHPACKER_EVACUATED = 125955,
        NPC_SKITTERING_BRAINWORM = 136927,
        NPC_MERCILESS_MINDEATER = 136938,
    };

    bool OnGossipHello(Player* player) override
    {
        if (player->GetQuestStatus(QUEST_EVACUATE_THE_PREMISES) == QUEST_STATUS_INCOMPLETE)
        {
            Talk(rand() % 4);
            player->KilledMonsterCredit(NPC_ANGLEPOINT_FISHPACKER_EVACUATED);
            if (TempSummon* ts1 = me->SummonCreature(NPC_SKITTERING_BRAINWORM, me->GetPositionX(), me->GetPositionY() + urand(2, 3), me->GetPositionZ(), 0, TEMPSUMMON_CORPSE_DESPAWN))
                ts1->AI()->AttackStart(player);
            if (TempSummon* ts2 = me->SummonCreature(NPC_MERCILESS_MINDEATER, me->GetPositionX(), me->GetPositionY() - urand(2, 3), me->GetPositionZ(), 0, TEMPSUMMON_CORPSE_DESPAWN))
                ts2->AI()->AttackStart(player);
            me->GetMotionMaster()->MoveFleeing(player, 3s);
            me->DespawnOrUnsummon(3s);
        }

        return true;
    }
};

//125959, Parasitic Mindstealer
class npc_parasitic_mindstealer : public ScriptedAI
{
public:
    npc_parasitic_mindstealer(Creature* creature) : ScriptedAI(creature) { }

    enum MyEnums
    {
        QUEST_POSSESSIVE_HEADGEAR = 48356,
        NPC_POSSESSED_DOCKWORKER = 125957,
    };

    void DamageTaken(Unit* attacker, uint32& /*damage*/, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/) override
    {
        if (Player* player = attacker->ToPlayer())
            if (me->GetHealth() < me->CountPctFromMaxHealth(10)) // the intent is not to kill but to sparr
                FreeTheVillager(player);

    }

    void JustDied(Unit* killer) override
    {
        if (Player* player = killer->ToPlayer())
            FreeTheVillager(player, false);
    }

    void FreeTheVillager(Player* player, bool credit = true)
    {
        if (player->GetQuestStatus(QUEST_POSSESSIVE_HEADGEAR) == QUEST_STATUS_INCOMPLETE)
        {
            if (credit)
                player->KilledMonsterCredit(me->GetEntry());
            if (TempSummon* ts = me->SummonCreature(NPC_POSSESSED_DOCKWORKER, me->GetPosition(), TEMPSUMMON_MANUAL_DESPAWN))
            {
                ts->GetMotionMaster()->MoveFleeing(player, 3s);
                ts->AI()->Talk(rand() % 3);
                ts->DespawnOrUnsummon(3s);
            }
            me->DespawnOrUnsummon(100ms);
        }
    }
};

enum StormsongEnums
{
    QUEST_THE_YONG_LORD_STORMSONG = 48365,
    NPC_BRANNON_STORMSONG = 126298,
    NPC_BROTHER_THEROLD = 125922
};

//125961, Brother Conway
class npc_brother_conway : public ScriptedAI
{
public:
    npc_brother_conway(Creature* creature) : ScriptedAI(creature) { }

    bool talk0, talk1, talk2;

    void Reset() override
    {
        ScriptedAI::Reset();
        talk0 = false;
        talk1 = false;
        talk2 = false;
    }

    void MoveInLineOfSight(Unit* unit) override
    {
        if (Player* player = unit->ToPlayer())
            if (player->GetDistance(me) < 10.0f)
                if (!talk0)
                {
                    talk0 = true;
                    Talk(0);
                }
    }

    void DamageTaken(Unit* attacker, uint32& /*damage*/, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/) override
    {
        if (!talk1) {
            Talk(1);
            talk1 = true;
        }

        if (Player* player = attacker->ToPlayer())
            if (me->GetHealth() < me->CountPctFromMaxHealth(75)) // the intent is not to kill but to sparr
            {
                if (!talk2) {
                    Talk(2);
                    talk2 = true;
                }
            }
    }

    void JustDied(Unit* killer) override
    {
        Talk(3);
        if (Player* player = killer->ToPlayer())
            if (player->HasQuest(QUEST_THE_YONG_LORD_STORMSONG))
            {
                if (Creature* brannon = player->FindNearestCreature(NPC_BRANNON_STORMSONG, 100.0f))
                {
                    brannon->SetStandState(UNIT_STAND_STATE_STAND);
                    brannon->GetMotionMaster()->MoveFollow(player, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
                    brannon->AI()->Talk(0);
                    brannon->GetScheduler().Schedule(Milliseconds(10000), [](TaskContext context)
                        {
                            GetContextCreature()->AI()->Talk(1);
                        });
                }
            }
    }
};

//126308, Keegan Alby
class npc_keegan_alby : public ScriptedAI
{
public:
    npc_keegan_alby(Creature* creature) : ScriptedAI(creature) { }

    bool talk;

    void Reset() override
    {
        ScriptedAI::Reset();
        talk = false;
    }

    void MoveInLineOfSight(Unit* unit) override
    {
        if (Player* player = unit->ToPlayer())
            if (player->GetQuestStatus(QUEST_THE_YONG_LORD_STORMSONG) == QUEST_STATUS_INCOMPLETE)
                if (player->GetDistance(me) < 30.0f)
                {
                    if (!talk)
                    {
                        Talk(0);
                        talk = true;
                        if (Creature* brother = player->FindNearestCreature(NPC_BROTHER_THEROLD, 100.0f))
                        {
                            brother->GetScheduler().Schedule(Milliseconds(1000), [](TaskContext context)
                                {
                                    GetContextCreature()->AI()->Talk(5);
                                });
                            brother->GetScheduler().Schedule(Milliseconds(15000), [](TaskContext context)
                                {
                                    GetContextCreature()->AI()->Talk(6);
                                });
                        }
                        if (Creature* brannon = player->FindNearestCreature(NPC_BRANNON_STORMSONG, 100.0f))
                        {
                            brannon->GetScheduler().Schedule(Milliseconds(6000), [](TaskContext context)
                                {
                                    GetContextCreature()->AI()->Talk(2);
                                });
                            brannon->GetScheduler().Schedule(Milliseconds(20000), [](TaskContext context)
                                {
                                    GetContextCreature()->DespawnOrUnsummon();
                                });
                        }
                        me->GetScheduler().Schedule(Milliseconds(120000), [](TaskContext context)
                            {
                                GetContextCreature()->AI()->Reset();
                            });
                    }
                }
    }
};

//126431, Escape Rowboat
struct npc_escape_rowboat : public VehicleAI
{
    npc_escape_rowboat(Creature* creature) : VehicleAI(creature) { }

    enum MyEnums
    {
        QUEST_PADDLE_TO_SAFETY = 48366,
        NPC_ESCAPE_ROWBOAT = 126437,
        ISLAND_REACHED_KILL_CREDIT = 131348,
        EVENT_START_PADDLE = 1,
        WAYPOINT_ID_ROWBOAT = 12643101
    };

    ObjectGuid  m_playerGUID;
    EventMap    m_events;

    void Reset() override
    {
        m_events.Reset();
        m_playerGUID = ObjectGuid::Empty;
    }

    void GetPlayersForQuest()
    {
        Map::PlayerList const& playerList = me->GetMap()->GetPlayers();
        if (!playerList.isEmpty())
            for (Map::PlayerList::const_iterator i = playerList.begin(); i != playerList.end(); ++i)
                if (Player* player = i->GetSource())
                    if (player->GetDistance2d(me) < 20.0f)
                        m_playerGUID = player->GetGUID();
    }

    void IsSummonedBy(WorldObject* summoner) override
    {
        if (Player* player = summoner->ToPlayer())
        {
            if (player->GetQuestStatus(QUEST_PADDLE_TO_SAFETY) == QUEST_STATUS_INCOMPLETE)
            {
                me->GetVehicleKit();
                player->EnterVehicle(me);
                player->KilledMonsterCredit(NPC_ESCAPE_ROWBOAT);
                m_events.ScheduleEvent(EVENT_START_PADDLE, 1s);
            }
        }
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        if (type == WAYPOINT_MOTION_TYPE && id == 3)
        {
            GetPlayersForQuest();
            if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                player->KilledMonsterCredit(ISLAND_REACHED_KILL_CREDIT);
            me->DespawnOrUnsummon(100ms);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        m_events.Update(diff);

        while (uint32 eventId = m_events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_START_PADDLE:
                me->GetMotionMaster()->MovePath(WAYPOINT_ID_ROWBOAT, false);
                break;
            default:
                break;
            }
        }
    }
};

enum OkriEnums
{
    QUEST_THE_DEADLIEST_CATCH = 49302,
    NPC_PLUMERIA = 129427,
    NPC_GRYPHON_RIDE_KILL_CREDIT = 129456,
    EVENT_START_FLY_PLUMERIA = 1,
    WAYPOINT_ID_PLUMERIA = 12942701
};

// 128680, Okri Putterwrench
class npc_okri_putterwrench : public ScriptedAI
{
public:
    npc_okri_putterwrench(Creature* creature) : ScriptedAI(creature) { }

    bool OnGossipSelect(Player* player, uint32 /*menuId*/, uint32 /*gossipListId*/) override
    {
        if (player->GetQuestStatus(QUEST_THE_DEADLIEST_CATCH) == QUEST_STATUS_INCOMPLETE)
        {
            Talk(0);
            player->KilledMonsterCredit(NPC_GRYPHON_RIDE_KILL_CREDIT);
            player->SummonCreature(NPC_PLUMERIA, player->GetPosition(), TEMPSUMMON_MANUAL_DESPAWN);
            CloseGossipMenuFor(player);
        }

        return true;
    }
};

//129427, Plumeria
struct npc_plumeria : public VehicleAI
{
    npc_plumeria(Creature* creature) : VehicleAI(creature) { }

    ObjectGuid  m_playerGUID;
    EventMap    m_events;

    void Reset() override
    {
        m_events.Reset();
        m_playerGUID = ObjectGuid::Empty;
    }

    void GetPlayersForQuest()
    {
        Map::PlayerList const& playerList = me->GetMap()->GetPlayers();
        if (!playerList.isEmpty())
            for (Map::PlayerList::const_iterator i = playerList.begin(); i != playerList.end(); ++i)
                if (Player* player = i->GetSource())
                    if (player->GetDistance2d(me) < 20.0f)
                        m_playerGUID = player->GetGUID();
    }

    void IsSummonedBy(WorldObject* summoner) override
    {
        if (Player* player = summoner->ToPlayer())
        {
            if (player->GetQuestStatus(QUEST_THE_DEADLIEST_CATCH) == QUEST_STATUS_INCOMPLETE)
            {
                me->GetVehicleKit();
                player->EnterVehicle(me);
                m_events.ScheduleEvent(EVENT_START_FLY_PLUMERIA, 1s);
            }
        }
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        if (type == WAYPOINT_MOTION_TYPE && id == 4)
        {
            GetPlayersForQuest();
            if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                player->ForceCompleteQuest(QUEST_THE_DEADLIEST_CATCH);
            me->DespawnOrUnsummon(100ms);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        m_events.Update(diff);

        while (uint32 eventId = m_events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_START_FLY_PLUMERIA:
                me->GetMotionMaster()->MovePath(WAYPOINT_ID_PLUMERIA, false);
                break;
            default:
                break;
            }
        }
    }
};

struct go_humming_ice_277262 : public GameObjectAI
{
    go_humming_ice_277262(GameObject* go) : GameObjectAI(go) { }

    bool OnReportUse(Player* /*player*/)
    {
        me->SummonCreature(128062, me->GetPosition(), TEMPSUMMON_MANUAL_DESPAWN);
        me->SummonCreature(128062, me->GetPosition(), TEMPSUMMON_MANUAL_DESPAWN);
        me->SummonCreature(128062, me->GetPosition(), TEMPSUMMON_MANUAL_DESPAWN);
        return true;
    }
};

void AddSC_zone_tiragarde_sound()
{
    RegisterCreatureAI(npc_jaina_boralus_intro);
    RegisterSceneScript(scene_jaina_to_proudmoore_keep);
    RegisterSpellScript(aura_tol_dagor_intro_prisoner);
    RegisterQuestScript(quest_out_like_flynn);
    RegisterCreatureAI(npc_flynn_fairwind);
    RegisterGameObjectAI(go_toldagor_cell_block_lever);
    RegisterSceneScript(scene_flynn_jailbreak);
    RegisterCreatureAI(npc_flynn_fairwind_follower);
    RegisterCreatureAI(npc_tol_dagor_enter_sewer_credit);
    RegisterCreatureAI(npc_tol_dagor_getaway_boat);
    RegisterConversationScript(conversation_tol_dagor_inmate);
    RegisterConversationScript(conversation_tol_dagor_escape);
    RegisterSceneScript(scene_tol_dagor_getaway_boat);
    RegisterConversationScript(conversation_boralus_get_your_bearings);
    RegisterCreatureAI(npc_taelia_get_your_bearings);
    RegisterSceneScript(scene_boralus_old_knight);
    RegisterCreatureAI(npc_cyrus_crestfall);
    RegisterConversationScript(conversation_cyrus_story);
    RegisterCreatureAI(npc_boralus_portal_maga);
    RegisterCreatureAI(npc_taelia_harbormaster);
    RegisterCreatureAI(npc_boralus_adventure_map);
    RegisterCreatureAI(npc_flynn_allured);
    RegisterCreatureAI(npc_lugeia);
    RegisterCreatureAI(npc_flynn_lovesick_escort);
    RegisterCreatureAI(npc_hilde_firebreaker_queststarter);
    RegisterCreatureAI(npc_hilde_firebreaker_protect);
    RegisterCreatureAI(npc_penny_hardwick);
    RegisterCreatureAI(npc_penny_hardwick_escort);
    RegisterCreatureAI(npc_riding_macaw);
    RegisterCreatureAI(npc_riding_macaw_patrol);
    RegisterCreatureAI(npc_ralston_karn);
    RegisterPlayerScript(boralus_harbor);
    RegisterPlayerScript(old_knight_check);
    RegisterCreatureAI(npc_katherine_proudmoore_121144);
    RegisterCreatureAI(npc_grand_admiral_jes_tereth_135681);
    RegisterCreatureAI(npc_injured_marine);
    RegisterSpellScript(spell_releasing);
    RegisterCreatureAI(npc_stoat_den);
    RegisterSpellScript(spell_shoot_bola);
    RegisterCreatureAI(npc_be_our_guest_quest);
    RegisterCreatureAI(npc_cooper_main);
    RegisterCreatureAI(npc_cooper_mount);
    RegisterCreatureAI(npc_lord_aldarius_norwington);
    RegisterCreatureAI(npc_rose_main);
    RegisterCreatureAI(npc_rose_mount);
    RegisterPlayerScript(kultiras_phase_manager);
    RegisterSpellScript(spell_battlehorn_of_the_mountain);
    RegisterSpellScript(spell_shaving);
    RegisterCreatureAI(npc_greatfeather);
    RegisterSpellScript(spell_tether_shot);
    RegisterCreatureAI(npc_cagney);
    RegisterCreatureAI(npc_beachcomber_bob);
    RegisterCreatureAI(npc_my_favorite_things_quest);
    RegisterCreatureAI(npc_flynn_fairwind_126620);
    RegisterCreatureAI(npc_flynn_fairwind_130081);
    RegisterGameObjectAI(go_irontide_recruiting_poster);
    RegisterCreatureAI(npc_galeheart);
    RegisterCreatureAI(npc_vigil_hill_cannon);
    RegisterCreatureAI(npc_meredith);
    RegisterCreatureAI(npc_scratchy);
    RegisterItemScript(item_proudmoore_battle_standard);
    RegisterCreatureAI(npc_taelia_142393);
    RegisterCreatureAI(npc_galeheart_124727);
    RegisterCreatureAI(npc_galeheart_142407);
    RegisterCreatureAI(npc_proudmoore_guard);
    RegisterCreatureAI(npc_proudmoore_charger);
    RegisterCreatureAI(npc_taelia_135259);
    RegisterCreatureAI(npc_nauseated_villager);
    RegisterCreatureAI(npc_barrel_of_fish);
    RegisterCreatureAI(npc_anglepoint_fishpacker);
    RegisterCreatureAI(npc_parasitic_mindstealer);
    RegisterCreatureAI(npc_brother_conway);
    RegisterCreatureAI(npc_keegan_alby);
    RegisterCreatureAI(npc_escape_rowboat);
    RegisterCreatureAI(npc_okri_putterwrench);
    RegisterCreatureAI(npc_plumeria);
    RegisterGameObjectAI(go_humming_ice_277262);
}
