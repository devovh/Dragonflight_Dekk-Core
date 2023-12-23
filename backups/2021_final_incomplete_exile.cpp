/*
 * Copyright 2021
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
#include "Conversation.h"
#include "ScriptedEscortAI.h"
#include "ObjectMgr.h"
#include "FunctionProcessor.h"
#include "GameObject.h"
#include "GameObjectAI.h"
#include "ObjectAccessor.h"
#include "AreaTriggerAI.h"
#include "AreaTrigger.h"
#include "InstanceScript.h"
#include "CombatAI.h"
#include "PhasingHandler.h"
#include "TaskScheduler.h"
#include "TemporarySummon.h"
#include "Transport.h"
#include "Vehicle.h"
#include "Log.h"
#include "Chat.h"
#include "ZoneScript.h"
#include "ScriptedGossip.h"
#include "Weather.h"
#include "Spell.h"

/******************************************************
// TODO: https://www.wowhead.com/npc=153964/bloodbeak spells
// TODO: redeather fix killing cadavers find out how to get the spell casted or zombie killed in the scene
// TODO: finish herbert rescuing with walking npc to others
// TODO: killclaw treasurechest flags and position z
// TODO: hrun before die burning
// TODO: crenna rescue need to make it more blizzlike with walking lana wonsa crenna
// TODO: cast spell on choppy make it malfunctioning
// TODO: crenna should walk down to the base
// TODO: npc_breka_grimaxe_167145 should do with escortai AssistPlayerInCombatAgainst
// TODO: CONTINUE ADD THE ALL SCRIPT (note to self (fluxurion))
// TODO: Moveformation changed need another method to move these guys.. find moveformation and rewrite
******************************************************/

enum ExileReach
{
    QUEST_WARMING_UP = 56775,
    QUEST_STAND_YOUR_GROUND = 58209,
    QUEST_BRACE_FOR_IMPACT = 58208,
    QUEST_MURLOC_MANIA = 55122,
    QUEST_EMERGENCY_FIRST_AID = 54951,
    QUEST_FINDING_THE_LOST_EXPEDITION = 54952,
    QUEST_COOKING_MEAT = 55174,
    QUEST_ENHANCED_COMBAT_TACTICS = 59254,
    QUEST_NORTHBOUND = 55173,
    QUEST_DOWN_WITH_THE_QUILBOAR = 55186,
    QUEST_FORBIDDEN_QUILBOAR_NECROMANCY = 55184,
    QUEST_THE_SCOUT_O_MATIC_5000 = 55193,
    QUEST_RE_SIZING_THE_SITUATION = 56034,
    QUEST_RIDE_OF_THE_SCIENTIFICALLY_ENHANCED_BOAR = 55879,
    QUEST_THE_HARPY_PROBLEM = 55196,
    QUEST_THE_RESCUE_OF_MEREDY_HUNTSWELL = 55763,
    QUEST_HARPY_CULLING = 55764,
    QUEST_PURGE_THE_TOTEMS = 55881,
    QUEST_MESSAGE_TO_BASE = 55882,
    QUEST_WESTWARD_BOUND = 55965,
    QUEST_WHO_LURKS_IN_THE_PIT = 55639,
    QUEST_TO_DARKMAUL_CITADEL = 56344,
    QUEST_RIGHT_BENEATH_THEIR_EYES = 55981,
    QUEST_LIKE_OGRES_TO_THE_SLAUGHTER = 55988,
    QUEST_CATAPULT_DESTRUCTION = 55989,
    QUEST_CONTROLLING_THEIR_STONES = 55990,
    QUEST_DUNGEON_DARKMAUL_CITADEL = 55992,
    QUEST_AN_END_TO_BEGINNINGS = 55991,

    QUEST_FREEING_THE_LIGHT = 54933,

    //Quest Horde
    QUEST_H_WARMING_UP = 59926,
    QUEST_H_STAND_YOUR_GROUND = 59927,
    QUEST_H_BRACE_FOR_IMPACT = 59928,
    QUEST_H_MURLOCK_MANIA = 59929,
    QUEST_H_EMERGENCY_FIRST_AID = 59930,
    QUEST_H_FINDING_THE_LOST_EXPEDITION = 59931,
    QUEST_H_COOKING_MEAT = 59932,
    QUEST_H_ENHANCED_COMBAT_TACTICS = 59933,
    QUEST_H_NORTHBOUND = 59935,
    QUEST_H_DOWN_WITH_THE_QUILBOAR = 59938,
    QUEST_H_FORBIDDEN_QUILBOAR_NECROMANCY = 59939,
    QUEST_H_THE_CHOPPY_BOOSTER_MK5 = 59940,
    QUEST_H_RESIZING_THE_SITUATION = 59941,
    QUEST_H_THE_REDEATHER = 59942,
    QUEST_H_THE_HARPY_PROBLEM = 59943,
    QUEST_H_THE_RESCUE_OF_HERBERT_GLOOMBURST = 59944,
    QUEST_H_HARPY_CULLING = 59945,
    QUEST_H_PURGE_THE_TOTEMS = 59946,
    QUEST_H_MESSAGE_TO_BASE = 59947,
    QUEST_H_WESTWARD_BOUND = 59948,
    QUEST_H_WHO_LURKS_IN_THE_PIT = 59949,
    QUEST_H_STOCKING_UP_ON_SUPPLIES = 59950,
    QUEST_H_TO_DARKMAUL_CITADEL = 59975,
    QUEST_H_RIGHT_BENEATH_THEIR_EYES = 59978,

    SCENE_ALI_HORDE_CRASHED_ON_ISLAND = 2708,
    SCENE_DEER_GO_UP                    = 2775,
    SCENE_HENRY_REUNION = 2717,
    SCENE_GRIMAXE_REUNION = 2905,
    SCENE_OGRE_BEFORE_BIG_BOAR_ATTACK   = 2932,
    SCENE_GORGROTHS_RITUAL = 2820,
    SCENE_H_GORGROTHS_RITUAL = 2904,
    SCENE_SKELETON_ARMY_DARKMAUL_PLAINS = 2712,
    SCENE_FLY_AWAY = 2908,
    SCENE_HIDE_TRANSITION_TO_BEACH = 3108,
    SCENE_LIGHTSPAWN_RITUAL_COMPLETE = 2809,

    AREA_NORTH_SEA = 10639,
    COOKED_MEAT = 174074,

    // SPELLS
    SPELL_QUEST_INVISIBILITY = 85813,
    SPELL_SEE_QUEST_INVISIBILITY = 103339,
    SPELL_KNEEL = 325061,
    SPELL_FIRST_AID = 297415,
    SPELL_A_RESIZING_BACKPACK = 305750,
    SPELL_H_RESIZING_BACKPACK = 326634,
    SPELL_A_SHIP_CRASH = 347654,
    SPELL_H_SHIP_CRASH = 346799,
    SPELL_GORGROTH_SCENE = 326626,
    SPELL_UPHEAVAL = 319273,
    SPELL_EARTHBOLT = 270453,
    SPELL_RESIZER_CHANNELING = 313269,
    SPELL_RESIZER_SPARKS = 305742,
    SPELL_RESIZER_OVERCHARGE = 325347,
    SPELL_RESIZER_HIT_TOSMALL = 305724,
    SPELL_RESIZER_HIT_EXPLODE = 305721,
    SPELL_INCREASE_SIZE = 283095,
    SPELL_SPIRIT_BOLT = 319294,
    SPELL_SOUL_GRASP = 319298,
    SPELL_HARPIE_LIGHTNINGBOLT = 270929,
    SPELL_HARPIE_TALONRAKE = 317077,
    SPELL_WORG_CLAW = 275012,
    SPELL_BURNING_TOP = 353675,
    SPELL_BURNING_FULL = 317299,
    SPELL_ROPE = 252051,
    SPELL_EARTHSHATTER = 319292,
    SPELL_BACKHAND = 276991,
    SPELL_BRUTAL_STRIKE = 317383,
    SPELL_EARTH_BOLT_2 = 321188,
    SPELL_QUILBOAR_SLEEP = 313265,
    SPELL_POISONED_BITE = 316927,
    SPELL_DRAIN_SPIRIT = 319310,
    SPELL_FINAL_SACRIFICE_FIRE = 322352,
    SPELL_RITUAL_SCENE_MAIN_BEAM = 321690,
    SPELL_RITUAL_SCENE_HARPY_BEAM = 321691,
    SPELL_RITUAL_SCENE_HRUN_BEAM = 321692,
    SPELL_RITUAL_SCENE_OGRE_CITADEL = 321693,
    SPELL_LIGHTSPAWN_FREED = 319108,
    SPELL_LIGHTSPAWN_NECROTICBALL = 319136,
    SPELL_LIGHTSPAWN_LIGHTBALL = 319137,
    SPELL_LIGHTSPAWN_LIGHTSBOON = 319160,

    // PHASES
    // used phaseid-s from sniff and times of use in a selected sniff:
    // used phaseid-s from sniff and times of use in a selected sniff:
    /*
    * 13566 x406
    * 13766 x497
    * 13783 x379
    * 13814 x406
    * 13834 x52
    * 13878 x408
    * 14328 x4
    * 14663 x5
    * 14695 x35
    * 15278 x81
    * 15293 x183
    * 15298 x59
    * 15312 x24
    * 15313 x3
    * 15314 x183
    * 15315 x412
    * 15316 x406
    * 15318 x412
    * 15319 x8
    * 15327 x16
    * 15329 x4
    * 15330 x180
    * 15337 x86
    * 15338 x75
    * 15341 x19
    * 15342 x19
    * 15353 x130
    * 15354 x28
    * 15355 x406
    * 15357 x406
    * 15358 x406
    * 15377 x187
    * 15447 x183
    * 15473 x31
    * 15475 x36
    * 15485 x180
    * 15492 x10
    * 15493 x10
    * 15558 x1
    */
    HORDE_PHASE_1 = 12930,
    PHASE_GENERIC = 12940,         // generic phase, active always
    HORDE_PHASE_2 = 12950,
    PHASE_ZOMBIE = 16752,       // generic phase for zombies etc. removed on chapter 7
    PHASE_KILLING_TORGOK = 12980,
    PHASE_AFTER_ZOMBIEKILL = 12990, // basic base without shuja herbert bo crenna
    PHASE_BEFORE_HARPIE_KILL = 13000, // phase before the harpie quests
    PHASE_BEFORE_SPIDER_KILL = 13010, // phase before the pit quests
    PHASE_AFTER_HARPIE_KILL = 13020, // shuja herbert bo arrived to base -- horde camp adds sometimes 13040.. is it the client?
    PHASE_AFTER_SPIDER_KILL = 13050, // crenna arrived to base

    PHASE_OGRE_TRANSFORM = 13060,


    NPC_GRIMAXE = 166573,       //creatureId
    NPC_THROG = 166784,
    NPC_JINHAKE = 166800,
    NPC_LIGHTSPAWN_PEDESTAL_INVISBUNNY = 155080,

};

bool FIZZLEPOP_SAY;

enum CaptainGarricTxt
{
    NPC_GARRIC = 156280,            //creatureId

    GARRIC_TXT0 = 0,                //cTextGuid
    GARRIC_TXT1 = 1,
    GARRIC_TXT2 = 2,
    GARRIC_TXT3 = 3,
    GARRIC_TXT4 = 4,

};

static const Position BoPositions[3] =
{
    { -437.132f, -2607.76f, 0.368385f },
    { -410.63f, -2598.41f,  1.1679f },
    { -385.65f, -2594.42f,  3.2135f }
};


static const Position MithdranRastrealbaPositions[3] =
{
    { -433.115f, -2607.78f, 0.33381f },
     { -412.63f, -2598.41f,  1.1679f },
    { -385.65f, -2594.42f,  3.2135f }
};


static const Position LanaJordanPositions[3] =
{
    { -431.541f, -2611.1f, 0.627797f },
     { -410.63f, -2598.41f,  1.1679f },
    { -387.65f, -2596.42f,  3.2135f }
};

static const Position GrimaxePositions[2] =
{
    { -414.6278f, -2605.1f, 0.80453f },
    { -435.5199f, -2611.19f, 0.583902f }
};

static const Position MessagetoBase[7] =
{
     { 380.685f, -2432.11f, 120.123f },
     { 373.603f, -2396.19f,  110.98f },
     { 351.159f, -2374.26f,  103.288f },
     { 320.583f, -2357.17f,  93.3038f },
     { 272.537f, -2347.56f,  83.2454f },
     { 229.929f, -2325.4f, 80.9311f },
     { 196.863f, -2289.92f,  80.7383f }
};

// Horde encounter transport
Position const throgtransport = { -12.5183f, 13.4584f, 0.368385f, 4.72395f };
Position const warlordbrekagrimaxe = { -6.39468f, -0.548941f, 8.91498f, 3.01749f };

// Alliance encounter transport
Position const privatecoletransport = { -8.83094f, 2.16856f, 5.42808f, 4.46237f };

class exiles_reach : public PlayerScript
{
public:
    exiles_reach() : PlayerScript("exiles_reach") { }

    void OnLogin(Player* player, bool /*firstLogin*/) override
    {
        // Set zone as sanctuary
        if (player->GetMapId() == MAP_NPE)
            player->AddPvpFlag(UNIT_BYTE2_FLAG_SANCTUARY);

        OnCheckPhase(player);
    }

    void OnUpdateArea(Player* player, uint32 newArea, uint32 /*oldArea*/) override
    {
        // Set zone as sanctuary
        if (player->GetMapId() == MAP_NPE)
            player->AddPvpFlag(UNIT_BYTE2_FLAG_SANCTUARY);

        switch (newArea = player->GetAreaId())
        {
        case AREA_NORTH_SEA:
            if (Creature* npc = player->FindNearestCreature(NPC_GARRIC, 20.0f, true))
            {
                player->GetScheduler().Schedule(Milliseconds(5000), [npc, player](TaskContext context)
                    {
                        npc->AI()->Talk(GARRIC_TXT0, player);
                    });
            }
            break;
        default:
            break;
        }

        if (player->GetAreaId() != AREA_NORTH_SEA && player->GetMapId() == 2175)
        {
            if (!player->HasAura(SPELL_RITUAL_SCENE_HARPY_BEAM))
            {
                // adding ritual scene auras to see ritual effects very nice
                player->CastSpell(player, SPELL_RITUAL_SCENE_HARPY_BEAM, true);
                player->CastSpell(player, SPELL_RITUAL_SCENE_HRUN_BEAM, true);
                player->CastSpell(player, SPELL_RITUAL_SCENE_MAIN_BEAM, true);
                player->CastSpell(player, SPELL_RITUAL_SCENE_OGRE_CITADEL, true);
            }
        }

        if (player->GetMapId() != 2175 && player->HasAura(SPELL_RITUAL_SCENE_HARPY_BEAM))
        {
            // Remove sanctuary flag when leaving exiles reach
            player->RemovePvpFlag(UNIT_BYTE2_FLAG_SANCTUARY);
            // removing ritual scene auras to unsee ritual effects very nice
            player->RemoveAura(SPELL_RITUAL_SCENE_HARPY_BEAM);
            player->RemoveAura(SPELL_RITUAL_SCENE_HRUN_BEAM);
            player->RemoveAura(SPELL_RITUAL_SCENE_MAIN_BEAM);
            player->RemoveAura(SPELL_RITUAL_SCENE_OGRE_CITADEL);
        }

        OnCheckPhase(player);
    }

    // after the ship crash movie play the tiny murloc scene
    void OnMovieComplete(Player* player, uint32 movieId) override
    {
        if (movieId == 931)
        {
            player->RemoveAura(325131); //big black screen - means eyes closed //  remove - means opening eyes
            player->GetSceneMgr().PlaySceneByPackageId(SCENE_ALI_HORDE_CRASHED_ON_ISLAND, SceneFlag::None); // when opening eyes seeing murlocs xD
        }
    }

    void OnQuestStatusChange(Player* player, uint32 /*questId*/) override
    {
        OnCheckPhase(player);
    }

    void OnPlayerRepop(Player* player) override
    {
        OnCheckPhase(player);
    }

    void OnPlayerResurrect(Player* player) override
    {
        OnCheckPhase(player);
    }

    // phase checker mechanic (phasing)
    void OnCheckPhase(Player* player)
    {
        if ((player->GetMapId() == MAP_NPE) && (player->GetZoneId() == 10424))
        {
            // horde
            if (player->IsInHorde())
            {
                // after ship quests add basic phases
                if ((player->GetQuestStatus(QUEST_H_BRACE_FOR_IMPACT) == QUEST_STATUS_COMPLETE) || (player->GetQuestStatus(QUEST_H_BRACE_FOR_IMPACT) == QUEST_STATUS_REWARDED))
                {
                    if (!player->GetPhaseShift().HasPhase(PHASE_GENERIC)) PhasingHandler::AddPhase(player, PHASE_GENERIC, true);
                }
                // if zombie kill quest not done yet add zombie phase
                if (player->GetQuestStatus(QUEST_H_THE_REDEATHER) == QUEST_STATUS_NONE || player->GetQuestStatus(QUEST_H_THE_REDEATHER) == QUEST_STATUS_INCOMPLETE)
                {
                    if (!player->GetPhaseShift().HasPhase(PHASE_ZOMBIE)) PhasingHandler::AddPhase(player, PHASE_ZOMBIE, true);
                }
                // after zombie kill quest remove zombie phase
                if (player->GetQuestStatus(QUEST_H_THE_REDEATHER) == QUEST_STATUS_REWARDED)
                {
                    if (player->GetPhaseShift().HasPhase(PHASE_ZOMBIE)) PhasingHandler::RemovePhase(player, PHASE_ZOMBIE, true);
                }
                // if zombie kill done add phase after zombie kill
                if (player->GetQuestStatus(QUEST_H_THE_REDEATHER) == QUEST_STATUS_REWARDED)
                {
                    //12990
                    if (!player->GetPhaseShift().HasPhase(PHASE_AFTER_ZOMBIEKILL)) PhasingHandler::AddPhase(player, PHASE_AFTER_ZOMBIEKILL, true);
                }
                // before spider kill get wonsa and lana pointing to the pit
                if (player->GetQuestStatus(QUEST_H_THE_REDEATHER) == QUEST_STATUS_REWARDED && (player->GetQuestStatus(QUEST_H_WESTWARD_BOUND) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(QUEST_H_WESTWARD_BOUND) == QUEST_STATUS_NONE || player->GetQuestStatus(QUEST_H_WESTWARD_BOUND) == QUEST_STATUS_COMPLETE))
                {
                    //13010
                    if (!player->GetPhaseShift().HasPhase(PHASE_BEFORE_SPIDER_KILL)) PhasingHandler::AddPhase(player, PHASE_BEFORE_SPIDER_KILL, true);
                }
                // before spider kill at the entrance of the pit remove old lana and wonsa
                if (player->GetQuestStatus(QUEST_H_WESTWARD_BOUND) == QUEST_STATUS_REWARDED)
                {
                    //-13010
                    if (player->GetPhaseShift().HasPhase(PHASE_BEFORE_SPIDER_KILL)) PhasingHandler::RemovePhase(player, PHASE_BEFORE_SPIDER_KILL, true);
                }
                // before harpie kill get shuja bo and herbert to the harpies
                if (player->GetQuestStatus(QUEST_H_THE_REDEATHER) == QUEST_STATUS_REWARDED && (player->GetQuestStatus(QUEST_H_MESSAGE_TO_BASE) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(QUEST_H_MESSAGE_TO_BASE) == QUEST_STATUS_NONE || player->GetQuestStatus(QUEST_H_MESSAGE_TO_BASE) == QUEST_STATUS_COMPLETE))
                {
                    //13000
                    if (!player->GetPhaseShift().HasPhase(PHASE_BEFORE_HARPIE_KILL)) PhasingHandler::AddPhase(player, PHASE_BEFORE_HARPIE_KILL, true);
                }
                // after spider kill get crenna to the base
                if (player->GetQuestStatus(QUEST_H_WHO_LURKS_IN_THE_PIT) == QUEST_STATUS_COMPLETE || player->GetQuestStatus(QUEST_H_WHO_LURKS_IN_THE_PIT) == QUEST_STATUS_REWARDED)
                {
                    //13050
                    if (!player->GetPhaseShift().HasPhase(PHASE_AFTER_SPIDER_KILL)) PhasingHandler::AddPhase(player, PHASE_AFTER_SPIDER_KILL, true);
                }
                // after harpie kill get shuja bo and herbert to the base
                if (player->GetQuestStatus(QUEST_H_MESSAGE_TO_BASE) == QUEST_STATUS_COMPLETE || player->GetQuestStatus(QUEST_H_MESSAGE_TO_BASE) == QUEST_STATUS_REWARDED)
                {
                    //13020
                    if (!player->GetPhaseShift().HasPhase(PHASE_AFTER_HARPIE_KILL)) PhasingHandler::AddPhase(player, PHASE_AFTER_HARPIE_KILL, true);
                }
                else
                {
                    //-13020
                    if (player->GetPhaseShift().HasPhase(PHASE_AFTER_HARPIE_KILL)) PhasingHandler::RemovePhase(player, PHASE_AFTER_HARPIE_KILL, true);
                }
                // if harpie quests and spider quests complete
                // removing old npc's and old phases
                // all we need is in 13060
                if (player->GetQuestStatus(QUEST_H_MESSAGE_TO_BASE) == QUEST_STATUS_REWARDED && player->GetQuestStatus(QUEST_H_WHO_LURKS_IN_THE_PIT) == QUEST_STATUS_REWARDED)
                {
                    //-12990 -13020 -13050
                    //13060
                    if (player->GetPhaseShift().HasPhase(PHASE_AFTER_ZOMBIEKILL)) PhasingHandler::RemovePhase(player, PHASE_AFTER_ZOMBIEKILL, true);
                    if (player->GetPhaseShift().HasPhase(PHASE_AFTER_HARPIE_KILL)) PhasingHandler::RemovePhase(player, PHASE_AFTER_HARPIE_KILL, true);
                    if (player->GetPhaseShift().HasPhase(PHASE_AFTER_SPIDER_KILL)) PhasingHandler::RemovePhase(player, PHASE_AFTER_SPIDER_KILL, true);
                    if (!player->GetPhaseShift().HasPhase(PHASE_OGRE_TRANSFORM)) PhasingHandler::AddPhase(player, PHASE_OGRE_TRANSFORM, true);
                }
            }

            // alli
            if (player->IsInAlliance())
            {
                // after ship quests add basic phases
                if ((player->GetQuestStatus(QUEST_BRACE_FOR_IMPACT) == QUEST_STATUS_COMPLETE) || (player->GetQuestStatus(QUEST_BRACE_FOR_IMPACT) == QUEST_STATUS_REWARDED) && (player->GetQuestStatus(QUEST_RIDE_OF_THE_SCIENTIFICALLY_ENHANCED_BOAR) != QUEST_STATUS_REWARDED))
                {
                    if (!player->GetPhaseShift().HasPhase(PHASE_GENERIC)) PhasingHandler::AddPhase(player, PHASE_GENERIC, true);
                    if (!player->GetPhaseShift().HasPhase(PHASE_ZOMBIE)) PhasingHandler::AddPhase(player, PHASE_ZOMBIE, true);
                }

                if ((player->GetQuestStatus(QUEST_RIDE_OF_THE_SCIENTIFICALLY_ENHANCED_BOAR) == QUEST_STATUS_REWARDED) || (player->GetQuestStatus(QUEST_H_THE_REDEATHER) == QUEST_STATUS_REWARDED))
                {
                    PhasingHandler::RemovePhase(player, PHASE_ZOMBIE, true);
                }
                if ((player->GetQuestStatus(QUEST_RIDE_OF_THE_SCIENTIFICALLY_ENHANCED_BOAR) == QUEST_STATUS_REWARDED) || (player->GetQuestStatus(QUEST_H_THE_REDEATHER) == QUEST_STATUS_REWARDED))
                {
                    PhasingHandler::AddPhase(player, PHASE_AFTER_ZOMBIEKILL, true);
                }
            }
        }
    }

