/*
 * Copyright 2023 Fluxurion
 */

/*
-:: FluxPhaseMgr ::-

A custom phase manager which can add/remove phase to player based on options from database.

Created by Fluxurion in 2022. (DekkCore)

-- Table Structure SQL Query:

CREATE TABLE IF NOT EXISTS `flux_phasing` (
  `ID` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `Method` int(1) unsigned NOT NULL DEFAULT 0,
  `PhaseID` int(11) unsigned NOT NULL DEFAULT 0,
  `ActiveQuestID` int(11) unsigned NOT NULL DEFAULT 0,
  `QuestObjectiveIndex` int(11) unsigned NOT NULL DEFAULT 0,
  `QuestObjectiveAmount` int(11) unsigned NOT NULL DEFAULT 0,
  `CompletedQuestID` int(11) unsigned NOT NULL DEFAULT 0,
  `RewardedQuestID` int(11) unsigned NOT NULL DEFAULT 0,
  `MapID` int(11) unsigned NOT NULL DEFAULT 0,
  `ZoneID` int(11) unsigned NOT NULL DEFAULT 0,
  `AreaID` int(11) unsigned NOT NULL DEFAULT 0,
  `OnMovieComplete` int(1) unsigned NOT NULL DEFAULT 1,
  `OnSceneComplete` int(1) unsigned NOT NULL DEFAULT 1,
  `OnSceneCancel` int(1) unsigned NOT NULL DEFAULT 1,
  `OnUpdateArea` int(1) unsigned NOT NULL DEFAULT 1,
  `OnQuestStatusChange` int(1) unsigned NOT NULL DEFAULT 1,
  `OnPlayerEnterVehicle` int(1) unsigned NOT NULL DEFAULT 1,
  `OnPlayerExitVehicle` int(1) unsigned NOT NULL DEFAULT 1,
  `OnPlayerRepop` int(1) unsigned NOT NULL DEFAULT 1,
  `OnPlayerResurrect` int(1) unsigned NOT NULL DEFAULT 1,
  `OnLogin` int(1) unsigned NOT NULL DEFAULT 1,
  `OnGmOff` int(1) unsigned NOT NULL DEFAULT 1,
  `Comment` text DEFAULT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- Quick info:
- ID is an identifier with auto increment to separate custom flux_phasings
- Method, 0 does nothing, 1 for add phase, 2 for remove phase
- PhaseID where creatures or gameobject are visible (phase can be modified in creature/gameobject table)
- ActiveQuestID, phase will be activated only when the player has this quest but not completed yet.
- QuestObjectiveIndex is the id of quest objective which can be located in quest_objectives table.
- QuestObjectiveAmount is the specified progress of quest objective where the phase change will be activated.
- CompletedQuestID, phase will be activated only when the player completed this quest.
- RewardedQuestID, phase will be activated only when the player turned in this quest and got the reward.
- MapID, phase change will occur only on this map. If you leave it on 0 phase will be changed everywhere.
- ZoneID, phase change will occur only on this zone. If you leave it on 0 phase will be changed everywhere.
- AreaID, phase change will occur only on this area. If you leave it on 0 phase will be changed everywhere.
- Phase Check will happen at the following events:
    OnMovieComplete, OnSceneComplete, OnSceneCancel, OnUpdateArea, OnQuestStatusChange,
    OnPlayerEnterVehicle, OnPlayerExitVehicle, OnPlayerRepop, OnPlayerResurrect, OnLogin, OnGMOff
- Comment, where you can add some info about the phase change.
*/

#include "PhasingHandler.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "ObjectMgr.h"
#include "Config.h"
#include "ZoneScript.h"
#include "Timer.h"
#include "DatabaseEnv.h"

class FluxPhaseMgr_Phasing : public PlayerScript
{
public:
    FluxPhaseMgr_Phasing() : PlayerScript("FluxPhaseMgr_Phasing") { }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // CUSTOM PHASING FUNCTIONS ------------------------------------------------------------------------------------------- >
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
        // shorten the phase add method
        void AddPhase(Player* player, uint32 phaseID)
        {
            if (!player->GetPhaseShift().HasPhase(phaseID))
            {
                PhasingHandler::AddPhase(player, phaseID, true);

                if (sConfigMgr->GetBoolDefault("FluxPhaseMgr.ConsoleDebug", false))
                    TC_LOG_INFO("server.FluxPhaseMgr", "[FluxPhaseMgr]: AddPhase {} for {}.", phaseID, player->GetName());
            }
        }

