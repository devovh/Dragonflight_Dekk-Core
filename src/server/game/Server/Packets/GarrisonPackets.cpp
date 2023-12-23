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

#include "GarrisonPackets.h"
#include "DB2Structure.h"
#include "Errors.h"

namespace WorldPackets::Garrison
{
WorldPacket const* GarrisonCreateResult::Write()
{
    _worldPacket << uint32(Result);
    _worldPacket << uint32(GarrSiteLevelID);

    return &_worldPacket;
}

WorldPacket const* GarrisonDeleteResult::Write()
{
    _worldPacket << uint32(Result);
    _worldPacket << uint32(GarrSiteID);

    return &_worldPacket;
}

ByteBuffer& operator<<(ByteBuffer& data, GarrisonPlotInfo const& plotInfo)
{
    data << uint32(plotInfo.GarrPlotInstanceID);
    data << plotInfo.PlotPos;
    data << uint32(plotInfo.PlotType);

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, GarrisonBuildingInfo const& buildingInfo)
{
    data << uint32(buildingInfo.GarrPlotInstanceID);
    data << uint32(buildingInfo.GarrBuildingID);
    data << buildingInfo.TimeBuilt;
    data << uint32(buildingInfo.CurrentGarSpecID);
    data << buildingInfo.TimeSpecCooldown;
    data.WriteBit(buildingInfo.Active);
    data.FlushBits();

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, GarrisonFollower const& follower)
{
    data << uint64(follower.DbID);
    data << uint32(follower.GarrFollowerID);
    data << uint32(follower.Quality);
    data << uint32(follower.FollowerLevel);
    data << uint32(follower.ItemLevelWeapon);
    data << uint32(follower.ItemLevelArmor);
    data << uint32(follower.Xp);
    data << uint32(follower.Durability);
    data << uint32(follower.CurrentBuildingID);
    data << uint32(follower.CurrentMissionID);
    data << uint32(follower.AbilityID.size());
    data << uint32(follower.ZoneSupportSpellID);
    data << uint32(follower.FollowerStatus);
    data << int32(follower.Health);
    data << int8(follower.BoardIndex);
    data << follower.HealingTimestamp;
    for (GarrAbilityEntry const* ability : follower.AbilityID)
        data << uint32(ability->ID);

    data.WriteBits(follower.CustomName.length(), 7);
    data.FlushBits();
    data.WriteString(follower.CustomName);

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, GarrisonEncounter const& encounter)
{
    data << int32(encounter.GarrEncounterID);
    data << uint32(encounter.Mechanics.size());
    data << int32(encounter.GarrAutoCombatantID);
    data << int32(encounter.Health);
    data << int32(encounter.MaxHealth);
    data << int32(encounter.Attack);
    data << int8(encounter.BoardIndex);
    data << uint32(encounter.MechanicCount);

    if (!encounter.Mechanics.empty())
        data.append(encounter.Mechanics.data(), encounter.Mechanics.size());


    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, GarrisonMissionReward const& missionRewardItem)
{
    data << int32(missionRewardItem.ItemID);
    data << uint32(missionRewardItem.ItemQuantity);
    data << int32(missionRewardItem.CurrencyID);
    data << uint32(missionRewardItem.CurrencyQuantity);
    data << uint32(missionRewardItem.FollowerXP);
    data << uint32(missionRewardItem.GarrMssnBonusAbilityID);
    data << int32(missionRewardItem.ItemFileDataID);
    data.WriteBit(missionRewardItem.ItemInstance.has_value());
    data.FlushBits();

    if (missionRewardItem.ItemInstance)
        data << *missionRewardItem.ItemInstance;

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, GarrisonMission const& mission)
{
    data << uint64(mission.DbID);
    data << int32(mission.MissionRecID);
    data << mission.OfferTime;
    data << mission.OfferDuration;
    data << mission.StartTime;
    data << mission.TravelDuration;
    data << mission.MissionDuration;
    data << int32(mission.MissionState);
    data << int32(mission.SuccessChance);
    data << uint32(mission.Flags);
    data << float(mission.MissionScalar);
    data << int32(mission.ContentTuningID);
    data << uint32(mission.Encounters.size());
    data << uint32(mission.Rewards.size());
    data << uint32(mission.OvermaxRewards.size());

    for (GarrisonEncounter const& encounter : mission.Encounters)
        data << encounter;

    for (GarrisonMissionReward const& missionRewardItem : mission.Rewards)
        data << missionRewardItem;

    for (GarrisonMissionReward const& missionRewardItem : mission.OvermaxRewards)
        data << missionRewardItem;

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, GarrisonMissionBonusAbility const& areaBonus)
{
    data << areaBonus.StartTime;
    data << uint32(areaBonus.GarrMssnBonusAbilityID);

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, GarrisonTalentSocketData const& talentSocketData)
{
    data << int32(talentSocketData.SoulbindConduitID);
    data << int32(talentSocketData.SoulbindConduitRank);

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, GarrisonTalent const& talent)
{
    data << int32(talent.GarrTalentID);
    data << int32(talent.Rank);
    data << talent.ResearchStartTime;
    data << int32(talent.Flags);
    data.WriteBit(talent.Socket.has_value());
    data.FlushBits();

    if (talent.Socket)
        data << *talent.Socket;

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, GarrisonCollectionEntry const& collectionEntry)
{
    data << int32(collectionEntry.EntryID);
    data << int32(collectionEntry.Rank);

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, GarrisonCollection const& collection)
{
    data << int32(collection.Type);
    data << uint32(collection.Entries.size());
    for (GarrisonCollectionEntry const& collectionEntry : collection.Entries)
        data << collectionEntry;

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, GarrisonEventEntry const& event)
{
    data << int64(event.EventValue);
    data << int32(event.EntryID);

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, GarrisonEventList const& eventList)
{
    data << int32(eventList.Type);
    data << uint32(eventList.Events.size());
    for (GarrisonEventEntry const& event : eventList.Events)
        data << event;

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, GarrisonSpecGroup const& specGroup)
{
    data << int32(specGroup.ChrSpecializationID);
    data << int32(specGroup.SoulbindID);

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, GarrisonInfo const& garrison)
{
    ASSERT(garrison.Missions.size() == garrison.MissionRewards.size());
    ASSERT(garrison.Missions.size() == garrison.MissionOvermaxRewards.size());
    ASSERT(garrison.Missions.size() == garrison.CanStartMission.size());

    data << int32(garrison.GarrTypeID);
    data << int32(garrison.GarrSiteID);
    data << int32(garrison.GarrSiteLevelID);
    data << uint32(garrison.Buildings.size());
    data << uint32(garrison.Plots.size());
    data << uint32(garrison.Followers.size());
    data << uint32(garrison.AutoTroops.size());
    data << uint32(garrison.Missions.size());
    data << uint32(garrison.MissionRewards.size());
    data << uint32(garrison.MissionOvermaxRewards.size());
    data << uint32(garrison.MissionAreaBonuses.size());
    data << uint32(garrison.Talents.size());
    data << uint32(garrison.Collections.size());
    data << uint32(garrison.EventLists.size());
    data << uint32(garrison.SpecGroups.size());
    data << uint32(garrison.CanStartMission.size());
    data << uint32(garrison.ArchivedMissions.size());
    data << int32(garrison.NumFollowerActivationsRemaining);
    data << uint32(garrison.NumMissionsStartedToday);
    data << int32(garrison.MinAutoTroopLevel);

    for (GarrisonPlotInfo* plot : garrison.Plots)
        data << *plot;

    for (std::vector<GarrisonMissionReward> const& missionReward : garrison.MissionRewards)
        data << uint32(missionReward.size());

    for (std::vector<GarrisonMissionReward> const& missionReward : garrison.MissionOvermaxRewards)
        data << uint32(missionReward.size());

    for (GarrisonMissionBonusAbility const* areaBonus : garrison.MissionAreaBonuses)
        data << *areaBonus;

    for (GarrisonCollection const& collection : garrison.Collections)
        data << collection;

    for (GarrisonEventList const& eventList : garrison.EventLists)
        data << eventList;

    for (GarrisonSpecGroup const& specGroup : garrison.SpecGroups)
        data << specGroup;

    if (!garrison.ArchivedMissions.empty())
        data.append(garrison.ArchivedMissions.data(), garrison.ArchivedMissions.size());

    for (GarrisonBuildingInfo const* building : garrison.Buildings)
        data << *building;

    for (bool canStartMission : garrison.CanStartMission)
        data.WriteBit(canStartMission);

    data.FlushBits();

    for (GarrisonFollower const* follower : garrison.Followers)
        data << *follower;

    for (GarrisonFollower const* follower : garrison.AutoTroops)
        data << *follower;

    for (GarrisonMission const* mission : garrison.Missions)
        data << *mission;

    for (GarrisonTalent const& talent : garrison.Talents)
        data << talent;

    for (std::vector<GarrisonMissionReward> const& missionReward : garrison.MissionRewards)
        for (GarrisonMissionReward const& missionRewardItem : missionReward)
            data << missionRewardItem;

    for (std::vector<GarrisonMissionReward> const& missionReward : garrison.MissionOvermaxRewards)
        for (GarrisonMissionReward const& missionRewardItem : missionReward)
            data << missionRewardItem;

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, FollowerSoftCapInfo const& followerSoftCapInfo)
{
    data << int32(followerSoftCapInfo.GarrFollowerTypeID);
    data << uint32(followerSoftCapInfo.Count);
    return data;
}

WorldPacket const* GetGarrisonInfoResult::Write()
{
    _worldPacket << int32(FactionIndex);
    _worldPacket << uint32(Garrisons.size());
    _worldPacket << uint32(FollowerSoftCaps.size());
    for (FollowerSoftCapInfo const& followerSoftCapInfo : FollowerSoftCaps)
        _worldPacket << followerSoftCapInfo;

    for (GarrisonInfo const& garrison : Garrisons)
        _worldPacket << garrison;

    return &_worldPacket;
}

ByteBuffer& operator<<(ByteBuffer& data, GarrisonRemoteBuildingInfo const& building)
{
    data << uint32(building.GarrPlotInstanceID);
    data << uint32(building.GarrBuildingID);

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, GarrisonRemoteSiteInfo const& site)
{
    data << uint32(site.GarrSiteLevelID);
    data << uint32(site.Buildings.size());
    for (GarrisonRemoteBuildingInfo const& building : site.Buildings)
        data << building;

    return data;
}

WorldPacket const* GarrisonRemoteInfo::Write()
{
    _worldPacket << uint32(Sites.size());
    for (GarrisonRemoteSiteInfo const& site : Sites)
        _worldPacket << site;

    return &_worldPacket;
}

void GarrisonPurchaseBuilding::Read()
{
    _worldPacket >> NpcGUID;
    _worldPacket >> PlotInstanceID;
    _worldPacket >> BuildingID;
}

WorldPacket const* GarrisonPlaceBuildingResult::Write()
{
    _worldPacket << uint32(GarrTypeID);
    _worldPacket << uint32(Result);
    _worldPacket << BuildingInfo;
    _worldPacket.WriteBit(PlayActivationCinematic);
    _worldPacket.FlushBits();

    return &_worldPacket;
}

void GarrisonCancelConstruction::Read()
{
    _worldPacket >> PlotInstanceID;
}

WorldPacket const* GarrisonBuildingRemoved::Write()
{
    _worldPacket << uint32(GarrBuildingID);

    return &_worldPacket;
}

WorldPacket const* GarrisonLearnBlueprintResult::Write()
{
    _worldPacket << int32(GarrTypeID);
    _worldPacket << uint32(Result);
    _worldPacket << uint32(BuildingID);

    return &_worldPacket;
}

WorldPacket const* GarrisonUnlearnBlueprintResult::Write()
{
    _worldPacket << int32(GarrTypeID);
    _worldPacket << uint32(Result);
    _worldPacket << uint32(BuildingID);

    return &_worldPacket;
}

WorldPacket const* GarrisonRequestBlueprintAndSpecializationDataResult::Write()
{
    _worldPacket << uint32(GarrTypeID);
    _worldPacket << uint32(BlueprintsKnown ? BlueprintsKnown->size() : 0);
    _worldPacket << uint32(SpecializationsKnown ? SpecializationsKnown->size() : 0);
    if (BlueprintsKnown)
        for (uint32 blueprint : *BlueprintsKnown)
            _worldPacket << uint32(blueprint);

    if (SpecializationsKnown)
        for (uint32 specialization : *SpecializationsKnown)
            _worldPacket << uint32(specialization);

    return &_worldPacket;
}

ByteBuffer& operator<<(ByteBuffer& data, GarrisonBuildingMapData const& building)
{
    data << uint32(building.GarrBuildingPlotInstID);
    data << building.Pos;

    return data;
}

WorldPacket const* GarrisonMapDataResponse::Write()
{
    _worldPacket << uint32(Buildings.size());

    for (GarrisonBuildingMapData& landmark : Buildings)
        _worldPacket << landmark;

    return &_worldPacket;
}

WorldPacket const* GarrisonPlotPlaced::Write()
{
    _worldPacket << uint32(GarrTypeID);
    _worldPacket << *PlotInfo; 

    return &_worldPacket;
}

WorldPacket const* GarrisonPlotRemoved::Write()
{
    _worldPacket << uint32(GarrPlotInstanceID);

    return &_worldPacket;
}

WorldPacket const* GarrisonAddFollowerResult::Write()
{
    _worldPacket << uint32(GarrTypeID);
    _worldPacket << uint32(Result);
    _worldPacket << Follower;

    return &_worldPacket;
}

WorldPacket const* GarrisonRemoveFollowerResult::Write()
{
    _worldPacket << uint64(FollowerDBID);
    _worldPacket << int32(GarrTypeID);
    _worldPacket << uint32(Result);
    _worldPacket << uint32(Destroyed);

    return &_worldPacket;
}

WorldPacket const* GarrisonBuildingActivated::Write()
{
    _worldPacket << uint32(GarrPlotInstanceID);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Garrison::GarrisonAddMissionResult::Write()
{
    _worldPacket << int32(GarrType);
    _worldPacket << int32(Result);
    _worldPacket << State;
    _worldPacket << Mission;

    _worldPacket << uint32(Rewards.size());
    _worldPacket << uint32(BonusRewards.size());

    for (GarrisonMissionReward const& missionRewardItem : Rewards)
        _worldPacket << missionRewardItem;

    for (GarrisonMissionReward const& missionRewardItem : BonusRewards)
        _worldPacket << missionRewardItem;

    _worldPacket.WriteBit(Success);
    _worldPacket.WriteBit(CanStart);
    _worldPacket.FlushBits();

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Garrison::GarrisonAssignFollowerToBuildingResult::Write()
{
    _worldPacket << FollowerDBID;
    _worldPacket << Result;
    _worldPacket << PlotInstanceID;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Garrison::GarrisonCompleteMissionResult::Write()
{
    _worldPacket << Result;
    _worldPacket << MissionData;
    _worldPacket << MissionRecID;
    _worldPacket.WriteBit(Succeeded);
    _worldPacket.FlushBits();

    return &_worldPacket;
}

WorldPacket const* GarrisonFollowerActivationSet::Write()
{
    _worldPacket << GarrSiteID;
    _worldPacket << NumActivations;

    return &_worldPacket;
}

WorldPacket const* GarrisonCheckUpgradeableResult::Write()
{
    _worldPacket << uint32(!IsUpgradeable);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Garrison::GarrisonRemoveFollowerFromBuildingResult::Write()
{
    _worldPacket << FollowerDBID;
    _worldPacket << Result;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Garrison::GarrisonStartMissionResult::Write()
{
    _worldPacket << Result;
    _worldPacket << FailReason;
    _worldPacket << Mission;

    _worldPacket << Followers.size();
    for (uint64 followerDbID : Followers)
        _worldPacket << followerDbID;

    return &_worldPacket;
}

void WorldPackets::Garrison::GarrisonUpgrade::Read()
{
    _worldPacket >> NpcGUID;
}

WorldPacket const* WorldPackets::Garrison::GarrisonUpgradeResult::Write()
{
    return &_worldPacket;
}
WorldPacket const* CreateShipmentResponse::Write()
{
    _worldPacket << ShipmentID;
    _worldPacket << ShipmentRecID;
    _worldPacket << Result;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Garrison::GarrisonFollowerChangedItemLevel::Write()
{
    _worldPacket << Follower;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Garrison::GarrisonFollowerChangedXP::Write()
{
    _worldPacket << TotalXp;
    _worldPacket << Result;
    _worldPacket << Follower;
    _worldPacket << Follower2;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Garrison::GarrisonMissionBonusRollResult::Write()
{
    _worldPacket << MissionData;
    _worldPacket << MissionRecID;
    _worldPacket << Result;

    return &_worldPacket;
}

void WorldPackets::Garrison::GarrisonOpenMissionNpcRequest::Read()
{
    _worldPacket >> NpcGUID;
    _worldPacket >> GarrFollowerTypeID;
}

WorldPacket const* WorldPackets::Garrison::GarrisonOpenRecruitmentNpc::Write()
{
    _worldPacket << Guid;
    _worldPacket << GarrTypeID;
    _worldPacket << Result;
    for (auto const& follower : Followers)
        _worldPacket << follower;
    _worldPacket << CanRecruitFollower;
    _worldPacket << UnkBit;
    _worldPacket.FlushBits();

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Garrison::GarrisonRecruitFollowerResult::Write()
{
    _worldPacket << Result;
    _worldPacket << Follower;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Garrison::GarrisonResearchTalent::Write()
{
    _worldPacket << Result;
    _worldPacket << GarrTypeID;
    _worldPacket << TalentID;
    _worldPacket << ResearchTime;
    _worldPacket << Flags;
    _worldPacket << Rank;
    _worldPacket << GarrTalentTreeID;
    _worldPacket << State;

    _worldPacket.WriteBit(Socket);
    _worldPacket.WriteBit(IsTemporary);

    return &_worldPacket;
}

void WorldPackets::Garrison::GarrisonAssignFollowerToBuilding::Read()
{
    _worldPacket >> NpcGUID;
    _worldPacket >> BuildingID;
    _worldPacket >> FollowerDBID;
}

void WorldPackets::Garrison::GarrisonCompleteMission::Read()
{
    _worldPacket >> NpcGUID;
    _worldPacket >> MissionRecID;
}

void WorldPackets::Garrison::GarrisonGenerateRecruits::Read()
{
    _worldPacket >> NpcGuid;
    _worldPacket >> MechanicTypeID;
    _worldPacket >> TraitID;
}

void WorldPackets::Garrison::GarrisonMissionBonusRoll::Read()
{
    _worldPacket >> NpcGUID;
    _worldPacket >> MissionRecID;
}

void WorldPackets::Garrison::GarrisonRecruitFollower::Read()
{
    _worldPacket >> Guid;
    _worldPacket >> FollowerIndex;
    _worldPacket >> quality;
}

void WorldPackets::Garrison::GarrisonRemoveFollower::Read()
{
    _worldPacket >> Guid;
    _worldPacket >> FollowerDBID;
}

void WorldPackets::Garrison::GarrisonRemoveFollowerFromBuilding::Read()
{
    _worldPacket >> NpcGUID;
    _worldPacket >> FollowerDBID;
}

void WorldPackets::Garrison::GarrisonRenameFollower::Read()
{
    _worldPacket >> FollowerDBID;
    FollowerName = _worldPacket.ReadString(_worldPacket.ReadBits(7));
}

void WorldPackets::Garrison::GarrisonRequestShipmentInfo::Read()
{
    _worldPacket >> NpcGUID;
}

void WorldPackets::Garrison::GarrisonRequestResearchTalent::Read()
{
    _worldPacket >> NPCTalentGuid;
    _worldPacket >> GarrTalentID;
    auto GarrisonTalentFlag = _worldPacket.read<int32>();
    IsTemporary = _worldPacket.ReadBit();

}

void WorldPackets::Garrison::GarrisonSetFollowerInactive::Read()
{
    _worldPacket >> FollowerDBID;
    Inactive = _worldPacket.ReadBit();
}

void WorldPackets::Garrison::GarrisonSetRecruitmentPreferences::Read()
{
    _worldPacket >> NpcGuid;
    _worldPacket >> MechanicTypeID;
    _worldPacket >> TraitID;
}

void WorldPackets::Garrison::GarrisonStartMission::Read()
{
    _worldPacket >> NpcGUID;
    _worldPacket >> InfoCount;
    _worldPacket >> MissionRecID;

    for (uint32 i = 0; i < InfoCount; ++i)
    {
        uint64 followerDbID;
        _worldPacket >> followerDbID;
        FollowerDBIDs.push_back(followerDbID);
    }
}

void WorldPackets::Garrison::GarrisonSwapBuildings::Read()
{
    _worldPacket >> NpcGUID >> PlotId1 >> PlotId2;
}

WorldPacket const* WorldPackets::Garrison::GarrisonMissionUpdate::Write()
{
    _worldPacket << uint32(ArchivedMissions.size());
    _worldPacket << uint32(CanStartMission.size());

    if (!ArchivedMissions.empty())
        _worldPacket.append(ArchivedMissions.data(), ArchivedMissions.size());

    for (bool canStartMission : CanStartMission)
        _worldPacket.WriteBit(canStartMission);

    _worldPacket.FlushBits();

    return &_worldPacket;
}

WorldPacket const* GetlandingpageshipmentResponse::Write()
{
    _worldPacket << Unk;
    _worldPacket << ShipmentsCount;

    return &_worldPacket;
}

void WorldPackets::Garrison::CreateShipment::Read()
{
    _worldPacket >> NpcGUID;
    _worldPacket >> Count;
    _worldPacket >> unk10;
    
}

WorldPacket const* WorldPackets::Garrison::GetShipmentInfoResponse::Write()
{
    _worldPacket.WriteBit(Success);
    _worldPacket.FlushBits();

    _worldPacket << ShipmentID;
    _worldPacket << MaxShipments;
    _worldPacket << static_cast<uint32>(Shipments.size());
    _worldPacket << PlotInstanceID;
   // for (auto const& map : Shipments)
    //    _worldPacket << map;

    return &_worldPacket;
}

ByteBuffer& operator<<(ByteBuffer& _worldPacket, WorldPackets::Garrison::GarrTradeSkill const& tradeSkill)
{
    _worldPacket << tradeSkill.SpellID;
    _worldPacket << static_cast<uint32>(tradeSkill.SkillLineIDs.size());
    _worldPacket << static_cast<uint32>(tradeSkill.SkillRanks.size());
    _worldPacket << static_cast<uint32>(tradeSkill.SkillMaxRanks.size());
    _worldPacket << static_cast<uint32>(tradeSkill.KnownAbilitySpellIDs.size());

    for (auto const& data : tradeSkill.SkillLineIDs)
        _worldPacket << data;

    for (auto const& data : tradeSkill.SkillRanks)
        _worldPacket << data;

    for (auto const& data : tradeSkill.SkillMaxRanks)
        _worldPacket << data;

    for (auto const& data : tradeSkill.KnownAbilitySpellIDs)
        _worldPacket << data;

    return _worldPacket;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Garrison::Shipment const& shipment)
{
    data << shipment.ShipmentRecID;
    data << shipment.ShipmentID;
    data << shipment.FollowerDBID;
    data << uint32(shipment.CreationTime);
    data << shipment.ShipmentDuration;
    data << shipment.BuildingTypeID;

    return data;
}

WorldPacket const* WorldPackets::Garrison::DeleteExpiredMissionsResult::Write()
{
    _worldPacket << GarrTypeID;
    _worldPacket << Result;

    for (auto itr = 0; itr < RemovedMissionsCount; itr++)
        _worldPacket << RemovedMissions;

    _worldPacket.WriteBit(Succeeded);
    _worldPacket.WriteBit(LegionUnkBit);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Garrison::GetTrophyListResponse::Write()
{
    _worldPacket.WriteBit(Success);
    _worldPacket.FlushBits();

    _worldPacket << static_cast<uint32>(MsgData.size());
    for (auto const& map : MsgData)
    {
        _worldPacket << map.TrophyID;
        _worldPacket << map.Unk1;
        _worldPacket << map.Unk2;
    }

    return &_worldPacket;
}

WorldPacket const* GetSelectedTrophyIdResponse::Write()
{
    _worldPacket.FlushBits();
    _worldPacket.WriteBit(Success);
    _worldPacket << TrophyID;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Garrison::GarrisonCompleteShipmentResponse::Write()
{
    _worldPacket << ShipmentID;
    _worldPacket << Result;
    return &_worldPacket;
}

WorldPacket const* GarrisonTalentWorldQuestUnlocksResponse::Write()
{
    _worldPacket << int32(UNK1);
    _worldPacket << int32(State);
    _worldPacket << int32(QuestID1);
    _worldPacket << int32(UNK2);
    _worldPacket << int32(QuestID2);
    _worldPacket << int32(MapID);
    _worldPacket << uint8(UNK3);
    _worldPacket << int32(UNK4);
    _worldPacket << uint8(UNK5);
    _worldPacket << int32(UNK8);
    _worldPacket.WriteBit(UNK6);
    _worldPacket.WriteBit(UNK7);
    _worldPacket << uint8(UNK9);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Garrison::GarrisonResponseClassSpecCategoryInfo::Write()
{
    _worldPacket << GarrFollowerTypeID;
    _worldPacket << static_cast<uint32>(Datas.size());
    for (auto const& v : Datas)
    {
        _worldPacket << v.Category;
        _worldPacket << v.Option;
    }

    return &_worldPacket;
}

void WorldPackets::Garrison::GarrisonRequestClassSpecCategoryInfo::Read()
{
    _worldPacket >> ClassID;
}

void WorldPackets::Garrison::GarrisonRequestGarrisonTalentWorldQuestUnlocks::Read()
{
}

WorldPacket const* GarrisonCollectionUpdateEntry::Write()
{
    _worldPacket << int32(EntryTypeId);
    _worldPacket << uint32(Type);
    _worldPacket << int32(UNK3);
    _worldPacket << uint8(UNK4);
    _worldPacket << uint8(UNK);
    _worldPacket << uint8(UNK1);
    _worldPacket.WriteBit(UNK2);

    return &_worldPacket;
}

WorldPacket const* GarrisonsTalentCompleted::Write()
{
    _worldPacket << int32(GarrTypeID);
    _worldPacket << uint32(GarrTalentID);
    _worldPacket << uint8(IsComplete);
    _worldPacket << int32(Flags);
    _worldPacket << uint8(State);
    _worldPacket << uint8(Unk);
    _worldPacket.WriteBit(unk1);
    _worldPacket.WriteBit(unk2);
    _worldPacket.WriteBit(unk3);
    _worldPacket.WriteBit(unk4);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Garrison::ReplaceTrophyResponse::Write()
{
    _worldPacket.WriteBit(Success);
    _worldPacket.FlushBits();

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Garrison::GarrisonTradeSkillResponse::Write()
{
    _worldPacket << GUID;
    _worldPacket << TradeSkill;

    _worldPacket << static_cast<uint32>(PlayerConditionID.size());
    for (auto const& data : PlayerConditionID)
        _worldPacket << data;

    return &_worldPacket;
}

WorldPacket const* UpdateGarrisonMonumentSections::Write()
{
    _worldPacket << uint32(TrophyCount);
    _worldPacket << int32(TrophyID);

    return &_worldPacket;
}

WorldPacket const* GarrisonFollowerFatigueCleared::Write()
{
    _worldPacket << int32(GarrTypeID);
    _worldPacket << int32(Result);

    return &_worldPacket;
}

void WorldPackets::Garrison::TrophyData::Read()
{
    _worldPacket >> TrophyGUID;
    _worldPacket >> NewTrophyID;
}

void WorldPackets::Garrison::GetTrophyList::Read()
{
    _worldPacket >> TrophyTypeID;
}

void WorldPackets::Garrison::GarrisonGetMissionReward::Read()
{
    _worldPacket >> unkint64;
    _worldPacket >> missionID;
}

void WorldPackets::Garrison::GarrisonSetBuildingActive::Read()
{
    _worldPacket >> plotInstanceID;
}

void WorldPackets::Garrison::GarrisonSetFollowerFavorite::Read()
{
    _worldPacket >> FollowerDbId;
    _worldPacket.WriteBit(Favorite);
    _worldPacket.FlushBits();
}

void WorldPackets::Garrison::GarrisonAddFollowerHealth::Read()
{
    _worldPacket >> FollowerDBID;
    _worldPacket >> Health;
}

void WorldPackets::Garrison::GarrisonFullyHealAllFollowers::Read()
{
    _worldPacket >> unkint32;
}

void WorldPackets::Garrison::GarrisonLearnTalent::Read()
{
    _worldPacket >> talentid;
    _worldPacket >> unk2int32;
}
WorldPacket const* GarrisonSwitchTalentTreeBranch::Write()
{
    _worldPacket << int32(GarrTypeID);
    _worldPacket << int32(GarrTalentID);
    _worldPacket << uint32(ResearchStartTime);
    _worldPacket << uint32(Flags);
    _worldPacket << int32(Rank);
    _worldPacket << uint32(Unk);

    return &_worldPacket;
}

void GarrisonOpenshipmentnpc::Read()
{
    _worldPacket >> NpcGuid;
}

WorldPacket const* GarrisonLearnSpecializationResult::Write()
{
    _worldPacket << uint32(SpecId);

    return &_worldPacket;
}

WorldPacket const* GarrisonBuildingSetActiveSpecializationResult::Write()
{
    _worldPacket << uint32(SpecId);

    return &_worldPacket;
}

WorldPacket const* GarrisonDeleteMissionResult::Write()
{
    _worldPacket << uint32(Missionid);

    return &_worldPacket;
}
WorldPacket const* GarrisonCompleteBuildingResult::Write()
{
    _worldPacket << uint32(buildingid);

    return &_worldPacket;
}

void GarrisonSocketTalent::Read()
{
    _worldPacket >> unk1;
    _worldPacket >> unk2;
    _worldPacket >> unk3;
    _worldPacket >> unk4;
}
}
