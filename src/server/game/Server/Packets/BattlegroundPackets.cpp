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

#include "BattlegroundPackets.h"

WorldPacket const* WorldPackets::Battleground::SeasonInfo::Write()
{
    _worldPacket << int32(MythicPlusDisplaySeasonID);
    _worldPacket << int32(MythicPlusMilestoneSeasonID);
    _worldPacket << int32(CurrentArenaSeason);
    _worldPacket << int32(PreviousArenaSeason);
    _worldPacket << int32(ConquestWeeklyProgressCurrencyID);
    _worldPacket << int32(PvpSeasonID);
    _worldPacket.WriteBit(WeeklyRewardChestsEnabled);
    _worldPacket.FlushBits();

    return &_worldPacket;
}

void WorldPackets::Battleground::AreaSpiritHealerQuery::Read()
{
    _worldPacket >> HealerGuid;
}

void WorldPackets::Battleground::AreaSpiritHealerQueue::Read()
{
    _worldPacket >> HealerGuid;
}

WorldPacket const* WorldPackets::Battleground::AreaSpiritHealerTime::Write()
{
    _worldPacket << HealerGuid;
    _worldPacket << int32(TimeLeft);

    return &_worldPacket;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Battleground::PVPMatchStatistics::RatingData const& ratingData)
{
    for (std::size_t i = 0; i < 2; ++i)
    {
        data << int32(ratingData.Prematch[i]);
        data << int32(ratingData.Postmatch[i]);
        data << int32(ratingData.PrematchMMR[i]);
    }
    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Battleground::PVPMatchStatistics::HonorData const& honorData)
{
    data << uint32(honorData.HonorKills);
    data << uint32(honorData.Deaths);
    data << uint32(honorData.ContributionPoints);
    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Battleground::PVPMatchStatistics::PVPMatchPlayerPVPStat const& pvpStat)
{
    data << int32(pvpStat.PvpStatID);
    data << int32(pvpStat.PvpStatValue);
    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Battleground::PVPMatchStatistics::PVPMatchPlayerStatistics const& playerData)
{
    data << playerData.PlayerGUID;
    data << uint32(playerData.Kills);
    data << uint32(playerData.DamageDone);
    data << uint32(playerData.HealingDone);
    data << uint32(playerData.Stats.size());
    data << int32(playerData.PrimaryTalentTree);
    data << int8(playerData.Sex);
    data << int32(playerData.Race);
    data << int32(playerData.Class);
    data << int32(playerData.CreatureID);
    data << int32(playerData.HonorLevel);
    data << int32(playerData.Role);
    for (WorldPackets::Battleground::PVPMatchStatistics::PVPMatchPlayerPVPStat const& pvpStat : playerData.Stats)
        data << pvpStat;

    data.WriteBit(playerData.Faction != 0);
    data.WriteBit(playerData.IsInWorld);
    data.WriteBit(playerData.Honor.has_value());
    data.WriteBit(playerData.PreMatchRating.has_value());
    data.WriteBit(playerData.RatingChange.has_value());
    data.WriteBit(playerData.PreMatchMMR.has_value());
    data.WriteBit(playerData.MmrChange.has_value());
    data.WriteBit(playerData.PostMatchMMR.has_value());
    data.FlushBits();

    if (playerData.Honor)
        data << *playerData.Honor;

    if (playerData.PreMatchRating)
        data << uint32(*playerData.PreMatchRating);

    if (playerData.RatingChange)
        data << int32(*playerData.RatingChange);

    if (playerData.PreMatchMMR)
        data << uint32(*playerData.PreMatchMMR);

    if (playerData.MmrChange)
        data << int32(*playerData.MmrChange);

    if (playerData.PostMatchMMR)
        data << uint32(*playerData.PostMatchMMR);

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Battleground::PVPMatchStatistics const& pvpLogData)
{
    data.WriteBit(pvpLogData.Ratings.has_value());
    data << uint32(pvpLogData.Statistics.size());
    data.append(pvpLogData.PlayerCount.data(), pvpLogData.PlayerCount.size());

    if (pvpLogData.Ratings.has_value())
        data << *pvpLogData.Ratings;

    for (WorldPackets::Battleground::PVPMatchStatistics::PVPMatchPlayerStatistics const& player : pvpLogData.Statistics)
        data << player;

    return data;
}

WorldPacket const* WorldPackets::Battleground::PVPMatchStatisticsMessage::Write()
{
    _worldPacket.reserve(Data.Statistics.size() * sizeof(PVPMatchStatistics::PVPMatchPlayerStatistics) + sizeof(PVPMatchStatistics));

    _worldPacket << Data;

    return &_worldPacket;
}