        // shorten the phase remove method
        void RemovePhase(Player* player, uint32 phaseID)
        {
            if (player->GetPhaseShift().HasPhase(phaseID))
            {
                PhasingHandler::RemovePhase(player, phaseID, true);

                if (sConfigMgr->GetBoolDefault("FluxPhaseMgr.ConsoleDebug", false))
                    TC_LOG_INFO("server.FluxPhaseMgr", "[FluxPhaseMgr]: RemovePhase {} for {}.", phaseID, player->GetName());
            }
        }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // THE ULTIMATE PHASE HANDLING ---------------------------------------------------------------------------------------- >
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void CheckPhase(Player* player, uint32 fluxPhaseID)
        {
            if (sConfigMgr->GetBoolDefault("FluxPhaseMgr.ConsoleDebug", false))
                TC_LOG_DEBUG("server.FluxPhaseMgr", "[FluxPhaseMgr]: Checking Phase fluxPhaseID {} for {}.", fluxPhaseID, player->GetName());

            auto fp = sObjectMgr->GetFluxPhase(fluxPhaseID);

            if (!fp)
                return;

            if (fp->Method == 0)
                return;

            if (sConfigMgr->GetBoolDefault("FluxPhaseMgr.ConsoleDebug", false))
                TC_LOG_DEBUG("server.FluxPhaseMgr", "[FluxPhaseMgr]: fluxPhaseID: {} with PhaseID: {} and Method {} available.", fp->ID, fp->PhaseID, fp->Method);

            // Checking for conditions of phase shifting
            if ((fp->ActiveQuestID != 0 && player->HasQuest(fp->ActiveQuestID))
                || (fp->ActiveQuestID != 0 && fp->QuestObjectiveIndex != 0 && fp->QuestObjectiveAmount != 0 && player->GetQuestObjectiveProgress(fp->ActiveQuestID, fp->QuestObjectiveIndex) != fp->QuestObjectiveAmount)
                || (fp->CompletedQuestID != 0 && player->GetQuestStatus(fp->CompletedQuestID) == QUEST_STATUS_COMPLETE)
                || (fp->RewardedQuestID != 0 && player->GetQuestStatus(fp->RewardedQuestID) == QUEST_STATUS_REWARDED)
                || (fp->MapID != 0 && player->GetMapId() == fp->MapID)
                || (fp->ZoneID != 0 && player->GetZoneId() == fp->ZoneID)
                || (fp->AreaID != 0 && player->GetAreaId() == fp->AreaID))
            {
                switch (fp->Method)
                {
                case 1:
                    AddPhase(player, fp->PhaseID);
                    break;
                case 2:
                    RemovePhase(player, fp->PhaseID);
                    break;
                }
            }
        }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // AFTER MOVIES ---------------------------------------------------------------------------------------------------------------------------------------------- >
    void OnMovieComplete(Player* player, uint32 movieId) override
    {
        if (sConfigMgr->GetBoolDefault("FluxPhaseMgr.Enable", false))
        for (ObjectMgr::FluxPhaseContainer::iterator itr = sObjectMgr->_fluxphaseStore.begin(); itr != sObjectMgr->_fluxphaseStore.end(); itr++)
        {
            auto& fluxphase = itr->second;

            if (fluxphase.OnMovieComplete != 0)
                CheckPhase(player, fluxphase.ID);
        }
    }

    // AFTER SCENES --------------------------------------------------------------------------------------------------------------------------------------------------- >
    void OnSceneComplete(Player* player, uint32 /*sceneInstanceID*/) override
    {
        if (sConfigMgr->GetBoolDefault("FluxPhaseMgr.Enable", false))
        for (ObjectMgr::FluxPhaseContainer::iterator itr = sObjectMgr->_fluxphaseStore.begin(); itr != sObjectMgr->_fluxphaseStore.end(); itr++)
        {
            auto& fluxphase = itr->second;

            if (fluxphase.OnSceneComplete != 0)
                CheckPhase(player, fluxphase.ID);
        }
    }
    void OnSceneCancel(Player* player, uint32 /*sceneInstanceID*/) override
    {
        if (sConfigMgr->GetBoolDefault("FluxPhaseMgr.Enable", false))
        for (ObjectMgr::FluxPhaseContainer::iterator itr = sObjectMgr->_fluxphaseStore.begin(); itr != sObjectMgr->_fluxphaseStore.end(); itr++)
        {
            auto& fluxphase = itr->second;

            if (fluxphase.OnSceneCancel != 0)
                CheckPhase(player, fluxphase.ID);
        }
    }

    // ON AREA CHANGE ------------------------------------------------------------------------------------------------------------------------------------------------ >
    void OnUpdateArea(Player* player, uint32 /*newArea*/, uint32 /*oldArea*/) override
    {
        if (sConfigMgr->GetBoolDefault("FluxPhaseMgr.Enable", false))
        for (ObjectMgr::FluxPhaseContainer::iterator itr = sObjectMgr->_fluxphaseStore.begin(); itr != sObjectMgr->_fluxphaseStore.end(); itr++)
        {
            auto& fluxphase = itr->second;

            if (fluxphase.OnUpdateArea != 0)
                CheckPhase(player, fluxphase.ID);
        }
    }

