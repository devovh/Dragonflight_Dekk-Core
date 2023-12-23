/*
 * Copyright 2022 DekkCore
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
#include "Player.h"
#include "ScriptMgr.h"
#include "ObjectMgr.h"
#include "ZoneScript.h"
#include "zone_exiles_reach.h"

/////////////////////////////////////////////////////////////////////////////
// This script has been made for handling all phase change in exile reach. //
/////////////////////////////////////////////////////////////////////////////

/* Note! This phase handling based on actual creature spawn phases! (2022.02.28) */

enum ExilesReach_Phases
{
    PHASE_GENERIC = 12940,
    HORDE_PHASE_1 = 12930,
    HORDE_PHASE_2 = 12950,
    PHASE_ZOMBIE = 16752,
    PHASE_KILLING_TORGOK = 12980,
    PHASE_AFTER_ZOMBIEKILL = 12990,
    HERBERT_IS_IN_TROUBLE = 13000,
    PHASE_BEFORE_SPIDER_KILL = 13010,
    PHASE_AFTER_HARPIE_KILL = 13020,
    PHASE_AFTER_SPIDER_KILL = 13050,
    PHASE_OGRE_TRANSFORM = 13060,
    ALLIANCE_PHASE_1 = 13157,
    ALLIANCE_PHASE_2 = 13784,
    ALLIANCE_PHASE_3 = 13814,
    ALLIANCE_PHASE_4 = 13779,
    ALLIANCE_PHASE_5 = 13780,
    ALLIANCE_PHASE_6 = 13070,
    ALLIANCE_PHASE_7 = 13080,
    ALLIANCE_PHASE_8 = 13090,
    ALLIANCE_PHASE_9 = 12960,
    ALLIANCE_PHASE_10 = 12970,
    ALLIANCE_PHASE_11 = 13100,
    ALLIANCE_PHASE_12 = 13110,
    ALLIANCE_PHASE_13 = 13317,
    ALLIANCE_PHASE_1313 = 13443,
    ALLIANCE_PHASE_14 = 13503,
    ALLIANCE_PHASE_1414 = 13776,
    ALLIANCE_PHASE_15 = 13762,
    ALLIANCE_PHASE_16 = 13766,
    ALLIANCE_PHASE_17 = 13619,
    ALLIANCE_PHASE_18 = 13566,
    ALLIANCE_PHASE_19 = 13783,
    ALLIANCE_PHASE_20 = 13845,
    ALLIANCE_PHASE_21 = 13878,
    ALLIANCE_PHASE_22 = 13880,
    ALLIANCE_PHASE_23 = 14677,
    ALLIANCE_PHASE_24 = 15274,
    ALLIANCE_PHASE_25 = 12941,
};

class exiles_reach_phasehandler : public PlayerScript
{
public:
    exiles_reach_phasehandler() : PlayerScript("exiles_reach_phasehandler") { }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // CUSTOM PHASING FUNCTIONS ------------------------------------------------------------------------------------------- >
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        // shorten the phase add method
        void AddPhase(Player* player, uint32 phaseID)
        {
            if (!player->GetPhaseShift().HasPhase(phaseID))
                PhasingHandler::AddPhase(player, phaseID, true);
        }

        // shorten the phase remove method
        void RemovePhase(Player* player, uint32 phaseID)
        {
            if (player->GetPhaseShift().HasPhase(phaseID))
                PhasingHandler::RemovePhase(player, phaseID, true);
        }

        // only activate phase if quest objective not done
        void SwitchPhaseBasedOnQObjStatus(Player* player, uint32 phaseID, uint32 questID, uint32 questObjectiveIndex, uint32 questObjectiveAmount)
        {
            //auto quest = sObjectMgr->GetQuestTemplate(questID);
            //int8 objamount = quest->Objectives[questObjectiveIndex].Amount;

            if (player->HasQuest(questID) && player->GetQuestStatus(questID) != QUEST_STATUS_REWARDED)
            {
                if (player->GetQuestObjectiveProgress(questID, questObjectiveIndex) != questObjectiveAmount)
                    AddPhase(player, phaseID);

                if (player->GetQuestObjectiveProgress(questID, questObjectiveIndex) == questObjectiveAmount)
                    RemovePhase(player, phaseID);
            }
        }