void WorldPackets::Battleground::BattlemasterJoin::Read()
{
    QueueIDs.resize(_worldPacket.read<uint32>());
    _worldPacket >> Roles;
    _worldPacket >> BlacklistMap[0] >> BlacklistMap[1];
    for (uint64& queueId : QueueIDs)
        _worldPacket >> queueId;
}

void WorldPackets::Battleground::BattlemasterJoinArena::Read()
{
    _worldPacket >> TeamSizeIndex;
    _worldPacket >> Roles;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Battleground::BattlefieldStatusHeader const& header)
{
    data << header.Ticket;
    data << uint32(header.QueueID.size());
    data << uint8(header.RangeMin);
    data << uint8(header.RangeMax);
    data << uint8(header.TeamSize);
    data << uint32(header.InstanceID);
    for (uint64 queueID : header.QueueID)
        data << uint64(queueID);

    data.WriteBit(header.RegisteredMatch);
    data.WriteBit(header.TournamentRules);
    data.FlushBits();
    return data;
}

WorldPacket const* WorldPackets::Battleground::BattlefieldStatusNone::Write()
{
    _worldPacket << Ticket;
    return &_worldPacket;
}

WorldPacket const* WorldPackets::Battleground::BattlefieldStatusNeedConfirmation::Write()
{
    _worldPacket << Hdr;
    _worldPacket << uint32(Mapid);
    _worldPacket << uint32(Timeout);
    _worldPacket << uint8(Role);
    return &_worldPacket;
}

WorldPacket const* WorldPackets::Battleground::BattlefieldStatusActive::Write()
{
    _worldPacket << Hdr;
    _worldPacket << uint32(Mapid);
    _worldPacket << uint32(ShutdownTimer);
    _worldPacket << uint32(StartTimer);
    _worldPacket.WriteBit(ArenaFaction != 0);
    _worldPacket.WriteBit(LeftEarly);
    _worldPacket.FlushBits();
    return &_worldPacket;
}

WorldPacket const* WorldPackets::Battleground::BattlefieldStatusQueued::Write()
{
    _worldPacket << Hdr;
    _worldPacket << uint32(AverageWaitTime);
    _worldPacket << uint32(WaitTime);
    _worldPacket << int32(Unused920);
    _worldPacket.WriteBit(AsGroup);
    _worldPacket.WriteBit(EligibleForMatchmaking);
    _worldPacket.WriteBit(SuspendedQueue);
    _worldPacket.FlushBits();
    return &_worldPacket;
}

WorldPacket const* WorldPackets::Battleground::BattlefieldStatusFailed::Write()
{
    _worldPacket << Ticket;
    _worldPacket << uint64(QueueID);
    _worldPacket << uint32(Reason);
    _worldPacket << ClientID;
    return &_worldPacket;
}

void WorldPackets::Battleground::BattlefieldPort::Read()
{
    _worldPacket >> Ticket;
    AcceptedInvite = _worldPacket.ReadBit();
}

void WorldPackets::Battleground::BattlefieldListRequest::Read()
{
    _worldPacket >> ListID;
}

WorldPacket const* WorldPackets::Battleground::BattlefieldList::Write()
{
    _worldPacket << BattlemasterGuid;
    _worldPacket << int32(BattlemasterListID);
    _worldPacket << uint8(MinLevel);
    _worldPacket << uint8(MaxLevel);
    _worldPacket << uint32(Battlefields.size());
    if (!Battlefields.empty())
        _worldPacket.append(Battlefields.data(), Battlefields.size());

    _worldPacket.WriteBit(PvpAnywhere);
    _worldPacket.WriteBit(HasRandomWinToday);
    _worldPacket.FlushBits();
    return &_worldPacket;
}

WorldPacket const* WorldPackets::Battleground::PVPOptionsEnabled::Write()
{
    _worldPacket.WriteBit(RatedBattlegrounds);
    _worldPacket.WriteBit(PugBattlegrounds);
    _worldPacket.WriteBit(WargameBattlegrounds);
    _worldPacket.WriteBit(WargameArenas);
    _worldPacket.WriteBit(RatedArenas);
    _worldPacket.WriteBit(ArenaSkirmish);
    _worldPacket.FlushBits();
    return &_worldPacket;
}

void WorldPackets::Battleground::ReportPvPPlayerAFK::Read()
{
    _worldPacket >> Offender;
}

WorldPacket const* WorldPackets::Battleground::ReportPvPPlayerAFKResult::Write()
{
    _worldPacket << Offender;
    _worldPacket << uint8(Result);
    _worldPacket << uint8(NumBlackMarksOnOffender);
    _worldPacket << uint8(NumPlayersIHaveReported);
    return &_worldPacket;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Battleground::BattlegroundPlayerPosition const& playerPosition)
{
    data << playerPosition.Guid;
    data << playerPosition.Pos;
    data << int8(playerPosition.IconID);
    data << int8(playerPosition.ArenaSlot);
    return data;
}