    // QUEST_H_THE_REDEATHER plays 2497 scene (the reunion) on reward with rewardspell 326678
    // after that we check for phases if the scene is this scene
    // even if the player cancels the scene
    void OnSceneComplete(Player* player, uint32 sceneInstanceID) override
    {
        if (player->GetSceneMgr().GetSceneTemplateFromInstanceId(sceneInstanceID)->SceneId == 2497)
            OnCheckPhase(player);
    }
    void OnSceneCancel(Player* player, uint32 sceneInstanceID)
    {
        if (player->GetSceneMgr().GetSceneTemplateFromInstanceId(sceneInstanceID)->SceneId == 2497)
            OnCheckPhase(player);
    }

    // for dancing in the ogre cooking area
    void OnTextEmote(Player* player, uint32 textEmote, uint32 emoteNum, ObjectGuid guid) override
    {
        /* this one is for getting correct emote id lol
        std::ostringstream checkemote;
        checkemote << "textEmote: " << textEmote << " emoteNum: " << emoteNum;
        player->Say(checkemote.str(), LANG_UNIVERSAL);
        */
        if (player->GetMapId() == MAP_NPE)
            if (emoteNum == 34) // dance
                if (player->IsActiveQuest(QUEST_H_RIGHT_BENEATH_THEIR_EYES))
                {
                    if (Quest const* quest = sObjectMgr->GetQuestTemplate(QUEST_H_RIGHT_BENEATH_THEIR_EYES))
                        for (QuestObjective const& obj : quest->GetObjectives())
                            if (obj.ObjectID == 85148) // dancing in the cooking area
                            {
                                uint16 slot = player->FindQuestSlot(QUEST_H_RIGHT_BENEATH_THEIR_EYES);
                                if (!player->IsQuestObjectiveComplete(slot, quest, obj)) // just make it complete once
                                {
                                    player->SetQuestObjectiveData(obj, 1);
                                    player->SendQuestUpdateAddCredit(quest, ObjectGuid::Empty, obj, 1);

                                    if (Creature* grunk = player->FindNearestCreature(154145, 40.0f, true))
                                    {
                                        grunk->AddDelayedEvent(2000, [grunk, player]()        // time_delay
                                            {
                                                grunk->Talk("This one got some moves!", CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, 25, player);
                                                grunk->SetEmoteState(EMOTE_STATE_DANCE);
                                                if (Creature* jugnug = grunk->FindNearestCreature(154147, 40.0f, true))
                                                    jugnug->SetEmoteState(EMOTE_STATE_DANCE);
                                                if (Creature* wug = grunk->FindNearestCreature(154146, 40.0f, true))
                                                    wug->SetEmoteState(EMOTE_STATE_DANCE);
                                            });
                                        grunk->AddDelayedEvent(8000, [grunk, player]()        // time_delay
                                            {
                                                grunk->GetMotionMaster()->MovePoint(1, 545.088f, -2047.535f, 151.438f, true);
                                                grunk->DespawnOrUnsummon(5s, 60s);

                                                if (Creature* jugnug = grunk->FindNearestCreature(154147, 40.0f, true))
                                                {
                                                    jugnug->GetMotionMaster()->MovePoint(1, 545.088f, -2047.535f, 151.438f, true);
                                                    grunk->DespawnOrUnsummon(5s, 60s);
                                                }
                                                if (Creature* wug = grunk->FindNearestCreature(154146, 40.0f, true))
                                                {
                                                    wug->GetMotionMaster()->MovePoint(1, 633.747f, -2098.588f, 158.566f, true);
                                                    grunk->DespawnOrUnsummon(5s, 60s);
                                                }
                                                if (Creature* breka = grunk->FindNearestCreature(167596, 40.0f, true))
                                                {
                                                    breka->Talk("I said act like an ogre, not dance like one!", CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, 25, player);
                                                    breka->Say(196532);
                                                    breka->PlayDirectSound(157178, player, 196532);
                                                }
                                            });
                                    }

                                }
                            }
                }

    }

};

//160737 npc_combat_dummy
struct npc_combat_dummy : public ScriptedAI
{
    npc_combat_dummy(Creature* creature) : ScriptedAI(creature)
    {
        SetCombatMovement(false);
        me->SetReactState(REACT_PASSIVE);
        me->SetRegenerateHealth(false);
    }

    void JustEngagedWith(Unit* who) override
    {
        if (who->hasQuest(QUEST_H_WARMING_UP) || who->hasQuest(QUEST_WARMING_UP))
        {
            me->SetRegenerateHealth(false);
        }
    }

    void JustDied(Unit* killer) override
    {
        if (Player* player = killer->ToPlayer())
        {
            if (player->IsActiveQuest(QUEST_WARMING_UP))
            {
                player->KilledMonsterCredit(174954);

                if (Creature* npc = player->FindNearestCreature(NPC_GARRIC, 20.0f, true))
                {
                    //npc->PlayDirectSound(152718, player); // TODO: find correct text and sound
                    me->GetMap()->SetZoneWeather(10424, WEATHER_STATE_EXILE_REACH_RAIN, 0.5f);
                    npc->AI()->Talk(GARRIC_TXT2);
                }
            }

            if (player->IsActiveQuest(QUEST_H_WARMING_UP))
            {
                player->KilledMonsterCredit(174954);

                if (Creature* npc = player->FindNearestCreature(NPC_GRIMAXE, 20.0f, true))
                {
                    //npc->PlayDirectSound(152718, player); // TODO: find correct text and sound
                    me->GetMap()->SetZoneWeather(10424, WEATHER_STATE_EXILE_REACH_RAIN, 0.5f);
                    npc->AI()->Talk(3);
                }
            }
            me->ForcedDespawn(1000, 5s);
        }
    }
};

// warlord_breka_grimaxe_166573
struct warlord_breka_grimaxe_166573 : public ScriptedAI
{
    warlord_breka_grimaxe_166573(Creature* creature) : ScriptedAI(creature) { }

    void QuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_H_WARMING_UP)
        {
            Talk(0);
            me->GetMap()->SetZoneWeather(10424, WEATHER_STATE_EXILE_REACH, 0.5f);
        }
    }

    void QuestReward(Player* player, Quest const* quest, LootItemType /*type*/, uint32 /*opt*/) override
    {
        if (quest->GetQuestId() == QUEST_H_WARMING_UP)
            Talk(1);

        if (quest->GetQuestId() == QUEST_H_BRACE_FOR_IMPACT)
        {
            Talk(2);
            me->GetMap()->SetZoneWeather(10424, WEATHER_STATE_EXILE_REACH, 0.5f);
            player->GetSceneMgr().PlaySceneByPackageId(SCENE_HIDE_TRANSITION_TO_BEACH, SceneFlag::None);
            player->TeleportTo(2175, -462.722f, -2620.544f, 0.472f, 0.760f);
            player->CastSpell(player, SPELL_H_SHIP_CRASH);
        }

    }
};

// warlord_breka_grimaxe_166782
class warlord_breka_grimaxe_166782 : public CreatureScript
{
public:
    warlord_breka_grimaxe_166782 () : CreatureScript("warlord_breka_grimaxe_166782") { }


    bool OnQuestReward(Player* player, Creature* creature, Quest const* quest, uint32 /*item*/) override
    {
        if (quest->GetQuestId() == QUEST_H_MURLOCK_MANIA)
        {
            creature->RemoveAura(SPELL_KNEEL);
        }
        return true;
    }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new warlord_breka_grimaxe_166782AI(creature);
    }

    struct warlord_breka_grimaxe_166782AI : public EscortAI
    {
        warlord_breka_grimaxe_166782AI(Creature* creature) : EscortAI(creature) {}

        void QuestAccept(Player* player, Quest const* quest) override
        {
            if (quest->GetQuestId() == QUEST_H_MURLOCK_MANIA)
            {
                Talk(1);
            }

            if (quest->GetQuestId() == QUEST_H_EMERGENCY_FIRST_AID)
            {
                if (Creature* npc = me->FindNearestCreature(NPC_THROG, 5.0f, true))
                {
                    me->CastSpell(npc, SPELL_FIRST_AID);

                    player->GetScheduler().Schedule(Milliseconds(4000), [this] (TaskContext context)
                    {
                        me->SetWalk(true);
                        me->GetMotionMaster()->MovePoint(1, GrimaxePositions[0]);
                        me->RemoveNpcFlag(UNIT_NPC_FLAG_QUESTGIVER);
                    })
                    .Schedule(15s, [this] (TaskContext context)
                    {
                        if (Creature* npc = me->FindNearestCreature(NPC_JINHAKE, 5.0f, true))
                            me->CastSpell(npc, SPELL_FIRST_AID);
                            me->GetMotionMaster()->MovePoint(2, GrimaxePositions[1]);
                    })
                    .Schedule(28s, [this](TaskContext context)
                    {
                        me->AddNpcFlag(UNIT_NPC_FLAG_QUESTGIVER);
                        me->SetFacingTo(3.4f, true);
                    });
                }
            }

            if (quest->GetQuestId() == QUEST_H_FINDING_THE_LOST_EXPEDITION)
            {
                // despawn old version after 60 sec and 30 sec for respawning
                me->DespawnOrUnsummon(60s, 30s);
                SetRun(true);
                Start(true, true);
                SetEscortPaused(true);
                player->GetScheduler().Schedule(2s, [this,player] (TaskContext /*context*/)
                {
                    SetEscortPaused(false);
                    Talk(2, player);
                });
            }
        }

        void WaypointReached(uint32 pointId, uint32 pathId) override
        {
            switch (pointId)
            {
            case 3:
                if (Creature* npc = me->FindNearestCreature(166786, 5.0f, true))
                {
                    npc->GetMotionMaster()->MovePoint(1, BoPositions[1]);
                    npc->GetMotionMaster()->MovePoint(2, BoPositions[2]);
                    me->SetRespawnDelay(30); // 30 sec for respawning
                    npc->ForcedDespawn(5000);
                }

                if (Creature* npc = me->FindNearestCreature(166791, 5.0f, true))
                {
                    npc->GetMotionMaster()->MovePoint(1, MithdranRastrealbaPositions[1]);
                    npc->GetMotionMaster()->MovePoint(2, MithdranRastrealbaPositions[2]);
                    me->SetRespawnDelay(30); // 30 sec for respawning
                    npc->ForcedDespawn(5000);
                }

                if (Creature* npc = me->FindNearestCreature(166796, 5.0f, true))
                {
                    npc->GetMotionMaster()->MovePoint(1, LanaJordanPositions[1]);
                    npc->GetMotionMaster()->MovePoint(2, LanaJordanPositions[2]);
                    me->SetRespawnDelay(30); // 30 sec for respawning
                    npc->ForcedDespawn(5000);
                }
                break;
            case 6:
                SetEscortPaused(true);
                {
                    if (HasEscortState(STATE_ESCORT_PAUSED))
                    {
                        SetPauseTimer(3s);
                        SetEscortPaused(false);
                    }
                }
                break;
            case 10:
                SetEscortPaused(true);
                SetPauseTimer(3s);
                SetEscortPaused(false);
                break;
            case 16:
                SetEscortPaused(true);
                // despawn old version after 30 sec and 10 sec for respawning
                me->DespawnOrUnsummon(30s, 10s);
                if (Creature* npc = me->FindNearestCreature(166854, 10.0f, true))
                {
                    npc->AI()->Talk(2);
                    me->GetScheduler().Schedule(Milliseconds(3000), [this] (TaskContext context)
                    {
                        me->AI()->Talk(3);
                    });
                }
                break;
            default:
                break;
            }
        }

        void QuestReward(Player* player, Quest const* quest, LootItemType type, uint32 /*opt*/) override
        {
            if (quest->GetQuestId() == QUEST_H_MURLOCK_MANIA)
                me->RemoveAura(SPELL_KNEEL);
        }
    };
};


// warlord_breka_grimaxe_166906
struct warlord_breka_grimaxe_166906 : public ScriptedAI
{
    warlord_breka_grimaxe_166906(Creature* creature) : ScriptedAI(creature) { }

    void Reset()
    {
        me->SetFullHealth();
        me->SetFaction(2361);
    }

    void QuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_H_COOKING_MEAT)

            if (Creature* npc = me->FindNearestCreature(166854, 10.0f, true))
            {
                npc->AI()->Talk(0);
                npc->PlayDirectSound(156959, player, 195978);
                me->AddDelayedEvent(3000, [this, player]()        // time_delay
                    {
                        me->AI()->Talk(0);
                        player->GetScheduler().Schedule(5s, [this, player](TaskContext context)
                            {
                                me->AI()->Talk(1);
                            });
                    });


            }

        if (quest->GetQuestId() == QUEST_H_ENHANCED_COMBAT_TACTICS)
        {
            Talk(2);

            player->GetScheduler().Schedule(2s, [this](TaskContext context)
                {
                    me->GetMotionMaster()->MovePoint(1, -245.244f, -2479.318f, 18.080f, true);
                }).Schedule(4s, [this](TaskContext context)
                    {
                        me->GetMotionMaster()->MovePoint(2, -216.699f, -2508.925f, 22.957f, true);
                    }).Schedule(12s, [this, player](TaskContext context)
                        {
                            me->AI()->Talk(3);
                            me->SetFaction(14);
                            me->SetLevel(player->GetLevel());
                            me->SetTarget(player->GetGUID());
                        });




        }
    }

    void QuestReward(Player* player, Quest const* quest, LootItemType /*type*/, uint32 /*opt*/) override
    {
        // remove raw meat if not removed already
        if (quest->GetQuestId() == QUEST_H_COOKING_MEAT)
            if (player->GetItemByEntry(174072))
            {
                uint32 count = player->GetItemCount(174072);
                player->DestroyItemCount(174072, count, true, false);
            }
    }

    void SpellHit(WorldObject* caster, SpellInfo const* spell) override
    {

        int8 itwilldo = rand() % 2 + 1; // 50% chance to breka will accept the spell;

        // check if caster is pet credit goes to owner
        if (caster->IsPlayer())
        {
            // giving one credit to the quest objective
            Player* player = caster->ToPlayer();
            if (player->IsActiveQuest(QUEST_H_ENHANCED_COMBAT_TACTICS) && itwilldo == 1)
            {

                if (Quest const* quest = sObjectMgr->GetQuestTemplate(QUEST_H_ENHANCED_COMBAT_TACTICS))
                    for (QuestObjective const& obj : quest->GetObjectives())
                    {
                        uint16 slot = player->FindQuestSlot(QUEST_H_ENHANCED_COMBAT_TACTICS);
                        if (!player->IsQuestObjectiveComplete(slot, quest, obj))
                        {
                            player->KilledMonsterCredit(164577);
                        }
                        else
                        {
                            me->AttackStop();
                            Reset();
                            player->GetScheduler().Schedule(2s, [this](TaskContext context)
                                {
                                    // quest completed go back
                                    me->GetMotionMaster()->MovePoint(2, -249.05f, -2492.35f, 17.957f, true);
                                });
                        }

                    }
            }
        }
    }

    void JustReachedHome() override
    {
        if (Creature* wonsa = me->FindNearestCreature(166854, 20.0f, true))
            me->SetFacingToObject(wonsa, true); // setfacing to wonsa
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if ((me->GetHealth() <= damage) || (me->GetHealth() <= me->GetHealth() * 0.2))
        {
            Player* player = attacker->ToPlayer();
            if (player->IsActiveQuest(QUEST_H_ENHANCED_COMBAT_TACTICS))
            {
                if (Quest const* quest = sObjectMgr->GetQuestTemplate(QUEST_H_ENHANCED_COMBAT_TACTICS))
                    for (QuestObjective const& obj : quest->GetObjectives())
                    {
                        uint16 slot = player->FindQuestSlot(QUEST_H_ENHANCED_COMBAT_TACTICS);
                        // if breka got too much dmg but the quest is not completed breka got full hp just like on retail
                        if (!player->IsQuestObjectiveComplete(slot, quest, obj)) // just make it complete once
                        {
                            damage = 0;
                            me->SetFullHealth();
                        }
                        else
                        {
                            damage = 0;
                            me->AttackStop();
                            Reset();
                            player->GetScheduler().Schedule(2s, [this](TaskContext context)
                                {
                                    me->GetMotionMaster()->MovePoint(1, -245.244f, -2479.318f, 18.080f, true);
                                }).Schedule(12s, [this](TaskContext context)
                                    {
                                        me->GetMotionMaster()->MovePoint(2, -249.05f, -2492.35f, 17.957f, true);
                                    });
                        }
                    }
            }

        }
    }

    void JustDied(Unit* killer /*killer*/)
    {
        Player* player = killer->ToPlayer();
        if (player->IsActiveQuest(QUEST_H_ENHANCED_COMBAT_TACTICS))
        {
            if (Quest const* quest = sObjectMgr->GetQuestTemplate(QUEST_H_ENHANCED_COMBAT_TACTICS))
                for (QuestObjective const& obj : quest->GetObjectives())
                {
                    uint16 slot = player->FindQuestSlot(QUEST_H_ENHANCED_COMBAT_TACTICS);
                    // if breka got too much dmg but the quest is not completed breka got full hp just like on retail
                    if (!player->IsQuestObjectiveComplete(slot, quest, obj)) // just make it complete once
                    {
                        me->Respawn();
                        me->SetFullHealth();
                    }
                    else
                    {
                        me->Respawn();
                        me->AttackStop();
                        Reset();
                        me->GetMotionMaster()->MovePoint(3, -249.05f, -2492.35f, 17.957f, true);
                    }
                }
        }
    }

};

// npc_Throg_166784
struct npc_Throg_166784 : public ScriptedAI
{
    npc_Throg_166784 (Creature* creature) : ScriptedAI(creature) { }

    void SpellHit(WorldObject* caster, SpellInfo const* spell) override
    {
        if (spell->Id != SPELL_FIRST_AID)
            return;

            me->SetStandState(UNIT_STAND_STATE_STAND);
            me->SetWalk(true);

            caster->ToUnit()->GetScheduler().Schedule(Milliseconds(2000), [this] (TaskContext /*task*/)
            {
                me->GetMotionMaster()->MovePoint(1, -389.445f, -2593.08f, 4.0625f);
                me->ForcedDespawn(15000);
            });
    }
};

// npc_Jinhake_16680
struct npc_Jinhake_16680 : public ScriptedAI
{
    npc_Jinhake_16680 (Creature* creature) : ScriptedAI(creature) { }