        // add phase if quest status is the specified
        void AddPhaseForQuestStatus(Player* player, uint32 questID, uint32 questStatus, uint32 phaseID)
        {
            if (player->HasQuest(questID) && player->GetQuestStatus(questID) == questStatus)
                    AddPhase(player, phaseID);
        }

        void SwitchPhaseByQuestStatus(Player* player, uint32 questID, uint32 questStatus, uint32 oldPhaseID, uint32 newPhaseID)
        {
            if (player->HasQuest(questID) && player->GetQuestStatus(questID) == questStatus)
            {
                RemovePhase(player, oldPhaseID);
                AddPhase(player, newPhaseID);
            }
        }

        // activate phase for zone if player has quest
        void SwitchPhaseByAreaID(Player* player, uint32 questID, uint32 oldPhaseID, uint32 newPhaseID, uint32 areaID)
        {
            if (player->HasQuest(questID) && player->GetAreaId() == areaID)
            {
                RemovePhase(player, oldPhaseID);
                AddPhase(player, newPhaseID);
            }
        }

        // Add phaseIfQuestActive if the player accepted the quest but not rewarded. Adds the phaseIfQuestDone if the quest rewarded.
        // For questID add phaseIfQuestActive and add phaseIfQuestDone
        void SwitchPhaseByRewardedQuest(Player* player, uint32 questID, uint32 phaseIfQuestActive, uint32 phaseIfQuestDone)
        {
            // not rewarded yet but accepted (does nothing if 0)
            if (player->HasQuest(questID) && phaseIfQuestActive != 0)
            {
                AddPhase(player, phaseIfQuestActive);
            }
            // rewarded already (does nothing if 0)
            else if (player->GetQuestStatus(questID) == QUEST_STATUS_REWARDED && phaseIfQuestDone != 0)
            {
                //remove phase if it's still active
                if (phaseIfQuestActive != 0)
                    RemovePhase(player, phaseIfQuestActive);

                AddPhase(player, phaseIfQuestDone);
            }
        }