WorldPacket const* WorldPackets::Battleground::BattlegroundPlayerPositions::Write()
{
    _worldPacket << uint32(FlagCarriers.size());
    for (BattlegroundPlayerPosition const& pos : FlagCarriers)
        _worldPacket << pos;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Battleground::BattlegroundPlayerJoined::Write()
{
    _worldPacket << Guid;
    return &_worldPacket;
}

WorldPacket const* WorldPackets::Battleground::BattlegroundPlayerLeft::Write()
{
    _worldPacket << Guid;
    return &_worldPacket;
}

WorldPacket const* WorldPackets::Battleground::DestroyArenaUnit::Write()
{
    _worldPacket << Guid;
    return &_worldPacket;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Battleground::RatedPvpInfo::BracketInfo const& bracketInfo)
{
    data << uint32(bracketInfo.PersonalRating);
    data << uint32(bracketInfo.Ranking);
    data << uint32(bracketInfo.SeasonPlayed);
    data << uint32(bracketInfo.SeasonWon);
    data << uint32(bracketInfo.Unused1);
    data << uint32(bracketInfo.Unused2);
    data << uint32(bracketInfo.WeeklyPlayed);
    data << uint32(bracketInfo.WeeklyWon);
    data << uint32(bracketInfo.RoundsSeasonPlayed);
    data << uint32(bracketInfo.RoundsSeasonWon);
    data << uint32(bracketInfo.RoundsWeeklyPlayed);
    data << uint32(bracketInfo.RoundsWeeklyWon);
    data << uint32(bracketInfo.BestWeeklyRating);
    data << uint32(bracketInfo.LastWeeksBestRating);
    data << uint32(bracketInfo.BestSeasonRating);
    data << uint32(bracketInfo.PvpTierID);
    data << uint32(bracketInfo.Unused3);
    data << uint32(bracketInfo.Unused4);
    data << uint32(bracketInfo.Rank);
    data.WriteBit(bracketInfo.Disqualified);
    data.FlushBits();

    return data;
}

WorldPacket const* WorldPackets::Battleground::RatedPvpInfo::Write()
{
    for (BracketInfo const& bracket : Bracket)
        _worldPacket << bracket;

    return &_worldPacket;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Battleground::RatedMatchDeserterPenalty const& ratedMatchDeserterPenalty)
{
    data << int32(ratedMatchDeserterPenalty.PersonalRatingChange);
    data << int32(ratedMatchDeserterPenalty.QueuePenaltySpellID);
    data << ratedMatchDeserterPenalty.QueuePenaltyDuration;
    return data;
}

WorldPacket const* WorldPackets::Battleground::PVPMatchInitialize::Write()
{
    _worldPacket << uint32(MapID);
    _worldPacket << uint8(State);
    _worldPacket << StartTime;
    _worldPacket << Duration;
    _worldPacket << uint8(ArenaFaction);
    _worldPacket << uint32(BattlemasterListID);
    _worldPacket.FlushBits();

    _worldPacket.WriteBit(Registered);
    _worldPacket.WriteBit(AffectsRating);
    _worldPacket.WriteBit(DeserterPenalty.has_value());
    _worldPacket.FlushBits();

    if (DeserterPenalty)
        _worldPacket << *DeserterPenalty;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Battleground::PVPMatchSetState::Write()
{
    _worldPacket << uint8(State);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Battleground::PVPMatchComplete::Write()
{
    _worldPacket << uint8(Winner);
    _worldPacket << Duration;
    _worldPacket.WriteBit(LogData.has_value());
    _worldPacket.WriteBits(SoloShuffleStatus, 2);
    _worldPacket.FlushBits();

    if (LogData)
        _worldPacket << *LogData;

    return &_worldPacket;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Battleground::BattlegroundCapturePointInfo const& battlegroundCapturePointInfo)
{
    data << battlegroundCapturePointInfo.Guid;
    data << battlegroundCapturePointInfo.Pos;
    data << int8(battlegroundCapturePointInfo.State);

    if (battlegroundCapturePointInfo.State == WorldPackets::Battleground::BattlegroundCapturePointState::ContestedHorde || battlegroundCapturePointInfo.State == WorldPackets::Battleground::BattlegroundCapturePointState::ContestedAlliance)
    {
        data << battlegroundCapturePointInfo.CaptureTime;
        data << battlegroundCapturePointInfo.CaptureTotalDuration;
    }

    return data;
}

WorldPacket const* WorldPackets::Battleground::UpdateCapturePoint::Write()
{
    _worldPacket << CapturePointInfo;
    return &_worldPacket;
}

WorldPacket const* WorldPackets::Battleground::CapturePointRemoved::Write()
{
    _worldPacket << CapturePointGUID;
    return &_worldPacket;
}



// DekkCore >
WorldPacket const* WorldPackets::Battleground::ArenaPrepOpponentSpecializations::Write()
{
    _worldPacket << uint32(Data.size());
    for (auto const& itr : Data)
    {
        _worldPacket << itr.SpecializationID;
        _worldPacket << itr.Unk;
        _worldPacket << itr.Guid;
    }

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Battleground::BattlegroundPoints::Write()
{
    _worldPacket << BgPoints;
    _worldPacket.WriteBit(Team);
    _worldPacket.FlushBits();

    return &_worldPacket;
}

void WorldPackets::Battleground::AcceptWargameInvite::Read()
{
    _worldPacket >> OpposingPartyMember;
    _worldPacket >> QueueID;
    _worldPacket >> Accept;
}

WorldPacket const* WorldPackets::Battleground::ConquestFormulaConstants::Write()
{
    _worldPacket << uint32(PvpMinCPPerWeek);
    _worldPacket << uint32(PvpMaxCPPerWeek);
    _worldPacket << uint32(PvpCPBaseCoefficient);
    _worldPacket << float(PvpCPExpCoefficient);
    _worldPacket << float(PvpCPNumerator);

    return &_worldPacket;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Battleground::BattlegroundCapturePointInfoData& info)
{
    data << info.Guid;
    data << info.Pos;
    data << int8(info.NodeState);
    if (info.NodeState == NODE_STATE_HORDE_ASSAULT || info.NodeState == NODE_STATE_ALLIANCE_ASSAULT)
    {
        data << info.CaptureTime;
        data << info.CaptureTotalDuration;
    }

    return data;
}

WorldPacket const* WorldPackets::Battleground::MapObjectivesInit::Write()
{
    _worldPacket << static_cast<uint32>(CapturePointInfo.size());
    for (auto& v : CapturePointInfo)
    {
        _worldPacket << v.CaptureTime;
        _worldPacket << v.CaptureTotalDuration;
        _worldPacket << v.Guid;
        _worldPacket << v.NodeState;
        _worldPacket << v.Pos;
    }

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Battleground::CheckWargameEntry::Write()
{
    _worldPacket << OpposingPartyMember;
    _worldPacket << RealmID;
    _worldPacket << UnkShort;
    _worldPacket << OpposingPartyUserServer;
    _worldPacket << OpposingPartyBnetAccountID;
    _worldPacket << QueueID;
    _worldPacket << TimeoutSeconds;
    _worldPacket << TournamentRules;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Battleground::StatusWaitForGroups::Write()
{
    _worldPacket << Header;

    _worldPacket << Mapid;
    _worldPacket << Timeout;

    for (uint8 i = TEAM_ALLIANCE; i < TEAM_MAX; ++i)
    {
        _worldPacket << TotalPlayers[i];
        _worldPacket << AwaitingPlayers[i];
    }

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Battleground::Init::Write()
{
    _worldPacket << ServerTime; // C_PvP.GetArenaCrowdControlInfo
    _worldPacket << MaxPoints;

    return &_worldPacket;
}

void WorldPackets::Battleground::BattlemasterJoinBrawl::Read()
{
    _worldPacket >> RolesMask;
}

void WorldPackets::Battleground::JoinSkirmish::Read()
{
    JoinAsGroup = _worldPacket.ReadBit();
    UnkBool = _worldPacket.ReadBit();
    _worldPacket >> Roles;
    _worldPacket >> Bracket;
}

WorldPacket const* WorldPackets::Battleground::SendPvpBrawlInfo::Write()
{
    _worldPacket << int32(PvpBrawlID);
    _worldPacket << int32(TimeToBrawl);
    _worldPacket.FlushBits();
    _worldPacket.WriteBit(IsActive = true);

    _worldPacket << uint8(Bracket);
    _worldPacket << int32(UNK);
    _worldPacket << uint8(UNK1);
    _worldPacket << int32(UNK2);
    _worldPacket.WriteBit(UNK3);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Battleground::MapObjEvents::Write()
{
    _worldPacket << UniqueID;
    _worldPacket << DataSize;
    _worldPacket << static_cast<uint32>(Unk2.size());
    for (auto const& itr : Unk2)
        _worldPacket << itr;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Battleground::WargameRequestSuccessfullySentToOpponent::Write()
{
    _worldPacket << opponentguid;

    return &_worldPacket;
}

void WorldPackets::Battleground::BattlemasterJoinRatedSoloShuffle::Read()
{
    _worldPacket >> RolesMask;
}
// < DekkCore