    void SpellHit(WorldObject* caster, SpellInfo const* spell) override
    {
        if (spell->Id != SPELL_FIRST_AID)
            return;

        me->SetStandState(UNIT_STAND_STATE_STAND);
        me->SetWalk(true);

        caster->ToUnit()->GetScheduler().Schedule(Milliseconds(2000), [this] (TaskContext /*task*/)
        {
            me->GetMotionMaster()->MovePoint(1, -389.445f, -2593.08f, 4.0625f);
            me->ForcedDespawn(10000);
        });
    }
};

// npc_Bo_166786
struct npc_Bo_166786 : public ScriptedAI
{
    npc_Bo_166786(Creature* creature) : ScriptedAI(creature) { }

    void SpellHit(WorldObject* caster, SpellInfo const* spell) override
    {
        if (spell->Id != SPELL_FIRST_AID)
            return;

        Player* player = caster->ToPlayer();
        if (player && player->GetQuestStatus(QUEST_H_EMERGENCY_FIRST_AID) != QUEST_STATUS_REWARDED)
        {
                Talk(0);
                //me->CastSpell(me, SPELL_FIRST_AID, true);
                me->SetStandState(UNIT_STAND_STATE_STAND);

                    player->GetScheduler().Schedule(Milliseconds(3000), [this] (TaskContext )
                    {
                        me->GetMotionMaster()->MovePoint(1, BoPositions[0]);
                    });                
        }
    }
};

// npc_Mithdran_Rastrealba_166791
struct npc_Mithdran_Rastrealba_166791 : public ScriptedAI
{
    npc_Mithdran_Rastrealba_166791(Creature* creature) : ScriptedAI(creature) { }

    void SpellHit(WorldObject* caster, SpellInfo const* spell) override
    {
        if (spell->Id != SPELL_FIRST_AID)
            return;

        Player* player = caster->ToPlayer();
        if (player && player->GetQuestStatus(QUEST_H_EMERGENCY_FIRST_AID) != QUEST_STATUS_REWARDED)
        {
            Talk(0);
            //me->CastSpell(me, SPELL_FIRST_AID, true);
            me->SetStandState(UNIT_STAND_STATE_STAND);

            if (Unit* MoveFormationLeader = me->FindNearestCreature(166800, 50.f, true))
                player->GetScheduler().Schedule(Milliseconds(3000), [this] (TaskContext)
                {
                    me->GetMotionMaster()->MovePoint(1, MithdranRastrealbaPositions[0]);
                });            
        }
    }
};

// npc_Lana_Jordan_166796
struct npc_Lana_Jordan_166796 : public ScriptedAI
{
     npc_Lana_Jordan_166796 (Creature* creature) : ScriptedAI(creature) { }

    void SpellHit(WorldObject* caster, SpellInfo const* spell) override
    {
        if (spell->Id != SPELL_FIRST_AID)
            return;

        Player* player = caster->ToPlayer();
        if (player && player->GetQuestStatus(QUEST_H_EMERGENCY_FIRST_AID) != QUEST_STATUS_REWARDED)
        {
            Talk(0);
            //me->CastSpell(me, SPELL_FIRST_AID, true);
            me->SetStandState(UNIT_STAND_STATE_STAND);

            if (Unit* MoveFormationLeader = me->FindNearestCreature(166800, 50.f, true))
                player->GetScheduler().Schedule(Milliseconds(3000), [this] (TaskContext)
                {
                    me->GetMotionMaster()->MovePoint(1, LanaJordanPositions[0]);
                });            
        }
    }
};



// 166583_grunt_throg
struct grunt_throg_166583 : public ScriptedAI
{
    grunt_throg_166583(Creature* creature) : ScriptedAI(creature) { }

    void QuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_H_STAND_YOUR_GROUND)
        {
            Talk(0);

            player->GetScheduler().Schedule(Milliseconds(5000), [this] (TaskContext context)
            {
                me->SetFaction(14);
                me->SetReactState(REACT_AGGRESSIVE);
                me->AI()->Talk(1);
            });
        }

        if (quest->GetQuestId() == QUEST_H_BRACE_FOR_IMPACT)
        {
            Talk(2);
            me->GetMap()->SetZoneWeather(10424, WEATHER_STATE_EXILE_REACH_HEAVY_RAIN, 0.5f);

            if (Creature* npc = player->FindNearestCreature(NPC_GRIMAXE, 20.0f, true))
            {
                player->GetScheduler().Schedule(Milliseconds(4000), [npc] (TaskContext context)
                {
                    npc->AI()->Talk(4);
                });
            }
        }

    }


    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if (Player* player = attacker->ToPlayer())
        {
            if ((me->GetHealth() < me->CountPctFromMaxHealth(60)) && (me->GetHealth() >= me->CountPctFromMaxHealth(50)))
            {
                Talk(3);
            }
            if (me->HealthBelowPctDamaged(30, damage))
            {
                damage = 0;
                me->SetFullHealth();
                me->SetFaction(35);
                me->AttackStop();
                attacker->ToPlayer()->KilledMonsterCredit(155607, ObjectGuid::Empty);
                me->SummonCreature(166827, me->GetPosition(), TEMPSUMMON_MANUAL_DESPAWN);

                attacker->ToPlayer()->GetScheduler().Schedule(Milliseconds(2000), [this] (TaskContext context)
                {
                    me->AI()->Talk(4);
                });
            }
        }
    }
};


// npc_wonsa_166854
    struct npc_wonsa_166854 : public ScriptedAI
    {
        npc_wonsa_166854 (Creature* creature) : ScriptedAI(creature) { }

        void Reset()
        {
            say = false;
        }

        void MoveInLineOfSight(Unit* who) override
        {
            if (Player* player = who->ToPlayer())
            {
                if (player->IsInDist(me, 10.0f))
                {
                    if (player->IsActiveQuest(QUEST_H_FINDING_THE_LOST_EXPEDITION))
                    {
                        player->KilledMonsterCredit(166854, ObjectGuid::Empty);
                    }

                    if (player->HasItemCount(COOKED_MEAT, 2, false))
                    {
                        if (Creature* npc = me->FindNearestCreature(166906, 10.0f, true))
                        {
                            if (!say)
                            {
                                Talk(1);

                                player->GetScheduler().Schedule(4s, [npc] (TaskContext context)
                                {
                                    npc->AI()->Talk(4);
                                });
                                say = true;
                            }
                        }
                    }

                }
            }
        }
    private:
        bool say;


        void QuestReward(Player* player, Quest const* quest, LootItemType type, uint32 /*opt*/) override
        {
            if (quest->GetQuestId() == QUEST_H_COOKING_MEAT)
            {
                me->SetStandState(UNIT_STAND_STATE_STAND);
            }
        }
    };


    // npc_wonsa_175030
    struct npc_wonsa_175030 : public ScriptedAI
    {
        npc_wonsa_175030 (Creature* creature) : ScriptedAI(creature) { }

        void QuestAccept(Player* player, Quest const* quest) override
        {
            if (quest->GetQuestId() == (QUEST_H_NORTHBOUND))
            {
                if (Creature* npc = me->FindNearestCreature(166906, 10.0f, true))
                {
                    me->AddAura(SPELL_QUEST_INVISIBILITY, npc);
                    player->GetScheduler().Schedule(Milliseconds(60000), [npc] (TaskContext context)
                    {
                        npc->RemoveAllAuras();
                    });
                }

                if (TempSummon* grimaxe = player->SummonCreature(166824, Position(-249.05f, -2492.35f, 17.957f, 0.466517f), TEMPSUMMON_MANUAL_DESPAWN, 1min, 0U))
                {
                    grimaxe->AI()->Talk(0);
                    player->GetScheduler().Schedule(Milliseconds(8000), [this] (TaskContext context)
                    {
                        me->AI()->Talk(0);
                    }).Schedule(Milliseconds(12000), [this,grimaxe] (TaskContext context)
                    {
                        grimaxe->AI()->Talk(1);
                    });
                    grimaxe->SetSpeed(MOVE_RUN, player->GetSpeed(MOVE_RUN));
                    // running to mithdran
                    grimaxe->AddDelayedEvent(3000, [grimaxe](){ grimaxe->GetMotionMaster()->MovePoint(1, -238.273f, -2467.617f, 15.990f, true); });
                    grimaxe->AddDelayedEvent(7500, [grimaxe](){ grimaxe->GetMotionMaster()->MovePoint(1, -220.940f, -2470.998f, 16.520f, true); });
                    grimaxe->AddDelayedEvent(10000, [grimaxe](){ grimaxe->GetMotionMaster()->MovePoint(1, -197.318f, -2510.768f, 21.758f, true); });
                    grimaxe->AddDelayedEvent(17500, [grimaxe](){ grimaxe->GetMotionMaster()->MovePoint(1, -190.575f, -2555.820f, 27.304f, true); });
                    grimaxe->AddDelayedEvent(25000, [grimaxe](){ grimaxe->GetMotionMaster()->MovePoint(1, -173.183f, -2584.720f, 33.041f, true); });
                    grimaxe->AddDelayedEvent(30000, [grimaxe](){ grimaxe->GetMotionMaster()->MovePoint(1, -155.535f, -2596.575f, 36.171f, true); });
                    grimaxe->AddDelayedEvent(33500, [grimaxe](){ grimaxe->GetMotionMaster()->MovePoint(1, -137.381f, -2625.058f, 44.287f, true); });
                    grimaxe->AddDelayedEvent(43000, [grimaxe](){ grimaxe->GetMotionMaster()->MovePoint(1, -143.075f, -2640.596f, 48.804f, true); });
                    grimaxe->AddDelayedEvent(48000, [grimaxe]() { grimaxe->SetFacingTo(6.2f, true); });
                    grimaxe->AddDelayedEvent(50000, [grimaxe]() { grimaxe->SetEmoteState(EMOTE_STATE_KNEEL); });
                }
            }
        }

    };

// npc_Mithdran_Dawntracker_166996
    struct  npc_Mithdran_Dawntracker_166996 : public ScriptedAI
    {
        npc_Mithdran_Dawntracker_166996 (Creature* creature) : ScriptedAI(creature) {}

        void QuestAccept(Player* player, Quest const* quest) override
        {
            if ( player->IsActiveQuest(QUEST_H_DOWN_WITH_THE_QUILBOAR) && player->IsActiveQuest(QUEST_H_FORBIDDEN_QUILBOAR_NECROMANCY))
            {
                if (Creature* npc = me->FindNearestCreature(166997, 5.0f, true))
                {
                    player->GetScheduler().Schedule(Milliseconds(5000), [this] (TaskContext context)
                    {
                        me->AI()->Talk(0);

                    }).Schedule(Milliseconds(10000), [this,npc] (TaskContext context)
                    {
                        npc->AI()->Talk(1);
                        me->SetStandState(UNIT_STAND_STATE_STAND);
                        npc->SetStandState(UNIT_STAND_STATE_STAND);
                        me->GetMotionMaster()->MovePoint(1, -128.411f, -2637.33f, 48.4554f, true);
                        npc->GetMotionMaster()->MoveFollow(me, PET_FOLLOW_DIST, npc->GetFollowAngle());

                    }).Schedule(Milliseconds(13000), [this, npc] (TaskContext context)
                    {
                        me->GetMotionMaster()->MovePoint(2, -109.327f, -2646.74f, 52.5968f, true);

                    }).Schedule(Milliseconds(17000), [this, npc] (TaskContext context)
                    {
                        me->GetMotionMaster()->MovePoint(3, -81.9396f, -2644.04f, 57.433f, true);
                        me->ForcedDespawn(5000);
                        npc->ForcedDespawn(5000);
                   });
                }
            }
        }

        void QuestReward(Player* player, Quest const* quest, LootItemType type, uint32 /*opt*/) override
        {
            if (quest->GetQuestId() == QUEST_H_NORTHBOUND)
            {
                me->AI()->Talk(1);

                if (Creature* npc = me->FindNearestCreature(166824, 20.0f, true))
                {
                    npc->ForcedDespawn(1000);
                }
            }
        }
    };

enum privatecole {

    NPC_COLE_TXT0 = 0,
    NPC_COLE_TXT1 = 1,
    NPC_COLE_TXT2 = 2,
    NPC_COLE_TXT3 = 3,
    NPC_COLE_TXT4 = 4,

};

// npc_private_cole
struct npc_private_cole_160664 : public ScriptedAI
{
    npc_private_cole_160664(Creature* creature) : ScriptedAI(creature) { }
    bool talked;

    void Reset() override
    {
        ScriptedAI::Reset();
        talked = false;
    }

    void QuestAccept(Player* player, Quest const* quest) override
    {

        if (quest->GetQuestId() == QUEST_STAND_YOUR_GROUND)
        {
            Talk(NPC_COLE_TXT0);

            _scheduler.Schedule(Seconds(5), [this](TaskContext context)
            {
                me->SetFaction(14);
                me->SetReactState(REACT_AGGRESSIVE);
                Talk(NPC_COLE_TXT1);
            });
        }

        if (quest->GetQuestId() == QUEST_BRACE_FOR_IMPACT)
        {
            Talk(NPC_COLE_TXT4);
            me->GetMap()->SetZoneWeather(10424, WEATHER_STATE_EXILE_REACH_HEAVY_RAIN, 0.5f);

            if (Creature* npc = player->FindNearestCreature(NPC_GARRIC, 20.0f, true))
            {
                _scheduler.Schedule(Seconds(3), [npc](TaskContext context)
                {
                    npc->AI()->Talk(GARRIC_TXT4);
                });
            }
        }
    }

    void UpdateAI(uint32 diff) override
    {
        _scheduler.Update(diff);
    }

    void DamageTaken(Unit* attacker, uint32& damage ) override
    {
        if (Player* player = attacker->ToPlayer())
        {
            if (!talked)
                if ((me->GetHealth() < me->CountPctFromMaxHealth(60)) && (me->GetHealth() >= me->CountPctFromMaxHealth(50)))
                {
                    Talk(NPC_COLE_TXT2);
                    talked = true;
                }

            if (me->GetHealth() <= damage )
            {
                damage = 0;
                me->SetFaction(35);
                me->AttackStop();
                me->SetFullHealth();
                attacker->ToPlayer()->KilledMonsterCredit(155607, ObjectGuid::Empty);


                me->AI()->Reset();
                attacker->ToPlayer()->GetScheduler().Schedule(Seconds(3), [this](TaskContext context)
                {
                    Talk(NPC_COLE_TXT3);
                });
            }
        }

    }
private:
    TaskScheduler _scheduler;

};

//157051 npc_private_cole
struct npc_private_cole_dummy : public ScriptedAI
{
    npc_private_cole_dummy(Creature* creature) : ScriptedAI(creature)
    {
        SetCombatMovement(false); // tempfix for npc falling through the boat
    }

    void Reset() override
    {
        ScriptedAI::Reset();
    }

    void QuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_STAND_YOUR_GROUND)
        {
            me->SetFaction(14);
            me->SetReactState(REACT_AGGRESSIVE);
            me->Say(177657);

        }
    }

    void IsSummonedBy(WorldObject* summoner) override
    {
        if (Player* player = summoner->ToPlayer())
            if (player->IsActiveQuest(QUEST_STAND_YOUR_GROUND))
        {
            me->Say(177657);
        }
    }

    void JustDied(Unit* killer) override
    {
        if (Player* player = killer->ToPlayer())
            if (player->IsActiveQuest(QUEST_STAND_YOUR_GROUND))
            {
                player->KilledMonsterCredit(155607);
                me->Say(177677);
                me->DespawnOrUnsummon(10ms);
                me->SummonCreature(160664, player->GetPosition(), TEMPSUMMON_MANUAL_DESPAWN);
            }
    }
};

struct npc_dead_exile_reach : public ScriptedAI
{
    npc_dead_exile_reach(Creature* creature) : ScriptedAI(creature)
    {
        me->AddDynamicFlag(UNIT_DYNFLAG_DEAD);
        me->AddUnitFlag2(UNIT_FLAG2_FEIGN_DEATH);
        me->AddUnitState(UNIT_STATE_CANNOT_TURN);
    }
};

//168410 item_first_aid_kit
class item_first_aid_kit : public ItemScript
{
public:
    item_first_aid_kit() : ItemScript("item_first_aid_kit") { }


    bool OnUse(Player* player, Item* item, SpellCastTargets const& targets, ObjectGuid /*castId*/) override
    {
        std::list<Creature*> spellTargetsList;
        player->GetCreatureListWithEntryInGrid(spellTargetsList, 156609, 2.0f);
        player->GetCreatureListWithEntryInGrid(spellTargetsList, 156610, 2.0f);
        player->GetCreatureListWithEntryInGrid(spellTargetsList, 156612, 2.0f);
        player->GetCreatureListWithEntryInGrid(spellTargetsList, 166786, 2.0f);
        player->GetCreatureListWithEntryInGrid(spellTargetsList, 166791, 2.0f);
        player->GetCreatureListWithEntryInGrid(spellTargetsList, 166796, 2.0f);
        for (auto& targets : spellTargetsList)
        {
            targets->RemoveDynamicFlag(UNIT_DYNFLAG_DEAD);
            targets->RemoveUnitFlag2(UNIT_FLAG2_FEIGN_DEATH);
            targets->ClearUnitState(UNIT_STATE_CANNOT_TURN);

          switch (targets->GetEntry())
          {
            case 156609:
            {
                player->CastSpell(targets, SPELL_FIRST_AID);
                player->AddDelayedEvent(1000, [player]() -> void
                        {
                            player->KilledMonsterCredit(156609);
                        });
                break;
            }
            case 156612:
            {
                player->CastSpell(targets, SPELL_FIRST_AID);
                player->AddDelayedEvent(1000, [player]() -> void
                    {
                        player->KilledMonsterCredit(156612);
                    });
                break;
            }
            case 156610:
            {
                player->CastSpell(targets, SPELL_FIRST_AID);
                player->AddDelayedEvent(1000, [player]() -> void
                    {
                        player->KilledMonsterCredit(156610);
                    });
                break;
            }
            case 166786:
            {
                player->CastSpell(targets, SPELL_FIRST_AID);
                player->AddDelayedEvent(1000, [player]() -> void
                    {
                        player->KilledMonsterCredit(166786);
                    });
                break;
            }
            case 166791:
            {
                player->CastSpell(targets, SPELL_FIRST_AID);
                player->AddDelayedEvent(1000, [player]() -> void
                    {
                        player->KilledMonsterCredit(166791);
                    });
                break;
            }
            case 166796:
            {
                player->CastSpell(targets, SPELL_FIRST_AID);
                player->AddDelayedEvent(1000, [player]() -> void
                    {
                        player->KilledMonsterCredit(166796);
                    });
                break;
            }
          }
        }
        return true;
    }
};


//go_campfire_339769
struct go_campfire_339769 : public GameObjectAI
{
    go_campfire_339769(GameObject* go) : GameObjectAI(go) { }

    bool GossipHello(Player* player) override
    {
        if (player->GetQuestObjectiveProgress(QUEST_COOKING_MEAT, 2) == 5 || player->GetQuestObjectiveProgress(QUEST_H_COOKING_MEAT, 2) == 5 )
        {
            if (player->IsActiveQuest(QUEST_COOKING_MEAT) || player->IsActiveQuest(QUEST_H_COOKING_MEAT))
                player->ForceCompleteQuest(QUEST_COOKING_MEAT);

            player->AddItem(174074, 1);

        }

        return true;

    }
};


//npc_Cork_Fizzlepop_167019
struct npc_Cork_Fizzlepop_167019 : public ScriptedAI
{
    npc_Cork_Fizzlepop_167019 (Creature* c) : ScriptedAI(c) { }

    void Reset()
    {
        FIZZLEPOP_SAY = false;
    }

