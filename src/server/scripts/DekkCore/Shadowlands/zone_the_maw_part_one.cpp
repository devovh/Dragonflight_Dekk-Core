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
    QUEST_THROUGH_SHATTERED_SKY = 59751,
    QUEST_A_FRACTURED_BLADE = 59752,
    QUEST_MAWSWORN_MENACE = 59907,
    QUEST_RUINERS_END = 59753,
    QUEST_FEAR_TO_TREAD = 59914,
    QUEST_ON_BLACKENED_WINGS = 59754,
    QUEST_A_FLIGHT_FROM_DARKNESS = 59755,
    QUEST_A_MOMENTS_RESPITE = 59756,
    QUEST_FIELD_SEANCE = 59757,
    QUEST_SPEAKING_TO_THE_DEAD = 59758,
    QUEST_SOUL_IN_HAND = 59915,
    QUEST_THE_LIONS_CAGE = 59759,
    QUEST_THE_AFFLICTORS_KEY = 59760,
    QUEST_AN_UNDESERVED_FATE = 59761,
    QUEST_FROM_THE_MOUTH_OF_MADNESS = 59776,
    QUEST_BY_AND_DOWN_THE_RIVER = 59762,
    QUEST_WOUNDS_BEYOND_FLESH = 59765,
    QUEST_A_GOOD_AXE = 59766,
    QUEST_DRAW_OUT_THE_DARKNESS = 60644,
    QUEST_THE_PATH_TO_SALVATION = 59767,
    QUEST_STAND_AS_ONE = 59770,

    QUEST_LINK_TO_THE_MAW             = 63661,
    QUEST_UNTANGLING_THE_SIGIL        = 63726,
    QUEST_THE_PRIMUS_RETURNS          = 63727,

    NPC_HIGHLORD_DARION               = 165918,
    NPC_SUMMONED_HIGHLORD_DARION      = 166886,

    SPELL_SUMMON_DARION               = 325165,
    SPELL_DOMINATING                  = 345180,
    SPELL_SHOCKWAVE                   = 342875,
    SPELL_FROSTBOLT                   = 279564,
    SPELL_MASS_INVISIBILITY_TRIGGER   = 327872,
    SPELL_MASS_INVISIBILITY           = 327876,
    SPELL_MAW_JUMP_TO_COCYRUS         = 319721,
    SPELL_WARRIOR_SKYJUMPED           = 247860,
    SPELL_SKYJUMP                     = 319741,

    SCENE_THROUGH_SHATTERED_SKY       = 329462,   //package 2643_3099
    SCENE_ON_BLACKENED_WINGS          = 309769,   //package 2356_3092
    SCENE_THE_LIONS_CAGE              = 331392,   //package 2648_3105
    SCENE_BY_AND_DOWN_THE_RIVER       = 327206,   //package 2646_3019
    SCENE_STAND_AS_ONE                = 328173,   //package 2647_2993
    SCENE_RUNECARVER_RETURNS          = 352877,   //package 2755_3259

    AREA_CRUCIBLE_OF_THE_DAMNED       = 13350,  

    ENCOUNTER_EBLADE_NUMBER           = 6,
    ENCOUNTER_EBLADE_NUMBER2          = 3,
};

enum TheMawDailog
{
    //Fear to Tread
    EVENT_JAINA_CAST0 = 3,
    EVENT_JAINA_CAST1 = 4,
    EVENT_JAINA_CAST2 = 5,
    EVENT_JAINA_CAST3 = 6,
    EVENT_JAINA_CAST4 = 7,
    EVENT_JAINA_CAST5 = 8,
    EVENT_JAINA_CAST6 = 9,

    //On Blackened Wings
    EVENT_JAINA59754_PHASE0 = 11,
    EVENT_JAINA59754_PHASE1 = 12,
    EVENT_JAINA59754_PHASE2 = 13,
    EVENT_JAINA59754_PHASE3 = 14,
    EVENT_JAINA59754_PHASE4 = 15,
    EVENT_JAINA59754_PHASE5 = 16,
    EVENT_JAINA59754_PHASE6 = 17,
    EVENT_JAINA59754_PHASE7 = 18,
    EVENT_JAINA59754_PHASE8 = 19,

    //From the Mouths of Madness
    EVENT_MADNESS_PHASE1 = 60,
    EVENT_MADNESS_PHASE2 = 61,
    EVENT_MADNESS_PHASE3 = 62,
    EVENT_MADNESS_PHASE4 = 63,
    EVENT_MADNESS_PHASE5 = 64,
    EVENT_MADNESS_PHASE6 = 65,

    //The Path to Salvation
    EVENT_SALVATION_PHASE1 = 70,  
    EVENT_SALVATION_PHASE2 = 71,
    EVENT_SALVATION_PHASE3 = 73,
};

enum AFlightfromDarkness
{
    EVENT_FLIGHT_EVENT1 = 21,
    EVENT_FLIGHT_EVENT2 = 22,
    EVENT_FLIGHT_EVENT3 = 23,
    EVENT_FLIGHT_EVENT4 = 24,
    EVENT_FLIGHT_EVENT5 = 25,
    EVENT_FLIGHT_EVENT6 = 26,
    EVENT_FLIGHT_EVENT7 = 27,
    EVENT_FLIGHT_EVENT8 = 28,
    EVENT_FLIGHT_EVENT9 = 29,
    EVENT_FLIGHT_EVENT10 = 30,
    EVENT_FLIGHT_EVENT11 = 31,
    EVENT_FLIGHT_EVENT12 = 32,
    EVENT_FLIGHT_EVENT13 = 33,
    EVENT_FLIGHT_EVENT14 = 34,
    EVENT_FLIGHT_EVENT15 = 35,
    EVENT_FLIGHT_EVENT16 = 36,
    EVENT_FLIGHT_EVENT17 = 37,
    EVENT_FLIGHT_EVENT18 = 38,
    EVENT_FLIGHT_EVENT19 = 39,
};

enum AMomentsRespite
{
    EVENT_MOMENT_EVENT1 = 41,
    EVENT_MOMENT_EVENT2 = 42,
    EVENT_MOMENT_EVENT3 = 43,
    EVENT_MOMENT_EVENT4 = 44,
    EVENT_MOMENT_EVENT5 = 45,
    EVENT_MOMENT_EVENT6 = 46,
    EVENT_MOMENT_EVENT7 = 47,
    EVENT_MOMENT_EVENT8 = 48,
    EVENT_MOMENT_EVENT9 = 49,
    EVENT_MOMENT_EVENT10 = 50,
    EVENT_MOMENT_EVENT11 = 51,
    EVENT_MOMENT_EVENT12 = 52,
    EVENT_MOMENT_EVENT13 = 53,
    EVENT_MOMENT_EVENT14 = 54,
    EVENT_MOMENT_EVENT15 = 55,
    EVENT_MOMENT_EVENT16 = 56,
    EVENT_MOMENT_EVENT17 = 57,
    EVENT_MOMENT_EVENT18 = 58,
};

class player_the_maw : public PlayerScript
{
public:
    player_the_maw() : PlayerScript("player_the_maw"){}

private:
    bool JumpIntoMaw;

    void OnUpdate(Player* player, uint32 diff) override
    {
        if (!player->HasQuest(QUEST_UNTANGLING_THE_SIGIL) || player->GetLevel() != 60)
            return;

        JumpIntoMaw = false;

        if (player->IsFalling() && player->GetPositionZ() >= 4878.83f && player->GetAreaId() == 13437 && !JumpIntoMaw)
        {
            JumpIntoMaw = true;
            player->CastSpell(player, SPELL_SKYJUMP,true);
        }
    }
  
    void OnLogout(Player* player) override
    {
       if (player->GetQuestStatus(QUEST_THE_AFFLICTORS_KEY) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(QUEST_AN_UNDESERVED_FATE) == QUEST_STATUS_INCOMPLETE)
        {
            if (Creature* thrall = GetClosestCreatureWithEntry(player, 167176, 20.0f))
                thrall->ForcedDespawn(3000);
            if (Creature* jaina = GetClosestCreatureWithEntry(player, 167154, 20.0f))
                jaina->ForcedDespawn(3000);
        }

       if (player->HasQuest(QUEST_WOUNDS_BEYOND_FLESH) || player->HasQuest(QUEST_A_GOOD_AXE))
       {
           if (Creature* thrall = GetClosestCreatureWithEntry(player, 166981, 20.0f))
               thrall->ForcedDespawn(3000);
       }
    }

    void OnSceneComplete(Player* player, uint32 sceneInstanceID) override
    {
        if (player->GetSceneMgr().GetSceneTemplateFromInstanceId(sceneInstanceID)->SceneId == 2648)
            if (player->GetQuestObjectiveProgress(QUEST_THE_LIONS_CAGE, 1) && player->GetQuestObjectiveProgress(QUEST_THE_LIONS_CAGE, 0) && player->GetQuestObjectiveProgress(QUEST_THE_LIONS_CAGE, 2))
            {
                if (!player->GetPhaseShift().HasPhase(10040)) PhasingHandler::AddPhase(player, 10040, true);
                if (!player->GetPhaseShift().HasPhase(10041)) PhasingHandler::AddPhase(player, 10041, true);
                if (!player->GetPhaseShift().HasPhase(10044)) PhasingHandler::AddPhase(player, 10044, true);
            }
        if (player->GetSceneMgr().GetSceneTemplateFromInstanceId(sceneInstanceID)->SceneId == 2646)
            if (player->GetQuestStatus(QUEST_BY_AND_DOWN_THE_RIVER) == QUEST_STATUS_COMPLETE)
            {
                if (Creature* thrall = GetClosestCreatureWithEntry(player, 166981, 20.0f))
                {
                    thrall->SetStandState(UNIT_STAND_STATE_KNEEL);
                    thrall->NearTeleportTo(4524.15f, 7445.41f, 4794.17f, 2.36874f, false);
                }
                if (Creature* jaina = GetClosestCreatureWithEntry(player, 166980, 20.0f))
                {
                    jaina->SetAIAnimKitId(14323);  //sit anim
                    jaina->NearTeleportTo(4519.52f, 7446.75f, 4794.29f, 0.165895f, false);
                }
                if (Creature* anduin = GetClosestCreatureWithEntry(player, 167833, 20.0f))
                {
                    anduin->SetAIAnimKitId(14323);  //sit anim
                    anduin->NearTeleportTo(4524.53f, 7446.78f, 4794.29f, 3.15694f, false);
                }

                player->NearTeleportTo(4519.92f, 7443.71f, 4793.88f, 0.6461f, false);
                player->GetScheduler().Schedule(2s, [this,  player](TaskContext context)
                    {
                        if (!player->GetPhaseShift().HasPhase(10040)) PhasingHandler::AddPhase(player, 10040, true);
                        if (!player->GetPhaseShift().HasPhase(10044)) PhasingHandler::AddPhase(player, 10044, true);
                    });
            }
        if (player->GetSceneMgr().GetSceneTemplateFromInstanceId(sceneInstanceID)->SceneId == 2647)
            if (player->GetQuestStatus(QUEST_STAND_AS_ONE) == QUEST_STATUS_REWARDED)
            {
                player->TeleportTo(2222, -1834.23f, 1542.29f, 5274.15f, 4.36339f);
                if (!player->GetPhaseShift().HasPhase(10060)) PhasingHandler::AddPhase(player, 10060, true);
                if (!player->GetPhaseShift().HasPhase(10061)) PhasingHandler::AddPhase(player, 10061, true);
            }
    }

    void OnUpdateZone(Player* player, uint32 newzone, uint32 /*oldZone*/) override
    {
        if (player->GetZoneId() == 13332)  //The Maw
        {
            if (player->HasQuest(QUEST_A_FRACTURED_BLADE) || player->HasQuest(QUEST_MAWSWORN_MENACE) || player->HasQuest(QUEST_RUINERS_END)
               || player->HasQuest(QUEST_FIELD_SEANCE) || player->HasQuest(QUEST_SPEAKING_TO_THE_DEAD))
            {
                if (!player->FindNearestCreature(NPC_SUMMONED_HIGHLORD_DARION, 100.f, true))
                    player->CastSpell(player, SPELL_SUMMON_DARION, false);
            }
        }
    }

    void OnUpdateArea(Player* player, uint32 newArea, uint32 /*oldArea*/) override
    {
        if (player->GetAreaId() == AREA_CRUCIBLE_OF_THE_DAMNED)
            if (player->HasQuest(QUEST_THROUGH_SHATTERED_SKY))
            {
                player->AddDelayedEvent(2000, [this, player]() -> void
                    {
                        player->CastSpell(player, SCENE_THROUGH_SHATTERED_SKY, true);
                    });
            }

        if (player->GetAreaId() == 13352)
        {
            if (player->GetQuestObjectiveProgress(QUEST_FEAR_TO_TREAD, 7))
            {
                if (!player->GetPhaseShift().HasPhase(10029)) PhasingHandler::AddPhase(player, 10029, true);
            }

            if (player->GetQuestStatus(QUEST_FEAR_TO_TREAD) == QUEST_STATUS_REWARDED)
            {
                if (player->GetPhaseShift().HasPhase(10029)) PhasingHandler::RemovePhase(player, 10029, true);
                if (!player->GetPhaseShift().HasPhase(10030)) PhasingHandler::AddPhase(player, 10030, true);
            }
            if (player->GetQuestObjectiveProgress(QUEST_ON_BLACKENED_WINGS, 0))
            {
                if (player->GetPhaseShift().HasPhase(10029)) PhasingHandler::RemovePhase(player, 10029, true);
                if (player->GetPhaseShift().HasPhase(10030)) PhasingHandler::RemovePhase(player, 10030, true);
                if (!player->GetPhaseShift().HasPhase(10031)) PhasingHandler::AddPhase(player, 10031, true);
            }

            if (player->GetQuestStatus(QUEST_ON_BLACKENED_WINGS) == QUEST_STATUS_REWARDED)
            {
                if (player->GetPhaseShift().HasPhase(10029)) PhasingHandler::RemovePhase(player, 10029, true);
                if (player->GetPhaseShift().HasPhase(10030)) PhasingHandler::RemovePhase(player, 10030, true);
                if (player->GetPhaseShift().HasPhase(10031)) PhasingHandler::RemovePhase(player, 10031, true);
            }
        }

        if (player->GetAreaId() == 13344)
        {
            if (player->GetQuestStatus(QUEST_ON_BLACKENED_WINGS) == QUEST_STATUS_REWARDED)
            {
                if (!player->GetPhaseShift().HasPhase(10040)) PhasingHandler::AddPhase(player, 10040, true);
            }
            if (player->GetQuestStatus(QUEST_A_FLIGHT_FROM_DARKNESS) == QUEST_STATUS_REWARDED)
            {
                if (!player->GetPhaseShift().HasPhase(10040)) PhasingHandler::AddPhase(player, 10040, true);
                if (!player->GetPhaseShift().HasPhase(10041)) PhasingHandler::AddPhase(player, 10041, true);
            }
            if (player->GetQuestStatus(QUEST_SOUL_IN_HAND) == QUEST_STATUS_COMPLETE)
            {
                if (player->GetPhaseShift().HasPhase(10041)) PhasingHandler::RemovePhase(player, 10041, true);
                if (!player->GetPhaseShift().HasPhase(10040)) PhasingHandler::AddPhase(player, 10040, true);
                if (!player->GetPhaseShift().HasPhase(10042)) PhasingHandler::AddPhase(player, 10042, true);
            }
            if (player->GetQuestStatus(QUEST_SOUL_IN_HAND) == QUEST_STATUS_REWARDED)
            {
                if (player->GetPhaseShift().HasPhase(10041)) PhasingHandler::RemovePhase(player, 10041, true);
                if (player->GetPhaseShift().HasPhase(10042)) PhasingHandler::RemovePhase(player, 10042, true);
                if (!player->GetPhaseShift().HasPhase(10040)) PhasingHandler::AddPhase(player, 10040, true);
                if (!player->GetPhaseShift().HasPhase(10043)) PhasingHandler::AddPhase(player, 10043, true);
            }
            if (player->GetQuestObjectiveProgress(QUEST_FROM_THE_MOUTH_OF_MADNESS, 1))
            {
                if (player->GetPhaseShift().HasPhase(10043)) PhasingHandler::RemovePhase(player, 10043, true);
                if (!player->GetPhaseShift().HasPhase(10040)) PhasingHandler::AddPhase(player, 10040, true);
                if (!player->GetPhaseShift().HasPhase(10044)) PhasingHandler::AddPhase(player, 10044, true);
                if (!player->GetPhaseShift().HasPhase(10045)) PhasingHandler::AddPhase(player, 10045, true);
            }
            if (player->GetQuestStatus(QUEST_FROM_THE_MOUTH_OF_MADNESS) == QUEST_STATUS_REWARDED)
            {
                if (player->GetPhaseShift().HasPhase(10043)) PhasingHandler::RemovePhase(player, 10043, true);
                if (!player->GetPhaseShift().HasPhase(10040)) PhasingHandler::AddPhase(player, 10040, true);
                if (!player->GetPhaseShift().HasPhase(10046)) PhasingHandler::AddPhase(player, 10046, true);
            }
        }

        if (player->GetAreaId() == 13357)
        {
            if (!player->GetPhaseShift().HasPhase(10040)) PhasingHandler::AddPhase(player, 10040, true);
        }

        if (player->GetAreaId() == 13356)
        {

            if (!player->GetPhaseShift().HasPhase(10040)) PhasingHandler::AddPhase(player, 10040, true);
            if (!player->GetPhaseShift().HasPhase(10043)) PhasingHandler::AddPhase(player, 10043, true);

        }

        if (player->GetAreaId() == 13355)
        {
            if (player->GetQuestStatus(QUEST_THE_LIONS_CAGE) == QUEST_STATUS_INCOMPLETE)
            {
                if (!player->GetPhaseShift().HasPhase(10040)) PhasingHandler::AddPhase(player, 10040, true);
                if (!player->GetPhaseShift().HasPhase(10041)) PhasingHandler::AddPhase(player, 10041, true);
                if (!player->GetPhaseShift().HasPhase(10043)) PhasingHandler::AddPhase(player, 10043, true);
            }
            if (player->GetQuestStatus(QUEST_THE_LIONS_CAGE) == QUEST_STATUS_COMPLETE ||player->GetQuestStatus(QUEST_THE_LIONS_CAGE) == QUEST_STATUS_REWARDED)
            {
                if (!player->GetPhaseShift().HasPhase(10040)) PhasingHandler::AddPhase(player, 10040, true);
                if (!player->GetPhaseShift().HasPhase(10041)) PhasingHandler::AddPhase(player, 10041, true);
                if (!player->GetPhaseShift().HasPhase(10044)) PhasingHandler::AddPhase(player, 10044, true);
                if (player->GetPhaseShift().HasPhase(10043)) PhasingHandler::RemovePhase(player, 10043, true);
            }
            if (player->GetQuestStatus(QUEST_THE_AFFLICTORS_KEY) == QUEST_STATUS_COMPLETE || player->GetQuestStatus(QUEST_AN_UNDESERVED_FATE) == QUEST_STATUS_COMPLETE)
            {
                if (!player->GetPhaseShift().HasPhase(10040)) PhasingHandler::AddPhase(player, 10040, true);
                if (!player->GetPhaseShift().HasPhase(10041)) PhasingHandler::AddPhase(player, 10041, true);
                if (player->GetPhaseShift().HasPhase(10044)) PhasingHandler::RemovePhase(player, 10044, true);
                if (player->GetPhaseShift().HasPhase(10043)) PhasingHandler::RemovePhase(player, 10043, true);
            }
        }

        if (player->GetAreaId() == 13354)
        {
            if (!player->GetPhaseShift().HasPhase(10040)) PhasingHandler::AddPhase(player, 10040, true);


            if (player->GetQuestStatus(QUEST_STAND_AS_ONE) == QUEST_STATUS_INCOMPLETE)
            {
                if (!player->GetPhaseShift().HasPhase(10040)) PhasingHandler::AddPhase(player, 10040, true);
            }
        }

        if (player->GetAreaId() == 13332)
        {
            if (!player->GetPhaseShift().HasPhase(10040)) PhasingHandler::AddPhase(player, 10040, true);
        }

        if (player->GetAreaId() == 13351)
        {
            if (player->GetQuestStatus(QUEST_BY_AND_DOWN_THE_RIVER) == QUEST_STATUS_INCOMPLETE)
            {
                if (!player->GetPhaseShift().HasPhase(10040)) PhasingHandler::AddPhase(player, 10040, true);
            }
        }

        if (player->GetAreaId() == 13405)
        {
            if (player->GetQuestObjectiveProgress(QUEST_BY_AND_DOWN_THE_RIVER, 0) && player->GetQuestObjectiveProgress(QUEST_BY_AND_DOWN_THE_RIVER, 2))
            {
                if (!player->GetPhaseShift().HasPhase(10040)) PhasingHandler::AddPhase(player, 10040, true);
                if (!player->GetPhaseShift().HasPhase(10044)) PhasingHandler::AddPhase(player, 10044, true);
            }

            if (player->GetQuestStatus(QUEST_BY_AND_DOWN_THE_RIVER) == QUEST_STATUS_REWARDED)
            {
                if (player->GetPhaseShift().HasPhase(10044)) PhasingHandler::RemovePhase(player, 10044, true);
                if (!player->GetPhaseShift().HasPhase(10041)) PhasingHandler::AddPhase(player, 10041, true);
                if (!player->GetPhaseShift().HasPhase(10040)) PhasingHandler::AddPhase(player, 10040, true);
            }

            if (player->GetQuestStatus(QUEST_DRAW_OUT_THE_DARKNESS) == QUEST_STATUS_INCOMPLETE)
            {
                if (player->GetPhaseShift().HasPhase(10041)) PhasingHandler::RemovePhase(player, 10041, true);
                if (!player->GetPhaseShift().HasPhase(10042)) PhasingHandler::AddPhase(player, 10042, true);
                if (!player->GetPhaseShift().HasPhase(10040)) PhasingHandler::AddPhase(player, 10040, true);
            }

            if (player->GetQuestStatus(QUEST_DRAW_OUT_THE_DARKNESS) == QUEST_STATUS_REWARDED)
            {
                if (player->GetPhaseShift().HasPhase(10041)) PhasingHandler::RemovePhase(player, 10041, true);
                if (player->GetPhaseShift().HasPhase(10042)) PhasingHandler::RemovePhase(player, 10042, true);
                if (!player->GetPhaseShift().HasPhase(10043)) PhasingHandler::AddPhase(player, 10043, true);
                if (!player->GetPhaseShift().HasPhase(10040)) PhasingHandler::AddPhase(player, 10040, true);
            }

        }
        if (player->GetAreaId() == 13358)
        {
            if (player->GetQuestStatus(QUEST_THE_PATH_TO_SALVATION) == QUEST_STATUS_INCOMPLETE)
            {
                if (!player->GetPhaseShift().HasPhase(10040)) PhasingHandler::AddPhase(player, 10040, true);
            }
        }

        if (player->GetAreaId() == 13437)
        {
            if (player->GetQuestStatus(63660) == QUEST_STATUS_REWARDED) //TODOO: temporary Phase
            {
                if (player->GetPhaseShift().HasPhase(10200)) PhasingHandler::RemovePhase(player, 10200, true);
            }
            if (player->GetQuestStatus(QUEST_LINK_TO_THE_MAW) == QUEST_STATUS_INCOMPLETE)
            {
                if (player->HasAura(SPELL_MAW_JUMP_TO_COCYRUS))
                {
                    player->CastSpell(player, SPELL_SKYJUMP, false);
                   player->GetScheduler().Schedule(Milliseconds(2000), [player](TaskContext context)
                        {
                            player->RemoveAura(SPELL_MAW_JUMP_TO_COCYRUS);
                            player->RemoveAura(SPELL_WARRIOR_SKYJUMPED);
                            player->KilledMonsterCredit(177717);
                        });
                }
            }
        }
        if (player->GetAreaId() == 13547)
        {
            if (player->GetQuestObjectiveProgress(QUEST_UNTANGLING_THE_SIGIL, 1) && !player->GetQuestObjectiveProgress(QUEST_UNTANGLING_THE_SIGIL, 0))
            {
                if (!player->FindNearestCreature(178774, 20.f, true))
                    if (Creature* bh = player->SummonCreature(178774, 2688.57f, 2104.67f, 317.48f, 6.05716f, TEMPSUMMON_TIMED_DESPAWN, 5min))
                    {
                        player->GetScheduler().Schedule(Milliseconds(4000), [bh](TaskContext context)
                            {
                                bh->AI()->Talk(0);
                            });
                    }
            }
            if (player->GetQuestStatus(QUEST_UNTANGLING_THE_SIGIL) == QUEST_STATUS_COMPLETE)
            {
                if (Creature* runecarver = player->FindNearestCreature(164937, 20.f, true))
                {
                    runecarver->SetDisplayId(99095);
                    if (!player->FindNearestCreature(178774, 20.f, true))
                        player->SummonCreature(178774, 2688.57f, 2104.67f, 317.48f, 6.05716f, TEMPSUMMON_TIMED_DESPAWN, 5min);
                }
            }
        }
    }
};

