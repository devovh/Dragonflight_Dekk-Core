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

#ifndef GarrisonPackets_h__
#define GarrisonPackets_h__

#include "Packet.h"
#include "ItemPacketsCommon.h"
#include "ObjectGuid.h"
#include "Optional.h"
#include "Position.h"
#include "PacketUtilities.h"
#include <list>
#include <unordered_set>
#include <vector>

struct GarrAbilityEntry;

namespace WorldPackets
{
    namespace Garrison
    {
        class GarrisonCreateResult final : public ServerPacket
        {
        public:
            GarrisonCreateResult() : ServerPacket(SMSG_GARRISON_CREATE_RESULT, 4 + 4) { }

            WorldPacket const* Write() override;

            uint32 GarrSiteLevelID = 0;
            uint32 Result = 0;
        };

        class GarrisonDeleteResult final : public ServerPacket
        {
        public:
            GarrisonDeleteResult() : ServerPacket(SMSG_GARRISON_DELETE_RESULT, 4 + 4) { }

            WorldPacket const* Write() override;

            uint32 Result = 0;
            uint32 GarrSiteID = 0;
        };

        class GetGarrisonInfo final : public ClientPacket
        {
        public:
            GetGarrisonInfo(WorldPacket&& packet) : ClientPacket(CMSG_GET_GARRISON_INFO, std::move(packet)) { }

            void Read() override { }
        };

        struct GarrisonPlotInfo
        {
            uint32 GarrPlotInstanceID = 0;
            TaggedPosition<Position::XYZO> PlotPos;
            uint32 PlotType = 0;
        };

        struct GarrisonBuildingInfo
        {
            uint32 GarrPlotInstanceID = 0;
            uint32 GarrBuildingID = 0;
            uint64 TimeBuilt;
            uint32 CurrentGarSpecID = 0;
            uint64 TimeSpecCooldown = time_t(2288912640);   // 06/07/1906 18:35:44 - another in the series of magic blizz dates
            bool Active = false;
        };

        struct GarrisonFollower
        {
            uint64 DbID = 0;
            uint32 GarrFollowerID = 0;
            uint32 Quality = 0;
            uint32 FollowerLevel = 0;
            uint32 ItemLevelWeapon = 0;
            uint32 ItemLevelArmor = 0;
            uint32 Xp = 0;
            uint32 Durability = 0;
            uint32 CurrentBuildingID = 0;
            uint32 CurrentMissionID = 0;
            std::list<GarrAbilityEntry const*> AbilityID;
            uint32 ZoneSupportSpellID = 0;
            uint32 FollowerStatus = 0;
            int32 Health = 0;
            Timestamp<> HealingTimestamp;
            int8 BoardIndex = 0;
            std::string CustomName;
        };

        struct GarrisonEncounter
        {
            int32 GarrEncounterID = 0;
            std::vector<int32> Mechanics;
            uint32 MechanicCount;
            int32 GarrAutoCombatantID = 0;
            int32 Health = 0;
            int32 MaxHealth = 0;
            int32 Attack = 0;
            int8 BoardIndex = 0;
        };

        struct GarrisonMissionReward
        {
            int32 ItemID = 0;
            uint32 ItemQuantity = 0;
            int32 CurrencyID = 0;
            uint32 CurrencyQuantity = 0;
            uint32 FollowerXP = 0;
            uint32 GarrMssnBonusAbilityID = 0;
            int32 ItemFileDataID = 0;
            Optional<Item::ItemInstance> ItemInstance;
        };

        struct GarrisonMission
        {
            uint64 DbID = 0;
            int32 MissionRecID = 0;
            Timestamp<> OfferTime;
            Duration<Seconds> OfferDuration;
            Timestamp<> StartTime = time_t(2288912640);
            Duration<Seconds> TravelDuration;
            Duration<Seconds> MissionDuration;
            int32 MissionState = 0;
            int32 SuccessChance = 0;
            uint32 Flags = 0;
            float MissionScalar = 1.0f;
            int32 ContentTuningID = 0;
            std::vector<GarrisonEncounter> Encounters;
            std::vector<GarrisonMissionReward> Rewards;
            std::vector<GarrisonMissionReward> OvermaxRewards;
        };

        struct GarrisonMissionBonusAbility
        {
            uint32 GarrMssnBonusAbilityID = 0;
            Timestamp<> StartTime;
        };

        struct GarrisonTalentSocketData
        {
            int32 SoulbindConduitID = 0;
            int32 SoulbindConduitRank = 0;
        };

        struct GarrisonTalent
        {
            int32 GarrTalentID = 0;
            int32 Rank = 0;
            Timestamp<> ResearchStartTime;
            int32 Flags = 0;
            Optional<GarrisonTalentSocketData> Socket;
        };

        struct GarrisonCollectionEntry
        {
            int32 EntryID = 0;
            int32 Rank = 0;
        };

        struct GarrisonCollection
        {
            int32 Type = 0;
            std::vector<GarrisonCollectionEntry> Entries;
        };

        struct GarrisonEventEntry
        {
            int32 EntryID = 0;
            int64 EventValue = 0;
        };

        struct GarrisonEventList
        {
            int32 Type = 0;
            std::vector<GarrisonEventEntry> Events;
        };