    void QuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_H_THE_CHOPPY_BOOSTER_MK5)
        {

            player->SummonCreature(167905, Position(106.282f, -2415.671f, 90.41f, 3.601f), TEMPSUMMON_MANUAL_DESPAWN);
            player->FindNearestCreature(167905, 10.0f, true)->SetHover(true);
            player->FindNearestCreature(167905, 10.0f, true)->SetHoverHeight(3);

            player->GetScheduler().Schedule(Milliseconds(2000), [this](TaskContext context)
                {
                    Creature* fake_choppy = me->FindNearestCreature(167905, 10.0f, true);
                    me->SelectNearbyTarget(fake_choppy, 10.0f);
                    me->CastSpell(fake_choppy, SPELL_RESIZER_CHANNELING);

                }).Schedule(Milliseconds(5000), [this, player](TaskContext context)
                    {
                        Creature* fake_choppy = me->FindNearestCreature(167905, 10.0f, true);
                        fake_choppy->CastSpell(fake_choppy, SPELL_RESIZER_HIT_TOSMALL);
                        me->AI()->Talk(0);

                    }).Schedule(Milliseconds(10000), [this, player](TaskContext context)
                        {

                            Creature* fake_choppy = me->FindNearestCreature(167905, 10.0f, true);
                            fake_choppy->RemoveAura(SPELL_RESIZER_HIT_TOSMALL);
                            fake_choppy->CastSpell(fake_choppy, SPELL_INCREASE_SIZE);
                            me->AI()->Talk(1);

                        }).Schedule(Milliseconds(12000), [this, player](TaskContext context)
                            {
                                Creature* fake_choppy = me->FindNearestCreature(167905, 10.0f, true);
                                fake_choppy->RemoveAura(SPELL_INCREASE_SIZE);
                                me->AI()->Talk(2);

                            }).Schedule(Milliseconds(15000), [this](TaskContext context)
                                {
                                    Creature* fake_choppy = me->FindNearestCreature(167905, 10.0f, true);
                                    fake_choppy->DespawnOrUnsummon(2s);

                                }).Schedule(Milliseconds(17000), [this, player](TaskContext context)
                                    {
                                        player->SummonCreature(167027, Position(103.419f, -2417.24f, 92.4075f, 0.55373f), TEMPSUMMON_MANUAL_DESPAWN);
                                    });

        }

        if (quest->GetQuestId() == QUEST_H_RESIZING_THE_SITUATION)
        {
            player->CastSpell(player, SPELL_H_RESIZING_BACKPACK); // apply the magical resizer unit to the players back (bro i found this spell hhaha)

            // add phase for sure
            PhasingHandler::AddPhase(player, PHASE_GENERIC, true);

            me->AI()->Talk(3);
            me->GetMotionMaster()->MoveFollow(player, PET_FOLLOW_DIST, me->GetFollowAngle());
        }

        if (quest->GetQuestId() == QUEST_H_THE_REDEATHER)
        {
            me->AI()->Talk(4);
            me->SetWalk(false);
            me->GetMotionMaster()->MovePoint(1, 100.742f, -2417.84f, 90.3844f, true, 3.81584f);
            me->AddDelayedEvent(3000, [this, player]() -> void
                {
                    player->SummonCreature(167142, Position(103.419f, -2417.24f, 93.4075f, 0.55373f), TEMPSUMMON_TIMED_DESPAWN, 5min, 0);
                });
        }
    }

    void MoveInLineOfSight(Unit* who) override
    {
        // hide when old fizzlepop coming
        if (Creature* oldme = me->FindNearestCreature(167008, 100.0f))
            if (oldme->IsInDist(me, 100.0f))
                me->AddAura(SPELL_QUEST_INVISIBILITY, me);
        // show when old fizzlepop going to despawn
        if (Creature* oldme = me->FindNearestCreature(167008, 100.0f))
            if (oldme->IsInDist(me, 1.0f))
                me->RemoveAura(SPELL_QUEST_INVISIBILITY);
        // another fizzlepop due to backpack aura - removing
        if (Creature* fizzlepop_duplication = me->FindNearestCreature(167915, 100.0f))
            if (fizzlepop_duplication->IsInDist(me, 20.0f))
                fizzlepop_duplication->AddAura(SPELL_QUEST_INVISIBILITY, fizzlepop_duplication);

        if (Player* player = who->ToPlayer())
        {
           if (player->IsInDist(me, 10.0f))
          {
           if (player->IsActiveQuest(QUEST_H_RESIZING_THE_SITUATION))
            {
                if (player->GetQuestObjectiveProgress(QUEST_H_RESIZING_THE_SITUATION, 0) == 1)
                {
                    if (!FIZZLEPOP_SAY)
                    {
                        Talk(5);
                        FIZZLEPOP_SAY = true;
                    }
                }
                if (player->GetQuestObjectiveProgress(QUEST_H_RESIZING_THE_SITUATION, 0) == 2)
                {
                    if (!FIZZLEPOP_SAY)
                    {
                        Talk(6);
                        FIZZLEPOP_SAY = true;
                    }
                }
                if (player->GetQuestObjectiveProgress(QUEST_H_RESIZING_THE_SITUATION, 0) == 3)
                {
                    if (!FIZZLEPOP_SAY)
                    {
                        Talk(7);
                        FIZZLEPOP_SAY = true;
                        me->SetWalk(false);
                        me->GetMotionMaster()->MovePoint(1, 100.742f, -2417.84f, 90.3844f, true, 3.81584f);
                    }
                }
              }

           if (player->GetQuestStatus(QUEST_H_RESIZING_THE_SITUATION) == QUEST_STATUS_NONE)
           {
               me->GetMotionMaster()->MoveTargetedHome();
               player->RemoveAura(SPELL_H_RESIZING_BACKPACK);
           }

           if (player->GetQuestStatus(QUEST_H_RESIZING_THE_SITUATION) == QUEST_STATUS_COMPLETE)
           {
               me->GetMotionMaster()->MoveTargetedHome();
               player->RemoveAura(SPELL_H_RESIZING_BACKPACK);
           }
          }

        }
     }

    void QuestReward(Player* player, Quest const* quest, LootItemType type, uint32 /*opt*/) override
    {
        if (quest->GetQuestId() == QUEST_H_DOWN_WITH_THE_QUILBOAR)
        {
            //Talk(8); this talk moved from here to fizzlepop walk event 196008 version of fizz
            if (Creature* grimaxe = player->FindNearestCreature(167021, 10.0f, true))
            {
                player->GetScheduler().Schedule(Milliseconds(6000), [grimaxe] (TaskContext context)
                {
                    grimaxe->AI()->Talk(0);
                }).Schedule(Milliseconds(11000), [this] (TaskContext context)
                {
                    me->AI()->Talk(9);
                });
            }
        }
    }
};



// npc_Choppy_Booster_167027 (for observe zombies)
struct npc_Choppy_Booster_167027: public EscortAI
{
    npc_Choppy_Booster_167027(Creature* creature) : EscortAI(creature)
    {
        me->CanFly();
        me->SetFlying(true);
    }

private:
    Vehicle* vehicle;


    void IsSummonedBy(WorldObject* summoner) override
    {
        if (Player* player = summoner->ToPlayer())
        {
            if (player->IsActiveQuest(QUEST_H_THE_CHOPPY_BOOSTER_MK5))
            {
                // TODO: change to correct vehicle where the passenger hanging
                vehicle = me->GetVehicleKit();
                summoner->ToUnit()->EnterVehicle(me);
                me->CastSpell(summoner, SPELL_ROPE);
                me->SetFaction(summoner->GetFaction());
                PhasingHandler::InheritPhaseShift(me, summoner);
                me->GetMotionMaster()->MovePoint(1, 264.0f, -2310.0f, 117.0f, true);
                me->AddDelayedEvent(2000, [this, player]() -> void
                    {
                        player->PlayConversation(14517);
                    });

                me->AddDelayedEvent(25000, [this, player]() -> void
                    {
                        player->ExitVehicle();
                        player->CastSpell(player, SPELL_GORGROTH_SCENE);
                        player->KilledMonsterCredit(167027);
                        player->EnterVehicle(me);
                    });

                me->AddDelayedEvent(50000, [this]() -> void
                    {
                        me->GetMotionMaster()->MovePoint(2, 108.609f, -2413.905f, 95.530f, true);
                    });
                me->AddDelayedEvent(55000, [this, player]() -> void
                    {
                        player->PlayConversation(14520);
                    });

                me->AddDelayedEvent(75000, [this, player]() -> void
                    {
                        player->NearTeleportTo(102.3729f, -2418.46f, 91.2849f, 3.6666f, false);
                        me->DespawnOrUnsummon(1s);
                    });

            }
        }

    }
};

// npc_Choppy_Booster_167142 (for killing zombies)
// INSERT INTO `creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `name`, `femaleName`, `subname`, `TitleAlt`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `HealthScalingExpansion`, `RequiredExpansion`, `VignetteID`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `dmgschool`, `BaseAttackTime`, `RangeAttackTime`, `BaseVariance`, `RangeVariance`, `unit_class`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `family`, `trainer_class`, `type`, `type_flags`, `type_flags2`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `spell6`, `spell7`, `spell8`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `InhabitType`, `HoverHeight`, `HealthModifier`, `HealthModifierExtra`, `ManaModifier`, `ManaModifierExtra`, `ArmorModifier`, `DamageModifier`, `ExperienceModifier`, `RacialLeader`, `movementId`, `CreatureDifficultyID`, `WidgetSetID`, `WidgetSetUnitConditionID`, `RegenHealth`, `mechanic_immune_mask`, `spell_school_immune_mask`, `flags_extra`, `ScriptName`, `VerifiedBuild`) VALUES (167142, 0, 0, 0, 0, 0, 'Choppy Booster Mk. 5', 'Choppy Booster Mk. 5', NULL, NULL, 'vehichlecursor', 0, 1, 10, 0, 0, 0, 35, 16777216, 1, 1, 1, 0, 0, 2000, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 9, 525312, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 294128, 0, 0, 0, 0, 338564, 0, 0, 463, 0, 0, '', 0, 7, 5, 1, 1, 1, 1, 1, 3, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 'npc_Choppy_Booster_167142', 36949);
struct npc_Choppy_Booster_167142 : public EscortAI
{
    npc_Choppy_Booster_167142(Creature* creature) : EscortAI(creature)
    {
        me->CanFly();
        me->SetFlying(true);
        me->SetSpeed(MOVE_FLIGHT, 10);
    }

private:
    Vehicle* vehicle;

    void IsSummonedBy(WorldObject* summoner) override
    {
        if (Player* player = summoner->ToPlayer())
        {
            if (player->IsActiveQuest(QUEST_H_THE_REDEATHER))
            {
                vehicle = me->GetVehicleKit();
                summoner->ToUnit()->EnterVehicle(me);
                player->KilledMonsterCredit(167142);

            }
        }
    }

};

// npc_Shuja_Grimaxe_167126
struct npc_Shuja_Grimaxe_167126 : public ScriptedAI
{
    npc_Shuja_Grimaxe_167126(Creature* creature) : ScriptedAI(creature) { }

    void Reset()
    {
        say = false;
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
            if (player->IsInDist(me, 10.0f))
                if (player->IsActiveQuest(QUEST_H_THE_REDEATHER))
                    if (player->GetQuestObjectiveProgress(QUEST_H_THE_REDEATHER, 1))
                    {
                        if (!say)
                        {
                            PhasingHandler::RemovePhase(player, PHASE_ZOMBIE, true);
                            me->RemoveAllAuras();
                            me->NearTeleportTo(230.912f, -2297.61f, 80.7119f, 1.31069f, false);
                            if (Creature* grimaxe = player->FindNearestCreature(167145, 15.0f, true))
                            {
                                player->GetScheduler().Schedule(Milliseconds(3000), [this, grimaxe] (TaskContext context)
                                {
                                    grimaxe->AI()->Talk(0);
                                }).Schedule(Milliseconds(6000), [this, player] (TaskContext context)
                                {
                                    me->AI()->Talk(0);
                                }).Schedule(Milliseconds(12000), [player, grimaxe] (TaskContext context)
                                {
                                    grimaxe->AI()->Talk(1);
                                    //player->ForceCompleteQuest(QUEST_H_THE_REDEATHER);
                                });
                            }
                            say = true;
                        }
                    }
    }
    bool say;

    void QuestReward(Player* player, Quest const* quest, LootItemType type, uint32 /*opt*/) override
    {
        if (quest->GetQuestId() == QUEST_H_THE_REDEATHER)
        {
            if (Creature* grimaxe = player->FindNearestCreature(167145, 50.0f, true))
            {
                grimaxe->ForcedDespawn(100);
            }

            me->ForcedDespawn(100);

            // placing the player to the right location of the scene
            player->NearTeleportTo(229.736f, -2296.816f, 80.894f, 1.03f, false);

        }
    }
};

// npc_Shuja_Grimaxe_167219
struct npc_Shuja_Grimaxe_167219 : public ScriptedAI
{
    npc_Shuja_Grimaxe_167219(Creature* creature) : ScriptedAI(creature) { }

    void QuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_H_THE_HARPY_PROBLEM)
        {
            Talk(0);

            me->GetMotionMaster()->MovePoint(1, 332.667f, -2358.745f, 96.335f, true);

            me->AddDelayedEvent(6000, [this]() -> void
                {
                    me->GetMotionMaster()->MovePoint(1, 375.262f, -2399.416f, 112.083f, true);
                });
            me->AddDelayedEvent(12000, [this]() -> void
                {
                    me->GetMotionMaster()->MovePoint(1, 392.667f, -2440.18f, 125.475f, true);
                });
        }
    }

};

enum MessagetoBaseEvents
{
    EVENT_WAYPOINT_REACHED,
};

// npc_Shuja_Grimaxe_167290_q59947
struct npc_Shuja_Grimaxe_167290 : public EscortAI
{
    npc_Shuja_Grimaxe_167290(Creature* creature) : EscortAI(creature) { }

    void QuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_H_MESSAGE_TO_BASE)
        {
            Talk(0);
            SetRun(true);
            player->GetScheduler().Schedule(Milliseconds(3500), [this, player] (TaskContext context)
            {
                Start(true, true);
                BoRunBase(player);
                HerbertRunBase(player);
            });
        }
    }


    void BoRunBase(Player* player)
    {
        if (Creature* bo = player->FindNearestCreature(167291, 20.0f, true))
        {
            bo->SetWalk(false);
            player->GetScheduler().Schedule(Milliseconds(3000), [this, bo] (TaskContext context)
            {
                bo->GetMotionMaster()->MovePoint(1, MessagetoBase[0]);
            }).Schedule(Milliseconds(6000), [this, bo] (TaskContext context)
            {
                bo->GetMotionMaster()->MovePoint(1, MessagetoBase[1]);
            }).Schedule(Milliseconds(10000), [this, bo] (TaskContext context)
            {
                bo->GetMotionMaster()->MovePoint(1, MessagetoBase[2]);
            }).Schedule(Milliseconds(14000), [this, bo] (TaskContext context)
            {
                bo->GetMotionMaster()->MovePoint(1, MessagetoBase[3]);
            }).Schedule(Milliseconds(20000), [this, bo] (TaskContext context)
            {
                bo->GetMotionMaster()->MovePoint(1, MessagetoBase[4]);
            }).Schedule(Milliseconds(26000), [this, bo] (TaskContext context)
            {
                bo->GetMotionMaster()->MovePoint(1, MessagetoBase[5]);
            }).Schedule(Milliseconds(32000), [this, bo] (TaskContext context)
            {
                bo->GetMotionMaster()->MovePoint(1, MessagetoBase[6]);
            }).Schedule(Milliseconds(35000), [this, bo] (TaskContext context)
            {
                bo->GetMotionMaster()->MovePoint(1, 187.056f, -2279.94f, 81.8464f, false, 5.02938f);
            });
        }
    }

    void HerbertRunBase(Player* player)
    {
        if (Creature* herbert = player->FindNearestCreature(167182, 20.0f, true))
        {
            herbert->SetWalk(false);
            herbert->SetFlying(false);
            player->GetScheduler().Schedule(Milliseconds(4000), [this, herbert] (TaskContext context)
            {
                herbert->GetMotionMaster()->MovePoint(2, MessagetoBase[0]);
            }).Schedule(Milliseconds(7000), [this, herbert] (TaskContext context)
            {
                herbert->GetMotionMaster()->MovePoint(2, MessagetoBase[1]);
            }).Schedule(Milliseconds(11000), [this, herbert] (TaskContext context)
            {
                herbert->GetMotionMaster()->MovePoint(2, MessagetoBase[2]);
            }).Schedule(Milliseconds(17000), [this, herbert] (TaskContext context)
            {
                herbert->GetMotionMaster()->MovePoint(2, MessagetoBase[3]);
            }).Schedule(Milliseconds(23000), [this, herbert] (TaskContext context)
            {
                herbert->GetMotionMaster()->MovePoint(2, MessagetoBase[4]);
            }).Schedule(Milliseconds(30000), [this, herbert] (TaskContext context)
            {
                herbert->GetMotionMaster()->MovePoint(2, MessagetoBase[5]);
            }).Schedule(Milliseconds(36000), [this, herbert] (TaskContext context)
            {
                herbert->GetMotionMaster()->MovePoint(2, MessagetoBase[6]);
            }).Schedule(Milliseconds(40000), [this, herbert] (TaskContext context)
            {
                herbert->GetMotionMaster()->MovePoint(2, 182.311f, -2281.61f, 81.8466f, false, 4.7545f);
            });

        }
    }
};

// npc_Bo_167291
struct npc_Bo_167291 : public ScriptedAI
{
    npc_Bo_167291(Creature* creature) : ScriptedAI(creature) { }

    void Reset()
    {
        say = false;
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
        {
            if (player->IsInDist(me, 20.0f))
            {
                if (player->IsActiveQuest(QUEST_H_THE_HARPY_PROBLEM))
                    if (!say)
                    {
                        Talk(0);

                        player->GetScheduler().Schedule(Milliseconds(4000), [player] (TaskContext context)
                        {
                            if (Creature* shuja = player->FindNearestCreature(167219, 20.0f, true))
                                shuja->AI()->Talk(1);
                        });
                        say = true;
                    }
            }

        }
    }

    void QuestAccept(Player* player, Quest const* quest) override
    {
        if (player->IsActiveQuest(QUEST_H_THE_RESCUE_OF_HERBERT_GLOOMBURST) && player->IsActiveQuest(QUEST_H_HARPY_CULLING) && player->IsActiveQuest(QUEST_H_PURGE_THE_TOTEMS))
            if (Creature* shuja = player->FindNearestCreature(167290, 10.0f, true))
            {
                shuja->AI()->Talk(1);
                player->GetScheduler().Schedule(Milliseconds(4000), [this] (TaskContext context)
                {
                    me->AI()->Talk(1);
                });
            }
    }

    void QuestReward(Player* player, Quest const* quest, LootItemType type, uint32 /*opt*/) override
    {
        if (Creature* shuja = player->FindNearestCreature(167219, 20.0f, true))
        {
            shuja->DespawnOrUnsummon(1s);
            shuja->SetRespawnDelay(10);
        }
    }
    bool say;
};

// npc_Herbert_Gloomburst_167182
struct npc_Herbert_Gloomburst_167182 : public ScriptedAI
{
    npc_Herbert_Gloomburst_167182(Creature* creature) : ScriptedAI(creature) { }

    uint32 bloodbeak = 153964;
    uint32 harpy_ambusher = 155192;
    uint32 hunting_worg = 169162;
    bool triggered = false;

    bool GossipHello(Player* player) override
    {        
        if (player->IsActiveQuest(QUEST_H_THE_RESCUE_OF_HERBERT_GLOOMBURST) && triggered == false)
            AddGossipItemFor(player, GossipOptionIcon::None, "I'll fight the harpies that come. Can you use your magic to break free?", GOSSIP_SENDER_MAIN, 0);
        else
            player->PrepareQuestMenu(me->GetGUID());
        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, me->GetGUID());
        return true;
    }

    bool GossipSelect(Player* player, uint32 sender, uint32 action) override
    {
        switch (action)
        {
        case 0:
            me->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
            me->AddNpcFlag(UNIT_NPC_FLAG_QUESTGIVER);
            me->Say(196511);
            me->PlayDirectSound(157187, player, 196511);
            FightingBloodBeak(player);
            triggered = true;
            break;
        }
        return true;
    }

    void QuestReward(Player* player, Quest const* quest, LootItemType type, uint32 /*opt*/) override
    {
        if (quest->GetQuestId() == QUEST_H_THE_RESCUE_OF_HERBERT_GLOOMBURST)
        {
            me->RemoveAura(305513);
            if (me->IsHovering())
            {
                me->SetHoverHeight(1);
                me->SetHover(false);
            }
            me->GetMotionMaster()->MovePoint(1, 495.411f, -2354.56f, 160.3297f, true);
            me->Say(196516);
            me->PlayDirectSound(157190, player, 196516);
            me->ForcedDespawn(3000);
        }
    }

    void FightingBloodBeak(Player* player)
    {
        me->AddDelayedEvent(5000, [this, player]() -> void
            {
                TempSummon* worg1 = player->SummonCreature(hunting_worg, Position(477.835f, -2381.51f, 156.771f, 1.1187f), TEMPSUMMON_TIMED_DESPAWN, 15s, 0U);
                worg1->GetMotionMaster()->MovePoint(1, 494.811f, -2359.18f, 159.993f, true);
                TempSummon* harpy1 = player->SummonCreature(harpy_ambusher, Position(500.832f, -2331.03f, 164.318f, 4.25644f), TEMPSUMMON_TIMED_DESPAWN, 15s, 0U);
                harpy1->GetMotionMaster()->MovePoint(1, 494.811f, -2359.18f, 159.993f, true);
            });
        me->AddDelayedEvent(20000, [this, player]() -> void
            {
                TempSummon* worg2 = player->SummonCreature(hunting_worg, Position(472.838f, -2351.12f, 162.377f, 5.558f), TEMPSUMMON_TIMED_DESPAWN, 15s, 0U);
                worg2->GetMotionMaster()->MovePoint(1, 494.811f, -2359.18f, 159.993f, true);
                TempSummon* harpy2 = player->SummonCreature(harpy_ambusher, Position(492.376f, -2397.87f, 154.462f, 1.52499f), TEMPSUMMON_TIMED_DESPAWN, 15s, 0U);
                harpy2->GetMotionMaster()->MovePoint(1, 494.811f, -2359.18f, 159.993f, true);
            });
        me->AddDelayedEvent(30000, [this, player]() -> void
            {
                if (TempSummon* bo1 = player->SummonCreature(167300, Position(498.74f, -2359.76f, 159.924f, 4.6364f), TEMPSUMMON_TIMED_DESPAWN, 15s, 0U))
                {
                    bo1->AI()->Talk(0);
                    player->GetScheduler().Schedule(Milliseconds(30000), [this, bo1](TaskContext context)
                        {
                            bo1->AI()->Talk(1);
                        });
                }
                if (TempSummon* shuja1 = player->SummonCreature(167301, Position(490.437f, -2359.71f, 159.975f, 3.96887f), TEMPSUMMON_TIMED_DESPAWN, 15s, 0U))
                {
                    player->GetScheduler().Schedule(Milliseconds(3000), [this, shuja1](TaskContext context)
                        {
                            shuja1->AI()->Talk(0);
                        }).Schedule(Milliseconds(33000), [this, shuja1](TaskContext context)
                            {
                                shuja1->AI()->Talk(1);
                            });
                }

            });

        me->AddDelayedEvent(40000, [this, player]() -> void
            {
                TempSummon* worg3 = player->SummonCreature(hunting_worg, Position(477.835f, -2381.51f, 156.771f, 1.1187f), TEMPSUMMON_TIMED_DESPAWN, 15s, 0U);
                worg3->GetMotionMaster()->MovePoint(1, 494.811f, -2359.18f, 159.993f, true);
                TempSummon* harpy3 = player->SummonCreature(harpy_ambusher, Position(500.832f, -2331.03f, 164.318f, 4.25644f), TEMPSUMMON_TIMED_DESPAWN, 15s, 0U);
                harpy3->GetMotionMaster()->MovePoint(1, 494.811f, -2359.18f, 159.993f, true);
            });
        me->AddDelayedEvent(55000, [this, player]() -> void
            {
                TempSummon* worg4 = player->SummonCreature(hunting_worg, Position(472.838f, -2351.12f, 162.377f, 5.558f), TEMPSUMMON_TIMED_DESPAWN, 15s, 0U);
                worg4->GetMotionMaster()->MovePoint(1, 494.811f, -2359.18f, 159.993f, true);
                TempSummon* harpy4 = player->SummonCreature(harpy_ambusher, Position(492.376f, -2397.87f, 154.462f, 1.52499f), TEMPSUMMON_TIMED_DESPAWN, 15s, 0U);
                harpy4->GetMotionMaster()->MovePoint(1, 494.811f, -2359.18f, 159.993f, true);
            });
        me->AddDelayedEvent(70000, [this, player]() -> void
            {
                TempSummon* bloodbeak1 = player->SummonCreature(bloodbeak, Position(491.975f, -2414.46f, 162.734f, 1.502485f), TEMPSUMMON_TIMED_DESPAWN, 15s, 0U);
                bloodbeak1->GetMotionMaster()->MovePoint(1, 494.811f, -2359.18f, 159.993f, true);
            });
    }
};