    // ON QUEST STATUS CHANGE ---------------------------------------------------------------------------------------------------------------------------------------- >
    void OnQuestStatusChange(Player* player, uint32 /*questId*/) override
    {
        if (sConfigMgr->GetBoolDefault("FluxPhaseMgr.Enable", false))
        for (ObjectMgr::FluxPhaseContainer::iterator itr = sObjectMgr->_fluxphaseStore.begin(); itr != sObjectMgr->_fluxphaseStore.end(); itr++)
        {
            auto& fluxphase = itr->second;

            if (fluxphase.OnQuestStatusChange != 0)
                CheckPhase(player, fluxphase.ID);
        }
    }

    // ON ENTER VEHICLE ---------------------------------------------------------------------------------------------------------------------------------------------- >
    void OnPlayerEnterVehicle(Player* player) override
    {
        if (sConfigMgr->GetBoolDefault("FluxPhaseMgr.Enable", false))
        for (ObjectMgr::FluxPhaseContainer::iterator itr = sObjectMgr->_fluxphaseStore.begin(); itr != sObjectMgr->_fluxphaseStore.end(); itr++)
        {
            auto& fluxphase = itr->second;

            if (fluxphase.OnPlayerEnterVehicle != 0)
                CheckPhase(player, fluxphase.ID);
        }
    }

    // ON EXIT VEHICLE ----------------------------------------------------------------------------------------------------------------------------------------------- >
    void OnPlayerExitVehicle(Player* player) override
    {
        if (sConfigMgr->GetBoolDefault("FluxPhaseMgr.Enable", false))
        for (ObjectMgr::FluxPhaseContainer::iterator itr = sObjectMgr->_fluxphaseStore.begin(); itr != sObjectMgr->_fluxphaseStore.end(); itr++)
        {
            auto& fluxphase = itr->second;

            if (fluxphase.OnPlayerEnterVehicle != 0)
                CheckPhase(player, fluxphase.ID);
        }
    }

    // ON SPIRIT HEALER REVIVE --------------------------------------------------------------------------------------------------------------------------------------- >
    void OnPlayerRepop(Player* player) override
    {
        if (sConfigMgr->GetBoolDefault("FluxPhaseMgr.Enable", false))
        for (ObjectMgr::FluxPhaseContainer::iterator itr = sObjectMgr->_fluxphaseStore.begin(); itr != sObjectMgr->_fluxphaseStore.end(); itr++)
        {
            auto& fluxphase = itr->second;

            if (fluxphase.OnPlayerRepop != 0)
                CheckPhase(player, fluxphase.ID);
        }
    }

    // ON RELEASE SPIRIT --------------------------------------------------------------------------------------------------------------------------------------------- >
    void OnPlayerResurrect(Player* player) override
    {
        if (sConfigMgr->GetBoolDefault("FluxPhaseMgr.Enable", false))
        for (ObjectMgr::FluxPhaseContainer::iterator itr = sObjectMgr->_fluxphaseStore.begin(); itr != sObjectMgr->_fluxphaseStore.end(); itr++)
        {
            auto& fluxphase = itr->second;

            if (fluxphase.OnPlayerResurrect != 0)
                CheckPhase(player, fluxphase.ID);
        }
    }

    // ON PLAYER LOGIN ----------------------------------------------------------------------------------------------------------------------------------------------- >
    void OnLogin(Player* player, bool /*firstLogin*/) override
    {
        if (sConfigMgr->GetBoolDefault("FluxPhaseMgr.Enable", false))
        for (ObjectMgr::FluxPhaseContainer::iterator itr = sObjectMgr->_fluxphaseStore.begin(); itr != sObjectMgr->_fluxphaseStore.end(); itr++)
        {
            auto& fluxphase = itr->second;

            if (fluxphase.OnLogin != 0)
                CheckPhase(player, fluxphase.ID);
        }
    }

    // ON PLAYER GM OFF ----------------------------------------------------------------------------------------------------------------------------------------------- >
    void OnPlayerGMOFF(Player* player) override
    {
        if (sConfigMgr->GetBoolDefault("FluxPhaseMgr.Enable", false))
        for (ObjectMgr::FluxPhaseContainer::iterator itr = sObjectMgr->_fluxphaseStore.begin(); itr != sObjectMgr->_fluxphaseStore.end(); itr++)
        {
            auto& fluxphase = itr->second;

            if (fluxphase.OnGMOff != 0)
                CheckPhase(player, fluxphase.ID);
        }
    }

};

void AddSC_Flux_Phase_Manager()
{
    new FluxPhaseMgr_Phasing();
}