        struct GarrisonSpecGroup
        {
            int32 ChrSpecializationID = 0;
            int32 SoulbindID = 0;
        };

        struct GarrisonInfo
        {
            int32 GarrTypeID = 0;
            uint32 GarrSiteID = 0;
            uint32 GarrSiteLevelID = 0;
            uint32 NumFollowerActivationsRemaining = 0;
            uint32 NumMissionsStartedToday = 0;   // might mean something else, but sending 0 here enables follower abilities "Increase success chance of the first mission of the day by %."
            int32 MinAutoTroopLevel = 0;
            std::vector<GarrisonPlotInfo*> Plots;
            std::vector<GarrisonBuildingInfo const*> Buildings;
            std::vector<GarrisonFollower const*> Followers;
            std::vector<GarrisonFollower const*> AutoTroops;
            std::vector<GarrisonMission const*> Missions;
            std::vector<std::vector<GarrisonMissionReward>> MissionRewards;
            std::vector<std::vector<GarrisonMissionReward>> MissionOvermaxRewards;
            std::vector<GarrisonMissionBonusAbility const*> MissionAreaBonuses;
            std::vector<GarrisonTalent> Talents;
            std::vector<GarrisonCollection> Collections;
            std::vector<GarrisonEventList> EventLists;
            std::vector<GarrisonSpecGroup> SpecGroups;
            std::vector<bool> CanStartMission;
            std::vector<int32> ArchivedMissions;
        };

        struct FollowerSoftCapInfo
        {
            int32 GarrFollowerTypeID;
            uint32 Count;
        };

        class GetGarrisonInfoResult final : public ServerPacket
        {
        public:
            GetGarrisonInfoResult() : ServerPacket(SMSG_GET_GARRISON_INFO_RESULT) { }

            WorldPacket const* Write() override;

            uint32 FactionIndex = 0;
            std::vector<GarrisonInfo> Garrisons;
            std::vector<FollowerSoftCapInfo> FollowerSoftCaps;
        };

        struct GarrisonRemoteBuildingInfo
        {
            GarrisonRemoteBuildingInfo() : GarrPlotInstanceID(0), GarrBuildingID(0) { }
            GarrisonRemoteBuildingInfo(uint32 plotInstanceId, uint32 buildingId) : GarrPlotInstanceID(plotInstanceId), GarrBuildingID(buildingId) { }

            uint32 GarrPlotInstanceID;
            uint32 GarrBuildingID;
        };

        struct GarrisonRemoteSiteInfo
        {
            uint32 GarrSiteLevelID = 0;
            std::vector<GarrisonRemoteBuildingInfo> Buildings;
        };

        enum GarrisonTalentFlag : int32
        {
            Researched = 0x1,
            Respec = 0x2,
            Researching = 0x4,
            Ongoing = 0x8,
            DontReset = 0x10,
        };

        class GarrisonRemoteInfo final : public ServerPacket
        {
        public:
            GarrisonRemoteInfo() : ServerPacket(SMSG_GARRISON_REMOTE_INFO) { }

            WorldPacket const* Write() override;

            std::vector<GarrisonRemoteSiteInfo> Sites;
        };

        class GarrisonPurchaseBuilding final : public ClientPacket
        {
        public:
            GarrisonPurchaseBuilding(WorldPacket&& packet) : ClientPacket(CMSG_GARRISON_PURCHASE_BUILDING, std::move(packet)) { }

            void Read() override;

            ObjectGuid NpcGUID;
            uint32 BuildingID = 0;
            uint32 PlotInstanceID = 0;
        };

        class GarrisonPlaceBuildingResult final : public ServerPacket
        {
        public:
            GarrisonPlaceBuildingResult() : ServerPacket(SMSG_GARRISON_PLACE_BUILDING_RESULT) { }

            WorldPacket const* Write() override;

            uint32 GarrTypeID = 0;
            uint32 Result = 0;
            GarrisonBuildingInfo BuildingInfo;
            bool PlayActivationCinematic = true;
        };

        class GarrisonCancelConstruction final : public ClientPacket
        {
        public:
            GarrisonCancelConstruction(WorldPacket&& packet) : ClientPacket(CMSG_GARRISON_CANCEL_CONSTRUCTION, std::move(packet)) { }

            void Read() override;

            uint32 PlotInstanceID = 0;
        };

        class GarrisonBuildingRemoved final : public ServerPacket
        {
        public:
            GarrisonBuildingRemoved() : ServerPacket(SMSG_GARRISON_BUILDING_REMOVED) { }

            WorldPacket const* Write() override;

            uint32 GarrBuildingID = 0;
        };

        class GarrisonLearnBlueprintResult final : public ServerPacket
        {
        public:
            GarrisonLearnBlueprintResult() : ServerPacket(SMSG_GARRISON_LEARN_BLUEPRINT_RESULT, 4 + 4 + 4) { }

            WorldPacket const* Write() override;

            int32 GarrTypeID = 0;
            uint32 BuildingID = 0;
            uint32 Result = 0;
        };

