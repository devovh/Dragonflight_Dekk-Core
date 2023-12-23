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

#include "WorldSession.h"
#include "Garrison.h"
#include "GarrisonMgr.h"
#include "GarrisonPackets.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "NPCPackets.h"
#include "WodGarrison.h"
#include "ClassHall.h"
#include "GarrisonAI.h"
#include <AI/ScriptedAI/ScriptedGossip.h>
#include <Quests/WorldQuestMgr.h>


void WorldSession::HandleGetGarrisonInfo(WorldPackets::Garrison::GetGarrisonInfo& /*getGarrisonInfo*/)
{
    _player->SendGarrisonInfo();
}

void WorldSession::HandleGarrisonPurchaseBuilding(WorldPackets::Garrison::GarrisonPurchaseBuilding& garrisonPurchaseBuilding)
{
    if (!_player->GetNPCIfCanInteractWith(garrisonPurchaseBuilding.NpcGUID, UNIT_NPC_FLAG_NONE, UNIT_NPC_FLAG_2_GARRISON_ARCHITECT))
        return;

    if (Garrison* garrison = _player->GetGarrison(GARRISON_TYPE_GARRISON))
        garrison->ToWodGarrison()->PlaceBuilding(garrisonPurchaseBuilding.PlotInstanceID, garrisonPurchaseBuilding.BuildingID);
}

void WorldSession::HandleGarrisonCancelConstruction(WorldPackets::Garrison::GarrisonCancelConstruction& garrisonCancelConstruction)
{
    if (Garrison* garrison = _player->GetGarrison(GARRISON_TYPE_GARRISON))
        garrison->ToWodGarrison()->CancelBuildingConstruction(garrisonCancelConstruction.PlotInstanceID);
}

void WorldSession::HandleGarrisonRequestBlueprintAndSpecializationData(WorldPackets::Garrison::GarrisonRequestBlueprintAndSpecializationData& /*garrisonRequestBlueprintAndSpecializationData*/)
{
    _player->SendGarrisonBlueprintAndSpecializationData();
}

void WorldSession::HandleGarrisonGetMapData(WorldPackets::Garrison::GarrisonGetMapData& /*garrisonGetMapData*/)
{
    if (Garrison* garrison = _player->GetGarrison(GARRISON_TYPE_GARRISON))
        garrison->ToWodGarrison()->SendMapData(_player);
}

void WorldSession::HandleGarrisonCheckUpgradeable(WorldPackets::Garrison::GarrisonCheckUpgradeable& /*garrisonCheckUpgradeable*/)
{
    bool canUpgrade = false;
    if (Garrison* garrison = _player->GetGarrison(GARRISON_TYPE_GARRISON))
        canUpgrade = garrison->ToWodGarrison()->CanUpgrade(false);

    SendPacket(WorldPackets::Garrison::GarrisonCheckUpgradeableResult(canUpgrade).Write());
}

void WorldSession::HandleGarrisonUpgrade(WorldPackets::Garrison::GarrisonUpgrade& garrisonUpgrade)
{
    if (!_player->GetNPCIfCanInteractWith(garrisonUpgrade.NpcGUID, UNIT_NPC_FLAG_NONE, UNIT_NPC_FLAG_2_GARRISON_ARCHITECT))
        return;

    if (Garrison* garrison = _player->GetGarrison(GARRISON_TYPE_GARRISON))
    {
        /*bool result = */garrison->ToWodGarrison()->Upgrade();
        //SendPacket(WorldPackets::Garrison::GarrisonUpgradeResult().Write());
    }
}