        // This is for stucked phase when creatures spawned in wrong phases and im tired to spawn them to right phases
        void RemovePhaseOnlyInActiveQuest(Player* player, uint32 questID, uint32 phaseToRemove)
        {
            // not rewarded yet but accepted
            if (player->HasQuest(questID))
                RemovePhase(player, phaseToRemove);
        }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // THE ULTIMATE PHASE HANDLING ---------------------------------------------------------------------------------------- >
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void CheckExilesReachPhase(Player* player)
        {
            if (player->GetMapId() == MAP_EXILES_REACH)
            {
                // Clear all phases
                player->GetPhaseShift().ClearPhases();

                // Add generic phase
                AddPhase(player, PHASE_GENERIC);

                // HORDE
                if (player->IsInHorde())
                {
                    // QUEST_H_WARMING_UP (phase 0)
                    // QUEST_H_STAND_YOUR_GROUND (phase 0)
                    // QUEST_H_BRACE_FOR_IMPACT (phase 0)
                    SwitchPhaseByRewardedQuest(player, QUEST_H_BRACE_FOR_IMPACT,              HORDE_PHASE_1,          HORDE_PHASE_1);
                    SwitchPhaseByRewardedQuest(player, QUEST_H_MURLOCK_MANIA,                 HORDE_PHASE_1,          HORDE_PHASE_1);
                    SwitchPhaseByRewardedQuest(player, QUEST_H_EMERGENCY_FIRST_AID,           HORDE_PHASE_1,          HORDE_PHASE_2);
                    SwitchPhaseByRewardedQuest(player, QUEST_H_FINDING_THE_LOST_EXPEDITION,   HORDE_PHASE_2,          ALLIANCE_PHASE_9);
                    SwitchPhaseByRewardedQuest(player, QUEST_H_COOKING_MEAT,                  ALLIANCE_PHASE_9,       ALLIANCE_PHASE_9);
                    SwitchPhaseByRewardedQuest(player, QUEST_H_ENHANCED_COMBAT_TACTICS,       ALLIANCE_PHASE_9,       ALLIANCE_PHASE_10);
                    SwitchPhaseByRewardedQuest(player, QUEST_H_NORTHBOUND,                    ALLIANCE_PHASE_10,      ALLIANCE_PHASE_10);
                    SwitchPhaseByRewardedQuest(player, QUEST_H_NORTHBOUND,                    PHASE_AFTER_ZOMBIEKILL, PHASE_AFTER_ZOMBIEKILL);
                    SwitchPhaseByRewardedQuest(player, QUEST_H_DOWN_WITH_THE_QUILBOAR,        ALLIANCE_PHASE_10,      ALLIANCE_PHASE_10);
                    SwitchPhaseByRewardedQuest(player, QUEST_H_FORBIDDEN_QUILBOAR_NECROMANCY, ALLIANCE_PHASE_10,      ALLIANCE_PHASE_10);
                    SwitchPhaseByRewardedQuest(player, QUEST_H_FORBIDDEN_QUILBOAR_NECROMANCY, PHASE_KILLING_TORGOK,   PHASE_KILLING_TORGOK);
                    SwitchPhaseByRewardedQuest(player, QUEST_H_THE_CHOPPY_BOOSTER_MK5,        ALLIANCE_PHASE_10,      ALLIANCE_PHASE_10);
                    SwitchPhaseByRewardedQuest(player, QUEST_H_RESIZING_THE_SITUATION,        ALLIANCE_PHASE_10,      ALLIANCE_PHASE_10);
                    SwitchPhaseByRewardedQuest(player, QUEST_H_THE_REDEATHER,                 PHASE_KILLING_TORGOK,   PHASE_AFTER_ZOMBIEKILL);
                    RemovePhaseOnlyInActiveQuest(player, QUEST_H_THE_REDEATHER, PHASE_AFTER_ZOMBIEKILL);
                    // if zombies not dead yet we see zombies otherwise we dont
                    SwitchPhaseBasedOnQObjStatus(player, PHASE_ZOMBIE, QUEST_H_THE_REDEATHER, 2, 8);
                    // when zombies and torgok finally dead
                    SwitchPhaseByRewardedQuest(player, QUEST_H_THE_REDEATHER, 0, PHASE_AFTER_ZOMBIEKILL);
                    // QUEST_H_STOCKING_UP_ON_SUPPLIES (next activate handles it)
                    // QUEST_H_THE_HARPY_PROBLEM (next activate handles it)
                    // QUEST_H_HARPY_CULLING (next activate handles it)
                    SwitchPhaseByRewardedQuest(player, QUEST_H_THE_RESCUE_OF_HERBERT_GLOOMBURST, PHASE_AFTER_ZOMBIEKILL, PHASE_BEFORE_SPIDER_KILL);
                    SwitchPhaseByRewardedQuest(player, QUEST_H_THE_RESCUE_OF_HERBERT_GLOOMBURST, HERBERT_IS_IN_TROUBLE, 0);
                    SwitchPhaseByRewardedQuest(player, QUEST_H_MESSAGE_TO_BASE, PHASE_BEFORE_SPIDER_KILL, PHASE_AFTER_HARPIE_KILL);
                    SwitchPhaseByRewardedQuest(player, QUEST_H_WESTWARD_BOUND, PHASE_AFTER_HARPIE_KILL, ALLIANCE_PHASE_3);
                    AddPhaseForQuestStatus(player, QUEST_H_WHO_LURKS_IN_THE_PIT, QUEST_STATUS_COMPLETE, PHASE_AFTER_SPIDER_KILL);
                    SwitchPhaseByRewardedQuest(player, QUEST_H_WHO_LURKS_IN_THE_PIT, ALLIANCE_PHASE_3, PHASE_OGRE_TRANSFORM);
                    SwitchPhaseByRewardedQuest(player, QUEST_H_TO_DARKMAUL_CITADEL, PHASE_OGRE_TRANSFORM, ALLIANCE_PHASE_6);
                    SwitchPhaseByAreaID(player, QUEST_H_TO_DARKMAUL_CITADEL, PHASE_OGRE_TRANSFORM, ALLIANCE_PHASE_6, 10568); // darkmaul ridge
                    SwitchPhaseByQuestStatus(player, QUEST_H_RIGHT_BENEATH_THEIR_EYES, QUEST_STATUS_COMPLETE, ALLIANCE_PHASE_6, ALLIANCE_PHASE_7);
                    SwitchPhaseByRewardedQuest(player, QUEST_H_RIGHT_BENEATH_THEIR_EYES, ALLIANCE_PHASE_6, ALLIANCE_PHASE_7);
                    // QUEST_H_LIKE_OGRES_TO_THE_SLAUGHTER (last switch still work)
                    // QUEST_H_CATAPULT_DESTRUCTION (last switch still work)
                    SwitchPhaseByQuestStatus(player, QUEST_H_CONTROLLING_THEIR_STONES, QUEST_STATUS_COMPLETE, ALLIANCE_PHASE_7, ALLIANCE_PHASE_8);
                    SwitchPhaseByRewardedQuest(player, QUEST_H_CONTROLLING_THEIR_STONES, ALLIANCE_PHASE_7, ALLIANCE_PHASE_8);
                    SwitchPhaseByRewardedQuest(player, QUEST_H_CONTROLLING_THEIR_STONES, ALLIANCE_PHASE_10, ALLIANCE_PHASE_8);
                    RemovePhaseOnlyInActiveQuest(player, QUEST_H_DUNGEON_DARKMAUL_CITADEL, PHASE_GENERIC);
                    SwitchPhaseByQuestStatus(player, QUEST_H_DUNGEON_DARKMAUL_CITADEL, QUEST_STATUS_COMPLETE, ALLIANCE_PHASE_8, ALLIANCE_PHASE_12);
                    SwitchPhaseByRewardedQuest(player, QUEST_H_DUNGEON_DARKMAUL_CITADEL, ALLIANCE_PHASE_8, ALLIANCE_PHASE_12);
                    /*
                        QUEST_H_AN_END_TO_BEGINNINGS = 59985,
                    */
                }

                // ALLIANCE
                if (player->IsInAlliance())
                {
                    //QUEST_WARMING_UP = 56775, // phase 0
                    //QUEST_STAND_YOUR_GROUND = 58209, // phase 0
                    //QUEST_BRACE_FOR_IMPACT = 58208, // phase 0
                    SwitchPhaseByRewardedQuest(player, QUEST_BRACE_FOR_IMPACT, ALLIANCE_PHASE_1, ALLIANCE_PHASE_1);
                    SwitchPhaseByRewardedQuest(player, QUEST_MURLOC_MANIA, ALLIANCE_PHASE_1, ALLIANCE_PHASE_1);
                    SwitchPhaseByRewardedQuest(player, QUEST_EMERGENCY_FIRST_AID, ALLIANCE_PHASE_1, ALLIANCE_PHASE_13);
                    SwitchPhaseByRewardedQuest(player, QUEST_FINDING_THE_LOST_EXPEDITION, ALLIANCE_PHASE_13, ALLIANCE_PHASE_1313);
                    //QUEST_COOKING_MEAT = 55174, // still the latest phase
                    SwitchPhaseByRewardedQuest(player, QUEST_ENHANCED_COMBAT_TACTICS, ALLIANCE_PHASE_1313, ALLIANCE_PHASE_14);
                    SwitchPhaseByRewardedQuest(player, QUEST_NORTHBOUND, ALLIANCE_PHASE_14, ALLIANCE_PHASE_18);
                    //QUEST_DOWN_WITH_THE_QUILBOAR = 55186, // still the latest phase
                    SwitchPhaseByRewardedQuest(player, QUEST_FORBIDDEN_QUILBOAR_NECROMANCY, ALLIANCE_PHASE_18, ALLIANCE_PHASE_17);
                    //QUEST_THE_SCOUT_O_MATIC_5000 = 55193, ALLIANCE_PHASE_17
                    //QUEST_RE_SIZING_THE_SITUATION = 56034, ALLIANCE_PHASE_17
                    //QUEST_RIDE_OF_THE_SCIENTIFICALLY_ENHANCED_BOAR = 55879, ALLIANCE_PHASE_17
                    //QUEST_THE_HARPY_PROBLEM = 55196, ALLIANCE_PHASE_2
				    SwitchPhaseByRewardedQuest(player, QUEST_RIDE_OF_THE_SCIENTIFICALLY_ENHANCED_BOAR, ALLIANCE_PHASE_17, ALLIANCE_PHASE_2);
                    AddPhaseForQuestStatus(player, QUEST_RIDE_OF_THE_SCIENTIFICALLY_ENHANCED_BOAR, QUEST_STATUS_INCOMPLETE, PHASE_ZOMBIE);
                    //QUEST_THE_RESCUE_OF_MEREDY_HUNTSWELL = 55763, ALLIANCE_PHASE_1414 ALLIANCE_PHASE_15
                    AddPhaseForQuestStatus(player, QUEST_THE_RESCUE_OF_MEREDY_HUNTSWELL, QUEST_STATUS_INCOMPLETE, ALLIANCE_PHASE_15);
				    SwitchPhaseByRewardedQuest(player, QUEST_THE_HARPY_PROBLEM, ALLIANCE_PHASE_17, ALLIANCE_PHASE_1414);
                    //QUEST_HARPY_CULLING = 55764, ALLIANCE_PHASE_1414
                    AddPhaseForQuestStatus(player, QUEST_PURGE_THE_TOTEMS, QUEST_STATUS_INCOMPLETE, ALLIANCE_PHASE_25);
                    //QUEST_PURGE_THE_TOTEMS = 55881, ALLIANCE_PHASE_1414 ALLIANCE_PHASE_25
                    //QUEST_MESSAGE_TO_BASE = 55882, ALLIANCE_PHASE_1414
                    //QUEST_STOCKING_UP_ON_SUPPLIES = 55194, ALLIANCE_PHASE_1414
                    //QUEST_WESTWARD_BOUND = 55965, ALLIANCE_PHASE_4
				    SwitchPhaseByRewardedQuest(player, QUEST_STOCKING_UP_ON_SUPPLIES, ALLIANCE_PHASE_1414, ALLIANCE_PHASE_20);
                    //QUEST_WHO_LURKS_IN_THE_PIT = 55639, ALLIANCE_PHASE_4
                    //QUEST_TO_DARKMAUL_CITADEL = 56344, ALLIANCE_PHASE_22 ALLIANCE_PHASE_20
				    SwitchPhaseByRewardedQuest(player, QUEST_WHO_LURKS_IN_THE_PIT, ALLIANCE_PHASE_4, ALLIANCE_PHASE_22);
                    //QUEST_RIGHT_BENEATH_THEIR_EYES = 55981, ALLIANCE_PHASE_22 questender 156961 no spawned
                    //QUEST_LIKE_OGRES_TO_THE_SLAUGHTER = 55988, ALLIANCE_PHASE_24
                    SwitchPhaseByRewardedQuest(player, QUEST_LIKE_OGRES_TO_THE_SLAUGHTER, ALLIANCE_PHASE_21, ALLIANCE_PHASE_21);
				    SwitchPhaseByRewardedQuest(player, QUEST_RIGHT_BENEATH_THEIR_EYES, ALLIANCE_PHASE_24, ALLIANCE_PHASE_22);
                    //QUEST_CATAPULT_DESTRUCTION = 55989, queststarter/questender 156960 no spawned
                    //QUEST_CONTROLLING_THEIR_STONES = 55990, ALLIANCE_PHASE_24 ALLIANCE_PHASE_21 queststarter 156961 no spawned
				    RemovePhaseOnlyInActiveQuest(player, QUEST_CONTROLLING_THEIR_STONES, ALLIANCE_PHASE_25);
				    AddPhaseForQuestStatus(player, QUEST_CATAPULT_DESTRUCTION, QUEST_STATUS_COMPLETE, ALLIANCE_PHASE_21);
                    //QUEST_DUNGEON_DARKMAUL_CITADEL = 55992, ALLIANCE_PHASE_22 questender 156961 no spawned
                    //QUEST_AN_END_TO_BEGINNINGS = 55991, ALLIANCE_PHASE_22 and ALLIANCE_PHASE_24 queststarter 156961 no spawned
                    AddPhaseForQuestStatus(player, QUEST_AN_END_TO_BEGINNINGS, QUEST_STATUS_INCOMPLETE, ALLIANCE_PHASE_24);
                }
            }
        }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // AFTER MOVIES ---------------------------------------------------------------------------------------------------------------------------------------------- >
    void OnMovieComplete(Player* player, uint32 movieId) override
    {
        /* *********** HORDE > *********** */
        if (movieId == 931)
        {
            player->GetScheduler().Schedule(Milliseconds(3000), [player](TaskContext context)
                {
                    if (!player->GetPhaseShift().HasPhase(HORDE_PHASE_1)) PhasingHandler::AddPhase(player, HORDE_PHASE_1, true);
                    if (!player->GetPhaseShift().HasPhase(PHASE_GENERIC)) PhasingHandler::AddPhase(player, PHASE_GENERIC, true);
                });
        }
        /* ***********< HORDE *********** */

        /* *********** ALLIANCE > *********** */
        if (movieId == 895)
        {
            player->GetScheduler().Schedule(Milliseconds(2000), [player](TaskContext context)
                {
                    if (!player->GetPhaseShift().HasPhase(ALLIANCE_PHASE_1)) PhasingHandler::AddPhase(player, ALLIANCE_PHASE_1, true);
                    if (!player->GetPhaseShift().HasPhase(PHASE_GENERIC)) PhasingHandler::AddPhase(player, PHASE_GENERIC, true);
                });
        }
        /* *********** < ALLIANCE *********** */
    }