        class GarrisonUnlearnBlueprintResult final : public ServerPacket
        {
        public:
            GarrisonUnlearnBlueprintResult() : ServerPacket(SMSG_GARRISON_UNLEARN_BLUEPRINT_RESULT, 4 + 4 + 4) { }

            WorldPacket const* Write() override;

            int32 GarrTypeID = 0;
            uint32 BuildingID = 0;
            uint32 Result = 0;
        };

        class GarrisonRequestBlueprintAndSpecializationData final : public ClientPacket
        {
        public:
            GarrisonRequestBlueprintAndSpecializationData(WorldPacket&& packet) : ClientPacket(CMSG_GARRISON_REQUEST_BLUEPRINT_AND_SPECIALIZATION_DATA, std::move(packet)) { }

            void Read() override { }
        };

        class GarrisonRequestBlueprintAndSpecializationDataResult final : public ServerPacket
        {
        public:
            GarrisonRequestBlueprintAndSpecializationDataResult() : ServerPacket(SMSG_GARRISON_REQUEST_BLUEPRINT_AND_SPECIALIZATION_DATA_RESULT, 400) { }

            WorldPacket const* Write() override;

            uint32 GarrTypeID = 0;
            std::unordered_set<uint32> const* SpecializationsKnown = nullptr;
            std::unordered_set<uint32> const* BlueprintsKnown = nullptr;
        };

        class GarrisonGetMapData final : public ClientPacket
        {
        public:
            GarrisonGetMapData(WorldPacket&& packet) : ClientPacket(CMSG_GARRISON_GET_MAP_DATA, std::move(packet)) { }

            void Read() override { }
        };

        struct GarrisonBuildingMapData
        {
            GarrisonBuildingMapData() : GarrBuildingPlotInstID(0) { }
            GarrisonBuildingMapData(uint32 buildingPlotInstId, Position const& pos) : GarrBuildingPlotInstID(buildingPlotInstId), Pos(pos) { }

            uint32 GarrBuildingPlotInstID;
            TaggedPosition<Position::XYZ> Pos;
        };

        class GarrisonMapDataResponse final : public ServerPacket
        {
        public:
            GarrisonMapDataResponse() : ServerPacket(SMSG_GARRISON_MAP_DATA_RESPONSE) { }

            WorldPacket const* Write() override;

            std::vector<GarrisonBuildingMapData> Buildings;

        };

        class GarrisonPlotPlaced final : public ServerPacket
        {
        public:
            GarrisonPlotPlaced() : ServerPacket(SMSG_GARRISON_PLOT_PLACED) { }

            WorldPacket const* Write() override;

            uint32 GarrTypeID = 0;
            GarrisonPlotInfo* PlotInfo = nullptr; 
        };

        class GarrisonPlotRemoved final : public ServerPacket
        {
        public:
            GarrisonPlotRemoved() : ServerPacket(SMSG_GARRISON_PLOT_REMOVED, 4) { }

            WorldPacket const* Write() override;

            uint32 GarrPlotInstanceID = 0;
        };

        class GarrisonAddFollowerResult final : public ServerPacket
        {
        public:
            GarrisonAddFollowerResult() : ServerPacket(SMSG_GARRISON_ADD_FOLLOWER_RESULT) { }

            WorldPacket const* Write() override;

            uint32 GarrTypeID = 0;
            GarrisonFollower Follower;
            uint32 Result = 0;
        };

        class GarrisonRemoveFollowerResult final : public ServerPacket
        {
        public:
            GarrisonRemoveFollowerResult() : ServerPacket(SMSG_GARRISON_REMOVE_FOLLOWER_RESULT, 8 + 4 + 4 + 4) { }

            WorldPacket const* Write() override;

            uint64 FollowerDBID = 0;
            int32 GarrTypeID = 0;
            uint32 Result = 0;
            uint32 Destroyed = 0;
        };

        class GarrisonBuildingActivated final : public ServerPacket
        {
        public:
            GarrisonBuildingActivated() : ServerPacket(SMSG_GARRISON_BUILDING_ACTIVATED, 4) { }

            WorldPacket const* Write() override;

            uint32 GarrPlotInstanceID = 0;
        };

        class GarrisonAddMissionResult final : public ServerPacket
        {
        public:
            GarrisonAddMissionResult() : ServerPacket(SMSG_GARRISON_ADD_MISSION_RESULT) { }

            int32 GarrType = 0;
            int32 Result = 0;
            uint8 State = 0;
            GarrisonMission Mission;

            std::vector<GarrisonMissionReward> Rewards;
            std::vector<GarrisonMissionReward> BonusRewards;

            bool Success = true;
            bool CanStart = true;

            WorldPacket const* Write() override;
        };

        class GarrisonAssignFollowerToBuildingResult final : public ServerPacket
        {
        public:
            GarrisonAssignFollowerToBuildingResult() : ServerPacket(SMSG_GARRISON_ASSIGN_FOLLOWER_TO_BUILDING_RESULT, 8 + 4 + 4) { }

                WorldPacket const* Write() override;

            uint64 FollowerDBID = 0;
            int32 Result = 0;
            int32 PlotInstanceID = 0;
        };

