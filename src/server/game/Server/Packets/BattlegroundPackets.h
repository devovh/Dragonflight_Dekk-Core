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

#ifndef BattlegroundPackets_h__
#define BattlegroundPackets_h__

#include "Packet.h"
#include "LFGPacketsCommon.h"
#include "ObjectGuid.h"
#include "Optional.h"
#include "PacketUtilities.h"
#include "Position.h"
#include <array>

namespace WorldPackets
{
    namespace Battleground
    {
        class SeasonInfo final : public ServerPacket
        {
        public:
            SeasonInfo() : ServerPacket(SMSG_SEASON_INFO, 4 + 4 + 4 + 4 + 4 + 1) { }

            WorldPacket const* Write() override;

            int32 MythicPlusDisplaySeasonID = 19;
            int32 MythicPlusMilestoneSeasonID = 93;
            int32 PreviousArenaSeason = 34;
            int32 CurrentArenaSeason = 33;
            int32 PvpSeasonID = 27;
            int32 ConquestWeeklyProgressCurrencyID = 1891;
            bool WeeklyRewardChestsEnabled = true;
        };

        class AreaSpiritHealerQuery final : public ClientPacket
        {
        public:
            AreaSpiritHealerQuery(WorldPacket&& packet) : ClientPacket(CMSG_AREA_SPIRIT_HEALER_QUERY, std::move(packet)) { }

            void Read() override;

            ObjectGuid HealerGuid;
        };

        class AreaSpiritHealerQueue final : public ClientPacket
        {
        public:
            AreaSpiritHealerQueue(WorldPacket&& packet) : ClientPacket(CMSG_AREA_SPIRIT_HEALER_QUEUE, std::move(packet)) { }

            void Read() override;

            ObjectGuid HealerGuid;
        };

        class AreaSpiritHealerTime final : public ServerPacket
        {
        public:
            AreaSpiritHealerTime() : ServerPacket(SMSG_AREA_SPIRIT_HEALER_TIME, 14 + 4) { }

            WorldPacket const* Write() override;

            ObjectGuid HealerGuid;
            int32 TimeLeft = 0;
        };

        class HearthAndResurrect final : public ClientPacket
        {
        public:
            HearthAndResurrect(WorldPacket&& packet) : ClientPacket(CMSG_HEARTH_AND_RESURRECT, std::move(packet)) { }

            void Read() override { }
        };

        class PVPLogDataRequest final : public ClientPacket
        {
        public:
            PVPLogDataRequest(WorldPacket&& packet) : ClientPacket(CMSG_PVP_LOG_DATA, std::move(packet)) { }

            void Read() override { }
        };

        struct PVPMatchStatistics
        {
            struct RatingData
            {
                RatingData() { } // work around clang bug https://gcc.gnu.org/bugzilla/show_bug.cgi?id=101227

                int32 Prematch[2] = { };
                int32 Postmatch[2] = { };
                int32 PrematchMMR[2] = { };
            };

            struct HonorData
            {
                HonorData() { } // work around clang bug https://gcc.gnu.org/bugzilla/show_bug.cgi?id=101227

                uint32 HonorKills = 0;
                uint32 Deaths = 0;
                uint32 ContributionPoints = 0;
            };

            struct PVPMatchPlayerPVPStat
            {
                PVPMatchPlayerPVPStat() : PvpStatID(0), PvpStatValue(0) { }
                PVPMatchPlayerPVPStat(int32 pvpStatID, int32 pvpStatValue) : PvpStatID(pvpStatID), PvpStatValue(pvpStatValue) { }

                int32 PvpStatID;
                int32 PvpStatValue;
            };

            struct PVPMatchPlayerStatistics
            {
                ObjectGuid PlayerGUID;
                uint32 Kills = 0;
                uint8 Faction = 0;
                bool IsInWorld = false;
                Optional<HonorData> Honor;
                uint32 DamageDone = 0;
                uint32 HealingDone = 0;
                Optional<uint32> PreMatchRating;
                Optional<int32> RatingChange;
                Optional<uint32> PreMatchMMR;
                Optional<int32> MmrChange;
                Optional<uint32> PostMatchMMR;
                std::vector<PVPMatchPlayerPVPStat> Stats;
                int32 PrimaryTalentTree = 0;
                int8 Sex = 0;
                int32 Race = 0;
                int32 Class = 0;
                int32 CreatureID = 0;
                int32 HonorLevel = 0;
                int32 Role = 0;
            };