class scene_the_maw : public SceneScript
{
public:
    scene_the_maw() : SceneScript("scene_the_maw") { }

    void OnSceneStart(Player* player, uint32 /*sceneInstanceID*/, SceneTemplate const* sceneTemplate) override
    {
        if (sceneTemplate->ScenePackageId == 3259) //The primus returns
        {
            if (Creature* runecarver = player->FindNearestCreature(164937, 20.f, true))
                runecarver->DespawnOrUnsummon(1s, 49s);
            if (Creature* bh = player->FindNearestCreature(178774, 20.f, true))
                bh->ForcedDespawn();
        }
    }

    // Called when a scene is canceled
    void OnSceneCancel(Player* player, uint32 /*sceneInstanceID*/, SceneTemplate const* sceneTemplate) override
    {
        if (sceneTemplate->ScenePackageId == 3227)
            teleportDesmotaeron(player);
        if (sceneTemplate->ScenePackageId == 3306)
            resumeMovementVenari(player);
    }

    // Called when a scene is completed
    void OnSceneComplete(Player* player, uint32 /*sceneInstanceID*/, SceneTemplate const* sceneTemplate) override
    {
        if (sceneTemplate->ScenePackageId == 3227)
            teleportDesmotaeron(player);
        if (sceneTemplate->ScenePackageId == 3306)
            resumeMovementVenari(player);
    }

    void teleportDesmotaeron(Player* player)
    {
        player->NearTeleportTo(Position(3829.52f, 6137.94f, 4836.74f, 0.3474f), false);
        player->SummonCreature(162804, Position(3832.46f, 6135.32f, 4836.82f, 3.6107f), TEMPSUMMON_TIMED_DESPAWN, 300s);
    }

    void resumeMovementVenari(Player* player)
    {
        if (Creature* venari = player->FindNearestCreature(177819, 20.0f, true))
            venari->AI()->DoAction(1);
    }
};

//166963
struct npc_knight_of_the_ebon_blade_166963 : public ScriptedAI
{
    npc_knight_of_the_ebon_blade_166963(Creature* creature) : ScriptedAI(creature) { Reset(); }

    uint32 waitTime;
    ObjectGuid guardTarget;

    void Reset() override
    {
        guardTarget = ObjectGuid::Empty;
        waitTime = urand(0, 2000);
        me->SetSheath(SHEATH_STATE_MELEE);
        me->RemoveUnitFlag(UNIT_FLAG_IMMUNE_TO_PC);
    }

    void DamageTaken(Unit* target, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/) override
    {
        if (target->ToCreature())
            if (me->GetHealth() <= damage || me->GetHealthPct() <= 80.0f)
                damage = 0;
    }

    void DamageDealt(Unit* target, uint32& damage, DamageEffectType /*damageType*/) override
    {
        if (target->ToCreature())
            if (target->GetHealth() <= damage || target->GetHealthPct() <= 70.0f)
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

        waitTime = urand(30000, 40000);

        if (!guardTarget.IsEmpty())
        {
            if (Creature* guard = ObjectAccessor::GetCreature(*me, guardTarget))
            {
                    if (guard->IsAlive())
                    {
                        if (me->GetVictim() != guard)
                        {
                            me->GetThreatManager().AddThreat(guard, 1000000.0f);
                            guard->GetThreatManager().AddThreat(me, 1000000.0f);
                            me->Attack(guard, true);
                        }
                    }
                    else
                    {
                        guard->DespawnOrUnsummon();
                        guardTarget = ObjectGuid::Empty;
                    }
               
            }
        }
        else
        {
            Position guardPos = me->GetPosition();
            GetPositionWithDistInFront(me, 2.5f, guardPos);

            if (Creature* guard = me->SummonCreature(165860, guardPos))
            {
                me->GetThreatManager().AddThreat(guard, 1000000.0f);
                guard->GetThreatManager().AddThreat(me, 1000000.0f);
                AttackStart(guard);
                guard->SetFacingToObject(me);
                guardTarget = guard->GetGUID();
            }
        }
    }

    void OnSpellClick(Unit* clicker, bool /*result*/) override
    {
        if (Player* player = clicker->ToPlayer())
        {
            if (player->GetQuestStatus(QUEST_A_FRACTURED_BLADE) == QUEST_STATUS_INCOMPLETE)
            {
                if (Creature* darion = GetClosestCreatureWithEntry(player, NPC_SUMMONED_HIGHLORD_DARION, 20.0f))
                {
                    if (rand32() % 100 < 35)                              //50% random talk 
                    darion->AI()->Talk(2);
                }
            }
        }
    }
private:
    TaskScheduler _scheduler;
    std::unordered_set<uint32> _randomEmotes;
};

// 324812 Rallying
struct spell_rallying_324812 : public SpellScript
{
    PrepareSpellScript(spell_rallying_324812);

    bool onekillCret = false;

    void HandleDummy(SpellEffIndex /*effindex*/)
    {
        if (!onekillCret)
        {
            onekillCret = true;
            if (Unit* caster = GetCaster())
                if (Player* player = caster->ToPlayer())
                    if (Unit* target = player->GetSelectedUnit())
                        if (Creature* creTarget = target->ToCreature())
                        {
                            player->KilledMonsterCredit(166605);
                            creTarget->RemoveNpcFlag(UNIT_NPC_FLAG_SPELLCLICK);
                            creTarget->SetWalk(false);
                            creTarget->GetMotionMaster()->MoveRandom(10.f);
                            creTarget->ForcedDespawn(3000);
                        }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_rallying_324812::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

enum darionSkill
{
   EVENT_DARK_COMMAND = 3000,
   DARK_COMMAND = 327528,
};

// 166886 highlord_darion_mograine
struct npc_highlord_darion_mograine_166886 : public EscortAI
{
    npc_highlord_darion_mograine_166886(Creature* creature) : EscortAI(creature) {}

private:
    bool say59757;
    bool say59915;
    bool say59776;
    TaskScheduler _scheduler;
    EventMap _events;
    ObjectGuid m_playerGUID;
    ObjectGuid DarionGUID;
    ObjectGuid AmalgamationGUID;
    ObjectGuid jainaGUID;
    ObjectGuid EBladeGUID[ENCOUNTER_EBLADE_NUMBER];

    void Reset() override
    {
        say59757 = false;
        say59915 = false;
        say59776 = false;
        jainaGUID.Clear();
        AmalgamationGUID.Clear();
        _events.Reset();
    }


    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_SOUL_IN_HAND)
        {
            say59915 = false;
            me->AI()->Talk(34);
            if (Creature* amalgamation = me->FindNearestCreature(165976, 30.0f))
                player->GetScheduler().Schedule(3s, [this, amalgamation, player](TaskContext context)
                    {
                        me->GetMotionMaster()->MovePoint(0, 4828.89f, 7572.28f, 4798.02f, true);

                        amalgamation->GetMotionMaster()->MovePoint(0, 4828.89f, 7572.28f, 4798.02f, true);

                    }).Schedule(5s, [this, amalgamation, player](TaskContext context)
                        {
                            me->ForcedDespawn(1000);
                            amalgamation->ForcedDespawn(1000);
                        });
        }
    }

    void OnQuestReward(Player* player, Quest const* quest, LootItemType /*type*/, uint32 /*opt*/) override
    {
        if (quest->GetQuestId() == QUEST_A_FRACTURED_BLADE)
            me->AI()->Talk(3);
    }

    void IsSummonedBy(WorldObject* unit) override
    {
        if (Player* player = unit->ToPlayer())
        {
            if (player->HasQuest(QUEST_FEAR_TO_TREAD))
            {
                me->AI()->Talk(5);
                player->GetScheduler().Schedule(5s, [this](TaskContext context) { Talk(6); });
            }

            if (player->HasQuest(QUEST_FIELD_SEANCE))
                me->AI()->Talk(29);

            if (player->HasQuest(QUEST_SPEAKING_TO_THE_DEAD))
                me->AI()->Talk(31);
        }
    }

    void MoveInLineOfSight(Unit* target) override
    {
        Position _positon = target->GetPosition();

        if (Player* player = target->ToPlayer())
        {

            if (target->GetDistance2d(me) > 20.0f)
                me->UpdatePosition(_positon, false);

            if (player->IsInDist(me, 5.0f))
            {
                if ((player->GetQuestObjectiveProgress(QUEST_RUINERS_END, 0) == 1))
                {
                    player->KilledMonsterCredit(NPC_HIGHLORD_DARION);
                }
                if (!say59757)
                {
                    if ((player->GetQuestObjectiveProgress(QUEST_FIELD_SEANCE, 0) == 5))
                    {
                        me->AI()->Talk(30);
                        say59757 = true;
                    }
                }

                if (!say59915)
                {
                    if (player->GetQuestStatus(QUEST_SOUL_IN_HAND) == QUEST_STATUS_COMPLETE)
                    {
                        if (Creature* Amalgamation = player->FindNearestCreature(167690, 30.0f))
                        {
                            me->AI()->Talk(35);
                            me->CastSpell(Amalgamation, 326267);
                            Amalgamation->CastSpell(Amalgamation, 326388);
                            me->AddDelayedEvent(4000, [this, Amalgamation]()
                                {
                                    me->CastStop();
                                    Amalgamation->CastStop();
                                    Amalgamation->AI()->Talk(0);
                                });
                        }
                    }
                    say59915 = true;
                }

                if (!say59776)
                {
                    if (player->GetQuestObjectiveProgress(QUEST_FROM_THE_MOUTH_OF_MADNESS, 1) && !player->GetQuestObjectiveProgress(QUEST_FROM_THE_MOUTH_OF_MADNESS, 0))
                        if (me->GetPhaseShift().HasPhase(10045))
                        {
                            me->AI()->Talk(36);
                            me->AddDelayedEvent(4000, [this]()
                                {
                                    me->AI()->Talk(37);
                                });
                        }
                    say59776 = true;
                }
            }
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

        DoMeleeAttackIfReady();

        _events.Update(diff);

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_DARK_COMMAND:
                if (me->IsWithinCombatRange(me->GetVictim(), 10.f))
                    DoCastVictim(DARK_COMMAND);
                _events.Repeat(randtime(15s, 20s));
                break;
            }
        }
    }

    void JustEngagedWith(Unit* attacker) override                  // attack_started
    {
        _events.ScheduleEvent(EVENT_DARK_COMMAND, 5s);

        if (Creature* npc = attacker->ToCreature())
            if (npc->GetEntry() == 165976)
            {
                me->AI()->Talk(32);
                me->AddDelayedEvent(4000, [this]()
                    {
                        SpawnNPC();
                    });
            }
    }

    void SpawnNPC()
    {
        Unit* temp = nullptr;

        for (uint8 i = 0; i < ENCOUNTER_EBLADE_NUMBER2; ++i)
        {
            temp = ObjectAccessor::GetCreature(*me, EBladeGUID[i]);
            if (!temp)
            {
                temp = me->SummonCreature(165862, me->GetPosition().GetPositionWithOffset({ float(rand32() % 3), float(rand32() % 3) }), TEMPSUMMON_TIMED_DESPAWN, 180s);
                temp->SetFaction(3222);
                if (temp->IsInDist(me, 20.0f))
                {
                    if (temp->GetEntry() == 165976)
                    {
                        me->GetThreatManager().AddThreat(temp, 1000000.0f);
                        me->Attack(temp, true);
                    }
                }
                EBladeGUID[i] = temp->GetGUID();
            }
        }
    }
};


//165918
class npc_highlord_darion_mograine_165918 : public CreatureScript
{
public:
    npc_highlord_darion_mograine_165918() : CreatureScript("npc_highlord_darion_mograine_165918") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_highlord_darion_mograine_165918AI(creature);
    }
    struct npc_highlord_darion_mograine_165918AI : public EscortAI
    {
        npc_highlord_darion_mograine_165918AI(Creature* creature) : EscortAI(creature) { Reset(); }

    private:
        bool say;
        TaskScheduler _scheduler;
        ObjectGuid m_playerGUID;
        EventMap _events;


        void Reset() override
        {
            say = false;
            m_playerGUID = ObjectGuid::Empty;
            _events.Reset();
        }

        void OnQuestAccept(Player* player, Quest const* quest) override
        {
            if (quest->GetQuestId() == QUEST_A_FRACTURED_BLADE || quest->GetQuestId() == QUEST_MAWSWORN_MENACE || quest->GetQuestId() == QUEST_FEAR_TO_TREAD || quest->GetQuestId() == QUEST_FIELD_SEANCE || quest->GetQuestId() == QUEST_SPEAKING_TO_THE_DEAD)
            {
                if (!player->FindNearestCreature(NPC_SUMMONED_HIGHLORD_DARION, 50.f, true))
                {
                    player->CastSpell(player, SPELL_SUMMON_DARION, false);
                    me->DespawnOrUnsummon(1s, 60s);
                }
            }
        }

        void OnQuestReward(Player* player, Quest const* quest, LootItemType /*type*/, uint32 /*opt*/) override
        {
            if (quest->GetQuestId() == QUEST_FROM_THE_MOUTH_OF_MADNESS)
            {
                me->AI()->Talk(42);
                player->GetScheduler().Schedule(7s, [this](TaskContext context)
                    {
                        me->AI()->Talk(43);
                    });
            }
        }

        bool OnGossipHello(Player* player) override
        {
            if (me->IsQuestGiver())
            {
                player->PrepareQuestMenu(me->GetGUID());
                SendGossipMenuFor(player, player->GetGossipTextId(me), me->GetGUID());  //Gossip_menu_id must have number.
            }
            if (player->GetQuestObjectiveProgress(QUEST_FROM_THE_MOUTH_OF_MADNESS, 1))
            {
                m_playerGUID = player->GetGUID();
                AddGossipItemFor(player, GossipOptionNpc::None, " Make it talk.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                SendGossipMenuFor(player, 40523, me->GetGUID());
                player->PrepareQuestMenu(me->GetGUID());
            }

            return true;
        }

        bool OnGossipSelect(Player* player, uint32 /*MenuID*/, uint32 gossipListId) override
        {
            uint32 const action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);
            ClearGossipMenuFor(player);
            switch (action)
            {
            case GOSSIP_ACTION_INFO_DEF + 1:

                CloseGossipMenuFor(player);

                if (Creature* jaina = GetClosestCreatureWithEntry(me, 166980, 20.0f))
                {
                    if (jaina->GetPhaseShift().HasPhase(10045))
                        jaina->GetMotionMaster()->MoveTargetedHome();
                }
                if (Creature* thrall = GetClosestCreatureWithEntry(me, 166981, 20.0f))
                {
                    if (thrall->GetPhaseShift().HasPhase(10045))
                        thrall->GetMotionMaster()->MoveTargetedHome();
                }
                if (Creature* anduin = GetClosestCreatureWithEntry(me, 167833, 20.0f))
                {
                    if (anduin->GetPhaseShift().HasPhase(10045))
                        anduin->ForcedDespawn(1000, 60s);
                }
                if (player->GetPhaseShift().HasPhase(10045)) PhasingHandler::RemovePhase(player, 10045, true);
                if (!player->GetPhaseShift().HasPhase(10046)) PhasingHandler::AddPhase(player, 10046, true);

                _events.ScheduleEvent(EVENT_MADNESS_PHASE1, 1s);
                break;
            }
            return true;
        }

        void MoveInLineOfSight(Unit* target) override
        {
            if (Player* player = target->ToPlayer())
            {
                if (player->IsInDist(me, 5.0f))
                {
                    if (!say)
                    {
                        if (player->HasQuest(QUEST_THROUGH_SHATTERED_SKY))
                        {
                            me->AI()->Talk(0);
                            me->AddDelayedEvent(8000, [this]() {  me->AI()->Talk(1); });
                        }
                        say = true;
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
                case EVENT_MADNESS_PHASE1:
                    if (Creature* Amalgamation = GetClosestCreatureWithEntry(me, 167690, 30.0f))
                    {
                        me->CastSpell(Amalgamation, 326267);
                        me->AddAura(326388, Amalgamation);
                        me->AI()->Talk(38);
                    }
                    _events.ScheduleEvent(EVENT_MADNESS_PHASE2, 5s);
                    break;

                case EVENT_MADNESS_PHASE2:
                    if (Creature* Amalgamation = GetClosestCreatureWithEntry(me, 167690, 30.0f))
                    {
                        me->CastSpell(Amalgamation, 326267);
                        me->AddAura(326388, Amalgamation);
                        Amalgamation->AI()->Talk(1);
                    }
                    _events.ScheduleEvent(EVENT_MADNESS_PHASE3, 5s);
                    break;

                case EVENT_MADNESS_PHASE3:
                    if (Creature* Amalgamation = GetClosestCreatureWithEntry(me, 167690, 30.0f))
                    {
                        me->CastSpell(Amalgamation, 326267);
                        me->AI()->Talk(39);
                    }
                    _events.ScheduleEvent(EVENT_MADNESS_PHASE4, 6s);
                    break;

                case EVENT_MADNESS_PHASE4:
                    if (Creature* Amalgamation = GetClosestCreatureWithEntry(me, 167690, 30.0f))
                    {
                        Amalgamation->RemoveAura(326388);
                        Amalgamation->AI()->Talk(2);
                        me->AddDelayedEvent(5000, [this, Amalgamation]()
                            {
                                Amalgamation->AI()->Talk(3);
                            });
                        me->AddDelayedEvent(9000, [this, Amalgamation]()
                            {
                                Amalgamation->AI()->Talk(4);
                            });
                        me->AddDelayedEvent(14000, [this, Amalgamation]()
                            {
                                Amalgamation->ForcedDespawn(1000, 120s);
                            });
                    }
                    _events.ScheduleEvent(EVENT_MADNESS_PHASE5, 15s);
                    break;

                case EVENT_MADNESS_PHASE5:
                    if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                    {
                        if (Creature* jaina = GetClosestCreatureWithEntry(me, 166980, 10.0f))
                        {
                            me->AI()->Talk(40);
                            me->AddDelayedEvent(6000, [this, jaina]()
                                {
                                    jaina->AI()->Talk(38);
                                });
                            me->AddDelayedEvent(14000, [this, player]()
                                {
                                    player->KilledMonsterCredit(NPC_HIGHLORD_DARION);
                                    me->AI()->Talk(41);
                                    me->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
                                });
                        }
                    }
                    break;
                }
            }
        }

    };
};