        class GarrisonCompleteMissionResult final : public ServerPacket
        {
        public:
            GarrisonCompleteMissionResult() : ServerPacket(SMSG_GARRISON_COMPLETE_MISSION_RESULT, 36 + 4 + 4 + 1) { }

            WorldPacket const* Write() override;

            uint32 Result = 0;
            GarrisonMission MissionData;
            uint32 MissionRecID = 0;
            bool Succeeded = false;
        };

        class GarrisonFollowerActivationSet final : public ServerPacket
        {
        public:
            GarrisonFollowerActivationSet() : ServerPacket(SMSG_GARRISON_FOLLOWER_ACTIVATIONS_SET) { }

            WorldPacket const* Write() override;

            uint32 GarrSiteID;
            uint32 NumActivations = 2;
        };

        class GarrisonCheckUpgradeableResult final : public ServerPacket
        {
        public:
            GarrisonCheckUpgradeableResult(bool upgradeable = false) : ServerPacket(SMSG_GARRISON_IS_UPGRADEABLE_RESPONSE, 4), IsUpgradeable(upgradeable) { }

            WorldPacket const* Write() override;

            bool IsUpgradeable = false;
        };

        class GarrisonRemoveFollowerFromBuildingResult final : public ServerPacket
        {
        public:
            GarrisonRemoveFollowerFromBuildingResult() : ServerPacket(SMSG_GARRISON_REMOVE_FOLLOWER_FROM_BUILDING_RESULT, 12) { }

            WorldPacket const* Write() override;

            uint64 FollowerDBID = 0;
            int32 Result = 0;
        };

        class GarrisonStartMissionResult final : public ServerPacket
        {
        public:
            GarrisonStartMissionResult() : ServerPacket(SMSG_GARRISON_START_MISSION_RESULT) { }

            uint32 Result;
            uint16 FailReason = 1;
            GarrisonMission Mission;
            std::vector<uint64 /* dbID */> Followers;

            WorldPacket const* Write() override;
        };

        class GarrisonCheckUpgradeable final : public ClientPacket
        {
        public:
            GarrisonCheckUpgradeable(WorldPacket&& packet) : ClientPacket(CMSG_GARRISON_CHECK_UPGRADEABLE, std::move(packet)) { }

            int32 GarrSiteID;

            void Read() override { }
        };

        class GarrisonUpgrade final : public ClientPacket
        {
        public:
            GarrisonUpgrade(WorldPacket&& packet) : ClientPacket(CMSG_UPGRADE_GARRISON, std::move(packet)) { }

            void Read() override;

            ObjectGuid NpcGUID;
        };

        class GarrisonUpgradeResult final : public ServerPacket
        {
        public:
            GarrisonUpgradeResult() : ServerPacket(SMSG_GARRISON_UPGRADE_RESULT, 4 + 4) { }

            WorldPacket const* Write() override;

            uint32 GarrSiteLevelID = 0;
            uint32 Result = 0;
        };

        class CreateShipmentResponse final : public ServerPacket
        {
        public:
            CreateShipmentResponse() : ServerPacket(SMSG_CREATE_SHIPMENT_RESPONSE, 8 + 4 + 4) { }

            WorldPacket const* Write() override;

            uint64 ShipmentID = 0;
            uint32 ShipmentRecID = 0;
            uint32 Result = 0;
        };

        class GarrisonFollowerChangedItemLevel final : public ServerPacket
        {
        public:
            GarrisonFollowerChangedItemLevel() : ServerPacket(SMSG_GARRISON_FOLLOWER_CHANGED_ITEM_LEVEL, sizeof(GarrisonFollower)) { }

            WorldPacket const* Write() override;

            GarrisonFollower Follower;
        };

        class GarrisonFollowerChangedXP final : public ServerPacket
        {
        public:
            GarrisonFollowerChangedXP() : ServerPacket(SMSG_GARRISON_FOLLOWER_CHANGED_XP, sizeof(GarrisonFollower) * 2 + 8) { }

            WorldPacket const* Write() override;

            GarrisonFollower Follower;
            GarrisonFollower Follower2;
            int32 Result = 0;
            uint32 TotalXp = 0;
        };

        class GarrisonMissionBonusRollResult final : public ServerPacket
        {
        public:
            GarrisonMissionBonusRollResult() : ServerPacket(SMSG_GARRISON_MISSION_BONUS_ROLL_RESULT, sizeof(GarrisonMission) + 4 + 4) { }

            WorldPacket const* Write() override;

            GarrisonMission MissionData;
            uint32 MissionRecID = 0;
            uint32 Result = 0;
        };

        class GarrisonOpenMissionNpcRequest final : public ClientPacket
        {
        public:
            GarrisonOpenMissionNpcRequest(WorldPacket&& packet) : ClientPacket(CMSG_OPEN_MISSION_NPC, std::move(packet)) { }

            void Read() override;

            ObjectGuid NpcGUID;
            uint32 GarrFollowerTypeID = 0;
        };

        class GarrisonOpenRecruitmentNpc final : public ServerPacket
        {
        public:
            GarrisonOpenRecruitmentNpc() : ServerPacket(SMSG_GARRISON_OPEN_RECRUITMENT_NPC, sizeof(GarrisonFollower) * 3 + 16 + 8 + 2) { }