            std::vector<PVPMatchPlayerStatistics> Statistics;
            Optional<RatingData> Ratings;
            std::array<int8, 2> PlayerCount = { };
        };

        class PVPMatchStatisticsMessage final : public ServerPacket
        {
        public:
            PVPMatchStatisticsMessage() : ServerPacket(SMSG_PVP_MATCH_STATISTICS, 0) { }

            WorldPacket const* Write() override;

            PVPMatchStatistics Data;
        };

        struct BattlefieldStatusHeader
        {
            WorldPackets::LFG::RideTicket Ticket;
            std::vector<uint64> QueueID;
            uint8 RangeMin = 0;
            uint8 RangeMax = 0;
            uint8 TeamSize = 0;
            uint32 InstanceID = 0;
            bool RegisteredMatch = false;
            bool TournamentRules = false;
        };

        class BattlefieldStatusNone final : public ServerPacket
        {
        public:
            BattlefieldStatusNone() : ServerPacket(SMSG_BATTLEFIELD_STATUS_NONE, 16 + 4 + 4 + 4) { }

            WorldPacket const* Write() override;

            WorldPackets::LFG::RideTicket Ticket;
        };

        class BattlefieldStatusNeedConfirmation final : public ServerPacket
        {
        public:
            BattlefieldStatusNeedConfirmation() : ServerPacket(SMSG_BATTLEFIELD_STATUS_NEED_CONFIRMATION, 4 + 4 + sizeof(BattlefieldStatusHeader) + 1) { }

            WorldPacket const* Write() override;

            uint32 Timeout = 0;
            uint32 Mapid = 0;
            BattlefieldStatusHeader Hdr;
            uint8 Role = 0;
        };

        class BattlefieldStatusActive final : public ServerPacket
        {
        public:
            BattlefieldStatusActive() : ServerPacket(SMSG_BATTLEFIELD_STATUS_ACTIVE, sizeof(BattlefieldStatusHeader) + 4 + 1 + 1 + 4 + 4) { }

            WorldPacket const* Write() override;

            BattlefieldStatusHeader Hdr;
            uint32 ShutdownTimer = 0;
            uint8 ArenaFaction = 0;
            bool LeftEarly = false;
            uint32 StartTimer = 0;
            uint32 Mapid = 0;
        };

        class BattlefieldStatusQueued final : public ServerPacket
        {
        public:
            BattlefieldStatusQueued() : ServerPacket(SMSG_BATTLEFIELD_STATUS_QUEUED, 4 + sizeof(BattlefieldStatusHeader) + 1 + 1 + 1 + 4) { }

            WorldPacket const* Write() override;

            uint32 AverageWaitTime = 0;
            BattlefieldStatusHeader Hdr;
            bool AsGroup = false;
            bool SuspendedQueue = false;
            bool EligibleForMatchmaking = false;
            uint32 WaitTime = 0;
            int32 Unused920 = 0;
        };

        class BattlefieldStatusFailed final : public ServerPacket
        {
        public:
            BattlefieldStatusFailed() : ServerPacket(SMSG_BATTLEFIELD_STATUS_FAILED, 8 + 16 + 4 + 16 + 4 + 4 + 4) { }

            WorldPacket const* Write() override;

            uint64 QueueID = 0;
            ObjectGuid ClientID;
            int32 Reason = 0;
            WorldPackets::LFG::RideTicket Ticket;
        };

        class BattlemasterJoin final : public ClientPacket
        {
        public:
            BattlemasterJoin(WorldPacket&& packet) : ClientPacket(CMSG_BATTLEMASTER_JOIN, std::move(packet)) { }

            void Read() override;

            Array<uint64, 1> QueueIDs;
            uint8 Roles = 0;
            int32 BlacklistMap[2] = { };
        };

        class BattlemasterJoinArena final : public ClientPacket
        {
        public:
            BattlemasterJoinArena(WorldPacket&& packet) : ClientPacket(CMSG_BATTLEMASTER_JOIN_ARENA, std::move(packet)) { }

            void Read() override;

            uint8 TeamSizeIndex = 0;
            uint8 Roles = 0;
        };

        class BattlefieldLeave final : public ClientPacket
        {
        public:
            BattlefieldLeave(WorldPacket&& packet) : ClientPacket(CMSG_BATTLEFIELD_LEAVE, std::move(packet)) { }