void WorldSession::HandleGarrisonOpenMissionNpc(WorldPackets::Garrison::GarrisonOpenMissionNpcRequest& packet)
{
    if (!_player->GetNPCIfCanInteractWith(packet.NpcGUID, UNIT_NPC_FLAG_NONE, UNIT_NPC_FLAG_2_GARRISON_MISSION_NPC))
        return;

    switch (packet.GarrFollowerTypeID)
    {
    case 1://Draenor Garrison
    {
        WorldPackets::Garrison::DeleteExpiredMissionsResult response;
        response.GarrTypeID = 2;
        SendPacket(response.Write());
        break;
    }
    case 2://Draenor Naval
    {
        WorldPackets::Garrison::DeleteExpiredMissionsResult response;
        response.GarrTypeID = 2;
        SendPacket(response.Write());

        break;
    }
    case 4://Class Hall
    {
        WorldPackets::Garrison::DeleteExpiredMissionsResult response;
        response.GarrTypeID = 3;
        response.LegionUnkBit = true;
        SendPacket(response.Write());

        break;
    }
    case 22://BFA
    {
        WorldPackets::Garrison::DeleteExpiredMissionsResult response;
        response.GarrTypeID = 9;
        response.LegionUnkBit = false;
        SendPacket(response.Write());
        break;
    }
    case 123://Shadowland
    {
        WorldPackets::Garrison::DeleteExpiredMissionsResult response;
        response.GarrTypeID = 111;
        response.LegionUnkBit = false;
        SendPacket(response.Write());

        break;
    }
    default:
        break;
    }
    }

    void WorldSession::HandleGarrisonAssignFollowerToBuilding(WorldPackets::Garrison::GarrisonAssignFollowerToBuilding & packet)
    {
        if (Garrison* garrison = _player->GetGarrison(GARRISON_TYPE_GARRISON))
        {
            if (auto follower = garrison->GetFollower(packet.FollowerDBID))
            {
                if (!follower)
                    return;

                // Look up the building based on the provided building ID
              //  WodGarrison::Building* building = _player->GetGarrison(GARRISON_TYPE_GARRISON)->GetBuilding(packet.BuildingID);
               // if (!building)
                //    return;

                 // Assign the follower to the building
                //building->AssignFollower(follower TODO
            }
        }
    }

    void WorldSession::HandleGarrisonCompleteMission(WorldPackets::Garrison::GarrisonCompleteMission & packet)
    {
        if (!_player->GetNPCIfCanInteractWith(packet.NpcGUID, UNIT_NPC_FLAG_NONE, UNIT_NPC_FLAG_2_GARRISON_MISSION_NPC))
            return;

        GarrMissionEntry const* missionEntry = sGarrMissionStore.LookupEntry(packet.MissionRecID);
        if (!missionEntry)
            return;

        Garrison* garrison = _player->GetGarrison(GarrisonType(missionEntry->GarrTypeID));
        if (!garrison)
            return;

        garrison->CompleteMission(packet.MissionRecID);
    }

    void WorldSession::HandleGarrisonGenerateRecruits(WorldPackets::Garrison::GarrisonGenerateRecruits& /*packet*/)
    { }

    void WorldSession::HandleGarrisonMissionBonusRoll(WorldPackets::Garrison::GarrisonMissionBonusRoll & packet)
    {
        if (!_player->GetNPCIfCanInteractWith(packet.NpcGUID, UNIT_NPC_FLAG_NONE, UNIT_NPC_FLAG_2_GARRISON_MISSION_NPC))
            return;

        GarrMissionEntry const* missionEntry = sGarrMissionStore.LookupEntry(packet.MissionRecID);
        if (!missionEntry)
            return;

        Garrison* garrison = _player->GetGarrison(GarrisonType(missionEntry->GarrTypeID));
        if (!garrison)
            return;

        garrison->CalculateMissonBonusRoll(packet.MissionRecID);
    }

    void WorldSession::HandleGarrisonRecruitFollower(WorldPackets::Garrison::GarrisonRecruitFollower & packet)
    {
        // Get the follower ID and quality from the packet
        uint32 followerID = packet.FollowerIndex;
        uint32 quality = packet.quality;

        if (_player == nullptr)
            return;

        Garrison* garrison = _player->GetGarrison(GARRISON_TYPE_GARRISON);

        if (!garrison)
            return;

        WorldPackets::Garrison::GarrisonRecruitFollowerResult result;
        std::unordered_map<uint64 /*dbId*/, Garrison::Follower> followers = garrison->GetFollowers();

        if (Creature* unit = _player->GetNPCIfCanInteractWith(packet.Guid, UNIT_NPC_FLAG_NONE, UNIT_NPC_FLAG_2_NONE))
        {
            result.Result = uint32(GarrisonError::GARRISON_SUCCESS);

            for (auto& follower : followers)
            {
                if (follower.second.PacketInfo.GarrFollowerID == packet.FollowerIndex)
                {
                    //result.Follower.push_back(follower.second.PacketInfo);
                    garrison->AddFollower(packet.FollowerIndex);
                    //l_Garrison->SetCanRecruitFollower(false);
                    //m_Player->SetCharacterWorldState(CharacterWorldStates::GarrisonTavernBoolCanRecruitFollower, 0);
                    break;
                }
            }
            SendPacket(result.Write());
        }
    }

    void WorldSession::HandleGarrisonRemoveFollower(WorldPackets::Garrison::GarrisonRemoveFollower & packet)
    {
        // Get the follower ID from the packet
        uint64 followerID = packet.FollowerDBID;

        // Remove the follower from the player's garrison
      //  _player->RemoveFollower(followerID);
    }

    void WorldSession::HandleGarrisonRemoveFollowerFromBuilding(WorldPackets::Garrison::GarrisonRemoveFollowerFromBuilding& /*packet*/)
    { }

    void WorldSession::HandleGarrisonRenameFollower(WorldPackets::Garrison::GarrisonRenameFollower & packet)
    {
        // Get the follower ID and new name from the packet
        uint64 followerID = packet.FollowerDBID;
        std::string newName = packet.FollowerName;

        // Rename the follower for the player
        //_player->RenameFollower(followerID, newName);
    }

    void WorldSession::HandleGarrisonRequestShipmentInfo(WorldPackets::Garrison::GarrisonRequestShipmentInfo & packet)
    {

        if (!_player->GetNPCIfCanInteractWith(packet.NpcGUID, UNIT_NPC_FLAG_NONE, UNIT_NPC_FLAG_2_SHIPMENT_CRAFTER))
            return;

        if (Garrison* garrison = _player->GetGarrison(_player->GetCurrentGarrison()))
            garrison->SendShipmentInfo(packet.NpcGUID);
    }

    void WorldSession::HandleGarrisonResearchTalent(WorldPackets::Garrison::GarrisonRequestResearchTalent & packet)
    {
        // Check if the talent ID is valid
        TalentEntry const* talent = sTalentStore.LookupEntry(packet.GarrTalentID);
        if (!talent)
        {
            // Send an error message to the player
            return;
        }

        if (Garrison* garrison = _player->GetGarrison(GARRISON_TYPE_GARRISON))
            garrison->StartClassHallUpgrade(packet.GarrTalentID);
        CloseGossipMenuFor(_player);

        //_player->ResearchGarrisonTalent(talent);
    }

    void WorldSession::HandleGarrisonSetFollowerInactive(WorldPackets::Garrison::GarrisonSetFollowerInactive & packet)
    {
        if (!_player->HasEnoughMoney(int64(2500000)))
            return;

        if (Garrison* garrison = _player->GetGarrison(GARRISON_TYPE_GARRISON))
        {
            if (auto follower = garrison->GetFollower(packet.FollowerDBID))
            {
                if (packet.Inactive)
                {
                    if (follower->PacketInfo.FollowerStatus & GarrisonFollowerStatus::FOLLOWER_STATUS_INACTIVE)
                        return;

                    follower->PacketInfo.FollowerStatus |= GarrisonFollowerStatus::FOLLOWER_STATUS_INACTIVE;

                    //    WorldPackets::Garrison::GarrisonFollowerChangeStatus packetStatus;
                       // packetStatus.Follower = follower->PacketInfo;
                       // packetStatus.Result = 0;
                      //  _player->SendDirectMessage(packetStatus.Write());

                    WorldPackets::Garrison::GarrisonRemoveFollowerFromBuildingResult packetResult;
                    packetResult.FollowerDBID = packet.FollowerDBID;
                    packetResult.Result = 0;
                    _player->SendDirectMessage(packetResult.Write());
                }
                else
                {
                    if (!(follower->PacketInfo.FollowerStatus & GarrisonFollowerStatus::FOLLOWER_STATUS_INACTIVE))
                        return;

                    follower->PacketInfo.FollowerStatus &= ~GarrisonFollowerStatus::FOLLOWER_STATUS_INACTIVE;

                    //     WorldPackets::Garrison::GarrisonFollowerChangeStatus packetStatus;
                      //   packetStatus.Follower = follower->PacketInfo;
                    //     packetStatus.Result = 0;
                       //  _player->SendDirectMessage(packetStatus.Write());

                    WorldPackets::Garrison::GarrisonFollowerActivationSet packetResult;
                    packetResult.NumActivations;
                    packetResult.GarrSiteID;
                    _player->SendDirectMessage(packetResult.Write());
                }
                //  follower->DbState = DB_STATE_CHANGED;
                _player->ModifyMoney(-int64(2500000));
            }
        }
    }

    void WorldSession::HandleGarrisonSetRecruitmentPreferences(WorldPackets::Garrison::GarrisonSetRecruitmentPreferences& /*packet*/)
    { }

    void WorldSession::HandleGarrisonStartMission(WorldPackets::Garrison::GarrisonStartMission & packet)
    {
        if (!_player->GetNPCIfCanInteractWith(packet.NpcGUID, UNIT_NPC_FLAG_NONE, UNIT_NPC_FLAG_2_GARRISON_MISSION_NPC /* | UNIT_NPC_FLAG_2_SHIPMENT_CRAFTER*/))
            return;

        GarrMissionEntry const* missionEntry = sGarrMissionStore.LookupEntry(packet.MissionRecID);
        if (!missionEntry)
            return;

        Garrison* garrison = _player->GetGarrison(GarrisonType(missionEntry->GarrTypeID));
        if (!garrison)
            return;

        garrison->StartMission(packet.MissionRecID, packet.FollowerDBIDs);
    }

    void WorldSession::HandleGarrisonSwapBuildings(WorldPackets::Garrison::GarrisonSwapBuildings & packet)
    {
        //if (Garrison* garrison = _player->GetGarrison(GARRISON_TYPE_GARRISON))
          //  garrison->Swap(packet.PlotId1, packet.PlotId2);
    }

    void WorldSession::HandleGetLandingPageShipmentsOpcode(WorldPackets::Garrison::GarrisonGetLandingPageShipments&)
    {
        if (!_player)
            return;

        Garrison* garrison = _player->GetGarrison(GarrisonType(GarrisonType::GARRISON_TYPE_CLASS_HALL));
        if (!garrison)
            return;
        // TODO rewrite
     //   WorldPackets::Garrison::GarrisonGetLandingPageShipmentsResponse packet;
      //  packet.Result = GarrisonType::GARRISON_TYPE_CLASS_HALL;
     //   _player->SendDirectMessage(packet.Write());
    }

    void WorldSession::HandleRequestClassSpecCategoryInfo(WorldPackets::Garrison::GarrisonRequestClassSpecCategoryInfo & packet)
    {
        WorldPackets::Garrison::GarrisonResponseClassSpecCategoryInfo resp;
        resp.GarrFollowerTypeID = packet.ClassID;

        /*WorldPackets::Garrison::GarrisonResponseClassSpecCategoryInfo::FollowersClassSpecStruct& d1 = resp.Datas.back();
        d1.Category = 99;
        d1.Option = 43;
        resp.Datas.emplace_back();
        WorldPackets::Garrison::GarrisonResponseClassSpecCategoryInfo::FollowersClassSpecStruct &d2 = resp.Datas.back();
        d2.Category = 100;
        d2.Option = 44; */

        SendPacket(resp.Write());
    }

    void WorldSession::HandleRequestGarrissonTalentsWQUnlocks(WorldPackets::Garrison::GarrisonRequestGarrisonTalentWorldQuestUnlocks & packet)
    {
        if (!GetPlayer())
            return;

        uint32 questId = 0;

        ActiveWorldQuest const* activeWorldQuest = sWorldQuestMgr->GetActiveWorldQuest(questId);
        WorldPackets::Garrison::GarrisonTalentWorldQuestUnlocksResponse response;
        response.UNK1;
        response.State;
        response.QuestID1;
        response.UNK2;
        response.QuestID2;
        response.MapID;
        response.UNK3;
        response.UNK4;
        response.UNK5;
        response.UNK8;
        response.UNK6;
        response.UNK7;
        response.UNK9;

        SendPacket(response.Write());
    }

    void WorldSession::HandleTrophyData(WorldPackets::Garrison::TrophyData & packet)
    {
        switch (packet.GetOpcode())
        {
        case CMSG_REPLACE_TROPHY:
            break;
        case CMSG_CHANGE_MONUMENT_APPEARANCE:
            break;
        default:
            break;
        }
    }

    void WorldSession::HandleCreateShipment(WorldPackets::Garrison::CreateShipment & packet)
    {
        if (!_player->GetNPCIfCanInteractWith(packet.NpcGUID, UNIT_NPC_FLAG_NONE, UNIT_NPC_FLAG_2_STEERING))
            return;

        if (Garrison* garrison = _player->GetGarrison(GARRISON_TYPE_GARRISON))
            garrison->CreateShipment(packet.NpcGUID, packet.Count);
    }

    void WorldSession::HandleGetTrophyList(WorldPackets::Garrison::GetTrophyList & packet)
    {
    }

    void WorldSession::HandleGarrisonGetMissionReward(WorldPackets::Garrison::GarrisonGetMissionReward& /*packet*/)
    {
    }

    void WorldSession::HandleGarrisonSetBuildingActive(WorldPackets::Garrison::GarrisonSetBuildingActive & packet)
    {

        // Get the building on the specified plot
       // WodGarrison::Plot plots = _player->GetGarrison(GARRISON_TYPE_GARRISON)->GetPlots();
       // auto itr = plots.find(packet.plotInstanceID);
      //  if (itr == plots.end())
       // {
            // Send an error message to the player
         //   return;
        //}

        // Set the active state of the building
       // itr->second.BuildingInfo.Active = packet.active;
    }

    void WorldSession::HandleGarrisonSetFollowerFavorite(WorldPackets::Garrison::GarrisonSetFollowerFavorite& /*packet*/)
    {

    }

    void WorldSession::HandleGarrisonAddFollowerHealth(WorldPackets::Garrison::GarrisonAddFollowerHealth & packet)
    {
        if (!GetPlayer())
            return;

        Player* player = nullptr;

        // Look up the follower based on the provided database ID
        Garrison::Follower* follower = player->GetGarrison(GARRISON_TYPE_MAX)->GetFollower(packet.FollowerDBID);

        if (!follower)
            return;

        // Restore the follower's health
     //   follower->SetHealth(follower->GetHealth() + packet.Health); TODO
    }

    void WorldSession::HandleGarrisonFullyHealAllFollowers(WorldPackets::Garrison::GarrisonFullyHealAllFollowers& /*packet*/)
    { }

    void WorldSession::HandleGarrisonLearnTalent(WorldPackets::Garrison::GarrisonLearnTalent& /*packet*/)
    { }

    void WorldSession::HandleGarrisonOpenshipmentnpc(WorldPackets::Garrison::GarrisonOpenshipmentnpc& /*packet*/)
    {
    }

    void WorldSession::HandleGarrisonSocketTalent(WorldPackets::Garrison::GarrisonSocketTalent&)
    {
    }