            WorldPacket const* Write() override;

            std::vector<GarrisonFollower> Followers;
            ObjectGuid Guid;
            uint32 GarrTypeID = 0;
            uint32 Result = 0;
            bool CanRecruitFollower = false;
            bool UnkBit = false;
        };

        class GarrisonRecruitFollowerResult final : public ServerPacket
        {
        public:
            GarrisonRecruitFollowerResult() : ServerPacket(SMSG_GARRISON_RECRUIT_FOLLOWER_RESULT, 36 + 4 + 4) { }

            WorldPacket const* Write() override;

            GarrisonFollower Follower;
            int32 Result = 0;
        };

        class GarrisonResearchTalent final : public ServerPacket
        {
        public:
            GarrisonResearchTalent() : ServerPacket(SMSG_GARRISON_RESEARCH_TALENT_RESULT, 20) { }

            WorldPacket const* Write() override;

            int32 Result = 0;  // if > 0 entire packet is unhandled
            int32 GarrTypeID = 0;
            int32 TalentID = 0;
            uint32 ResearchTime = time_t(0);;
            uint32 Flags = 0;
            int32 Rank;
            int32 GarrTalentTreeID;
            uint8 State;
            bool Socket = false;
            bool IsTemporary = false;
        };

        class GarrisonAssignFollowerToBuilding final : public ClientPacket
        {
        public:
            GarrisonAssignFollowerToBuilding(WorldPacket&& packet) : ClientPacket(CMSG_GARRISON_ASSIGN_FOLLOWER_TO_BUILDING, std::move(packet)) { }

            void Read() override;

            ObjectGuid NpcGUID;
            int64 FollowerDBID = 0;
            uint32 BuildingID = 0;
        };

        class GarrisonCompleteMission final : public ClientPacket
        {
        public:
            GarrisonCompleteMission(WorldPacket&& packet) : ClientPacket(CMSG_GARRISON_COMPLETE_MISSION, std::move(packet)) { }

            void Read() override;

            ObjectGuid NpcGUID;
            uint32 MissionRecID = 0;
        };

        class GarrisonGenerateRecruits final : public ClientPacket
        {
        public:
            GarrisonGenerateRecruits(WorldPacket&& packet) : ClientPacket(CMSG_GARRISON_GENERATE_RECRUITS, std::move(packet)) { }

            void Read() override;

            ObjectGuid NpcGuid;
            uint32 MechanicTypeID = 0;
            uint32 TraitID = 0;
        };

        class GarrisonMissionBonusRoll final : public ClientPacket
        {
        public:
            GarrisonMissionBonusRoll(WorldPacket&& packet) : ClientPacket(CMSG_GARRISON_MISSION_BONUS_ROLL, std::move(packet)) { }

            void Read() override;

            ObjectGuid NpcGUID;
            uint32 MissionRecID = 0;
        };

        class GarrisonRecruitFollower final : public ClientPacket
        {
        public:
            GarrisonRecruitFollower(WorldPacket&& packet) : ClientPacket(CMSG_GARRISON_RECRUIT_FOLLOWER, std::move(packet)) { }

            void Read() override;

            ObjectGuid Guid;
            int32 FollowerIndex = 0;
            uint32 quality;
        };

        class GarrisonRemoveFollower final : public ClientPacket
        {
        public:
            GarrisonRemoveFollower(WorldPacket&& packet) : ClientPacket(CMSG_GARRISON_REMOVE_FOLLOWER, std::move(packet)) { }

            void Read() override;

            ObjectGuid Guid;
            uint64 FollowerDBID = 0;
        };

        class GarrisonRemoveFollowerFromBuilding final : public ClientPacket
        {
        public:
            GarrisonRemoveFollowerFromBuilding(WorldPacket&& packet) : ClientPacket(CMSG_GARRISON_REMOVE_FOLLOWER_FROM_BUILDING, std::move(packet)) { }

            void Read() override;

            ObjectGuid NpcGUID;
            uint64 FollowerDBID = 0;
        };

        class GarrisonRenameFollower final : public ClientPacket
        {
        public:
            GarrisonRenameFollower(WorldPacket&& packet) : ClientPacket(CMSG_GARRISON_RENAME_FOLLOWER, std::move(packet)) { }

            void Read() override;

            uint64 FollowerDBID = 0;
            std::string FollowerName;
        };

        class GarrisonRequestShipmentInfo final : public ClientPacket
        {
        public:
            GarrisonRequestShipmentInfo(WorldPacket&& packet) : ClientPacket(CMSG_GARRISON_REQUEST_SHIPMENT_INFO, std::move(packet)) { }

            void Read() override;

            ObjectGuid NpcGUID;
        };

        class GarrisonRequestResearchTalent final : public ClientPacket
        {
        public:
            GarrisonRequestResearchTalent(WorldPacket&& packet) : ClientPacket(CMSG_GARRISON_RESEARCH_TALENT, std::move(packet)) { }

            void Read() override;

            ObjectGuid NPCTalentGuid;
            uint32 GarrTalentID = 0;
            std::vector<GarrisonTalentFlag> GarrisonTalentFlag;
            bool IsTemporary;
        };