            void Read() override { }
        };

        class BattlefieldPort final : public ClientPacket
        {
        public:
            BattlefieldPort(WorldPacket&& packet) : ClientPacket(CMSG_BATTLEFIELD_PORT, std::move(packet)) { }

            void Read() override;

            WorldPackets::LFG::RideTicket Ticket;
            bool AcceptedInvite = false;
        };

        class BattlefieldListRequest final : public ClientPacket
        {
        public:
            BattlefieldListRequest(WorldPacket&& packet) : ClientPacket(CMSG_BATTLEFIELD_LIST, std::move(packet)) { }

            void Read() override;

            int32 ListID = 0;
        };

        class BattlefieldList final : public ServerPacket
        {
        public:
            BattlefieldList() : ServerPacket(SMSG_BATTLEFIELD_LIST, 1 + 1 + 16 + 1 + 1 + 1 + 4 + 1 + 4) { }

            WorldPacket const* Write() override;

            ObjectGuid BattlemasterGuid;
            int32 BattlemasterListID = 0;
            uint8 MinLevel = 0;
            uint8 MaxLevel = 0;
            std::vector<int32> Battlefields;    // Players cannot join a specific battleground instance anymore - this is always empty
            bool PvpAnywhere = false;
            bool HasRandomWinToday = false;
        };

        class GetPVPOptionsEnabled final : public ClientPacket
        {
        public:
            GetPVPOptionsEnabled(WorldPacket&& packet) : ClientPacket(CMSG_GET_PVP_OPTIONS_ENABLED, std::move(packet)) { }

            void Read() override { }
        };

        class PVPOptionsEnabled final : public ServerPacket
        {
        public:
            PVPOptionsEnabled() : ServerPacket(SMSG_PVP_OPTIONS_ENABLED, 1) { }

            WorldPacket const* Write() override;

            bool WargameArenas = false;
            bool RatedArenas = false;
            bool WargameBattlegrounds = false;
            bool ArenaSkirmish = false;
            bool PugBattlegrounds = false;
            bool RatedBattlegrounds = false;
        };

        class RequestBattlefieldStatus final : public ClientPacket
        {
        public:
            RequestBattlefieldStatus(WorldPacket&& packet) : ClientPacket(CMSG_REQUEST_BATTLEFIELD_STATUS, std::move(packet)) { }

            void Read() override { }
        };

        class ReportPvPPlayerAFK final : public ClientPacket
        {
        public:
            ReportPvPPlayerAFK(WorldPacket&& packet) : ClientPacket(CMSG_REPORT_PVP_PLAYER_AFK, std::move(packet)) { }

            void Read() override;

            ObjectGuid Offender;
        };

        class ReportPvPPlayerAFKResult final : public ServerPacket
        {
        public:
            ReportPvPPlayerAFKResult() : ServerPacket(SMSG_REPORT_PVP_PLAYER_AFK_RESULT, 16 + 1 + 1 + 1) { }

            WorldPacket const* Write() override;

            enum ResultCode : uint8
            {
                PVP_REPORT_AFK_SUCCESS = 0,
                PVP_REPORT_AFK_GENERIC_FAILURE = 1, // there are more error codes but they are impossible to receive without modifying the client
                PVP_REPORT_AFK_SYSTEM_ENABLED = 5,
                PVP_REPORT_AFK_SYSTEM_DISABLED = 6
            };

            ObjectGuid Offender;
            uint8 NumPlayersIHaveReported = 0;
            uint8 NumBlackMarksOnOffender = 0;
            uint8 Result = PVP_REPORT_AFK_GENERIC_FAILURE;
        };

        struct BattlegroundPlayerPosition
        {
            ObjectGuid Guid;
            TaggedPosition<Position::XY> Pos;
            int8 IconID = 0;
            int8 ArenaSlot = 0;
        };

        class BattlegroundPlayerPositions final : public ServerPacket
        {
        public:
            BattlegroundPlayerPositions() : ServerPacket(SMSG_BATTLEGROUND_PLAYER_POSITIONS, 4) { }

            WorldPacket const* Write() override;

            std::vector<BattlegroundPlayerPosition> FlagCarriers;
        };

        class BattlegroundPlayerJoined final : public ServerPacket
        {
        public:
            BattlegroundPlayerJoined() : ServerPacket(SMSG_BATTLEGROUND_PLAYER_JOINED, 16) { }

