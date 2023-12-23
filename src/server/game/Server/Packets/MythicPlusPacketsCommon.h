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

#ifndef MythicPlusPacketsCommon_h__
#define MythicPlusPacketsCommon_h__

#include "ObjectGuid.h"
#include "PacketUtilities.h"
#include "Packet.h"

namespace WorldPackets
{
    namespace MythicPlus
    {
        struct DungeonScoreMapSummary
        {
            int32 ChallengeModeID = 0;
            float MapScore = 0.0f;
            int32 BestRunLevel = 0;
            int32 BestRunDurationMS = 0;
            bool FinishedSuccess = false;
        };

        struct DungeonScoreSummary
        {
            float OverallScoreCurrentSeason = 0.0f;
            float LadderScoreCurrentSeason = 0.0f;
            std::vector<DungeonScoreMapSummary> Runs;
        };

        struct MythicPlusMember
        {
            ObjectGuid BnetAccountGUID;
            uint64 GuildClubMemberID = 0;
            ObjectGuid GUID;
            ObjectGuid GuildGUID;
            uint32 NativeRealmAddress = 0;
            uint32 VirtualRealmAddress = 0;
            int32 ChrSpecializationID = 0;
            int16 RaceID = 0;
            int32 ItemLevel = 0;
            int32 CovenantID = 0;
            int32 SoulbindID = 0;
        };

        struct MythicPlusRun
        {
            int32 MapChallengeModeID = 0;
            bool Completed = false;
            uint32 Level = 0;
            int32 DurationMs = 0;
            Timestamp<> StartDate;
            Timestamp<> CompletionDate;
            int32 Season = 9;
            std::vector<MythicPlusMember> Members;
            float RunScore = 0.0f;
            std::array<int32, 4> KeystoneAffixIDs;
        };

        struct DungeonScoreBestRunForAffix
        {
            int32 KeystoneAffixID = 0;
            MythicPlusRun Run;
            float Score = 0.0f;
        };

        struct DungeonScoreMapData
        {
            int32 MapChallengeModeID = 0;
            std::vector<DungeonScoreBestRunForAffix> BestRuns;
            float OverAllScore = 0.0f;

        };

        struct DungeonScoreSeasonData
        {
            int32 Season = 9;
            std::vector<DungeonScoreMapData> SeasonMaps;
            std::vector<DungeonScoreMapData> LadderMaps;
            float SeasonScore = 0.0f;
            float LadderScore = 0.0f;
        };

        struct DungeonScoreData
        {
            std::vector<DungeonScoreSeasonData> Seasons;
            int32 TotalRuns = 0;
        };


        struct MythicPlusReward
        {
            uint32 Unk1;
            uint32 Unk2;
            uint64 Unk3;
            uint64 Unk4;
            uint64 Unk5;
            bool UnknownBool;
        };

        ByteBuffer& operator<<(ByteBuffer& data, DungeonScoreSummary const& dungeonScoreSummary);
        ByteBuffer& operator<<(ByteBuffer& data, DungeonScoreData const& dungeonScoreData);

        class MythicPlusSeasonData final : public ClientPacket
             {
        public:
            MythicPlusSeasonData(WorldPacket && packet) : ClientPacket(CMSG_REQUEST_MYTHIC_PLUS_SEASON_DATA, std::move(packet)) { }

                void Read() override;
         };

        class MythicPlusRequestMapStatsResult final : public ServerPacket
        {
        public:
            MythicPlusRequestMapStatsResult() : ServerPacket(SMSG_MYTHIC_PLUS_ALL_MAP_STATS) { }

            WorldPacket const* Write() override;

            uint32 RunCount = 0;
            uint32 RewardCount = 0;
            uint32 Season = 9;
            uint32 Subseason = 0;
            std::vector<MythicPlusRun> mythicPlusRuns;
            std::vector<MythicPlusReward> mythicPlusRewards;
        };