        class GarrisonSetFollowerInactive final : public ClientPacket
        {
        public:
            GarrisonSetFollowerInactive(WorldPacket&& packet) : ClientPacket(CMSG_GARRISON_SET_FOLLOWER_INACTIVE, std::move(packet)) { }

            void Read() override;

            uint64 FollowerDBID = 0;
            bool Inactive = false;
        };

        class GarrisonSetRecruitmentPreferences final : public ClientPacket
        {
        public:
            GarrisonSetRecruitmentPreferences(WorldPacket&& packet) : ClientPacket(CMSG_GARRISON_SET_RECRUITMENT_PREFERENCES, std::move(packet)) { }

            void Read() override;

            ObjectGuid NpcGuid;
            int32 MechanicTypeID = 0;
            int32 TraitID = 0;
        };

        class GarrisonStartMission final : public ClientPacket
        {
        public:
            GarrisonStartMission(WorldPacket&& packet) : ClientPacket(CMSG_GARRISON_START_MISSION, std::move(packet)) { }

            void Read() override;

            std::vector<uint64> FollowerDBIDs;
            ObjectGuid NpcGUID;
            int32 InfoCount = 0;
            int32 MissionRecID = 0;
        };

        class GarrisonSwapBuildings final : public ClientPacket
        {
        public:
            GarrisonSwapBuildings(WorldPacket&& packet) : ClientPacket(CMSG_GARRISON_SWAP_BUILDINGS, std::move(packet)) { }

            void Read() override;

            ObjectGuid NpcGUID;
            uint32 PlotId1 = 0;
            uint32 PlotId2 = 0;
        };

        class GarrisonMissionUpdate final : public ServerPacket
        {
        public:
            GarrisonMissionUpdate() : ServerPacket(SMSG_GARRISON_MISSION_START_CONDITION_UPDATE, 8) { }

            WorldPacket const* Write() override;

            std::vector<int32> ArchivedMissions;
            std::vector<bool> CanStartMission;
        };

        class GetlandingpageshipmentResponse final : public ServerPacket
        {
        public:
            GetlandingpageshipmentResponse() : ServerPacket(SMSG_GET_LANDING_PAGE_SHIPMENTS_RESPONSE, 8) { }

            WorldPacket const* Write() override;

            uint32 Unk;
            uint32 ShipmentsCount;
        };

        class CreateShipment final : public ClientPacket
        {
        public:
            CreateShipment(WorldPacket&& packet) : ClientPacket(CMSG_CREATE_SHIPMENT, std::move(packet)) { }

            void Read() override;

            ObjectGuid NpcGUID;
            uint32 Count = 0;
            uint32 unk10;
        };

        struct Shipment
        {
            Shipment() = default;
            uint64 FollowerDBID = 0;
            uint64 ShipmentID = 0;
            uint32 ShipmentRecID = 0;
            uint32 BuildingTypeID = 0;
            time_t CreationTime = time(nullptr);
            int32 ShipmentDuration = 0;

            bool finished = false;
          //  ObjectDBState DbState = DB_STATE_NEW;
            uint32 end = 0;
        };

        class GetShipmentInfoResponse final : public ServerPacket
        {
        public:
            GetShipmentInfoResponse() : ServerPacket(SMSG_GET_SHIPMENT_INFO_RESPONSE, 1 + 4 + 4 + 4 + 4) { }

            WorldPacket const* Write() override;

            std::vector<Shipment> Shipments;
            uint32 ShipmentID = 0;
            uint32 MaxShipments = 0;
            uint32 PlotInstanceID = 0;
            bool Success = false;
        };

        struct GarrTradeSkill
        {
            std::vector<uint32> SkillLineIDs;
            std::vector<uint32> SkillRanks;
            std::vector<uint32> SkillMaxRanks;
            std::vector<uint32> KnownAbilitySpellIDs;
            uint32 SpellID = 0;
        };

        struct FollowersClassSpecInfo
        {
            uint32 Category = 0;
            uint32 Option = 0;
        };

        struct TrophyListInfo
        {
            uint32 TrophyID = 0;
            uint32 Unk1 = 0;
            uint32 Unk2 = 0;
        };

        class GetTrophyListResponse final : public ServerPacket
        {
        public:
            GetTrophyListResponse() : ServerPacket(SMSG_GET_TROPHY_LIST_RESPONSE, 1 + 4) { }

            WorldPacket const* Write() override;

            std::vector<TrophyListInfo> MsgData;
            bool Success = false;
        };

        class GetSelectedTrophyIdResponse final : public ServerPacket
        {
        public:
            GetSelectedTrophyIdResponse() : ServerPacket(SMSG_GET_SELECTED_TROPHY_ID_RESPONSE) { }

            WorldPacket const* Write() override;

            bool Success = false;
            int32 TrophyID;
        };

        class GarrisonCompleteShipmentResponse final : public ServerPacket
        {
        public:
            GarrisonCompleteShipmentResponse() : ServerPacket(SMSG_COMPLETE_SHIPMENT_RESPONSE, 4) { }
            uint64 ShipmentID = 0;
            uint32 Result = 0;

            WorldPacket const* Write() override;
        };