// npc_wilona_thorne_170624
struct npc_wilona_thorne_170624 : public ScriptedAI
{
    npc_wilona_thorne_170624(Creature* creature) : ScriptedAI(creature) {}

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_RUINERS_END)
        {
            if (!player->FindNearestCreature(NPC_SUMMONED_HIGHLORD_DARION, 50.f, true))
            {
                player->CastSpell(player, SPELL_SUMMON_DARION, false);
            }
        }
    }
};

enum ruinerSkill
{
    EVENT_ACCURSED_STRENGTH    = 2000,
    EVENT_MASSIVE_STRIKE,

    ACCURSED_STRENGTH          = 294165,
    MASSIVE_STRIKE             = 292903,
};


//npc_ruiner_maroth_166174_q59753
class npc_ruiner_maroth_166174 : public CreatureScript
{
public:
    npc_ruiner_maroth_166174() : CreatureScript("npc_ruiner_maroth_166174") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_ruiner_maroth_166174AI(creature);
    }
    struct npc_ruiner_maroth_166174AI : public EscortAI
    {
        npc_ruiner_maroth_166174AI(Creature* creature) : EscortAI(creature) { Reset(); }

        Position const EbonBlase = { 4461.22f, 7859.01f, 4904.56f, 0.94046f };

        uint32 waitTime;
        ObjectGuid guardTarget;
        ObjectGuid  DarionGUID;
        ObjectGuid EBladeGUID[ENCOUNTER_EBLADE_NUMBER];
        EventMap _events;

        void Reset() override
        {
            guardTarget = ObjectGuid::Empty;
            DarionGUID = ObjectGuid::Empty;
            waitTime = urand(0, 2000);
            me->SetSheath(SHEATH_STATE_MELEE);
            me->RemoveUnitFlag(UNIT_FLAG_IMMUNE_TO_PC);
            _events.Reset();
            for (uint8 i = 0; i < ENCOUNTER_EBLADE_NUMBER; ++i)
            {
                if (Creature* temp = ObjectAccessor::GetCreature(*me, EBladeGUID[i]))
                    temp->setDeathState(JUST_DIED);
                EBladeGUID[i].Clear();
            }
        }

        void JustEngagedWith(Unit* victim) override
        {
            _events.ScheduleEvent(EVENT_ACCURSED_STRENGTH, 1s);
            _events.ScheduleEvent(EVENT_MASSIVE_STRIKE, 5s);
        }

        void DamageTaken(Unit* target, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/) override
        {
            if (target->ToCreature())
                if (me->GetHealth() <= damage || me->GetHealthPct() <= 80.0f)
                    damage = 0;
        }

        void DamageDealt(Unit* target, uint32& damage, DamageEffectType /*damageType*/) override
        {
            if (target->ToCreature())
                if (target->GetHealth() <= damage || target->GetHealthPct() <= 70.0f)
                    damage = 0;
        }

        void JustDied(Unit* killer) override
        {
            if (Player* player = killer->ToPlayer())
            {
                if (player->HasQuest(QUEST_RUINERS_END))
                {
                    me->AI()->Talk(0);
                    std::list<Creature*> cList = me->FindNearestCreatures(NPC_SUMMONED_HIGHLORD_DARION, 30.0f);
                    for (std::list<Creature*>::const_iterator itr = cList.begin(); itr != cList.end(); ++itr)
                    {
                        if (Creature* darion = *itr)
                        {
                            player->GetScheduler().Schedule(4s, [this, darion, player](TaskContext context)
                                {
                                    darion->AI()->Talk(4);
                                    darion->ForcedDespawn(10000);
                                    darion->SetWalk(false);
                                    darion->GetMotionMaster()->MovePoint(0, 4464.15f, 7836.78f, 4905.6f, true);
                                }).Schedule(8s, [this, darion, player](TaskContext context)
                                    {
                                        darion->GetMotionMaster()->MovePoint(1, 4469.36f, 7813.69f, 4903.54f, true);
                                    });
                        }
                    }

                }

                std::list<Creature*> cList = me->FindNearestCreatures(165862, 30.0f);
                for (std::list<Creature*>::const_iterator itr = cList.begin(); itr != cList.end(); ++itr)
                {
                    if (Creature* guard = *itr)
                    {
                        guard->ForcedDespawn(2000);
                    }
                }

            }
        }

        void SpawnNPC()
        {
            Unit* temp = nullptr;

            for (uint8 i = 0; i < ENCOUNTER_EBLADE_NUMBER; ++i)
            {
                temp = ObjectAccessor::GetCreature(*me, EBladeGUID[i]);
                if (!temp)
                {
                    temp = me->SummonCreature(165862, EbonBlase.GetPositionWithOffset({ float(rand32() % 15), float(rand32() % 15) }), TEMPSUMMON_MANUAL_DESPAWN, 300s);
                    temp->SetFaction(3222);
                    EBladeGUID[i] = temp->GetGUID();
                }
            }
        }

        void UpdateAI(uint32 diff) override
        {
            DoMeleeAttackIfReady();

            _events.Update(diff);
            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_ACCURSED_STRENGTH:
                    if (me->IsWithinCombatRange(me->GetVictim(), 10.f))
                        DoCastVictim(ACCURSED_STRENGTH);
                    _events.Repeat(randtime(25s, 30s));
                    break;
                case EVENT_MASSIVE_STRIKE:
                    if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0))
                        if (me->IsWithinCombatRange(target, 10.f))
                            DoCastVictim(MASSIVE_STRIKE);
                    _events.Repeat(randtime(12s, 17s));
                    break;
                }
            }

            if (waitTime && waitTime >= diff)
            {
                waitTime -= diff;
                return;
            }

            waitTime = urand(10000, 20000);

            if (!guardTarget.IsEmpty())
            {
                if (Creature* guard = ObjectAccessor::GetCreature(*me, guardTarget))
                {
                    if (guard->IsAlive())
                    {
                        if (me->GetVictim() != guard)
                        {
                            me->GetThreatManager().AddThreat(guard, 1000.0f);
                            guard->GetThreatManager().AddThreat(me, 1000.0f);
                            me->Attack(guard, true);
                        }
                    }
                    else
                    {
                        guard->DespawnOrUnsummon();
                        guardTarget = ObjectGuid::Empty;
                    }
                }
            }
            else
            {
                SpawnNPC();
                std::list<Creature*> cList = me->FindNearestCreatures(165862, 30.0f);
                for (std::list<Creature*>::const_iterator itr = cList.begin(); itr != cList.end(); ++itr)
                {
                    if (Creature* guard = *itr)
                    {
                        me->GetThreatManager().AddThreat(guard, 1000.0f);
                        guard->GetThreatManager().AddThreat(me, 1000.0f);
                        AttackStart(guard);
                        guard->SetFacingToObject(me);
                        guardTarget = guard->GetGUID();
                    }
                }
            }
        }
    };
};

//171423_Fear_to_Tread_q59914
struct npc_dnt_credit_first_clue : public ScriptedAI
{
    npc_dnt_credit_first_clue(Creature* creature) : ScriptedAI(creature) { Reset(); }

private:
    bool say;

    void Reset() override
    {
        say = false;
    }

    void MoveInLineOfSight(Unit* target) override
    {
        if (!target->IsPlayer())
            return;

        if (Player* player = target->ToPlayer())
        {
            if (player->GetDistance2d(me) <= 7.0f)
                if (!player->GetQuestObjectiveProgress(QUEST_FEAR_TO_TREAD, 5))
                    if (Creature* darion = GetClosestCreatureWithEntry(player, NPC_SUMMONED_HIGHLORD_DARION, 30.0f))
                    {
                        if (!say)
                        {
                            say = true;
                            darion->AI()->Talk(7);
                            player->GetScheduler().Schedule(6s, [this, darion, player](TaskContext context)
                                {
                                    darion->AI()->Talk(8);
                                    player->KilledMonsterCredit(171423);
                                });
                        }

                    }
            if (!player->IsWithinDistInMap(me, 30.0f) && say)
                if (player->GetQuestStatus(QUEST_FEAR_TO_TREAD) == QUEST_STATUS_INCOMPLETE)
                    say = false;
        }
    }
};

//171424_Fear_to_Tread_q59914
struct npc_dnt_credit_second_clue : public ScriptedAI
{
    npc_dnt_credit_second_clue(Creature* creature) : ScriptedAI(creature) { Reset(); }

private:
    bool say;

    void Reset() override
    {
        say = false;
    }

    void MoveInLineOfSight(Unit* target) override
    {
        if (!target->IsPlayer())
            return;

       
        if (Player* player = target->ToPlayer())
        {
            if (player->GetDistance2d(me) <= 10.0f)
                if (!player->GetQuestObjectiveProgress(QUEST_FEAR_TO_TREAD, 6))
                    if (Creature* darion = GetClosestCreatureWithEntry(player, NPC_SUMMONED_HIGHLORD_DARION, 30.0f))
                    {
                        if (!say)
                        {
                            say = true;
                            darion->AI()->Talk(9);
                            player->GetScheduler().Schedule(4s, [this, darion, player](TaskContext context)
                                {
                                    darion->AI()->Talk(10);
                                    player->KilledMonsterCredit(171424);
                                });
                        }

                    }
            if (!player->IsWithinDistInMap(me, 30.0f) && say)
                if (player->GetQuestStatus(QUEST_FEAR_TO_TREAD) == QUEST_STATUS_INCOMPLETE)
                    say = false;
        }
    
    }
};

//171425_Fear_to_Tread_q59914
struct npc_dnt_credit_third_clue : public ScriptedAI
{
    npc_dnt_credit_third_clue(Creature* creature) : ScriptedAI(creature) { Reset(); }

private:
    bool third_say;

    void Reset() override
    {
        third_say = false;
    }

    void MoveInLineOfSight(Unit* target) override
    {
        if (!target->IsPlayer())
            return;

        if (Player* player = target->ToPlayer())
        {
            if (player->GetDistance2d(me) <= 15.0f)
                if (!player->GetQuestObjectiveProgress(QUEST_FEAR_TO_TREAD, 7))
                    if (Creature* darion = GetClosestCreatureWithEntry(player, NPC_SUMMONED_HIGHLORD_DARION, 30.0f))
                    {
                        if (!third_say)
                        {
                            third_say = true;
                            darion->AI()->Talk(11);
                            player->GetScheduler().Schedule(7s, [this, darion, player](TaskContext context)
                                {
                                    darion->AI()->Talk(12);
                                    player->KilledMonsterCredit(171425);
                                    PhasingHandler::AddPhase(player, 10029, true);
                                    PhasingHandler::AddPhase(darion, 10029, true);
                                });
                        }

                    }
            if (!player->IsWithinDistInMap(me, 30.0f) && third_say)
                if (player->GetQuestStatus(QUEST_FEAR_TO_TREAD) == QUEST_STATUS_INCOMPLETE)
                    third_say = false;
        }
    }
};

Position const JainaPisitionLoc[] =
{
    {4876.65f, 7719.6f, 4831.66f, 0},     // 0 JainaWaypoint loc
    {4893.59f, 7724.92f, 4828.31f, 0},    //1
    {4911.11f, 7710.87f, 4823.58f, 0},    //2
    {4949.39f, 7680.36f, 4813.21f, 0},    //3
    {4938.93f, 7641.66f, 4809.88f, 0},    //4
    {4866.08f, 7565.08f, 4798.62f, 0},    //5
    {4833.42f, 7602.61f, 4803.86f, 0},    //6
    {4832.02f, 7618.37f, 4802.94f, 0},    //7
    {4824.68f, 7649.51f, 4797.25f, 0},    //8
    {4799.78f, 7662.31f, 4784.87f, 0},    //9
    {4788.63f, 7661.59f, 4779.37f, 0},    //10
    {4773.65f, 7655.76f, 4773.26f, 0},    //11
    {4733.91f, 7651.51f, 4772.02f, 0},    //12 jaina
    {4733.15f, 7653.59f, 4772.54f, 0},    //13 thrall
    {4738.03f, 7650.39f, 4772.01f, 0},    //14 darion
    {4735.33f, 7649.77f, 4772.09f, 0},    //15 rahm
    {4741.85f, 7645.02f, 4771.91f, 0}     //16 move to staff's place Q59759
};

Position const JainaPisitionQ59756[] =
{
    {4718.45f, 7645.57f, 4772.13f, 0},    // 0 JainaPisition Q59756
    {4730.47f, 7640.31f, 4771.97f, 0},    //1
    {4738.42f, 7634.72f, 4772.32f, 0},    //2 orient 2.13879f
    {4730.71f, 7646.96f, 4771.99f, 0},    //3 orient 4.40691f
    {4742.17f, 7663.21f, 4772.65f, 0},    //4 Darion position 1
    {4734.45f, 7645.73f, 4771.93f, 0},    //5 Darion position 2
    {4733.15f, 7653.59f, 4772.54f, 0},    //6 orient 0.174609f
    {4738.03f, 7650.39f, 4772.01f, 0},    //7 Darion position 3 orient 1.91819f
};

//Quest 59767 The Path to Salvation
Position const JainaPosition59767[] =
{
    {4513.53f, 7423.06f, 4794.35f, 0},    //0 JainaWaypoint 59767
    {4530.76f, 7389.21f, 4794.96f, 0},    //1
    {4507.57f, 7348.96f, 4809.16f, 0},    //2 jaina
    {4509.89f, 7348.26f, 4808.86f, 0},    //3 thrall
    {4507.23f, 7351.22f, 4808.89f, 0},    //4 anduin
    {4509.91f, 7351.39f, 4808.06f, 0},    //5 baine
};

// The Jailer, Sylvanas Windrunner, Mawsworn Guard Positions
static const Position Q59756FirstPositions[10] =
{
    { 4726.43f, 7636.89f, 4772.06f, 5.59625f },   //The Jailer
    { 4732.21f, 7633.68f, 4772.18f, 2.67834f },   //Sylvanas Windrunner
    { 4731.79f, 7646.21f, 4772.14f, 4.55846f },   //Mawsworn Guard
    { 4734.75f, 7645.56f, 4772.11f, 4.55846f },
    { 4737.71f, 7644.86f, 4772.01f, 4.55846f },
    { 4729.01f, 7646.78f, 4772.18f, 4.55846f },
    { 4737.15f, 7642.24f, 4771.98f, 4.55846f },
    { 4734.43f, 7642.86f, 4772.07f, 4.55846f },
    { 4731.53f, 7643.59f, 4772.13f, 4.55846f },
    { 4728.61f, 7644.24f, 4772.18f, 4.55846f }
};

// Baine Bloodhoof, Mawsworn Harbinger, The Jailer, Sylvanas Windrunner Positions
static const Position Q59756SecundPositions[5] =
{
    { 4728.81f, 7636.38f, 4772.07f, 2.80367f },   // Baine Bloodhoof
    { 4735.43f, 7643.89f, 4772.06f, 4.33155f },   // The Jailer aiAnimKit 20915
    { 4728.96f, 7640.09f, 4777.82f, 4.18723f },   //Mawsworn Harbinger aiAnimKit 20912
    { 4733.08f, 7637.74f, 4772.06f, 3.37245f },   //Mawsworn Harbinger aiAnimKit 20914
    { 4727.51f, 7632.51f, 4772.05f, 1.25022f },   //Mawsworn Harbinger aiAnimKit 20916
};

// Malice Shadow, Tormented Soul Positions
static const Position Q59756ThirdPositions[7] =
{
    { 4729.92f, 7639.68f, 4772.15f, 1.1392f },   //Malice Shadow
    { 4726.69f, 7639.66f, 4772.23f, 6.16556f },   //Tormented Soul
    { 4735.4f, 7638.37f, 4772.01f, 3.00754f },
    { 4732.65f, 7642.02f, 4772.1f, 3.90712f },
    { 4728.3f, 7644.23f, 4772.19f, 5.06212f },
    { 4724.03f, 7636.26f, 4771.9f, 0.5567f },
    { 4730.47f, 7634.7f, 4771.99f, 1.77494f }
};