            WorldPacket const* Write() override;

            ObjectGuid Guid;
        };

        class BattlegroundPlayerLeft final : public ServerPacket
        {
        public:
            BattlegroundPlayerLeft() : ServerPacket(SMSG_BATTLEGROUND_PLAYER_LEFT, 16) { }

            WorldPacket const* Write() override;

            ObjectGuid Guid;
        };

        class DestroyArenaUnit final : public ServerPacket
        {
        public:
            DestroyArenaUnit() : ServerPacket(SMSG_DESTROY_ARENA_UNIT, 16) { }

            WorldPacket const* Write() override;

            ObjectGuid Guid;
        };

        class RequestPVPRewards final : public ClientPacket
        {
        public:
            RequestPVPRewards(WorldPacket&& packet) : ClientPacket(CMSG_REQUEST_PVP_REWARDS, std::move(packet)) { }

            void Read() override { }
        };

        class RequestRatedPvpInfo final : public ClientPacket
        {
        public:
            RequestRatedPvpInfo(WorldPacket&& packet) : ClientPacket(CMSG_REQUEST_RATED_PVP_INFO, std::move(packet)) { }

            void Read() override { }
        };

        class RatedPvpInfo final : public ServerPacket
        {
        public:
            RatedPvpInfo() : ServerPacket(SMSG_RATED_PVP_INFO, 7 * sizeof(BracketInfo)) { }

            WorldPacket const* Write() override;

            struct BracketInfo
            {
                uint32 PersonalRating = 0;
                uint32 Ranking = 0;
                uint32 SeasonPlayed = 0;
                uint32 SeasonWon = 0;
                uint32 Unused1 = 0;
                uint32 Unused2 = 0;
                uint32 WeeklyPlayed = 0;
                uint32 WeeklyWon = 0;
                uint32 RoundsSeasonPlayed = 0;
                uint32 RoundsSeasonWon = 0;
                uint32 RoundsWeeklyPlayed = 0;
                uint32 RoundsWeeklyWon = 0;
                uint32 BestWeeklyRating = 0;
                uint32 LastWeeksBestRating = 0;
                uint32 BestSeasonRating = 0;
                uint32 PvpTierID = 0;
                uint32 Unused3 = 0;
                uint32 Unused4 = 0;
                uint32 Rank = 0;
                bool Disqualified = false;
            } Bracket[7];
        };

        struct RatedMatchDeserterPenalty
        {
            int32 PersonalRatingChange = 0;
            int32 QueuePenaltySpellID = 0;
            WorldPackets::Duration<Milliseconds, int32> QueuePenaltyDuration;
        };

        enum class PVPMatchState : uint8
        {
            Waiting = 0,
            StartUp = 1,
            Engaged = 2,
            PostRound = 3,
            Inactive = 4,
            Complete = 5
        };

        class PVPMatchInitialize final : public ServerPacket
        {
        public:
            PVPMatchInitialize() : ServerPacket(SMSG_PVP_MATCH_INITIALIZE, 4 + 1 + 4 + 4 + 1 + 4 + 1) { }

            WorldPacket const* Write() override;

            uint32 MapID = 0;
            PVPMatchState State = PVPMatchState::Inactive;
            Timestamp<> StartTime;
            WorldPackets::Duration<Seconds> Duration;
            Optional<RatedMatchDeserterPenalty> DeserterPenalty;
            uint8 ArenaFaction = 0;
            uint32 BattlemasterListID = 0;
            bool Registered = false;
            bool AffectsRating = false;
            bool HasRatedMatchDeserterPenalty = false;
        };

        class PVPMatchSetState final : public ServerPacket
        {
        public:
            explicit PVPMatchSetState(PVPMatchState state) : ServerPacket(SMSG_PVP_MATCH_SET_STATE, 1), State(state) { }

            WorldPacket const* Write() override;

            PVPMatchState State;
        };

        class PVPMatchComplete final : public ServerPacket
        {
        public:
            PVPMatchComplete() : ServerPacket(SMSG_PVP_MATCH_COMPLETE) { }

            WorldPacket const* Write() override;

            uint8 Winner = 0;
            WorldPackets::Duration<Seconds> Duration;
            Optional<PVPMatchStatistics> LogData;
            uint32 SoloShuffleStatus = 0;
        };