        class MythicPlusCurrentAffixes final : public ClientPacket
        {
        public:
            MythicPlusCurrentAffixes(WorldPacket&& packet) : ClientPacket(CMSG_REQUEST_MYTHIC_PLUS_AFFIXES, std::move(packet)) { }

            void Read() override;
        };

        class MythicPlusSeasonDataResult final : public ServerPacket
        {
        public:
            MythicPlusSeasonDataResult() : ServerPacket(SMSG_MYTHIC_PLUS_SEASON_DATA) { }

            WorldPacket const* Write() override;

            uint8 IsMythicPlusActive = 128;//Hack fix
        };

        class MythicPlusRequestMapStats final : public ClientPacket
        {
        public:
            MythicPlusRequestMapStats(WorldPacket&& packet) : ClientPacket(CMSG_MYTHIC_PLUS_REQUEST_MAP_STATS, std::move(packet)) { }//15

            void Read() override;

            ObjectGuid BnetAccountGUID;
            int64 MapChallengeModeID = 0;
        };

        class MythicPlusRequestWeeklyRewards final : public ClientPacket
        {
        public:
            MythicPlusRequestWeeklyRewards(WorldPacket&& packet) : ClientPacket(CMSG_REQUEST_WEEKLY_REWARDS, std::move(packet)) { }//0

            void Read() override;
        };

        class MythicPlusCurrentAffixesResult final : public ServerPacket
        {
        public:
            MythicPlusCurrentAffixesResult() : ServerPacket(SMSG_MYTHIC_PLUS_CURRENT_AFFIXES) { }

            WorldPacket const* Write() override;

            uint32 Count = 4;
            std::array<uint32, 4> Affixes{ 0, 0, 0, 0};
            std::array<uint32, 4> RequiredSeason{ 0, 0, 0, 0};
        };

        struct ItemReward
        {
            uint32 ItemID = 0;
            uint32 ItemDisplayID = 0;
            uint32 Quantity = 0;
        };

        struct CurrencyReward
        {
            CurrencyReward(uint32 ID, uint32 count) : CurrencyID(ID), Quantity(count) { }

            uint32 CurrencyID = 0;
            uint32 Quantity = 0;
        };

        struct MapChallengeModeReward
        {
            struct ChallengeModeReward
            {
                uint32 Money = 0;
                std::vector<CurrencyReward> CurrencyRewards;
            };

            uint32 MapId = 0;
            std::vector<ChallengeModeReward> Rewards;
        };

        struct ModeAttempt
        {
            struct Member
            {
                ObjectGuid Guid;
                uint32 VirtualRealmAddress = 0;
                uint32 NativeRealmAddress = 0;
                uint32 SpecializationID = 0;
            };

            uint32 InstanceRealmAddress = 0;
            uint32 AttemptID = 0;
            uint32 CompletionTime = 0;
            time_t CompletionDate = time(nullptr);
            uint32 MedalEarned = 0;
            std::vector<Member> Members;
        };

        struct ChallengeModeMap
        {
            struct bMember
            {
                ObjectGuid PlayerGuid;
                ObjectGuid GuildGuid;
                uint32 VirtualRealmAddress = 0;
                uint32 NativeRealmAddress = 0;
                uint32 SpecializationID = 0;
                uint32 Unk4 = 0;
                uint32 EquipmentLevel = 0;
            };

            uint32 MapId = 0;
            uint32 BestCompletionMilliseconds = 0;
            uint32 LastCompletionMilliseconds = 0;
            uint32 CompletedChallengeLevel = 0;
            uint32 ChallengeID = 0;
            time_t BestMedalDate = time(nullptr);
            std::vector<uint16> BestSpecID;
            std::array<uint32, 4> Affixes;
        };

        class Rewards final : public ServerPacket
        {
        public:
            Rewards() : ServerPacket(SMSG_WEEKLY_REWARDS_PROGRESS_RESULT, 8) { }

            WorldPacket const* Write() override;

            std::vector<MapChallengeModeReward> MapChallengeModeRewards;
            std::vector<ItemReward> ItemRewards;
        };
    }
}

#endif // MythicPlusPacketsCommon_h__