        class GarrisonGetLandingPageShipments final : public ClientPacket
        {
        public:
            GarrisonGetLandingPageShipments(WorldPacket&& packet) : ClientPacket(CMSG_GET_LANDING_PAGE_SHIPMENTS, std::move(packet)) { }

            void Read() override { }
        };

        class GarrisonTalentWorldQuestUnlocksResponse final : public ServerPacket
        {
        public:
            GarrisonTalentWorldQuestUnlocksResponse() : ServerPacket(SMSG_GARRISON_TALENT_WORLD_QUEST_UNLOCKS_RESPONSE) { }
            int32 UNK1;
            int32 State;
            int32 QuestID1;
            int32 UNK2;
            int32 QuestID2;
            int32 MapID;
            uint8 UNK3;
            int32 UNK4;
            uint8 UNK5;
            int32 UNK8;
            bool UNK6;
            bool UNK7;
            uint8 UNK9;

            WorldPacket const* Write() override;
        };

        class GarrisonResponseClassSpecCategoryInfo final : public ServerPacket
        {
        public:
            GarrisonResponseClassSpecCategoryInfo() : ServerPacket(SMSG_GARRISON_GET_CLASS_SPEC_CATEGORY_INFO_RESULT, 4 + 4) { }

            WorldPacket const* Write() override;

            int32 GarrFollowerTypeID = 0;
            std::vector<FollowersClassSpecInfo> Datas;
        };

        class GarrisonRequestClassSpecCategoryInfo final : public ClientPacket
        {
        public:
            GarrisonRequestClassSpecCategoryInfo(WorldPacket&& packet) : ClientPacket(CMSG_GARRISON_GET_CLASS_SPEC_CATEGORY_INFO, std::move(packet)) { }

            void Read() override;

            uint32 ClassID = 0;
        };

        class GarrisonCollectionUpdateEntry final : public ServerPacket // full
        {
        public:
            GarrisonCollectionUpdateEntry() : ServerPacket(SMSG_GARRISON_COLLECTION_UPDATE_ENTRY) { }

            WorldPacket const* Write() override;

            int32 EntryTypeId;
            uint32 Type;
            int32 UNK3;
            uint8 UNK4;
            uint8 UNK;
            uint8 UNK1;
            bool UNK2 = false;
        };

        class GarrisonsTalentCompleted final : public ServerPacket // full
        {
        public:
            GarrisonsTalentCompleted() : ServerPacket(SMSG_GARRISON_TALENT_COMPLETED) { }

            WorldPacket const* Write() override;

            int32 GarrTypeID = 0;
            uint32 GarrTalentID;
            uint8 IsComplete;
            int32 Flags = 0;
            uint8 State;
            uint8 Unk;
            bool unk1;
            bool unk2;
            bool unk3;
            bool unk4;
        };

        class GarrisonRequestGarrisonTalentWorldQuestUnlocks final : public ClientPacket
        {
        public:
            GarrisonRequestGarrisonTalentWorldQuestUnlocks(WorldPacket&& packet) : ClientPacket(CMSG_REQUEST_GARRISON_TALENT_WORLD_QUEST_UNLOCKS, std::move(packet)) { }

            void Read() override;
        };

        class ReplaceTrophyResponse final : public ServerPacket
        {
        public:
            ReplaceTrophyResponse() : ServerPacket(SMSG_REPLACE_TROPHY_RESPONSE, 1) { }

            WorldPacket const* Write() override;

            bool Success = false;
        };


        class DeleteExpiredMissionsResult final : public ServerPacket
        {
        public:
            DeleteExpiredMissionsResult() : ServerPacket(SMSG_DELETE_EXPIRED_MISSIONS_RESULT) { }

            WorldPacket const* Write() override;

            uint32 GarrTypeID = 0;
            uint32 Result = 0;
            uint32 RemovedMissions = 0;
            uint32 RemovedMissionsCount = 0;
            bool Succeeded = false;
            bool LegionUnkBit = false;
        };

        class GarrisonTradeSkillResponse final : public ServerPacket
        {
        public:
            GarrisonTradeSkillResponse() : ServerPacket(SMSG_GARRISON_OPEN_CRAFTER, 16 + sizeof(GarrTradeSkill) + 4) { }

            WorldPacket const* Write() override;

            ObjectGuid GUID;
            GarrTradeSkill TradeSkill;
            std::vector<uint32> PlayerConditionID;
        };

        class UpdateGarrisonMonumentSections final : public ServerPacket
        {
        public:
            UpdateGarrisonMonumentSections() : ServerPacket(SMSG_GARRISON_UPDATE_GARRISON_MONUMENT_SELECTIONS) { }

            WorldPacket const* Write() override;

            uint32 TrophyCount;
            int32 TrophyID;
        };

        class GarrisonFollowerFatigueCleared final : public ServerPacket
        {
        public:
            GarrisonFollowerFatigueCleared() : ServerPacket(SMSG_GARRISON_FOLLOWER_FATIGUE_CLEARED) { }

            WorldPacket const* Write() override;

            int32 GarrTypeID;
            int32 Result;
        };

        class GarrisonSwitchTalentTreeBranch final : public ServerPacket
        {
        public:
            GarrisonSwitchTalentTreeBranch() : ServerPacket(SMSG_GARRISON_SWITCH_TALENT_TREE_BRANCH) { }