// npc_Lana_Jordan_167221
struct npc_Lana_Jordan_167221 : public EscortAI
{
    npc_Lana_Jordan_167221(Creature* creature) : EscortAI(creature) { }

    void QuestReward(Player* player, Quest const* quest, LootItemType /*type*/, uint32 /*opt*/) override
    {
        if (quest->GetQuestId() == QUEST_H_WESTWARD_BOUND)
        {
            
            if (Creature* wonsa = player->FindNearestCreature(167222, 10.0f, true))
            {
                wonsa->ForcedDespawn(100);
            }
            
            me->ForcedDespawn(100);
        }
    }

    void QuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_H_WESTWARD_BOUND)
        {
            Talk(0);
            me->RemoveNpcFlag(UNIT_NPC_FLAG_QUESTGIVER);
            if (Creature* wonsa = player->FindNearestCreature(167222, 10.0f, true))
            {
                wonsa->SetWalk(false);
                wonsa->GetMotionMaster()->MovePoint(1, 151.681f, -2305.85f, 86.9756f, true);
                player->GetScheduler().Schedule(Milliseconds(2000), [wonsa] (TaskContext context)
                {
                    wonsa->GetMotionMaster()->MovePoint(2, 137.013f, -2297.01f, 91.3035f, true);

                })
                    .Schedule(Milliseconds(4500), [wonsa] (TaskContext context)
                {
                    wonsa->GetMotionMaster()->MovePoint(3, 120.97f, -2284.04f, 96.3998f, true);

                })
                    .Schedule(Milliseconds(7000), [wonsa] (TaskContext context)
                {
                    wonsa->GetMotionMaster()->MovePoint(4, 106.753f, -2267.51f, 96.8173f, true);

                })
                    .Schedule(Milliseconds(10500), [wonsa] (TaskContext context)
                {
                    wonsa->GetMotionMaster()->MovePoint(5, 89.954f, -2249.46f, 94.7153f, true);

                });

            }

            player->GetScheduler().Schedule(Milliseconds(2000), [this] (TaskContext context)
            {
                SetRun(true);
                Start(true, false);
            });
        }
    }

    void WaypointReached(uint32 waypointId, uint32 /*pathId*/) override
    {
        if (waypointId == 5)
        {
            Talk(1);
            me->AddNpcFlag(UNIT_NPC_FLAG_QUESTGIVER);
        }
    }

};

//npc_Lana_Jordan_167225
struct npc_Lana_Jordan_167225 : public ScriptedAI
{
    npc_Lana_Jordan_167225(Creature* creature) : ScriptedAI(creature) { }

    void QuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_H_WHO_LURKS_IN_THE_PIT)
        {
            Talk(1);
            player->GetScheduler().Schedule(Milliseconds(5000), [this, player] (TaskContext context)
            {
                if (Creature* wonsa = me->FindNearestCreature(167226, 10.0f, true))
                    wonsa->AI()->Talk(0);
            });
        }
    }

};

// npc_Crenna_Earth_Daughter_167250
struct npc_Crenna_Earth_Daughter_167250 : public ScriptedAI
{
    npc_Crenna_Earth_Daughter_167250(Creature* creature) : ScriptedAI(creature) { }

    void Reset()
    {
        say = false;
        say2 = false;
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
        {
            if (player->IsInDist(me, 100.0f))
                if (player->IsActiveQuest(QUEST_H_WHO_LURKS_IN_THE_PIT))
                    if (player->GetQuestObjectiveProgress(QUEST_H_WHO_LURKS_IN_THE_PIT, 0) == 5)
                        if (!say)
                        {
                            player->PlayConversation(14540);
                            say = true;
                        }
            if (player->IsInDist(me, 20.0f))
                if (player->IsActiveQuest(QUEST_H_WHO_LURKS_IN_THE_PIT))
                    if (player->GetQuestObjectiveProgress(QUEST_H_WHO_LURKS_IN_THE_PIT, 0) == 5)
                        if (!say2)
                        {
                            if (Creature* hrun = player->FindNearestCreature(156900, 20.0f, true))
                            {
                                hrun->AI()->Talk(0);
                                player->GetScheduler().Schedule(Milliseconds(3000), [this] (TaskContext context)
                                {
                                    me->AI()->Talk(0);
                                });
                            }
                            say2 = true;
                        }
        }

    }

    bool say;
    bool say2;
};

// npc_Crenna_Earth_Daughter_167254
struct npc_Crenna_Earth_Daughter_167254 : public ScriptedAI
{
    npc_Crenna_Earth_Daughter_167254(Creature* creature) : ScriptedAI(creature) { }

    void OnSpellClick(Unit* clicker, bool result) override
    {
        if (Player* player = clicker->ToPlayer())
            if (player->IsActiveQuest(QUEST_H_WHO_LURKS_IN_THE_PIT))
            {
                player->KilledMonsterCredit(167254);
                player->GetScheduler().Schedule(Milliseconds(50000), [this, player] (TaskContext context)
                {
                        player->NearTeleportTo(108.546f, -2271.868f, 97.172f, 5.64f, false);
                });

            }
    }

};

//npc_Warlord_Breka_Grimaxe_167212
struct npc_Warlord_Breka_Grimaxe_167212 : public EscortAI
{
    npc_Warlord_Breka_Grimaxe_167212(Creature* creature) : EscortAI(creature) { }

    ObjectGuid   m_shujaGUID;
    ObjectGuid   m_herbertGUID;
    ObjectGuid   m_crennaGUID;
    ObjectGuid   m_playerGUID;

    void Initialize()
    {
        m_shujaGUID = ObjectGuid::Empty;
        m_herbertGUID = ObjectGuid::Empty;
        m_crennaGUID = ObjectGuid::Empty;
        m_playerGUID = ObjectGuid::Empty;
    }

    void GetFollowerMember()
    {
        if (!m_shujaGUID)
            if (Creature* shuja = me->FindNearestCreature(167290, 20.0f, true))
                m_shujaGUID = shuja->GetGUID();

        if (!m_herbertGUID)
            if (Creature* herbert = me->FindNearestCreature(167182, 20.0f, true))
                m_herbertGUID = herbert->GetGUID();

        if (!m_crennaGUID)
            if (Creature* crenna = me->FindNearestCreature(166575, 20.0f, true))
                m_crennaGUID = crenna->GetGUID();

    }

    void QuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_H_STOCKING_UP_ON_SUPPLIES)
        {
            me->AddDelayedEvent(4000, [this, player]() -> void
                {
                    player->ForceCompleteQuest(QUEST_H_STOCKING_UP_ON_SUPPLIES);
                });
        }

        if (quest->GetQuestId() == QUEST_H_TO_DARKMAUL_CITADEL)
        {
            Talk(0);
            Start(true, false);
            GetFollowerMember();

            if (player->ToPlayer())
                m_playerGUID = player->GetGUID();                   //write_player

            if (Creature* shuja = ObjectAccessor::GetCreature(*me, m_shujaGUID))
                shuja->GetMotionMaster()->MoveFollow(me, PET_FOLLOW_DIST, shuja->GetFollowAngle(), MOTION_SLOT_ACTIVE);

            if (Creature* herbert = ObjectAccessor::GetCreature(*me, m_herbertGUID))
                herbert->GetMotionMaster()->MoveFollow(me, 3.0f, herbert->GetFollowAngle(), MOTION_SLOT_ACTIVE);

            if (Creature* crenna = ObjectAccessor::GetCreature(*me, m_crennaGUID))
                crenna->GetMotionMaster()->MoveFollow(me, PET_FOLLOW_DIST, crenna->GetFollowAngle(), MOTION_SLOT_ACTIVE);
        }
    }

    void WaypointReached(uint32 waypointId, uint32 /*pathId*/) override
    {
        if (waypointId == 13)
        {
            if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))       //call_player
            {
                PhasingHandler::AddPhase(player, 13080, true);
                player->ForceCompleteQuest(QUEST_H_TO_DARKMAUL_CITADEL);
            }
            if (Creature* shuja = ObjectAccessor::GetCreature(*me, m_shujaGUID))
                shuja->ForcedDespawn();
            if (Creature* herbert = ObjectAccessor::GetCreature(*me, m_herbertGUID))
                herbert->ForcedDespawn();
            if (Creature* crenna = ObjectAccessor::GetCreature(*me, m_crennaGUID))
                crenna->ForcedDespawn();
        }
    }

    void QuestReward(Player* player, Quest const* quest, LootItemType /*type*/, uint32 /*opt*/) override
    {
        if ((player->GetQuestStatus(QUEST_H_MESSAGE_TO_BASE) == QUEST_STATUS_REWARDED) || (player->GetQuestStatus(QUEST_H_WESTWARD_BOUND) == QUEST_STATUS_REWARDED))
        {
            me->AddAura(SPELL_QUEST_INVISIBILITY, me);
            if (player->HasAura(SPELL_SEE_QUEST_INVISIBILITY))
                player->RemoveAura(SPELL_SEE_QUEST_INVISIBILITY);
        }
    }

};

// npc_Monstrous_Cadaver_157091
struct npc_Monstrous_Cadaver_157091 : public ScriptedAI
{
    npc_Monstrous_Cadaver_157091 (Creature* creature) : ScriptedAI(creature) { }

    void Reset()
    {
        say = false;
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
            if (player->IsInDist(me, 100.0f))
                if (player->IsActiveQuest(QUEST_H_THE_REDEATHER))
                    if (player->GetQuestObjectiveProgress(QUEST_H_THE_REDEATHER, 2) == 8)
                    {
                        PhasingHandler::RemovePhase(player, PHASE_ZOMBIE, true);
                        FizzPlayConversation(player);
                    }
    }


    void FizzPlayConversation(Player* player)
    {
        if (!say)
        {
            player->PlayConversation(14521);
            player->GetScheduler().Schedule(Milliseconds(6000), [this, player] (TaskContext context)
            {
                //player->NearTeleportTo(263.819f, -2290.56f, 80.5917f, 2.115129f, false);

                if (TempSummon* grimaxe = player->SummonCreature(167145, Position(264.819f, -2291.56f, 80.5917f, 2.115129f), TEMPSUMMON_TIMED_DESPAWN, 5min, 0U))
                {
                    //grimaxe->GetMotionMaster()->MoveFollow(player, PET_FOLLOW_DIST, grimaxe->GetFollowAngle());            //follower_player

                    player->GetScheduler().Schedule(Milliseconds(3000), [player, grimaxe] (TaskContext context)
                    {
                        if (Creature* Shuja = player->FindNearestCreature(167126, 50.0f, true))
                        {
                            Shuja->AI()->Talk(1);

                            player->GetScheduler().Schedule(Milliseconds(4000), [grimaxe] (TaskContext context)
                            {
                                grimaxe->AI()->Talk(2);
                                grimaxe->GetMotionMaster()->MovePoint(1, 235.297f, -2268.614f, 80.895f, true);
                            });
                        }
                    });
                }

            });
        }

         say = true;
    }

    void JustDied(Unit* killer) override
    {
        if (Vehicle* vehicle = killer->GetVehicleKit())
            if (Unit* passenger = vehicle->GetPassenger(0))
                if (Player* player = passenger->ToPlayer())
                    player->KilledMonsterCredit(165150);
    }

    bool say;
};

//npc_lindie_springstock_149899
struct npc_lindie_springstock_149899 : public ScriptedAI
{
    bool talked;
    npc_lindie_springstock_149899(Creature* c) : ScriptedAI(c) { }

    void Reset() override
    {
        ScriptedAI::Reset();
        talked = false;
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
        {
            if (player->GetQuestStatus(QUEST_DOWN_WITH_THE_QUILBOAR) == QUEST_STATUS_COMPLETE)
            {
                if (player->IsInDist(me, 20.0f) && !talked)
                {
                    talked = true;

                    Talk(0);

                    if (Creature* captainGarrick = me->FindNearestCreature(156280, 50.0f)) {
                        player->GetScheduler().Schedule(Milliseconds(6000), [captainGarrick] (TaskContext context)
                        {
                            captainGarrick->AI()->Talk(5);
                        })
                        .Schedule(Milliseconds(11000), [this] (TaskContext context)
                        {
                            me->AI()->Talk(1);
                        });
                    }
                }
            }
        }
    }

    void QuestReward(Player* player, Quest const* quest, LootItemType /*type*/, uint32 /*opt*/) override
    {
        if (quest->GetQuestId() == QUEST_DOWN_WITH_THE_QUILBOAR)
            me->AI()->Reset();

        if (quest->GetQuestId() == QUEST_THE_SCOUT_O_MATIC_5000) {
            Talk(5);
            if (Creature* captainGarrick = me->FindNearestCreature(156280, 50.0f)) {
                player->GetScheduler().Schedule(Milliseconds(6000), [captainGarrick] (TaskContext context)
                {
                    captainGarrick->AI()->Talk(8);
                })
                .Schedule(Milliseconds(12000), [this] (TaskContext context)
                {
                    me->AI()->Talk(6);
                })
                .Schedule(Milliseconds(21000), [captainGarrick] (TaskContext context)
                {
                    captainGarrick->AI()->Talk(9);
                });
            }
        }
    }

    void QuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_THE_SCOUT_O_MATIC_5000) {
            Talk(2);
            player->GetScheduler().Schedule(Milliseconds(10000), [this] (TaskContext context)
            {
                me->AI()->Talk(3);
            })
            .Schedule(Milliseconds(15000), [player, this] (TaskContext context)
            {
                me->AI()->Talk(4);
                player->SummonCreature(156518, player->GetPosition(), TEMPSUMMON_MANUAL_DESPAWN);
            });
        }
    }

    void DoAction(int32 param) override
    {
        if (param == 1)
        {
            talked = true;
        }
    }
};

// npc_scout_o_matic_5000
struct npc_scout_o_matic_5000 : public ScriptedAI
{
    npc_scout_o_matic_5000(Creature* creature) : ScriptedAI(creature)
    {
        me->CanFly();
        me->SetFlying(true);
    }

private:
    Vehicle* vehicle;

    void IsSummonedBy(WorldObject* summoner) override
    {
        vehicle = me->GetVehicleKit();
        summoner->ToUnit()->EnterVehicle(me);
        Player* player = summoner->ToPlayer();
        me->GetMotionMaster()->MovePoint(1, 206.0f, -2289.0f, 117.0f, true);

        player->GetScheduler().Schedule(Milliseconds(25000), [this, player] (TaskContext context)
        {
            player->GetSceneMgr().PlaySceneByPackageId(SCENE_OGRE_BEFORE_BIG_BOAR_ATTACK, SceneFlag::None);
            player->KilledMonsterCredit(156518);
        }).Schedule(Milliseconds(50000), [this] (TaskContext context)
        {
            me->GetMotionMaster()->MovePoint(2, 108.609f, -2413.905f, 95.530f, true);
        }).Schedule(Milliseconds(75000), [this, player] (TaskContext context)
        {
            player->ExitVehicle();
            me->DespawnOrUnsummon(1s);
        });
    }
};

// item 170557 item_resizer_v901
class item_resizer_v901 : public ItemScript
{
public:
    item_resizer_v901() : ItemScript("item_resizer_v901") { }

    int8 castcount = 1; // removed randomize to get all the experience of this magical awesome weapon

    bool OnUse(Player* player, Item* item, SpellCastTargets const& targets, ObjectGuid /*castId*/) override
    {
        if (player->IsActiveQuest(QUEST_H_RESIZING_THE_SITUATION))
        {

            int8 random_casting_mechanic = rand() % 3 + 1; // randomize number 1-3 to select a mechanic

            if (random_casting_mechanic == 1) // mechanic1 - channeling with aura on mob then resizing success
            {
                if (Unit* target = player->GetSelectedUnit())
                    if (target->GetEntry() == 156716)
                    {
                        player->CastSpell(target, SPELL_RESIZER_CHANNELING); // channeling resizing til another action
                        player->GetScheduler().Schedule(Milliseconds(4000), [this, player](TaskContext context)
                            {
                                player->CastStop();
                                player->CastSpell(player->GetSelectedUnit(), SPELL_RESIZER_HIT_TOSMALL); // making the boar small + credit
                                player->GetSelectedUnit()->ToCreature()->ForcedDespawn(1000, 15s); // despawn resized mob
                            });
                        FIZZLEPOP_SAY = false;
                    }
            }
            if (random_casting_mechanic == 2) // channeling with aura
            {
                if (Unit* target = player->GetSelectedUnit())
                    if (target->GetEntry() == 156716)
                    {
                        player->CastSpell(target, SPELL_RESIZER_CHANNELING); // channeling resizing til another action
                        player->GetScheduler().Schedule(Milliseconds(4000), [this, player](TaskContext context)
                            {
                                player->CastStop();
                                player->CastSpell(player->GetSelectedUnit(), SPELL_RESIZER_HIT_EXPLODE); // explode + credit
                                player->GetSelectedUnit()->ToCreature()->ForcedDespawn(1000, 15s); // despawn old mob 
                            });
                    }
            }
            if (random_casting_mechanic == 3)
            {
                if (Unit* target = player->GetSelectedUnit())
                    if (target->GetEntry() == 156716)
                    {
                        player->CastSpell(player->GetSelectedUnit(), SPELL_RESIZER_OVERCHARGE); // overcharged hit kills with electro balls + credit
                    }
            }

        }
        return true;
    }
};

// item 178051 item_h_resizer_v901
class item_h_resizer_v901 : public ItemScript
{
public:
    item_h_resizer_v901() : ItemScript("item_h_resizer_v901") { }

    bool OnUse(Player* player, Item* item, SpellCastTargets const& targets, ObjectGuid /*castId*/) override
    {
        if (player->IsActiveQuest(QUEST_H_RESIZING_THE_SITUATION))
        {

            int8 random_casting_mechanic = rand() % 3 + 1; // randomize number 1-3 to select a mechanic

            if (random_casting_mechanic == 1) // mechanic1 - channeling with aura on mob then resizing success
            {
                if (Unit* target = player->GetSelectedUnit())
                    if (target->GetEntry() == 156716)
                    {
                        player->CastSpell(target, SPELL_RESIZER_CHANNELING); // channeling resizing til another action
                        player->GetScheduler().Schedule(Milliseconds(4000), [this, player](TaskContext context)
                            {
                                player->CastStop();
                                player->CastSpell(player->GetSelectedUnit(), SPELL_RESIZER_HIT_TOSMALL); // making the boar small + credit
                                player->GetSelectedUnit()->ToCreature()->ForcedDespawn(1000, 15s); // despawn resized mob
                            });
                        FIZZLEPOP_SAY = false;
                    }
            }
            if (random_casting_mechanic == 2) // channeling with aura
            {
                if (Unit* target = player->GetSelectedUnit())
                    if (target->GetEntry() == 156716)
                    {
                        player->CastSpell(target, SPELL_RESIZER_CHANNELING); // channeling resizing til another action
                        player->GetScheduler().Schedule(Milliseconds(4000), [this, player](TaskContext context)
                            {
                                player->CastStop();
                                player->CastSpell(player->GetSelectedUnit(), SPELL_RESIZER_HIT_EXPLODE); // explode + credit
                                player->GetSelectedUnit()->ToCreature()->ForcedDespawn(1000, 15s); // despawn old mob 
                            });
                    }
            }
            if (random_casting_mechanic == 3)
            {
                if (Unit* target = player->GetSelectedUnit())
                    if (target->GetEntry() == 156716)
                    {
                        player->CastSpell(player->GetSelectedUnit(), SPELL_RESIZER_OVERCHARGE); // overcharged hit kills with electro balls + credit
                    }
            }

        }
        return true;
    }

};