        enum class BattlegroundCapturePointState : uint8
        {
            Neutral             = 1,
            ContestedHorde      = 2,
            ContestedAlliance   = 3,
            HordeCaptured       = 4,
            AllianceCaptured    = 5
        };

        struct BattlegroundCapturePointInfo
        {
            ObjectGuid Guid;
            TaggedPosition<Position::XY> Pos;
            BattlegroundCapturePointState State = BattlegroundCapturePointState::Neutral;
            Timestamp<> CaptureTime;
            Duration<Milliseconds, uint32> CaptureTotalDuration;
        };

        class UpdateCapturePoint final : public ServerPacket
        {
        public:
            UpdateCapturePoint() : ServerPacket(SMSG_UPDATE_CAPTURE_POINT) { }

            WorldPacket const* Write() override;

            BattlegroundCapturePointInfo CapturePointInfo;
        };

        class CapturePointRemoved final : public ServerPacket
        {
        public:
            CapturePointRemoved() : ServerPacket(SMSG_CAPTURE_POINT_REMOVED) { }
            CapturePointRemoved(ObjectGuid capturePointGUID) : ServerPacket(SMSG_CAPTURE_POINT_REMOVED), CapturePointGUID(capturePointGUID) { }

            WorldPacket const* Write() override;

            ObjectGuid CapturePointGUID;
        };



        // DekkCore >
        class WargameRequestSuccessfullySentToOpponent final : public ServerPacket
        {
        public:
            WargameRequestSuccessfullySentToOpponent() : ServerPacket(SMSG_WARGAME_REQUEST_SUCCESSFULLY_SENT_TO_OPPONENT, 6) { }

            WorldPacket const* Write() override;

            ObjectGuid opponentguid;
        };

        class ArenaPrepOpponentSpecializations final : public ServerPacket
        {
        public:
            ArenaPrepOpponentSpecializations() : ServerPacket(SMSG_ARENA_PREP_OPPONENT_SPECIALIZATIONS) { }

            WorldPacket const* Write() override;

            struct OpponentSpecData
            {
                int32 SpecializationID = 0;
                int32 Unk = 0;
                ObjectGuid Guid;
            };

            std::vector<OpponentSpecData> Data;
        };

        class BattlegroundPoints final : public ServerPacket
        {
        public:
            BattlegroundPoints() : ServerPacket(SMSG_BATTLEGROUND_POINTS, 3) { }

            WorldPacket const* Write() override;

            uint16 BgPoints = 0;
            bool Team = false; // 0 - alliance; 1 - horde
        };

        class AcceptWargameInvite final : public ClientPacket
        {
        public:
            AcceptWargameInvite(WorldPacket&& packet) : ClientPacket(CMSG_ACCEPT_WARGAME_INVITE, std::move(packet)) { }

            void Read() override;

            ObjectGuid OpposingPartyMember;
            int64 QueueID = 0;
            bool Accept = false;
        };

        class RequestConquestFormulaConstants final : public ClientPacket
        {
        public:
            RequestConquestFormulaConstants(WorldPacket&& packet) : ClientPacket(CMSG_REQUEST_CONQUEST_FORMULA_CONSTANTS, std::move(packet)) { }

            void Read() override { }
        };

        class ConquestFormulaConstants final : public ServerPacket
        {
        public:
            ConquestFormulaConstants() : ServerPacket(SMSG_CONQUEST_FORMULA_CONSTANTS, 20) { }

            WorldPacket const* Write() override;

            uint32 PvpMinCPPerWeek = 0;
            uint32 PvpMaxCPPerWeek = 0;
            float PvpCPBaseCoefficient = 0.0f;
            float PvpCPExpCoefficient = 0.0f;
            float PvpCPNumerator = 0.0f;
        };

        struct BattlegroundCapturePointInfoData
        {
            ObjectGuid Guid;
            TaggedPosition<Position::XY> Pos;
            uint32 CaptureTime = 0;
            uint32 CaptureTotalDuration = 0;
            int8 NodeState;
        };

        class MapObjectivesInit final : public ServerPacket
        {
        public:
            MapObjectivesInit() : ServerPacket(SMSG_MAP_OBJECTIVES_INIT, 25) { }

            WorldPacket const* Write() override;

            std::vector<BattlegroundCapturePointInfoData> CapturePointInfo;
        };