//166980
class npc_lady_jaina_proudmoore_166980 : public CreatureScript
{
public:
    npc_lady_jaina_proudmoore_166980() : CreatureScript("npc_lady_jaina_proudmoore_166980") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_lady_jaina_proudmoore_166980AI(creature);
    }
    struct npc_lady_jaina_proudmoore_166980AI : public EscortAI
    {
        npc_lady_jaina_proudmoore_166980AI(Creature* creature) : EscortAI(creature) { Reset(); }

         void Reset() override
        {
            thrallGUID.Clear();
            darionGUID.Clear();
            anduinGUID.Clear();
            baineGUID.Clear();
            darion = false;
            tariesh = false;
            binger5 = false;
            binger6 = false;
            say59759 = false;
             _events.Reset();
            me->NearTeleportTo(me->GetHomePosition());
        } 

       void MoveInLineOfSight(Unit* target) override
        {
            
            if (Player* player = target->ToPlayer())
            {
                if (player->GetDistance2d(me) <= 50.0f)
                {
                    if (player->GetQuestObjectiveProgress(QUEST_FEAR_TO_TREAD, 7))
                    {
                        m_playerGUID = player->GetGUID();
                        if (me->GetPhaseShift().HasPhase(10029))
                            if (!darion)
                            {
                                darion = true;
                                player->KilledMonsterCredit(166980);
                                _events.ScheduleEvent(EVENT_JAINA_CAST0, 3s);
                            }
                    }

                    if (player->GetQuestObjectiveProgress(QUEST_ON_BLACKENED_WINGS, 1) == 5)
                    {
                        if (!binger5)
                        {
                            binger5 = true;
                            _events.ScheduleEvent(EVENT_JAINA59754_PHASE5, 1s);
                        }
                    }

                    if (player->GetQuestStatus(QUEST_ON_BLACKENED_WINGS) == QUEST_STATUS_COMPLETE)
                    {
                        m_playerGUID = player->GetGUID();
                        if (!binger6)
                        {
                            binger6 = true;
                            _events.ScheduleEvent(EVENT_JAINA59754_PHASE6, 1s);
                        }
                    }
                }
                if (target->GetDistance2d(me) <= 20.0f)
                {
                    if (player->GetQuestStatus(QUEST_THE_LIONS_CAGE) == QUEST_STATUS_INCOMPLETE)
                        if (player->GetQuestObjectiveProgress(QUEST_THE_LIONS_CAGE, 1) && !player->GetQuestObjectiveProgress(QUEST_THE_LIONS_CAGE, 0) && !player->GetQuestObjectiveProgress(QUEST_THE_LIONS_CAGE, 2))
                        {
                            if (!say59759)
                            {
                                player->KilledMonsterCredit(167833);
                                me->AI()->Talk(34);
                                me->AddDelayedEvent(3000, [this]()
                                    {
                                        if (Creature* thrall = me->FindNearestCreature(166981, 20.0f))
                                            thrall->AI()->Talk(7);
                                    });
                                me->AddDelayedEvent(8000, [this]()
                                    {
                                        me->AI()->Talk(35);
                                    });
                            }
                            say59759 = true;
                        }
                }
                if (!player->IsWithinDistInMap(me, 80.0f) && darion)
                    if (player->GetQuestStatus(QUEST_FEAR_TO_TREAD) == QUEST_STATUS_COMPLETE)
                        darion = false;
            }

            if (Creature* creature = target->ToCreature())
            {
                if (target = me->FindNearestCreature(167263, 20.0f, false))
                {
                    if (!tariesh)
                    {
                        tariesh = true;
                        _events.ScheduleEvent(EVENT_FLIGHT_EVENT12, 7s);
                    }

                }
            }
        } 

        bool OnGossipHello(Player* player) override
        {
            if (me->IsQuestGiver())
            {
                player->PrepareQuestMenu(me->GetGUID());
                SendGossipMenuFor(player, player->GetGossipTextId(me), me->GetGUID());
            }

            if (player->GetQuestStatus(QUEST_A_FLIGHT_FROM_DARKNESS) == QUEST_STATUS_INCOMPLETE)
                AddGossipItemFor(player, GossipOptionNpc::None, "I am ready.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

            if (player->GetQuestStatus(QUEST_A_MOMENTS_RESPITE) == QUEST_STATUS_INCOMPLETE)
                if (!player->GetQuestObjectiveProgress(QUEST_A_MOMENTS_RESPITE, 0) && (!player->GetQuestObjectiveProgress(QUEST_A_MOMENTS_RESPITE, 1)) && (!player->GetQuestObjectiveProgress(QUEST_A_MOMENTS_RESPITE, 2)))
                    AddGossipItemFor(player, GossipOptionNpc::None, "Tell me about this place.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);

            if (player->GetQuestStatus(QUEST_A_MOMENTS_RESPITE) == QUEST_STATUS_INCOMPLETE)
                if (player->GetQuestObjectiveProgress(QUEST_A_MOMENTS_RESPITE, 0) && (!player->GetQuestObjectiveProgress(QUEST_A_MOMENTS_RESPITE, 1)) && (!player->GetQuestObjectiveProgress(QUEST_A_MOMENTS_RESPITE, 2)))
                    AddGossipItemFor(player, GossipOptionNpc::None, "Tell me more of the Jailer.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);

            if (player->GetQuestStatus(QUEST_A_MOMENTS_RESPITE) == QUEST_STATUS_INCOMPLETE)
                if (player->GetQuestObjectiveProgress(QUEST_A_MOMENTS_RESPITE, 0) && (player->GetQuestObjectiveProgress(QUEST_A_MOMENTS_RESPITE, 1)) && (!player->GetQuestObjectiveProgress(QUEST_A_MOMENTS_RESPITE, 2)))
                    AddGossipItemFor(player, GossipOptionNpc::None, "What about the others who were taken?", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);

            if (player->GetQuestStatus(QUEST_THE_LIONS_CAGE) == QUEST_STATUS_INCOMPLETE)
                if (player->GetQuestObjectiveProgress(QUEST_THE_LIONS_CAGE, 1))
                    AddGossipItemFor(player, GossipOptionNpc::None, "Lie low and observe.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);

            if (player->GetQuestStatus(QUEST_THE_PATH_TO_SALVATION) == QUEST_STATUS_INCOMPLETE)
                if (player->GetQuestObjectiveProgress(QUEST_THE_PATH_TO_SALVATION, 0))
                    AddGossipItemFor(player, GossipOptionNpc::None, " I am ready.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);

            SendGossipMenuFor(player, player->GetGossipTextId(me), me->GetGUID());

            return true;
        }

        bool OnGossipSelect(Player* player, uint32 /*MenuID*/, uint32 gossipListId) override
        {
            uint32 const action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);
            ClearGossipMenuFor(player);
            switch (action)
            {
            case GOSSIP_ACTION_INFO_DEF + 1:
                CloseGossipMenuFor(player);
                if (Creature* thrall = GetClosestCreatureWithEntry(me, 166981, 20.0f))
                    thrallGUID = thrall->GetGUID();
                if (Creature* darion = GetClosestCreatureWithEntry(me, NPC_HIGHLORD_DARION, 20.0f))
                    darionGUID = darion->GetGUID();
                if (Creature* rahm = GetClosestCreatureWithEntry(me, 174681, 20.0f))
                    rahmGUID = rahm->GetGUID();
                m_playerGUID = player->GetGUID();
                me->RemoveAura(329892);
                _events.ScheduleEvent(EVENT_FLIGHT_EVENT1, 1s);
                break;

            case GOSSIP_ACTION_INFO_DEF + 2:
                CloseGossipMenuFor(player);
                if (Creature* thrall = GetClosestCreatureWithEntry(me, 166981, 20.0f))
                    thrallGUID = thrall->GetGUID();
                m_playerGUID = player->GetGUID();
                me->RemoveAura(329892);
                _events.ScheduleEvent(EVENT_MOMENT_EVENT1, 1s);
                break;

            case GOSSIP_ACTION_INFO_DEF + 3:
                CloseGossipMenuFor(player);
                player->GetScheduler().Schedule(Milliseconds(8000), [this, player](TaskContext context)
                    {
                        if (Creature* jialer = GetClosestCreatureWithEntry(me, 170151, 20.0f))
                            jialer->ForcedDespawn();
                        if (Creature* sylvanas = GetClosestCreatureWithEntry(me, 170152, 20.0f))
                            sylvanas->ForcedDespawn();
                        std::list<Creature*> cList = me->FindNearestCreatures(170180, 50.0f);
                        for (std::list<Creature*>::const_iterator itr = cList.begin(); itr != cList.end(); ++itr)
                        {
                            if (Creature* guard = *itr)
                            {
                                guard->ForcedDespawn();
                            }
                        }
                    });
                _events.ScheduleEvent(EVENT_MOMENT_EVENT7, 1s);
                break;

            case GOSSIP_ACTION_INFO_DEF + 4:
                CloseGossipMenuFor(player);
                if (Creature* darion = GetClosestCreatureWithEntry(me, NPC_HIGHLORD_DARION, 50.0f))
                    darionGUID = darion->GetGUID();
                m_playerGUID = player->GetGUID();
                me->AI()->Talk(25);
                player->GetScheduler().Schedule(Milliseconds(4000), [this, player](TaskContext context)
                    {
                        if (Creature* jialer = GetClosestCreatureWithEntry(me, 170151, 30.0f))
                            jialer->ForcedDespawn();
                        if (Creature* baine = GetClosestCreatureWithEntry(me, 170163, 30.0f))
                            baine->ForcedDespawn();
                        std::list<Creature*> cList = me->FindNearestCreatures(170216, 30.0f);
                        for (std::list<Creature*>::const_iterator itr = cList.begin(); itr != cList.end(); ++itr)
                        {
                            if (Creature* harbinger = *itr)
                            {
                                harbinger->ForcedDespawn();
                            }
                        }
                    });
                _events.ScheduleEvent(EVENT_MOMENT_EVENT12, 8s);
                break;

            case GOSSIP_ACTION_INFO_DEF + 5:
                CloseGossipMenuFor(player);
                if (player->HasQuest(QUEST_THE_LIONS_CAGE))
                {
                    player->KilledMonsterCredit(166980);
                    player->AddDelayedEvent(1000, [this, player]() -> void
                        {
                            player->NearTeleportTo(5354.01f, 7623.45f, 4897.44f, 0.4738f, false);
                            player->CastSpell(player, SCENE_THE_LIONS_CAGE, true);
                        });
                }
                break;
            case GOSSIP_ACTION_INFO_DEF + 6:
                CloseGossipMenuFor(player);
                if (player->HasQuest(QUEST_THE_PATH_TO_SALVATION))
                {
                    m_playerGUID = player->GetGUID();
                    if (Creature* thrall = GetClosestCreatureWithEntry(me, 166981, 20.0f))
                    {
                        thrallGUID = thrall->GetGUID();
                        me->AddDelayedEvent(8500, [this, thrall]()
                            {
                                thrall->AI()->Talk(26);
                            });
                    }
                    me->AddDelayedEvent(12500, [this]()
                        {
                            me->AI()->Talk(44);
                        });
                    me->AddDelayedEvent(15500, [this, player]()
                        {
                            me->CastSpell(player, SPELL_MASS_INVISIBILITY_TRIGGER, true);
                            me->AddAura(SPELL_MASS_INVISIBILITY, player);
                        });
                    me->AddDelayedEvent(17000, [this, player]()
                        {
                            Start(true, player->GetGUID());
                            if (Creature* thrall = ObjectAccessor::GetCreature(*me, thrallGUID))
                            {
                                thrall->GetMotionMaster()->MoveFollow(me, 1.0f, 60);
                                thrall->SetUnitFlag(UNIT_FLAG_IMMUNE_TO_NPC);
                                thrall->SetReactState(REACT_PASSIVE);
                            }
                            if (Creature* anduin = ObjectAccessor::GetCreature(*me, anduinGUID))
                            {
                                anduin->GetMotionMaster()->MoveFollow(me, 1.0f, 120);
                                anduin->SetFaction(35);
                                anduin->SetUnitFlag(UNIT_FLAG_IMMUNE_TO_NPC);
                                anduin->SetReactState(REACT_PASSIVE);
                            }
                            if (Creature* baine = ObjectAccessor::GetCreature(*me, baineGUID))
                            {
                                baine->GetMotionMaster()->MoveFollow(me, 1.0f, 180);
                                baine->SetUnitFlag(UNIT_FLAG_IMMUNE_TO_NPC);
                                baine->SetReactState(REACT_PASSIVE);
                            }
                        });
                    if (Creature* anduin = GetClosestCreatureWithEntry(me, 167833, 20.0f))
                        anduinGUID = anduin->GetGUID();
                    if (Creature* baine = GetClosestCreatureWithEntry(me, 168162, 20.0f))
                    {
                        baineGUID = baine->GetGUID();
                        baine->AI()->Talk(5);
                    }
                }
                break;
            }
            return true;
        }

        void WaypointReached(uint32 pointId, uint32 pathId) override
        {
            switch (pointId)
            {
            case 1:
                me->AI()->Talk(45);
                break;

            case 2:
                if (Creature* soulhunter = me->FindNearestCreature(167947, 50.f))
                    if (soulhunter->GetSpawnId() == 345892)
                    {
                        soulhunter->SetWalk(true);
                        soulhunter->GetMotionMaster()->MovePoint(0, 4434.66f, 7302.16f, 4822.57f, true);
                        soulhunter->DespawnOrUnsummon(35s, 30s);
                    }
                break;

            case 3:
                me->AI()->Talk(46);
                break;

            case 4:
                if (Creature* baine = ObjectAccessor::GetCreature(*me, baineGUID))
                    baine->AI()->Talk(6);
                break;

            case 6:
                SetEscortPaused(true);
                {
                    if (HasEscortState(STATE_ESCORT_PAUSED))
                    {
                        me->AI()->Talk(47);

                        if (Creature* eye = me->FindNearestCreature(169832, 50.f))
                        {
                            eye->SetWalk(true);
                            eye->GetMotionMaster()->MovePoint(0, 4405.88f, 7189.05f, 4824.83f, true);
                            eye->DespawnOrUnsummon(30s, 30s);
                        }
                        SetPauseTimer(10s);
                        SetEscortPaused(false);
                    }
                }
                break;

            case 7:
                if (Creature* anduin = ObjectAccessor::GetCreature(*me, anduinGUID))
                    anduin->AI()->Talk(10);
                break;

            case 11:
                SetEscortPaused(true);
                {
                    if (HasEscortState(STATE_ESCORT_PAUSED))
                    {
                        me->AI()->Talk(47);

                        if (Creature* eye = me->FindNearestCreature(169832, 50.f))
                        {
                            eye->SetWalk(true);
                            eye->GetMotionMaster()->MovePoint(0, 4484.21f, 7022.51f, 4839.81f, true);
                            eye->DespawnOrUnsummon(30s, 30s);
                        }
                        me->AddDelayedEvent(3000, [this]()
                            {
                                if (Creature* baine = ObjectAccessor::GetCreature(*me, baineGUID))
                                    baine->AI()->Talk(7);
                            });
                        me->AddDelayedEvent(10000, [this]()
                            {
                                if (Creature* thrall = ObjectAccessor::GetCreature(*me, thrallGUID))
                                    thrall->AI()->Talk(27);
                            });
                        SetPauseTimer(10s);
                        SetEscortPaused(false);
                    }
                }
                break;

            case 15:
                SetEscortPaused(true);
                {
                    if (HasEscortState(STATE_ESCORT_PAUSED))
                    {
                        if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                            player->RemoveAura(SPELL_MASS_INVISIBILITY);
                        me->AI()->Talk(48);
                        me->AddDelayedEvent(7000, [this]()
                            {
                                me->AI()->Talk(49);
                            });
                        SetPauseTimer(5s);
                        SetEscortPaused(false);
                    }
                }
                break;

            case 19:
                if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                    if (Creature* thrall = ObjectAccessor::GetCreature(*me, thrallGUID))
                        if (Creature* anduin = ObjectAccessor::GetCreature(*me, anduinGUID))
                            if (Creature* baine = ObjectAccessor::GetCreature(*me, baineGUID))
                            {
                                thrall->GetMotionMaster()->MovePoint(0, 4572.59f, 6904.2f, 4867.25f, true, 1.4416f);
                                anduin->GetMotionMaster()->MovePoint(0, 4568.91f, 6907.98f, 4867.33f, true, 1.3881f);
                                baine->GetMotionMaster()->MovePoint(0, 4566.8f, 6905.02f, 4867.33f, true, 1.1754f);
                                anduin->AI()->Talk(11);
                                me->AddDelayedEvent(8000, [this, anduin]()
                                    {
                                        anduin->AI()->Talk(12);
                                    });
                                me->AddDelayedEvent(13000, [this, player]()
                                    {
                                        me->AI()->Talk(50, player);
                                    });
                            }
                break;

            case 21:
                SetEscortPaused(true);
                {
                    SetPauseTimer(10s);
                    SetEscortPaused(false);
                }
                break;

            case 22:
                if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                    if (Creature* thrall = ObjectAccessor::GetCreature(*me, thrallGUID))
                        if (Creature* anduin = ObjectAccessor::GetCreature(*me, anduinGUID))
                            if (Creature* baine = ObjectAccessor::GetCreature(*me, baineGUID))
                            {
                                player->ForceCompleteQuest(QUEST_THE_PATH_TO_SALVATION);
                                thrall->DespawnOrUnsummon(1s, 30s);
                                anduin->DespawnOrUnsummon(1s, 30s);
                                baine->DespawnOrUnsummon(1s, 30s);
                                me->DespawnOrUnsummon(1s, 30s);
                            }
                break;
            }
        }


        void OnQuestAccept(Player* player, Quest const* quest) override
        {
            if (quest->GetQuestId() == QUEST_ON_BLACKENED_WINGS)
            {
                m_playerGUID = player->GetGUID();
                me->SetStandState(UNIT_STAND_STATE_STAND);
                me->AI()->Talk(1);
                _events.ScheduleEvent(EVENT_JAINA59754_PHASE0, 4s);
            }

            if (quest->GetQuestId() == QUEST_THE_LIONS_CAGE)
            {
                m_playerGUID = player->GetGUID();
                if (Creature* thrall = GetClosestCreatureWithEntry(me, 166981, 50.0f))
                    thrallGUID = thrall->GetGUID();
                me->SetWalk(true);
                me->AI()->Talk(29);
                me->GetMotionMaster()->MovePoint(0, JainaPisitionLoc[16]);
                me->AddDelayedEvent(8000, [this, player]()
                    {
                        PhasingHandler::AddPhase(player, 10044, true);
                        me->SetSheath(SHEATH_STATE_UNARMED);
                    });
                me->AddDelayedEvent(11000, [this, player]()
                    {
                        if (Creature* thrall = ObjectAccessor::GetCreature(*me, thrallGUID))
                        {
                            if (Creature* jaina1 = me->SummonCreature(166980, *me, TEMPSUMMON_MANUAL_DESPAWN, 300s, 0))
                            {
                                me->ForcedDespawn(500, 180s);
                                thrall->ForcedDespawn(500, 180s);
                                jaina1->RemoveAura(329892);
                                jaina1->SetOwnerGUID(player->GetGUID());                   //set player onwer
                                jaina1->GetMotionMaster()->MoveFollow(player, 2.0f, 70);
                                PhasingHandler::AddPhase(jaina1, 10040, true);
                                PhasingHandler::AddPhase(jaina1, 10041, true);
                                jaina1->AI()->Talk(30);
                            }

                            if (Creature* thrall1 = me->SummonCreature(166981, *thrall, TEMPSUMMON_MANUAL_DESPAWN, 300s, 0))
                            {
                                thrall1->SetOwnerGUID(player->GetGUID());                   //set player onwer
                                thrall1->GetMotionMaster()->MoveFollow(player, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
                                if (int32 playerFaction = player->GetFaction())
                                    thrall1->SetFaction(playerFaction);

                                if (int32 playerAttackPower = player->GetTotalAttackPowerValue(BASE_ATTACK) * 2.0f)
                                    thrall1->SetAttackPower(playerAttackPower);
                                PhasingHandler::AddPhase(thrall1, 10040, true);
                                PhasingHandler::AddPhase(thrall1, 10041, true);
                            }

                        }
                    });
            }

            Position _positon = player->GetPosition();

            if (quest->GetQuestId() == QUEST_BY_AND_DOWN_THE_RIVER)
            {
                m_playerGUID = player->GetGUID();
                if (player->GetPhaseShift().HasPhase(10046)) PhasingHandler::RemovePhase(player, 10046, true);

                player->SummonCreature(167154, Position(_positon), TEMPSUMMON_TIMED_DESPAWN, 900s);
                player->SummonCreature(167176, Position(_positon), TEMPSUMMON_TIMED_DESPAWN, 900s);
                player->SummonCreature(167906, Position(_positon), TEMPSUMMON_TIMED_DESPAWN, 900s);
            }

            if (quest->GetQuestId() == QUEST_THE_PATH_TO_SALVATION)
            {
                m_playerGUID = player->GetGUID();
                if (Creature* thrall = GetClosestCreatureWithEntry(me, 166981, 20.0f))
                {
                    thrallGUID = thrall->GetGUID();
                    thrall->AI()->Talk(25);
                }
                if (Creature* anduin = GetClosestCreatureWithEntry(me, 167833, 20.0f))
                    anduinGUID = anduin->GetGUID();
                if (Creature* baine = GetClosestCreatureWithEntry(me, 168162, 20.0f))
                    baineGUID = baine->GetGUID();
                _events.ScheduleEvent(EVENT_SALVATION_PHASE1, 6s);
            }
        }

        void OnQuestReward(Player* player, Quest const* quest, LootItemType /*type*/, uint32 /*opt*/) override
        {
            if (quest->GetQuestId() == QUEST_ON_BLACKENED_WINGS)
            {
                binger5 = false;
                binger6 = false;
            }

            if (quest->GetQuestId() == QUEST_A_FLIGHT_FROM_DARKNESS)
            {
                if (Creature* thrall = ObjectAccessor::GetCreature(*me, thrallGUID))
                {
                    thrall->ForcedDespawn(1000, 15s);
                }
                if (Creature* darion = ObjectAccessor::GetCreature(*me, darionGUID))
                {
                    darion->ForcedDespawn(1000, 15s);
                }
                if (Creature* rahm = ObjectAccessor::GetCreature(*me, rahmGUID))
                {
                    rahm->ForcedDespawn(1000, 15s);
                }
                me->ForcedDespawn(1000, 15s);
                PhasingHandler::AddPhase(player, 10041, true);
            }

             if (quest->GetQuestId() == QUEST_BY_AND_DOWN_THE_RIVER)
            {
                 if (Creature* thrall = me->FindNearestCreature(166981, 20.0f))
                 {
                     if (thrall->GetPhaseShift().HasPhase(10044))
                         thrall->ForcedDespawn(1000, 15s);
                 }
                 if (Creature* anduin = me->FindNearestCreature(167833, 20.0f))
                 {
                     if (anduin->GetPhaseShift().HasPhase(10044))
                         anduin->ForcedDespawn(1000, 15s);
                 }
                 if (Creature* bain = me->FindNearestCreature(168162, 20.0f))
                 {
                     if (bain->GetPhaseShift().HasPhase(10044))
                         bain->ForcedDespawn(1000, 15s);
                 }
                 if (me->GetPhaseShift().HasPhase(10044))
                     me->ForcedDespawn(1000, 15s);
                 if (player->GetPhaseShift().HasPhase(10044)) PhasingHandler::RemovePhase(player, 10044, true);
                 if (!player->GetPhaseShift().HasPhase(10041)) PhasingHandler::AddPhase(player, 10041, true);
            }
        }

        void Q59756FirstCreatureSummon()
        {
            TempSummon* Jailer = me->SummonCreature(170151, Q59756FirstPositions[0], TEMPSUMMON_TIMED_DESPAWN, 60s);
            TempSummon* Sylvanas = me->SummonCreature(170152, Q59756FirstPositions[1], TEMPSUMMON_TIMED_DESPAWN, 60s);
            TempSummon* guard1 = me->SummonCreature(170180, Q59756FirstPositions[2], TEMPSUMMON_TIMED_DESPAWN, 60s);
            TempSummon* guard2 = me->SummonCreature(170180, Q59756FirstPositions[3], TEMPSUMMON_TIMED_DESPAWN, 60s);
            TempSummon* guard3 = me->SummonCreature(170180, Q59756FirstPositions[4], TEMPSUMMON_TIMED_DESPAWN, 60s);
            TempSummon* guard4 = me->SummonCreature(170180, Q59756FirstPositions[5], TEMPSUMMON_TIMED_DESPAWN, 60s);
            TempSummon* guard5 = me->SummonCreature(170180, Q59756FirstPositions[6], TEMPSUMMON_TIMED_DESPAWN, 60s);
            TempSummon* guard6 = me->SummonCreature(170180, Q59756FirstPositions[7], TEMPSUMMON_TIMED_DESPAWN, 60s);
            TempSummon* guard7 = me->SummonCreature(170180, Q59756FirstPositions[8], TEMPSUMMON_TIMED_DESPAWN, 60s);
            TempSummon* guard8 = me->SummonCreature(170180, Q59756FirstPositions[9], TEMPSUMMON_TIMED_DESPAWN, 60s);
        }

        void Q59756SecundCreatureSummon()
        {
            if (TempSummon* Jailer = me->SummonCreature(170151, Q59756SecundPositions[1], TEMPSUMMON_TIMED_DESPAWN, 60s))
                Jailer->SetAIAnimKitId(20915);

            TempSummon* Baine = me->SummonCreature(170163, Q59756SecundPositions[0], TEMPSUMMON_TIMED_DESPAWN, 60s);

            if (TempSummon* Harbinger1 = me->SummonCreature(170216, Q59756SecundPositions[2], TEMPSUMMON_TIMED_DESPAWN, 60s))
                Harbinger1->SetAIAnimKitId(20912);
            if (TempSummon* Harbinger2 = me->SummonCreature(170216, Q59756SecundPositions[3], TEMPSUMMON_TIMED_DESPAWN, 60s))
                Harbinger2->SetAIAnimKitId(20916);
            if (TempSummon* Harbinger3 = me->SummonCreature(170216, Q59756SecundPositions[4], TEMPSUMMON_TIMED_DESPAWN, 60s))
                Harbinger3->SetAIAnimKitId(20914);
        }

        void Q59756ThirdCreatureSummon()
        {
            TempSummon* shadow = me->SummonCreature(170220, Q59756ThirdPositions[0], TEMPSUMMON_TIMED_DESPAWN, 15s);
            TempSummon* soul1 = me->SummonCreature(170222, Q59756ThirdPositions[1], TEMPSUMMON_TIMED_DESPAWN, 15s);
            TempSummon* soul2 = me->SummonCreature(170222, Q59756ThirdPositions[2], TEMPSUMMON_TIMED_DESPAWN, 15s);
            TempSummon* soul3 = me->SummonCreature(170222, Q59756ThirdPositions[3], TEMPSUMMON_TIMED_DESPAWN, 15s);
            TempSummon* soul4 = me->SummonCreature(170222, Q59756ThirdPositions[4], TEMPSUMMON_TIMED_DESPAWN, 15s);
            TempSummon* soul5 = me->SummonCreature(170222, Q59756ThirdPositions[5], TEMPSUMMON_TIMED_DESPAWN, 15s);
            TempSummon* soul6 = me->SummonCreature(170222, Q59756ThirdPositions[6], TEMPSUMMON_TIMED_DESPAWN, 15s);
        }

       void UpdateAI(uint32 diff) override
        {
            EscortAI::UpdateAI(diff);                              //It is used because of Start function.

            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_JAINA_CAST0:
                {
                    if (me->GetPhaseShift().HasPhase(10029))
                    _events.ScheduleEvent(EVENT_JAINA_CAST1, 3s);
                    break;
                }
                case EVENT_JAINA_CAST1:
                {
                    if (me->GetPhaseShift().HasPhase(10029))
                    {
                        if (Creature* darion = GetClosestCreatureWithEntry(me, NPC_SUMMONED_HIGHLORD_DARION, 80.0f))
                            darion->AI()->Talk(13);
                        me->CastSpell(me, 279565, true);
                    }
                    _events.ScheduleEvent(EVENT_JAINA_CAST2, 3s);
                    break;
                }
                case EVENT_JAINA_CAST2:
                {
                    if (me->GetPhaseShift().HasPhase(10029))
                    {
                        std::list<Creature*> cList1 = me->FindNearestCreatures(168531, 50.0f);
                        for (std::list<Creature*>::const_iterator itr = cList1.begin(); itr != cList1.end(); ++itr)
                        {
                            if (Creature* npc = *itr)
                            {
                                me->CastSpell(npc, 342755, true);
                                npc->AddAura(342755, npc);
                            }
                        }

                        std::list<Creature*> cList2 = me->FindNearestCreatures(168533, 50.0f);
                        for (std::list<Creature*>::const_iterator itr = cList2.begin(); itr != cList2.end(); ++itr)
                        {
                            if (Creature* npc = *itr)
                            {
                                me->CastSpell(npc, 342755, true);
                                npc->AddAura(342755, npc);

                            }
                        }

                        std::list<Creature*> cList3 = me->FindNearestCreatures(168534, 50.0f);
                        for (std::list<Creature*>::const_iterator itr = cList3.begin(); itr != cList3.end(); ++itr)
                        {
                            if (Creature* npc = *itr)
                            {
                                me->CastSpell(npc, 342755, true);
                                npc->AddAura(342755, npc);
                            }
                        }
                    }
                    _events.ScheduleEvent(EVENT_JAINA_CAST3, 3s);
                    break;
                }
                case EVENT_JAINA_CAST3:
                {
                    if (me->GetPhaseShift().HasPhase(10029))
                    {
                        me->NearTeleportTo(4824.99f, 7741.24f, 4844.32f, 1.4445f, false);
                        TempSummon* large_aoi_effect1 = me->SummonCreature(169474, Position(4821.99f, 7736.24f, 4844.32f, 1.4445f), TEMPSUMMON_TIMED_DESPAWN, 5s, 0U);
                        TempSummon* large_aoi_effect2 = me->SummonCreature(169474, Position(4825.99f, 7743.24f, 4844.32f, 1.4445f), TEMPSUMMON_TIMED_DESPAWN, 5s, 0U);
                        TempSummon* large_aoi_effect3 = me->SummonCreature(169474, Position(4824.99f, 7741.24f, 4844.32f, 1.4445f), TEMPSUMMON_TIMED_DESPAWN, 5s, 0U);
                        TempSummon* large_aoi_effect4 = me->SummonCreature(169474, Position(4820.99f, 7738.24f, 4844.32f, 1.4445f), TEMPSUMMON_TIMED_DESPAWN, 5s, 0U);
                        TempSummon* large_aoi_effect5 = me->SummonCreature(169474, Position(4818.99f, 7745.24f, 4844.32f, 1.4445f), TEMPSUMMON_TIMED_DESPAWN, 5s, 0U);
                    }
                    _events.ScheduleEvent(EVENT_JAINA_CAST4, 5s);
                    break;
                }
                case EVENT_JAINA_CAST4:
                {
                    if (me->GetPhaseShift().HasPhase(10029))
                    {
                        std::list<Creature*> cList1 = me->FindNearestCreatures(168531, 60.0f);
                        for (std::list<Creature*>::const_iterator itr = cList1.begin(); itr != cList1.end(); ++itr)
                        {
                            if (Creature* npc = *itr)
                            {
                                me->CastSpell(npc, 342755, true);
                                npc->KillSelf();
                            }
                        }

                        std::list<Creature*> cList2 = me->FindNearestCreatures(168533, 60.0f);
                        for (std::list<Creature*>::const_iterator itr = cList2.begin(); itr != cList2.end(); ++itr)
                        {
                            if (Creature* npc = *itr)
                            {
                                me->CastSpell(npc, 342755, true);
                                npc->KillSelf();
                            }
                        }

                        std::list<Creature*> cList3 = me->FindNearestCreatures(168534, 60.0f);
                        for (std::list<Creature*>::const_iterator itr = cList3.begin(); itr != cList3.end(); ++itr)
                        {
                            if (Creature* npc = *itr)
                            {
                                me->CastSpell(npc, 342755, true);
                                npc->KillSelf();
                            }
                        }
                        if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                        if (Creature* darion = GetClosestCreatureWithEntry(player, NPC_SUMMONED_HIGHLORD_DARION, 30.0f))
                        {
                            darion->SetWalk(true);
                            darion->AI()->Talk(14);
                            darion->GetMotionMaster()->Clear();
                            darion->GetMotionMaster()->MovePoint(0, 4820.61f, 7755.77f, 4841.18f, true);
                            darion->ForcedDespawn(7000);
                        }
                    }
                    _events.ScheduleEvent(EVENT_JAINA_CAST5, 5s);
                    break;
                }
                case EVENT_JAINA_CAST5:
                {
                    if (me->GetPhaseShift().HasPhase(10029))
                    {
                        me->RemoveAura(329892);
                        me->SetStandState(UNIT_STAND_STATE_KNEEL);
                        Talk(0);
                    }
                    _events.ScheduleEvent(EVENT_JAINA_CAST6, 3s);
                    break;
                }
                case EVENT_JAINA_CAST6:
                {
                    if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                    {
                        PhasingHandler::AddPhase(player, 10030, true);
                        PhasingHandler::RemovePhase(player, 10029, true);
                        if (me->GetPhaseShift().HasPhase(10029))
                        {
                            me->DespawnOrUnsummon(1s);
                            me->SetRespawnDelay(10);
                        }
                        break;
                    }
                }
                //59754 On Blackened Wings Event
                case EVENT_JAINA59754_PHASE0:
                {
                    if (Creature* thrall = GetClosestCreatureWithEntry(me, 166981, 10.0f))
                    {
                        thrall->SetFacingToObject(me);
                        thrall->SetStandState(UNIT_STAND_STATE_STAND);
                        thrall->AI()->Talk(0);
                    }
                    _events.ScheduleEvent(EVENT_JAINA59754_PHASE1, 3s);
                    break;
                }
                case EVENT_JAINA59754_PHASE1:
                {
                    if (Creature* thrall = GetClosestCreatureWithEntry(me, 166981, 10.0f))
                        thrall->AI()->Talk(1);
                    _events.ScheduleEvent(EVENT_JAINA59754_PHASE2, 4s);
                    break;
                }
                case EVENT_JAINA59754_PHASE2:
                {
                    if (Creature* darion = GetClosestCreatureWithEntry(me, NPC_HIGHLORD_DARION, 40.0f))
                        darion->AI()->Talk(21);
                    if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                    {
                        player->GetScheduler().Schedule(Milliseconds(3000), [this, player](TaskContext context)
                            {
                                player->PlayConversation(15169);
                            });
                    }
                    _events.ScheduleEvent(EVENT_JAINA59754_PHASE3, 7s);
                    break;
                }
                case EVENT_JAINA59754_PHASE3:
                {
                    me->AI()->Talk(2);
                    _events.ScheduleEvent(EVENT_JAINA59754_PHASE4, 3s);
                    break;
                }
                case EVENT_JAINA59754_PHASE4:
                {
                    if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                    {
                        PhasingHandler::RemovePhase(player, 10030, true);
                        player->GetSceneMgr().PlaySceneByPackageId(3092, SceneFlag::NotCancelable);
                        player->KilledMonsterCredit(166980);
                        PhasingHandler::AddPhase(player, 10031, true);
                    }
                    break;
                }
                //Defeat five QuestObjective Harbinger
                case EVENT_JAINA59754_PHASE5:
                {
                    me->AI()->Talk(3);
                    break;
                }
                //Defeat six QuestObjective Harbinger
                case EVENT_JAINA59754_PHASE6:
                {
                    if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                    {
                        player->CastSpell(player, 325412, true);
                        player->NearTeleportTo(4870.75f, 7706.34f, 4831.19f, 2.94011f, false);
                        PhasingHandler::RemovePhase(player, 10031, true);
                    }
                    _events.ScheduleEvent(EVENT_JAINA59754_PHASE7, 3s);
                    break;
                }
                case EVENT_JAINA59754_PHASE7:
                {
                    if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                    {
                        player->RemoveAura(325412);
                    }
                    std::list<Creature*> cList = me->FindNearestCreatures(165862, 40.0f);
                    for (std::list<Creature*>::const_iterator itr = cList.begin(); itr != cList.end(); ++itr)
                    {
                        if (Creature* ebon = *itr)
                        {
                            ebon->RemoveAura(325412);
                            ebon->CastSpell(ebon, 325412, true);
                        }
                    }
                    me->RemoveAura(325412);
                    me->CastSpell(me, 325412, true);
                    me->AI()->Talk(4);
                    _events.ScheduleEvent(EVENT_JAINA59754_PHASE8, 4s);
                    break;
                }
                case EVENT_JAINA59754_PHASE8:
                {
                    me->AI()->Talk(5);
                    break;
                }
                //59755 A Flight from Darkness
                case EVENT_FLIGHT_EVENT1:
                {
                    if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                    {
                        std::list<Creature*> cList = me->FindNearestCreatures(165862, 40.0f);
                        for (std::list<Creature*>::const_iterator itr = cList.begin(); itr != cList.end(); ++itr)
                        {
                            if (Creature* guard = *itr)
                            {
                                guard->SetOwnerGUID(player->GetGUID());
                                guard->GetMotionMaster()->MovePoint(0, JainaPisitionLoc[0].GetPositionWithOffset({ float(rand32() % 15), float(rand32() % 15) }));
                            }
                        }
                        if (Creature* thrall = ObjectAccessor::GetCreature(*me, thrallGUID))
                        {
                            thrall->RemoveUnitFlag(UNIT_FLAG_IMMUNE_TO_NPC);
                            thrall->SetOwnerGUID(player->GetGUID());
                            thrall->GetMotionMaster()->MovePoint(0, JainaPisitionLoc[0].GetPositionWithOffset({ float(rand32() % 10), float(rand32() % 10), +1.0f }));
                        }
                        if (Creature* darion = ObjectAccessor::GetCreature(*me, darionGUID))
                        {
                            darion->RemoveUnitFlag(UNIT_FLAG_IMMUNE_TO_NPC);
                            darion->SetOwnerGUID(player->GetGUID());
                            darion->GetMotionMaster()->MovePoint(0, JainaPisitionLoc[0].GetPositionWithOffset({  float(rand32() % 10), float(rand32() % 10), +1.0f }));
                        }
                        if (Creature* rahm = ObjectAccessor::GetCreature(*me, rahmGUID))
                        {
                            rahm->RemoveUnitFlag(UNIT_FLAG_IMMUNE_TO_NPC);
                            rahm->SetOwnerGUID(player->GetGUID());
                            rahm->GetMotionMaster()->MovePoint(0, JainaPisitionLoc[0].GetPositionWithOffset({  float(rand32() % 10), float(rand32() % 10), +1.0f }));
                        }
                    }

                    me->AI()->Talk(6);
                    me->RemoveUnitFlag(UNIT_FLAG_IMMUNE_TO_NPC);
                    me->GetMotionMaster()->MovePoint(0, JainaPisitionLoc[0]);
                    _events.ScheduleEvent(EVENT_FLIGHT_EVENT2, 6s);
                    break;
                }
                case EVENT_FLIGHT_EVENT2:
                {
                    std::list<Creature*> cList = me->FindNearestCreatures(165862, 40.0f);
                    for (std::list<Creature*>::const_iterator itr = cList.begin(); itr != cList.end(); ++itr)
                    {
                        if (Creature* guard = *itr)
                        {
                            guard->GetMotionMaster()->MovePoint(0, JainaPisitionLoc[1].GetPositionWithOffset({ float(rand32() % 15), float(rand32() % 15) }));
                        }
                    }
                    if (Creature* thrall = ObjectAccessor::GetCreature(*me, thrallGUID))
                    {
                        thrall->GetMotionMaster()->MovePoint(0, JainaPisitionLoc[1].GetPositionWithOffset({  float(rand32() % 10), float(rand32() % 10), +1.0f }));
                        thrall->AI()->Talk(2);
                    }
                    if (Creature* darion = ObjectAccessor::GetCreature(*me, darionGUID))
                        darion->GetMotionMaster()->MovePoint(0, JainaPisitionLoc[1].GetPositionWithOffset({  float(rand32() % 10), float(rand32() % 10), +1.0f }));
                    if (Creature* rahm = ObjectAccessor::GetCreature(*me, rahmGUID))
                        rahm->GetMotionMaster()->MovePoint(0, JainaPisitionLoc[1].GetPositionWithOffset({  float(rand32() % 10), float(rand32() % 10), +1.0f }));
                    if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                    {
                        player->GetScheduler().Schedule(Milliseconds(5000), [this](TaskContext context)
                            {
                               me->AI()->Talk(7);
                            });
                    }
                    me->GetMotionMaster()->MovePoint(0, JainaPisitionLoc[1]);
                    _events.ScheduleEvent(EVENT_FLIGHT_EVENT3, 6s);
                    break;
                }
                case EVENT_FLIGHT_EVENT3:
                {
                    std::list<Creature*> cList = me->FindNearestCreatures(165862, 40.0f);
                    for (std::list<Creature*>::const_iterator itr = cList.begin(); itr != cList.end(); ++itr)
                    {
                        if (Creature* guard = *itr)
                        {
                            guard->GetMotionMaster()->MovePoint(0, JainaPisitionLoc[2].GetPositionWithOffset({ float(rand32() % 15), float(rand32() % 15) }));
                        }
                    }
                    if (Creature* thrall = ObjectAccessor::GetCreature(*me, thrallGUID))
                    {
                        thrall->GetMotionMaster()->MovePoint(0, JainaPisitionLoc[2].GetPositionWithOffset({  float(rand32() % 10), float(rand32() % 10), +1.0f }));
                    }
                    if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                    {
                        if (Creature* darion = ObjectAccessor::GetCreature(*me, darionGUID))
                        {
                            darion->GetMotionMaster()->MovePoint(0, JainaPisitionLoc[2].GetPositionWithOffset({  float(rand32() % 10), float(rand32() % 10), +1.0f }));
                            player->GetScheduler().Schedule(Milliseconds(2000), [this, darion](TaskContext context)
                                {
                                    darion->AI()->Talk(22);
                                });
                        }
                    }
                    if (Creature* rahm = ObjectAccessor::GetCreature(*me, rahmGUID))
                        rahm->GetMotionMaster()->MovePoint(0, JainaPisitionLoc[2].GetPositionWithOffset({  float(rand32() % 10), float(rand32() % 10), +1.0f }));

                    me->GetMotionMaster()->MovePoint(0, JainaPisitionLoc[2]);

                    _events.ScheduleEvent(EVENT_FLIGHT_EVENT4, 6s);
                    break;
                }
                case EVENT_FLIGHT_EVENT4:
                {
                    me->SummonCreature(165983, Position(4914.29f, 7706.71f, 4825.75f, 1.57802f), TEMPSUMMON_TIMED_DESPAWN, 60s, 0U);
                    me->SummonCreature(165983, Position(4924.94f, 7715.31f, 4822.66f, 2.75612f), TEMPSUMMON_TIMED_DESPAWN, 60s, 0U);
                    me->SummonCreature(165983, Position(4915.71f, 7722.95f, 4824.98f, 4.00491f), TEMPSUMMON_TIMED_DESPAWN, 60s, 0U);
                    me->SummonCreature(165983, Position(4908.11f, 7716.91f, 4827.38f, 5.81917f), TEMPSUMMON_TIMED_DESPAWN, 60s, 0U);
                    _events.ScheduleEvent(EVENT_FLIGHT_EVENT5, 20s);
                    break;
                }
                case EVENT_FLIGHT_EVENT5:
                {
                    std::list<Creature*> cList = me->FindNearestCreatures(165862, 40.0f);
                    for (std::list<Creature*>::const_iterator itr = cList.begin(); itr != cList.end(); ++itr)
                    {
                        if (Creature* guard = *itr)
                        {
                            guard->DespawnOrUnsummon(1s, 1min);
                        }
                    }
                    std::list<Creature*> cList1 = me->FindNearestCreatures(165983, 30.0f);
                    for (std::list<Creature*>::const_iterator itr = cList1.begin(); itr != cList1.end(); ++itr)
                    {
                        if (Creature* harbinger = *itr)
                        {
                            harbinger->ForcedDespawn();
                        }
                    }
                    if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                    {
                        if (Creature* thrall = ObjectAccessor::GetCreature(*me, thrallGUID))
                        {
                            thrall->GetMotionMaster()->MovePoint(0, JainaPisitionLoc[3].GetPositionWithOffset({  float(rand32() % 10), float(rand32() % 10), +1.0f}));
                        }
                        if (Creature* darion = ObjectAccessor::GetCreature(*me, darionGUID))
                        {
                            darion->GetMotionMaster()->MovePoint(0, JainaPisitionLoc[3].GetPositionWithOffset({  float(rand32() % 10), float(rand32() % 10), +1.0f }));
                            player->GetScheduler().Schedule(Milliseconds(3000), [this, darion](TaskContext context)
                                {
                                    darion->AI()->Talk(23);
                                });
                        }
                        if (Creature* rahm = ObjectAccessor::GetCreature(*me, rahmGUID))
                            rahm->GetMotionMaster()->MovePoint(0, JainaPisitionLoc[3].GetPositionWithOffset({  float(rand32() % 10), float(rand32() % 10), +1.0f }));
                        me->GetMotionMaster()->MovePoint(0, JainaPisitionLoc[3]);
                        me->AI()->Talk(8);
                        player->GetScheduler().Schedule(Milliseconds(10000), [this](TaskContext context)
                            {
                                me->AI()->Talk(9);
                            }).Schedule(Milliseconds(16000), [this](TaskContext context)
                                {
                                    me->AI()->Talk(10);
                                });
                    }
                    _events.ScheduleEvent(EVENT_FLIGHT_EVENT6, 11s);
                    break;
                }
                case EVENT_FLIGHT_EVENT6:
                {
                    if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                    {
                        if (Creature* thrall = ObjectAccessor::GetCreature(*me, thrallGUID))
                        {
                            thrall->GetMotionMaster()->MovePoint(0, JainaPisitionLoc[4].GetPositionWithOffset({  float(rand32() % 10), float(rand32() % 10), +1.0f}));
                            player->GetScheduler().Schedule(Milliseconds(10000), [this, thrall](TaskContext context)
                                {
                                    thrall->AI()->Talk(3);
                                });
                        }
                    }
                    if (Creature* darion = ObjectAccessor::GetCreature(*me, darionGUID))
                    {
                        darion->GetMotionMaster()->MovePoint(0, JainaPisitionLoc[4].GetPositionWithOffset({  float(rand32() % 10), float(rand32() % 10), +1.0f}));
                    }
                    if (Creature* rahm = ObjectAccessor::GetCreature(*me, rahmGUID))
                        rahm->GetMotionMaster()->MovePoint(0, JainaPisitionLoc[4].GetPositionWithOffset({  float(rand32() % 10), float(rand32() % 10), +1.0f}));

                    me->GetMotionMaster()->MovePoint(0, JainaPisitionLoc[4]);

                    _events.ScheduleEvent(EVENT_FLIGHT_EVENT7, 10s);
                    break;
                }
                case EVENT_FLIGHT_EVENT7:
                {

                    me->SummonCreature(165983, Position(4942.94f, 7642.44f, 4811.86f, 2.12757f), TEMPSUMMON_TIMED_DESPAWN, 60s, 0U);
                    me->SummonCreature(165983, Position(4936.19f, 7650.21f, 4813.14f, 5.39483f), TEMPSUMMON_TIMED_DESPAWN, 60s, 0U);
                    me->SummonCreature(165983, Position(4935.23f, 7640.83f, 4811.84f, 0.80025f), TEMPSUMMON_TIMED_DESPAWN, 60s, 0U);

                    if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                    {
                        player->GetScheduler().Schedule(Milliseconds(3000), [this](TaskContext context)
                            {
                                if (Creature* harbinger = me->FindNearestCreature(165983, 15.0f))
                                {
                                    if (Creature* thrall = ObjectAccessor::GetCreature(*me, thrallGUID))
                                    {
                                        thrall->AI()->AttackStart(harbinger);
                                    }
                                    if (Creature* darion = ObjectAccessor::GetCreature(*me, darionGUID))
                                    {
                                        darion->AI()->AttackStart(harbinger);
                                    }
                                    AttackStart(harbinger);
                                }
                            });
                    }
                    _events.ScheduleEvent(EVENT_FLIGHT_EVENT8, 15s);
                    break;
                }
                case EVENT_FLIGHT_EVENT8:
                {
                    std::list<Creature*> cList1 = me->FindNearestCreatures(165983, 30.0f);
                    for (std::list<Creature*>::const_iterator itr = cList1.begin(); itr != cList1.end(); ++itr)
                    {
                        if (Creature* harbinger = *itr)
                        {
                            harbinger->ForcedDespawn();
                        }
                    }
                    if (Creature* thrall = ObjectAccessor::GetCreature(*me, thrallGUID))
                        thrall->AttackStop();

                    if (Creature* rahm = ObjectAccessor::GetCreature(*me, rahmGUID))
                        rahm->AttackStop();

                        if (Creature* darion = ObjectAccessor::GetCreature(*me, darionGUID))
                        {
                            darion->AttackStop();
                            darion->AI()->Talk(24);
                        }
                    _events.ScheduleEvent(EVENT_FLIGHT_EVENT9, 3s);
                    break;
                }
                case EVENT_FLIGHT_EVENT9:
                {
                    if (Creature* thrall = ObjectAccessor::GetCreature(*me, thrallGUID))
                        thrall->GetMotionMaster()->MovePoint(0, JainaPisitionLoc[5].GetPositionWithOffset({  float(rand32() % 10), float(rand32() % 10), +1.0f}));
          
                    if (Creature* rahm = ObjectAccessor::GetCreature(*me, rahmGUID))
                        rahm->GetMotionMaster()->MovePoint(0, JainaPisitionLoc[5].GetPositionWithOffset({  float(rand32() % 10), float(rand32() % 10), +1.0f}));
                 
                    if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                    {
                        if (Creature* darion = ObjectAccessor::GetCreature(*me, darionGUID))
                        {
                            darion->GetMotionMaster()->MovePoint(0, JainaPisitionLoc[5].GetPositionWithOffset({  float(rand32() % 10), float(rand32() % 10), +1.0f}));
                           
                            player->GetScheduler().Schedule(Milliseconds(7000), [this, darion](TaskContext context)
                                {
                                    darion->AI()->Talk(25);
                                });
                        }
                        me->GetMotionMaster()->MovePoint(0, JainaPisitionLoc[5]);
                        player->GetScheduler().Schedule(Milliseconds(3000), [this](TaskContext context)
                            {
                                me->AI()->Talk(11);
                            }).Schedule(Milliseconds(14000), [this](TaskContext context)
                                {
                                    me->AI()->Talk(12);
                                });
                    }
                    _events.ScheduleEvent(EVENT_FLIGHT_EVENT10, 18s);
                    break;
                }
                case EVENT_FLIGHT_EVENT10:
                {
                    if (Creature* thrall = ObjectAccessor::GetCreature(*me, thrallGUID))
                    {
                        thrall->GetMotionMaster()->MovePoint(0, JainaPisitionLoc[6].GetPositionWithOffset({  float(rand32() % 10), float(rand32() % 10), +1.0f}));
                    }
                    if (Creature* darion = ObjectAccessor::GetCreature(*me, darionGUID))
                    {
                        darion->GetMotionMaster()->MovePoint(0, JainaPisitionLoc[6].GetPositionWithOffset({  float(rand32() % 10), float(rand32() % 10), +1.0f}));
                    }
                    if (Creature* rahm = ObjectAccessor::GetCreature(*me, rahmGUID))
                        rahm->GetMotionMaster()->MovePoint(0, JainaPisitionLoc[6].GetPositionWithOffset({  float(rand32() % 10), float(rand32() % 10), +1.0f}));

                    me->GetMotionMaster()->MovePoint(0, JainaPisitionLoc[6]);

                    _events.ScheduleEvent(EVENT_FLIGHT_EVENT11, 10s);
                    break;
                }
                case EVENT_FLIGHT_EVENT11:
                {
                    if (Creature* tariesh = me->SummonCreature(167263, Position(4829.59f, 7590.65f, 4813.75f, 1.23369f), TEMPSUMMON_MANUAL_DESPAWN))
                    {
                        if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                        {
                            if (Creature* darion = ObjectAccessor::GetCreature(*me, darionGUID))
                            {
                                player->GetScheduler().Schedule(Milliseconds(25000), [this, darion, tariesh](TaskContext context)
                                    {
                                        darion->AI()->Talk(26);
                                        darion->AI()->AttackStart(tariesh);
                                    });
                            }
                            
                            if (Creature* thrall = ObjectAccessor::GetCreature(*me, thrallGUID))
                                {
                                    player->GetScheduler().Schedule(Milliseconds(25000), [this, thrall, tariesh](TaskContext context)
                                        {
                                            thrall->AI()->AttackStart(tariesh);
                                        });
                                }
                                
                            if (Creature* rahm = ObjectAccessor::GetCreature(*me, rahmGUID))
                                    {
                                        player->GetScheduler().Schedule(Milliseconds(25000), [this, rahm, tariesh](TaskContext context)
                                            {
                                                rahm->AI()->AttackStart(tariesh);
                                            });
                                    }

                            player->GetScheduler().Schedule(Milliseconds(3000), [this, tariesh](TaskContext context)
                                {
                                    tariesh->AI()->Talk(1);
                                }).Schedule(Milliseconds(8000), [this](TaskContext context)
                                    {
                                        me->AI()->Talk(13);
                                    }).Schedule(Milliseconds(12000), [this, tariesh](TaskContext context)
                                        {
                                            tariesh->AI()->Talk(2);
                                        }).Schedule(Milliseconds(18000), [this, tariesh](TaskContext context)
                                            {
                                                tariesh->AI()->Talk(3);
                                                tariesh->SetFaction(14);
                                                tariesh->AI()->AttackStart(me);
                                                AttackStart(tariesh);
                                            });
                        }

                    }
                    break;
                case EVENT_FLIGHT_EVENT12:
                {
                    me->AI()->Talk(14);
                    _events.ScheduleEvent(EVENT_FLIGHT_EVENT13, 3s);
                    break;
                }
                case EVENT_FLIGHT_EVENT13:
                {
                    if (Creature* thrall = ObjectAccessor::GetCreature(*me, thrallGUID))
                        thrall->GetMotionMaster()->MovePoint(0, JainaPisitionLoc[7].GetPositionWithOffset({  float(rand32() % 10), float(rand32() % 10), +1.0f}));
             
                    if (Creature* darion = ObjectAccessor::GetCreature(*me, darionGUID))
                        darion->GetMotionMaster()->MovePoint(0, JainaPisitionLoc[7].GetPositionWithOffset({  float(rand32() % 10), float(rand32() % 10), +1.0f}));
          
                    if (Creature* rahm = ObjectAccessor::GetCreature(*me, rahmGUID))
                        rahm->GetMotionMaster()->MovePoint(0, JainaPisitionLoc[7].GetPositionWithOffset({  float(rand32() % 10), float(rand32() % 10), +1.0f}));
                
                    me->GetMotionMaster()->MovePoint(0, JainaPisitionLoc[7]);
                    _events.ScheduleEvent(EVENT_FLIGHT_EVENT14, 5s);
                    break;
                }
                case EVENT_FLIGHT_EVENT14:
                {
                    if (Creature* thrall = ObjectAccessor::GetCreature(*me, thrallGUID))
                        thrall->GetMotionMaster()->MovePoint(0, JainaPisitionLoc[8].GetPositionWithOffset({ float(rand32() % 3), float(rand32() % 3) }));
        
                    if (Creature* darion = ObjectAccessor::GetCreature(*me, darionGUID))
                        darion->GetMotionMaster()->MovePoint(0, JainaPisitionLoc[8].GetPositionWithOffset({ float(rand32() % 3), float(rand32() % 3) }));

                    if (Creature* rahm = ObjectAccessor::GetCreature(*me, rahmGUID))
                        rahm->GetMotionMaster()->MovePoint(0, JainaPisitionLoc[8].GetPositionWithOffset({ float(rand32() % 3), float(rand32() %3) }));

                    me->GetMotionMaster()->MovePoint(0, JainaPisitionLoc[8]);
                    _events.ScheduleEvent(EVENT_FLIGHT_EVENT15, 5s);
                    break;
                }
                case EVENT_FLIGHT_EVENT15:
                {
                    if (Creature* thrall = ObjectAccessor::GetCreature(*me, thrallGUID))
                        thrall->GetMotionMaster()->MovePoint(0, JainaPisitionLoc[9].GetPositionWithOffset({ float(rand32() % 3), float(rand32() % 3) }));
             
                    if (Creature* darion = ObjectAccessor::GetCreature(*me, darionGUID))
                        darion->GetMotionMaster()->MovePoint(0, JainaPisitionLoc[9].GetPositionWithOffset({ float(rand32() % 3), float(rand32() % 3) }));

                    if (Creature* rahm = ObjectAccessor::GetCreature(*me, rahmGUID))
                        rahm->GetMotionMaster()->MovePoint(0, JainaPisitionLoc[9].GetPositionWithOffset({ float(rand32() % 3), float(rand32() % 3) }));

                    me->GetMotionMaster()->MovePoint(0, JainaPisitionLoc[9]);
                    _events.ScheduleEvent(EVENT_FLIGHT_EVENT16, 5s);
                    break;
                }
                case EVENT_FLIGHT_EVENT16:
                {
                    if (Creature* thrall = ObjectAccessor::GetCreature(*me, thrallGUID))
                        thrall->GetMotionMaster()->MovePoint(0, JainaPisitionLoc[10]);
          
                    if (Creature* darion = ObjectAccessor::GetCreature(*me, darionGUID))
                        darion->GetMotionMaster()->MovePoint(0, JainaPisitionLoc[10]);
            
                    if (Creature* rahm = ObjectAccessor::GetCreature(*me, rahmGUID))
                        rahm->GetMotionMaster()->MovePoint(0, JainaPisitionLoc[10]);
               
                    me->GetMotionMaster()->MovePoint(0, JainaPisitionLoc[10]);
                    _events.ScheduleEvent(EVENT_FLIGHT_EVENT17, 4s);
                    break;
                }
                case EVENT_FLIGHT_EVENT17:
                {
                    if (Creature* thrall = ObjectAccessor::GetCreature(*me, thrallGUID))
                        thrall->GetMotionMaster()->MovePoint(0, JainaPisitionLoc[11]);

                    if (Creature* darion = ObjectAccessor::GetCreature(*me, darionGUID))
                        darion->GetMotionMaster()->MovePoint(0, JainaPisitionLoc[11]);

                    if (Creature* rahm = ObjectAccessor::GetCreature(*me, rahmGUID))
                        rahm->GetMotionMaster()->MovePoint(0, JainaPisitionLoc[11]);

                    me->GetMotionMaster()->MovePoint(0, JainaPisitionLoc[11]);
                    _events.ScheduleEvent(EVENT_FLIGHT_EVENT18, 5s);
                    break;
                }
                case EVENT_FLIGHT_EVENT18:
                {
                    if (Creature* thrall = ObjectAccessor::GetCreature(*me, thrallGUID))
                    {
                        thrall->SetWalk(true);
                        thrall->GetMotionMaster()->MovePoint(0, JainaPisitionLoc[13]);
                    }
                    if (Creature* darion = ObjectAccessor::GetCreature(*me, darionGUID))
                    {
                        darion->SetWalk(true);
                        darion->GetMotionMaster()->MovePoint(0, JainaPisitionLoc[14]);
                    }
                    if (Creature* rahm = ObjectAccessor::GetCreature(*me, rahmGUID))
                    {
                        rahm->SetWalk(true);
                        rahm->GetMotionMaster()->MovePoint(0, JainaPisitionLoc[15]);
                    }

                    if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                    {
                        me->SetWalk(true);
                        me->GetMotionMaster()->MovePoint(0, JainaPisitionLoc[12]);
                        me->AI()->Talk(15);
                        player->ForceCompleteQuest(QUEST_A_FLIGHT_FROM_DARKNESS);
                        player->GetScheduler().Schedule(Milliseconds(4000), [this](TaskContext context)
                            {
                                me->AI()->Talk(16);
                            }).Schedule(Milliseconds(12000), [this](TaskContext context)
                                {
                                    me->AI()->Talk(17);
                                });
                    }
                    break;
                }
             
                case EVENT_MOMENT_EVENT1:                           //Start Event Quest59756 A Moment's Respite
                {
                    me->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
                    me->AI()->Talk(18);
                    me->SetWalk(true);
                    _events.ScheduleEvent(EVENT_MOMENT_EVENT2, 3s);
                    break;
                }
                case EVENT_MOMENT_EVENT2:                         
                {
                    me->AI()->Talk(19);
                    me->GetMotionMaster()->MovePoint(0, JainaPisitionQ59756[0]);
                    _events.ScheduleEvent(EVENT_MOMENT_EVENT3, 10s);
                    break;
                }
                case EVENT_MOMENT_EVENT3:                          
                {
                    if (Creature* thrall = ObjectAccessor::GetCreature(*me, thrallGUID))
                    {
                        thrall->AI()->Talk(4);
                    }
                    _events.ScheduleEvent(EVENT_MOMENT_EVENT4, 8s);
                    break;
                }
                case EVENT_MOMENT_EVENT4:
                {
                    me->AI()->Talk(20);
                    me->GetMotionMaster()->MovePoint(0, JainaPisitionQ59756[1],true, 4.413332f);
                    _events.ScheduleEvent(EVENT_MOMENT_EVENT5, 8s);
                    break;
                }
                case EVENT_MOMENT_EVENT5:
                {
                    me->CastSpell(me, 329892);
                    _events.ScheduleEvent(EVENT_MOMENT_EVENT6, 2s);
                    break;
                }
                case EVENT_MOMENT_EVENT6:
                {
                    if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                    {
                        player->KilledMonsterCredit(166980);;
                    }
                    Q59756FirstCreatureSummon();
                    me->RemoveAura(329892);
                    me->AI()->Talk(21);
                    me->SetNpcFlag(UNIT_NPC_FLAG_GOSSIP);
                    break;
                }
                case EVENT_MOMENT_EVENT7:
                {
                    me->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
                    me->AI()->Talk(22);
                    _events.ScheduleEvent(EVENT_MOMENT_EVENT8, 7s);
                    break;
                }
                case EVENT_MOMENT_EVENT8:
                {
                    me->AI()->Talk(23);
                    me->GetMotionMaster()->MovePoint(0, JainaPisitionQ59756[2], true, 2.13879f);
                    _events.ScheduleEvent(EVENT_MOMENT_EVENT9, 5s);
                    break;
                }
                case EVENT_MOMENT_EVENT9:
                {
                    me->CastSpell(me, 329892);
                    _events.ScheduleEvent(EVENT_MOMENT_EVENT10, 2s);
                    break;
                }
                case EVENT_MOMENT_EVENT10:
                {
                    Q59756SecundCreatureSummon();
                    me->RemoveAura(329892);
                    if (Creature* thrall = ObjectAccessor::GetCreature(*me, thrallGUID))
                    {
                        thrall->AI()->Talk(5);
                    }
                    _events.ScheduleEvent(EVENT_MOMENT_EVENT11, 12s);
                    break;
                }
                case EVENT_MOMENT_EVENT11:
                {
                    if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                    {
                        player->KilledMonsterCredit(170173);;
                    }
                    me->AI()->Talk(24);
                    me->SetNpcFlag(UNIT_NPC_FLAG_GOSSIP);
                    break;
                }
                case EVENT_MOMENT_EVENT12:
                {
                    me->AI()->Talk(26);
                    me->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
                    _events.ScheduleEvent(EVENT_MOMENT_EVENT13, 9s);
                    break;
                }
                case EVENT_MOMENT_EVENT13:
                {
                    if (Creature* darion = ObjectAccessor::GetCreature(*me, darionGUID))
                    {
                        darion->SetWalk(true);
                        darion->GetMotionMaster()->MovePoint(0, JainaPisitionQ59756[4], true);
                        darion->AI()->Talk(27);
                    }
                    _events.ScheduleEvent(EVENT_MOMENT_EVENT14, 10s);
                    break;
                }
                case EVENT_MOMENT_EVENT14:
                {
                    me->AI()->Talk(27);
                    me->GetMotionMaster()->MovePoint(0, JainaPisitionQ59756[3], true, 4.40691f);
                    if (Creature* darion = ObjectAccessor::GetCreature(*me, darionGUID))
                    {
                        darion->GetMotionMaster()->MovePoint(0, JainaPisitionQ59756[5], true);
                    }
                    _events.ScheduleEvent(EVENT_MOMENT_EVENT15, 10s);
                    break;
                }
                case EVENT_MOMENT_EVENT15:
                {
                    me->CastSpell(me, 329892);
                    _events.ScheduleEvent(EVENT_MOMENT_EVENT16, 2s);
                    break;
                }
                case EVENT_MOMENT_EVENT16:
                {
                    Q59756ThirdCreatureSummon();
                    me->RemoveAura(329892);
                    me->AI()->Talk(28);
                    _events.ScheduleEvent(EVENT_MOMENT_EVENT17, 8s);
                    break;
                }
                case EVENT_MOMENT_EVENT17:
                {
                    if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                    {
                        player->KilledMonsterCredit(170174);;
                    }
                    if (Creature* darion = ObjectAccessor::GetCreature(*me, darionGUID))
                    {
                        darion->AI()->Talk(28);
                    }
                    _events.ScheduleEvent(EVENT_MOMENT_EVENT18, 9s);
                    break;
                }
                case EVENT_MOMENT_EVENT18:
                {
                    me->GetMotionMaster()->MovePoint(0, JainaPisitionQ59756[6], true, 0.174609f);
                    me->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
                    if (Creature* darion = ObjectAccessor::GetCreature(*me, darionGUID))
                    {
                        darion->GetMotionMaster()->MovePoint(0, JainaPisitionQ59756[7], true, 1.91819f);
                    }
                    break;
                }
                //Quest 59767 The Path to Salvation
                case EVENT_SALVATION_PHASE1:
                    me->GetMotionMaster()->MovePoint(0, JainaPosition59767[0], true);
                    if (Creature* thrall = ObjectAccessor::GetCreature(*me, thrallGUID))
                    {
                        thrall->GetMotionMaster()->MoveFollow(me, 1.0f, 90);
                    }
                    if (Creature* anduin = ObjectAccessor::GetCreature(*me, anduinGUID))
                    {
                        anduin->GetMotionMaster()->MoveFollow(me, 1.0f, 180);
                    }
                    if (Creature* baine = ObjectAccessor::GetCreature(*me, baineGUID))
                    {
                        baine->GetMotionMaster()->MoveFollow(me, 1.0f, 270);
                        baine->AI()->Talk(3);
                    }
                    _events.ScheduleEvent(EVENT_SALVATION_PHASE2, 5s);
                    break;

                case EVENT_SALVATION_PHASE2:
                    me->GetMotionMaster()->MovePoint(0, JainaPosition59767[1], true);
                    if (Creature* thrall = ObjectAccessor::GetCreature(*me, thrallGUID))
                    {
                        thrall->GetMotionMaster()->MoveFollow(me, 1.0f, 90);
                    }
                    if (Creature* anduin = ObjectAccessor::GetCreature(*me, anduinGUID))
                    {
                        anduin->GetMotionMaster()->MoveFollow(me, 1.0f, 180);
                    }
                    if (Creature* baine = ObjectAccessor::GetCreature(*me, baineGUID))
                    {
                        baine->GetMotionMaster()->MoveFollow(me, 1.0f, 270);
                        me->AddDelayedEvent(2000, [this, baine]()
                            {
                                baine->AI()->Talk(4);
                            });
                    }
                    _events.ScheduleEvent(EVENT_SALVATION_PHASE3, 8s);
                    break;

                case EVENT_SALVATION_PHASE3:
                    me->GetMotionMaster()->MovePoint(0, JainaPosition59767[2], true);
                    if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                    {
                        me->AddDelayedEvent(3000, [this, player]()
                            {
                                me->AI()->Talk(43);
                                player->KilledMonsterCredit(168346);
                            });
                    }
                    me->ForcedDespawn(12000, 60s);
                    if (Creature* thrall = ObjectAccessor::GetCreature(*me, thrallGUID))
                    {
                        thrall->GetMotionMaster()->MovePoint(0, JainaPosition59767[3], true);
                        thrall->ForcedDespawn(12000, 60s);
                    }
                    if (Creature* anduin = ObjectAccessor::GetCreature(*me, anduinGUID))
                    {
                        anduin->GetMotionMaster()->MovePoint(0, JainaPosition59767[4], true);
                        anduin->ForcedDespawn(12000, 60s);
                    }
                    if (Creature* baine = ObjectAccessor::GetCreature(*me, baineGUID))
                    {
                        baine->GetMotionMaster()->MovePoint(0, JainaPosition59767[5], true);
                        baine->ForcedDespawn(12000, 60s);
                    }
                    break;
                }
                }
            }
        }

    private: 
        ObjectGuid thrallGUID;
        ObjectGuid darionGUID;
        ObjectGuid anduinGUID;
        ObjectGuid baineGUID;
        ObjectGuid rahmGUID;
        ObjectGuid m_playerGUID;
        EventMap _events;
        bool darion;
        bool tariesh;
        bool binger5;
        bool binger6;
        bool say59759;
    };
};

//177071_on_blackened_wings_q_59754
struct npc_mawsworn_harbinger_177071 : public ScriptedAI
{
    npc_mawsworn_harbinger_177071(Creature* creature) : ScriptedAI(creature) { Reset(); }

private:
    uint32 waitTime;
    ObjectGuid BladeTarget;
    ObjectGuid EBladeGUID[ENCOUNTER_EBLADE_NUMBER2];

    void Reset() override
    {
        BladeTarget = ObjectGuid::Empty;
        waitTime = urand(0, 2000);
        me->SetSheath(SHEATH_STATE_MELEE);
        me->SetHoverHeight(2.0f);
        
        for (uint8 i = 0; i < ENCOUNTER_EBLADE_NUMBER2; ++i)
        {
            if (Creature* temp = ObjectAccessor::GetCreature(*me, EBladeGUID[i]))
                temp->setDeathState(JUST_DIED);
            EBladeGUID[i].Clear();
        }
    }

    void DamageTaken(Unit* target, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/) override
    {
        if (target->ToCreature())
            if (me->GetHealth() <= damage || me->GetHealthPct() <= 90.0f)
                damage = 0;

        if (Player* player = target->ToPlayer())
            if (me->GetHealthPct() <= 90.0f)
                AttackStart(player);
    }

    void DamageDealt(Unit* target, uint32& damage, DamageEffectType /*damageType*/) override
    {
        if (target->ToCreature())
            if (target->GetHealth() <= damage || target->GetHealthPct() <= 80.0f)
                damage = 0;
    }

    void JustDied(Unit* killer) override
    {
        if (Player* player = killer->ToPlayer())
        {
            std::list<Creature*> cList = me->FindNearestCreatures(165862, 5.0f);
            for (std::list<Creature*>::const_iterator itr = cList.begin(); itr != cList.end(); ++itr)
            {
                if (Creature* ebon = *itr)
                {
                    ebon->ForcedDespawn(1000);
                }
            }
        }
    }


    void SpawnNPC()
    {
        Unit* temp = nullptr;

        for (uint8 i = 0; i < ENCOUNTER_EBLADE_NUMBER2; ++i)
        {
            temp = ObjectAccessor::GetCreature(*me, EBladeGUID[i]);
            if (!temp)
            {
                temp = me->SummonCreature(165862, me->GetPosition().GetPositionWithOffset({ float(rand32() % 3), float(rand32() % 3) }), TEMPSUMMON_MANUAL_DESPAWN);
                temp->SetFaction(3222);
                EBladeGUID[i] = temp->GetGUID();
            }
        }
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

        if (!BladeTarget.IsEmpty())
        {
            if (Creature* ebon = ObjectAccessor::GetCreature(*me, BladeTarget))
            {

                if (ebon->IsAlive())
                {
                    if (me->GetVictim() != ebon)
                    {
                        me->Attack(ebon, true);
                    }
                }
                else
                {
                    ebon->DespawnOrUnsummon();
                    BladeTarget = ObjectGuid::Empty;
                }
            }
        }
        else
        {
            SpawnNPC();
            std::list<Creature*> cList = me->FindNearestCreatures(165862, 5.0f);
            for (std::list<Creature*>::const_iterator itr = cList.begin(); itr != cList.end(); ++itr)
            {
                if (Creature* ebon = *itr)
                {
                    AttackStart(ebon);
                    ebon->SetFacingToObject(me);
                    BladeTarget = ebon->GetGUID();
                }
            }
        }
    }
};

//166981_npc_thrall
class npc_thrall_166981 : public CreatureScript
{
public:
    npc_thrall_166981() : CreatureScript("npc_thrall_166981") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_thrall_166981AI(creature);
    }
    struct npc_thrall_166981AI : public EscortAI
    {
        npc_thrall_166981AI(Creature* creature) : EscortAI(creature) { Reset(); }

    private:
        uint32 waitTime;
        ObjectGuid guardTarget;
        ObjectGuid thrallGUID;
        ObjectGuid m_playerGUID;
        bool say59759_1;
        bool say59759_2;
        bool say59765;
        bool say59766_1;
        bool say59766_2;
        bool say59766_3;
        bool say59766_4;
        bool say59766_5;

        void Reset() override
        {
            thrallGUID.Clear();
            m_playerGUID.Clear();
            guardTarget = ObjectGuid::Empty;
            say59759_1 = false;
            say59759_2 = false;
            say59765 = false;
            say59766_1 = false;
            say59766_2 = false;
            say59766_3 = false;
            say59766_4 = false;
            say59766_5 = false;

            waitTime = urand(0, 2000);
            me->SetSheath(SHEATH_STATE_MELEE);
            me->RemoveUnitFlag(UNIT_FLAG_IMMUNE_TO_NPC);
            me->SetReactState(REACT_AGGRESSIVE);
        }

        void OnQuestAccept(Player* player, Quest const* quest) override
        {
            if (quest->GetQuestId() == QUEST_WOUNDS_BEYOND_FLESH || quest->GetQuestId() == QUEST_A_GOOD_AXE)
            {
                if (player->SummonCreature(166981, *me, TEMPSUMMON_TIMED_DESPAWN, 900s, 0))
                {
                    me->ForcedDespawn(1000, 180s);
                }
            }

            if (quest->GetQuestId() == QUEST_DRAW_OUT_THE_DARKNESS)
            {
                me->GetMotionMaster()->MovePoint(0, 4524.14f, 7445.41f, 4794.16f, true);
                me->AddDelayedEvent(6000, [this, player]()
                    {
                        if (player->GetPhaseShift().HasPhase(10041)) PhasingHandler::RemovePhase(player, 10041, true);
                        if (!player->GetPhaseShift().HasPhase(10042)) PhasingHandler::AddPhase(player, 10042, true);
                        me->ForcedDespawn(1000);
                    });

            }
        }

        void IsSummonedBy(WorldObject* unit) override
        {
            if (Player* player = unit->ToPlayer())
            {
                me->SetOwnerGUID(player->GetGUID());  //set player onwer
                me->GetMotionMaster()->MoveFollow(player, PET_FOLLOW_DIST, 60.f);
                if (int32 playerFaction = player->GetFaction())
                    me->SetFaction(playerFaction);

                if (int32 playerAttackPower = player->GetTotalAttackPowerValue(BASE_ATTACK) * 2.0f)
                    me->SetAttackPower(playerAttackPower);
            }
        }

        void DamageTaken(Unit* target, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/) override
        {
            if (target->ToCreature())
                if (me->GetHealth() <= damage || me->GetHealthPct() <= 80.0f)
                    damage = 0;
        }

        void MoveInLineOfSight(Unit* target) override
        {
            if (Player* player = target->ToPlayer())
            {
                if (target->IsInDist(me, 10.0f))
                {
                    if (player->GetQuestStatus(QUEST_WOUNDS_BEYOND_FLESH) == QUEST_STATUS_COMPLETE)
                    {
                        if (!say59765)
                        {
                            me->AI()->Talk(12);
                            say59765 = true;
                        }
                    }

                    if (player->GetQuestObjectiveProgress(QUEST_A_GOOD_AXE, 0) == 1)
                        if (!say59766_1)
                        {

                            me->AI()->Talk(13);
                            me->AddDelayedEvent(6000, [this]()
                                {
                                    me->AI()->Talk(14);
                                });

                            say59766_1 = true;
                        }

                    if (player->GetQuestObjectiveProgress(QUEST_A_GOOD_AXE, 0) == 2)
                        if (!say59766_2)
                        {

                            me->AI()->Talk(15);
                            me->AddDelayedEvent(7000, [this]()
                                {
                                    me->AI()->Talk(16);
                                });

                            say59766_2 = true;
                        }
                    if (player->GetQuestObjectiveProgress(QUEST_A_GOOD_AXE, 0) == 3)
                        if (!say59766_3)
                        {

                            me->AI()->Talk(17);
                            me->AddDelayedEvent(9000, [this]()
                                {
                                    me->AI()->Talk(18);
                                });

                            say59766_3 = true;
                        }

                    if (player->GetQuestObjectiveProgress(QUEST_A_GOOD_AXE, 0) == 4)
                        if (!say59766_4)
                        {

                            me->AI()->Talk(19);
                            me->AddDelayedEvent(4000, [this]()
                                {
                                    me->AI()->Talk(20);
                                });
                            say59766_4 = true;
                        }

                    if (player->GetQuestObjectiveProgress(QUEST_A_GOOD_AXE, 0) == 5)
                        if (!say59766_5)
                        {
                            player->KilledMonsterCredit(169602);
                            me->AI()->Talk(21);
                            me->AddDelayedEvent(5000, [this]()
                                {
                                    me->AI()->Talk(22);
                                });

                            say59766_5 = true;
                        }
                }
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

                    if (creature->GetEntry() == 167971)      // Use another npc to talk on the bridge
                    {
                        if (Creature* jaina = me->FindNearestCreature(166980, 30.0f))
                        {
                            if (!say59759_1)
                            {
                                me->AI()->Talk(6);
                                me->AddDelayedEvent(7000, [this, jaina]()
                                    {
                                        jaina->AI()->Talk(31);
                                    });
                            }
                            say59759_1 = true;
                        }

                    }
                }
                if (creature->IsInDist(me, 30.0f))
                {
                    if (creature->GetEntry() == 167961)      // Use npc to talk on Tremaculum Portal
                    {
                        if (Creature* jaina = me->FindNearestCreature(166980, 30.0f))
                        {
                            if (!say59759_2)
                            {
                                jaina->AI()->Talk(32);
                                me->AddDelayedEvent(3000, [this, jaina]()
                                    {
                                        jaina->AI()->Talk(33);
                                    });

                            }
                            say59759_2 = true;
                            say59759_1 = false;
                        }
                    }
                }
            }
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

            if (me->GetPhaseShift().HasPhase(10031))
                if (!guardTarget.IsEmpty())
                {
                    if (Creature* guard = ObjectAccessor::GetCreature(*me, guardTarget))
                    {
                        if (guard->IsAlive())
                        {
                            if (me->GetVictim() != guard)
                            {
                                me->GetThreatManager().AddThreat(guard, 1000000.0f);
                                guard->GetThreatManager().AddThreat(me, 1000000.0f);
                                me->Attack(guard, true);
                            }
                        }
                        else
                        {
                            guard->DespawnOrUnsummon();
                            guardTarget = ObjectGuid::Empty;
                        }

                    }
                }
                else
                {
                    Position guardPos = me->GetPosition();
                    GetPositionWithDistInFront(me, 2.5f, guardPos);

                    float z = me->GetMap()->GetHeight(me->GetPhaseShift(), guardPos.GetPositionX(), guardPos.GetPositionY(), guardPos.GetPositionZ());
                    guardPos.m_positionZ = z;

                    if (Creature* guard = me->SummonCreature(177071, guardPos))
                    {
                        me->GetThreatManager().AddThreat(guard, 1000000.0f);
                        guard->GetThreatManager().AddThreat(me, 1000000.0f);
                        AttackStart(guard);
                        guard->SetFacingToObject(me);
                        guardTarget = guard->GetGUID();
                    }
                }
        }

    };
};