//156595 npc_giant_boar
struct npc_giant_boar : public ScriptedAI
{
    npc_giant_boar(Creature* creature) : ScriptedAI(creature)
    {
        Vehicle* vehicle = me->GetVehicleKit();
    }

    void Reset() override
    {
        me->AddNpcFlag(UNIT_NPC_FLAG_SPELLCLICK);
    }

    void OnSpellClick(Unit* clicker, bool result) override
    {
        if (Player* player = clicker->ToPlayer())
        {
            // this first "if" will prevent players from cheating by using the huge pig again :) -Varjgard
            if ((player->GetQuestStatus(QUEST_RIDE_OF_THE_SCIENTIFICALLY_ENHANCED_BOAR) == QUEST_STATUS_COMPLETE) || (player->GetQuestStatus(QUEST_RIDE_OF_THE_SCIENTIFICALLY_ENHANCED_BOAR) == QUEST_STATUS_REWARDED))
            {
                player->RemoveUnitFlag(UNIT_FLAG_NON_ATTACKABLE);
                player->ExitVehicle();
                me->DespawnOrUnsummon(100ms);
            }
            else if (player->GetQuestStatus(QUEST_RIDE_OF_THE_SCIENTIFICALLY_ENHANCED_BOAR) == QUEST_STATUS_INCOMPLETE)
            {
                me->AddUnitFlag(UNIT_FLAG_NON_ATTACKABLE);
                me->SetWalk(false);

                player->EnterVehicle(me);
                player->AddUnitFlag(UNIT_FLAG_NON_ATTACKABLE);
                player->KilledMonsterCredit(156595); // ride the boar credit
            }
        }
    }

    void IsSummonedBy(WorldObject* summoner) override
    {
        me->AddUnitFlag(UNIT_FLAG_NON_ATTACKABLE);
        me->SetWalk(false);

        if (Player* player = summoner->ToPlayer())
        {
            player->EnterVehicle(me);
            player->AddUnitFlag(UNIT_FLAG_NON_ATTACKABLE);
            player->KilledMonsterCredit(156595); // ride the boar credit

            //Pretty weird combination between vehicle and scene, anyway, this is used as damage
            //player->GetSceneMgr().PlaySceneByPackageId(SCENE_SKELETON_ARMY_DARKMAUL_PLAINS);
        }
    }
};

//263018
class spell_giant_boar_trample : public SpellScript
{
	PrepareSpellScript(spell_giant_boar_trample);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (Unit* boar = GetCaster())
            if (Vehicle* vehicle = boar->GetVehicleKit())
                if (Unit* caster = vehicle->GetPassenger(0))
                    if (Player* player = caster->ToPlayer())
                        if (player->GetQuestStatus(QUEST_RIDE_OF_THE_SCIENTIFICALLY_ENHANCED_BOAR) == QUEST_STATUS_INCOMPLETE)
                        {
                            MoveForward(boar, 8.0f);

                            std::list<Creature*> spellTargetsCreatures;
                            player->GetCreatureListWithEntryInGrid(spellTargetsCreatures, 157091, 8.0f);
                            player->GetCreatureListWithEntryInGrid(spellTargetsCreatures, 156532, 8.0f);
                            for (auto& target : spellTargetsCreatures)
                            {
                                if (target->GetEntry() == 157091)
                                    player->KilledMonsterCredit(165150);

                                target->KillSelf();
                            }
                        }
    }

    void MoveForward(Unit* boar, float distance)
    {
        Position movePos;
        float ori = boar->GetOrientation();
        float x = boar->GetPositionX() + distance * cos(ori);
        float y = boar->GetPositionY() + distance * sin(ori);
        float z = boar->GetPositionZ();
        boar->GetNearPoint2D(boar, x, y, distance, ori);
        movePos = { x, y, z, ori };
        boar->GetMotionMaster()->MovePoint(0, movePos, false);
    }

	void Register() override
	{
        OnEffectHit += SpellEffectFn(spell_giant_boar_trample::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
	}
};

//156799, familiar script will be also for horde version, which is 167128, scene 2905
struct npc_henry_garrick_156799 : public ScriptedAI
{
    npc_henry_garrick_156799(Creature* creature) : ScriptedAI(creature) { }

    void QuestReward(Player* player, Quest const* quest, LootItemType type, uint32 /*opt*/) override
    {
        if (quest->GetQuestId() == QUEST_RIDE_OF_THE_SCIENTIFICALLY_ENHANCED_BOAR)
        {
            player->RemoveUnitFlag(UNIT_FLAG_NON_ATTACKABLE);
            //this scene is triggered from somewhere :D but where??? so dont use this part of c++ -Varjgard
            //player->GetSceneMgr().PlaySceneByPackageId(SCENE_HENRY_REUNION);
        }
    }
};

//156943
struct npc_meredy_huntswell_156943 : public ScriptedAI
{
    npc_meredy_huntswell_156943(Creature* creature) : ScriptedAI(creature) { }


    void QuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_RIGHT_BENEATH_THEIR_EYES)
        {
            me->CastSpell(player, 313583, false);
            me->AddAura(313583, player);
            AddTimedDelayedOperation(6100, [this, player]() -> void
            {
                //player->SetDisplayId(x, 1.0f);
            });
            //Temp, i set it also to ignore all objectives, since you can't complete even through command, it's using criteria, not specific objects
            if (player->IsActiveQuest(QUEST_RIGHT_BENEATH_THEIR_EYES))
                player->ForceCompleteQuest(QUEST_RIGHT_BENEATH_THEIR_EYES);

            player->NearTeleportTo(700.874f, -1882.962f, 186.508f, 1.208f, false);
        }
    }
};

// npc 156651 - captain garrick sparring version
struct npc_captain_garrick_156651 : public ScriptedAI
{
    npc_captain_garrick_156651(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        ScriptedAI::Reset();
    }

    void QuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_COOKING_MEAT)
        {
            if (Creature* alaria = me->FindNearestCreature(156607, 50.0f))
                alaria->AI()->Talk(0);

            player->GetScheduler().Schedule(Milliseconds(4000), [this] (TaskContext context)
            {
                me->AI()->Talk(0);
            });
        }

        if (quest->GetQuestId() == QUEST_ENHANCED_COMBAT_TACTICS)
        {
            if(Creature* npc = me->SummonCreature(me->GetEntry(), me->GetPosition(), TEMPSUMMON_MANUAL_DESPAWN, 0s, 0U))
            {
                npc->SetFaction(14);
                npc->SetReactState(REACT_AGGRESSIVE);
                npc->AI()->Talk(2);
                player->GetScheduler().Schedule(Milliseconds(6000), [this] (TaskContext context)
                {
                    me->AI()->Talk(3);
                });
            }

            me->DestroyForPlayer(player);
        }

        if (quest->GetQuestId() == QUEST_NORTHBOUND)
            Talk(4);
    }

    void QuestReward(Player* player, Quest const* quest, LootItemType /*type*/, uint32 /*opt*/) override
    {
        if (quest->GetQuestId() == QUEST_COOKING_MEAT)
        {
            if (Creature* alaria = me->FindNearestCreature(156607, 50.0f))
                alaria->AI()->Talk(1);

            player->GetScheduler().Schedule(Milliseconds(3000), [this] (TaskContext context)
            {
                me->AI()->Talk(1);
            });
        }
    }

    void DamageTaken(Unit* attacker, uint32& /*damage*/) override
    {
        if (Player* player = attacker->ToPlayer())
            if (me->GetHealth() < me->CountPctFromMaxHealth(50)) // the intent is not to kill but to sparr
                if (player->IsActiveQuest(QUEST_ENHANCED_COMBAT_TACTICS))
                {
                    player->ForceCompleteQuest(QUEST_ENHANCED_COMBAT_TACTICS);
                    me->SummonCreature(me->GetEntry(), me->GetPosition(), TEMPSUMMON_MANUAL_DESPAWN, 0s, 0U);
                    me->DespawnOrUnsummon(10ms);
                }
    }

    void JustDied(Unit* killer) override
    {
        if (Player* player = killer->ToPlayer())
            if (player->IsActiveQuest(QUEST_ENHANCED_COMBAT_TACTICS))
            {
                player->ForceCompleteQuest(QUEST_ENHANCED_COMBAT_TACTICS);
                me->SummonCreature(me->GetEntry(), me->GetPosition(), TEMPSUMMON_MANUAL_DESPAWN, 0s, 0U);
                me->DespawnOrUnsummon(10ms);
            }
    }
};

//154327 Austin Huxworth
struct npc_austin_huxworth : public ScriptedAI
{
    npc_austin_huxworth(Creature* c) : ScriptedAI(c) { }

    void QuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_FORBIDDEN_QUILBOAR_NECROMANCY) {
            if (Creature* captainGarrick = me->FindNearestCreature(156662, 50.0f)) {
                captainGarrick->AI()->Talk(6);

                player->GetScheduler().Schedule(Milliseconds(4000), [this] (TaskContext context)
                {
                    me->AI()->Talk(0);
                })
                .Schedule(Milliseconds(9000), [captainGarrick] (TaskContext context)
                {
                    captainGarrick->AI()->Talk(7);
                });
            }
        }
    }
};

// npc_captain_garrick_156280
struct npc_captain_garrick_156280 : public ScriptedAI
{
    npc_captain_garrick_156280(Creature* creature) : ScriptedAI(creature) { }

    void QuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_RIDE_OF_THE_SCIENTIFICALLY_ENHANCED_BOAR)
        {
            player->SummonCreature(156595, player->GetPosition(), TEMPSUMMON_MANUAL_DESPAWN, 1min, 0U, 0U, player->GetGUID()); //10 minutes to despawn
        }

        if (quest->GetQuestId() == QUEST_WARMING_UP)
        {
            Talk(GARRIC_TXT1);
            me->GetMap()->SetZoneWeather(10424, WEATHER_STATE_EXILE_REACH, 0.5f);
        }
    }


    void QuestReward(Player* player, Quest const* quest, LootItemType /*type*/, uint32 /*opt*/) override
    {
        if (quest->GetQuestId() == QUEST_BRACE_FOR_IMPACT)
        {
            me->GetMap()->SetZoneWeather(10424, WEATHER_STATE_EXILE_REACH, 0.5f);
            player->GetSceneMgr().PlaySceneByPackageId(SCENE_HIDE_TRANSITION_TO_BEACH, SceneFlag::None);
            player->TeleportTo(2175, -462.722f, -2620.544f, 0.472f, 0.760f);
            player->CastSpell(player, SPELL_A_SHIP_CRASH);
        }

        if (quest->GetQuestId() == QUEST_WARMING_UP)
        {
            me->AI()->Talk(GARRIC_TXT3, player);
        }
    }
};

// npc 156626
struct npc_captain_garrick_156626 : public ScriptedAI
{
    bool talked;
    npc_captain_garrick_156626(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        ScriptedAI::Reset();
        talked = false;
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
        {
            if (player->GetQuestStatus(QUEST_BRACE_FOR_IMPACT) == QUEST_STATUS_REWARDED &&
                player->GetQuestStatus(QUEST_MURLOC_MANIA) == QUEST_STATUS_NONE )
            {
                if (player->IsInDist(me, 20.0f) && !talked)
                {
                    Talk(0);
                    talked = true;
                }
            }
        }
    }

    void QuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_MURLOC_MANIA)
        {
            Talk(1);
            me->AI()->Reset();
        }

        if (quest->GetQuestId() == QUEST_FINDING_THE_LOST_EXPEDITION)
            Talk(2);
    }
};

// npc 156626 npc_alaria_175031
struct npc_alaria_175031 : public ScriptedAI
{
    npc_alaria_175031(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        ScriptedAI::Reset();
    }

    void QuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_NORTHBOUND)
        {
            if (Creature* captainGarrick = me->FindNearestCreature(156651, 50.0f)) {
                captainGarrick->AI()->Talk(4);

                player->GetScheduler().Schedule(Milliseconds(2000), [captainGarrick] (TaskContext context)
                {
                    captainGarrick->GetMotionMaster()->MovePoint(1, -181.259f, -2575.281f, 31.0f, true);
                    captainGarrick->ForcedDespawn(10000);
                });
            }
        }
    }
};

// npc_geolord_grekog_151091
struct npc_geolord_grekog_151091 : public ScriptedAI
{
    npc_geolord_grekog_151091(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        ScriptedAI::Reset();
    }

    void JustEngagedWith(Unit* attacker) override                  // attack_started
    {
        if (Player* player = attacker->ToPlayer())
        {
            Talk(0);

            if (Creature* npc = player->FindNearestCreature(167008, 20.0f, true))
            {
                player->GetScheduler().Schedule(Milliseconds(5000), [npc] (TaskContext context)
                {
                    npc->AI()->Talk(0);
                });
            }


            me->AddDelayedEvent(3000, [this, player]()
                {
                    // cast upheaval
                    me->CastSpell(player, SPELL_UPHEAVAL);
                });
        }
    }

    void JustDied(Unit* attacker) override
    {
        if (Player* player = attacker->ToPlayer())
        {
            if (player->IsActiveQuest(QUEST_H_DOWN_WITH_THE_QUILBOAR))
            {
                    if (Creature* npc = player->FindNearestCreature(167008, 50.0f, true))
                    {
                        npc->SetCanFly(false);
                        npc->RemoveAura(305513);
                        npc->GetMotionMaster()->MovePoint(1, 16.6666f, -2511.82f, 73.39143f, true);
                        npc->AI()->Talk(1);

                        // walking to breka after freed
                        npc->AddDelayedEvent(3000, [npc]() { npc->GetMotionMaster()->MovePoint(1, 30.750f, -2513.189f, 73.408f, true); });
                        npc->AddDelayedEvent(5500, [npc]() { npc->GetMotionMaster()->MovePoint(1, 52.428f, -2481.062f, 78.931f, true); });
                        npc->AddDelayedEvent(11000, [npc]() { npc->GetMotionMaster()->MovePoint(1, 74.708f, -2458.385f, 88.283f, true); });
                        npc->AddDelayedEvent(16500, [npc]() { npc->GetMotionMaster()->MovePoint(1, 100.124f, -2429.822f, 90.373f, true); });
                        npc->AddDelayedEvent(22000, [npc, player]()
                            {
                                npc->GetMotionMaster()->MovePoint(1, 100.666f, -2417.773f, 90.392f, true);
                                npc->PlayDirectSound(156984, player, 196270); npc->Say(196270); // You must be Warlord Breka! Shuja's mother! She always said you'd save our hides.
                                npc->AddAura(SPELL_SEE_QUEST_INVISIBILITY, npc);
                            });
                        npc->AddDelayedEvent(25000, [npc]()
                            {
                                npc->SetFacingToObject(npc->FindNearestCreature(167021, 10.0f));
                                npc->SetRespawnDelay(30); // 30 sec for respawning
                                npc->DespawnOrUnsummon(1s); // despawn immediately
                            });
                    }
            }


            if (player->IsActiveQuest(QUEST_DOWN_WITH_THE_QUILBOAR))
            {
                if (Creature* npc = me->SummonCreature(149899, Position(16.4271f, -2511.82f, 78.8215f, 5.66398f), TEMPSUMMON_MANUAL_DESPAWN, 0s, 0U))
                {
                    npc->AI()->Talk(7);
                    npc->AI()->DoAction(1);

                    player->GetScheduler().Schedule(Milliseconds(1000), [npc, player] (TaskContext context)
                    {
                        player->ForceCompleteQuest(QUEST_DOWN_WITH_THE_QUILBOAR);
                        npc->GetMotionMaster()->MovePoint(1, 74.0192f, -2461.207764f, 88.0f, true);
                        npc->ForcedDespawn(4000);
                    });
                }
            }
        }

    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        uint32 maxhp = me->GetMaxHealth();
        uint32 hp = me->GetHealth();
        Player* player = attacker->ToPlayer();


        if ((hp < maxhp * 0.75) && (hp > maxhp * 0.6)) // if hp between 60-75%
        {
            // cast earthbolt
            me->CastSpell(player, SPELL_EARTHBOLT);
        }

        if ((hp < maxhp * 0.4) && (hp > maxhp * 0.3)) // if hp between 30-40%
        {
            // cast upheaval
            me->CastSpell(player, SPELL_UPHEAVAL);
        }
    }

};

//156929 npc_ralia_dreamchacer_156929
struct npc_ralia_dreamchacer_156929 : public ScriptedAI
{
    npc_ralia_dreamchacer_156929(Creature* creature) : ScriptedAI(creature)
    {
        Vehicle* vehicle = me->GetVehicleKit();
    }

    void Reset() override
    {
        me->AddNpcFlag(UNIT_NPC_FLAG_SPELLCLICK);
    }

    void OnSpellClick(Unit* clicker, bool result) override
    {
        if (Player* player = clicker->ToPlayer())
        {
            // this first "if" will prevent players from cheating by using ralia again :) -Varjgard
            if (player->GetQuestStatus(QUEST_WHO_LURKS_IN_THE_PIT) == QUEST_STATUS_REWARDED)
            {
                // player->ExitVehicle();
                me->DespawnOrUnsummon(100ms);
            }
            else if (player->IsActiveQuest(QUEST_WHO_LURKS_IN_THE_PIT))
            {
                me->AddUnitFlag(UNIT_FLAG_NON_ATTACKABLE);
                me->SetWalk(false);

                // player->EnterVehicle(me);
                player->ForceCompleteQuest(QUEST_WHO_LURKS_IN_THE_PIT);

                player->GetSceneMgr().PlaySceneByPackageId(SCENE_DEER_GO_UP, SceneFlag::None);
                player->TeleportTo(2175, 102.504f, -2262.636f, 96.465f, 5.411f);
            }
        }
    }

    void IsSummonedBy(WorldObject* summoner) override
    {
        me->AddUnitFlag(UNIT_FLAG_NON_ATTACKABLE);
        me->SetWalk(false);

        if (Player* player = summoner->ToPlayer())
        {
            // player->EnterVehicle(me);
            player->ForceCompleteQuest(QUEST_WHO_LURKS_IN_THE_PIT);

            player->GetSceneMgr().PlaySceneByPackageId(SCENE_DEER_GO_UP, SceneFlag::None);
            player->TeleportTo(2175, 102.504f, -2262.636f, 96.465f, 5.411f);
        }
    }
};

//npc_alliance_gryphon_154155
struct npc_alliance_gryphon_154155 : public ScriptedAI
{
    npc_alliance_gryphon_154155(Creature* creature) : ScriptedAI(creature)
    {
        Vehicle* vehicle = me->GetVehicleKit();
    }

    void Reset() override
    {
        me->AddNpcFlag(UNIT_NPC_FLAG_SPELLCLICK);
    }

    void OnSpellClick(Unit* clicker, bool result) override
    {
        if (Player* player = clicker->ToPlayer())
        {
            if (player->IsActiveQuest(QUEST_AN_END_TO_BEGINNINGS))
            {
                player->EnterVehicle(me);
                player->ForceCompleteQuest(QUEST_AN_END_TO_BEGINNINGS);

                player->GetSceneMgr().PlaySceneByPackageId(SCENE_FLY_AWAY, SceneFlag::None);
                player->GetScheduler().Schedule(Milliseconds(66000), [this, player] (TaskContext context)
                {
                    player->GetSceneMgr().CancelSceneByPackageId(SCENE_FLY_AWAY); // there will be black screen without this

                    switch (player->GetTeam())
                    {
                        case ALLIANCE:
                            player->TeleportTo(0, -9060.474f, 438.164f, 93.055f, 0.657f); // stormwind
                            break;

                        case HORDE:
                            player->TeleportTo(1, 1458.339f, -4420.480f, 25.453f, 0.161f); // orgrimmar
                            break;
                    }
                });
            }
        }
    }
};

//go_ogre_runestone_339865
struct go_ogre_runestone_339865 : public GameObjectAI
{
    go_ogre_runestone_339865(GameObject* go) : GameObjectAI(go) { }

    bool GossipHello(Player* player) override
    {
        if (player->IsActiveQuest(QUEST_CONTROLLING_THEIR_STONES))
        {
            if (player->GetQuestObjectiveProgress(QUEST_CONTROLLING_THEIR_STONES, 4) == 2)
            {
                player->ForceCompleteQuest(QUEST_CONTROLLING_THEIR_STONES);

                // TODO: kelra waypoints etc here
            }
            else
            {
                player->KillCreditGO(339865);
            }
        }

        return true;
    }
};