    // AFTER SCENES --------------------------------------------------------------------------------------------------------------------------------------------------- >
    void OnSceneComplete(Player* player, uint32 /*sceneInstanceID*/) override
    {
        CheckExilesReachPhase(player);
    }
    void OnSceneCancel(Player* player, uint32 /*sceneInstanceID*/) override
    {
        CheckExilesReachPhase(player);
    }

    // ON AREA CHANGE ------------------------------------------------------------------------------------------------------------------------------------------------ >
    void OnUpdateArea(Player* player, uint32 /*newArea*/, uint32 /*oldArea*/) override
    {
        CheckExilesReachPhase(player);
    }

    // ON QUEST STATUS CHANGE ---------------------------------------------------------------------------------------------------------------------------------------- >
    void OnQuestStatusChange(Player* player, uint32 /*questId*/) override
    {
        CheckExilesReachPhase(player);
    }

    // ON ENTER VEHICLE ---------------------------------------------------------------------------------------------------------------------------------------------- >
    void OnPlayerEnterVehicle(Player* player) override
    {
        CheckExilesReachPhase(player);
    }

    // ON EXIT VEHICLE ----------------------------------------------------------------------------------------------------------------------------------------------- >
    void OnPlayerExitVehicle(Player* player) override
    {
        CheckExilesReachPhase(player);
    }

    // ON SPIRIT HEALER REVIVE --------------------------------------------------------------------------------------------------------------------------------------- >
    void OnPlayerRepop(Player* player) override
    {
        CheckExilesReachPhase(player);
    }

    // ON RELEASE SPIRIT --------------------------------------------------------------------------------------------------------------------------------------------- >
    void OnPlayerResurrect(Player* player) override
    {
        CheckExilesReachPhase(player);
    }

    // ON PLAYER LOGIN ----------------------------------------------------------------------------------------------------------------------------------------------- >
    void OnLogin(Player* player, bool /*firstLogin*/) override
    {
        CheckExilesReachPhase(player);
    }

    // ON PLAYER GM OFF ----------------------------------------------------------------------------------------------------------------------------------------------- >
    void OnPlayerGMOFF(Player* player) override
    {
        CheckExilesReachPhase(player);
    }

};

void AddSC_zone_exiles_reach_phasehandler()
{
    new exiles_reach_phasehandler();
}