//165909 Malice Shadow
struct npc_Malice_Shadow_165909 : public ScriptedAI
{
    npc_Malice_Shadow_165909(Creature* creature) : ScriptedAI(creature) { }

    void DamageTaken(Unit* target, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/) override
    {
        if (Player* player = target->ToPlayer())
            if (player->HasQuest(QUEST_FIELD_SEANCE))
            {
                if (me->GetHealth() <= damage || me->GetHealthPct() <= 40.0f)
                {
                    damage = 0;
                    me->SetFaction(35);
                }
            }
    }

    void SpellHit(WorldObject* caster, SpellInfo const* spell) override
    {
        if (spell->Id != 326267)
            return;

        me->AI()->Talk(0);
    }

};

//165976 Tormented Amalgamation
struct npc_Tormented_Amalgamation_165976 : public ScriptedAI
{
    npc_Tormented_Amalgamation_165976(Creature* creature) : ScriptedAI(creature) { Reset(); }

    bool say;

    void Reset() override
    {
        say = false;
    }

    void DamageTaken(Unit* target, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/) override
    {
        if (Player* player = target->ToPlayer())
            if (player->HasQuest(QUEST_SPEAKING_TO_THE_DEAD))
            {
                if (me->GetHealth() <= damage || me->GetHealthPct() <= 20.0f)
                {
                    damage = 0;
                    me->SetFaction(35);

                    std::list<Creature*> cList = me->FindNearestCreatures(165862, 20.0f);
                    for (std::list<Creature*>::const_iterator itr = cList.begin(); itr != cList.end(); ++itr)
                    {
                        if (Creature* blade = *itr)
                        {
                            blade->ForcedDespawn(1000);
                        }
                    }

                    if (Creature* darion = me->FindNearestCreature(NPC_HIGHLORD_DARION, 30.0f))
                    {
                        if (!say)
                        {
                            darion->AI()->Talk(33);
                            say = true;
                        }
                    }
                }
            }
    }