        class CheckWargameEntry final : public ServerPacket
        {
        public:
            CheckWargameEntry() : ServerPacket(SMSG_CHECK_WARGAME_ENTRY, 16 + 8 + 8 + 4 + 1 + 1) { }

            WorldPacket const* Write() override;

            ObjectGuid OpposingPartyBnetAccountID;
            ObjectGuid OpposingPartyMember;
            uint64 QueueID = 0;
            uint32 TimeoutSeconds = 0;
            uint32 RealmID = 0;
            uint16 UnkShort = 0;
            uint8 OpposingPartyUserServer = 0;
            bool TournamentRules = false;
        };

        class StatusWaitForGroups final : public ServerPacket
        {
        public:
            StatusWaitForGroups() : ServerPacket(SMSG_BATTLEFIELD_STATUS_WAIT_FOR_GROUPS, 12) { }

            WorldPacket const* Write() override;

            BattlefieldStatusHeader Header;
            uint32 Mapid = 0;
            uint32 Timeout = 0;
            uint8 TotalPlayers[TEAM_MAX] = { };
            uint8 AwaitingPlayers[TEAM_MAX] = { };
        };

        //< SMSG_BATTLEGROUND_INFO_THROTTLED
        class NullSMsg final : public ServerPacket
        {
        public:
            NullSMsg(OpcodeServer opcode) : ServerPacket(opcode) { }

            WorldPacket const* Write() override { return &_worldPacket; }
        };

        class Init final : public ServerPacket
        {
        public:
            Init(uint16 maxPoints) : ServerPacket(SMSG_BATTLEGROUND_INIT, 2 + 4), MaxPoints(maxPoints) { }

            WorldPacket const* Write() override;

            uint32 ServerTime = getMSTime();
            uint16 MaxPoints = 0;
        };

        class BattlemasterJoinBrawl final : public ClientPacket
        {
        public:
            BattlemasterJoinBrawl(WorldPacket && packet) : ClientPacket(CMSG_BATTLEMASTER_JOIN_BRAWL, std::move(packet)) { }

            void Read() override;

            uint8 RolesMask = 0;
        };

        class JoinSkirmish final : public ClientPacket
        {
        public:
            JoinSkirmish(WorldPacket && packet) : ClientPacket(CMSG_BATTLEMASTER_JOIN_SKIRMISH, std::move(packet)) { }

            void Read() override;

            uint8 Roles = 0;
            uint8 Bracket = 0;
            bool JoinAsGroup = false;
            bool UnkBool = false;
         };


        class BattlemasterJoinRatedBG final : public ClientPacket
        {
        public:
            BattlemasterJoinRatedBG(WorldPacket&& packet) : ClientPacket(CMSG_JOIN_RATED_BATTLEGROUND, std::move(packet)) { }

            void Read() override { }
        };

        class SendPvpBrawlInfo final : public ServerPacket
        {
        public:
            SendPvpBrawlInfo() : ServerPacket(SMSG_REQUEST_SCHEDULED_PVP_INFO_RESPONSE) { }

            WorldPacket const* Write() override;

            int32 PvpBrawlID = 0;
            int32 TimeToBrawl = 0;
            bool IsActive = true;
            uint8 Bracket;
            int32 UNK;
            uint8 UNK1;
            int32 UNK2;
            bool UNK3;
        };

        class RequestScheduledPVPInfo final : public ClientPacket
        {
        public:
            RequestScheduledPVPInfo(WorldPacket&& packet) : ClientPacket(CMSG_REQUEST_SCHEDULED_PVP_INFO, std::move(packet)) { }

            void Read() override { }
        };

        class MapObjEvents final : public ServerPacket
        {
        public:
            MapObjEvents() : ServerPacket(SMSG_MAP_OBJ_EVENTS, 12) { }

            WorldPacket const* Write() override;

            uint32 UniqueID = 0;
            uint32 DataSize = 0;
            std::vector<uint8> Unk2;
        };

        class BattlemasterJoinRatedSoloShuffle final : public ClientPacket
        {
        public:
            BattlemasterJoinRatedSoloShuffle(WorldPacket&& packet) : ClientPacket(CMSG_BATTLEMASTER_JOIN_RATED_SOLO_SHUFFLE, std::move(packet)) { }

            void Read() override;

            uint8 RolesMask;
        };
        
        // < DekkCore
    }
}

#endif // BattlegroundPackets_h__