//npc_coulston_nereus
struct npc_coulston_nereus : public ScriptedAI
{
    npc_coulston_nereus(Creature* creature) : ScriptedAI(creature) { }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
        {
            if (player->GetQuestStatus(58917) == QUEST_STATUS_INCOMPLETE)
            {
                if (player->IsInDist(me, 30.0f))
                {
                    player->KilledMonsterCredit(162972, ObjectGuid::Empty);
                }
            }
        }
    }
};

// npc_gutgruck_the_tough
struct npc_gutgruck_the_tough : public ScriptedAI
{
    npc_gutgruck_the_tough(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        me->Yell("Me gonna get more sacrifices for ritual", LANG_UNIVERSAL);
        me->GetMotionMaster()->MoveRandom(20.0f);
    }

    void JustDied(Unit* killer) override
    {
        if (Player* player = killer->ToPlayer())
        {
            player->KilledMonsterCredit(175015);
        }

        Talk(0);
        me->DespawnOrUnsummon(1000ms);
    }

    void JustEngagedWith(Unit* who) override
    {
        Talk(1);
    }
};

// at_for_gutgruck_the_tough
struct at_for_gutgruck_the_tough : public AreaTriggerAI
{
    at_for_gutgruck_the_tough(AreaTrigger* at) : AreaTriggerAI(at) { }

    void OnUnitEnter(Unit* unit) override
    {
        Player* player = unit->ToPlayer();

        if (!player)
            return;

        if (player->GetQuestStatus(58933) == QUEST_STATUS_INCOMPLETE)
            at->SummonCreature(163031, Position(282.236420f, -2148.987793f, 103.455276f, 3.317860f), TEMPSUMMON_MANUAL_DESPAWN);
    }
};

// hrun_the_exiled_156900
struct hrun_the_exiled_156900 : public ScriptedAI
{
    hrun_the_exiled_156900(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        ScriptedAI::Reset();
    }

    bool casted1, casted2 = false;

    void JustEngagedWith(Unit* who) override
    {
        me->AddDelayedEvent(3000, [this, who]()
            {
                me->CastSpell(who, SPELL_SPIRIT_BOLT);
            });
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        uint32 maxhp = me->GetMaxHealth();
        uint32 hp = me->GetHealth();
        Player* player = attacker->ToPlayer();

        if ((hp < maxhp * 0.5) && !casted1)
        {
            me->CastSpell(player, SPELL_SPIRIT_BOLT);
            casted1 = true;
        }

        if (((hp < maxhp * 0.2) && !casted2) || damage > hp * 2)
        {
            me->CastSpell(player, SPELL_DRAIN_SPIRIT);
            casted2 = true;
        }

        if (damage > hp)
            me->CastSpell(me, SPELL_FINAL_SACRIFICE_FIRE);
    }

    void JustDied(Unit* attacker) override
    {
        if (Player* player = attacker->ToPlayer())
        {
            if (player->IsInAlliance())
            {
                if (player->IsActiveQuest(QUEST_WHO_LURKS_IN_THE_PIT))
                {
                    // ralia
                    if (Creature* npc = me->FindNearestCreature(156902, 100.0f, false))
                    {
                        npc->DestroyForPlayer(player);
                    }

                    // ralia mount
                    if (Creature* npc2 = player->SummonCreature(156929, player->GetPosition(), TEMPSUMMON_MANUAL_DESPAWN, 1min, 0U)) //10 minutes to despawn
                    {
                        npc2->Say(183971);
                        npc2->PlayDirectSound(152938, player, 183971);
                    }
                }
            }
            if (player->IsInHorde())
            {
                if (player->IsActiveQuest(QUEST_H_WHO_LURKS_IN_THE_PIT))
                {
                    Creature* crenna_noform = player->FindNearestCreature(167250, 50.0f, true);

                    if (Creature* crenna_travelform = player->SummonCreature(167254, crenna_noform->GetPosition(), TEMPSUMMON_MANUAL_DESPAWN, 1min, 0U))
                    {
                        PhasingHandler::RemovePhase(player, 13050, true);
                        crenna_travelform->Say(196467);
                        crenna_travelform->PlayDirectSound(157167, player, 196467);
                    }
                }
            }
        }
    }
};

// npc_ogre_overseer_156676
struct npc_ogre_overseer_156676 : public ScriptedAI
{
    npc_ogre_overseer_156676(Creature* creature) : ScriptedAI(creature) { }

    void JustEngagedWith(Unit* who) override
    {
        me->Say(178950);
        me->PlayDirectSound(150269, who->ToPlayer(), 178950);
        me->AddDelayedEvent(3000, [this, who]()
            {
                me->CastSpell(who->ToPlayer(), SPELL_EARTHSHATTER);
            });
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        uint32 maxhp = me->GetMaxHealth();
        uint32 hp = me->GetHealth();
        Player* player = attacker->ToPlayer();

        if ((hp < maxhp * 0.7) && (hp > maxhp * 0.4))
        {
            me->CastSpell(player, SPELL_BACKHAND);
        }
        if ((hp < maxhp * 0.3) && (hp > maxhp * 0.1))
        {
            me->CastSpell(attacker, SPELL_EARTHSHATTER);
        }

        if (damage > hp) // on last hit before die
            Talk(0);
    }

};

// npc_quilboars_warrior_geomancer_150237_150238
struct npc_quilboars_warrior_geomancer_150237_150238 : public ScriptedAI
{
    npc_quilboars_warrior_geomancer_150237_150238(Creature* creature) : ScriptedAI(creature) { }

    int8 random_of_5, chancetospeak;
    bool mid_talked;

    void JustEngagedWith(Unit* who) override
    {
        Player* player = who->ToPlayer();

        random_of_5 = rand() % 5 + 1; // randomize texts
        chancetospeak = rand() % 3 + 1; // chance to speak 33%

        if (me->HasAura(SPELL_QUILBOAR_SLEEP)) me->RemoveAura(SPELL_QUILBOAR_SLEEP);

        if (chancetospeak == 1)
        {
            if (random_of_5 == 1) { me->PlayDirectSound(150281, player, 176079); me->Say(176079); } // Death to the outsiders! 
            if (random_of_5 == 2) { me->PlayDirectSound(150283, player, 176082); me->Say(176082); } // More sacrifices for the ritual! 
            if (random_of_5 == 3) { me->PlayDirectSound(150282, player, 176082); me->Say(176080); } // Our thorns will strangle you!  
            if (random_of_5 == 4) { me->PlayDirectSound(150278, player, 176076); me->Say(176076); } // The geolord demands your demise!  
            if (random_of_5 == 5) { me->PlayDirectSound(150276, player, 176074); me->Say(176074); } // The ogres give us strength!
        }

        me->AddDelayedEvent(3000, [this, player]()
            {
                if (me->GetEntry() == 150237)
                    me->CastSpell(player, SPELL_BRUTAL_STRIKE);
                else
                    me->CastSpell(player, SPELL_EARTH_BOLT_2);
            });
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        uint32 maxhp = me->GetMaxHealth();
        uint32 hp = me->GetHealth();
        Player* player = attacker->ToPlayer();

        if ((hp < maxhp * 0.3) && (hp > maxhp * 0.2))
        {
            if (me->GetEntry() == 150237)
                me->CastSpell(player, SPELL_BRUTAL_STRIKE);
            else
                me->CastSpell(player, SPELL_EARTH_BOLT_2);

            if (!mid_talked)
            {
                random_of_5;
                mid_talked = true;

                if (random_of_5 == 1) { me->PlayDirectSound(150279, player, 176077); me->Say(176077); } // We serve the geolord! 
                if (random_of_5 == 3) { me->PlayDirectSound(150277, player, 176075); me->Say(176075); } // Your blood will earn our reward!
                if (random_of_5 == 5) { me->PlayDirectSound(150280, player, 176078); me->Say(176078); } // Witness our power! 
            }
        }

        if (damage > hp) // on last hit before die
        {
            random_of_5;

            if (random_of_5 == 1) { me->PlayDirectSound(150284, player, 176083); me->Say(176083); } // I return... to... the earth... 
            if (random_of_5 == 2) { me->PlayDirectSound(150286, player, 176085); me->Say(176085); } // Ogres... will... kill you... 
            if (random_of_5 == 3) { me->PlayDirectSound(150285, player, 176084); me->Say(176084); } // Our tribe... will... survive... 
            if (random_of_5 == 4) { me->PlayDirectSound(150287, player, 176086); me->Say(176086); } // The geolord... will not... lose...   
            if (random_of_5 == 5) { me->PlayDirectSound(150288, player, 176088); me->Say(176088); } // Thorns... take... you... 
        }
    }

};

// npc_gorgroth_153580
struct npc_gorgroth_153580 : public ScriptedAI
{
    npc_gorgroth_153580(Creature* creature) : ScriptedAI(creature) { }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
        {
            if (player->IsInDist(me, 40.0f))
            {
                if (player->IsActiveQuest(QUEST_H_RIGHT_BENEATH_THEIR_EYES))
                {
                    if (Quest const* quest = sObjectMgr->GetQuestTemplate(QUEST_H_RIGHT_BENEATH_THEIR_EYES))
                        for (QuestObjective const& obj : quest->GetObjectives())
                            if (obj.ObjectID == 17000) // find cooking area
                            {
                                uint16 slot = player->FindQuestSlot(QUEST_H_RIGHT_BENEATH_THEIR_EYES);
                                if (!player->IsQuestObjectiveComplete(slot, quest, obj)) // just make it complete once
                                {
                                    player->SetQuestObjectiveData(obj, 1);
                                    player->SendQuestUpdateAddCredit(quest, ObjectGuid::Empty, obj, 1);
                                    me->Say(174173);
                                    me->PlayDirectSound(153058, player, 174173);
                                }
                            }
                }
            }
        }
    }

    void ReceiveEmote(Player* player, uint32 emoteId) override
    {
        if (emoteId == 101) // wave (bro i had to make a script to get this id)
            if (player->IsActiveQuest(QUEST_H_RIGHT_BENEATH_THEIR_EYES))
            {
                if (Quest const* quest = sObjectMgr->GetQuestTemplate(QUEST_H_RIGHT_BENEATH_THEIR_EYES))
                    for (QuestObjective const& obj : quest->GetObjectives())
                        if (obj.ObjectID == 85146) // wave to gorgroth
                        {
                            uint16 slot = player->FindQuestSlot(QUEST_H_RIGHT_BENEATH_THEIR_EYES);
                            if (!player->IsQuestObjectiveComplete(slot, quest, obj)) // just make it complete once
                            {
                                player->SetQuestObjectiveData(obj, 1);
                                player->SendQuestUpdateAddCredit(quest, ObjectGuid::Empty, obj, 1);

                                me->AddDelayedEvent(2000, [this, player]()        // time_delay
                                    {
                                        me->SetEmoteState(EMOTE_ONESHOT_POINT);
                                        me->Say(175499);
                                        me->PlayDirectSound(153059, player, 175499);
                                    });
                                me->AddDelayedEvent(7000, [this, player]()        // time_delay
                                    {
                                        me->Say(175501);
                                        me->PlayDirectSound(153060, player, 175501);
                                        // despawn old version after 60 sec and 5 sec for respawning
                                        me->DespawnOrUnsummon(5s, 60s);
                                    });
                            }
                        }
            }
    }

};

// npc_torgok_162817
struct npc_torgok_162817 : public ScriptedAI
{
    npc_torgok_162817(Creature* creature) : ScriptedAI(creature) { }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        uint32 maxhp = me->GetMaxHealth();
        uint32 hp = me->GetHealth();
        Player* player = attacker->ToPlayer();

        if ((hp < maxhp) && (hp > maxhp * 0.95))
        {
            if (!attacker->HasAura(SPELL_SOUL_GRASP))
                if (attacker->IsWithinDist(me, 10.0f, true))
                    me->CastSpell(attacker, SPELL_SOUL_GRASP);
        }
        if ((hp < maxhp * 0.7) && (hp > maxhp * 0.6))
        {
            me->CastSpell(player, SPELL_SPIRIT_BOLT);
        }
        if ((hp < maxhp * 0.4) && (hp > maxhp * 0.30))
        {
            if (!attacker->HasAura(SPELL_SOUL_GRASP))
                if (attacker->IsWithinDist(me, 10.0f, true))
                    me->CastSpell(attacker, SPELL_SOUL_GRASP);
        }
    }

    void JustDied(Unit* killer)
    {
        Player* player = killer->ToPlayer();
        Creature* grimaxe = me->FindNearestCreature(167145, 20.0f, true);

        if (player->IsActiveQuest(QUEST_H_THE_REDEATHER))
        {
            // safeguard if the player don't meet the next quest level requirement
            if (player->GetLevel() < 6)
                player->GiveXP(player->GetXPForNextLevel(), player, 1);

            grimaxe->AttackStop();
            grimaxe->GetMotionMaster()->MovePoint(1, 231.739f, -2294.473f, 80.894f, true);
            
        }

        if (Player* player = killer->ToPlayer())
            if (player->IsActiveQuest(QUEST_RIDE_OF_THE_SCIENTIFICALLY_ENHANCED_BOAR))
                player->ForceCompleteQuest(QUEST_RIDE_OF_THE_SCIENTIFICALLY_ENHANCED_BOAR);
    }

};

//go_harpy_totem_350803
struct go_harpy_totem_350803 : public GameObjectAI
{
    go_harpy_totem_350803(GameObject* go) : GameObjectAI(go) { }

    bool GossipHello(Player* player) override
    {
        if (player->IsActiveQuest(QUEST_H_PURGE_THE_TOTEMS))
        {
            me->SummonCreature(167993, me->GetPosition(), TEMPSUMMON_TIMED_DESPAWN, 5s);
            if (Creature* harpy_totem_npc = me->FindNearestCreature(167993, 5.0f, true))
                me->CastSpell(harpy_totem_npc, SPELL_BURNING_TOP);
            player->KillCreditGO(350803);
            me->DestroyForPlayer(player);
        }
        return true;
    }
};

//go_thick_cocoon_339568_350796
struct go_thick_cocoon_339568_350796 : public GameObjectAI
{
    go_thick_cocoon_339568_350796(GameObject* go) : GameObjectAI(go) { }

    bool GossipHello(Player* player) override
    {
        // horde
        if (player->IsActiveQuest(QUEST_H_WHO_LURKS_IN_THE_PIT))
        {
            player->KillCreditGO(350796);
            me->CastSpell(player, 325524);
            me->DestroyForPlayer(player);
        }
        // alli
        if (player->IsActiveQuest(QUEST_WHO_LURKS_IN_THE_PIT))
        {
            player->KillCreditGO(350796);
            me->CastSpell(player, 325524);
            me->DestroyForPlayer(player);
        }

        return true;
    }
};

//npc_Freed_Expedition_Member_167244_160964
struct npc_Freed_Expedition_Member_167244_160964 : public ScriptedAI
{
    npc_Freed_Expedition_Member_167244_160964(Creature* creature) : ScriptedAI(creature) { }

    void IsSummonedBy(WorldObject* unit) override
    {
        if (Player* player = unit->ToPlayer())
        {
            me->SelectNearestPlayers(10.0f, true);
            me->SetFacingToObject(player);
            me->AI()->Talk(0);
            me->AddDelayedEvent(2000, [this]() -> void
                {
                    me->HandleEmoteCommand(EMOTE_ONESHOT_SALUTE);
                    me->GetMotionMaster()->MoveRandom(10.0f);
                    me->ForcedDespawn(2000);
                });
        }
    }
};

// npc_harpie_boltcaller_152843
struct npc_harpie_boltcaller_152843 : public ScriptedAI
{
    npc_harpie_boltcaller_152843(Creature* creature) : ScriptedAI(creature) { }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        uint32 maxhp = me->GetMaxHealth();
        uint32 hp = me->GetHealth();
        Player* player = attacker->ToPlayer();

        if ((hp < maxhp * 0.8) && (hp > maxhp * 0.7))
        {
            me->CastSpell(player, SPELL_HARPIE_LIGHTNINGBOLT);
        }
        if ((hp < maxhp * 0.4) && (hp > maxhp * 0.3))
        {
            me->CastSpell(player, SPELL_HARPIE_LIGHTNINGBOLT);
        }
    }

};

// npc_harpie_snatcher_152571
struct npc_harpie_snatcher_152571 : public ScriptedAI
{
    npc_harpie_snatcher_152571(Creature* creature) : ScriptedAI(creature) { }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        uint32 maxhp = me->GetMaxHealth();
        uint32 hp = me->GetHealth();
        Player* player = attacker->ToPlayer();

        if ((hp < maxhp * 0.8) && (hp > maxhp * 0.7))
        {
            me->CastSpell(player, SPELL_HARPIE_TALONRAKE);
        }
        if ((hp < maxhp * 0.4) && (hp > maxhp * 0.3))
        {
            me->CastSpell(player, SPELL_HARPIE_TALONRAKE);
        }
    }

};

// npc_hunting_worg_152998
struct npc_hunting_worg_152998 : public ScriptedAI
{
    npc_hunting_worg_152998(Creature* creature) : ScriptedAI(creature) { }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        uint32 maxhp = me->GetMaxHealth();
        uint32 hp = me->GetHealth();
        Player* player = attacker->ToPlayer();

        if ((hp < maxhp * 0.6) && (hp > maxhp * 0.3))
        {
            me->CastSpell(player, SPELL_WORG_CLAW);
        }
    }

};

//npc_breka_grimaxe_167145
struct npc_breka_grimaxe_167145 : public ScriptedAI
{
    npc_breka_grimaxe_167145(Creature* c) : ScriptedAI(c) { }

    void MoveInLineOfSight(Unit* who) override
    {
        if (who->IsInDist(me, 10.0f) && who->IsPlayer() == true && me->GetPositionX() == 235.297f) //player come close breka follows
        {
            me->GetMotionMaster()->MoveFollow(who, PET_FOLLOW_DIST, me->GetFollowAngle());
        }

        if (who->IsInDist(me, 10.0f) && who->GetEntry() == 162817) //torgok come close
        {
            me->SetTarget(who->GetGUID());
            me->SetFaction(FACTION_ESCORTEE_H_NEUTRAL_ACTIVE);
            me->SetBaseWeaponDamage(BASE_ATTACK, WeaponDamageRange(MAXDAMAGE), 0.5f);
            me->setAttackTimer(BASE_ATTACK, 2000);
            me->AI()->AttackStart(who);
        }
    }

};

// npc_barrow_spider_160394
struct npc_barrow_spider_160394 : public ScriptedAI
{
    npc_barrow_spider_160394(Creature* creature) : ScriptedAI(creature) { }

    bool casted = false;

    void JustEngagedWith(Unit* who) override
    {
        me->AddDelayedEvent(3000, [this, who]()
            {
                me->CastSpell(who, SPELL_POISONED_BITE);
            });
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        uint32 maxhp = me->GetMaxHealth();
        uint32 hp = me->GetHealth();
        Player* player = attacker->ToPlayer();

        if ((hp < maxhp * 0.5) && !casted)
        {
            me->CastSpell(player, SPELL_POISONED_BITE);
            casted = true;
        }
    }

};

//npc_breka_grimaxe_167596
struct npc_breka_grimaxe_167596 : public EscortAI
{
    npc_breka_grimaxe_167596(Creature* creature) : EscortAI(creature) { }

    ObjectGuid   m_shujaGUID;
    ObjectGuid   m_herbertGUID;
    ObjectGuid   m_crennaGUID;
    ObjectGuid   m_playerGUID;

    void Initialize()
    {
        m_shujaGUID = ObjectGuid::Empty;
        m_herbertGUID = ObjectGuid::Empty;
        m_crennaGUID = ObjectGuid::Empty;
        m_playerGUID = ObjectGuid::Empty;
    }

    void GetFollowerMember()
    {
        if (!m_shujaGUID)
            if (Creature* shuja = me->FindNearestCreature(167290, 20.0f, true))
                m_shujaGUID = shuja->GetGUID();

        if (!m_herbertGUID)
            if (Creature* herbert = me->FindNearestCreature(167182, 20.0f, true))
                m_herbertGUID = herbert->GetGUID();

        if (!m_crennaGUID)
            if (Creature* crenna = me->FindNearestCreature(166575, 20.0f, true))
                m_crennaGUID = crenna->GetGUID();

    }