    void SpellHit(WorldObject* caster, SpellInfo const* spell) override
    {
        if (spell->Id != 345181)
            return;

        me->AI()->Talk(0);
        me->AddDelayedEvent(5000, [this]()
            {
                me->CastSpell(me, 231409);     //Enslaved Aura
                me->CastSpell(me, 326320);     //Enslaved Aura
                me->AI()->Talk(1);
            });
    }

};

// item 178495 Shattered Helm of Domination
class item_Shattered_Helm_of_Domination : public ItemScript
{
public:
    item_Shattered_Helm_of_Domination() : ItemScript("item_Shattered_Helm_of_Domination") { }

    bool OnUse(Player* player, Item* item, SpellCastTargets const& targets, ObjectGuid /*castId*/) override
    {
        if (player->HasQuest(QUEST_FIELD_SEANCE))
        {
                if (Unit* target = player->GetSelectedUnit())
                    if (target->GetEntry() == 165909)
                    {
                        player->CastSpell(target, 326267);
                        target->CastSpell(target, 326388);
                        player->GetScheduler().Schedule(Milliseconds(4000), [this, target, player](TaskContext context)
                            {
                                player->CastStop();
                                if (target->GetFaction() == 35)
                                {
                                    player->GetSelectedUnit()->ToCreature()->ForcedDespawn(1000, 60s);
                                    player->KilledMonsterCredit(168009);
                                }
                            });
                    }
        }
        return true;
    }
};

