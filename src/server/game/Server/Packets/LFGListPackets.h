/*
 * Copyright (C) 2008-2022 TrinityCore <http://www.trinitycore.org/>
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

#ifndef LFGListPackets_h__
#define LFGListPackets_h__

#include "Packet.h"
#include "PacketUtilities.h"
#include "ItemPacketsCommon.h"
#include "LFGPacketsCommon.h"
#include "Optional.h"
#include "LFGPackets.h"
#include "SharedDefines.h"

namespace WorldPackets
{
    namespace LFGList
    {
        struct LFGListBlacklist
        {
            uint32 ActivityID = 0;
            uint32 Reason = 0;
        };

        struct ApplicationToGroup
        {
            LFG::RideTicket ApplicationTicket;
            uint32 ActivityID = 0;
            std::string Comment;
            uint8 Role = 0;
        };

        struct ListRequest
        {
            ListRequest() { }

            Optional<uint32> QuestID;
            int32 ActivityID = 0;
            float ItemLevel = 0.0f;
            uint32 HonorLevel = 0;
            std::string GroupName;
            std::string Comment;
            std::string VoiceChat;
            bool minChallege = false;
            bool PrivateGroup = false;
            bool HasQuest = false;
            bool AutoAccept = false;
            float TypeActivity = 0.0f;
            uint32 MinMyticPlusRating = 0;
        };

        struct MemberInfo
        {
            MemberInfo() { }
            MemberInfo(uint8 classID, uint8 role) : ClassID(classID), Role(role) { }

            uint8 ClassID = CLASS_NONE;
            uint8 Role = 0;
        };

        struct ListSearchResult
        {
            LFG::RideTicket ApplicationTicket;
            ListRequest JoinRequest;
            std::vector<MemberInfo> Members;
            GuidList BNetFriendsGuids;
            GuidList NumCharFriendsGuids;
            GuidList NumGuildMateGuids;
            ObjectGuid LastTouchedVoiceChat;
            ObjectGuid PartyGUID;
            ObjectGuid BNetFriends;
            ObjectGuid CharacterFriends;
            ObjectGuid GuildMates;
            uint32 VirtualRealmAddress = 0;
            uint32 CompletedEncounters = 0;
            uint32 Age = 0;
            uint32 ResultID = 0;
            uint8 ApplicationStatus = 0;
        };

        struct ApplicantStruct
        {
            ApplicantStruct() { }
            ApplicantStruct(ObjectGuid playerGUID, uint8 role) : PlayerGUID(playerGUID), Role(role) { }

            ObjectGuid PlayerGUID;
            uint8 Role = 0;
        };

        struct ApplicantMember
        {
            ApplicantMember() { }

            struct ACStatInfo
            {
                uint32 UnkInt4 = 0;
                uint32 UnkInt5 = 0;
            };

            std::list<ACStatInfo> AcStat;
            ObjectGuid PlayerGUID;
            uint32 VirtualRealmAddress = 0;
            uint32 Level = 0;
            uint32 HonorLevel = 0;
            float ItemLevel = 0.0f;
            uint8 PossibleRoleMask = 0;
            uint8 SelectedRoleMask = 0;
        };

        struct ApplicantInfo
        {
            std::vector<ApplicantMember> Member;
            LFG::RideTicket ApplicantTicket;
            ObjectGuid ApplicantPartyLeader;
            std::string Comment;
            uint8 ApplicationStatus = 0;
            bool Listed = false;
        };

        class LFGListApplyToGroup final : public ClientPacket
        {
        public:
            LFGListApplyToGroup(WorldPacket&& packet) : ClientPacket(CMSG_LFG_LIST_APPLY_TO_GROUP, std::move(packet)) { }

            void Read() override;

            ApplicationToGroup application;
        };

        class LFGListCancelApplication final : public ClientPacket
        {
        public:
            LFGListCancelApplication(WorldPacket&& packet) : ClientPacket(CMSG_LFG_LIST_CANCEL_APPLICATION, std::move(packet)) { }

            void Read() override;

            LFG::RideTicket ApplicantTicket;
        };

        class LFGListClubFinderRequestPendingClubList final : public ClientPacket
        {
        public:
            LFGListClubFinderRequestPendingClubList(WorldPacket&& packet) : ClientPacket(CMSG_CLUB_FINDER_REQUEST_PENDING_CLUBS_LIST, std::move(packet)) { }//1

            void Read() override;

            bool Queued = false;
        };

        class LFGListDeclineApplicant final : public ClientPacket
        {
        public:
            LFGListDeclineApplicant(WorldPacket&& packet) : ClientPacket(CMSG_LFG_LIST_DECLINE_APPLICANT, std::move(packet)) { }

            void Read() override;

            LFG::RideTicket ApplicantTicket;
            LFG::RideTicket ApplicationTicket;
        };

        class LFGListInviteApplicant final : public ClientPacket
        {
        public:
            LFGListInviteApplicant(WorldPacket&& packet) : ClientPacket(CMSG_LFG_LIST_INVITE_APPLICANT, std::move(packet)) { }

            void Read() override;

            std::list<ApplicantStruct> Applicant;
            LFG::RideTicket ApplicantTicket;
            LFG::RideTicket ApplicationTicket;
        };

        class LFGListUpdateRequest final : public ClientPacket
        {
        public:
            LFGListUpdateRequest(WorldPacket&& packet) : ClientPacket(CMSG_LFG_LIST_UPDATE_REQUEST, std::move(packet)) { }

            void Read() override;

            LFG::RideTicket Ticket;
            ListRequest UpdateRequest;
        };

        class LFGListGetStatus final : public ClientPacket
        {
        public:
            LFGListGetStatus(WorldPacket&& packet) : ClientPacket(CMSG_LFG_LIST_GET_STATUS, std::move(packet)) { }

            void Read() override { }
        };

        class LFGListInviteResponse final : public ClientPacket
        {
        public:
            LFGListInviteResponse(WorldPacket&& packet) : ClientPacket(CMSG_LFG_LIST_INVITE_RESPONSE, std::move(packet)) { }

            void Read() override;

            LFG::RideTicket ApplicantTicket;
            bool Accept = false;
        };

        class LFGListJoin final : public ClientPacket
        {
        public:
            LFGListJoin(WorldPacket&& packet) : ClientPacket(CMSG_LFG_LIST_JOIN, std::move(packet)) { }

            void Read() override;

            ListRequest Request;
        };

        class LFGListLeave final : public ClientPacket
        {
        public:
            LFGListLeave(WorldPacket&& packet) : ClientPacket(CMSG_LFG_LIST_LEAVE, std::move(packet)) { }

            void Read() override;

            LFG::RideTicket ApplicationTicket;
        };

        class LFGListSearch final : public ClientPacket
        {
        public:
            LFGListSearch(WorldPacket&& packet) : ClientPacket(CMSG_LFG_LIST_SEARCH, std::move(packet)) { }

            void Read() override;

            std::vector<LFGListBlacklist> Blacklist;
            GuidVector Guids;
            int32 CategoryID = 0;
            int32 SearchTerms = 0;
            int32 Filter = 0;
            int32 PreferredFilters = 0;
            std::string LanguageSearchFilter;
        };

        class RequestLFGListBlacklist final : public ClientPacket
        {
        public:
            RequestLFGListBlacklist(WorldPacket&& packet) : ClientPacket(CMSG_REQUEST_LFG_LIST_BLACKLIST, std::move(packet)) { }

            void Read() override { }
        };

        class LFGListApplicationUpdate final : public ServerPacket
        {
        public:
            LFGListApplicationUpdate() : ServerPacket(SMSG_LFG_LIST_APPLICANT_LIST_UPDATE, 4 + 4 + 4) { }

            WorldPacket const* Write() override;

            std::vector<ApplicantInfo> Applicants;
            LFG::RideTicket ApplicationTicket;
            uint32 UnkInt = 0;
        };

        class LFGListApplyToGroupResponce final : public ServerPacket
        {
        public:
            LFGListApplyToGroupResponce() : ServerPacket(SMSG_LFG_LIST_APPLY_TO_GROUP_RESULT, 28 + 28 + 4 + 4 + 1 + 1 + 150) { }

            WorldPacket const* Write() override;

            ListSearchResult SearchResult;
            LFG::RideTicket ApplicantTicket;
            LFG::RideTicket ApplicationTicket;
            uint32 InviteExpireTimer = 0;
            uint8 Status = 0;
            uint8 Role = 0;
            uint8 ApplicationStatus = 0;
        };

        class LFGListJoinResult final : public ServerPacket
        {
        public:
            LFGListJoinResult() : ServerPacket(SMSG_LFG_LIST_JOIN_RESULT, 28 + 1 + 1) { }

            WorldPacket const* Write() override;

            LFG::RideTicket ApplicationTicket;
            uint8 Status = 0;
            uint8 Result = 0;
        };

        class LFGListSearchResults final : public ServerPacket
        {
        public:
            LFGListSearchResults() : ServerPacket(SMSG_LFG_LIST_SEARCH_RESULTS, 6) { }

            WorldPacket const* Write() override;

            std::vector<ListSearchResult> SearchResults;
            uint16 AppicationsCount = 0;
        };

        class LFGListSearchStatus final : public ServerPacket
        {
        public:
            LFGListSearchStatus() : ServerPacket(SMSG_LFG_LIST_SEARCH_STATUS, 30) { }

            WorldPacket const* Write() override;

            LFG::RideTicket Ticket;
            uint8 Status = 0;
            bool UnkBit = false;
        };

        class LFGListUpdateBlacklist final : public ServerPacket
        {
        public:
            LFGListUpdateBlacklist() : ServerPacket(SMSG_LFG_LIST_UPDATE_BLACKLIST) { }

            WorldPacket const* Write() override;

            std::vector<LFGListBlacklist> Blacklist;
        };

        class LFGListUpdateStatus final : public ServerPacket
        {
        public:
            LFGListUpdateStatus() : ServerPacket(SMSG_LFG_LIST_UPDATE_STATUS, 28 + 1 + 1 + 4 + 4 + 2 + 2 + 2) { }

            WorldPacket const* Write() override;

            LFG::RideTicket ApplicationTicket;
            uint32 RemainingTime = 0;
            uint8 ResultId = 0;
            ListRequest Request;
            bool Listed = false;
        };

        struct LFGListSearchResult
        {
            std::vector<MemberInfo> Members;
            LFG::RideTicket ApplicationTicket;
            ListRequest JoinRequest;
            Optional<ObjectGuid> LeaderGuid;
            Optional<ObjectGuid> UnkGuid;
            Optional<ObjectGuid> UnkGuid2;
            Optional<ObjectGuid> UnkGuid3;
            Optional<uint32> VirtualRealmAddress;
            Optional<uint32> UnkInt2;
            uint32 UnkInt = 0;
            bool UnkBIt = false;
            bool UnkBIt2 = false;
            bool UnkBIt3 = false;
            bool UnkBIt4 = false;
            bool UnkBit96 = false;
        };

        class LFGListSearchResultUpdate final : public ServerPacket
        {
        public:
            LFGListSearchResultUpdate() : ServerPacket(SMSG_LFG_LIST_SEARCH_RESULTS_UPDATE) { }

            WorldPacket const* Write() override;

            Array<LFGListSearchResult, 50> ResultUpdate;
        };

        class LfgListUpdateExpiration final : public ServerPacket
        {
        public:
            LfgListUpdateExpiration() : ServerPacket(SMSG_LFG_LIST_UPDATE_EXPIRATION) { }

            WorldPacket const* Write() override;

            LFG::RideTicket ApplicationTicket;
            uint32 TimeoutTime = 0;
            uint8 Status = 0;
        };
    }
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::LFGList::LFGListBlacklist const& blackList);
ByteBuffer& operator>>(ByteBuffer& data, WorldPackets::LFGList::LFGListBlacklist& blackList);
ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::LFGList::ListSearchResult const& listSearch);
ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::LFGList::MemberInfo const& memberInfo);
ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::LFGList::ListRequest const& join);
ByteBuffer& operator>>(ByteBuffer& data, WorldPackets::LFGList::ListRequest& join);

#endif // LfgListPackets_h__