    void QuestAccept(Player* player, Quest const* quest) override
    {

        if (quest->GetQuestId() == QUEST_H_TO_DARKMAUL_CITADEL)
        {
            Talk(0); // Recruit, we push forward! The rest of you, hold this position.

            me->RemoveNpcFlag(UNIT_NPC_FLAG_QUESTGIVER);

            Start(true, false);
            GetFollowerMember();

            if (player->ToPlayer())
                m_playerGUID = player->GetGUID();                   //write_player

            Creature* shuja = ObjectAccessor::GetCreature(*me, m_shujaGUID);
            shuja->GetMotionMaster()->MoveFollow(me, -2.0f, shuja->GetFollowAngle(), MOTION_SLOT_ACTIVE);

            Creature* herbert = ObjectAccessor::GetCreature(*me, m_herbertGUID);
            herbert->GetMotionMaster()->MoveFollow(shuja, -2.0f, herbert->GetFollowAngle(), MOTION_SLOT_ACTIVE);

            Creature* crenna = ObjectAccessor::GetCreature(*me, m_crennaGUID);
            crenna->GetMotionMaster()->MoveFollow(herbert, -2.0f, crenna->GetFollowAngle(), MOTION_SLOT_ACTIVE);

            me->SetSpeed(MOVE_RUN, player->GetSpeed(MOVE_RUN));

            me->AddDelayedEvent(1000, [this]() -> void
                {
                    me->GetMotionMaster()->MovePoint(1, 210.077f, -2279.267f, 80.738f, true);
                });
            me->AddDelayedEvent(3500, [this]() -> void
                {
                    me->GetMotionMaster()->MovePoint(1, 215.775f, -2263.325f, 80.455f, true);
                });
            me->AddDelayedEvent(7000, [this]() -> void
                {
                    me->GetMotionMaster()->MovePoint(1, 209.081f, -2243.268f, 83.129f, true);
                });
            me->AddDelayedEvent(11000, [this]() -> void
                {
                    me->GetMotionMaster()->MovePoint(1, 209.794f, -2207.457f, 89.874f, true);
                });
            me->AddDelayedEvent(17000, [this]() -> void
                {
                    me->GetMotionMaster()->MovePoint(1, 226.471f, -2182.809f, 92.941f, true);
                });
            me->AddDelayedEvent(22000, [this]() -> void
                {
                    me->GetMotionMaster()->MovePoint(1, 256.923f, -2178.689f, 96.801f, true);
                });
            me->AddDelayedEvent(26000, [this]() -> void
                {
                    me->GetMotionMaster()->MovePoint(1, 296.684f, -2168.243f, 103.682f, true);
                });
            me->AddDelayedEvent(33000, [this]() -> void
                {
                    me->GetMotionMaster()->MovePoint(1, 321.692f, -2173.496f, 106.295f, true);
                });
            me->AddDelayedEvent(37000, [this, player]() -> void
                {
                    if (!player->GetPhaseShift().HasPhase(PHASE_OGRE_TRANSFORM)) PhasingHandler::AddPhase(player, PHASE_OGRE_TRANSFORM, true);
                    if (Creature* shuja = ObjectAccessor::GetCreature(*me, m_shujaGUID))
                    {
                        shuja->SetRespawnDelay(10);
                        shuja->DespawnOrUnsummon(100ms);
                    }
                    if (Creature* herbert = ObjectAccessor::GetCreature(*me, m_herbertGUID))
                    {
                        herbert->SetRespawnDelay(10);
                        herbert->DespawnOrUnsummon(100ms);
                    }
                    if (Creature* crenna = ObjectAccessor::GetCreature(*me, m_crennaGUID))
                    {
                        crenna->SetRespawnDelay(10);
                        crenna->DespawnOrUnsummon(100ms);
                    }
                    me->SetRespawnDelay(10);
                    me->DespawnOrUnsummon(100ms);
                });
        }
    }

};

//npc_warlord_mulgrin_thunderwalker_167646
struct npc_warlord_mulgrin_thunderwalker_167646 : public ScriptedAI
{
    npc_warlord_mulgrin_thunderwalker_167646(Creature* creature) : ScriptedAI(creature) { }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
        {
            if (player->IsInDist(me, 100.0f))
            {
                if (player->IsActiveQuest(QUEST_H_RIGHT_BENEATH_THEIR_EYES))
                {
                    if (Quest const* quest = sObjectMgr->GetQuestTemplate(QUEST_H_RIGHT_BENEATH_THEIR_EYES))
                        for (QuestObjective const& obj : quest->GetObjectives())
                            if (obj.ObjectID == 16893) // reach citadel entrance
                            {
                                uint16 slot = player->FindQuestSlot(QUEST_H_RIGHT_BENEATH_THEIR_EYES);
                                if (!player->IsQuestObjectiveComplete(slot, quest, obj)) // just make it complete once
                                {
                                    player->SetQuestObjectiveData(obj, 1);
                                    player->SendQuestUpdateAddCredit(quest, ObjectGuid::Empty, obj, 1);

                                    if (Creature* shuja = player->FindNearestCreature(167597, 100.0f, true))
                                    {
                                        me->AddDelayedEvent(3000, [shuja, player]()                    // time_delay
                                            {
                                                shuja->Say(196534);
                                                shuja->PlayDirectSound(157179, player, 196534);
                                            });
                                    }
                                    if (Creature* breka = player->FindNearestCreature(167596, 100.0f, true))
                                    {
                                        me->AddDelayedEvent(6000, [breka, player]()                    // time_delay
                                            {
                                                breka->Say(196535);
                                                breka->PlayDirectSound(157180, player, 196535);
                                            });
                                    }
                                    if (Creature* crenna = player->FindNearestCreature(167599, 100.0f, true))
                                    {

                                    }
                                    if (Creature* herbert = player->FindNearestCreature(167598, 100.0f, true))
                                    {

                                    }
                                }
                            }
                }
            }
        }
    }
};

// npc_herbert_gloomburst_167598
struct npc_herbert_gloomburst_167598 : public ScriptedAI
{
    npc_herbert_gloomburst_167598(Creature* creature) : ScriptedAI(creature) { }

    bool GossipHello(Player* player) override
    {
        if (!player->HasAura(298241) && player->IsActiveQuest(QUEST_H_RIGHT_BENEATH_THEIR_EYES))
        {
            ClearGossipMenuFor(player);
            AddGossipItemFor(player, GossipOptionIcon::None, "I'm ready, transorm me into an ogre and I'll sneak into their citadel.", GOSSIP_SENDER_MAIN, 0);
            AddGossipItemFor(player, GossipOptionIcon::None, "Nevermind..", GOSSIP_SENDER_MAIN, 1);
            SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, me->GetGUID());
        }
        else
            CloseGossipMenuFor(player);
        return true;
    }

    bool GossipSelect(Player* player, uint32 sender, uint32 action) override
    {
        switch (action)
        {
        case 0:

            Talk(0); // A willing test subject! Such a rare find.

            me->CastSpell(player, 313583); // ogre transformation herbert channeling

            if (Creature* crenna = player->FindNearestCreature(167597, 10.0f, true))
            {

                crenna->CastSpell(player, 313598); // ogre transformation crenna channeling
            }

            player->GetScheduler().Schedule(Milliseconds(3000), [this, player](TaskContext context)
                {
                    player->CastSpell(player, 298241); // transforming to ogre
                    player->SetObjectScale(2.0f); // make it big like and ogre
                }).Schedule(Milliseconds(4000), [this, player](TaskContext context)
                    {
                        // give the credit
                        player->KilledMonsterCredit(167598);

                        me->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);

                        // chaining
                        me->CastSpell(player, 329760); // left hand chain
                        me->GetMotionMaster()->MoveFollow(player, -4.0f, me->GetFollowAngle());
                        me->SetSpeed(MOVE_RUN, player->GetSpeed(MOVE_RUN));

                        if (Creature* shuja = player->FindNearestCreature(167597, 10.0f, true))
                        {
                            shuja->CastSpell(player, 329760); // left hand chain
                            shuja->GetMotionMaster()->MoveFollow(player, 2.0f, shuja->GetFollowAngle());
                            shuja->SetSpeed(MOVE_RUN, player->GetSpeed(MOVE_RUN));
                            shuja->AddDelayedEvent(15000, [this, player]() -> void
                                {
                                    me->Talk("Don't speak. Just smile and wave. The dumber you look, the better.", CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, 25, player);
                                });
                        }

                        if (Creature* breka = player->FindNearestCreature(167596, 10.0f, true))
                        {
                            breka->RemoveNpcFlag(UNIT_NPC_FLAG_QUESTGIVER);
                            breka->CastSpell(player, 329760); // left hand chain
                            breka->GetMotionMaster()->MoveFollow(player, -2.0f, breka->GetFollowAngle());
                            breka->SetSpeed(MOVE_RUN, player->GetSpeed(MOVE_RUN));
                            breka->Say(196529);
                            breka->PlayDirectSound(157175, player, 196529);
                        }

                        if (Creature* crenna = player->FindNearestCreature(167599, 10.0f, true))
                        {
                            crenna->CastSpell(player, 329760); // left hand chain
                            crenna->GetMotionMaster()->MoveFollow(player, 4.0f, crenna->GetFollowAngle());
                            crenna->SetSpeed(MOVE_RUN, player->GetSpeed(MOVE_RUN));
                        }
                    });

                CloseGossipMenuFor(player);

                break;
        case 1:

            CloseGossipMenuFor(player);

            break;
        }
        return true;
    }
};

//npc_ogre_brute_guid_1000000000000009
struct npc_ogre_brute_guid_1000000000000009 : public ScriptedAI
{
    npc_ogre_brute_guid_1000000000000009(Creature* creature) : ScriptedAI(creature) { }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
        {
            if (player->IsInDist(me, 40.0f))
            {
                if (player->IsActiveQuest(QUEST_H_RIGHT_BENEATH_THEIR_EYES))
                {
                    if (Quest const* quest = sObjectMgr->GetQuestTemplate(QUEST_H_RIGHT_BENEATH_THEIR_EYES))
                        for (QuestObjective const& obj : quest->GetObjectives())
                            if (obj.ObjectID == 16996) // enter dark maul citadel
                            {
                                uint16 slot = player->FindQuestSlot(QUEST_H_RIGHT_BENEATH_THEIR_EYES);
                                if (!player->IsQuestObjectiveComplete(slot, quest, obj)) // just make it complete once
                                {
                                    player->SetQuestObjectiveData(obj, 1);
                                    player->SendQuestUpdateAddCredit(quest, ObjectGuid::Empty, obj, 1);

                                    me->Talk("More sacrifices! Go to cooking pit, wave at Gor'groth. Show what you got!", CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, 25, player);

                                    if (Creature* breka = player->FindNearestCreature(167596, 40.0f, true))
                                    {
                                        me->AddDelayedEvent(3000, [breka, player]()                    // time_delay
                                            {
                                                breka->Say(196531);
                                                breka->PlayDirectSound(157177, player, 196531);
                                            });
                                    }
                                }
                            }
                }
            }
        }
    }
};

//npc_lightspawn_157114
class npc_lightspawn_157114 : public CreatureScript
{
public:
    npc_lightspawn_157114() : CreatureScript("npc_lightspawn_157114") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_lightspawn_157114AI(creature);
    }

    struct npc_lightspawn_157114AI : public ScriptedAI
    {
        npc_lightspawn_157114AI(Creature* creature) : ScriptedAI(creature) { }

        void QuestAccept(Player* player, Quest const* quest) override
        {
            if (quest->GetQuestId() == QUEST_FREEING_THE_LIGHT)
            {
                // Dispel the foul magic that binds me here. 
                me->Say(179206);
                me->PlayDirectSound(153065, player, 179206);

                player->CastSpell(player, SPELL_LIGHTSPAWN_LIGHTSBOON, true);
            }
        }

        void QuestReward(Player* player, Quest const* quest, LootItemType /*type*/, uint32 /*opt*/) override
        {
            if (quest->GetQuestId() == QUEST_FREEING_THE_LIGHT)
            {
                // Accept my blessing, and end the necromancy that plagues this island. 
                me->Say(179243);
                me->PlayDirectSound(153071, player, 179243);

                if (GameObject* pedestal1 = me->FindNearestGameObject(326716, 50.0f))
                    pedestal1->AI()->Reset();

                player->RemoveAura(SPELL_LIGHTSPAWN_LIGHTSBOON);
            }
        }

    };
};

struct go_ritual_predestal_326716_326717_326718_326719 : public GameObjectAI
{

    void Reset()
    {
        if (Creature* InvisBunny_OnPedestal = me->FindNearestCreature(NPC_LIGHTSPAWN_PEDESTAL_INVISBUNNY, 2.0f))
        {
            InvisBunny_OnPedestal->RemoveAllAuras();
            InvisBunny_OnPedestal->CastSpell(InvisBunny_OnPedestal, SPELL_LIGHTSPAWN_NECROTICBALL, true);
        }
    }

    go_ritual_predestal_326716_326717_326718_326719(GameObject* go) : GameObjectAI(go)
    {
        Reset();
    }

    bool GossipHello(Player* player) override
    {
        player->KillCreditGO(me->GetEntry());

        if (Creature* InvisBunny_OnPedestal = player->FindNearestCreature(NPC_LIGHTSPAWN_PEDESTAL_INVISBUNNY, 10.0f))
        {
            InvisBunny_OnPedestal->RemoveAllAuras();
            InvisBunny_OnPedestal->CastSpell(InvisBunny_OnPedestal, SPELL_LIGHTSPAWN_LIGHTBALL, true);

            InvisBunny_OnPedestal->DespawnOrUnsummon(120s, 5s); // despawn after 2min and respawn after 5s
        }

        if (Creature* LightSpawn = player->FindNearestCreature(157114, 50.0f))
        {
            if (player->GetQuestObjectiveProgress(QUEST_FREEING_THE_LIGHT, 1) == 1)
            {
                // I attempted to bring an end to these rituals... 
                LightSpawn->Say(179202);
                LightSpawn->PlayDirectSound(153066, player, 179202);
            }
            if (player->GetQuestObjectiveProgress(QUEST_FREEING_THE_LIGHT, 2) == 1)
            {
                // But the ogres ensnared me... drained my power... 
                LightSpawn->Say(179203);
                LightSpawn->PlayDirectSound(153067, player, 179203);
            }
            if (player->GetQuestObjectiveProgress(QUEST_FREEING_THE_LIGHT, 3) == 1)
            {
                // Free me, so I might recover... and pass some of my power to you.  
                LightSpawn->Say(179204);
                LightSpawn->PlayDirectSound(153068, player, 179204);
            }
            if (player->GetQuestObjectiveProgress(QUEST_FREEING_THE_LIGHT, 4) == 1)
            {
                player->CastSpell(player, SPELL_LIGHTSPAWN_FREED, true);
                // The spell is weakening... the Light returns to me!  
                LightSpawn->Say(179205);
                LightSpawn->PlayDirectSound(153069, player, 179205);
            }
        }

        return true;
    }

};

//325364 - resizer blast
class spell_resizer_blast : public SpellScript
{
    PrepareSpellScript(spell_resizer_blast);

    void HandleDummy(SpellEffIndex effIndex)
    {
        PreventHitDefaultEffect(effIndex);

        Unit* caster = GetCaster();
        Vehicle* vehicle = caster->GetVehicleKit();
        Unit* passenger = vehicle->GetPassenger(0);
        Player* player = passenger->ToPlayer();

        if (WorldLocation* targetedloc = GetHitDest())
        {
            player->CastSpell(targetedloc->GetPosition(), 325365, true);
        }
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_resizer_blast::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

//325365 - resizer blast explosion
class spell_resizer_blast_explosion : public SpellScript
{
    PrepareSpellScript(spell_resizer_blast_explosion);

    void HandleOnHit(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();

        if (WorldLocation* targetedloc = GetHitDest())
            if (TempSummon* tempSumm = caster->SummonCreature(WORLD_TRIGGER, targetedloc->GetPosition(), TEMPSUMMON_TIMED_DESPAWN, 3s))
            {
                tempSumm->SetFaction(caster->GetFaction());
                PhasingHandler::AddPhase(tempSumm, PHASE_ZOMBIE, true); // add zombie phase to find zombies

                std::list<Creature*> spellTargetCreatures = tempSumm->FindAllCreaturesInRange(15.0f);
                for (auto& target : spellTargetCreatures)
                {
                    if (target->IsAlive())
                        if (target->GetEntry() == 157091 || target->GetEntry() == 156532)
                        {
                            CastSpellExtraArgs args;
                            args.SetOriginalCaster(caster->GetGUID());
                            args.SetTriggerFlags(TRIGGERED_FULL_MASK);

                            tempSumm->CastSpell(target, 305721, args); // resizer blast explosion

                            if (caster->IsPlayer() && target->GetEntry() == 157091) // add kill credit for big zombie
                                caster->ToPlayer()->KilledMonsterCredit(165150);
                        }
                }
            }
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_resizer_blast_explosion::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

//344248 - exit vehicle
class spell_exit_vehicle_choppy : public SpellScript
{
    PrepareSpellScript(spell_exit_vehicle_choppy);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Unit* choppy = GetCaster();

        choppy->ToCreature()->DespawnOrUnsummon(100ms);
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_exit_vehicle_choppy::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

void AddSC_zone_exiles_reach()
{
    new exiles_reach();
    RegisterCreatureAI(warlord_breka_grimaxe_166573);
    new warlord_breka_grimaxe_166782();
    RegisterCreatureAI(warlord_breka_grimaxe_166906);
    RegisterCreatureAI(npc_Mithdran_Rastrealba_166791);
    RegisterCreatureAI(npc_Lana_Jordan_166796);
    RegisterCreatureAI(npc_Bo_166786);
    RegisterCreatureAI(npc_Jinhake_16680);
    RegisterCreatureAI(npc_Throg_166784);
    RegisterCreatureAI(grunt_throg_166583);
    RegisterCreatureAI(npc_wonsa_166854);
    RegisterCreatureAI(npc_wonsa_175030);
    RegisterCreatureAI(npc_Mithdran_Dawntracker_166996);
    RegisterCreatureAI(npc_combat_dummy);
    RegisterCreatureAI(npc_private_cole_160664);
    RegisterCreatureAI(npc_private_cole_dummy);
    RegisterItemScript(item_first_aid_kit);
    RegisterGameObjectAI(go_campfire_339769);
    RegisterCreatureAI(npc_Cork_Fizzlepop_167019);
    RegisterCreatureAI(npc_Choppy_Booster_167027);
    RegisterCreatureAI(npc_Choppy_Booster_167142);
    RegisterCreatureAI(npc_Shuja_Grimaxe_167126);
    RegisterCreatureAI(npc_Shuja_Grimaxe_167219);
    RegisterCreatureAI(npc_Shuja_Grimaxe_167290);
    RegisterCreatureAI(npc_Bo_167291);
    RegisterCreatureAI(npc_Herbert_Gloomburst_167182);
    RegisterCreatureAI(npc_Lana_Jordan_167221);
    RegisterCreatureAI(npc_Lana_Jordan_167225);
    RegisterCreatureAI(npc_Crenna_Earth_Daughter_167250);
    RegisterCreatureAI(npc_Crenna_Earth_Daughter_167254);
    RegisterCreatureAI(npc_Warlord_Breka_Grimaxe_167212);
    RegisterCreatureAI(npc_Monstrous_Cadaver_157091);
    RegisterCreatureAI(npc_lindie_springstock_149899);
    RegisterCreatureAI(npc_scout_o_matic_5000);
    RegisterItemScript(item_resizer_v901);
    new item_h_resizer_v901();
    RegisterCreatureAI(npc_giant_boar);
    RegisterSpellScript(spell_giant_boar_trample);
    RegisterCreatureAI(npc_meredy_huntswell_156943);
    RegisterCreatureAI(npc_henry_garrick_156799);
    RegisterCreatureAI(npc_captain_garrick_156651);
    RegisterCreatureAI(npc_captain_garrick_156280);
    RegisterCreatureAI(npc_alaria_175031);
    RegisterCreatureAI(npc_geolord_grekog_151091);
    RegisterCreatureAI(npc_ralia_dreamchacer_156929);
    RegisterCreatureAI(npc_alliance_gryphon_154155);
    RegisterCreatureAI(npc_coulston_nereus);
    RegisterCreatureAI(hrun_the_exiled_156900);
    // new phasehandler_exiles_reach();
    RegisterCreatureAI(npc_gutgruck_the_tough);
    RegisterAreaTriggerAI(at_for_gutgruck_the_tough);
    RegisterCreatureAI(npc_dead_exile_reach);
    RegisterCreatureAI(npc_austin_huxworth);
    RegisterCreatureAI(npc_captain_garrick_156626);
    RegisterCreatureAI(npc_ogre_overseer_156676);
    RegisterCreatureAI(npc_quilboars_warrior_geomancer_150237_150238);
    RegisterCreatureAI(npc_gorgroth_153580);
    RegisterCreatureAI(npc_torgok_162817);
    RegisterGameObjectAI(go_thick_cocoon_339568_350796);
    RegisterGameObjectAI(go_harpy_totem_350803);
    RegisterCreatureAI(npc_Freed_Expedition_Member_167244_160964);
    RegisterCreatureAI(npc_harpie_snatcher_152571);
    RegisterCreatureAI(npc_harpie_boltcaller_152843);
    RegisterCreatureAI(npc_hunting_worg_152998);
    RegisterCreatureAI(npc_breka_grimaxe_167145);
    RegisterCreatureAI(npc_barrow_spider_160394);
    RegisterCreatureAI(npc_breka_grimaxe_167596);
    RegisterCreatureAI(npc_warlord_mulgrin_thunderwalker_167646);
    RegisterCreatureAI(npc_herbert_gloomburst_167598);
    RegisterCreatureAI(npc_ogre_brute_guid_1000000000000009);
    new npc_lightspawn_157114();
    RegisterGameObjectAI(go_ritual_predestal_326716_326717_326718_326719);
    RegisterSpellScript(spell_resizer_blast);
    RegisterSpellScript(spell_resizer_blast_explosion);
    RegisterSpellScript(spell_exit_vehicle_choppy);
}