// item 184313 Shattered Helm of Domination 2
class item_Shattered_Helm_of_Domination_2 : public ItemScript
{
public:
    item_Shattered_Helm_of_Domination_2() : ItemScript("item_Shattered_Helm_of_Domination_2") { }

    bool OnUse(Player* player, Item* item, SpellCastTargets const& targets, ObjectGuid /*castId*/) override
    {
        if (player->HasQuest(QUEST_SPEAKING_TO_THE_DEAD))
        {
            if (Unit* target = player->GetSelectedUnit())
                if (target->GetEntry() == 165976)
                {
                    player->CastSpell(target, 345181);
                    target->CastSpell(target, 326388);
                    player->GetScheduler().Schedule(Milliseconds(4000), [this, target, player](TaskContext context)
                        {
                            player->CastStop();
                            if (target->GetFaction() == 35)
                            {
                                player->KilledMonsterCredit(165976);
                            }
                        });
                }
        }
        return true;
    }
};


//326260
/* struct spell_dominating : public SpellScript
{
    PrepareSpellScript(spell_dominating);

    void HandleAfterCast()
    {
        if (!GetCaster())
            return;

        if (Player* player = GetCaster()->ToPlayer())
            if (player->GetQuestStatus(QUEST_FIELD_SEANCE) == QUEST_STATUS_INCOMPLETE)
                if (Unit* GetTarget = player->GetSelectedUnit())                    
                    if (Creature* creTarget = GetTarget->ToCreature())
                        if (creTarget->GetEntry() == 165909)
                        {
                            if (creTarget->GetFaction() == 35)
                            {
                                creTarget->ForcedDespawn(1000);
                                player->KilledMonsterCredit(168009);
                            }
                        }
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_dominating::HandleAfterCast);
    }
};*/