            WorldPacket const* Write() override;

            int32 GarrTypeID;
            int32 GarrTalentID = 0;
            uint32 ResearchStartTime = time_t(0);;
            uint32 Flags = 0;
            int32 Rank;
            uint32 Unk;
        };

        //< CMSG_REPLACE_TROPHY
        //< CMSG_CHANGE_MONUMENT_APPEARANCE
        class TrophyData final : public ClientPacket
        {
        public:
            TrophyData(WorldPacket&& packet) : ClientPacket(std::move(packet)) { }

            void Read() override;

            ObjectGuid TrophyGUID;
            uint32 NewTrophyID = 0;
        };

        class GetTrophyList final : public ClientPacket
        {
        public:
            GetTrophyList(WorldPacket&& packet) : ClientPacket(CMSG_GET_TROPHY_LIST, std::move(packet)) { }

            void Read() override;

            uint32 TrophyTypeID = 0;
        };

        class GarrisonGetMissionReward final : public ClientPacket
        {
        public:
            GarrisonGetMissionReward(WorldPacket&& packet) : ClientPacket(CMSG_GARRISON_GET_MISSION_REWARD, std::move(packet)) { }

            void Read() override;

            int64 unkint64 = 0;
            uint32 missionID = 0;
        };

        class GarrisonSetBuildingActive final : public ClientPacket
        {
        public:
            GarrisonSetBuildingActive(WorldPacket&& packet) : ClientPacket(CMSG_GARRISON_SET_BUILDING_ACTIVE, std::move(packet)) { }

            void Read() override;

            uint32 plotInstanceID = 0;
        };

        class GarrisonSetFollowerFavorite final : public ClientPacket
        {
        public:
            GarrisonSetFollowerFavorite(WorldPacket&& packet) : ClientPacket(CMSG_GARRISON_SET_FOLLOWER_FAVORITE, std::move(packet)) { }

            void Read() override;

            int64 FollowerDbId = 0;
            bool Favorite = false;
        };

        class GarrisonAddFollowerHealth final : public ClientPacket
        {
        public:
            GarrisonAddFollowerHealth(WorldPacket&& packet) : ClientPacket(CMSG_GARRISON_ADD_FOLLOWER_HEALTH, std::move(packet)) { }

            void Read() override;

            uint32  FollowerDBID = 0;
            uint32 Health = 0;
        };

        class GarrisonFullyHealAllFollowers final : public ClientPacket
        {
        public:
            GarrisonFullyHealAllFollowers(WorldPacket&& packet) : ClientPacket(CMSG_GARRISON_FULLY_HEAL_ALL_FOLLOWERS, std::move(packet)) { }

            void Read() override;

            uint32 unkint32 = 0;
        };

        class GarrisonLearnTalent final : public ClientPacket
        {
        public:
            GarrisonLearnTalent(WorldPacket&& packet) : ClientPacket(CMSG_GARRISON_LEARN_TALENT, std::move(packet)) { }

            void Read() override;

            uint32 talentid = 0;
            uint32 unk2int32 = 0;
        };

        class GarrisonOpenshipmentnpc final : public ClientPacket
        {
        public:
            GarrisonOpenshipmentnpc(WorldPacket&& packet) : ClientPacket(CMSG_OPEN_SHIPMENT_NPC, std::move(packet)) { }

            void Read() override;

            ObjectGuid NpcGuid;
        };

        class GarrisonLearnSpecializationResult final : public ServerPacket
        {
        public:
            GarrisonLearnSpecializationResult() : ServerPacket(SMSG_GARRISON_LEARN_SPECIALIZATION_RESULT) { }

            WorldPacket const* Write() override;

            uint32 SpecId;
        };

        class GarrisonBuildingSetActiveSpecializationResult final : public ServerPacket
        {
        public:
            GarrisonBuildingSetActiveSpecializationResult() : ServerPacket(SMSG_GARRISON_BUILDING_SET_ACTIVE_SPECIALIZATION_RESULT) { }

            WorldPacket const* Write() override;

            uint32 SpecId;
        };

        class GarrisonDeleteMissionResult final : public ServerPacket
        {
        public:
            GarrisonDeleteMissionResult() : ServerPacket(SMSG_GARRISON_DELETE_MISSION_RESULT) { }

            WorldPacket const* Write() override;

            uint32 Missionid;
        };

        class GarrisonCompleteBuildingResult final : public ServerPacket
        {
        public:
            GarrisonCompleteBuildingResult() : ServerPacket(SMSG_GARRISON_COMPLETE_BUILDING_CONSTRUCTION_RESULT) { }

            WorldPacket const* Write() override;

            uint32 buildingid;
        };

        class GarrisonSocketTalent final : public ClientPacket
        {
        public:
            GarrisonSocketTalent(WorldPacket&& packet) : ClientPacket(CMSG_GARRISON_SOCKET_TALENT, std::move(packet)) { }

            void Read() override;

            int32 unk1;
            int32 unk2;
            int32 unk3;
            int32 unk4;
        };
        //
    }
}

#endif // GarrisonPackets_h__