//167834_Phael_the_Afflictor_q59760
struct npc_Phael_the_Afflictor_167834 : public ScriptedAI
{
    npc_Phael_the_Afflictor_167834(Creature* creature) : ScriptedAI(creature) { }

    void JustEngagedWith(Unit* victim) override
    {
       me->AI()->Talk(0);
    }

    void JustDied(Unit* killer) override
    {
        me->AI()->Talk(1);
    }
};

//168130_Fear_to_Tread_q59914
struct npc_thelonia_the_cruel : public ScriptedAI
{
    npc_thelonia_the_cruel(Creature* creature) : ScriptedAI(creature) { }

private:
    bool say;

    void Reset() override
    {
        say = false;
    }

    void MoveInLineOfSight(Unit* target) override
    {
        if (!target->IsPlayer())
            return;

        if (Player* player = target->ToPlayer())
        {
            if (player->GetDistance2d(me) <= 35.0f)
                if (Creature* darion = GetClosestCreatureWithEntry(player, NPC_HIGHLORD_DARION, 30.0f))
                    if (!say)
                    {
                        say = true;
                        darion->AI()->Talk(15);
                        player->GetScheduler().Schedule(6s, [this, darion, player](TaskContext context)
                            {
                                darion->AI()->Talk(16);
                            }).Schedule(9s, [this, darion, player](TaskContext context)
                                {
                                    me->AI()->Talk(0);
                                });
                    }
            if (!player->IsWithinDistInMap(me, 80.0f) && say)
                if (player->GetQuestStatus(QUEST_FEAR_TO_TREAD) == QUEST_STATUS_INCOMPLETE)
                    say = false;
        }
    }
};

//171100_Fear_to_Tread_q59914
struct npc_serath_the_gluttonous : public ScriptedAI
{
    npc_serath_the_gluttonous(Creature* creature) : ScriptedAI(creature) { }

private:
    bool say;

    void Reset() override
    {
        say = false;
    }

    void MoveInLineOfSight(Unit* target) override
    {
       
        if (Player* player = target->ToPlayer())
        {
            if (target->GetDistance2d(me) <= 30.0f)
                if (Creature* darion = GetClosestCreatureWithEntry(player, NPC_HIGHLORD_DARION, 30.0f))
                    if (!say)
                    {
                        say = true;
                        darion->AI()->Talk(17);
                        player->GetScheduler().Schedule(6s, [this, darion, player](TaskContext context)
                            {
                                darion->AI()->Talk(18);
                            });
                    }
            if (!player->IsWithinDistInMap(me, 50.0f) && say)
                if (player->GetQuestStatus(QUEST_FEAR_TO_TREAD) == QUEST_STATUS_INCOMPLETE)
                    say = false;
        }
    }
};

//165992_Fear_to_Tread_q59914
struct npc_mawsworn_soulrender : public ScriptedAI
{
    npc_mawsworn_soulrender(Creature* creature) : ScriptedAI(creature) { }

private:
    bool say;

    void Reset() override
    {
        say = false;
    }

    void MoveInLineOfSight(Unit* target) override
    {
        if (!target->IsPlayer())
            return;

       
        if (Player* player = target->ToPlayer())
        {
            if (player->GetDistance2d(me) <= 20.0f)
                if (Creature* darion = GetClosestCreatureWithEntry(player, NPC_HIGHLORD_DARION, 30.0f))
                    if (!say)
                    {
                        say = true;
                        darion->AI()->Talk(19);
                        player->GetScheduler().Schedule(7s, [this, darion, player](TaskContext context)
                            {
                                darion->AI()->Talk(20);
                            });
                    }
            if (!player->IsWithinDistInMap(me, 30.0f) && say)
                if (player->GetQuestStatus(QUEST_FEAR_TO_TREAD) == QUEST_STATUS_INCOMPLETE)
                    say = false;
        }
    }
};

//167961
struct npc_dnt_credit_tremaculum : public ScriptedAI
{
    npc_dnt_credit_tremaculum(Creature* creature) : ScriptedAI(creature) { Reset(); }

    bool summonTJ;

    void Reset() override
    {
        summonTJ = false;
    }

    void MoveInLineOfSight(Unit* target) override
    {
        if (!target->IsPlayer())
            return;

        if (target->GetDistance2d(me) <= 2.0f)
            if (Player* player = target->ToPlayer())
                if (player->GetQuestStatus(QUEST_THE_LIONS_CAGE) == QUEST_STATUS_INCOMPLETE)
                    if (!player->GetQuestObjectiveProgress(QUEST_THE_LIONS_CAGE, 1))
                    {
                        player->KilledMonsterCredit(167961);
                        player->NearTeleportTo(5214.227f, 7500.833f, 4895.500f, 0.741f, false);
                        if (Creature* jaina = me->FindNearestCreature(166980, 30.0f))
                            jaina->ForcedDespawn();
                        if (Creature* thrall = me->FindNearestCreature(166981, 30.0f))
                            thrall->ForcedDespawn();
                    }

        if (target->GetDistance2d(me) <= 2.0f)
            if (Player* player = target->ToPlayer())
                if (player->GetQuestObjectiveProgress(QUEST_THE_AFFLICTORS_KEY, 0) || player->GetQuestStatus(QUEST_AN_UNDESERVED_FATE) == QUEST_STATUS_COMPLETE)
                {
                    if (Creature* jaina = me->FindNearestCreature(167154, 30.0f))
                        jaina->ForcedDespawn();
                    if (Creature* thrall = me->FindNearestCreature(167176, 30.0f))
                        thrall->ForcedDespawn();
                    player->NearTeleportTo(5214.227f, 7500.833f, 4895.500f, 0.741f, false);
                }

        if (target->GetDistance2d(me) <= 20.0f)
            if (Player* player = target->ToPlayer())
                if (player->GetQuestStatus(QUEST_THE_AFFLICTORS_KEY) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(QUEST_AN_UNDESERVED_FATE) == QUEST_STATUS_INCOMPLETE)
                {
                    if (!player->GetPhaseShift().HasPhase(10040)) PhasingHandler::AddPhase(player, 10040, true);
                    if (!summonTJ)
                    {
                        if (Creature* thrall = GetClosestCreatureWithEntry(me, 167176, 20.0f))
                            if (Creature* jaina = GetClosestCreatureWithEntry(me, 167154, 20.0f))
                            {
                                thrall->ForcedDespawn();
                                jaina->ForcedDespawn();
                            }
                        me->AddDelayedEvent(500, [this, player]()
                            {
                                player->SummonCreature(167154, Position(5260.91f, 7529.63f, 4790.16f, 5.1908f), TEMPSUMMON_TIMED_DESPAWN, 600s);
                                player->SummonCreature(167176, Position(5260.91f, 7529.63f, 4790.16f, 5.1908f), TEMPSUMMON_TIMED_DESPAWN, 600s);
                            });
                    }
                    summonTJ = true;
                }
    }
};

//178108_Use as a portal
struct npc_Tremaculum_Enemy_178108 : public ScriptedAI
{
    npc_Tremaculum_Enemy_178108(Creature* creature) : ScriptedAI(creature) { }

    void MoveInLineOfSight(Unit* target) override
    {
        if (!target->IsPlayer())
            return;

        if (target->GetDistance2d(me) <= 20.0f)
            if (Player* player = target->ToPlayer())

            {
                if (player->GetQuestObjectiveProgress(QUEST_THE_LIONS_CAGE, 1))
                {
                    if (!player->GetPhaseShift().HasPhase(10040)) PhasingHandler::AddPhase(player, 10040, true);
                    if (!player->GetPhaseShift().HasPhase(10041)) PhasingHandler::AddPhase(player, 10041, true);
                    if (!player->GetPhaseShift().HasPhase(10043)) PhasingHandler::AddPhase(player, 10043, true);
                }
                if (player->GetQuestObjectiveProgress(QUEST_THE_AFFLICTORS_KEY, 0) || player->HasQuest(QUEST_AN_UNDESERVED_FATE))
                {
                    if (!player->GetPhaseShift().HasPhase(10040)) PhasingHandler::AddPhase(player, 10040, true);
                }
            }

        if (target->GetDistance2d(me) <= 2.0f)
            if (Player* player = target->ToPlayer())
                if (player->GetQuestStatus(QUEST_THE_AFFLICTORS_KEY) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(QUEST_AN_UNDESERVED_FATE) == QUEST_STATUS_INCOMPLETE)
                {
                    if (Creature* jaina = me->FindNearestCreature(166980, 30.0f))
                        jaina->ForcedDespawn();
                    if (Creature* thrall = me->FindNearestCreature(166981, 30.0f))
                        thrall->ForcedDespawn();
                    player->NearTeleportTo(5260.91f, 7529.63f, 4790.16f, 5.1908f, false);
                }
    }
};

//364374
struct go_portal_to_torghast : public GameObjectAI
{
    go_portal_to_torghast(GameObject* go) : GameObjectAI(go) { }

    void Reset() override
    {
        me->GetScheduler().CancelAll();
        _scheduler.Schedule(1s, [this](TaskContext context)
        {
            std::list<Player*> playerList;
            me->GetPlayerListInGrid(playerList, 3.0f);
            if (playerList.empty())
                return;

            for (Player* players : playerList)
            {
                if (players->GetLevel() == 60)
                    players->TeleportTo(2162, 1646.942f, 2315.244f, 383.060f, 4.733f);
            }

            if (me->IsInWorld())
                context.Repeat(3s);
        });
    }
private:
    TaskScheduler _scheduler;
    std::unordered_set<uint32> _randomEmotes;
};

//Mawforged Lock  351722
struct go_mawforged_lock : public GameObjectAI
{
    go_mawforged_lock(GameObject* go) : GameObjectAI(go) { }

    bool OnGossipHello(Player* player) override
    {
        if (player->GetQuestStatus(QUEST_THE_AFFLICTORS_KEY) == QUEST_STATUS_INCOMPLETE)
        {
            if (Creature* anduin = me->FindNearestCreature(167833, 20.0f))
                if (Creature* jaina = player->SummonCreature(166980, Position(5359.56f, 7627.21f, 4897.56f, 0.543354f), TEMPSUMMON_TIMED_DESPAWN, 600s))
                    if (Creature* thrall = player->SummonCreature(166981, Position(5357.76f, 7631.48f, 4897.81f, 5.95475f), TEMPSUMMON_TIMED_DESPAWN, 600s))
                    {
                        anduin->AI()->Talk(0, player);
                        player->GetScheduler().Schedule(Milliseconds(7000), [this, jaina](TaskContext context)
                            {
                                jaina->AI()->Talk(36);
                            }).Schedule(Milliseconds(12000), [this, anduin](TaskContext context)
                                {
                                    anduin->AI()->Talk(1);
                                });
                    }
            player->ForceCompleteQuest(QUEST_THE_AFFLICTORS_KEY);
        }
        return true;
    }
};

//Mawsteel cage 351761
struct go_mawsteel_cage : public GameObjectAI
{
    go_mawsteel_cage(GameObject* go) : GameObjectAI(go) {}

    uint8 SHACKLEDSOUL_NUMBER = urand(1, 3);

    bool OnGossipHello(Player* player) override
    {
        if (player->GetQuestStatus(QUEST_AN_UNDESERVED_FATE) == QUEST_STATUS_INCOMPLETE)
        {
            if (Creature* thrall = GetClosestCreatureWithEntry(player, 167176, 20.0f))
                if (Creature* jaina = GetClosestCreatureWithEntry(player, 167154, 20.0f))
                {
                    if (rand32() % 100 < 60)                              //50% random talk 
                        thrall->AI()->Talk(0);
                    else jaina->AI()->Talk(0);
                }

            for (uint8 i = 0; i < SHACKLEDSOUL_NUMBER; ++i)
            {
                player->KilledMonsterCredit(167847);
            }
        }
        return true;
    }
};

// 330574 Deathbolt Rift
struct spell_deathbolt_rift_330571 : public SpellScript
{
    PrepareSpellScript(spell_deathbolt_rift_330571);

    void HandleAfterCast()
    {
        if (Unit* caster = GetCaster())
        {
            // if (Unit* target = caster->GetVictim())
            caster->CastSpell(caster->GetVictim(), 330569, false);
        }
    }

    void Register()
    {
        AfterCast += SpellCastFn(spell_deathbolt_rift_330571::HandleAfterCast);
    }
};

// 164937 runecarver
struct npc_runecarver_164937 : public ScriptedAI
{
public:
    npc_runecarver_164937(Creature* creature) : ScriptedAI(creature) {}

    bool OnGossipHello(Player* player) override
    {
        if (me->IsQuestGiver())
        {
            player->PrepareQuestMenu(me->GetGUID());
            SendGossipMenuFor(player, player->GetGossipTextId(me), me->GetGUID());  //Gossip_menu_id must have number.
        }
        if (player->GetQuestObjectiveProgress(QUEST_UNTANGLING_THE_SIGIL, 1) && !player->GetQuestObjectiveProgress(QUEST_UNTANGLING_THE_SIGIL, 0))
        {
            AddGossipItemFor(player, GossipOptionNpc::None, "I need your help. We need to seal this sigil so the Jailer cannot retrieve it.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 0);
            SendGossipMenuFor(player, 40538, me->GetGUID());
            player->PrepareQuestMenu(me->GetGUID());
        }
        return true;
    }

    bool OnGossipSelect(Player* player, uint32 /*MenuID*/, uint32 gossipListId) override
    {
        uint32 const action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);
        ClearGossipMenuFor(player);
        switch (action)
        {
        case GOSSIP_ACTION_INFO_DEF + 0:
            CloseGossipMenuFor(player);
            player->GetSceneMgr().PlaySceneByPackageId(3258, SceneFlag::None);
            if (Creature* bh = player->FindNearestCreature(178774, 20.f, true))
                bh->ForcedDespawn();
            me->DespawnOrUnsummon(10s, 1min);
            player->GetScheduler().Schedule(44s, [player](TaskContext context)
                {
                    player->TeleportTo(2471, 2692.57f, 2099.33f, 318.109f, 6.2024f);
                });
            break;
        }
        return true;
    }


    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_THE_PRIMUS_RETURNS)
            player->CastSpell(player, SCENE_RUNECARVER_RETURNS);
    }
};

void AddSC_zone_the_maw_part_one()
{
    new player_the_maw();
    new scene_the_maw();
    RegisterCreatureAI(npc_knight_of_the_ebon_blade_166963);
    RegisterSpellScript(spell_rallying_324812);
    RegisterCreatureAI(npc_highlord_darion_mograine_166886);
    new npc_highlord_darion_mograine_165918();
    RegisterCreatureAI(npc_wilona_thorne_170624);
    new npc_ruiner_maroth_166174();
    RegisterCreatureAI(npc_dnt_credit_first_clue);
    RegisterCreatureAI(npc_dnt_credit_second_clue);
    RegisterCreatureAI(npc_dnt_credit_third_clue);
    new npc_lady_jaina_proudmoore_166980();
    RegisterCreatureAI(npc_mawsworn_harbinger_177071);
    new npc_thrall_166981();
    RegisterCreatureAI(npc_Malice_Shadow_165909);
    RegisterCreatureAI(npc_Tormented_Amalgamation_165976);
    new item_Shattered_Helm_of_Domination();
    new item_Shattered_Helm_of_Domination_2();
    //RegisterSpellScript(spell_dominating);
    RegisterCreatureAI(npc_Phael_the_Afflictor_167834);
    RegisterCreatureAI(npc_thelonia_the_cruel);
    RegisterCreatureAI(npc_serath_the_gluttonous);
    RegisterCreatureAI(npc_mawsworn_soulrender);
    RegisterCreatureAI(npc_dnt_credit_tremaculum);
    RegisterCreatureAI(npc_Tremaculum_Enemy_178108);
    RegisterGameObjectAI(go_portal_to_torghast);
    RegisterGameObjectAI(go_mawforged_lock);
    RegisterGameObjectAI(go_mawsteel_cage);
    RegisterSpellScript(spell_deathbolt_rift_330571);
    RegisterCreatureAI(npc_runecarver_164937);

}
